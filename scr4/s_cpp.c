#include "s_cpp.h"
#include <s_strs.h>

int     CPP_DIRCH  = '#', CPP_DEFCH = '&', CPP_EOLCH = '#';

/* ================================================================
Ouvre un fichier en mode interprétation. L'interpréteur gère les
fichiers inclus (include) et les macros (define).

&EN filename = nom du fichier
&EN type :
&EN2 0 pour un fichier normal,
&EN2 1 pour le stdin,
&EN3 2 pour un string. Si filename est défini, le string à interpréter
    est initialisé avec le contenu de filename. Sinon, la fonction
    CppRecord() ou CppRecordLILO() sera utilisée avant lecture.

Trois variables contenant les caractères spéciaux reconnus par les
fonctions du groupe Cpp peuvent être (re-)définies à tout moment.

&EN int CPP_DIRCH qui indique le début d'une directive ('#' par défaut)
&EN int CPP_DEFCH qui indique le début d'une macro ('&' par défaut)
&EN int CPP_EOLCH qui permet de prolonger une macro à la ligne suivante
    ('#' par défaut)

&IT Directives
──────────
Les directives suivantes sont interprétées :

&EN #include "filename"
&EN #define MacroName MacroDefinition
&EN #undef MacroName
&EN #ifdef MacroName
&EN #else
&EN #endif

Pour qu'une telle ligne ne soit pas interprétée, il suffit de placer
un point-virgule (;) après le caractère #.

&IT Macros
──────
Le nom des macros peuvent être quelconque mais ne doit pas contenir
de caractères autres qu'alphanumériques ou de soulignement (_).

L'utilisation des macros se fait à l'aide du caractère & suivi du
nom de la macro :

&CO
    #define UINT  unsigned short
    ...
    &UINT i;
&TX

Pour annuler l'interprétation d'une macro, il suffit de placer un
point-virgule (;) après le caractère &.

Le séparateur qui suit le nom de la macro peut être quelconque. S'il
s'agit d'un point-virgule (;), ce caractère est consommé.

&CO
    #define TX unsigned short
    &;;TX x,y
    &TX x,y
    &TX;x,y
&TX
Donne à l'interprétation
&CO
    &;TX x,y
    unsigned short x,y
    unsigned shortx,y
&TX

Pour prolonger le texte d'une macro sur plusieurs lignes, il suffit de
placer en fin de ligne le caractère #. La ligne suivante ne sera pas
interprétée (sauf en ce qui concerne les "sous-macros" utilisées) et fera
partie de la macro.

Pour placer le caractère # en fin de macro, il suffit de le doubler en
fin de ligne.

&CO
    #define PAR1 .par1 par_1#

    #define PAR2 .par1 par_2##
    &PAR1 texte1
    &PAR2 texte2
&TX
Donne à l'interprétation
&CO
    .par1 par_1
     texte1
    .par1 par2# texte2
&TX

Si une macro n'est pas définie, le texte du fichier n'est pas
interprété. Ainsi:

&CO
    #define A AVAL
    &A
    &B
&TX
Donne à l'interprétation
&CO
    AVAL
    &B
&TX

&RT La fonction retourne un pointeur vers une structure CPPFILE.

&SA CppGetc(), CppUngetc(), CppClose()
==================================================================== */

#ifdef __cplusplus
CPPFILE *CppOpen(
char    *filename,
int     type
)
#else
CPPFILE *CppOpen(filename, type)
char    *filename;
int     type;
#endif

{
    CPPFILE *cpp = 0;
    FILE    *fd = 0;

    switch(type) {
	case 0 : fd = fopen(filename, "r");
		 if(fd == 0) return(cpp);
		 break;
	case 1 : fd = stdin;
		 if(fd == 0) return(cpp);
		 break;
	case 2 : break;
	}
    if(type != 2 && fd == 0) return(cpp);

    cpp = (CPPFILE *) SCR_malloc(sizeof(CPPFILE));
    if(cpp == 0) return(cpp);
    cpp->fd = fd;
    cpp->prev = '\n';
    cpp->type = type;
    if(filename && type != 2) {
	cpp->filename = SCR_stracpy(filename);
	SCR_strip(cpp->filename);    /* JMP 11-05-97 */
	}
    if(filename && type == 2) CppRecord(cpp, filename);

    return(cpp);
}

/* ================================================================
Ferme le fichier préalablement ouvert par CppOpen() et libère les
buffers associés.

&EN cpp = pointeur retourné par CppOpen()

&SA CppOpen()
================================================================ */
int CppClose(cpp)
CPPFILE *cpp;
{
    CppCloseNoDef(cpp);
    CppFreeDefines();
    return(0);
}

/*NH*/
int CppCloseNoDef(cpp)
CPPFILE *cpp;
{
    if(cpp == 0) return(0);
    CppCloseNoDef(cpp->incl);
    if(cpp->fd) fclose(cpp->fd);
    SCR_free(cpp->filename);
    SCR_free(cpp->rec);
    SCR_free(cpp); /* JMP 22-08-2012 */
    return(0);
}

/* ================================================================
Place le string buf dans le stream du fichier cpp ouvert.
La lecture suivante retournera buf[0], ensuite buf[1], etc.

&EN cpp = descripteur du fichier ouvert
&EN buf = string terminé par un 0

Les appels successifs à CppRecord() placent les strings selon la
méthode LIFO (last in, first out). L'exemple suivant explicite la
méthode.

&EX
    CPPFILE *cpp;
    int     i;

    cpp = CppOpen(filename, 0);
    if(cpp == 0) return(-1);
    CppRecord(cpp, "1234");
    CppRecord(cpp, "A");
    CppRecord(cpp, "B");
    CppRecord(cpp, "C");

    for(i = 0 ; i < 7 ; i++)
	printf("%d : \n", i + 1, CppGetc(cpp));

    ...
&TX
Résultat :
&CO
    1 : C  (LIFO)
    2 : B  (LIFO)
    3 : A  (LIFO)
    4 : 1  (Premier caractère de la chaîne)
    5 : 2  (deuxième)
    6 : 3  (troisième)
    7 : 4  (quatrième)
&TX

&SA CppGetc(), CppUngetc(), CppOpen(), CppClose(), CppRecordLILO()
================================================================ */

int CppRecord(cpp, buf)
CPPFILE *cpp;
U_ch    *buf;
{
    int     i, j, lg;

    cpp = CppCurrent(cpp);
    lg = (int)strlen(buf);
    if(CppRecordRealloc(cpp, lg)) return(0);

    for(j = 0, i = lg - 1 ; i >= 0 ; j++, i--)
	cpp->rec[cpp->rec_nb + j] = buf[i];

    cpp->rec_nb += lg;
    return(0);
}

/* ================================================================
Place le string buf dans le stream du fichier cpp ouvert selon
la méthode LILO (Last In, First Out).

&EN cpp = descripteur du fichier ouvert
&EN buf = string terminé par un 0

L'exemple suivant explicite la méthode.

&EX
    CPPFILE *cpp;
    int     i;

    cpp = CppOpen(filename, 0);
    if(cpp == 0) return(-1);
    CppRecordLILO(cpp, "1234");
    CppRecordLILO(cpp, "A");
    CppRecordLILO(cpp, "B");
    CppRecordLILO(cpp, "C");

    for(i = 0 ; i < 7 ; i++)
	printf("%d : \n", i + 1, CppGetc(cpp));

    ...
&TX
Résultat :
&CO
    4 : 1  (Premier caractère de la chaîne)
    5 : 2  (deuxième)
    6 : 3  (troisième)
    7 : 4  (quatrième)
    3 : A  (LILO)
    2 : B  (LILO)
    1 : C  (LILO)
&TX

&SA CppGetc(), CppUngetc(), CppOpen(), CppClose(), CppRecord()
================================================================ */

int CppRecordLILO(cpp, buf)
CPPFILE *cpp;
U_ch    *buf;
{
    int     i, j, lg;

    cpp = CppCurrent(cpp);
    lg = (int)strlen(buf);
    if(CppRecordRealloc(cpp, lg)) return(-1);
    for(i = cpp->rec_nb - 1; i >= 0 ; i--)
	cpp->rec[lg + i] = cpp->rec[i];

    for(j = 0, i = lg - 1 ; i >= 0 ; j++, i--)
	cpp->rec[j] = buf[i];

    cpp->rec_nb += lg;
    return(0);
}

/*NH*/
CppRecordRealloc(cpp, lg)
CPPFILE *cpp;
int     lg;
{
    if(cpp->rec_max > cpp->rec_nb + lg) return(0);
    cpp->rec = (U_ch *)SCR_realloc(cpp->rec, 1, cpp->rec_max,
						    cpp->rec_max + lg + 512); /* JMP 27-12-97 */
    if(cpp->rec == 0) {
	CppPrintError(cpp, "Memory full");
	return(-1);
	}
    cpp->rec_max += lg + 512; /* JMP 27-12-97 */
    return(0);
}

/* ================================================================
Place le caractère ch dans le stream du fichier cpp ouvert.
La lecture suivante retournera ce caractère.

Contrairement à la fonction ungetc(), plusieurs CppUngetc() peuvent
être appelés successivement. Les caractères seront retournés selon
la méthode LIFO comme pour CppRecord().

&EN cpp = descripteur du fichier ouvert
&EN ch = caractère à enregistrer

&EX
    CPPFILE *cpp;
    int     i;

    cpp = CppOpen(filename, 0);
    if(cpp == 0) return(-1);
    CppUngetc(cpp, '1');
    CppUngetc(cpp, '2');
    CppUngetc(cpp, '3');

    for(i = 0 ; i < 3 ; i++)
	printf("%d : \n", i + 1, CppGetc(cpp));

    ...
&TX
Résultat :
&CO
    1 : 3  (LIFO)
    2 : 2  (LIFO)
    3 : 1  (LIFO)
&TX

&SA CppGetc(), CppRecord(), CppOpen(), CppClose()
================================================================ */

int CppUngetc(cpp, ch)
CPPFILE *cpp;
int     ch;
{
    U_ch    buf[2];

    buf[0] = ch;
    buf[1] = 0;
    CppRecord(cpp, buf);
    return(0);
}

/*NH*/
#ifdef __cplusplus
/*NH*/
CPPFILE *CppCurrent(
CPPFILE *cpp
)
#else
CPPFILE *CppCurrent(cpp)
CPPFILE *cpp;
#endif

{
    if(cpp->incl == 0) return(cpp);
    return(CppCurrent(cpp->incl));
}

/*NH*/
int CppCloseLast(cpp)
CPPFILE *cpp;
{
    CPPFILE     *cpp1 = cpp;

    while(cpp->incl) {
	cpp1 = cpp;
	cpp = cpp->incl;
	}
    cpp1->incl = 0;
    if(cpp1 != cpp) CppCloseNoDef(cpp);
    return(0);
}

/* ================================================================
Lit le caractère suivant dans le fichier décrit par cpp. Ce caractère
peut provenir d'un CppRecord() ou CppUngetc() précédent.

Si le fichier contient des directives, ces directives sont interprétées
par la fonction CppGetc() et le caractères retourné dépend des actions
déterminées par les directives.

Trois cas sont à prendre en considération :

&EN La directive include (commençant par le caractère défini dans
    CPP_DIRCH) en début de ligne. Le fichier indiqué est ouvert et
    le premier caractère du sous-fichier est retourné (sauf si
    on se trouve dans un des trois cas décrits ici). A la fin du
    sous-fichier, la lecture se poursuit à la ligne suivante du fichier
    principal.

&EN La directive define (commençant par le caractère défini dans
    CPP_DIRCH) en début de ligne. La définition est lue et s'ajoute à la
    table des defines. Le caractère suivant dans le fichier est retourné
    par la fonction.

&EN Un define (commençant par le caractère défini dans CPP_DEFCH). Le
    caractère retourné est le premier de la valeur définie du define.


&IT Paramètres

&EN cpp = pointeur retourné par CppOpen()

&RT Le caractère suivant dans le fihier cpp.

&EX
    CPPFILE *cpp;
    U_ch    *abuf;
    int     ch = 0;

    cpp = CppOpen(filename, 0);
    if(cpp == 0) return(-1);
    CppSkipSpaces(cpp);
    while(1) {
	ch = CppGetc(cpp);
	if(ch == EOF) break;
	printf("%c", ch);
	}
    CppClose(cpp);
&TX

&SA CppOpen(), CppUngetc(), CppClose(), CppSkipSpaces()
================================================================ */

CppGetc(cpp)
CPPFILE *cpp;
{
    int     ch, ch1, prev, skip;
    char    str[256];
    CPPFILE *cpp1 = cpp;

ag:
    ch = CppGetNext(cpp);
    prev = cpp->prev;
    cpp->prev = ch;
    if(!cpp->indef && ch == CPP_DIRCH && prev == '\n') {
	skip = cpp->skip;
	cpp->skip = 0;
	CppReadWord(cpp, str, 255);
	cpp->skip = skip;
	ch1 = CppGetNext(cpp);
	if(ch1 != ';') CppUngetc(cpp, ch1);

	if(strcmp(str, "endif") == 0) CppEndif(cpp);
	else if(strcmp(str, "ifdef") == 0) CppIfdef(cpp);
	else if(strcmp(str, "else") == 0) CppElse(cpp);
	else if(cpp->skip) goto ag;

	else if(strcmp(str, "define") == 0) CppReadDefine(cpp);
	else if(strcmp(str, "undef") == 0) CppReadUndef(cpp);
	else if(strcmp(str, "include") == 0) CppInclude(cpp);
	else {
	    CppRecord(cpp, str);
	    cpp->prev = 0;
	    return(CPP_DIRCH);
	    }
	goto ag;
	}

    if(ch == EOF) return(EOF);
    if(cpp->skip) goto ag;

    if(ch == CPP_DEFCH) {
	CppReadWord(cpp, str, 255);
	ch1 = CppGetNext(cpp);
	if(ch1 != ';') CppUngetc(cpp, ch1);
	if(CppApplyDefine(cpp, str) < 0) return(CPP_DEFCH);
	goto ag;
	}

    return(ch);
}

/*NH*/
int CppIfdef(cpp)
CPPFILE *cpp;
{
    U_ch    str[256];
    int     i;

    cpp->skip = 0;
    CppSkipSpaces(cpp);
    CppReadWord(cpp, str, 255);
    CppSkipToEol(cpp);
    i = CppFindDefine(str);
    if(i >= 0) cpp->skip = 0;
    else       cpp->skip = 1;
    cpp->iflevel++;
    return(0);
}

/*NH*/
int CppElse(cpp)
CPPFILE *cpp;
{
    int     skip = cpp->skip;

    cpp->skip = 0;
    CppSkipToEol(cpp);
    if(skip) cpp->skip = 0;
    else     cpp->skip = 1;
    return(0);
}

/*NH*/
int CppEndif(cpp)
CPPFILE *cpp;
{
    cpp->skip = 0;
    CppSkipToEol(cpp);
    cpp->iflevel--;
    return(0);
}

/*NH*/
CppGetNext(cpp)
CPPFILE *cpp;
{
    int     ch;
    CPPFILE *cpp1 = cpp;

ag:
    cpp = CppCurrent(cpp1);
    if(cpp->rec_nb) {
	ch = cpp->rec[cpp->rec_nb - 1];
	cpp->rec_nb--;
	}
    else {
	if(cpp->type != 2) ch = getc(cpp->fd);
	else               ch = EOF;
	if(ch == '\n') {cpp->cline++; cpp->ccol = 0;}
	else if(ch == '\t') {
	    for(; cpp->ccol % 8 ; cpp->ccol++) CppRecord(cpp, " ");
	    ch = ' ';
	    }
	cpp->ccol++;
	}

    if(ch == EOF) {
	cpp->skip = 0;
	cpp->iflevel = 0;
	if(cpp == cpp1) return(EOF);
	CppCloseLast(cpp1);
	goto ag;
	}

    return(ch);
}

U_ch    **CPP_DEFS = 0;
int     CPP_NB_DEFS = 0;

/*NH*/
CppAddDefine(str)
U_ch    *str;
{
    CPP_DEFS = (U_ch **)SCR_realloc(CPP_DEFS, 2 * sizeof(U_ch *),
				CPP_NB_DEFS, CPP_NB_DEFS + 1);

    CPP_DEFS[CPP_NB_DEFS * 2] = SCR_stracpy(str);
    CPP_NB_DEFS++;
    return(CPP_NB_DEFS - 1);
}

/*NH*/
CppFindDefine(str)
U_ch    *str;
{
    int     i;

    for(i = 0 ; i < CPP_NB_DEFS ; i++)
	if(strcmp(str, CPP_DEFS[2 * i]) == 0) return(i);
    return(-1);
}

/*NH ================================================================
Libère l'espace alloué par les define.

Cette fonction est pas appelée par CppClose();

&EX
    CPPFILE *cpp;

    CppDefine("TX", ".bl_off\n.bl_off\.par par_1\n");
    cpp = CppOpen(filename, 0);
    if(cpp == 0) return(-1);
    ...
    CppClose(cpp);
&TX

&SA CppOpen(), CppClose(), CppDefine()
================================================================ */

int CppFreeDefines()
{
    int     i;

    for(i = 0 ; i < CPP_NB_DEFS ; i++) {
	SCR_free(CPP_DEFS[2 * i]);
	SCR_free(CPP_DEFS[2 * i + 1]);
	}
    SCR_free(CPP_DEFS);
    CPP_NB_DEFS = 0;
    CPP_DEFS = 0;
    return(0);
}

/* ================================================================
Definit une macro. L'espace nécessaire pour stocker name et txt est
alloué par la fonction. Cet espace sera libéré par l'appel à
CppFreeDefines(). La fonction CppClose() ne libère pas cet espace.

&EN name = nom de la macro
&EN txt = texte de la macro

&EX
    CPPFILE *cpp;

    CppDefine("TX", ".bl_off\n.bl_off\.par par_1\n");
    cpp = CppOpen(filename, 0);
    if(cpp == 0) return(-1);
    ...
    CppClose(cpp);
&TX

&SA CppOpen(), CppClose(), CppFreeDefine()
================================================================ */

int CppDefine(name, txt)
U_ch    *name, *txt;
{
    int     i;

    i = CppFindDefine(name);
    if(i < 0) i = CppAddDefine(name);
    if(CPP_DEFS[2 * i + 1]) SCR_free(CPP_DEFS[2 * i + 1]);
    CPP_DEFS[2 * i + 1] = SCR_stracpy(txt);
    return(0);
}

/*NH*/
int CppReadDefine(cpp)
CPPFILE *cpp;
{
    U_ch    str[256], *def = 0;
    int     i, ch, j = 0;

    cpp->indef = 1;
    CppSkipSpaces(cpp);
    CppReadWord(cpp, str, 255);
    i = CppFindDefine(str);
    if(i < 0) i = CppAddDefine(str);
    CppSkipSpaces(cpp);
    while(1) {
	ch = CppGetc(cpp);
	if(ch == EOF) break;
	if(ch == '\n') {
	    if(def == 0 || def[j - 1] != CPP_EOLCH) break;
	    j--;
	    if(j > 0 && def[j - 1] == CPP_EOLCH) break;
	    }
	if(j % 20 == 0) def = SCR_realloc(def, 1, j, j + 21);
	def[j++] = ch;
	}

    if(def) def[j] = 0;
    if(CPP_DEFS[2 * i + 1]) SCR_free(CPP_DEFS[2 * i + 1]);
    CPP_DEFS[2 * i + 1] = def;
    cpp->indef = 0;
    return(0);
}

/*NH*/
int CppReadUndef(cpp)
CPPFILE *cpp;
{
    U_ch    str[256];
    int     i;

    CppSkipSpaces(cpp);
    CppReadWord(cpp, str, 255);
    CppSkipToEol(cpp);
    i = CppFindDefine(str);
    if(i < 0) return(0);
    CPP_DEFS[2 * i][0] = 0xFF;
    return(0);
}

/*NH*/
CppApplyDefine(cpp, str)
CPPFILE     *cpp;
U_ch        *str;
{
    int     i;

    i = CppFindDefine(str);
    if(i < 0) CppRecord(cpp, str);
    else if(CPP_DEFS[i * 2 + 1]) CppRecord(cpp, CPP_DEFS[i * 2 + 1]);
    return(i);
}

/*NH*/
int CppInclude(cpp)
CPPFILE *cpp;
{
    U_ch    filename[256];
    int     i = 0, lg;

    CppSkipSpaces(cpp);
    CppReadString(cpp, filename, 255);
    if(filename[0] == '"') i = 1;
    lg = (int)strlen(filename);
    if(lg && filename[lg - 1] == '"') filename[lg - 1] = 0;
    CppSkipToEol(cpp);
    cpp->incl = CppOpen(filename + i, 0);
    if(cpp->incl == 0)
	CppPrintError(cpp, "Cannot open file");
    return(0);
}


/* ================================================================
Retourne dans un buffer statique un texte indiquant le fichier, la ligne
et le caractère courants. Le format est :

&CO
    nom_du_fichier[line;col]
&TX

&EN cpp = pointeur retourné par CppOpen()

&SA CppOpen(), CppClose(), CppError()
================================================================ */

char *CppError(cpp)
CPPFILE *cpp;
{
    //static char buf[128];
    static char buf[512]; /* JMP 10-01-11 */

    sprintf(buf, "%s[%d;%d]", cpp->filename, cpp->cline, cpp->ccol);
    return(buf);
}



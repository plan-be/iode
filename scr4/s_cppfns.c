#include "s_cpp.h"

/* ================================================================
Saute les caractŠres suivants dans le fichier d‚crit par cpp jusqu'au
premier diff‚rent de SPACE et de TAB. Le caractŠre suivant est retourn‚,
mais sera relu lors du prochain appel … CppGetc().

&EN cpp = pointeur retourn‚ par CppOpen()

&RT le caractŠre non SPACE et non TAB suivant

&SA CppOpen(), CppGetc(), CppUngetc(), CppSkipToChars()
================================================================ */

CppSkipSpaces(cpp)
CPPFILE *cpp;
{
    int     ch;

    while(1) {
	ch = CppGetc(cpp);
	if(ch == ' ' || ch == '\t') continue;
	CppUngetc(cpp, ch);
	return(ch);
	}
}

/* ================================================================
Saute les caractŠres suivants dans le fichier d‚crit par cpp jusqu'au
premier appartenant … la chaŒne chs (ou … la fin du fichier).
Ce caractŠre est retourn‚ par la fonction mais ne le sera plus par
l'appel suivant … CppGetc().

&EN cpp = pointeur retourn‚ par CppOpen()
&EN chs = chaŒne de caractŠres termin‚ par un z‚ro

&RT le premier caractŠre appartenant … chs ou EOF

&SA CppOpen(), CppGetc(), CppUngetc(), CppSkipSpaces()
================================================================ */

CppSkipToChars(cpp, chs)
CPPFILE *cpp;
U_ch    *chs;
{
    int     ch;

    while(1) {
	ch = CppGetc(cpp);
	if(ch == EOF || U_is_in(ch, chs)) return(ch);
	}
}

/* ================================================================
Lit les caractŠres suivants dans le fichier d‚crit par cpp jusqu'au
premier appartenant … la chaŒne chs (ou … la fin du fichier).
Le premier caractŠre appartenant … chs n'est pas consomm‚.

&EN cpp = pointeur retourn‚ par CppOpen()
&EN chs = chaŒne de caractŠres termin‚ par un z‚ro

&RT un pointeur allou‚ vers la chaŒne lue ou 0 si aucun caractŠre n'est
lu (fin de fichier ou ligne vide).

&SA CppOpen(), CppGetc(), CppUngetc(), CppSkipSpaces()
================================================================ */

U_ch *CppReadToChars(cpp, chs)
CPPFILE *cpp;
U_ch    *chs;
{
    int     i = 0, lg = 0, ch;
    U_ch    *line = 0;

    while(1) {
	ch = CppGetc(cpp);
	if(ch == EOF || U_is_in(ch, chs)) {
	    if(line) line[i] = 0;
	    if(ch != EOF) CppUngetc(cpp, ch);
	    return(line);
	    }
	if(i >= lg) {
	    line = (U_ch *) SCR_realloc(line, 1, lg, lg + 65);
	    lg += 64;
	    }
	line[i++] = ch;
	}
}

/* ================================================================
Lit la ligne suivante dans le fichier d‚crit par cpp.

&EN cpp = pointeur retourn‚ par CppOpen()

&RT buffer allou‚ contenant la ligne lue

&SA CppOpen(), CppGetc(), CppUngetc(), CppReadToChars()
================================================================ */

U_ch *CppReadLine(cpp)
CPPFILE *cpp;
{
    U_ch    *abuf;

    abuf = CppReadToChars(cpp, "\n");
    CppGetc(cpp);
    return(abuf);
}

/* ================================================================
Saute les caractŠres suivants dans le fichier d‚crit par cpp jusqu'…
la fin de la ligne courante (ou … la fin du fichier).
Ce caractŠre est retourn‚ par la fonction et "consomm‚" (il ne sera pas
return‚ lors de l'appel suivant … CppGetc()).

&EN cpp = pointeur retourn‚ par CppOpen()

&RT '\n' ou EOF

&SA CppOpen(), CppGetc(), CppUngetc(), CppSkipToChars()
================================================================ */

int CppSkipToEol(cpp)
CPPFILE *cpp;
{
    CppSkipToChars(cpp, "\n");
    return(0);
}

/* ================================================================
Lit les caractŠres suivants dans le fichier d‚crit par cpp. La lecture
s'arrˆte dŠs que le caractŠre lu n'appartient plus … la suite chs.

&EN cpp = pointeur retourn‚ par CppOpen()
&EN chs = table des caractŠres accept‚s
&EN str = pointeur vers un buffer de taille ‚gale au moins … maxlg
    caractŠres
&EN maxlg = taille maximum du mot.


&RT longueur du mot lu

&EX
    CPPFILE *cpp;
    U_ch    buf[81];

    cpp = CppOpen(filename, 0);
    if(cpp == 0) return(-1);
    CppSkipSpaces(cpp);
    CppReadOnlyChars(cpp, "0123456789-", str, 80);
&TX

&REM
Si le string d‚passe maxlg caractŠres, la fonction retourne et la suite du
texte sera lue lors des appels suivants. Un mot peut donc ˆtre de la
sorte coup‚ en deux.

&SA CppOpen(), CppGetc(), CppUngetc(), CppReadString(), CppReadLong()
================================================================ */

CppReadOnlyChars(cpp, chs, str, maxlg)
CPPFILE *cpp;
U_ch    *chs, *str;
int     maxlg;
{
    int     ch, i = 0;

    str[0] = 0;
    while(1) {
	ch = CppGetc(cpp);
	if(!U_is_in(ch, chs)) break;
	str[i++] = ch;
	if(i >= maxlg) break;
	}
    str[i] = 0;
    CppUngetc(cpp, ch);
    return(i);
}

/* ================================================================
Lit le "mot" suivant dans le fichier d‚crit par cpp. Un mot est une
suite de caractŠres alphanum‚riques (y compris le caractŠre de
soulignement '_').

&EN cpp = pointeur retourn‚ par CppOpen()
&EN str = pointeur vers un buffer de taille ‚gale au moins … maxlg
    caractŠres
&EN maxlg = taille maximum du mot.


&RT longueur du mot lu

&EX
    CPPFILE *cpp;
    U_ch    buf[81];

    cpp = CppOpen(filename, 0);
    if(cpp == 0) return(-1);
    CppSkipSpaces(cpp);
    CppReadWord(cpp, buf, 80);
&TX

&REM
Si le mot d‚passe maxlg caractŠres, la fonction retourne et la suite du
mot sera lue lors des appels suivants. Un mot est donc de la sorte coup‚
en deux.

&SA CppOpen(), CppGetc(), CppUngetc(), CppReadString(), CppReadLong()
================================================================ */

CppReadWord(cpp, str, maxlg)
CPPFILE *cpp;
U_ch    *str;
int     maxlg;
{
    int     ch, i = 0;

    str[0] = 0;
    while(1) {
	ch = CppGetc(cpp);
	if(!SCR_is_anum(ch) && ch != '_') break;
	str[i++] = ch;
	if(i >= maxlg) break;
	}
    str[i] = 0;
    CppUngetc(cpp, ch);
    return(i);
}

/* ================================================================
Lit l'entier suivant dans le fichier d‚crit par cpp.  Les espaces
sont pr‚alablement saut‚s par la fonction.

&EN cpp = pointeur retourn‚ par CppOpen()

&RT entier lu

&REM
Un entier ne peut pas d‚passer 50 caractŠres. Si plusieurs signes - se
trouvent dans le mˆme nombre, ils sont tous lus.

&SA CppOpen(), CppGetc(), CppUngetc(), CppReadString(), CppReadWord()
================================================================ */

long CppReadLong(cpp)
CPPFILE *cpp;
{
    char    str[51];
/*    long    atol(); /* JMP 04-01-98 */

    CppSkipSpaces(cpp);
    CppReadOnlyChars(cpp, "0123456789-", str, 50);
    return(atol(str));
}

/* ================================================================
Lit le nombre r‚el suivant dans le fichier d‚crit par cpp. Les espaces
sont pr‚alablement saut‚s par la fonction.

&EN cpp = pointeur retourn‚ par CppOpen()

&RT le nombre lu

&REM
Un nombre ne peut pas d‚passer 50 caractŠres.

&SA CppOpen(), CppGetc(), CppUngetc(), CppReadString(), CppReadWord()
================================================================ */

double CppReadDbl(cpp)
CPPFILE *cpp;
{
    U_ch    str[51];
    double  val;
/*    extern  double atof(); /* JMP 04-01-98 */

    CppSkipSpaces(cpp);
    CppReadOnlyChars(cpp, "0123456789-.eE", str, 50);
    val = atof(str);
    return(val);
}


/* ================================================================
Lit le string suivant dans le fichier d‚crit par cpp. Un string est une
suite de caractŠres quelconques … l'exclusion de SPACE, TAB et NEWLINE.
DŠs qu'un de ces trois caractŠres est lu, le string est termin‚.

&EN cpp = pointeur retourn‚ par CppOpen()
&EN str = pointeur vers un buffer de taille ‚gale au moins … maxlg
    caractŠres
&EN maxlg = taille maximum string

&RT longueur du string lu

&EX
    CPPFILE *cpp;
    U_ch    buf[81];

    cpp = CppOpen(filename, 0);
    if(cpp == 0) return(-1);
    CppSkipSpaces(cpp);
    CppReadString(cpp, buf, 80);
&TX

&REM
Si le string d‚passe maxlg caractŠres, la fonction retourne et la suite
du string sera lue lors des appels suivants. Un string est donc de la
sorte coup‚ en deux.

&SA CppOpen(), CppGetc(), CppReadWord(), CppReadLong()
================================================================ */

CppReadString(cpp, str, maxlg)
CPPFILE *cpp;
U_ch    *str;
int     maxlg;
{
    int     ch, i = 0;

    str[0] = 0;
    while(1) {
	ch = CppGetc(cpp);
	if(ch == ' ' || ch == '\t' || ch == '\n') break;
	str[i++] = ch;
	if(i >= maxlg) break;
	}
    str[i] = 0;
    CppUngetc(cpp, ch);
    return(i);
}

/* ================================================================
Lit le texte suivant dans le fichier d‚crit par cpp. Le texte doit ˆtre
termin‚ par le caractŠre QUOTE ("), NEWLINE (\n) ou EOF (-1).
Si le string contient un QUOTE ou un NEWLINE, ceux-ci doivent ˆtre
pr‚c‚d‚s par le caractŠre BACKSLASH (\) d‚fini dans la variable globale
CPP_EOLCH.

&EN cpp = pointeur retourn‚ par CppOpen()

&RT Un pointeur allou‚ (malloc) vers un buffer contenant le texte lu

&EX
    CPPFILE *cpp;
    U_ch    *abuf;
    int     ch;

    cpp = CppOpen(filename, 0);
    if(cpp == 0) return(-1);
    CppSkipSpaces(cpp);
    ch = CppGetc(cpp);
    if(ch == '"') abuf = CppReadQuotedString(af->af_cpp);
&TX

&REM
Le caractŠre QUOTE d'ouverture du string doit ˆtre lu avant d'entrer dans
la fonction.

&SA CppOpen(), CppGetc(), CppReadWord(), CppReadString()
================================================================ */

U_ch *CppReadQuotedString(cpp)
CPPFILE *cpp;
{
    int     i = 0, lg = 0, ch;
    U_ch    *line = 0;

    while(1) {
	ch = CppGetc(cpp);
	if(ch == CPP_EOLCH || ch == '"') {
	    if(i && line[i - 1] == CPP_EOLCH) {
		line[i - 1] = ch;
		continue;
		}
	    }

	if(ch == EOF || ch == '"' || ch == '\n') {
	    if(line) line[i] = 0;
	    if(ch == '\n') CppUngetc(cpp, ch);
	    return(line);
	    }
	if(i >= lg) {
	    line = (U_ch *) SCR_realloc(line, 1, lg, lg + 65);
	    lg += 64;
	    }
	line[i++] = ch;
	}
}


#include "s_cpp.h"

/* ================================================================
Saute les caractères suivants dans le fichier décrit par cpp jusqu'au
premier différent de SPACE et de TAB. Le caractère suivant est retourné,
mais sera relu lors du prochain appel à CppGetc().

&EN cpp = pointeur retourné par CppOpen()

&RT le caractère non SPACE et non TAB suivant

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
Saute les caractères suivants dans le fichier décrit par cpp jusqu'au
premier appartenant à la chaîne chs (ou à la fin du fichier).
Ce caractère est retourné par la fonction mais ne le sera plus par
l'appel suivant à CppGetc().

&EN cpp = pointeur retourné par CppOpen()
&EN chs = chaîne de caractères terminé par un zéro

&RT le premier caractère appartenant à chs ou EOF

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
Lit les caractères suivants dans le fichier décrit par cpp jusqu'au
premier appartenant à la chaîne chs (ou à la fin du fichier).
Le premier caractère appartenant à chs n'est pas consommé.

&EN cpp = pointeur retourné par CppOpen()
&EN chs = chaîne de caractères terminé par un zéro

&RT un pointeur alloué vers la chaîne lue ou 0 si aucun caractère n'est
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
Lit la ligne suivante dans le fichier décrit par cpp.

&EN cpp = pointeur retourné par CppOpen()

&RT buffer alloué contenant la ligne lue

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
Saute les caractères suivants dans le fichier décrit par cpp jusqu'à
la fin de la ligne courante (ou à la fin du fichier).
Ce caractère est retourné par la fonction et "consommé" (il ne sera pas
returné lors de l'appel suivant à CppGetc()).

&EN cpp = pointeur retourné par CppOpen()

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
Lit les caractères suivants dans le fichier décrit par cpp. La lecture
s'arrête dès que le caractère lu n'appartient plus à la suite chs.

&EN cpp = pointeur retourné par CppOpen()
&EN chs = table des caractères acceptés
&EN str = pointeur vers un buffer de taille égale au moins à maxlg
    caractères
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
Si le string dépasse maxlg caractères, la fonction retourne et la suite du
texte sera lue lors des appels suivants. Un mot peut donc être de la
sorte coupé en deux.

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
Lit le "mot" suivant dans le fichier décrit par cpp. Un mot est une
suite de caractères alphanumériques (y compris le caractère de
soulignement '_').

&EN cpp = pointeur retourné par CppOpen()
&EN str = pointeur vers un buffer de taille égale au moins à maxlg
    caractères
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
Si le mot dépasse maxlg caractères, la fonction retourne et la suite du
mot sera lue lors des appels suivants. Un mot est donc de la sorte coupé
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
Lit l'entier suivant dans le fichier décrit par cpp.  Les espaces
sont préalablement sautés par la fonction.

&EN cpp = pointeur retourné par CppOpen()

&RT entier lu

&REM
Un entier ne peut pas dépasser 50 caractères. Si plusieurs signes - se
trouvent dans le même nombre, ils sont tous lus.

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
Lit le nombre réel suivant dans le fichier décrit par cpp. Les espaces
sont préalablement sautés par la fonction.

&EN cpp = pointeur retourné par CppOpen()

&RT le nombre lu

&REM
Un nombre ne peut pas dépasser 50 caractères.

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
Lit le string suivant dans le fichier décrit par cpp. Un string est une
suite de caractères quelconques à l'exclusion de SPACE, TAB et NEWLINE.
Dès qu'un de ces trois caractères est lu, le string est terminé.

&EN cpp = pointeur retourné par CppOpen()
&EN str = pointeur vers un buffer de taille égale au moins à maxlg
    caractères
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
Si le string dépasse maxlg caractères, la fonction retourne et la suite
du string sera lue lors des appels suivants. Un string est donc de la
sorte coupé en deux.

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
Lit le texte suivant dans le fichier décrit par cpp. Le texte doit être
terminé par le caractère QUOTE ("), NEWLINE (\n) ou EOF (-1).
Si le string contient un QUOTE ou un NEWLINE, ceux-ci doivent être
précédés par le caractère BACKSLASH (\) défini dans la variable globale
CPP_EOLCH.

&EN cpp = pointeur retourné par CppOpen()

&RT Un pointeur alloué (malloc) vers un buffer contenant le texte lu

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
Le caractère QUOTE d'ouverture du string doit être lu avant d'entrer dans
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


#include "s_a2m.h"
#include <s_ini.h>

/********************* Parameters *************************/

/* Public */
int     A2M_CSV_SEP = ',';
int     A2M_CSV_QUOTE = '"';
int     A2M_CSV_NA = 1;

/* Private */
FILE    *A2M_fdcsv;

/* ================================================================
Interpr�te le contenu d'un fichier a2m et g�n�re un fichier .csv.

&EN a2mfile = nom du fichier a2m � interpr�ter
&EN outfile = nom du fichier csv � g�n�rer

&RT La fonction retourne 0 si le processus s'est bien d�roul�, -1 sinon.

La syntaxe des fichiers a2m est d�crite dans un chapitre sp�cifique.

&TI Fichier a2m.ini
���������������
Le fichier a2m.ini (ou un autre) contient des param�tres pour la
g�n�ration du fichier csv et la lecture du fichier a2m. Pour que ces
param�tres soient pris en compte par le programme A2mToCsv(), il faut
appeler la fonction A2mCsvReadIni(filename) avant de lancer la fonction
d'interpr�tation et de g�n�ration.

&CO
    #include <s_a2m.h>

    A2mCsvReadIni("myfile.ini");
    A2mToCsv("myfile", "test.csv");
&TX

&TI Variables globales
������������������
Les variables globales d�crites dans le fichier .ini peuvent �galement
�tre modifi�es dans le programme.

&IT Variables influen�ant la lecture du fichier
������������������������������������������
Ces variables sont d�finies dans la secion [A2M] du fichier ini.

&EN int A2M_ESCCH = caract�re d'escape (enrichissements et caract�res
    sp�ciaux) : '\'par d�faut
&EN int A2M_CMDCH = caract�re de commande ('.' par d�faut)
    sp�ciaux)
&EN int A2M_DEFCH = caract�re pr�fixant les macros ('&' par d�faut)
&EN int A2M_SEPCH = caract�re de s�paration des cellules ('&' par d�faut)
&EN int A2M_LFON = conserve les linefeed (1) ou non (0)
&EN int A2M_BLON = conserve les blancs (1) ou non (0)

&IT Variable influen�ant le fichier g�n�r�
��������������������������������������
Ces variables sont d�finies dans la section [CSV] du fichier .ini.

&EN int A2M_CSV_SEP = s�parateur de cellules (d�faut ';')
&EN int A2M_CSV_NA  = remplace le texte "--" par "#N/A"
&EN int A2M_CSV_QUOTE = caract�re encadrant le texte des cellules
			(d�faut '"');

&RT
&EN 0 en cas de succ�s
&EN -1 si le fichier ne peut �tre ouvert
&EN -2 si le fichier output n'a pu �tre cr��


&SA A2mCsvReadIni(), A2mToRtf(), A2mToMif(), A2mToHtml(), A2mPrintError()
==================================================================== */

A2mToCsv(a2mfile, outfile)
char    *a2mfile, *outfile;
{
    A2MFILE *af;
    A2MOBJ  *ao;

    af = A2mOpen(a2mfile, 0);
    if(af == 0) return(-1);

    if(A2mCsvInit(outfile)) {
	A2mClose(af);
	return(-2);
	}

    while(1) {
	ao = A2mRead(af);
	if(ao == 0) break;
	A2mCsvPrintObj(ao);
	}

    A2mCsvEnd();
    A2mClose(af);
    return(0);
}

/*NH*/
A2mCsvInit(outfile)
char    *outfile;
{
    extern int  A2M_CUROBJ;

    A2M_fdcsv = fopen(outfile, "w+");
    if(A2M_fdcsv == 0) return(-2);
    A2M_CUROBJ = 1;
    return(0);
}

/*NH*/
A2mCsvEnd()
{
    fclose(A2M_fdcsv);
    A2M_fdcsv = 0;
    return(0);
}

/*NH*/
A2mCsvPrintObj(ao)
A2MOBJ  *ao;
{
    char        msg[80];
    extern int  A2M_CUROBJ;

    sprintf(msg, "Printing Object %d", A2M_CUROBJ++);
    A2mMessage(msg);
    if(ao->ao_type == A2M_PAR)
	    A2mCsvPrintPar((A2MPAR *)ao->ao_ptr, 0, 0);

    if(ao->ao_type == A2M_TBL)
	    A2mCsvPrintTbl((A2MTBL *)ao->ao_ptr);

    if(ao->ao_type == A2M_GRF)
	    A2mCsvPrintGrf((A2MGRF *)ao->ao_ptr);

    A2mFreeObj(ao);
    return(0);
}

/*NH*/
A2mCsvPrintPar(ap, tbl, tcol)
A2MPAR  *ap;
int     tbl, tcol;
{
    int     i;

    if(ap == 0 || ap->ap_strs == 0) return(0);
    A2mSetParProps(ap->ap_tag, &(ap->ap_ppr));

    fprintf(A2M_fdcsv, "\"");
    if(ap->ap_ppr.pp_bullet) {
	for(i = 0 ; i < ap->ap_lmarg / 8 ; i++) fprintf(A2M_fdcsv, " ");
	fprintf(A2M_fdcsv, " - ");
	}

    for(i = 0 ; ap->ap_strs[i] ; i++)
	A2mCsvPrintStr(ap, i);

    fprintf(A2M_fdcsv, "\"");
    if(tbl == 0) fprintf(A2M_fdcsv, "\n");
    else         fprintf(A2M_fdcsv, "%c", A2M_CSV_SEP);

    return(0);
}

/*NH*/
A2mCsvPrintStr(ap, n)
A2MPAR  *ap;
int     n;
{
    A2MSTR  *as = ap->ap_strs[n];
    int     i;
    char    buf[65];

    if(A2M_NUMBERS && n == 0 && ap->ap_ppr.pp_number >= 1) {
	A2mCalcNumbering(ap->ap_ppr.pp_number - 1, buf);
	for(i = 0 ; buf[i] ; i++) A2mCsvPrintChar(buf[i]);
	}

    switch(as->as_type) {
	case A2M_TEXT :
	    if(A2M_CSV_NA && strcmp(as->as_txt, "-.-") == 0)
		fprintf(A2M_fdcsv, "#N/A");
	    else {
		for(i = 0 ; as->as_txt[i] ; i++)
		    A2mCsvPrintChar(as->as_txt[i]);
		}
	    break;

	case A2M_NEWLINE : fprintf(A2M_fdcsv, "\n"); break;
	case A2M_TAB : fprintf(A2M_fdcsv, " "); break;
	}
    return(0);
}

/*NH*/
A2mCsvPrintChar(ch)
int     ch;
{
    if(ch == A2M_CSV_QUOTE)
	fprintf(A2M_fdcsv, "%c%c",A2M_CSV_QUOTE, A2M_CSV_QUOTE);
    else if(ch == '\t')
	fprintf(A2M_fdcsv, " ");
    else
	fprintf(A2M_fdcsv, "%c", SCR_OemToAnsiChar(ch));

    return(0);
}

/*NH*/
A2mCsvPrintTbl(at)
A2MTBL  *at;
{
    A2MTC   *tc;
    int     i, j;

    A2mCsvPrintPar(at->at_title, 0, 0);

    for(i = 0 ; i < at->at_nl ; i++) {
	if(at->at_tls[i].atl_type) {
	    fprintf(A2M_fdcsv, "\n");
	    continue;
	    }
	for(j = 0 ; j < at->at_nc ; j++) {
	    tc = at->at_tls[i].atl_tcs + j;
	    A2mCsvPrintPar(tc->atc_par, 1, j);
	    }

	fprintf(A2M_fdcsv, "\n");
	}

    A2mCsvPrintPar(at->at_footnote, 0, 0);
    return(0);
}

/*NH*/
A2mCsvPrintGrf(ag)
A2MGRF  *ag;
{
    fprintf(A2M_fdcsv, "\"*** GRAPH ***\"\n");
    return(0);
}

/*NH*/
A2mCsvAFrame(filename)
char    *filename;
{
    fprintf(A2M_fdcsv, "\"*** IMAGE %s ***\"\n", filename);
    return(0);
}

/* ================================================================
Fixe la valeur des variables globales avant l'interp�tation d'un fichier
a2m et la g�n�ration d'un fichier csv en se basant sur les d�finitions
du fichier .ini associ�.

Les sections [A2M] et [CSV] du fichier .ini sont interpr�t�es.

&EN filename = nom du fichier .ini

La syntaxe des fichiers a2m est d�crite dans un chapitre sp�cifique.

&TI Fichier a2m.ini
���������������
Le fichier a2m.ini (ou un autre) contient des param�tres pour
la lecture du fichier a2m etla g�n�ration du fichier csv. Pour que ces
param�tres soient pris en compte par le programme A2mToCsv(), il faut
appeler la fonction A2mCsvReadIni(filename) avant de lancer la fonction
d'interpr�tation et d'impression.

&CO
    #include <s_a2m.h>

    A2mCsvReadIni("myfile.ini");
    A2mToCsv("myfile.a2m", "myfile.csv");
&TX

&SA A2mToGdi(), A2mToRtf(), A2mToMif(), A2mToCsv(), A2mPrintError()
==================================================================== */
A2mCsvReadIni(filename)
char    *filename;
{
    U_ch    buf[256];

    A2mReadA2mIni(filename);
    if(IniReadTxtParm(filename, "CSV", "SEP", buf, 255) == 0)
	A2M_CSV_SEP = buf[0];
    if(IniReadTxtParm(filename, "CSV", "QUOTE", buf, 255) == 0)
	A2M_CSV_QUOTE = buf[0];

    IniReadNumParm(filename, "CSV", "NUMBERING",      &A2M_NUMBERS);
    return(0);
}











#include "scr.h"

ISAM    *PR_IS;
char    *PR_FILENAME;
int     PR_FILE_NLS[4];
PAGE    *PR_PG_REF;
extern  long PR_NB, PR_NR;

#ifdef SCRPROTO
int     (*PR_FN)(ISAM *, long, long, int);
#else
int     (*PR_FN)();
#endif

/*NH*/
PR_file_nl(filename, first_ch)
char    *filename;
int     first_ch;
{
#ifdef SCRCPP
    FILE            *PR_file_open(char *);
#else
    FILE            *PR_file_open();
#endif
    FILE            *fd;
    int             ch, nb_lines = 0;

    fd = PR_file_open(filename);
    if(fd == 0) return(0);
    while(1) {
	ch = PR_file_getc(fd);
	if(ch == EOF) {
	    fclose(fd);
	    return(nb_lines);
	    }
	if(first_ch && SCR_upper_char(ch) == first_ch) nb_lines++;
	PR_file_eol(fd);
	}
}


/* ====================================================================
Initialise le processus d'impression de PR_listing(). Cette fonction
peut cependant �tre utilis�e seule ou en conjonction avec PR_listing_1()
pour des impressions sp�cifiques.

Elle cr�e les variables utiles � PR_listing_1() et PR_listing_eop().

&EX
    if(PR_listing_0(p_is_fact, "fact.pr")) return(-1);
    PR_select(my_pr);
    IS_empty_rec(is_fact);
    if(IS_search(is_fact, 0, SCR_GE, (char *)0)) return;
    numcli = is_fact_NUMCLI;
    while(1) {
	if(numcli != is_fact_NUMCLI) {
	    PR_listing_eop();
	    PR_file_col("fact.pr", PR_PG_REF, 0, -1, 'R');
	    numcli = is_fact_NUMCLI;
	    }
	PR_listing_1(is_fact, 0L, 0L, 1);
	}
    PR_send();
&TX

&RT -1 si aucun isam n'est trouv� dans la page de r�f�rence pg_ref, 0
    sinon.

&SA SC_scan_page(), SC_scan_isam(), PR_file_col(),
    PR_listing(), PR_listing_1(), PR_listing_eop()
======================================================================= */

PR_listing_0(pg_ref, file)
PAGE    *pg_ref;
char    *file;
{
    PR_PG_REF= pg_ref;
    PR_NB    = PR_NR = 0;
    PR_IS    = PG_find_isam(pg_ref);
    if(PR_IS == 0) return(-1);

    PR_FILENAME = file;
    PR_FILE_NLS[0] = PR_file_nl(file, 'H');
    PR_FILE_NLS[1] = PR_file_nl(file, 'T');
    PR_FILE_NLS[2] = PR_file_nl(file, 'F');
    PR_FILE_NLS[3] = PR_file_nl(file, 'E');
    return(0);
}

/* ====================================================================
Force un saut de page bas� sur un fichier de d�finion (.pr). Cette
fonction n'est utilisable qu'apr�s appel � PR_listing_0().

Elle imprime les lignes pr�fix�es par 'F', effectue un saut de page et
imprime les lignes pr�fix�es par 'H'.

&EX
    if(PR_listing_0(p_is_fact, "fact.pr")) return(-1);
    PR_select(my_pr);
    IS_empty_rec(is_fact);
    if(IS_search(is_fact, 0, SCR_GE, (char *)0)) return;
    numcli = is_fact_NUMCLI;
    while(1) {
	if(numcli != is_fact_NUMCLI) {
	    PR_listing_eop();
	    PR_file_col("fact.pr", p_is_fact, 0, -1, 'R');
	    numcli = is_fact_NUMCLI;
	    }
	PR_listing_1(is_fact, 0L, 0L, 1);
	if(IS_next(is_fact) != 0) break; // Fin du traitement
	}
    PR_send();
&TX

&RT Si le fichier ne peut �tre ouvert, PR_file retourne -1. Sinon
    la fonction retourne 0

&SA SC_scan_page(), SC_scan_isam(), PR_file_col(),
    PR_listing(), PR_listing_1(), PR_listing_0()
======================================================================= */

PR_listing_eop()
{
    PR_file_col(PR_FILENAME, PR_PG_REF, 0, -1, 'F');
    PR_formfeed();
    PR_file_col(PR_FILENAME, PR_PG_REF, 0, -1, 'H');
    return(0);
}

/* ====================================================================
Fonction centrale d'impression de PR_listing(). Cette fonction
peut cependant �tre utilis�e en conjonction avec PR_listing_0()
pour des impressions sp�cifiques.

&NO Cette fonction n'est utilisable qu'apr�s appel � PR_listing_0().

Elle imprime les lignes pr�fix�es par 'B' avant la premi�re impression.
Ensuite, elle imprime en d�but de chaque page les lignes pr�fix�es par 'H'.
Les sauts de pages sont g�r�s par cette fonction.

&EX
    if(PR_listing_0(p_is_fact, "fact.pr")) return(-1);
    PR_select(my_pr);
    IS_empty_rec(is_fact);
    if(IS_search(is_fact, 0, SCR_GE, (char *)0)) return;
    numcli = is_fact_NUMCLI;
    while(1) {
	if(numcli != is_fact_NUMCLI) {
	    PR_listing_eop();
	    PR_file_col("fact.pr", PR_PG_REF, 0, -1, 'R');
	    numcli = is_fact_NUMCLI;
	    }
	PR_listing_1(is_fact, 0L, 0L, 1);
	}
    PR_send();
&TX

&RT -1 si aucun isam n'est trouv� dans la page de r�f�rence pg_ref, 0
    sinon.

&SA SC_scan_page(), SC_scan_isam(), PR_file_col(),
    PR_listing(), PR_listing_1(), PR_listing_eop()
======================================================================= */

PR_listing_1(is, nb_read, nb_found, success)
ISAM    *is;
long    nb_read, nb_found;
int     success;
{
    if(PR_FN != 0)      /* BP_MODIF 17-11-95 */
	switch((*PR_FN)(is, nb_read, nb_found, success))
	{
	    case 1  : return(1); /* JMP 23-11-2003 */
	    case -1 : return(-1);
	    default : break;
	}

    if(success == 2 && PR_NB) {
	PR_file_col(PR_FILENAME, PR_PG_REF, 0, -1, 'F');
	if(PR_FILE_NLS[3] > 0) {
	    if(PR_FILE_NLS[3] + PR_CURRENT->pr_cline >= PR_CURRENT->pr_nl)
		PR_formfeed();
	    PR_file_col(PR_FILENAME, PR_PG_REF, 0, -1, 'E');
	    }
	return(2);  /* JMP 23-11-2003 */
	}

    PR_NR++;
    if(success != 1) return(0);
    SCR_isam_to_page(PR_PG_REF, PR_IS);
    ACT_exec_nu(PR_PG_REF->pg_disp_act);

    if(PR_NB == 0) {
	PR_file_col(PR_FILENAME, PR_PG_REF, 0, -1, 'B');
	PR_file_col(PR_FILENAME, PR_PG_REF, 0, -1, 'H');
	}

/***** JMP 18-08-05 !!!!!!!!!!!
    if(PR_FILE_NLS[1] + PR_FILE_NLS[2] + PR_CURRENT->pr_cline >=
       PR_CURRENT->pr_nl)
			    PR_listing_eop();
****/
    if(PR_FILE_NLS[1] + PR_FILE_NLS[2] + PR_CURRENT->pr_cline >
       PR_CURRENT->pr_nl)
			    PR_listing_eop();


    PR_NB++;
    PR_file_col(PR_FILENAME, PR_PG_REF, 0, -1, 'T');
    return(0);
}


/* ====================================================================
Cette fonction permet d'imprimer un ISAM ou une partie d'ISAM en g�rant les
sauts de page, etc, et ce en utilisant essentiellement SC_scan_page() ou
SC_scan_isam() et PR_file_nl().

Les param�tres sont les suivants :

&EN pr : printer destination de l'impression

&EN pg_ref : PAGE de r�f�rence dont l'ISAM scann� est d�duit

&EN pg_scan : PAGE de s�lection �ventuelle. En cas de pointeur nul,
    tout l'ISAM est scann�

&EN cmp : ordre de lecture (num�ro d'indice)

&EN file : fichier ascii � interpr�ter et � imprimer.

Le premier caract�re des lignes du fichier file sert � d�terminer
� quel moment la ligne doit �tre imprim�e :

&EN B (Begin)  en d�but d'impression
&EN E (End)    en fin d'impression
&EN H (Header) en d�but de page
&EN F (Footer) en fin de page
&EN T (Text)   sur chaque record

Pour la syntaxe de d�finition du fichier ASCII, il faut se reporter
� la fonction PR_file_nl().

La display_fn de la page de r�f�rence est ex�cut�e apr�s ISAM_TO_PAGE et
avant l'impression. Cette possibilit� permet d'adapter les champs calcul�s
de la PAGE de r�f�rence et de les utiliser dans le fichier de d�finition
d'impression.

&EX
FICHIER INPUT : PROD.PR
�����������������������

B  $.LDATE                                              - $.PG -
B                      LISTING DES PRODUITS
B
H  =============================================================
H  |      | CODE  | STOCK | DESCRIPTION                        |
H  =============================================================
T  | $.NB | $CODE | $STOCK| $DESCR                             |
F  =============================================================
E
E  Nombre de produits : $.NB

APPEL EN C
����������

    PR_listing(my_pr, p_is_prod, (PAGE *)0, 0, "prod.pr");


RESULTAT
��������

  12-01-1992                                           - 1 -
		      LISTING DES PRODUITS

  =============================================================
  |      | CODE  | STOCK | DESCRIPTION                        |
  =============================================================
  | 1    | CO01  | 122   | Couleur bleue                      |
  | 2    | CO02  |  92   | Couleur rouge                      |
  | 3    | CO03  |  11   | Couleur verte                      |
  | 4    | CO04  |   0   | Couleur jaune                      |
  | 5    | CO05  | 342   | Couleur noire                      |
  | 6    | CO06  | 111   | Couleur blanche                    |
  | 7    | CO07  |  22   | Couleur orange                     |
  | 8    | CO08  |  35   | Couleur rouge                      |
  =============================================================

  Nombre de produits : 8

&TX

&RT Si le fichier ne peut �tre ouvert, PR_file retourne -1. Sinon
    la fonction retourne 0

&SA SC_scan_page(), SC_scan_isam(), PR_file_col()
======================================================================= */


PR_listing(pr, pg_ref, pg_scan, cmp, file)
PR_DEF  *pr;
char    *file;
PAGE    *pg_ref, *pg_scan;
int     cmp;
{
    if(PR_listing_0(pg_ref, file)) return(-1);
    PR_select(pr);

    if(pg_scan)
	SC_scan_page(pg_scan, PR_IS, cmp, PR_listing_1, 1);
    else
	SC_scan_isam(PR_IS, 0L, 0L, cmp, PR_listing_1, SCR_UPDATE, 1);

    PR_send();
    return(0);
}

/*NH*/
#ifdef SCRPROTO
/*NH*/
PR_listing_fn(
PR_DEF  *pr,
PAGE    *pg_ref,
PAGE    *pg_scan,
int     cmp,
char    *file,
int     (*fn)(ISAM *, long, long, int)
)
#else
PR_listing_fn(pr, pg_ref, pg_scan, cmp, file, fn) /* BP_MODIF 17-11-95 */
PR_DEF  *pr;
PAGE    *pg_ref, *pg_scan;
int     cmp;
char    *file;
int     (*fn)();
#endif
{
    PR_FN = fn; /* BP_MODIF 17-11-95 */

    PR_listing(pr, pg_ref, pg_scan, cmp, file);
    return(0);
}





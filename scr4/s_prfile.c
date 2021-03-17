#include "scr.h"

PAGE    *PR_PG;
U_ch    *PR_PTR = 0, *PR_REPL_FROM = 0, *PR_REPL_TO = 0 ;

/*NH*/
unsigned char *PR_file_pg(fld_name)
unsigned char    *fld_name;
{
    int          fld_nb;
    static U_ch  tmp[1];
    char         buf1[2] = ".", buf2[2] = ",";

    tmp[0] = 0;
    fld_nb = PG_find_nb(PR_PG, fld_name);
    if(fld_nb < 0) return((U_ch  *)0);
    if(SCR_is_lower(fld_name[0]) && PG_is_fld_empty(PR_PG, fld_nb)) return(tmp); /* JMP 07-10-01 */
    SCR_free(PR_PTR);
    PR_PTR = PG_aget_rec_txt(PR_PG, fld_nb);
    buf2[0] = SCR_sDECIMAL;
    if(SCR_USE_LOCALS && SCR_sDECIMAL != '.' && PR_PG->pg_flds[fld_nb].fld_type == SCR_REAL) /* JMP 26-10-2004 */
	    SCR_replace(PR_PTR, buf1, buf2);                               /* JMP 26-10-2004 */
    if(PR_REPL_FROM && PR_REPL_TO)
	SCR_replace(PR_PTR, PR_REPL_FROM, PR_REPL_TO);    /* JMP 28-03-02 */
    return(PR_PTR);
}

/*NH*/
PG_find_nb(pg, fld_name)
PAGE    *pg;
char    *fld_name;
{
    int     i;
    int     step = 0;
    char    vector[SCR_MAX_FIELD_LENGTH+1];
    char    *name;

    if(pg == 0) return(-1);
    strcpy(vector, fld_name);
    SCR_upper(vector);
/*  step = SCR_anal_vector(vector); */

    for(i = 0 ; i < pg->pg_nb_flds ; i++) {
	name = PG_fld_name(pg, i);
	if(name != 0 && strcmp(vector, name) == 0)
	    return(i + step);
    }
    return(-1);
}

/* ====================================================================
Imprime le contenu d'un fichier en rempla‡ant les champs indiqu‚s
dans le fichier par des $nom ou ##nom par leur valeur dans une PAGE.

Cette PAGE doit contenir le mot-cl‚ NAME_MEM pour que les noms des
champs soient conserv‚s en m‚moire au run time, et donc utilisables
par la fonction PR_file.

Les champs non trouv‚s dans la PAGE sont remplac‚s par le nom pr‚sent
dans le fichier.

Les noms de champs sont pr‚c‚d‚s d'un $ ou d'un ##. Dans le premier cas
le champ est extrait comme il est d‚fini dans la page. Dans le second cas,
le texte du champ est stripp‚ (les blancs de fin ne sont pas imprim‚s).

Certaines variables ind‚pendantes de la PAGE de r‚f‚rence peuvent ‚galement
ˆtre imprim‚es. Leur syntaxe est $.nom o— nom vaut :

&EN PG : num‚ro de la page courante
&EN NB : num‚ro du record courant (dans le cas de PR_listing())
&EN DATE : date en format dd-mm-yy
&EN LDATE : date en format dd-mm-yyyy
&EN TIME : heure
&EN SUITE : mot (suite) … partir du second record, rien au premier
&EN VERVOLG : mot (vervolg) … partir du second record, rien au premier
&EN CNT : mot (cnt) … partir du second record, rien au premier

Pour imprimer un $, un \ ou un ##, il suffit de les doubler.

Un saut de page forc‚ peut ˆtre obtenu par \f ou par le caractŠre Formfeed
dans le fichier.

Les attributs peuvent ˆtre modifi‚s par les caractŠres suivants
(pr‚c‚d‚s de \, p.ex. \C) :

&EN B : bold
&EN U : soulignement
&EN C : condens‚
&EN E : ‚largi
&EN N : normal

&RT Si le fichier ne peut ˆtre ouvert, PR_file retourne -1. Sinon
    la fonction retourne 0

&EX
    PR_select(pr_deskjet);
    SCR_isam_to_page(pg_client, is_client);
    PR_file("mail.txt", pg_client);
    PR_send();
&TX

&SA SCR_isam_to_page(), PR_select(), PR_send(), PR_file_col()
======================================================================= */

PR_file(filename, pg)
char    *filename;
PAGE    *pg;
{
    return(PR_file_col(filename, pg, 0, -1, 0));
}

/* ====================================================================
Fonction semblable … PR_file() mais … caractŠre plus g‚n‚ral : elle
permet d'ajouter trois paramŠtres lors de l'impression :

&EN col qui indique en quelle colonne le texte d‚fini par le fichier
    doit ˆtre imprim‚ (valable pour les ‚tiquettes)

&EN nl qui indique le nombre de lignes maximum … imprimer (si ce nombre
    vaut -1, il n'y a pas de limite). A nouveau, ceci est surtout utile
    pour des ‚tiquettes

&EN first_ch permet de s‚lectionner les lignes … imprimer dans le fichier
    filename. Seules les lignes qui commencent par ce caractŠre sont
    imprim‚es (majuscules ou minuscules). Si ce caractŠre est nul, toutes
    les lignes sont s‚lectionn‚es.

Dans le cas o— first_ch est sp‚cifi‚, les lignes dont le premier
caractŠre est identique … first_ch et est majuscule sont de toute
fa‡on imprim‚es. Celle dont le premier cacactŠre est identique, mais
en minuscule, ne sont imprim‚es que dans 2 cas : soit si aucun champ
n'est pr‚sent dans cette ligne, soit si au moins un champ de la ligne
est non vide.

Lorsque un des caractŠres verticaux semi-graphiques (simple ou double) ou
le caractŠre de hachurage de mt, ou le caractŠre pipe (|) apparaissent
dans une ligne, le pointeur d'impression est remplac‚ … cet endroit
quelque soit la longueur des champs pr‚c‚dents.

Le caractŠre ampersand a le mˆme effet, mais n'est pas imprim‚ sauf s'il
est doubl‚.

La fonction
&CO
	PR_file(filename, pg)
&TX
vaut simplement
&CO
	PR_file_col(filename, pg, 0, -1, 0)
&TX

&RT Si le fichier ne peut ˆtre ouvert, PR_file retourne -1. Sinon
    la fonction retourne 0

&EX
    PR_select(pr_deskjet);
    ...
	SCR_isam_to_page(pg_client, is_client);
	PR_file("etiq.txt", pg_client, 40, 9, 0);
    ...
    PR_send();
&TX

&SA SCR_isam_to_page(), PR_select(), PR_send(), PR_file(), PR_listing()
======================================================================= */

PR_file_col(filename, pg, col, nl, first_ch)
char    *filename;
PAGE    *pg;
int     col, nl, first_ch;
{
    PR_PG = pg;
    return(PR_mail_col(filename, PR_file_pg, col, nl, first_ch));
}


PR_file_col_repl(filename, pg, col, nl, first_ch, repl, by)
char    *filename;
PAGE    *pg;
int     col, nl, first_ch;
unsigned char *repl, *by;
{
    PR_PG = pg;
    return(PR_mail_col_repl(filename, PR_file_pg, col, nl, first_ch, repl, by));
}

/*NH*/
/*
PR_print_pg_fld(pg, fld_nb, type)
PAGE    *pg;
int     fld_nb, type;
{
    char    *ptr;

    if(pg == 0) return;
    if(!PG_is_fld_empty(pg, fld_nb)) PR_FILE_EMPTY = 0;
    ptr = PG_aget_rec_txt(pg, fld_nb);
    if(type == '#') SCR_strip(ptr);
    PR_string(ptr);
    SCR_free(ptr);
}
*/

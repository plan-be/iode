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
Imprime le contenu d'un fichier en remplaçant les champs indiqués
dans le fichier par des $nom ou ##nom par leur valeur dans une PAGE.

Cette PAGE doit contenir le mot-clé NAME_MEM pour que les noms des
champs soient conservés en mémoire au run time, et donc utilisables
par la fonction PR_file.

Les champs non trouvés dans la PAGE sont remplacés par le nom présent
dans le fichier.

Les noms de champs sont précédés d'un $ ou d'un ##. Dans le premier cas
le champ est extrait comme il est défini dans la page. Dans le second cas,
le texte du champ est strippé (les blancs de fin ne sont pas imprimés).

Certaines variables indépendantes de la PAGE de référence peuvent également
être imprimées. Leur syntaxe est $.nom où nom vaut :

&EN PG : numéro de la page courante
&EN NB : numéro du record courant (dans le cas de PR_listing())
&EN DATE : date en format dd-mm-yy
&EN LDATE : date en format dd-mm-yyyy
&EN TIME : heure
&EN SUITE : mot (suite) à partir du second record, rien au premier
&EN VERVOLG : mot (vervolg) à partir du second record, rien au premier
&EN CNT : mot (cnt) à partir du second record, rien au premier

Pour imprimer un $, un \ ou un ##, il suffit de les doubler.

Un saut de page forcé peut être obtenu par \f ou par le caractère Formfeed
dans le fichier.

Les attributs peuvent être modifiés par les caractères suivants
(précédés de \, p.ex. \C) :

&EN B : bold
&EN U : soulignement
&EN C : condensé
&EN E : élargi
&EN N : normal

&RT Si le fichier ne peut être ouvert, PR_file retourne -1. Sinon
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
Fonction semblable à PR_file() mais à caractère plus général : elle
permet d'ajouter trois paramètres lors de l'impression :

&EN col qui indique en quelle colonne le texte défini par le fichier
    doit être imprimé (valable pour les étiquettes)

&EN nl qui indique le nombre de lignes maximum à imprimer (si ce nombre
    vaut -1, il n'y a pas de limite). A nouveau, ceci est surtout utile
    pour des étiquettes

&EN first_ch permet de sélectionner les lignes à imprimer dans le fichier
    filename. Seules les lignes qui commencent par ce caractère sont
    imprimées (majuscules ou minuscules). Si ce caractère est nul, toutes
    les lignes sont sélectionnées.

Dans le cas où first_ch est spécifié, les lignes dont le premier
caractère est identique à first_ch et est majuscule sont de toute
façon imprimées. Celle dont le premier cacactère est identique, mais
en minuscule, ne sont imprimées que dans 2 cas : soit si aucun champ
n'est présent dans cette ligne, soit si au moins un champ de la ligne
est non vide.

Lorsque un des caractères verticaux semi-graphiques (simple ou double) ou
le caractère de hachurage de mt, ou le caractère pipe (|) apparaissent
dans une ligne, le pointeur d'impression est remplacé à cet endroit
quelque soit la longueur des champs précédents.

Le caractère ampersand a le même effet, mais n'est pas imprimé sauf s'il
est doublé.

La fonction
&CO
	PR_file(filename, pg)
&TX
vaut simplement
&CO
	PR_file_col(filename, pg, 0, -1, 0)
&TX

&RT Si le fichier ne peut être ouvert, PR_file retourne -1. Sinon
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

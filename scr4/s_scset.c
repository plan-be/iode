#include "scr.h"

/*  SET FIELD IN ISAM FROM PAGE DEFINITION */

char    *SC_REC_DEF, *SC_REC;

/*NH*/
SC_set_isam_gn_1(is, nb_read, nb_found, success)
ISAM    *is;
long    nb_read, nb_found;
int     success;
{
    ISFIELD     *isf;
    int         i;

    if(success != 1) return(0);
    isf = is->is_flds;
    for(i = 0 ; i < is->is_nb_flds ; i++, isf++)
	if(SC_REC_DEF[i] == 0)
	    memcpy(is->is_rec + isf->isf_start, SC_REC + isf->isf_start, isf->isf_length);

    return(IS_rewrite(is));
}


/* ======================================================================
Fixe la valeur de certains champs de tous les records existant de l'ISAM
is. rec indique les champs à fixer et leur valeur.

Si disp n'est pas nul, un compteur indiquant le nombre de records lus
apparaît dans le bas de l'écran. Ce compteur est rafraîchi
toutes les disp lectures. Si disp vaut 0, ce compteur n'apparaît pas.

Si stop vaut 1, la frappe de n'importe quelle touche en cours de
traitement arrête celui-ci et il est demandé à l'utilisateur s'il faut
ou non continuer. Si stop vaut 0, le processus se poursuit jusqu'à la
fin du fichier.

&TI Sélection des records
─────────────────────
La sélection des records s'effectue comme suit : tous les champs de
l'ISAM définis par une valeur non vide (nulle ou blanche) dans rec
sont remplacés par la valeur indiquée dans rec.

&EX
    char    *old, tmp[256];

    old = is_client->is_rec;
    is_client->is_rec = tmp;
    memset(tmp, 0, 256);        ---> record nul
    is_client_VAL = 1000;       ---> champ val fixé à 1000
    is_client->is_rec = old;    ---> remise en place de l'ancien record
    SC_set_isam_gn(is_client, rec, 1, 1);
&TX
&RT nombre de records lus (long)
&SA SC_scan_isam_gn(), SC_set_page_gn()
------------------------------------------------------------------------ */

long SC_set_isam_gn(is, rec, disp, stop)
ISAM    *is;
char    *rec;
int     disp, stop;
{
    long    nb_read = 0L;
    ISFIELD *isf;
    int     i, success;

    /* CHECK IF FIELDS ARE EMPTY */
    SC_REC = rec;
    SC_REC_DEF = SCR_malloc(is->is_nb_flds);
    isf = is->is_flds;
    for(i = 0 ; i < is->is_nb_flds ; i++, isf++)
	SC_REC_DEF[i] = IS_is_fld_empty(rec + isf->isf_start, isf->isf_type, isf->isf_length);

    nb_read = SC_scan_isam_gn(is, 0L, 0L, 0, SC_set_isam_gn_1, SCR_UPDATE, disp, stop, 0);

    SCR_free(SC_REC_DEF);
    return(nb_read);
}

/* ======================================================================
Fixe la valeur de certains champs de tous les records existant de l'ISAM
is. pg indique la PAGE dont les champs doivent remplacer ceux
actuellement dans l'ISAM.

Si disp n'est pas nul, un compteur indiquant le nombre de records lus
apparaît dans le bas de l'écran. Ce compteur est rafraîchi
toutes les disp lectures. Si disp vaut 0, ce compteur n'apparaît pas.

Si stop vaut 1, la frappe de n'importe quelle touche en cours de
traitement arrête celui-ci et il est demandé à l'utilisateur s'il faut
ou non continuer. Si stop vaut 0, le processus se poursuit jusqu'à la
fin du fichier.

&TI Sélection des records
─────────────────────
La sélection des records s'effectue comme suit : tous les champs de
l'ISAM définis par une valeur non vide (nulle ou blanche) dans les
champs de la PAGE pg sont remplacés par la valeur indiquée dans rec.
Ceux qui n'apparaissent pas dans la PAGE ne sont pas modifiés.

&EX
    PG_edit_window(pg_client_modif);
    SC_set_isam_gn(pg_client_modif, is_client, 1, 1);
&TX
&RT nombre de records lus (long)
&SA SC_scan_page_gn(), SC_set_isam_gn()
------------------------------------------------------------------------ */

long SC_set_page_gn(pg, is, disp, stop)
PAGE    *pg;
ISAM    *is;
int     disp, stop;
{
    long    total = 0L;
    char    *rec, *buf;

    /* SAVE CRIT INTO FROM AND TO */
    rec = SCR_malloc(is->is_rec_len);
    buf = is->is_rec;
    is->is_rec = rec;
    SCR_page_to_isam(pg, is);
    is->is_rec = buf;

    total = SC_set_isam_gn(is, rec, disp, stop);

    SCR_free(rec);
    return(total);
}


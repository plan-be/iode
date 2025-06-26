#include "scr.h"


/*NH*/
PG_number_group_radio(pg, fld_nb, flag)        /* BP_M 21-01-97 */
PAGE    *pg;
int     fld_nb;
int     flag;
{
    int     i, gr_r, j;
/*    int     nb = 0; /* JMP 16-01-98 */
    FIELD   *fld;

    j = PG_find_first_radio(pg, fld_nb);
    if(j < 0) return(0);
    gr_r = pg->pg_flds[j].fld_radio;
    for(i = 0, fld = pg->pg_flds ; i < pg->pg_nb_flds ; i++, fld++)
	if(fld->fld_type == SCR_TYPE_RADIO &&
	   (fld->fld_radio == gr_r || fld->fld_radio - 127 == gr_r)) {
	     /*    nb++; /* JMP 16-01-98 */
		if(flag) PG_display_field(pg, i);
	}

    return(0);
}


/*NH*/
/* =+====== PG_find_val_radio ==================================
    retourne la valeur du groupe radio
    retourne -1 si le numéro du champs ne correspond pas
    à un champ radio
   ============================================================== */

PG_find_val_radio(pg, nfld)     /* BP_M 21-01-97 */
PAGE    *pg;
int     nfld;
{
    FIELD   *fld = pg->pg_flds + nfld;
    int     fradio;

    if(fld->fld_type != SCR_TYPE_RADIO) return(-1);
    fradio = PG_find_first_radio(pg, nfld);

    return((int)SCR_get_field_short(pg, fradio));
}

/*NH*/
/* =+====== PG_find_first_radio ================================
    retourne le numéro du premier champ d'un groupe radio
    retourne -1 si pas trouvé
   ============================================================== */

PG_find_first_radio(pg, nfld)   /* BP_M 21-01-97 */
PAGE    *pg;
int     nfld;
{
    FIELD   *fld = pg->pg_flds + nfld;
    int     fradio, i, j;

    if(!fld->fld_radio) return(-1);
    if(fld->fld_radio < 128) return(nfld);
    j = fld->fld_radio - 127;

    for(i = 0, fld = pg->pg_flds ; i < pg->pg_nb_flds ; i++, fld++)
	if(fld->fld_radio == j) return(i);

    PG_display_error("Internal error radio");
    return(-1);
}

/*NH*/
/* =+====== PG_display_group_radio =============================
    Retourne le numéro d'ordre d'un champ dans un groupe radio
   ============================================================== */

PG_calc_nummer_radio(pg, fld_nb)        /* BP_M 21-01-97 */
PAGE    *pg;
int     fld_nb;
{
    int     i;
    FIELD   *fld;

    i = PG_find_first_radio(pg, fld_nb);
    return(fld_nb - i);
}


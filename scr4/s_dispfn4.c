#include "scr.h"

/* =======================================================================
Affiche tous les champs de la PAGE pg dans l'IMAGE de l'écran ou
l'image SCR_TMP_IMAGE.

Cette fonction est compatible Windows et mode caractère.

&SA PG_display_flds(), PGd_display_field()
=========================================================================*/

PGd_display_flds(pg)
PAGE    *pg;
{
    SCR_apply_all_cmp(pg);      /* BP_M 19-10-95 */
    PGd_display_flds_no_cmp(pg);
    return(0);
}

/*NH*/
PGd_display_flds_no_cmp_1(pg)
PAGE    *pg;
{
    int     i;

    if(PG_EXEC_DISP_FN_FLDS) ACT_exec_nu(pg->pg_disp_act); /*JMP 25-05-95*/
    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	PGd_display_field(pg, i);

    return(0);
}

/*NH*/
PGd_display_flds_no_cmp(pg)
PAGE    *pg;
{
    TPAGES  *tpg;
    int     i;

    if(SCR_PTR_TYPE(pg) == SCR_TYPE_TPAGES) {
	tpg = (TPAGES *)pg;
	for(i = 0 ; i < tpg->tp_nb ; i++)
	    PGd_display_flds_no_cmp_1(PAGE_PTR(tpg->tp_pgsnbs[i]));
	}
    else
	PGd_display_flds_no_cmp_1(pg);

    return(0);
}



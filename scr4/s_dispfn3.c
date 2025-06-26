#include "scr.h"

/* =======================================================================
Affiche le champs fld_nb de la PAGE pg dans l'IMAGE de l'écran ou
l'image SCR_TMP_IMAGE.

Cette fonction est compatible Windows et mode caractère.

&SA PG_display_field(), PGd_display_flds()
=========================================================================*/

PGd_display_field(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    FIELD   *fld;
    char    *edtxt;
    char    buf_disp[80];
    int     flag = 0, len, i;

    fld = pg->pg_flds + fld_nb;

    if(fld->fld_io == SCR_HIDDEN - SCR_INPUT) return(0);

    /* Set default value */
    PG_set_dft_field(pg, fld_nb);

    /* Execute the display function */
    ACT_exec_nu(fld->fld_disp_act);

    /* Execute the begin function */    /* BP_M 30-03-1995 */
    if(SCR_EXEC_BEG_IN_DISP == 1)       /* BP_M 30-03-1995 */
	ACT_exec_nu(fld->fld_begin_act);  /* BP_M 30-03-1995 */

    if(fld->fld_type == SCR_EDITOR) {
#ifndef SCRWIN
	PG_create_fld_mmt(pg, fld_nb, 1);
#endif
	return(0);
    }

    len = fld->fld_nb_line * fld->fld_nb_column;
    if(len > 79) {
	edtxt = PG_aget_rec_txt(pg, fld_nb);
	flag = 1;
    }
    else {
	edtxt = buf_disp;
	PG_get_rec_txt(pg, fld_nb, edtxt, 1);
    }

    if(fld->fld_type == SCR_PASSWD) {
	for(i = 0 ; i < len ; i++)
	    if(edtxt[i] > ' ') edtxt[i] = '*';
    }

    PGd_write_field_cmp(pg, fld_nb, edtxt, 0); /* JMP_M 4.19 29-05-95 */
    if(flag) SCR_free(edtxt);

#ifdef WINDOWS
    SCR_wdispatch();
#endif

    return(0);
}



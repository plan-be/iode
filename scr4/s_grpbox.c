#include "scr.h"


PG_set_lc_grp(pg, nb)       /* BP_M 17-04-1997 */
PAGE    *pg;
int     nb;
{
    int     i, max_l = 0, max_c = 0, min_l = 255, min_c = 255, j, if_scr = 0;
    FIELD   *fld = pg->pg_flds;
    DRAWS   *drws = &(pg->pg_drws);
    int     nb_drws = drws->dw_nb_drws;
    DRAW    *drw;
    GRPBOX  *gr;

    if(pg->pg_scr_pos < 0) if_scr = 1;

    gr = pg->pg_grpbox + nb;
    for(i = 0 ; i < pg->pg_nb_flds ; i++, fld++)
	if(fld->fld_grp - 1 == nb) {
	    j = 0;
	    if(fld->fld_type == SCR_TYPE_RADIO || fld->fld_type == SCR_CHECK_BOX) j = 2;
	    min_l = _min_(min_l, fld->fld_line);
	    max_l = _max_(max_l, fld->fld_line + fld->fld_nb_line);
	    min_c = _min_(min_c, fld->fld_column);
	    max_c = _max_(max_c, fld->fld_column + fld->fld_nb_column + j);
	}
    for(i = 0 ; i < nb_drws ; i++) {
	drw = drws->dw_drws + i;
	if(drw->drw_grp - 1 == nb) {
	    j = 0;
	    min_l = _min_(min_l, drw->drw_line);
	    max_l = _max_(max_l, drw->drw_line + drw->drw_nb_line);
	    min_c = _min_(min_c, drw->drw_column);
	    max_c = _max_(max_c, drw->drw_column + drw->drw_nb_column + j);
	}
    }
    if(!gr->gr_f_line) gr->gr_line = min_l ;
    if(!gr->gr_f_col ) gr->gr_col  = min_c + if_scr;
    if(!gr->gr_f_nbl ) gr->gr_nbl  = max_l - gr->gr_line + 2*if_scr;
    if(!gr->gr_f_nbc ) gr->gr_nbc  = max_c - gr->gr_col - if_scr;
    return(0);
}






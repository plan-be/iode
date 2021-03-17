#include "scr.h"

PG_calc_cmt(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    if(fld_nb < 0) return(0);
    SCR_get_comment(pg->pg_cmt_pos, pg->pg_flds[fld_nb].fld_cmt, pg->pg_cmt);
    SCR_scmt_free();
    SCR_scmt_add_fnk(pg->pg_flds[fld_nb].fld_fnks);
    SCR_scmt_add_fnk(pg->pg_fnks);
    SCR_scmt_set_txt();
    return(0);
}


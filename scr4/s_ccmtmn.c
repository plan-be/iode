#include "scr.h"

MN_calc_cmt(mn)
MENU    *mn;
{
    MN_calc_cmt_opt(mn, mn->mn_choice);
    return(0);
}

MN_calc_cmt_opt(mn, optnb)
MENU    *mn;
int     optnb;
{
    SCR_get_comment(mn->mn_cmt_pos,
		    mn->mn_opts[optnb].opt_cmt,
		    mn->mn_cmt);

    return(0);
}


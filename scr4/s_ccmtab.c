#include "scr.h"

AB_calc_cmt(ab, choice)
A_BAR   *ab;
int     choice;
{
    A_OPT   *ao = ab->ab_opts + choice;

    SCR_get_comment(ab->ab_cmt_pos, ao->ao_cmt, ab->ab_cmt);
    return(0);
}


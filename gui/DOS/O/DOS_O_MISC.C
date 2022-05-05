#include "iode.h"

ODE_ask(n)
int     n;
{
    return(SCR_confirme(B_msg(n)));
}

ODE_edit_window(pg)
PAGE    *pg;
{
    if (pg == NULL) return(-1);

    PG_edit_window(pg);
    if(SCR_APP_ABORT || SCR_LKEY == SCR_ESCAPE) return(-1);

    return(0);
}

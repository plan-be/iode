#include "scr.h"

/*NH*/
SCR_cmt_alloc()
{
    int     lg;

    lg = SCR_CMT.c_nc * SCR_CMT.c_nl + 10;
    lg = _max_(lg, 81);
    SCR_free(SCR_CMT_TXT);                  /* JMP 11-01-11 */
    SCR_CMT_TXT = (char *)SCR_malloc(lg);   /* JMP 11-01-11 */

    return(0);
}


#include "scr4.h" /* JMP 06-01-98 */

PG_check_fmt_date(str, fmt)
char    *str, *fmt;
{
    return(SCR_check_fmt_date(str, fmt));
}

long PG_fdate_to_long(txt, fmt)
char    *txt, *fmt;
{
#ifndef __cplusplus
    extern long SCR_fdate_to_long();
#endif

    return(SCR_fdate_to_long(txt, fmt));
}



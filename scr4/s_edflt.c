#include "scr4.h" /* JMP 06-01-98 */

PG_check_fmt_time(str, fmt)
char    *str, *fmt;
{
    return(SCR_check_fmt_time(str, fmt));
}

long PG_ftime_to_long(txt, fmt)
char    *txt, *fmt;
{
    return(SCR_ftime_to_long(txt, fmt));
}




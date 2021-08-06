// copied/pasted from the /cmd directory

#define _IODEVARG_
#include "../../api/iode.h"

kerror(level, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10)
int     level;
char    *st1, *st2, *st3, *st4, *st5, *st6, *st7, *st8, *st9, *st10;
{
    printf(st1, st2, st3, st4, st5, st6, st7, st8, st9, st10);
    printf("\n");
}

kmsg(st1, st2, st3, st4, st5, st6, st7, st8, st9, st10)
char    *st1, *st2, *st3, *st4, *st5, *st6, *st7, *st8, *st9, *st10;
{
    printf(st1, st2, st3, st4, st5, st6, st7, st8, st9, st10);
    printf("\n");
}
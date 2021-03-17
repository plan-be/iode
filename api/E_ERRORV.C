#define _IODEVARG_
#include "iode.h"

void E_msg(st1, st2, st3, st4, st5, st6, st7, st8, st9, st10)
char    *st1, *st2, *st3, *st4, *st5, *st6, *st7, *st8, *st9, *st10;
{
    kmsg(st1, st2, st3, st4, st5, st6, st7, st8, st9, st10);
}

void E_error(st1, st2, st3, st4, st5, st6, st7, st8, st9, st10)
char    *st1, *st2, *st3, *st4, *st5, *st6, *st7, *st8, *st9, *st10;
{
    B_seterror(st1, st2, st3, st4, st5, st6, st7, st8, st9, st10);
}

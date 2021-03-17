
#define _IODEVARG_
#include <iode.h>

int     K_SCR = 0, K_MSG = 0;

kerror(level, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10)
int     level;
char    *st1, *st2, *st3, *st4, *st5, *st6, *st7, *st8, *st9, *st10;
{
    char    buf[10256];

    sprintf(buf, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10);

    if (K_SCR == 0)  {
	fprintf(stderr, "%-79.79s\n", buf);
    }
    else  {
	PG_display_error(buf);
    }

    if(level == 1) {
	if(K_SCR != 0) SCR_end();
	exit(1);
    }
}

kmsg(st1, st2, st3, st4, st5, st6, st7, st8, st9, st10)
char    *st1, *st2, *st3, *st4, *st5, *st6, *st7, *st8, *st9, *st10;
{
    char    buf[10256];

    if(K_MSG == 1) kclmsg(0, 0);

    K_MSG = 1;
    sprintf(buf, st1, st2, st3, st4, st5, st6, st7, st8, st9, st10);

    if(K_SCR == 0)  fprintf(stderr, "%-79.79s\r", buf);
    else {
	kwrmsg(buf);
//      AddDebug(buf);
    }

}

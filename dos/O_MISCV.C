#include <scr.h>

ODE_error(n, a, b, c, d, e, f, g)
int     n;
unsigned char *a, *b, *c, *d, *e, *f, *g;
{
    extern char *B_msg(int);
    PG_display_error(B_msg(n), a, b, c, d, e, f, g);
    return(0);
}


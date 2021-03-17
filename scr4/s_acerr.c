#include <stdio.h>

#ifdef SCRPROTO
extern int A_error(char *);
#endif

/*NH*/
int A_cerror(a1, a2, a3, a4, a5)
char    *a1, *a2, *a3, *a4, *a5;
{
    char    buf[120];

    sprintf(buf, a1, a2, a3, a4, a5);
    A_error(buf);
    return(0);
}

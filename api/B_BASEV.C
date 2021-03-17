#include <scr.h>

char    **B_ERROR_MSG = NULL;
int     B_ERROR_NB = 0;

B_seterror(c1, c2, c3, c4, c5, c6, c7)
char    *c1, *c2, *c3, *c4, *c5, *c6, *c7;
{

    char    buffer[256];

    sprintf(buffer, c1, c2, c3, c4, c5, c6, c7);
    SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, buffer);
}


B_seterrn(n, c1, c2, c3, c4, c5, c6)
int     n;
char    *c1, *c2, *c3, *c4, *c5, *c6;
{
    char    *B_msg();

    B_seterror(B_msg(n), c1, c2, c3, c4, c5, c6);
}


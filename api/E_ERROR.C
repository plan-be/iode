#include "iode.h"

void E_msg_n(n)
int n;
{
    char    *B_msg();

    E_msg(B_msg(200 + n));
}

void E_error_n(n)
int n;
{
    char    *B_msg();

    E_error(B_msg(200 + n));
}

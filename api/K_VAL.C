#include "iode.h"

char *K_oval(kdb, pos, n)
KDB     *kdb;
int     pos, n;
{
    return(P_get_ptr(SW_getptr(kdb->k_objs[pos].o_val), n));
}

char *K_oval0(kdb, pos)
KDB     *kdb;
int     pos;
{
    return(K_oval(kdb, pos, 0));
}

char *K_oval1(kdb, pos)
KDB     *kdb;
int     pos;
{
    return(K_oval(kdb, pos, 1));
}
 IODE_REAL *K_vval(kdb, pos, t)
KDB     *kdb;
int     pos, t;
{
    return(((IODE_REAL *)K_oval(kdb, pos, 0)) + t);
}





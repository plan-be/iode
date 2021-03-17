#include "iode.h"
 IODE_REAL    *L_getvar(kdb, pos)
KDB     *kdb;
int     pos;
{
    return(KVVAL(kdb, pos, 0));
}
 IODE_REAL    L_getscl(kdb, pos)
KDB     *kdb;
int     pos;
{
    SCL *scl;

    scl = KSVAL(kdb, pos);
    return(scl->val);
}

SAMPLE  *L_getsmpl(kdb)
KDB     *kdb;
{
    return((SAMPLE *) KDATA(kdb));
}

int L_findscl(kdb, name)
KDB     *kdb;
char    *name;
{
    return(K_find(kdb, name));
}

int L_findvar(kdb, name)
KDB     *kdb;
char    *name;
{
    return(K_find(kdb, name));
}






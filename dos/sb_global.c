#include "iode.h"
#include "o_objs.h"

/* Déplacé dans b_base.c 
B_ScrollSet(arg, plong, inf, sup)
char    *arg;
long    *plong;
int     inf, sup;
{
    int     n;

    n = B_get1int(arg);
    if(n <= -100) return(-1);
    n = max(inf, n);
    n = min(sup, n);
    *plong = n;
    return(0);
}
*/

B_ScrollVarW(arg)
char    *arg;
{
    return(B_ScrollSet(arg, &(global_VW), 2, 12));
}

B_ScrollVarN(arg)
char    *arg;
{
    return(B_ScrollSet(arg, &(global_VN), -1, 6));
}

B_ScrollVarM(arg)
char    *arg;
{
    return(B_ScrollSet(arg, &(global_VM), 0, 2));
}

B_ScrollVarS(arg)
char    *arg;
{
    return(B_ScrollSet(arg, &(global_VS), 0, KSMPL(KV_WS)->s_nb - 1));
}

B_ScrollSclW(arg)
char    *arg;
{
    return(B_ScrollSet(arg, &(global_SW), 2, 12));
}

B_ScrollSclN(arg)
char    *arg;
{
    return(B_ScrollSet(arg, &(global_SN), -1, 6));
}

B_ScrollTblW(arg)
char    *arg;
{
    return(B_ScrollSet(arg, &(global_TW), 2, 60));
}

B_ScrollVTW(arg)
char    *arg;
{
    return(B_ScrollSet(arg, &(global_VTW), 2, 12));
}

B_ScrollVTW0(arg)
char    *arg;
{
    return(B_ScrollSet(arg, &(global_VT0W), 2, 60));
}

B_ScrollVTN(arg)
char    *arg;
{
    return(B_ScrollSet(arg, &(global_VTN), -1, 6));
}

B_DataEditGlobal(VM, VN)
int VM, VN;
{
    global_VM = VM;
    global_VN = VN;
}






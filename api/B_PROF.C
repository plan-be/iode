#include "iode.h"

B_ProfileLoad(arg)
char    *arg;
{
    if(SCR_prof_load(arg) == 0) return(0);
    B_seterrn(57, arg);
    return(-1);
}

B_ProfileSave(arg)
char    *arg;
{
    if(SCR_prof_save(arg) == 0) return(0);
    B_seterrn(58, arg);
    return(-1);
}

B_ProfileReset()
{
    SCR_prof_reset();
    return(0);
}

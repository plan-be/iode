#include "scr.h"

/* ===================================================================
    Calcul de l'activation key d'un programme SCR.
   =================================================================== */

long SCR_ak()
{
    U_ch    buf[200];

    if(SCR_serialnb == 0) return(0L);
    sprintf(buf, "%ld%s", SCR_limdate, SCR_serialnb);
    if(SCR_username) strcat(buf, SCR_username);
    SCR_sqz(buf);
    SCR_upper(buf);
    return(KEY_calc(buf));
}


#include "scr.h"

/* ==================================================================
     Vérification de la clé d'activation et de la date limite
   ================================================================== */

C_check_ak()
{
    int     diff;

    if(SCR_serialnb && SCR_ak() != SCR_actkey) {
	Werror("Activation key rejected. Sorry\n"); /* JMP 08-01-97 */
	exit(1);
	}

    if(SCR_limdate) {
	diff = 1 + DT_diff_dates(SCR_limdate, _SCR_current_date());
	if(diff < 0) {
	    Werror("Expiration date reached. Sorry\n"); /* JMP 08-01-97 */
	    exit(0);
	    }
	if(diff < 30) {
	    Werror("Warning : the expiration date will be reached in %d days.\n", diff);
	    Wprintf("Please contact your dealer. Press ENTER to continue\n");
#ifndef DOSWIN
	    while(getchar() != '\n');
#endif
	    }
	}

    return(0);
}





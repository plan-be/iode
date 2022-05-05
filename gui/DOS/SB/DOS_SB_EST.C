#include <iodedos.h>

SB_EqsEstimate()
{
    PG_edit_window(vkp_eqs);
    return(0);
}
/*
C_EqsEstimate()
{
    char    *buf;
    int     rc;

    if(C_DataEditEqs1())  {
	B_display_last_error();
	return(-1);
    }

    buf = SW_nalloc(strlen(vkp_eqs_NAME) + 25);
    if(buf == NULL) return(-1);
    sprintf(buf, "%s %s %s", vkp_eqs_FROM, vkp_eqs_TO, vkp_eqs_NAME);

    if(B_EqsEstimate(buf)) {
	B_display_last_error();
	rc = -1;
    }
    else {
	ODE_error(OM_EST_ACH);
	rc = 0;
    }

    SW_nfree(buf);
    return(rc);
}
*/




#include "scr.h"


char    *SCR_PROT_SEPARATEURS = ",;|";
int     SCR_CHECK_PROT = 1;

SCR_check_prot_vt(char *prot)
{
    unsigned  char  **tbl;
    int             i, ret = 0;

    if(!prot) return(0);

    tbl = SCR_vtoms(prot, SCR_PROT_SEPARATEURS);
    for(i = 0 ; tbl[i] ; i++)
	if(SCR_check_prot(tbl[i]) != 0) {
	    ret = -1;
	    break;  /* BP_M 08-02-2010 09:53 */
	}

    SCR_free_tbl(tbl);

    return(ret);
}



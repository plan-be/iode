#include <iodedos.h>

/* Model Simulation */

SB_ModelSimulate()
{
    PG_edit_window(vkip_sim);
    return(0);
}

C_ModelSimulate()
{
    char    *buf, *exo = NULL;
    int     rc = 0, lg = 0;

    if(C_ModelSimulateCnf()) {
	B_display_last_error();
	return(-1);
	}
    if(vkip_sim_EQS != NULL) lg = strlen(vkip_sim_EQS);
    buf = SW_nalloc(lg + 25);
    if(buf == NULL) return(-1);
    sprintf(buf, "%s %s %s", vkip_sim_FROM, vkip_sim_TO,
			     ((lg == 0) ? "" : vkip_sim_EQS));

    if(vkip_sim_EXO != NULL) B_ModelExchange(vkip_sim_EXO);

    if(B_ModelSimulate(buf)) {
	B_display_last_error();
	rc = -1;
    }

    SW_nfree(buf);
    return(rc);
}

C_ModelSimulateCnf()
{
    char    buf[81];

	sprintf(buf, "%lf %lf %ld %c %d %c %d", 
		    vkip_sim_EPS,
		    vkip_sim_RELAX,
		    vkip_sim_MAXIT,
		    "CTN"[vkip_sim_SORT],
		    vkip_sim_START,
		    "NY"[vkip_sim_DEBUG],
			vkip_sim_PASSES);

    return(B_ModelSimulateParms(buf));
}

SB_ModelCompile()
{
    PG_edit_window(vkip_compile);
    return(0);
}

C_ModelCompile()
{
    if(B_ModelCompile(vkip_compile_EQS)) {
	B_display_last_error();
	return(-1);
    }

    return(0);
}

/*************************** SCC Decomposition ***************************/
SB_ModelCalcSCC()
{
    PG_edit_window(vkip_calcscc);
    return(0);
}

C_ModelCalcSCC()
{
	char *buf;
	int	 lg = 0, rc;

    if(vkip_calcscc_EQS != NULL) lg = strlen(vkip_calcscc_EQS);
    buf = SW_nalloc(lg + 100);
    if(buf == NULL) return(-1);
	sprintf(buf, "%d %s %s %s %s", 
					vkip_calcscc_PASSES, vkip_calcscc_PRE, 
					vkip_calcscc_INTER, vkip_calcscc_POST,
					((lg == 0) ? "" : vkip_calcscc_EQS));
	
	rc = B_ModelCalcSCC(buf);
	SCR_free(buf);
	if(rc) B_display_last_error();
    return(rc);
}

SB_ModelSimulateSCC()
{
    PG_edit_window(vkip_simscc);
    return(0);
}

C_ModelSimulateSCC()
{
	char buf[1024];
	int	 rc;
	
	// 1. Parms
	sprintf(buf, "%lf %lf %ld %c %d %c %d",
		    vkip_simscc_EPS,
		    vkip_simscc_RELAX,
		    vkip_simscc_MAXIT,
			"CTN"[vkip_sim_SORT], // c'est bien sim car simscc ne demande pas ce parm
		    vkip_simscc_START,
		    "NY"[vkip_sim_DEBUG],
			vkip_sim_PASSES); // c'est bien sim car simscc ne demande pas ce parm);

	if(B_ModelSimulateParms(buf) < 0) {
		B_display_last_error();
		return(-1);
		}

	// 2. Simulation SCC
	sprintf(buf, "%s %s %s %s %s", 
					vkip_simscc_FROM, vkip_simscc_TO, 
					vkip_simscc_PRE, vkip_simscc_INTER, vkip_simscc_POST);
	
	rc = B_ModelSimulateSCC(buf);
	if(rc) B_display_last_error();
    return(rc);
}







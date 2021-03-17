#include "iode.h"

char **B_EqsSplitSmplName(arg, psmpl)
char    *arg;
SAMPLE  **psmpl;
{
    int     lg1, lg2;
    char    from[16], to[16], **eqs = 0;
    SAMPLE  *smpl;

    lg1 = B_get_arg0(from, arg, 15);
    lg2 = B_get_arg0(to, arg + lg1, 15);
    *psmpl = PER_atosmpl(from, to);

    if(*psmpl == NULL) {
	B_seterrn(39, from, to);
	return(eqs);
    }

    eqs = B_ainit_chk(arg + lg1 + lg2, NULL, 0);
    return(eqs);
}

B_EqsEstimate(arg)
char    *arg;
{
    int     rc = -1;
    char    **eqs;
    SAMPLE  *smpl;

    eqs = B_EqsSplitSmplName(arg, &smpl);
    if(smpl == 0) return(-1);
    rc = B_EqsEstimateEqs(smpl, eqs);
    SCR_free_tbl(eqs);
    SCR_free(smpl);
    return(rc);
}

B_EqsEstimateEqs(smpl, eqs)
SAMPLE  *smpl;
char    **eqs;
{
    KDB     *tdbe;
    int     rc;

    if (eqs == NULL || SCR_tbl_size(eqs) == 0) {
	B_seterrn(40);
	return(-1);
    }
    else {
	tdbe = K_refer(KE_WS, SCR_tbl_size(eqs), eqs);
	if(tdbe == 0 || KNB(tdbe) == 0)                                         /* JMP 12-05-11 */
	    rc = -1;                                                            /* JMP 12-05-11 */
	else                                                                    /* JMP 12-05-11 */
	    rc = KE_est_s(tdbe, KV_WS, KS_WS, smpl, SCR_tbl_size(eqs), eqs);    /* JMP 12-05-11 */
	K_free_kdb(tdbe);
    }

    return(rc);
}

/* EQSSET* */


B_EqsSetSample(arg)
char    *arg;
{
    int     rc = 0, i;
    char    **eqs;
    SAMPLE  *smpl;

    eqs = B_EqsSplitSmplName(arg, &smpl);
    if(smpl == 0) return(-1);
    for(i = 0 ; eqs[i] ; i++) {
	rc = B_DataUpdateEqs(eqs[i], 0L, 0L, -1, smpl, 0L, 0L, 0L, 0);
	if(rc) break;
	}

    SCR_free_tbl(eqs);
    SCR_free(smpl);
    return(rc);
}

B_EqsSetMethod(arg)
char    *arg;
{
    int     lg1, meth, rc = 0, i;
    char    tmeth[16], **eqs = 0;

    lg1 = B_get_arg0(tmeth, arg, 15);
    meth = atoi(tmeth);
    eqs = B_ainit_chk(arg + lg1, NULL, 0);
    for(i = 0 ; eqs[i] ; i++) {
	rc = B_DataUpdateEqs(eqs[i], 0L, 0L, meth, 0L, 0L, 0L, 0L, 0);
	if(rc) break;
	}

    SCR_free_tbl(eqs);
    return(rc);
}

B_EqsSetBloc(arg)
char    *arg;
{
    int     rc = 0, i;
    char    **eqs = 0, *bloc;
    U_ch    *SCR_mtov();

    eqs = B_ainit_chk(arg, NULL, 0);
    bloc = SCR_mtov(eqs, ';');
    for(i = 0 ; eqs[i] ; i++) {
	rc = B_DataUpdateEqs(eqs[i], 0L, 0L, -1, 0L, 0L, bloc, 0L, 0);
	if(rc) break;
	}

    SCR_free_tbl(eqs);
    SCR_free(bloc);
    return(rc);
}

B_EqsSetCmt(arg)
char    *arg;
{
    int     lg1, rc = 0;
    char    name[16];

    lg1 = B_get_arg0(name, arg, K_MAX_NAME + 1);
    rc = B_DataUpdateEqs(name, 0L, arg + lg1 + 1, -1, 0L, 0L, 0L, 0L, 0);
    return(rc);
}

B_EqsSetInstrs(arg)
char    *arg;
{
    int     lg1, rc = 0;
    char    name[16];

    lg1 = B_get_arg0(name, arg, K_MAX_NAME + 1);
    rc = B_DataUpdateEqs(name, 0L, 0L, -1, 0L, arg + lg1 + 1, 0L, 0L, 0);
    return(rc);
}

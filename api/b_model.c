#include "iode.h"

/* Model Simulation */

B_ModelSimulate(arg)
char    *arg;
{
    int     lg1, lg2;
    int     rc;
    char    from[16], to[16], **eqs;
    SAMPLE  *smpl;

    lg1 = B_get_arg0(from, arg, 15);
    lg2 = B_get_arg0(to, arg + lg1, 15);
    smpl = PER_atosmpl(from, to);

    if(smpl == NULL) {
        B_seterror("ModelSimulate: %s %s wrong sample", from, to);
        return(-1);
    }

    eqs = B_ainit_chk(arg + lg1 + lg2, NULL, 0);
    if(eqs == 0) {
        rc = -1;    /* JMP 06-07-94 */
        goto err;
    }

    rc = B_ModelSimulateEqs(smpl, eqs);

err:
    SCR_free_tbl(eqs);
    SCR_free(smpl);
    return(rc);
}

B_ModelSimulateEqs(smpl, eqs)
SAMPLE  *smpl;
char    **eqs;
{
    KDB     *tdbe;
    int     rc;

    if(eqs == NULL || SCR_tbl_size(eqs) == 0)
        rc = KE_simul_s(K_WS[K_EQS], K_WS[K_VAR], K_WS[K_SCL], smpl, KSIM_EXO, NULL);
    else {
        tdbe = K_refer(K_WS[K_EQS], SCR_tbl_size(eqs), eqs);
        rc = KE_simul_s(tdbe, K_WS[K_VAR], K_WS[K_SCL], smpl, KSIM_EXO, eqs);
        K_free_kdb(tdbe);
    }

    return(rc);
}

/* Simulation Parameters */
B_ModelSimulateParms(arg)
char    *arg;
{
    char    **args;
    int     rc = 0, nargs;
#ifndef WATCOM
    double  atof();
#endif

    args = (char **)SCR_vtoms(arg, B_SEPS);
    nargs = SCR_tbl_size(args);
    if(nargs < 6) {
        B_seterror("ModelSimulateParms : incorrect nb of parameters");
        rc = -1;
        goto fin;
    }

    KSIM_EPS   = atof(args[0]);
    KSIM_RELAX = atof(args[1]);
    KSIM_MAXIT = atoi(args[2]);
    KSIM_SORT  = B_argpos("CTN", args[3][0]);
    KSIM_START = args[4][0] - '0';
    KSIM_DEBUG = B_argpos("NY", args[5][0]);
	if(nargs > 6) KSIM_PASSES = atoi(args[6]); //JMP 14/3/2012

    if(nargs > 7)
        KSIM_DEBUGNEWTON = B_argpos("NY", args[7][0]); // JMP 6/3/2012
    else
        KSIM_DEBUGNEWTON = 0;

fin :
    SCR_free_tbl(args);
    return(rc);
}

/* GoalSeeking Parameters */
B_ModelExchange(arg)
char    *arg;
{
    if(KSIM_EXO) {
        SCR_free_tbl(KSIM_EXO);
        KSIM_EXO = NULL;
    }

    if(arg == NULL || (SCR_strip(arg))[0] == '\0') return(0);
    /*
    KSIM_EXO = (char **) SCR_vtoms(arg, B_SEPS);
    */
    KSIM_EXO = B_ainit_chk(arg, NULL, 0);
    return(0);
}

B_ModelCompile(arg)
char    *arg;
{
    char    **eqs = NULL;
    KDB     *tdbe = NULL;
    int     rc;

    if(arg == NULL || arg[0] == 0) {
        /* EndoExo whole WS */
        return(KE_compile(K_WS[K_EQS]));
    }
    else {
        eqs = B_ainit_chk(arg, NULL, 0);
        if(eqs == NULL || SCR_tbl_size(eqs) == 0)
            return(KE_compile(K_WS[K_EQS]));
        else {
            tdbe = K_refer(K_WS[K_EQS], SCR_tbl_size(eqs), eqs);
            rc = KE_compile(tdbe);
            K_free_kdb(tdbe);
            SCR_free_tbl(eqs);
        }
    }
    return(rc);
}



KE_compile(dbe)
KDB *dbe;
{
    int     i;
    EQ      *eq;

    if(dbe == NULL || KNB(dbe) == 0) {
        B_seterrn(110);
        return(-1);
    }

    for(i = 0; i < KNB(dbe); i++) {
        eq = KEVAL(dbe, i);
        B_DataUpdateEqs(KONAME(dbe, i), eq->lec, NULL, 0, NULL, NULL, NULL, NULL, 0);
        E_free(eq);
    }
}

// Syntax : $ModelCalcSCC nbtris prename intername postname [eqs]

B_ModelCalcSCC(char *arg) 
{
	char    **eqs, buf[256], pre[64], inter[64], post[64];
	int     rc = -1, lg1, tris;
	KDB		*tdbe = NULL;

	// Tri
	lg1 = B_get_arg0(buf,    arg, 15);
	tris = atoi(buf);
	if(tris < 0) tris = 0;
	
	// result list names
	lg1 += B_get_arg0(pre,   arg + lg1, 20);
	lg1 += B_get_arg0(inter, arg + lg1, 20);
	lg1 += B_get_arg0(post,  arg + lg1, 20);
	if(strlen(pre) == 0 || strlen(inter) == 0 || strlen(post) == 0) goto err;
	
	// eqs if given
	eqs = B_ainit_chk(arg + lg1, NULL, 0);
    if(SCR_tbl_size(eqs) == 0)
		tdbe = K_WS[K_EQS];
     else 
		tdbe = K_quick_refer(K_WS[K_EQS], eqs);
        
	rc = KE_ModelCalcSCC(tdbe, tris, pre, inter, post);
    
	if(SCR_tbl_size(eqs) != 0) K_free_kdb(tdbe);
err:    
	SCR_free_tbl(eqs);
    return(rc);
}

/**
Simulate the model based on 3 precalculated lists pre, post and inter
Syntax : ModelSimulateSCC from to pre inter post
**/

B_ModelSimulateSCC(char *arg) 
{
	int     lg1, lg2, rc, prepos, interpos, postpos;
	char    from[16], to[16], **lsts, **eqs, **eqs1, buf[256], **pre, **post, **inter;
    SAMPLE  *smpl;
	KDB     *tdbe;

    lg1 = B_get_arg0(from, arg, 15);
    lg2 = B_get_arg0(to, arg + lg1, 15);
    smpl = PER_atosmpl(from, to);

    if(smpl == NULL) {
        B_seterror("ModelSimulateSCC: %s %s wrong sample", from, to);
        return(-1);
    }

    // Extrait les listes restantes
    lsts = B_ainit_chk(arg + lg1 + lg2, NULL, 0);
    if(lsts == 0 || SCR_tbl_size(lsts) != 3) {
        rc = -1;
		B_seterror("ModelSimulateSCC: syntax error in lists");
        goto err;
    }

	prepos   = K_find(K_WS[K_LST], lsts[0]);
	interpos = K_find(K_WS[K_LST], lsts[1]);
	postpos  = K_find(K_WS[K_LST], lsts[2]);
    SCR_free_tbl(lsts);

	if(prepos < 0 || interpos < 0 || postpos < 0) {
		rc = -1;
		B_seterror("ModelSimulateSCC: pre, post or inter list not in WS");
        goto err;
	}
	
	pre   = KL_expand(KLVAL(K_WS[K_LST], prepos));
	inter = KL_expand(KLVAL(K_WS[K_LST], interpos));
	post  = KL_expand(KLVAL(K_WS[K_LST], postpos));
	
	// Regroupe les listes dans une seule avant de faire K_quick_refer
	eqs1 = SCR_union_quick(pre, inter); // JMP 29/8/2012
	eqs = SCR_union_quick(eqs1, post);  // JMP 29/8/2012
    SCR_free_tbl(eqs1);                 // JMP 29/8/2012
	tdbe = K_quick_refer(K_WS[K_EQS], eqs);
	SCR_free_tbl(eqs);
	
	// Lance la simulation
	rc = KE_simul_SCC(tdbe, K_WS[K_VAR], K_WS[K_SCL], smpl, pre, inter, post);
    
	// Cleanup
	K_free_kdb(tdbe);
	SCR_free_tbl(pre);
	SCR_free_tbl(inter);
	SCR_free_tbl(post);

err:
    SCR_free(smpl);
    return(rc);
}

extern IODE_REAL   *KSIM_NORMS;	// Norme de convergence par année JMP 21/3/2012

/*
Set all values of var <name> to NaN. 
Create var <name> if needed.
*/

int B_CreateEmptyVar(char *name)
{
    char        uname[1024], buf[4096];

    // Create var 
    strcpy(uname, name);
    SCR_sqz(SCR_upper(uname));
    sprintf(buf, "%s 1/0", uname); 
    B_DataCalcVar(buf);
    return(0);
}

IODE_REAL *B_GetVarPtr(char *name)
{
    int     pos;
    KDB     *dbv = K_WS[K_VAR];
    char    uname[1024];

    // get var position in KDB
    strcpy(uname, name);
    SCR_sqz(SCR_upper(uname));
    pos = K_find(dbv, uname);
    if(pos < 0) 
        return(NULL); 
    else
        return(KVVAL(dbv, pos, 0));
}

int B_CreateVarFromVecOfDoubles(char *name, IODE_REAL *vec)
{
    KDB         *dbv = K_WS[K_VAR];
    IODE_REAL   *x;
    int         t;

    // Create var and get Ptr
    B_CreateEmptyVar(name);
    x = B_GetVarPtr(name);
    if(x == 0) return(-1);
    
    // Copy values
    if(vec) {
        for(t = 0; t < KSMPL(dbv)->s_nb; t++)  
            x[t] = vec[t];
    }
    return(0);
}

int B_CreateVarFromVecOfInts(char *name, int *vec)
{
    KDB         *dbv = K_WS[K_VAR];
    IODE_REAL   *x;
    int         t;

    // Create var and get Ptr
    B_CreateEmptyVar(name);
    x = B_GetVarPtr(name);
    if(x == 0) return(-1);
    
    // Copy values
    if(vec) {
        for(t = 0; t < KSMPL(dbv)->s_nb; t++)  
            x[t] = vec[t];
    }        
    return(0);
}

int B_ModelSimulateSaveNIters(char *arg)
{
    extern int  *KSIM_NITERS;
    
    return(B_CreateVarFromVecOfInts(arg, KSIM_NITERS));
    
/*    
    KDB         *dbv = K_WS[K_VAR];
    extern int  *KSIM_NITERS;
    int         t;
    IODE_REAL   *x;

    // Create var 
    B_CreateEmptyVar(arg);
    
    // If No Sim, return
    if(KSIM_NITERS == 0) return(0); // Pas encore de simulation

    // Set values in arg
    x = B_GetVarPtr(arg);
    if(x == 0) return(-1);
    for(t = 0; t < KSMPL(dbv)->s_nb; t++)  
        x[t] = KSIM_NITERS[t];
    
    return(0);
*/    
}

int B_ModelSimulateSaveNorms(char *arg)
{
    extern IODE_REAL *KSIM_NORMS;
    
    return(B_CreateVarFromVecOfDoubles(arg, KSIM_NORMS));
}
  
/**
 *  @header4iode
 *
 *  Estimation
 *  ----------
 *  Estimation functions making the link between the estimation package (E_estim()) 
 *  and the workspaces of equations and scalars where the estimation results 
 *  are stored. 
 *  
 *  List of functions
 *  -----------------
 *      int KE_est_s(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** names)      Estimates an equation or a block of equations
 *      int KE_estim(char* veqs, char* afrom, char* ato)                            Estimates an equation or a block of equations on the specified sample. Simplified version of KE_est_s()
 *  
 *  Utility function
 *  ----------------
 *      void E_tests2scl(EQ* eq, int j, int n, int k)                               Creates the scalars containing the results of an estimated equation
 *  
 */

#include "iode.h"


// Fns declarations
static void E_savescl(double val, int eqnb, char*txt);
void E_tests2scl(EQ* eq, int j, int n, int k);
static void E_savevar(char* name, int eqnb, MAT* mat);
static int KE_update(char* name, char* lec, int method, SAMPLE* smpl, float* tests);
int KE_est_s(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** names);
int KE_estim(char* veqs, char* afrom, char* ato);

// Global vars
//char            **EST_ARGS; // not used ?
IODE_REAL       KEST_EPS = 0.00001;
char            KEST_METHOD = 0;
int             KEST_MAXIT = 100;
extern  int     E_T, E_NCE;


/**
 *  Creates or replaces a scalar containing one result of a block estimation. 
 *  
 *  The created (or replaced) coefficient name is e<eqnb>_<txt>. 
 *    
 *  That method allows the use of estimation tests / results in reports.
 *  
 *  See E_tests2scl() for a complete list of scalars.
 *
 *  @param [in] val  Description for val
 *  @param [in] eqnb Description for eqnb
 *  @param [in] txt  Description for txt
 *  
 */

static void E_savescl(double val, int eqnb, char*txt)  
{
    char    buf[40];                        // JMP 25/04/2022 : 20 -> 40 
    static SCL  scl = {0.9, 1.0, L_NAN}; // Why static ?

    scl.val = val;
    sprintf(buf, "e%d_%s", eqnb, txt);
    K_add(KS_WS, buf, &scl);
}


/**
 *  Creates the scalars containing the results of an estimated equation:
 *  
 *      - e<eqnb>_n      : number of observations in estimation SAMPLE 
 *      - e<eqnb>_k      : number of degrees of freedom 
 *      - e<eqnb>_stdev  : stddev of the residuals
 *      - e<eqnb>_meany  : mean of the observed values (Y)
 *      - e<eqnb>_ssres  : sum of square of residuals
 *      - e<eqnb>_stderr : std error of the residuals
 *      - e<eqnb>_fstat  : F-statistic
 *      - e<eqnb>_r2     : R-square
 *      - e<eqnb>_r2adj  : adjusted R-square
 *      - e<eqnb>_dw     : Durbin-Watson test
 *      - e<eqnb>_loglik : Log-likelihood
 *  
 *  @param [in] EQ*     eq  equation 
 *  @param [in] int     j   position of the equation in the block 
 *  @param [in] int     n   number of observations
 *  @param [in] int     k   number of degrees of freedom
 */

void E_tests2scl(EQ* eq, int j, int n, int k)
{
    if(eq == NULL) return;

    E_savescl((double)n, j, "n");
    E_savescl((double)k, j, "k");
    E_savescl(eq->tests[1], j,  "stdev");
    E_savescl(eq->tests[2], j,  "meany");
    E_savescl(eq->tests[3], j,  "ssres");
    E_savescl(eq->tests[4], j,  "stderr");
    E_savescl(eq->tests[5], j,  "sderrp");
    E_savescl(eq->tests[6], j,  "fstat");
    E_savescl(eq->tests[7], j,  "r2");
    E_savescl(eq->tests[8], j,  "r2adj");
    E_savescl(eq->tests[9], j,  "dw");
    E_savescl(eq->tests[10], j, "loglik");
}


/**
 *  Creates or replaces a VAR containing one result of a block estimation. 
 *  
 *  The created (or replaced) VAR name is name<eqnb>, for example 
 *  
 *      E_savevar("_YCALC", 2, E_RHS)
 *  
 *  creates _YCALC containing the RHS of the 2d equation in the estimated block.
 *    
 *  That method allows the use of estimation results in reports.
 *   
 *  @param [in] char* name  Prefix of the variable to create
 *  @param [in] int   eqnb  Suffix of the variable
 *  @param [in] MAT*  mat   content to store in the variable
 */
 
static void E_savevar(char* name, int eqnb, MAT* mat) 
{
    int         t;
    IODE_REAL   *var; 
    
    // Create varname with NaN 
    KV_add(name);
    var = KVPTR(name);
    if(var == NULL) return;
    
    // Copy mat to name from E_FROM to E_FROM + E_T
    for(t = E_FROM; t < E_FROM + E_T; t++) {
        // KV_set_at_t(name, t, (double) MATE(mat, eqnb, t));
        var[t] =  MATE(mat, eqnb, t - E_FROM);
    }
}


/**
 *  Saves an equation after the estimation. Saves only LEC form, method, estimation sample and tests.
 *  
 *  @param [in] char*   name    endogenous name
 *  @param [in] char*   lec     equation
 *  @param [in] int     method  estimation method
 *  @param [in] SAMPLE* smpl    estimation sample or NULL. If NULL, the estimation 
 *                              sample is read from the equation itself
 *  @param [in] float*  tests   list of tests (see KE_est_s() for the complete list)
 *  @return     int     0 on success, -1 on error
 *  
 */                               
 
static int KE_update(char* name, char* lec, int method, SAMPLE* smpl, float* tests)
{
    int     pos, rc;
    EQ      *eq;

    pos = K_find(KE_WS, name);
    if(pos < 0) 
        eq = (EQ *) SW_nalloc(sizeof(EQ));
    else 
        eq = KEVAL(K_WS[K_EQS], pos);

    SW_nfree(eq->lec);
    eq->lec = SCR_stracpy(lec);
    eq->method = method;
    eq->date = SCR_current_date();
    
    memcpy(&(eq->tests), tests, EQS_NBTESTS * sizeof(float));   
    memcpy(&(eq->smpl), smpl, sizeof(SAMPLE));
    rc = K_add(K_WS[K_EQS], name, eq, name);
    if(rc < 0) {
        rc = -1;
        B_seterror(L_error());
    }
    else rc = 0;

    E_free(eq);
    return(rc);
}


/**
 *  Estimates an equation or a block of equations.
 *  
 *  On success, the following results are produced:
 *      - estimated coefficients + stderr in SCL under their names (c1, c2 ...)
 *      - equation tests : 
 *          - in the equations themselves (in eq->eq_tests)
 *          - in global scalars in the form e<eqnb>_<testname> (e.g.: e0_dw, e1_r2...)
 *      - vectors of calculated, observed and residual values in _YCALC, _YOBS and _YRES
 *      
 *  TODO: replace _YCALC by _YCALC_0, _YCALC_1... Idem for _YOBS and Y_RES. Currently, 
 *      if there is more than 1 eq in the estimation block, _YCALC, _YOBS and _YRES correspond 
 *      to the last equation of the block.
 *  
 *  TODO: what if no block is passed in the equation ? 
 *          There should still be a creation of scalars and vars (_YRES...).
 *  
 *  TODO: check why, if the block consists of n equations, we estimate n times
 *  the same block ? Bad programmation ? It seams like we should first create the block of equations,
 *  then the instruments from on eof the equations, then estimate and finally save the
 *  block, instr and methods in each equation. Only one estimation should be required.
 *  
 *  @param [in] KDB*    dbe     KDB of EQ
 *  @param [in] KDB*    dbv     KDB of VAR
 *  @param [in] KDB*    dbs     KDB of SCL
 *  @param [in] SAMPLE* smpl    estimation sample
 *  @param [in] char**  names   vector of equation (endo) names
 *  @return     int             0 on success, -1 on error
 */
 
int KE_est_s(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** names)
{
    static  char met[6] = {"LZIGM"};
    int     i, j, pos, nb,
            method, error = 0,
                    nbl = 0, nbe = 0, nblk;
    SAMPLE  eq_smpl;
    U_ch    **endos = 0;
    U_ch    **blk = 0;
    U_ch    **lecs = 0;
    U_ch    **instrs = 0;
    float   tests[EQS_NBTESTS]; /* FLOAT 12-04-98 */
    EQ      *eq;

    nb = SCR_tbl_size(names);
    
    for(i = 0; i < nb; i++) {
        pos = K_find(dbe, names[i]);
        if(pos < 0) {
            B_seterrn(120, names[i]);
            goto err;
        }

        if(KEST_METHOD == 0) method = KEMETH(dbe, pos);
        else method = 0;

        if(smpl == 0) {
            memcpy(&eq_smpl, &(KESMPL(dbe, pos)), sizeof(SAMPLE));
            smpl = &eq_smpl;
            if(eq_smpl.s_nb < 1) {
                B_seterrn(121, names[i]);
                goto err;
            }
        }

        if(KEINSTR(dbe, pos) == NULL) instrs = NULL;
        else instrs =  SCR_vtoms(KEINSTR(dbe, pos), ",;");

        blk =  SCR_vtoms(KEBLK(dbe, pos), " ,;");
        nblk = SCR_tbl_size(blk);

        if(nblk == 0)  {
            SCR_add_ptr(&lecs, &nbl, KELEC(dbe, pos));
            SCR_add_ptr(&endos, &nbe, names[i]);
        }
        else {
            /* add elements of block */
            for(j = 0; j < nblk; j++) {
                SCR_sqz(blk[j]);
                pos = K_find(KE_WS, blk[j]);
                if(pos < 0) {
                    B_seterrn(120, blk[j]);
                    goto err;
                }
                SCR_add_ptr(&lecs, &nbl, KELEC(KE_WS, pos));
                SCR_add_ptr(&endos, &nbe, KONAME(KE_WS, pos));
            }
        }

        SCR_add_ptr(&lecs, &nbl, 0L);
        SCR_add_ptr(&endos, &nbe, 0L);

        error = E_est(endos, lecs,
                      dbv, dbs, smpl,
                      method, instrs, KEST_MAXIT, KEST_EPS);

        if(error == 0) {
            E_print_results(1, 1, 1, 1, 1);  /* JMP 23-03-98 */

            for(j = 0; j < nbe - 1; j++) {
                tests[0] = (float)MATE(E_MCORRU,      0, j);
                tests[1] = (float)MATE(E_STDEV,       0, j);
                tests[2] = (float)MATE(E_MEAN_Y,      0, j);
                tests[3] = (float)MATE(E_SSRES,       0, j);
                tests[4] = (float)MATE(E_STDERR,      0, j);
                tests[5] = (float)MATE(E_STD_PCT,     0, j);
                tests[6] = (float)MATE(E_FSTAT,       0, j);
                tests[7] = (float)MATE(E_RSQUARE,     0, j);
                tests[8] = (float)MATE(E_RSQUARE_ADJ, 0, j);
                tests[9] = (float)MATE(E_DW,          0, j);
                tests[10]= (float)MATE(E_LOGLIK,      0, j);

                //B_DataUpdateEqs(endos[j], lecs[j], NULL, method, smpl, NULL, NULL, tests, 1);
                KE_update(endos[j], lecs[j], method, smpl, tests);
                pos = K_find(KE_WS, endos[j]);   /* JMP 24-06-98 */
                eq = KEVAL(KE_WS, pos);          /* JMP 24-06-98 */
                E_tests2scl(eq, j, E_T, E_NCE);  /* JMP 27-09-96 */
                E_free(eq);
                eq = NULL;           // GB 14/11/2012
                E_savevar("_YCALC", j, E_RHS);   /* JMP 27-09-96 */
                E_savevar("_YOBS", j, E_LHS);    /* JMP 27-09-96 */
                E_savevar("_YRES", j, E_U);      /* JMP 27-09-96 */
            }
        }

        /* endos, lecs and instruments freed in E_est */
        SCR_free_tbl(blk);

        lecs = endos = instrs = blk = NULL;
        nbl = nbe = 0;

        E_free_work();

        if(error != 0) return(-1);
    }

    return(0);

err :
    // SCR_free_tbl(lecs); // Already freed in E_est()
    // SCR_free_tbl(instrs);
    // SCR_free_tbl(endos);
    SCR_free_tbl(blk);
    return(-1);
}

/**
 *  Estimates an equation or a block of equations on the specified sample.
 *  
 *  Simplified version of KE_est_s() with current global workspaces as parameters instead of any workspaces and
 *  the smpl given in text format.
 *  
 *  @param [in] char*   veqs        comma separated list of equation names (=endo  names)
 *  @param [in] char*   afrom       first period of the estimation sample
 *  @param [in] char*   ato         last period of the estimation sample
 *  @return     int                 0 on success, -1 on error
 *  
 */
int KE_estim(char* veqs, char* afrom, char* ato)
{
    char**  teqs;
    SAMPLE  *smpl;
    int     rc;
    
    smpl = PER_atosmpl(afrom, ato);
    teqs =  SCR_vtoms(veqs, ",; ");     // TODO: replace hard-coded separators by a (new?) variable 
    rc = KE_est_s(KE_WS, KV_WS, KS_WS, smpl, teqs);
    
    SCR_free_tbl(teqs);
    SCR_free(smpl);
    return(rc);
}


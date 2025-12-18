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
 *      int KE_est_s()                          Estimates an equation or a block of equations
 *      int KE_estim(char* afrom, char* ato)    Estimates an equation or a block of equations on the specified sample. Simplified version of KE_est_s()
 *  
 *  Utility function
 *  ----------------
 *      void E_tests2scl(Equation* eq, int j, int n, int k)   Creates the scalars containing the results of an estimated equation
 *  
 */
#include "scr4/s_prodt.h"

#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/equations.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/estimation/estimation.h"


/**
 *  Creates or replaces a scalar containing one result of a block estimation. 
 *  
 *  The created (or replaced) coefficient name is e<eqnb>_<txt>. 
 *    
 *  That method is useful for using estimation tests in reports.
 *  
 *  See E_tests2scl() for a complete list of scalars.
 *
 *  @param [in] double  val  value to put in scalar whose name suffix is txt 
 *  @param [in] int     eqnb position of the equation in the estimated block
 *  @param [in] char*   txt  suffix of the scalar to modify (stdev, meany...)
 *  
 */
void Estimation::E_savescl(double val, int eqnb, char*txt)  
{
    char buf[40];
    Scalar scl(0.9, 1.0, IODE_NAN);

    scl.value = val;
    sprintf(buf, "e%d_%s", eqnb, txt);
    global_ws_scl->set_obj(buf, &scl);
}

/**
 *  Creates the scalars containing the results of an estimated equation:
 *  
 *      - e<eqnb>_n      : number of observations in estimation Sample 
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
 *  @param [in] Equation*     eq  equation 
 *  @param [in] int     j   position of the equation in the block 
 *  @param [in] int     n   number of observations
 *  @param [in] int     k   number of degrees of freedom
 */
void Estimation::E_tests2scl(Equation* eq, int j, int n, int k)
{
    if(!eq) 
        return;

    E_savescl((double) n, j, "n");
    E_savescl((double) k, j, "k");
    E_savescl(eq->tests[EQ_STDEV], j,   "stdev");
    E_savescl(eq->tests[EQ_MEANY], j,   "meany");
    E_savescl(eq->tests[EQ_SSRES], j,   "ssres");
    E_savescl(eq->tests[EQ_STDERR], j,  "stderr");
    E_savescl(eq->tests[EQ_STDERRP], j, "stderrp");
    E_savescl(eq->tests[EQ_FSTAT], j,   "fstat");
    E_savescl(eq->tests[EQ_R2], j,      "r2");
    E_savescl(eq->tests[EQ_R2ADJ], j,   "r2adj");
    E_savescl(eq->tests[EQ_DW], j,      "dw");
    E_savescl(eq->tests[EQ_LOGLIK], j,  "loglik");
}

/**
 *  Creates or replaces a VAR containing one result of a block estimation. 
 *  
 *  The created (or replaced) VAR name is name<eqnb>, for example: 
 *  
 *      E_savevar("_YCALC", 2, E_RHS)
 *  
 *  creates _YCALC2 containing the RHS of equation number 2 (i.e. the third one) in the estimated block.
 *    
 *  That method allows the use of estimation results in reports.
 *   
 *  @param [in] char* name  Prefix of the variable to create
 *  @param [in] int   eqnb  Suffix of the variable
 *  @param [in] MAT*  mat   content to store in the variable
 */
 void Estimation::E_savevar(char* name, int eqnb, MAT* mat) 
{
    // varname = name{eqnb}. Ex: _YRES0, _YCALC1...
    char varname[80];
    sprintf(varname, "%s%d", name, eqnb);
    
    // create varname with NaN 
    KV_add(global_ws_var.get(), varname);
    double* var = KVVAL(global_ws_var.get(), varname);
    if(var == NULL) 
        return;
    
    // copy mat to name from E_FROM to E_FROM + E_T
    for(int t = E_FROM; t < E_FROM + E_T; t++)
        var[t] =  MATE(mat, eqnb, t - E_FROM);
}


/**
 *  Saves an equation after the estimation. Saves only LEC form, method, estimation sample and tests.
 *  
 *  @param [in] char*   name    endogenous name
 *  @param [in] char*   lec     equation
 *  @param [in] int     method  estimation method
 *  @param [in] Sample* smpl    estimation sample or NULL. If NULL, the estimation 
 *                              sample is read from the equation itself
 *  @param [in] float*  tests   list of tests (see KE_est_s() for the complete list)
 *  @return     int             0 on success, -1 on error
 */
int Estimation::KE_update(char* name, char* c_lec, int i_method, Sample* smpl, float* tests)
{
    std::string endo(name);
    std::string lec(c_lec);
    IodeEquationMethod method = EQ_LSQ;
    if (i_method >= 0 && i_method < IODE_NB_EQ_METHODS)
        method = (IodeEquationMethod) i_method;

    Equation* eq;
    if(!E_DBE->contains(name)) 
    {
        std::string comment = "";
        std::string instruments = "";
        std::string block = endo;
        Period from_period = (smpl !=  NULL) ? smpl->start_period : Period();
        Period to_period = (smpl !=  NULL) ? smpl->end_period : Period();
        eq = new Equation(endo, lec, method, from_period.to_string(), to_period.to_string(), 
                          comment, instruments, block, true);
    }
    else
    {
        eq = KEVAL(E_DBE, name);
        eq->sample = (smpl != nullptr) ? *smpl : Sample();
        eq->set_lec(lec);
        eq->set_method(method);
        eq->update_date();
    }

    memcpy(eq->tests.data(), tests, EQS_NBTESTS * sizeof(float));   

    bool success = E_DBE->set_obj(name, eq);
    delete eq;
    eq = nullptr;
    if(!success) 
    {
        error_manager.append_error(std::string(L_error()));
        return -1;
    }

    return 0;
}

/**
 *  Estimates an equation or a block of equations.
 *  
 *  On success, the following results are produced:
 *      - estimated coefficients + stderr in Scalar under their names (c1, c2 ...)
 *      - equation tests : 
 *          - in the equations themselves (in eq->eq_tests)
 *          - in global scalars in the form e<eqnb>_<testname> (e.g.: e0_dw, e1_r2...)
 *      - vectors of calculated, observed and residual values in _YCALC0, _YOBS0 and _YRES0
 *        
 *  TODO: what if no block is passed in the equation ? 
 *          There should still be a creation of scalars and vars (_YRES...).
 *  
 *  @return     int                  0 on success, -1 on error
 */
int Estimation::KE_est_s(Sample* smpl)
{
    static char met[6] = {"LZIGM"};
    int        nb, error = 0, nbl = 0, nbe = 0, nblk;
    Sample*    eq_smpl;
    U_ch**     endos = 0;
    U_ch**     blk = 0;
    U_ch**     lecs = 0;
    U_ch**     instrs = 0;
    float      tests[EQS_NBTESTS];
    Equation*  eq;
    
    nb = SCR_tbl_size((unsigned char**) est_endos);
    
    std::string endo;
    std::set<std::string> estimated_eqs;
    for(int i = 0; i < nb; i++) 
    {    
        // check if the equation has already been estimated (through a block)
        endo = std::string(est_endos[i]);
        if(estimated_eqs.contains(endo))
            continue;

        if(!E_DBE->contains(endo)) 
        {
            std::string error_msg  = "Equation '" + endo + "' not found";
            error_manager.append_error(error_msg);
            goto err;
        }

        if(est_method < 0) 
            E_MET = KEMETH(E_DBE, endo);
        else 
            E_MET = est_method;

        if(!smpl) 
        {
            eq_smpl = new Sample(KESMPL(E_DBE, endo));
            E_SMPL = eq_smpl;
        }
        else
            E_SMPL = smpl;
        if(E_SMPL->nb_periods < 1) 
        {
            std::string error_msg = "Equation '" + endo + "': estimation sample undefined";
            error_manager.append_error(error_msg);
            goto err;
        }
        E_T = E_SMPL->nb_periods;

        std::string _instruments = KEINSTR(E_DBE, endo);
        if(_instruments.empty()) 
            instrs = NULL;
        else 
            instrs = SCR_vtoms((unsigned char*) _instruments.c_str(), (unsigned char*) ",;");

        std::string _block = KEBLK(E_DBE, endo);
        blk =  SCR_vtoms((unsigned char*) _block.c_str(), (unsigned char*) " ,;");
        nblk = SCR_tbl_size(blk);

        std::string _lec;
        std::string eq_name;
        if(nblk == 0)  
        {
            _lec = KELEC(E_DBE, endo);
            SCR_add_ptr(&lecs, &nbl, (unsigned char*) _lec.c_str());
            SCR_add_ptr(&endos, &nbe, (unsigned char*) endo.c_str());
        }
        else 
        {
            /* add elements of block */
            for(int j = 0; j < nblk; j++) 
            {
                SCR_sqz(blk[j]);
                eq_name = std::string((char*) blk[j]);
                if(!global_ws_eqs->contains(eq_name)) 
                {
                    std::string error_msg = "Equation '" + eq_name + "' "; 
                    error_msg += "not found in block of equation";
                    error_manager.append_error(error_msg);
                    SCR_free_tbl(lecs);
                    SCR_free_tbl(endos);
                    SCR_free_tbl(instrs);
                    SCR_free_tbl(blk);
                    goto err;
                }

                _lec = KELEC(global_ws_eqs.get(), eq_name);
                SCR_add_ptr(&lecs, &nbl, (unsigned char*) _lec.c_str());
                SCR_add_ptr(&endos, &nbe, (unsigned char*) eq_name.c_str());
            }
        }

        SCR_add_ptr(&lecs, &nbl, 0L);
        SCR_add_ptr(&endos, &nbe, 0L);

        error = E_est((char**) endos, (char**) lecs, (char**) instrs);

        if(error == 0) 
        {
            E_print_results(1, 1, 1, 1, 1);

            for(int j = 0; j < nbe - 1; j++) 
            {
                tests[EQ_CORR]    = (float)MATE(E_MCORRU,      0, j);
                tests[EQ_STDEV]   = (float)MATE(E_STDEV,       0, j);
                tests[EQ_MEANY]   = (float)MATE(E_MEAN_Y,      0, j);
                tests[EQ_SSRES]   = (float)MATE(E_SSRES,       0, j);
                tests[EQ_STDERR]  = (float)MATE(E_STDERR,      0, j);
                tests[EQ_STDERRP] = (float)MATE(E_STD_PCT,     0, j);
                tests[EQ_FSTAT]   = (float)MATE(E_FSTAT,       0, j);
                tests[EQ_R2]      = (float)MATE(E_RSQUARE,     0, j);
                tests[EQ_R2ADJ]   = (float)MATE(E_RSQUARE_ADJ, 0, j);
                tests[EQ_DW]      = (float)MATE(E_DW,          0, j);
                tests[EQ_LOGLIK]  = (float)MATE(E_LOGLIK,      0, j);

                eq_name = std::string((char*) endos[j]);
                KE_update((char*) eq_name.c_str(), (char*) lecs[j], E_MET, E_SMPL, tests);
                // create the Scalars containing the results of an estimated equation
                eq = KEVAL(E_DBE, eq_name);
                E_tests2scl(eq, j, E_T, E_NCE);
                if(eq) delete eq;
                eq = nullptr;
                // create the Variables containing the fitted, observed and residual values
                E_savevar("_YCALC", j, E_RHS);   /* JMP 27-09-96 */
                E_savevar("_YOBS", j, E_LHS);    /* JMP 27-09-96 */
                E_savevar("_YRES", j, E_U);      /* JMP 27-09-96 */

                estimated_eqs.insert(std::string((char*) endos[j]));    // mark the equation as estimated
            }
        }

        // endos, lecs and instruments freed in E_est
        SCR_free_tbl(blk);
        lecs = endos = instrs = blk = NULL;
        nbl = nbe = 0;
        estimated_eqs.clear();

        if(error != 0) 
            return(-1);
    }

    return(0);

err :
    SCR_free_tbl(blk);
    lecs = endos = instrs = blk = NULL;
    nbl = nbe = 0;
    estimated_eqs.clear();
    return(-1);
}

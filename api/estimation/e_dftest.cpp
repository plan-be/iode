/**
 *  @header4iode
 * 
 *  Implementation of the Dickey-Fuller test.
 *  
 *  TODO: 
 *      1. Use API "K_" functions instead of B_EqsEstimateEqs()
 *      2. Implement tests
 *  
 *  List of functions 
 *  -----------------
 *      double *E_UnitRoot(char* varname, int drift, int trend, int order)      : implementation of the Dickey-Fuller test
 *      void E_PrintDF(char* lec, double* res, int drift, int trend, int order) : prints the results of the Dickey-Fuller test
 *  
 *  Utilities
 *  ---------
 *      int E_GetLecName(char* lec, char* name)      : retrieves the name of the first variable in a lec expression
 *      void E_SclToReal(char* name, double* res) : Stores the content of a scalar in a vector of doubles
 */
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/write/write.h"
#include "api/estimation/estimation.h"


/**
 *  Returns the first (partial) Sample on which a specific variable is 
 *  defined (i.e.: is not NaN).
 *  
 *  @param [in, out]  Sample*  smpl     First sample on which the variable is not IODE_NAN
 *  @param [in]       char*    name     variable to be analysed (from global_ws_var)
 *  @return           int               0 if name is found in global_ws_var, -1 otherwise
 */
static int E_GetSmpl(Sample* smpl, char* c_name)
{
    std::string name = std::string(c_name);
    if(!global_ws_var->contains(name)) 
        return -1;
    
    double* val = KVVAL(global_ws_var.get(), name, 0);

    int t;
    Sample* wsmpl = global_ws_var->sample;
    for(t = 0 ; t < wsmpl->nb_periods ; t++)
        if(IODE_IS_A_NUMBER(val[t])) break;

    smpl->start_period = wsmpl->start_period.shift(t);

    for(; t < wsmpl->nb_periods ; t++)
        if(!IODE_IS_A_NUMBER(val[t])) break;

    smpl->end_period = wsmpl->start_period.shift(t);

    smpl->nb_periods = smpl->end_period.difference(smpl->start_period);
    
    return 0;
}


/**
 *  Retrieves the name of the first variable in a lec expression, "_DF" excluded.
 *  
 *  @param [in] char*   lec   LEC expression
 *  @param [in] char*   name  buffer where to put the result
 *  @return     int           0 on success, -1 if the lec expression cannot be compiled       
 */
int E_GetLecName(char* lec, char* name)
{
    int     j;
    CLEC    *clec = 0;

    name[0] = 0;
    clec = L_cc(lec);
    if(clec == 0) 
        return -1;
    
    std::string obj_name;
    for(j = 0 ; j < clec->nb_names ; j++) 
    {
        obj_name = std::string(clec->lnames[j].name);
        
        // skip "_DF" 
        if(obj_name == "_DF")
            continue;
        
        // if variable -> set name and exit loop
        if(!is_coefficient(obj_name))
        {
            strcpy(name, obj_name.c_str());
            break;
        } 
    }

    SW_nfree(clec);
    return 0;
}


/**
 *  Sub-function of E_UnitRoot(). Creates and estimates the D-F equation.
 *  
 *  @param [in] Sample*     smpl    estimation sample
 *  @param [in] char*       buf     LEC expression of the equation _DF.
 *  @return     int                 result the estimation: 0 on success, -1 on error.
 */
static int E_UnitRoot_1(Sample* smpl, char* buf)
{
    int     rc = -1, neqs = 0;
    char    **eqs = NULL;

    SCR_add_ptr((unsigned char***) &eqs, &neqs, (unsigned char*) "_DF");
    SCR_add_ptr((unsigned char***) &eqs, &neqs, NULL);

    rc = K_upd_eqs("_DF", buf, 0L, 0, 0L, 0L, 0L, 0L, 0);
    if(rc) 
        return(rc);
    
    Estimation est(eqs, global_ws_eqs.get(), global_ws_var.get(), global_ws_scl.get(), smpl);
    rc = est.estimate();

    global_ws_eqs->remove("_DF");
    SCR_free_tbl((unsigned char**) eqs);
    return(rc);
}


/**
 *  Implementation of the Dickey-Fuller test. DF-test tests 
 *  the null hypothesis that a unit root is present in an auto-regressive 
 *  time series model. 
 *  
 *  See https://en.wikipedia.org/wiki/Dickey%E2%80%93Fuller_test for more details on the DF-tests.
 *  See https://iode.plan.be/doku.php?id=statunitroot for the IODE implementation.
 *    
 *  @param [in] char*       lec     LEC expression to test
 *  @param [in] int         drift   0 or 1 indicating whether the formula to be estimated must incorporate a constant term (1) or not (0) 
 *  @param [in] int         trend   0 or 1 depending on whether the formula to be estimated should incorporate a trend term (1) or not (0) 
 *  @param [in] int         order   the order of the polynomial to be estimated 
 *  @return     double   allocated vector of the estimated scalars, 3 values for each estimated coefficient: 
 *                              (value, stderr, t-test)
 *                          NULL on error (illegal lec, sample too short)
 */
 
double *E_UnitRoot(char* lec, int drift, int trend, int order)
{
    int      i, pos, rc = -1;
    char     buf[1024], scl[11], varname[64];
    Sample   smpl;
    double   *res = NULL, *vec;

    // Computes the lec formula and stores the result in the VAR _DF
    vec = L_cc_link_exec(lec, global_ws_var.get(), global_ws_scl.get());
    if(vec == NULL) 
        return(NULL);
    strcpy(varname, "_DF");
    Sample* var_sample = global_ws_var->sample;
    if(!var_sample) 
    {
        kwarning("No sample defined for the Variables workspace");
        return NULL;
    }
    int nb_periods = var_sample->nb_periods;
    global_ws_var->set(varname, vec, nb_periods);
    SW_nfree(vec);
    
    // Checks that the sample is large enough for the estimation 
    E_GetSmpl(&smpl, varname);
    smpl.start_period = smpl.start_period.shift(1);
    smpl.nb_periods--;
    if(smpl.nb_periods < (drift + trend + order + 1) * 2) 
    {
        error_manager.append_error("Sample too small for this test");
        goto cleanup;
    }

    /* Dickey Fuller */
    // Construction de l'équation à estimer, partie par partie selon les parms
    sprintf(buf, "d(%s) := df_ * %s[-1]", varname, varname);
    global_ws_scl->set("df_", NULL);
    
    if(drift) 
    {
        sprintf(buf + strlen(buf), "+ df_d");
        global_ws_scl->set("df_d", NULL);
    }

    if(trend) 
    {
        sprintf(buf + strlen(buf), "+ df_t*t");
        global_ws_scl->set("df_t", NULL);
    }

    for(i = 1 ; i <= order ; i++) 
    {
        sprintf(scl, "df%d", i);
        global_ws_scl->set(scl, NULL);
    }

    if(order) 
    {
        for(i = 1;  i <= order ; i++)
            sprintf(buf + strlen(buf), " + df%d*d(%s[-%d])", i, varname, i);
    }

    smpl.start_period = smpl.start_period.shift(order);
    smpl.nb_periods -= order;
    rc = E_UnitRoot_1(&smpl, buf);

    if(rc == 0)
        res = (double *) SCR_malloc((drift + trend + order + 1)* 3 * sizeof(double));
    else
        res = NULL;

    pos = 0;
    if(res) E_SclToReal("df_", res + pos);
    pos += 3;
    global_ws_scl->remove("df_");

    if(drift) 
    {
        if(res) E_SclToReal("df_d", res + pos);
        pos += 3;
        global_ws_scl->remove("df_d");
    }

    if(trend) 
    {
        if(res) E_SclToReal("df_t", res + pos);
        pos += 3;
        global_ws_scl->remove("df_t");
    }

    for(i = 1 ; i <= order ; i++) 
    {
        sprintf(buf, "df%d", i);
        if(res) E_SclToReal(buf, res + pos);
        pos += 3;
        global_ws_scl->remove(buf);
    }

cleanup:
    // Deletes the tmp var _DF
    global_ws_var->remove("_DF");
    return(res);
}



/**
 *  Stores the content of a scalar in a vector of doubles : value, stderr and t-test.
 *  
 *  @param [in] char*       name    name of the scalar 
 *  @param [in] double*  res     pointer to the result 
 */
void E_SclToReal(char* name, double* res)
{
    if(!global_ws_scl->contains(name))
        return;

    Scalar* scl = KSVAL(global_ws_scl.get(), name);
    res[0] = scl->value;
    res[1] = scl->std;
    if(!IODE_IS_0(scl->std)) 
        res[2] = scl->value/scl->std;
}


/**
 *  Prints the results of the Dickey-Fuller test.
 *  
 *  @param [in] char*       lec     LEC expression 
 *  @param [in] double*  res     result (coefs) of the DF test
 *  @param [in] int         drift   parameter of E_UnitRoot()
 *  @param [in] int         trend   parameter of E_UnitRoot()
 *  @param [in] int         order   parameter of E_UnitRoot()
 */
void E_PrintDF(char* lec, double* res, int drift, int trend, int order)
{
    int     i, pos = 0;

    W_print_tit(2);
    W_printf((char*) "Augmented Dickey Fuller Test\n");
    W_print_enum(1);
    W_printf((char*) "ADF(%s, drift = %d, trend = %d, order = %d) = %lf\n",
             lec, drift, trend, order, res[2]);

    W_print_tb("Coefficients and tests", 4);
    W_printfRepl((char*) "&1C &1CValue&1CStandard Error&1CT-Statistic\n");
    W_printf((char*) ".tl\n");

    W_printfRepl((char*) "&1L%s&1D%lf&1D%lf&1D%lf\n",
             "ADF", res[pos], res[pos + 1], res[pos + 2]);
    pos += 3;

    if(drift) {
        W_printfRepl((char*) "&1L%s&1D%lf&1D%lf&1D%lf\n",
                 "Drift", res[pos], res[pos + 1], res[pos + 2]);
        pos += 3;
    }

    if(trend) {
        W_printfRepl((char*) "&1L%s&1D%lf&1D%lf&1D%lf\n",
                 "Trend", res[pos], res[pos + 1], res[pos + 2]);
        pos += 3;
    }

    for(i = 1 ; i <= order ; i++) {
        W_printfRepl((char*) "&1LOrder %d&1D%lf&1D%lf&1D%lf\n",
                 i, res[pos], res[pos + 1], res[pos + 2]);
        pos += 3;
    }
    W_printf((char*) ".te\n");
}

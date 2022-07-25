/**
 *  @header4iode
 * 
 *  Implementation of the Dickey-Fuller test.
 *  
 *  TODO: 
 *      1. Use API "K_" functions instead of B_DataUpdateEqs() and B_EqsEstimateEqs()
 *      2. Implement tests
 *  
 *  List of functions 
 *  -----------------
 *      IODE_REAL *E_UnitRoot(char* varname, int drift, int trend, int order)      : implementation of the Dickey-Fuller test
 *      void E_PrintDF(char* lec, IODE_REAL* res, int drift, int trend, int order) : prints the results of the Dickey-Fuller test
 *  
 *  Utilities
 *  ---------
 *      int E_GetLecName(char* lec, char* name)      : retrieves the name of the first variable in a lec expression
 *      void E_SclToReal(char* name, IODE_REAL* res) : Stores the content of a scalar in a vector of doubles
 */

#include "iode.h"


/**
 *  Returns the first (partial) SAMPLE on which a specific variable is 
 *  defined (i.e.: is not NaN).
 *  
 *  @param [in, out]  SAMPLE*  smpl     First sample on which the variable is not L_NAN
 *  @param [in]       char*    name     variable to be analysed (from KV_WS)
 *  @return           int               0 if name is found in KV_WS, -1 otherwise
 */
 
static int E_GetSmpl(SAMPLE *smpl, char *name)
{
    int     pos, t;
    IODE_REAL    *val;
    SAMPLE  *wsmpl = KSMPL(K_WS[K_VAR]);

    pos = K_find(K_WS[K_VAR], name);
    if(pos < 0) return(-1);
    val = KVVAL(K_WS[K_VAR], pos, 0);

    for(t = 0 ; t < wsmpl->s_nb ; t++)
        if(L_ISAN(val[t])) break;

    memcpy(&(smpl->s_p1), PER_addper(&(wsmpl->s_p1), t), sizeof(PERIOD));

    for(; t < wsmpl->s_nb ; t++)
        if(!L_ISAN(val[t])) break;

    memcpy(&(smpl->s_p2), PER_addper(&(wsmpl->s_p1), t), sizeof(PERIOD));

    smpl->s_nb = PER_diff_per(&(smpl->s_p2), &(smpl->s_p1));
    
    return(0);
}


/**
 *  Retrieves the name of the first variable in a lec expression, "_DF" excluded.
 *  
 *  @param [in] char*   lec  LEC expression
 *  @param [in] char*   name buffer where to put the result
 *  @return     int          0 on success, -1 if the lec expression cannot be compiled       
 */
int E_GetLecName(char* lec, char* name)
{
    int     j;
    CLEC    *clec = 0;

    name[0] = 0;
    clec = L_cc(lec);
    if(clec == 0) return(-1);
    for(j = 0 ; j < clec->nb_names ; j++) {
        strcpy(name, clec->lnames[j].name);
        if(strcmp(name, "_DF") && !L_ISCOEF(name)) break;
    }

    SW_nfree(clec);
    return(0);
}


/**
 *  Sub-function of E_UnitRoot(). Creates and estimates the D-F equation.
 *  
 *  @param [in] SAMPLE*     smpl    estimation sample
 *  @param [in] char*       buf     LEC expression of the equation _DF.
 *  @return     int                 result the estimation: 0 on success, -1 on error.
 */
static int E_UnitRoot_1(SAMPLE* smpl, char* buf)
{
    int     rc = -1, neqs = 0;
    char    **eqs = NULL;

    SCR_add_ptr(&eqs, &neqs, "_DF");
    SCR_add_ptr(&eqs, &neqs, NULL);

    rc = B_DataUpdateEqs("_DF", buf, 0L, 0, 0L, 0L, 0L, 0L, 0);
    if(rc) return(rc);
    rc = B_EqsEstimateEqs(smpl, eqs);

    //B_DataDelete("_DF", K_EQS);
    K_del_by_name(KE_WS, "_DF");
    SCR_free_tbl(eqs);
    return(rc);
}


/**
 *  Implementation of the Dickey-Fuller test. DF-test tests 
 *  the null hypothesis that a unit root is present in an autoregressive 
 *  time series model. 
 *  
 *  See https://en.wikipedia.org/wiki/Dickey%E2%80%93Fuller_test for more details on the DF-tests.
 *  See https://iode.plan.be/doku.php?id=statunitroot for the IODE implementation.
 *    
 *  @param [in] char*       varname name of the variable to test
 *  @param [in] int         drift   0 or 1 indicating whether the formula to be estimated must incorporate a constant term (1) or not (0) 
 *  @param [in] int         trend   0 or 1 depending on whether the formula to be estimated should incorporate a trend term (1) or not (0) 
 *  @param [in] int         order   the order of the polynomial to be estimated 
 *  @return     IODE_REAL   allocated vector of the estimated scalars, 3 values for each estimated coefficient: 
 *                              (value, stderr, t-test)
 */
 
IODE_REAL *E_UnitRoot(char* varname, int drift, int trend, int order)
{
    int     i, pos, rc = -1;
    char    buf[1024], scl[11];
    SAMPLE  smpl;
    IODE_REAL    *res = NULL;


    E_GetSmpl(&smpl, varname);
    memcpy(&(smpl.s_p1), PER_addper(&(smpl.s_p1), 1), sizeof(PERIOD));
    smpl.s_nb--;
    if(smpl.s_nb < (drift + trend + order + 1) * 2) {
        B_seterror("Sample to small for this test");
        return(res);
    }

    /* Dickey Fuller */
    // Construction de l'équation à estimer, partie par partie selon les parms
    sprintf(buf, "d(%s) := df_ * %s[-1]", varname, varname);
    //B_DataCreate("df_", K_SCL);
    K_add(KS_WS, "df_", NULL);
    
    if(drift) {
        sprintf(buf + strlen(buf), "+ df_d");
        //B_DataCreate("df_d", K_SCL);
        K_add(KS_WS, "df_d", NULL);
    }

    if(trend) {
        sprintf(buf + strlen(buf), "+ df_t*t");
        //B_DataCreate("df_t", K_SCL);
        K_add(KS_WS, "df_t", NULL);
    }

    for(i = 1 ; i <= order ; i++) {
        sprintf(scl, "df%d", i);
        //B_DataCreate(scl, K_SCL);
        K_add(KS_WS, scl, NULL);
    }

    if(order) {
        for(i = 1;  i <= order ; i++) {
            sprintf(buf + strlen(buf), " + df%d*d(%s[-%d])", i, varname, i);
        }
    }

    memcpy(&(smpl.s_p1), PER_addper(&(smpl.s_p1), order), sizeof(PERIOD));
    smpl.s_nb -= order;
    rc = E_UnitRoot_1(&smpl, buf);


    if(rc == 0)
        res = (IODE_REAL *) SCR_malloc((drift + trend + order + 1)* 3 * sizeof(IODE_REAL));
    else
        res = NULL;

    pos = 0;
    if(res) E_SclToReal("df_", res + pos);
    pos += 3;
    //B_DataDelete("df_", K_SCL);
    K_del_by_name(KS_WS, "df_");

    if(drift) {
        if(res) E_SclToReal("df_d", res + pos);
        pos += 3;
        //B_DataDelete("df_d", K_SCL);
        K_del_by_name(KS_WS, "df_d");
    }

    if(trend) {
        if(res) E_SclToReal("df_t", res + pos);
        pos += 3;
        //B_DataDelete("df_t", K_SCL);
        K_del_by_name(KS_WS, "df_t");
    }

    for(i = 1 ; i <= order ; i++) {
        sprintf(buf, "df%d", i);
        if(res) E_SclToReal(buf, res + pos);
        pos += 3;
        //B_DataDelete(buf, K_SCL);
        K_del_by_name(KS_WS, buf);
    }

    return(res);
}



/**
 *  Stores the content of a scalar in a vector of doubles : value, stderr and t-test.
 *  
 *  @param [in] char*       name    name of the scalar 
 *  @param [in] IODE_REAL*  res     pointer to the result 
 */
void E_SclToReal(char* name, IODE_REAL* res)
{
    int pos;
    SCL *scl;

    pos = K_find(KS_WS, name);
    if(pos < 0) return;

    scl = KSVAL(KS_WS, pos);
    res[0] = scl->val;
    res[1] = scl->std;
    if(!L_IS0(scl->std)) res[2] = scl->val/scl->std;
}


/**
 *  Prints the results of the Dickey-Fuller test.
 *  
 *  @param [in] char*       lec     LEC expression 
 *  @param [in] IODE_REAL*  res     result (coefs) of the DF test
 *  @param [in] int         drift   parameter of E_UnitRoot()
 *  @param [in] int         trend   parameter of E_UnitRoot()
 *  @param [in] int         order   parameter of E_UnitRoot()
 */
void E_PrintDF(char* lec, IODE_REAL* res, int drift, int trend, int order)
{
    int     i, pos = 0;

    W_print_tit(2);
    W_printf("Augmented Dickey Fuller Test\n");
    W_print_enum(1);
    W_printf("ADF(%s, drift = %d, trend = %d, order = %d) = %lf\n",
             lec, drift, trend, order, res[2]);

    W_print_tb("Coefficients and tests", 4);
    W_printf("&1C &1CValue&1CStandard Error&1CT-Statistic\n");
    W_printf(".tl\n");

    W_printf("&1L%s&1D%lf&1D%lf&1D%lf\n",
             "ADF", res[pos], res[pos + 1], res[pos + 2]);
    pos += 3;

    if(drift) {
        W_printf("&1L%s&1D%lf&1D%lf&1D%lf\n",
                 "Drift", res[pos], res[pos + 1], res[pos + 2]);
        pos += 3;
    }

    if(trend) {
        W_printf("&1L%s&1D%lf&1D%lf&1D%lf\n",
                 "Trend", res[pos], res[pos + 1], res[pos + 2]);
        pos += 3;
    }

    for(i = 1 ; i <= order ; i++) {
        W_printf("&1LOrder %d&1D%lf&1D%lf&1D%lf\n",
                 i, res[pos], res[pos + 1], res[pos + 2]);
        pos += 3;
    }
    W_printf(".te\n");
}


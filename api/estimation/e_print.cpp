/**
 *  @header4iode
 * 
 *  Functions to print the results of an estimation. The generated format is A2M.
 *  
 *  List of functions 
 *  -----------------
 *      int E_graph(char** titles, Sample* smpl, MAT* mlhs, MAT* mrhs, int view, int res)   Displays or prints the graphs of residuals or observed / fitted values 
 *      int E_print_results(int corr, int corru, int obs, int grobs, int grres)             Prints the estimation input and output of a block of equations. 
 */
#include "api/constants.h"
#include "api/k_lang.h"
#include "api/objs/objs.h"
#include "api/objs/scalars.h"
#include "api/print/print.h"
#include "api/write/write.h"
#include "api/estimation/estimation.h"
#include <algorithm>    // for std::min, std::max


// Declarations
static int E_graph_calc_lhs(char* name, char* res, char* rhs);

/**
 *  Prints the estimation parameters of the current estimation.
 */
void Estimation::E_print_parms()
{
    W_print_tb("Parameters", 2);

    std::string from = E_SMPL->start_period.to_string();
    std::string to = E_SMPL->end_period.to_string();

    W_printfRepl("&1NEstimation period        &1L%s:%s\n", (char*) from.c_str(), (char*) to.c_str());
    W_printfRepl("&1NEstimation method        &1L%c\n", "LZIG"[E_MET]);
    W_printfRepl("&1LNumber of observations   &1L%d\n", E_T);
    W_printfRepl("&1LNumber of equations      &1L%d\n", E_NEQ);
    W_printfRepl("&1LNumber of coefficients   &1L%d\n", E_NCE);
    W_printfRepl("&1LNumber of instruments    &1L%d\n", E_NINSTR);
    W_printfRepl("&1LMax number of iterations &1L%d\n", E_MAXIT);
    W_printfRepl("&1LConvergence limit        &1L%lf\n", (double)E_EPS);

    W_printf(".te\n");
}

/**
 *  Prints the LEC forms of the equations in the current estimation.
 */
void Estimation::E_print_eqs()
{
    int     i;

    W_print_tit(2);
    W_printf("Equations\n");
    for(i = 0 ; i < E_NEQ ; i++) {
        W_print_enum(2);
        W_printf("%s\n", E_LECS[i]);
    }
}


/**
 *  Prints the LEC forms of the instruments in the current estimation.
 */
void Estimation::E_print_instrs()
{
    int     i;

    if(E_NINSTR == 0 || E_MET == 0 || E_MET == 1) return;
    W_print_tit(2);
    W_printf("Instruments\n");
    for(i = 0 ; i < E_NINSTR ; i++) {
        W_print_enum(2);
        W_printf("%s\n", E_INSTRS[i]);
    }
}


/**
 *  Prints the estimated coefficients and their statistical tests 
 *  of the current estimation.
 */
void Estimation::E_print_coefs()
{
    Scalar* scl;

    W_print_tb("Coefficients and tests", 5);
    W_printfRepl("&1CName&1CValue&1CStandard Error&1CT-Statistic&1CRelax\n");
    W_printf(".tl\n");

    std::string scl_name;
    for(int i = 0 ; i < E_NC ; i++) 
    {
        scl_name = E_DBS->get_name(E_C_NBS[i]);
        scl = KSVAL(E_DBS, scl_name);
        //   if(scl->relax == 0) continue; /* JMP 12-03-98 */
        W_printfRepl("&1L%s&1D%lf&1D%lf&1D%lf&1D%lf\n",
                 scl_name.c_str(),
                 (double) scl->value,
                 (double) scl->std,
                 (double) E_div_0(scl->value, scl->std),
                 (double) scl->relax);
    }
    W_printf(".te\n");
}


/**
 *  Prints the correlation matrix of coefficients of the current estimation.
 */
void Estimation::E_print_mcorr()
{
    int     i, j, ic, jc;

    W_print_tb("Correlation matrix of coefficients", E_NCE + 1);
    // E_printf(".twidth 2 2\n"); // JMP 06/07/2022 => to keep .twidth empty as long as possible :-)

    W_printfRepl("&1C ");
    for(i = 0 ; i < E_NC ; i++)
        if(MATE(E_SMO, i, 0)) W_printfRepl("&1C%s", E_DBS->get_name(E_C_NBS[i]));

    W_printf("\n.tl\n");
    for(i = 0, ic = 0 ; i < E_NC ; i++) {
        if(MATE(E_SMO, i, 0) == 0) continue;
        W_printfRepl("&1L%s", E_DBS->get_name(E_C_NBS[i]));
        for(j = 0, jc = 0 ; j < E_NC ; j++) {
            if(MATE(E_SMO, j, 0) == 0) continue;
            W_printfRepl("&1D%lf", (double) MATE(E_MCORR, ic, jc));
            jc++;
        }
        W_printf("\n");
        ic++;
    }
    W_printf(".te\n");
}


/**
 *  Prints the correlation matrix of the residuals of the current estimation.
 */
void Estimation::E_print_mcorru()
{
    int     i, j;

    W_print_tb("Correlation of residuals", E_NEQ + 1);

    W_printfRepl("&1C ");
    for(i = 0 ; i < E_NEQ ; i++) W_printfRepl("&1C%s", E_ENDOS[i]);
    W_printfRepl("\n.tl\n");
    for(i = 0 ; i < E_NEQ ; i++) {
        W_printfRepl("&1L%s", E_ENDOS[i]);
        for(j = 0 ; j < E_NEQ ; j++) W_printfRepl("&1D%lf", (double) MATE(E_MCORRU, i, j));
        W_printfRepl("\n");
    }
    W_printfRepl(".te\n");
    /* JMP 13-07-96 */
}


/**
 *  Prints the first part of the estimation results of equation eq_nbi. 
 *  
 *  @param int  eq_nb   equation number
 */
void Estimation::E_print_eqres_1(int eq_nb)
{
    W_print_tb("", 2);

    W_printfRepl("&1C &1CValue\n");
    W_printfRepl(".tl\n");
    W_printfRepl("&1LNumber of coefficients      &1D%d\n", (int)MATE(E_NBCE, 0, eq_nb));
    W_printfRepl("&1LNumber of observations      &1D%d\n", E_T);
    W_printfRepl("&1LStandard deviation on YOBS  &1D%lf\n", (double) MATE(E_STDEV, 0, eq_nb));
    W_printfRepl("&1LMean of YOBS                &1D%lf\n", (double) MATE(E_MEAN_Y, 0, eq_nb));
    W_printfRepl(".tl\n");

    W_printfRepl("&1CTests&1CValue\n");
    W_printfRepl(".tl\n");
    W_printfRepl("&1LSum of square of residuals  &1D%lf\n", (double) MATE(E_SSRES, 0, eq_nb));
    W_printfRepl("&1LStandard error              &1D%lf\n", (double) MATE(E_STDERR, 0, eq_nb));
    W_printfRepl("&1LStandard error in %%        &1D%lf\n", (double) MATE(E_STD_PCT, 0, eq_nb));
    W_printfRepl("&1LF-Stat                      &1D%lf\n", (double) MATE(E_FSTAT, 0, eq_nb));
    W_printfRepl("&1LR2                          &1D%lf\n", (double) MATE(E_RSQUARE, 0, eq_nb));
    W_printfRepl("&1LR2 adjusted                 &1D%lf\n", (double) MATE(E_RSQUARE_ADJ, 0, eq_nb));
    W_printfRepl("&1LDurbin-Watson test          &1D%lf\n", (double) MATE(E_DW, 0, eq_nb));
    W_printfRepl("&1LLog likelihood              &1D%lf\n", (double) MATE(E_LOGLIK, 0, eq_nb));
    W_printfRepl(".tl\n");
    W_printfRepl(".te\n");
    /* JMP 13-07-96 */
}


/**
 *  Prints the second part of the estimation results of equation eq_nb. That part includes
 *  the left and right members and the matrix of residuals.
 *  
 *  @param int  eq_nb   equation number
 */
void Estimation::E_print_eqres_2(int eq_nb)
{
    int     i;
    double  respct;

    W_print_tb("Actual and fitted values", 5);
    W_printfRepl("&1CPeriod&1CObservations&1CFitted Values&1CResiduals&1CResiduals (%%)\n");
    W_printfRepl(".tl\n");
    for(i = 0 ; i < E_T ; i++) {
        Period per = E_SMPL->start_period.shift(i);
        std::string str_period = per.to_string();
        respct = 100 * E_div_0(MATE(E_U, eq_nb, i), MATE(E_LHS, eq_nb, i));
        W_printfRepl("&1L%s&1C%lf&1C%lf&1C%lf&1C%lf\n",
                 (char*) str_period.c_str(),
                 (double) MATE(E_LHS, eq_nb, i),
                 (double) MATE(E_RHS, eq_nb, i),
                 (double) MATE(E_U, eq_nb, i),
                 respct);
    }

    W_printfRepl(".te\n");
    /* JMP 13-07-96 */
}


/**
 *  Prints the estimation results of a block of equations.
 *  
 *  @param int  obs     if no null, includes the left and right members values + the residuals.
 */
void Estimation::E_print_eqres(int obs)
{
    int     i;

    W_print_tit(2);
    W_printf("Results by equation\n");
    for(i = 0 ; i < E_NEQ ; i++) {
        W_print_tit(3);
        W_printf("Equation : %s\n", E_ENDOS[i]);
        W_print_par(3);
        W_printf("%s\n", E_LECS[i]);
        E_print_eqres_1(i);
        if(obs) E_print_eqres_2(i);
        /*        E_print_eqres_3(i); */
    }
}


/**
 *  Extracts the left and right members of the equation named name.
 *  
 *  @param [in]     char*   name    equation name
 *  @param [out]    char*   res     left member (max 70 chars)
 *  @param [out]    char*   rhs     right member (max 70 chars)
 *  @return         int             -1 if no ":=" found in equation, 0 otherwise
 */
static int E_graph_calc_lhs(char* c_name, char* res, char* rhs)
{
    memset(res, 0, 71);
    memset(rhs, 0, 71);

    std::string name = std::string(c_name);
    if(!K_WS[EQUATIONS]->contains(name)) 
        return -1;
    
    std::string lec = KELEC(K_WS[EQUATIONS], name);
    char* c_lec = (char*) lec.c_str();
    int i = L_split_eq(c_lec);
    if(i < 0) 
        return -1;
    i = std::min(i, 70);
    memcpy(res, c_lec, i);

    SCR_strlcpy((unsigned char*) rhs, ((unsigned char*) c_lec) + i + 2, 70);
    SCR_replace((unsigned char*) rhs, (unsigned char*) "\n", (unsigned char*) " ");
    SCR_replace((unsigned char*) res, (unsigned char*) "\n", (unsigned char*) " ");
    return 0;
}

/**
 *  Displays or prints the graphs of residuals or observed / fitted values 
 *  for an equation block estimation.
 *  
 *  @param [in] char**      titles  titles, 1/graph
 *  @param [in] Sample*     smpl    sample of the estimation
 *  @param [in] MAT*        mlhs    array of LHS values
 *  @param [in] MAT*        mrhs    array of RHS values
 *  @param [in] int         view    displays (1) or print (0) the graph
 *  @param [in] int         res     print residuals 
 *  @return     int                 0 always
 */
int Estimation::E_graph(char** titles, Sample* smpl, MAT* mlhs, MAT* mrhs, int view, int res)
{
    char    buf[256], lhs[80], rhs[80];
    int     i, t, nt, ng;
    double    *y;

    ng = M_NL(mlhs);
    nt = M_NC(mlhs);

    y = (double *)SW_nalloc(sizeof(double) * nt);

    for(i = 0; i < ng && res == 0; i ++) {
        if(view) W_InitDisplay();

        T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT, 0, 0, IODE_NAN, IODE_NAN, IODE_NAN, IODE_NAN, 0, A2M_BOXWIDTH, A2M_BACKBRUSH); /* JMP 19-02-98 */
        sprintf(buf, "Equation %s : observed and fitted values", titles[i]);    /* JMP 26-02-98 */
        T_GraphTitle(buf);                             /* JMP 26-02-98 */

        for(t = 0; t < nt; t ++) y[t] = MATE(mlhs, i, t);
        E_graph_calc_lhs(titles[i], lhs, rhs);
        sprintf(buf, "%s : observed", lhs);
        T_GraphLegend(0, 'L', buf, NULL);

        T_GraphTimeData(smpl, y);

        for(t = 0; t < nt; t ++) y[t] = MATE(mrhs, i, t);
        E_graph_calc_lhs(titles[i], lhs, rhs);
        sprintf(buf, "%s : fitted", lhs);
        T_GraphLegend(0, 'L', buf, NULL);
        T_GraphTimeData(smpl, y);

        T_GraphEnd();

        sprintf(buf, "Equation %s : observed and fitted values", titles[i]);    /* JMP 26-02-98 */
        if(view) W_EndDisplay(buf, -ng, -i, -1, -1);
    }

    for(i = 0; i < ng && res == 1; i ++) {
        if(view) W_InitDisplay();

        T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT, 0, 0, IODE_NAN, IODE_NAN, IODE_NAN, IODE_NAN, 0, A2M_BOXWIDTH, A2M_BACKBRUSH);
        sprintf(buf, "Equation %s : residuals", titles[i]);    /* JMP 26-02-98 */
        T_GraphTitle(buf);                                     /* JMP 26-02-98 */

        for(t = 0; t < nt; t ++) y[t] = MATE(mlhs, i, t) - MATE(mrhs, i, t);
        E_graph_calc_lhs(titles[i], lhs, rhs);
        sprintf(buf, "(%s) - (%s) : residuals", lhs, rhs);
        T_GraphLegend(0, 'L', buf, NULL);
        T_GraphTimeData(smpl, y);

        T_GraphEnd();

        sprintf(buf, "Equation %s : residuals", titles[i]);    /* JMP 26-02-98 */
        if(view) W_EndDisplay(buf, -ng, -i, -1, -1);
    }


    SW_nfree(y);
    return(0);
}


/**
 *  Prints the estimation input and output of a block of equations. Some parts 
 *  of the output are optional.
 *  
 *  @param [in] int  corr   if not null, prints the correlation matrix of the coefficients 
 *  @param [in] int  corru  if not null, prints the correlation matrix of the residuals
 *  @param [in] int  obs    if not null, prints the table of obs, calc and residual values
 *  @param [in] int  grobs  if not null, creates the graph of observations/fitted values
 *  @param [in] int  grres  if not null, creates the graph of the residuals
 *  @return     int         0 always
 */
int Estimation::E_print_results(int corr, int corru, int obs, int grobs, int grres) 
{
    W_print_tit(1);
    W_printf("Estimation\n");

    E_print_eqs();
    E_print_parms();
    E_print_instrs();

    if(E_CONV == 0) {
        W_printf(".par1 parb\nTHE PROCESS DOES NOT CONVERGE\n");
        return(0);
    }

    E_print_coefs();
    if(corr) E_print_mcorr();           /* JMP38 27-09-92 */
    if(corru) E_print_mcorru();         /* JMP38 27-09-92 */
    E_print_eqres(obs);
    if(grobs) E_graph(E_ENDOS, E_SMPL, E_LHS, E_RHS, 0, 0);  /* JMP 23-03-98 */
    if(grres) E_graph(E_ENDOS, E_SMPL, E_LHS, E_RHS, 0, 1);  /* JMP 23-03-98 */

    W_flush();
    return(0);
}

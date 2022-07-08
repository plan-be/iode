/**
 *  @header4iode
 * 
 *  Functions to print the results of an estimation. The generated format is A2M.
 *  
 *  List of functions 
 *  -----------------
 *      int E_graph(char** titles, SAMPLE* smpl, MAT* mlhs, MAT* mrhs, int view, int res)   Displays or prints the graphs of residuals or observed / fitted values 
 *      int E_print_results(int corr, int corru, int obs, int grobs, int grres)             Prints the estimation input and output of a block of equations. 
 */

#include "iode.h"

#define E_printf   W_printf
#define E_BeginTbl W_print_tb

// Declarations
static void E_print_parms();
static void E_print_eqs();
static void E_print_instrs();
static void E_print_coefs();
static void E_print_mcorr();
static void E_print_mcorru();
static void E_print_eqres_1(int eq_nb);
static void E_print_eqres_2(int eq_nb);
static void E_print_eqres(int obs);
static int E_graph_calc_lhs(char* name, char* res, char* rhs);
int E_graph(char** titles, SAMPLE* smpl, MAT* mlhs, MAT* mrhs, int view, int res);
int E_print_results(int corr, int corru, int obs, int grobs, int grres);

 /**
 *  Prints the estimation parameters of the current estimation.
 */
static void E_print_parms()
{
    char    from[20], to[20];

    E_BeginTbl("Parameters", 2);

    PER_pertoa(&(E_SMPL->s_p1), from);
    PER_pertoa(&(E_SMPL->s_p2), to);

    E_printf("&1NEstimation period        &1L%s:%s\n", from, to);
    E_printf("&1NEstimation method        &1L%c\n", "LZIG"[E_MET]);
    E_printf("&1LNumber of observations   &1L%d\n", E_T);
    E_printf("&1LNumber of equations      &1L%d\n", E_NEQ);
    E_printf("&1LNumber of coefficients   &1L%d\n", E_NCE);
    E_printf("&1LNumber of instruments    &1L%d\n", E_NINSTR);
    E_printf("&1LMax number of iterations &1L%d\n", E_MAXIT);
    E_printf("&1LConvergence limit        &1L%lf\n", (double)E_EPS);

    E_printf(".te\n");
}

/**
 *  Prints the LEC forms of the equations in the current estimation.
 */
static void E_print_eqs()
{
    int     i;

    W_print_tit(2);
    E_printf("Equations\n");
    for(i = 0 ; i < E_NEQ ; i++) {
        W_print_enum(2);
        E_printf("%s\n", E_LECS[i]);
    }
}


/**
 *  Prints the LEC forms of the instruments in the current estimation.
 */
static void E_print_instrs()
{
    int     i;

    if(E_NINSTR == 0 || E_MET == 0 || E_MET == 1) return;
    W_print_tit(2);
    E_printf("Instruments\n");
    for(i = 0 ; i < E_NINSTR ; i++) {
        W_print_enum(2);
        E_printf("%s\n", E_INSTRS[i]);
    }
}


/**
 *  Prints the estimated coefficients and their statistical tests 
 *  of the current estimation.
 */
static void E_print_coefs()
{
    int     i;
    SCL     *scl;

    E_BeginTbl("Coefficients and tests", 5);
    E_printf("&1CName&1CValue&1CStandard Error&1CT-Statistic&1CRelax\n");
    E_printf(".tl\n");
    for(i = 0 ; i < E_NC ; i++) {
        scl = KSVAL(E_DBS, E_C_NBS[i]);
        //   if(scl->relax == 0) continue; /* JMP 12-03-98 */
        E_printf("&1L%s&1D%lf&1D%lf&1D%lf&1D%lf\n",
                 KONAME(E_DBS, E_C_NBS[i]),
                 (double) scl->val,
                 (double) scl->std,
                 (double) E_div_0(scl->val, scl->std),
                 (double) scl->relax);
    }
    E_printf(".te\n");
}


/**
 *  Prints the correlation matrix of coefficients of the current estimation.
 */
static void E_print_mcorr()
{
    int     i, j, ic, jc;

    E_BeginTbl("Correlation matrix of coefficients", E_NCE + 1);
    // E_printf(".twidth 2 2\n"); // JMP 06/07/2022 => to keep .twidth empty as long as possible :-)

    E_printf("&1C ");
    for(i = 0 ; i < E_NC ; i++)
        if(MATE(E_SMO, i, 0)) E_printf("&1C%s", KONAME(E_DBS, E_C_NBS[i]));

    E_printf("\n.tl\n");
    for(i = 0, ic = 0 ; i < E_NC ; i++) {
        if(MATE(E_SMO, i, 0) == 0) continue;
        E_printf("&1L%s", KONAME(E_DBS, E_C_NBS[i]));
        for(j = 0, jc = 0 ; j < E_NC ; j++) {
            if(MATE(E_SMO, j, 0) == 0) continue;
            E_printf("&1D%lf", (double) MATE(E_MCORR, ic, jc));
            jc++;
        }
        E_printf("\n");
        ic++;
    }
    E_printf(".te\n");
}


/**
 *  Prints the correlation matrix of the residuals of the current estimation.
 */
static void E_print_mcorru()
{
    int     i, j;

    E_BeginTbl("Correlation of residuals", E_NEQ + 1);

    E_printf("&1C ");
    for(i = 0 ; i < E_NEQ ; i++) E_printf("&1C%s", E_ENDOS[i]);
    E_printf("\n.tl\n");
    for(i = 0 ; i < E_NEQ ; i++) {
        E_printf("&1L%s", E_ENDOS[i]);
        for(j = 0 ; j < E_NEQ ; j++) E_printf("&1D%lf", (double) MATE(E_MCORRU, i, j));
        E_printf("\n");
    }
    E_printf(".te\n");
    /* JMP 13-07-96 */
}


/**
 *  Prints the first part of the estimation results of equation eq_nbi. 
 *  
 *  @param int  eq_nb   equation number
 */
static void E_print_eqres_1(int eq_nb)
{
    E_BeginTbl("", 2);

    E_printf("&1C &1CValue\n");
    E_printf(".tl\n");
    E_printf("&1LNumber of coefficients      &1D%d\n", (int)MATE(E_NBCE, 0, eq_nb));
    E_printf("&1LNumber of observations      &1D%d\n", E_T);
    E_printf("&1LStandard deviation on YOBS  &1D%lf\n", (double) MATE(E_STDEV, 0, eq_nb));
    E_printf("&1LMean of YOBS                &1D%lf\n", (double) MATE(E_MEAN_Y, 0, eq_nb));
    E_printf(".tl\n");

    E_printf("&1CTests&1CValue\n");
    E_printf(".tl\n");
    E_printf("&1LSum of square of residuals  &1D%lf\n", (double) MATE(E_SSRES, 0, eq_nb));
    E_printf("&1LStandard error              &1D%lf\n", (double) MATE(E_STDERR, 0, eq_nb));
    E_printf("&1LStandard error in %%        &1D%lf\n", (double) MATE(E_STD_PCT, 0, eq_nb));
    E_printf("&1LF-Stat                      &1D%lf\n", (double) MATE(E_FSTAT, 0, eq_nb));
    E_printf("&1LR2                          &1D%lf\n", (double) MATE(E_RSQUARE, 0, eq_nb));
    E_printf("&1LR2 adjusted                 &1D%lf\n", (double) MATE(E_RSQUARE_ADJ, 0, eq_nb));
    E_printf("&1LDurbin-Watson test          &1D%lf\n", (double) MATE(E_DW, 0, eq_nb));
    E_printf("&1LLog likelihood              &1D%lf\n", (double) MATE(E_LOGLIK, 0, eq_nb));
    E_printf(".tl\n");
    E_printf(".te\n");
    /* JMP 13-07-96 */
}


/**
 *  Prints the second part of the estimation results of equation eq_nb. That part includes
 *  the left and right members and the matrix of residuals.
 *  
 *  @param int  eq_nb   equation number
 */
static void E_print_eqres_2(int eq_nb)
{
    int     i;
    char    c_date[21];
    PERIOD  *per;
    double  respct;

    E_BeginTbl("Actual and fitted values", 5);
    E_printf("&1CPeriod&1CObservations&1CFitted Values&1CResiduals&1CResiduals (%%)\n");
    E_printf(".tl\n");
    for(i = 0 ; i < E_T ; i++) {
        per = PER_addper(&(E_SMPL->s_p1), i);
        PER_pertoa(per, c_date);
        respct = 100 * E_div_0(MATE(E_U, eq_nb, i), MATE(E_LHS, eq_nb, i));
        E_printf("&1L%s&1C%lf&1C%lf&1C%lf&1C%lf\n",
                 c_date,
                 (double) MATE(E_LHS, eq_nb, i),
                 (double) MATE(E_RHS, eq_nb, i),
                 (double) MATE(E_U, eq_nb, i),
                 respct);
    }

    E_printf(".te\n");
    /* JMP 13-07-96 */
}
/*
static void E_print_eqres_3(eq_nb)
int     eq_nb;
{
    double  miny, maxy, minr, maxr, diffr, diffy,
	    meanr = 0.0, std_error, lhsi, rhsi, ui;
    int     i, j, lg = 38, lgr = 18;
    char    line[121], buf[20], c_date[21];
    PERIOD  *per;

    memset(line, 0, sizeof(line));

    miny = maxy = MATE(E_LHS, eq_nb, 0);
    minr = maxr = MATE(E_U, eq_nb, 0);

    for(i = 0 ; i < E_T ; i++) {
	lhsi = MATE(E_LHS, eq_nb, i);
	rhsi = MATE(E_RHS, eq_nb, i);
	ui   = MATE(E_U, eq_nb, i);
	miny = (miny > lhsi) ? lhs[i] : miny;
	miny = (miny > rhsi) ? rhs[i] : miny;
	maxy = (maxy < lhsi) ? lhs[i] : maxy;
	maxy = (maxy < rhsi) ? rhs[i] : maxy;
	minr = (minr > ui)   ? ui : minr;
	maxr = (maxr < ui)   ? ui : maxr;
	meanr += ui;
	}

    meanr /= E_T;
    for(i = 0 ; i < E_T ; i++) {
	ui   = MATE(E_U, eq_nb, i);
	std_error += (ui - meanr) * (ui - meanr);
	}
    std_error = sqrt(std_error / (double)(E_T - 1));
    minr = (minr > (-std_error)) ? (-std_error) : minr;
    maxr = (maxr < std_error) ? std_error : maxr;

    diffy = (double)(lg - 1) / (maxy - miny);
    diffr = (double)(lgr - 1) / (maxr - minr);

    W_print_tit(2);
    E_printf("PLOT OF ACTUAL (o) AND FITTED (x) VALUESn\n");

    for(i = 0 ; i < E_T ; i++) {
	for(j = 0 ; j < 12 + lg + lgr ; j++) line[j] = ' ';
	line[10 + lg + (int)(1.5 + diffr * (meanr - minr))] = '|';
	line[10 + lg + (int)(1.5 + diffr * (- std_error - minr))] = ':';
	line[10 + lg + (int)(1.5 + diffr * (std_error - minr))] = ':';

	lhsi = MATE(E_LHS, eq_nb, i);
	rhsi = MATE(E_RHS, eq_nb, i);
	ui   = MATE(E_U, eq_nb, i);
	line[(int)(0.5 + diffy * (lhsi - miny))] = 'o';
	line[(int)(0.5 + diffy * (rhsi - miny))] = 'x';
	line[10 + lg + (int)(1.5 + diffr * (ui - minr))] = 'o';
	sprintf(buf, "%10.3le", ui);
	for(j = 0 ; j < 10 ; j++) line[lg + j] = buf[j];
	per = PER_addper(&(E_SMPL->s_p1), i);
	PER_pertoa(per, c_date);
	E_printf("%8s:%70s\n", c_date, line);
	}
}
*/


/**
 *  Prints the estimation results of a block of equations.
 *  
 *  @param int  obs     if no null, includes the left and right members values + the residuals.
 */
static void E_print_eqres(int obs)
{
    int     i;

    W_print_tit(2);
    E_printf("Results by equation\n");
    for(i = 0 ; i < E_NEQ ; i++) {
        W_print_tit(3);
        E_printf("Equation : %s\n", E_ENDOS[i]);
        W_print_par(3);
        E_printf("%s\n", E_LECS[i]);
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
 
static int E_graph_calc_lhs(char* name, char* res, char* rhs)
{
    int     i, pos;
    char    *lec;

    memset(res, 0, 71);
    memset(rhs, 0, 71);
    pos = K_find(K_WS[K_EQS], name);
    if(pos < 0) return(-1);
    lec = KELEC(K_WS[K_EQS], pos);
    i = L_split_eq(lec);
    if(i < 0) return(-1);
    i = min(i, 70);
    memcpy(res, lec, i);
    SCR_strlcpy(rhs, lec + i + 2, 70);
    SCR_replace(rhs, "\n", " ");  /* JMP 07-10-98 */
    SCR_replace(res, "\n", " ");  /* JMP 07-10-98 */
    return(0);
}

/**
 *  Displays or prints the graphs of residuals or observed / fitted values 
 *  for an equation block estimation.
 *  
 *  @param [in] char**      titles  titles, 1/graph
 *  @param [in] SAMPLE*     smpl    sample of the estimation
 *  @param [in] MAT*        mlhs    array of LHS values
 *  @param [in] MAT*        mrhs    array of RHS values
 *  @param [in] int         view    displays (1) or print (0) the graph
 *  @param [in] int         res     print residuals 
 *  @return     int                 0 always
 */
int E_graph(char** titles, SAMPLE* smpl, MAT* mlhs, MAT* mrhs, int view, int res)
{
    char    buf[256], lhs[80], rhs[80];
    int     i, t, nt, ng;
    IODE_REAL    *y;
    extern char *KLG_OBS[], *KLG_CALC[], *KLG_RES[];

    ng = M_NL(mlhs);
    nt = M_NC(mlhs);

    y = (IODE_REAL *)SW_nalloc(sizeof(IODE_REAL) * nt);

    for(i = 0; i < ng && res == 0; i ++) {
        if(view) W_InitDisplay();

        T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT, 0, 0, L_NAN, L_NAN, L_NAN, L_NAN, 0, A2M_BOXWIDTH, A2M_BACKBRUSH); /* JMP 19-02-98 */
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

        T_GraphInit(A2M_GWIDTH, A2M_GHEIGHT, 0, 0, L_NAN, L_NAN, L_NAN, L_NAN, 0, A2M_BOXWIDTH, A2M_BACKBRUSH);
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
int E_print_results(int corr, int corru, int obs, int grobs, int grres) 
{
    W_print_tit(1);
    E_printf("Estimation\n");

    E_print_eqs();
    E_print_parms();
    E_print_instrs();

    if(E_CONV == 0) {
        E_printf(".par1 parb\nTHE PROCESS DOES NOT CONVERGE\n");
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

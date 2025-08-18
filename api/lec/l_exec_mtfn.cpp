/**
 * @header4iode
 *
 * Functions to evaluate LEC "time functions" with possibly **more than one expression** 
 * in the function arguments.
 * 
 * Example:
 *
 *     corr([from [,to],] X+Y, Y+Z) 
 * 
 * computes the correlation between the series (X+Y) and (Y+Z) on the time interval [from, to].
 *      
 * The table (*L_TFN_FN)[] (at the end of this file) contains the pointers 
 * to these functions (#defines from L_LAG to L_LASTOBS (see iode.h)).
 *
 * Function signature:
 *      L_REAL <fnname>(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *  
 *  where:
 *      expr   = pointer to the current position in the CLEC expression (ex.: in "X + corr(A, B)", expr points to "A")
 *      nvargs = number of variables arguments (generally not used)
 *      t      = time of calculation
 *      stack  = pointer to the top of the stack of intermediate values / results
 *      nargs  = number of arguments of the calling function that determines the calculation interval.
 *  
 *  
 *  List of functions
 *  -----------------
 *  Note that these functions are all called by L_exec_sub() (@see l_exec.c).
 *  
 *      L_REAL L_corr(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *      L_REAL L_calccovar(unsigned char* expr1, short len1, unsigned char* expr2, short len2, int t, L_REAL* stack, int nargs, int orig)
 *      L_REAL L_covar(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *      L_REAL L_covar0(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *      L_REAL L_var(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *      L_REAL L_stddev(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *      L_REAL L_index(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *      L_REAL L_acf(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *      int    L_calcvals(unsigned char* expr1, short len1, int t, L_REAL* stack, int* vt, L_REAL* vy, int notnul)
 *      L_REAL L_interpol(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *      L_REAL L_app(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *      L_REAL L_dapp(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
 *      L_REAL L_hpall(unsigned char* expr, short len, int t, L_REAL* stack, int nargs, int std)
 *      L_REAL L_hp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_hpstd(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 */
#include <math.h>
#include "api/lec/lec.h"

#ifdef __GNUC__
    #define _isnan isnan
#endif

/* =============================== MTFN ============================== */

/**
 *  Sub function of L_corr() that calculates the correlation between 2 series defined the CLEC sub-expressions 
 *  expr1 and expr2.
 * 
 *  General formula:
 *      corr(from, to, expr1, expr2) = covar(from, to, expr1, expr2) / sqrt(var(from, to, expr1) * var(from, to, expr2))
 *  
 *  @param [in] expr1   unsigned char*  first CLEC sub-expression
 *  @param [in] len1    short           length of expr1    
 *  @param [in] expr2   unsigned char*  second CLEC sub-expression
 *  @param [in] len2    short           length of expr2
 *  @param [in] t       int             time of of the returned value
 *  @param [in] stack   L_REAL*         pointer to the top of the stack of intermediate values / results
 *  @param [in] nargs   int             number of arguments of the calling function that determines the calculation interval.
 *  @return         
 *  
 */
L_REAL L_calccorr(unsigned char* expr1, short len1, unsigned char* expr2, short len2, int t, L_REAL* stack, int nargs) 
{
    L_REAL  sxx = 0.0, syy = 0.0, sxy = 0.0, vx, vy, meanx, meany;
    int     from, to, j;

    meanx = L_mean(expr1, len1, t, stack, nargs - 1);
    if(!IODE_IS_A_NUMBER(meanx)) return(IODE_NAN);
    meany = L_mean(expr2, len2, t, stack, nargs - 1);
    if(!IODE_IS_A_NUMBER(meany)) return(IODE_NAN);

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    for(j = from ; j <= to ; j++) {
        vx = L_exec_sub(expr1, len1, j, stack);
        if(!IODE_IS_A_NUMBER(vx)) return(IODE_NAN);
        vy = L_exec_sub(expr2, len2, j, stack);
        if(!IODE_IS_A_NUMBER(vy)) return(IODE_NAN);

        sxx += (vx - meanx) * (vx - meanx);
        syy += (vy - meany) * (vy - meany);
        sxy += (vx - meanx) * (vy - meany);
    }

    if(sxx * syy <= 0) return(IODE_NAN);
    return(sxy / sqrt(sxx * syy));
}


/**
 *  Calculates the correlation at time t between two series on a given time interval. 
 *  Each series is the result of the evaluation of two LEC sub-expressions:
 *      - the first sub expression starts at the position 0 in expr.
 *      - the second sub expression starts at the position expr + length of the first sub expression.
 *
 *  Syntax: 
 *      corr([from [,to],] X, Y) 
 *      where 
 *          - X and Y are LEC expressions
 *          - from = period of the first data in the series, 0 by default
 *          - to = period of the last data in the series, t by default
 *  
 *  Formula on [0,t]:
 *      corr(X, Y) = covar(X, Y) / sqrt(var(X) * var(Y))
 *  
 *  General formula:
 *      corr(from, to X, Y) = covar(from, to, X, Y) / sqrt(var(from, to, X) * var(from, to, Y))
 * 
 *  @param [in] expr   unsigned char*   CLEC sub-expression
 *  @param [in] nvargs short            number of variables arguments (unused)
 *  @param [in] t      int              time of calculation
 *  @param [in] stack  L_REAL*          pointer to the top of the stack of intermediate values / results
 *  @param [in] nargs  int              number of arguments of the calling function that determines the calculation interval.
 *
 *  @return            L_REAL           computed correlation 
 *  
 */
L_REAL L_corr(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)  /* JMP 17-04-98 */
{
    short   len1, len2;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    return(L_calccorr(expr + sizeof(short), len1,
                      expr + len1 + 2 * sizeof(short), len2,
                      t, stack, nargs));
}

/**
 *  Sub function of L_covar(), L_covar0() and L_var() that calculates the correlation between 2 series 
 *  defined the CLEC sub-expressions expr1 and expr2.
 * 
 *  General formula:
 *      covar([from [,to],] X, Y)  = sum((Xi-Xm)*(Yi-Ym)) /n     
 *      covar0([from [,to],] X, Y) = sum(Xi * Yi) / n  
 *  
 *   where:
 *          Xm is the mean of X on [from, to]
 *          Ym is the mean of Y on [from, to]
 *          n is the number of observations in [from, to]
 *  
 *  @param [in] expr1   unsigned char*  first CLEC sub-expression
 *  @param [in] len1    short           length of expr1    
 *  @param [in] expr2   unsigned char*  second CLEC sub-expression
 *  @param [in] len2    short           length of expr2
 *  @param [in] t       int             time of of the returned value
 *  @param [in] stack   L_REAL*         pointer to the top of the stack of intermediate values / results
 *  @param [in] nargs   int             number of arguments of the calling function that determines the calculation interval.
 *  @return             L_REAL          computed covariance.
 *  
 */

L_REAL L_calccovar(unsigned char* expr1, short len1, unsigned char* expr2, short len2, int t, L_REAL* stack, int nargs, int orig) 
{
    L_REAL  sxx = 0.0, syy = 0.0, sxy = 0.0, vx, vy, meanx, meany;
    int     from, to, j, n;

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    n = 1 + to - from;
    if(n == 0) return(IODE_NAN);

    meanx = L_mean(expr1, len1, t, stack, nargs - 1);
    if(!IODE_IS_A_NUMBER(meanx)) return(IODE_NAN);
    meany = L_mean(expr2, len2, t, stack, nargs - 1);
    if(!IODE_IS_A_NUMBER(meany)) return(IODE_NAN);

    for(j = from ; j <= to ; j++) {
        vx = L_exec_sub(expr1, len1, j, stack);
        if(!IODE_IS_A_NUMBER(vx)) return(IODE_NAN);
        vy = L_exec_sub(expr2, len2, j, stack);
        if(!IODE_IS_A_NUMBER(vy)) return(IODE_NAN);

        if(orig) sxy += vx * vy;
        else     sxy += (vx - meanx) * (vy - meany);
    }

    return(sxy / n);
}


/**
 *  Calculates the covariance at time t between two series on a given time interval. 
 *  Each series is the result of the evaluation of two LEC sub-expressions:
 *      - the first sub expression starts at the position 0 in expr.
 *      - the second sub expression starts at the position expr + length of the first sub expression.
 *  
 *
 *  @return            L_REAL           computed covariance
 *
 *  @see L_corr() for the parameter definition
 *  @see L_calccovar() for the formulas.
 */
L_REAL L_covar(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) 
{
    short   len1, len2;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    return(L_calccovar(expr + sizeof(short), len1,
                       expr + len1 + 2 * sizeof(short), len2,
                       t, stack, nargs, 0));
}


/**
 *  Calculates the covariance at time t between two series on a given time interval.
 *  Same function as L_covar() but with the assumption that the means of each series is null. 
 *   
 *  @see L_covar() for more details
 *  @see L_calccovar() for the formulas.
 *  
 */
L_REAL L_covar0(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) 
{
    short   len1, len2;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    return(L_calccovar(expr + sizeof(short), len1,
                       expr + len1 + 2 * sizeof(short), len2,
                       t, stack, nargs, 1));
}


/**
 *  Calculates the variance at time t of a series on a given time interval.
 *  Same function as L_covar() but with twice the same series.
 *  
 *   General formula:
 *      var([from [,to],] X)  = sum((Xi-Xm)^2)) /n 
 *
 *   where:
 *          Xm is the mean of X on [from, to]
 *          n is the number of observations in [from, to]
 *
 *  
 *  @see L_corr() for the parameter definition
 *  @see L_covar() for more details.
 *  
 */
L_REAL L_var(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
{
    short   len1;

    memcpy(&len1, expr, sizeof(short));
    return(L_calccovar(expr + sizeof(short), len1,
                       expr + sizeof(short), len1,
                       t, stack, nargs + 1, 0));
}


/**
 *  Calculates the standard deviation at time t of a series on a given time interval.
 *  
 *   General formula:
 *      stddev([from [,to],] X)  = sqrt(var(X))
 *
 *  @see L_corr() for the parameter definition
 *  @see L_calccovar() for the formula.
 *  
 */
L_REAL L_stddev(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1;

    v1 = L_var(expr, nvargs, t, stack, nargs);
    if(!IODE_IS_A_NUMBER(v1) || v1 < 0) return(IODE_NAN);
    else return(sqrt(v1));
}


/**
 *  Calculates the position (index) of a value in a series on a given time interval.
 *  The value is the result of a CLEC sub-expression calculation.
 *  The series is the result of a second CLEC sub-expression calculation.
 *
 *  Syntax:
 *      index([from, [to],]X, Y)
 *      
 *  Example: 
 *      Let the series 
 *          X  = {0, 1, 2, 3, 4, 5}
 *      then 
 *          index(2, X) = 2
 *      or, on the full sample:
 *          index(2, X) = {na, na, 2, 2, 2}
 *          index(2, X) = {na, na, 2, 2, 2}
 *
 *  TODO: Clarify on sub intervals (index(2001Y1, 2010Y1, X, Y))
 *  
 *  Each series is the result of the evaluation of a LEC sub-expressions:
 *      - the first sub expression starts at the position 0 in expr.
 *      - the second sub expression starts at the position expr + length of the first sub expression.
 *  
 *  @see L_corr() for the parameter definition
 *
 *  @return            L_REAL           computed covariance
 */  

 L_REAL L_index(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs)
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     from, to, j, n;
    L_REAL  vx, vy;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr1 = ((char*) expr) + sizeof(short);
    expr2 = ((char*) expr) + len1 + 2 * sizeof(short);

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    n = 1 + to - from;
    if(n == 0) return(IODE_NAN);

    vx = L_exec_sub((unsigned char*) expr1, len1, t, stack);
    if(!IODE_IS_A_NUMBER(vx)) return(IODE_NAN);

    for(j = from ; j <= to ; j++) {
        vy = L_exec_sub((unsigned char*) expr2, len2, j, stack);
        if(!IODE_IS_A_NUMBER(vy)) return(IODE_NAN);

        if(fabs(vx - vy) < 1e-30) return((L_REAL)j);
    }

    return(IODE_NAN);
}


 /**
 *  Calculates the autocorrelation factor on a given degree of a given expression.
 *  
 *  Autocorrelation is the correlation of a signal with a delayed copy of itself as a function of delay. 
 *  Informally, it is the similarity between observations as a function of the time lag between them.
 *  (from https://en.wikipedia.org/wiki/Autocorrelationsee).
 *   
 *  Syntax 
 *      acf([from,[to,]] expr1, expr2) = autocorrelation factor of expr2 of degree expr1 on the period [from, to]
 *      
 *  The degree (expr1) may not exceeds the number of observations ([from, to] divided by 4. Otherwise, the function returns IODE_NAN.
 *  
 *  @see L_corr() for the parameter definition
 */
L_REAL L_acf(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) /* JMP 17-04-98 */
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     from, to, j, n, k;
    L_REAL  vx, vy, meanx, sxy = 0.0, sxy0 = 0.0;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr1 = ((char*) expr) + sizeof(short);
    expr2 = ((char*) expr) + len1 + 2 * sizeof(short);

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    n = 1 + to - from;
    if(n == 0) return(IODE_NAN);

    vx = L_exec_sub((unsigned char*) expr1, len1, t, stack);
    if(!IODE_IS_A_NUMBER(vx)) return(IODE_NAN);
    k = (int)vx;
    if(k < 0 || k > n / 4) return(IODE_NAN);

    meanx = L_mean((unsigned char*) expr2, len2, t, stack, nargs - 1);
    if(!IODE_IS_A_NUMBER(meanx)) return(IODE_NAN);

    for(j = from ; j <= to - k ; j++) {
        vx = L_exec_sub((unsigned char*) expr2, len2, j, stack);
        if(!IODE_IS_A_NUMBER(vx)) return(IODE_NAN);
        vy = L_exec_sub((unsigned char*) expr2, len2, j + k, stack);
        if(!IODE_IS_A_NUMBER(vy)) return(IODE_NAN);

        sxy += (vx - meanx) * (vy - meanx);
    }

    for(j = from ; j <= to ; j++) {
        vx = L_exec_sub((unsigned char*) expr2, len2, j, stack);
        if(!IODE_IS_A_NUMBER(vx)) return(IODE_NAN);
        sxy0 += (vx - meanx) * (vx - meanx);
    }

    return(sxy / sxy0);
}


/** 
 *  Returns in vy and vt the closest existing values (not IODE_NAN) defined around expr[t].
 *
 *  TODO: describe this better 
 *  
 *  @param [in]  expr1  unsigned char*   CLEC sub-expression (heterogeous container)
 *  @param [in]  len1   short            length of expr1 in bytes
 *  @param [in]  t      int              time of calculation
 *  @param [in]  stack  L_REAL*          pointer to the top of the stack of intermediate values / results
 *  @param [out] vt     L_REAL*          
 *  @param [out] vy     L_REAL* 
 *  @param [in]  notnul 
 *  @return             L_REAL           computed correlation 
 *  
 */
 
int L_calcvals(unsigned char* expr1, short len1, int t, L_REAL* stack, int* vt, L_REAL* vy, int notnul)
{
    int     nobs;

    /* 1. Calc val after t */
    vy[0] = vy[1] = IODE_NAN;
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;
    for(vt[1] = t + 1 ; vt[1] < nobs ; vt[1]++) {
        vy[1] = L_exec_sub(expr1, len1, vt[1], stack);
        if(IODE_IS_A_NUMBER(vy[1]) && (notnul == 0 || fabs(vy[1]) > 1e-15)) break;
    }

    /* 2. Calc val before t */
    for(vt[0] = t - 1 ; vt[0] >= 0 ; vt[0]--) {
        vy[0] = L_exec_sub(expr1, len1, vt[0], stack);
        if(IODE_IS_A_NUMBER(vy[0]) && (notnul == 0 || fabs(vy[0]) > 1e-15)) break;
    }

    /* if NO value after AND before t, return IODE_NAN */
    if(!IODE_IS_A_NUMBER(vy[0]) && !IODE_IS_A_NUMBER(vy[1])) return(-1);
    if(IODE_IS_A_NUMBER(vy[0]) && IODE_IS_A_NUMBER(vy[1])) return(0);

    /* if no value after, calc before t0 */
    if(!IODE_IS_A_NUMBER(vy[1])) {
        vy[1] = vy[0];
        vt[1] = vt[0];
        vy[0] = IODE_NAN;
        for(vt[0] = vt[1] - 1 ; vt[0] >= 0 ; vt[0]--) {
            vy[0] = L_exec_sub(expr1, len1, vt[0], stack);
            if(IODE_IS_A_NUMBER(vy[0]) && (notnul == 0 || fabs(vy[0]) > 1e-15)) break;
        }
    }

    /* if no value before, calc after t1 */
    else {
        vy[0] = vy[1];
        vt[0] = vt[1];
        vy[1] = IODE_NAN;
        for(vt[1] = vt[0] + 1 ; vt[1] < nobs ; vt[1]++) {
            vy[1] = L_exec_sub(expr1, len1, vt[1], stack);
            if(IODE_IS_A_NUMBER(vy[1])) break;
        }
    }

    return(0);
}

/**
 *  Computes a value for expr[t]. If expr[t] == IODE_NAN, returns the closer possible linear interpolation 
 *  depending on the existing values around expr[t].
 *  
 *  @see L_corr() for the parameter definition
 *  
 *  @return     L_REAL  value of expr[t] or interpolated value
 *  
 */
L_REAL L_interpol(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) /* JMP 17-04-98 */
{
    char    *expr1;
    short   len1;
    int     nobs, vt[2];
    L_REAL  vx, vy[2], itc;

    memcpy(&len1, expr, sizeof(short));
    expr1 = ((char*) expr) + sizeof(short);

    /* 1. Calc val in t */
    vx = L_exec_sub((unsigned char*) expr1, len1, t, stack);
    if(IODE_IS_A_NUMBER(vx)) return(vx);

    /* 2. Calc values around t */
    L_calcvals((unsigned char*) expr1, len1, t, stack, vt, vy, 0);
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;

    /* 3. Calc result */
    if(!IODE_IS_A_NUMBER(vy[0]) && !IODE_IS_A_NUMBER(vy[1])) return(IODE_NAN);
    if(vt[0] < 0) return(vy[1]);
    if(vt[1] >= nobs) return(vy[0]);
    itc = (vy[0] * vt[1] - vy[1] * vt[0]) / (vt[1] - vt[0]);
    return(itc + t * (vy[1] - vy[0]) / (vt[1] - vt[0]));
}

L_REAL L_app(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) /* JMP 17-04-98 */
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     j, nobs, vt[2];
    L_REAL  vx, vy[2], ayt, ay[2], delta;

    memcpy(&len1, expr, sizeof(short));
    expr1 = ((char*) expr) + sizeof(short);
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr2 = ((char*) expr) + len1 + 2 * sizeof(short);

    /* 1. Calc val in t */
    vx = L_exec_sub((unsigned char*) expr1, len1, t, stack);
    if(IODE_IS_A_NUMBER(vx)) return(vx);

    /* 2. Calc values around t */
    L_calcvals((unsigned char*) expr1, len1, t, stack, vt, vy, 1);
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;

    /* if NO value after AND before t, return IODE_NAN */
    if(!IODE_IS_A_NUMBER(vy[0]) && !IODE_IS_A_NUMBER(vy[1])) return(IODE_NAN);

    /* Valeurs apparentées */
    ayt = L_exec_sub((unsigned char*) expr2, len2, t, stack);
    if(!IODE_IS_A_NUMBER(ayt)) return(IODE_NAN);
    ay[0] = ay[1] = IODE_NAN; /* JMP 19-07-07 */
    if(vt[0] >= 0)   ay[0] = L_exec_sub((unsigned char*) expr2, len2, vt[0], stack);
    if(vt[1] < nobs) ay[1] = L_exec_sub((unsigned char*) expr2, len2, vt[1], stack);

    // Deux valeurs trouvées dans la série initiale
    // !! Les deux valeurs doivent exister dans la série apparentée
    if(IODE_IS_A_NUMBER(ay[0]) && IODE_IS_A_NUMBER(ay[1])) {
        if(vt[0] < t && vt[1] > t) {
            /*            return(ayt * (vy[0] / ay[0] + vy[1] / ay[1]) / 2); */
            // delta = (vy[1]/vy[0])/(ay[1]/ay[0]);
            if(fabs(vy[0] * ay[1]) < 1e-15) return(IODE_NAN);
            delta = (vy[1] * ay[0]) / (vy[0] * ay[1]);
            if(delta < 0) return(IODE_NAN);
            delta = pow(delta, 1.0/(vt[1]-vt[0]));
            if(_isnan(delta)) return(IODE_NAN); /* JMP 18-01-02 */
            return(vy[0] * (ayt/ay[0] * pow(delta, t - vt[0])));
        }
        else {
            j = 0;
            if(vt[0] < t) j = 1;
            if(fabs(ay[j]) < 1e-15) return(IODE_NAN); /* JMP 19-07-07 */
            return(ayt * (vy[j] / ay[j]));
        }
    }
    // Seule la valeur en t0 est définie : res(t) <- APP(t) * (ORIG(t0) / APP(t0))
    if(IODE_IS_A_NUMBER(ay[0])) {
        if(fabs(ay[0]) < 1e-15) return(IODE_NAN); /* JMP 19-07-07 */
        return(ayt * (vy[0] / ay[0]));
    }
    // Seule la valeur en t1 est définie : res(t) <- APP(t) * (ORIG(t1) / APP(t1))
    if(IODE_IS_A_NUMBER(ay[1])) {
        if(fabs(ay[1]) < 1e-15) return(IODE_NAN); /* JMP 19-07-07 */
        return(ayt * (vy[1] / ay[1]));
    }
    return(IODE_NAN);
}

L_REAL L_dapp(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs) /* GB 14-11-00 */
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     j, nobs, vt[2];
    L_REAL  vx, vy[2], ayt, ay[2], delta;

    memcpy(&len1, expr, sizeof(short));
    expr1 = ((char*) expr) + sizeof(short);
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr2 = ((char*) expr) + len1 + 2 * sizeof(short);

    /* 1. Calc val in t */
    vx = L_exec_sub((unsigned char*) expr1, len1, t, stack);
    if(IODE_IS_A_NUMBER(vx)) return(vx);

    /* 2. Calc values around t */
    L_calcvals((unsigned char*) expr1, len1, t, stack, vt, vy, 0);
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;

    /* if NO value after AND before t, return IODE_NAN */
    if(!IODE_IS_A_NUMBER(vy[0]) && !IODE_IS_A_NUMBER(vy[1])) return(IODE_NAN);

    /* Valeurs apparentées */
    ayt = L_exec_sub((unsigned char*)  expr2, len2, t, stack);
    if(!IODE_IS_A_NUMBER(ayt)) return(IODE_NAN);
    if(vt[0] >= 0)   ay[0] = L_exec_sub((unsigned char*) expr2, len2, vt[0], stack);
    if(vt[1] < nobs) ay[1] = L_exec_sub((unsigned char*) expr2, len2, vt[1], stack);

    if(IODE_IS_A_NUMBER(ay[0]) && IODE_IS_A_NUMBER(ay[1])) {
        if(vt[0] < t && vt[1] > t) {
            delta = (ay[1] - ay[0] + vy[0] - vy[1]) / (vt[1]-vt[0]);
            return(ayt + vy[0] - ay[0] - (t - vt[0]) * delta);
        }
        else {
            j = 0;
            if(vt[0] < t) j = 1;
            return(ayt + (vy[j] - ay[j]));
        }
    }
    if(IODE_IS_A_NUMBER(ay[0]))                
        return(ayt + (vy[0] - ay[0]));
    if(IODE_IS_A_NUMBER(ay[1]))                
        return(ayt + (vy[1] - ay[1]));
    return(IODE_NAN);
}

L_REAL L_hpall(unsigned char* expr, short len, int t, L_REAL* stack, int nargs, int std)
{
    char    *expr1, *expr2;
    short   len1, len2;
    double    v, *itmp = NULL, *otmp = NULL;
    //int     from, to, j, lg, lambda, nbna, dim;      /* JMP 7-3-2019 */
    int     from, to, j, lg, nbna, dim;         /* JMP 7-3-2019 */
    double   lambda;                         /* JMP 7-3-2019 */

    memcpy(&len1, expr, sizeof(short));
    expr1 = ((char*) expr) + sizeof(short);
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr2 = ((char*) expr) + len1 + 2 * sizeof(short);

    v = L_exec_sub((unsigned char*)  expr1, len1, t, stack);
    if(!IODE_IS_A_NUMBER(v)) return(IODE_NAN);
    lambda = v;

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    if(t < from || t > to) goto err;
    lg = to - from + 1;
    if(lg < 5) goto err;
    itmp = (double *) SCR_malloc(lg * sizeof(double));
    otmp = (double *) SCR_malloc(lg * sizeof(double));

    if(itmp == NULL || otmp == NULL) goto err;
    for(j = from ; j <= to ; j++) {
        itmp[j - from] = L_exec_sub((unsigned char*) expr2, len2, j, stack);
        // if(!IODE_IS_A_NUMBER(itmp[j - from])) goto err;            /* JMP 26-07-11 */
    }

    /*
        for(j = from, nbna = 0 ; j <= to ; j++, nbna++) {
    	if(IODE_IS_A_NUMBER(itmp[j - from])) break;
        }
        if(lg - nbna < 5) goto err;
    */

    HP_test(itmp, otmp, lg, &nbna, &dim);           /* JMP 26-07-11 */
    if(dim < 5) goto err;                           /* JMP 26-07-11 */
    HP_calc(itmp + nbna, otmp + nbna, dim, lambda, std); /* JMP 26-07-11 */ // JMP 12/4/2019

    SCR_free(itmp);
    v = otmp[t - from];
    SCR_free(otmp);
    //DebugActif = 1;
    //Debug("t=%d, lambda=%lf, v=%lf, from=%d, to=%d\n", t, lambda, v, from, to);
    return(v);

err:
    SCR_free(itmp);
    SCR_free(otmp);
    return(IODE_NAN);
}

L_REAL L_hp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    return(L_hpall(expr, len, t, stack, nargs, 0));
}


L_REAL L_hpstd(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    return(L_hpall(expr, len, t, stack, nargs, 1));
}

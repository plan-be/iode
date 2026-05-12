/**
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
 *      double <fnname>(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs)
 *  
 *  where:
 *      expr   = pointer to the current position in the CLEC expression (ex.: in "X + corr(A, B)", expr points to "A")
 *      nvargs = number of variables arguments (generally not used)
 *      t      = time of calculation
 *      stack  = pointer to the top of the stack of intermediate values / results
 *      nargs  = number of arguments of the calling function that determines the calculation interval.
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
 *  @param [in] stack   double*         pointer to the top of the stack of intermediate values / results
 *  @param [in] nargs   int             number of arguments of the calling function that determines the calculation interval.
 *  @return         
 *  
 */
double L_calccorr(unsigned char* expr1, short len1, unsigned char* expr2, short len2, 
    int from, int to, int t, std::deque<double>& stack, int nargs) 
{
    double mean_x = L_mean(expr1, len1, from, to, t, stack, nargs - 1);
    if(!IODE_IS_A_NUMBER(mean_x)) 
        return IODE_NAN;
    
    double mean_y = L_mean(expr2, len2, from, to, t, stack, nargs - 1);
    if(!IODE_IS_A_NUMBER(mean_y)) 
        return IODE_NAN;

    double x, y;
    double sum_xx = 0.0, sum_yy = 0.0, sum_xy = 0.0;
    for(int j = from; j <= to; j++) 
    {
        x = L_exec_sub(expr1, len1, j);
        if(!IODE_IS_A_NUMBER(x)) 
            return IODE_NAN;
        
        y = L_exec_sub(expr2, len2, j);
        if(!IODE_IS_A_NUMBER(y)) 
            return IODE_NAN;

        sum_xx += (x - mean_x) * (x - mean_x);
        sum_yy += (y - mean_y) * (y - mean_y);
        sum_xy += (x - mean_x) * (y - mean_y);
    }

    if(sum_xx * sum_yy <= 0) 
        return IODE_NAN;
    
    return sum_xy / sqrt(sum_xx * sum_yy);
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
 *  @param [in] stack  double*          pointer to the top of the stack of intermediate values / results
 *  @param [in] nargs  int              number of arguments of the calling function that determines the calculation interval.
 *
 *  @return            double           computed correlation 
 *  
 */
double L_corr(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    short len1, len2;
    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));

    unsigned char* expr1 = expr + sizeof(short); 
    unsigned char* expr2 = expr + len1 + 2 * sizeof(short);
    double result = L_calccorr(expr1, len1, expr2, len2, from, to, t, stack, nargs);
    return result;
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
 *  @param [in] stack   double*         pointer to the top of the stack of intermediate values / results
 *  @param [in] nargs   int             number of arguments of the calling function that determines the calculation interval.
 *  @return             double          computed covariance.
 *  
 */
double L_calccovar(unsigned char* expr1, short len1, unsigned char* expr2, short len2, 
    int from, int to, int t, std::deque<double>& stack, int nargs, int orig) 
{   
    int nb = 1 + to - from;
    if(nb == 0) 
        return IODE_NAN;
 
    double mean_x = L_mean(expr1, len1, from, to, t, stack, nargs - 1);
    if(!IODE_IS_A_NUMBER(mean_x)) 
        return IODE_NAN;
    
    double mean_y = L_mean(expr2, len2, from, to, t, stack, nargs - 1);
    if(!IODE_IS_A_NUMBER(mean_y)) 
        return IODE_NAN;

    double x, y;
    double sum_xx = 0.0, sum_yy = 0.0, sum_xy = 0.0;
    for(int j = from; j <= to; j++) 
    {
        x = L_exec_sub(expr1, len1, j);
        if(!IODE_IS_A_NUMBER(x)) 
            return IODE_NAN;
        
        y = L_exec_sub(expr2, len2, j);
        if(!IODE_IS_A_NUMBER(y)) 
            return IODE_NAN;

        if(orig) 
            sum_xy += x * y;
        else     
            sum_xy += (x - mean_x) * (y - mean_y);
    }

    return sum_xy / nb;
}


/**
 *  Calculates the covariance at time t between two series on a given time interval. 
 *  Each series is the result of the evaluation of two LEC sub-expressions:
 *      - the first sub expression starts at the position 0 in expr.
 *      - the second sub expression starts at the position expr + length of the first sub expression.
 *  
 *
 *  @return            double           computed covariance
 *
 *  @see L_corr() for the parameter definition
 *  @see L_calccovar() for the formulas.
 */
double L_covar(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs) 
{
    short len1, len2;
    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));

    unsigned char* expr1 = expr + sizeof(short); 
    unsigned char* expr2 = expr + len1 + 2 * sizeof(short);
    double result = L_calccovar(expr1, len1, expr2, len2, from, to, t, stack, nargs, 0);
    return result;
}


/**
 *  Calculates the covariance at time t between two series on a given time interval.
 *  Same function as L_covar() but with the assumption that the means of each series is null. 
 *   
 *  @see L_covar() for more details
 *  @see L_calccovar() for the formulas.
 *  
 */
double L_covar0(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs) 
{
    short len1, len2;
    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));

    unsigned char* expr1 = expr + sizeof(short); 
    unsigned char* expr2 = expr + len1 + 2 * sizeof(short);
    double result = L_calccovar(expr1, len1, expr2, len2, from, to, t, stack, nargs, 1);
    return result;
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
double L_var(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    short len1;
    memcpy(&len1, expr, sizeof(short));

    unsigned char* expr1 = expr + sizeof(short); 
    double result = L_calccovar(expr1, len1, expr1, len1, from, to, t, stack, nargs + 1, 0);
    return result;
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
double L_stddev(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    double var = L_var(expr, nvargs, from, to, t, stack, nargs);
    if(!IODE_IS_A_NUMBER(var) || var < 0.0) 
        return IODE_NAN;
    return sqrt(var);
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
 *  @return            double           computed covariance
 */  

 double L_index(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    short len1, len2;
    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    unsigned char* expr1 = expr + sizeof(short);
    unsigned char* expr2 = expr + len1 + 2 * sizeof(short);

    int nb = 1 + to - from;
    if(nb == 0) 
        return IODE_NAN;

    double x = L_exec_sub(expr1, len1, t);
    if(!IODE_IS_A_NUMBER(x)) 
        return IODE_NAN;

    double y;
    for(int j = from; j <= to; j++) 
    {
        y = L_exec_sub(expr2, len2, j);
        if(!IODE_IS_A_NUMBER(y)) 
            return IODE_NAN;

        if(fabs(x - y) < 1e-30) 
            return (double) j;
    }

    return IODE_NAN;
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
double L_acf(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs)
{    
    short len1, len2;
    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    unsigned char* expr1 = expr + sizeof(short);
    unsigned char* expr2 = expr + len1 + 2 * sizeof(short);

    int nb = 1 + to - from;
    if(nb == 0) 
        return IODE_NAN;

    double x = L_exec_sub(expr1, len1, t);
    if(!IODE_IS_A_NUMBER(x)) 
        return IODE_NAN;

    int k = (int) x;
    if(k < 0 || k > nb / 4) 
        return IODE_NAN;

    double mean_x = L_mean(expr2, len2, from, to, t, stack, nargs - 1);
    if(!IODE_IS_A_NUMBER(mean_x)) 
        return IODE_NAN;

    double y;
    double sum_xy = 0.0, sum_xy0 = 0.0;
    for(int j = from; j <= to - k; j++) 
    {
        x = L_exec_sub(expr2, len2, j);
        if(!IODE_IS_A_NUMBER(x)) 
            return IODE_NAN;
        
        y = L_exec_sub(expr2, len2, j + k);
        if(!IODE_IS_A_NUMBER(y)) 
            return IODE_NAN;

        sum_xy += (x - mean_x) * (y - mean_x);
    }

    for(int j = from; j <= to; j++) 
    {
        x = L_exec_sub(expr2, len2, j);
        if(!IODE_IS_A_NUMBER(x)) 
            return IODE_NAN;
        
        sum_xy0 += (x - mean_x) * (x - mean_x);
    }

    return sum_xy / sum_xy0;
}


/** 
 *  Returns in vy and vt the closest existing values (not IODE_NAN) defined around expr[t].
 *
 *  TODO: describe this better 
 *  
 *  @param [in]  expr1  unsigned char*   CLEC sub-expression (heterogeous container)
 *  @param [in]  len1   short            length of expr1 in bytes
 *  @param [in]  t      int              time of calculation
 *  @param [in]  stack  double*          pointer to the top of the stack of intermediate values / results
 *  @param [out] vt     double*          
 *  @param [out] vy     double* 
 *  @param [in]  notnul 
 *  @return             double           computed correlation 
 *  
 */
int L_calcvals(unsigned char* expr1, short len1, int t, std::deque<double>& stack, int* vt, double* vy, int notnul)
{
    vy[0] = vy[1] = IODE_NAN;
    int nb_obs = (L_getsmpl(L_EXEC_DBV))->nb_periods;

    // 1. Calculate value after t
    for(vt[1] = t + 1; vt[1] < nb_obs; vt[1]++) 
    {
        vy[1] = L_exec_sub(expr1, len1, vt[1]);
        if(IODE_IS_A_NUMBER(vy[1]) && (notnul == 0 || fabs(vy[1]) > 1e-15)) 
            break;
    }

    // 2. Calculate value before t
    for(vt[0] = t - 1; vt[0] >= 0; vt[0]--) 
    {
        vy[0] = L_exec_sub(expr1, len1, vt[0]);
        if(IODE_IS_A_NUMBER(vy[0]) && (notnul == 0 || fabs(vy[0]) > 1e-15)) 
            break;
    }

    // if NO value after AND before t, return IODE_NAN
    if(!IODE_IS_A_NUMBER(vy[0]) && !IODE_IS_A_NUMBER(vy[1])) 
        return -1;
    
    // if values found before and after t, return success
    if(IODE_IS_A_NUMBER(vy[0]) && IODE_IS_A_NUMBER(vy[1])) 
        return 0;

    // if no value after, calculate before t0
    if(!IODE_IS_A_NUMBER(vy[1])) 
    {
        vy[1] = vy[0];
        vt[1] = vt[0];
        vy[0] = IODE_NAN;
        for(vt[0] = vt[1] - 1; vt[0] >= 0; vt[0]--) 
        {
            vy[0] = L_exec_sub(expr1, len1, vt[0]);
            if(IODE_IS_A_NUMBER(vy[0]) && (notnul == 0 || fabs(vy[0]) > 1e-15)) 
                break;
        }
    }
    // if no value before, calculate after t1
    else 
    {
        vy[0] = vy[1];
        vt[0] = vt[1];
        vy[1] = IODE_NAN;
        for(vt[1] = vt[0] + 1; vt[1] < nb_obs; vt[1]++) 
        {
            vy[1] = L_exec_sub(expr1, len1, vt[1]);
            if(IODE_IS_A_NUMBER(vy[1])) 
                break;
        }
    }

    return 0;
}

/**
 *  Computes a value for expr[t]. If expr[t] == IODE_NAN, returns the closer possible linear interpolation 
 *  depending on the existing values around expr[t].
 *  
 *  @see L_corr() for the parameter definition
 *  
 *  @return     double  value of expr[t] or interpolated value
 *  
 */
double L_interpol(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs)
{   
    short len1;
    memcpy(&len1, expr, sizeof(short));
    unsigned char* expr1 = expr + sizeof(short);

    // 1. Calculate value in t
    double x = L_exec_sub(expr1, len1, t);
    if(IODE_IS_A_NUMBER(x)) 
        return x;

    // 2. Calculate values around t
    int vt[2];
    double vy[2];
    L_calcvals(expr1, len1, t, stack, vt, vy, 0);
    
    // if NO value after AND before t, return IODE_NAN
    if(!IODE_IS_A_NUMBER(vy[0]) && !IODE_IS_A_NUMBER(vy[1])) 
        return IODE_NAN;
    
    // 3. Calculate result
    int nb_obs = (L_getsmpl(L_EXEC_DBV))->nb_periods;
    if(vt[0] < 0) 
        return vy[1];
    if(vt[1] >= nb_obs) 
        return vy[0];
    
    double interpol = (vy[0] * vt[1] - vy[1] * vt[0]) / (vt[1] - vt[0]);
    interpol += t * (vy[1] - vy[0]) / (vt[1] - vt[0]);
    return interpol;
}

double L_app(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs)
{   
    short len1, len2;
    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    unsigned char* expr1 = expr + sizeof(short);
    unsigned char* expr2 = expr + len1 + 2 * sizeof(short);

    // 1. Calculate value in t
    double x = L_exec_sub(expr1, len1, t);
    if(IODE_IS_A_NUMBER(x)) 
        return x;

    // 2. Calculate values around t
    int vt[2];
    double vy[2];
    L_calcvals(expr1, len1, t, stack, vt, vy, 1);
    
    // if NO value after AND before t, return IODE_NAN
    if(!IODE_IS_A_NUMBER(vy[0]) && !IODE_IS_A_NUMBER(vy[1])) 
        return IODE_NAN;
    
    // ---- Valeurs apparentées ----
    double ayt = L_exec_sub(expr2, len2, t);
    if(!IODE_IS_A_NUMBER(ayt)) 
        return IODE_NAN;
    
    double ay[2];
    int nb_obs = (L_getsmpl(L_EXEC_DBV))->nb_periods;
    ay[0] = ay[1] = IODE_NAN;
    if(vt[0] >= 0)   
        ay[0] = L_exec_sub(expr2, len2, vt[0]);
    if(vt[1] < nb_obs) 
        ay[1] = L_exec_sub(expr2, len2, vt[1]);

    // ---- Deux valeurs trouvées dans la série initiale ----
    // !! Les deux valeurs doivent exister dans la série apparentée
    if(IODE_IS_A_NUMBER(ay[0]) && IODE_IS_A_NUMBER(ay[1])) 
    {
        if(vt[0] < t && vt[1] > t) 
        {
            if(fabs(vy[0] * ay[1]) < 1e-15) 
                return IODE_NAN;
            
            double delta = (vy[1] * ay[0]) / (vy[0] * ay[1]);
            if(delta < 0) 
                return IODE_NAN;
            
            delta = pow(delta, 1.0/(vt[1]-vt[0]));
            if(_isnan(delta)) 
                return IODE_NAN;
            
            double result = vy[0] * (ayt / ay[0] * pow(delta, t - vt[0]));
            return result;
        }
        else 
        {
            int j = (vt[0] < t) ? 1 : 0;
            if(fabs(ay[j]) < 1e-15) 
                return IODE_NAN;
            
            return ayt * (vy[j] / ay[j]);
        }
    }

    // Seule la valeur en t0 est définie : res(t) <- APP(t) * (ORIG(t0) / APP(t0))
    if(IODE_IS_A_NUMBER(ay[0])) 
    {
        if(fabs(ay[0]) < 1e-15) 
            return IODE_NAN;
        
        return ayt * (vy[0] / ay[0]);
    }

    // Seule la valeur en t1 est définie : res(t) <- APP(t) * (ORIG(t1) / APP(t1))
    if(IODE_IS_A_NUMBER(ay[1])) 
    {
        if(fabs(ay[1]) < 1e-15) 
            return IODE_NAN;
        
        return ayt * (vy[1] / ay[1]);
    }

    return IODE_NAN;
}

double L_dapp(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs)
{   
    short len1, len2;
    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    unsigned char* expr1 = expr + sizeof(short);
    unsigned char* expr2 = expr + len1 + 2 * sizeof(short);

    // 1. Calculate value in t
    double x = L_exec_sub(expr1, len1, t);
    if(IODE_IS_A_NUMBER(x)) 
        return x;

    // 2. Calculate values around t
    int vt[2];
    double vy[2];
    L_calcvals(expr1, len1, t, stack, vt, vy, 0);

    // if NO value after AND before t, return IODE_NAN
    if(!IODE_IS_A_NUMBER(vy[0]) && !IODE_IS_A_NUMBER(vy[1])) 
        return IODE_NAN;

    // ---- Valeurs apparentées ----
    double ayt = L_exec_sub(expr2, len2, t);
    if(!IODE_IS_A_NUMBER(ayt)) 
        return IODE_NAN;
    
    double ay[2];
    int nb_obs = (L_getsmpl(L_EXEC_DBV))->nb_periods;
    ay[0] = ay[1] = IODE_NAN;
    if(vt[0] >= 0)   
        ay[0] = L_exec_sub(expr2, len2, vt[0]);
    if(vt[1] < nb_obs) 
        ay[1] = L_exec_sub(expr2, len2, vt[1]);

    if(IODE_IS_A_NUMBER(ay[0]) && IODE_IS_A_NUMBER(ay[1])) 
    {
        if(vt[0] < t && vt[1] > t) 
        {
            double delta = (ay[1] - ay[0] + vy[0] - vy[1]) / (vt[1]-vt[0]);
            return ayt + vy[0] - ay[0] - (t - vt[0]) * delta;
        }
        else 
        {
            int j = (vt[0] < t) ? 1 : 0;
            return ayt + (vy[j] - ay[j]);
        }
    }

    if(IODE_IS_A_NUMBER(ay[0]))                
        return ayt + (vy[0] - ay[0]);
    
    if(IODE_IS_A_NUMBER(ay[1]))                
        return ayt + (vy[1] - ay[1]);
    
    return IODE_NAN;
}

double L_hpall(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs, int std)
{    
    short len1, len2;
    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    unsigned char* expr1 = expr + sizeof(short);
    unsigned char* expr2 = expr + len1 + 2 * sizeof(short);

    double value = L_exec_sub(expr1, len1, t);
    if(!IODE_IS_A_NUMBER(value)) 
        return IODE_NAN;

    if(t < from || t > to) 
        return IODE_NAN;
    int nb = to - from + 1;
    if(nb < 5) 
        return IODE_NAN;
    
    double* itmp = (double *) SCR_malloc(nb * sizeof(double));
    double* otmp = (double *) SCR_malloc(nb * sizeof(double));
    if(itmp == NULL || otmp == NULL) 
        return IODE_NAN;
    
    for(int j = from; j <= to; j++) 
        itmp[j - from] = L_exec_sub(expr2, len2, j);

    int nbna, dim;
    HP_test(itmp, otmp, nb, &nbna, &dim);
    if(dim < 5) 
    {
        SCR_free(itmp);
        SCR_free(otmp);
        return IODE_NAN;
    }
    
    double lambda = value;
    HP_calc(itmp + nbna, otmp + nbna, dim, lambda, std);

    SCR_free(itmp);
    value = otmp[t - from];
    SCR_free(otmp);
    return value;
}

double L_hp(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    return(L_hpall(expr, len, from, to, t, stack, nargs, 0));
}


double L_hpstd(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    return(L_hpall(expr, len, from, to, t, stack, nargs, 1));
}

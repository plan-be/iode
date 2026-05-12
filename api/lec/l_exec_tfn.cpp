/**
 * Functions to evaluate LEC "time functions". Time functions depend on "time" or "lag" parameters.
 * 
 * Example 1: optional time parameters
 *      sum(2000Y1, 2010Y1, A) computes the sum of elements of A between 2000 and 2010.
 *      sum(2010Y, A)          computes the sum of elements of A between 2010 and t (time of execution)
 *      sum(A)                 computes the sum of elements of A from the first element in sample(0) and t (time of execution)
 *  
 * Example 2: optional lag parameter
 *       dln(3, A+B) computes the difference of A+B in t and in t-3.
 *       dln(A+B)    computes the difference of A+B in t and in t-1.
 *       
 * The table (*L_TFN_FN)[] (at the end of this file) contains the pointers to the functions from L_LAG to L_LASTOBS (see iode.h).
 *
 * Function signature:
 *
 *      double <fnname>(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
 *  
 *  where:
 *      - expr points to the current position in the CLEC expression (in "sum(A + B))", expr points to "A + B")
 *      - len is the length of the (sub-) expression expr (here the length of "A+B" compiled)
 *      - t is the current execution time (index in the Sample) 
 *      - stack points to the top of the stack 
 *      - nargs is the number of arguments of the call to the function 
 *  
 *  Example 3: the sample is 2000Y1-2030Y1 and we want to calculate "55.1+dln(A+B)" in 200Y1.
 *  
 *      When entering the function L_dln(), the parameters are:
 *          - expr ~ "A+B"
 *          - len = length of "A+B" (compiled in CLEC)
 *          - t = 1 (2001Y1 is in position 1 with respect to 2000Y1, the beginning of the sample)
 *          - *stack = 55.1 = top of the stack before the call to L_dln()
 *          - nargs = 1 (A+B is the only argument of dln())
 *  
 *  Example 4: the sample is 2000Y1-2030Y1 and we want to calculate "55.1+dln(2, A+B)" in 200Y1.
 *  
 *      When entering the function L_dln(), the parameters are:
 *          - expr ~ "A+B"
 *          - len = length of "A+B" (compiled in CLEC)
 *          - t = 1 (2001Y1 is one position right from 2000Y1)
 *          - *stack = 2 is the current stack before the call to L_lag()
 *          - nargs = 2 (2 and A+B are the 2 arguments of dln) 
 */
#include <math.h>
#include "api/lec/lec.h"


/**
 *  Utility function to retrieve one or 2 optional time values on the stack.
 *  Ex.: 
 *      vmax(1990Y1, 2000Y1, A+B):  from=1990Y1, to=2000Y1
 *      vmax(1990Y1, A+B):          from=1990Y1, to=t
 *      vmax(A+B):                  from=0,      to=t
 * 
 *  Note: do not consume the arguments on the stack.
 *  
 *  @param [in]  t      int     current value of t (in a simulation loop for example)
 *  @param [in]  stack  double  top of the stack
 *  @param [in]  nargs  int     nb of args passed to the function
 *  @param [out] from   int*    position in the sample of the "from" arg
 *  @param [out] to     int*    position in the sample of the "to" arg
 */
void L_extract_time_range(int t, std::deque<double>& stack, int nargs, int& from, int& to)
{
    from = 0;
    to = t;
    if(nargs == 1) 
        return;

    double arg;
    if(nargs == 3)
    {
        arg = stack.back();
        to = L_intlag(arg);
        stack.pop_back();
    }

    arg = stack.back();
    from = L_intlag(arg);
    stack.pop_back();

    // if from > to, swap them
    if(from > to) 
    {
        int j = from;
        from = to;
        to = j;
    }
}

/* ============ TFN ============ */

/**
 *  Computes a lagged CLEC sub expression.
 *  
 *  If narg == 1, the argument is the expression 
 *  If narg == 2, the first argument (lag) is on the stack
 *  
 *  @param [in] expr    unsigned char*  compiled clec sub-expression to compute
 *  @param [in] len     short           length of expr
 *  @param [in] t       int             position in the current KDB Sample for which the expression will be calculated
 *  @param [in] stack   double*         top of the stack of values
 *  @param [in] nargs   int             number of arguments of the function (1 or 2)
 *  @return             double          result of "expr in t"
 *  
 */
double L_lag(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    int lag = 1;
    if(nargs == 2)
    {
        double x = stack.back();
        stack.pop_back();
        lag = L_intlag(x);
    } 
    
    double value = L_exec_sub(expr, len, t - lag);
    return value;
}


/**
 *  Computes expr[t] - expr[t-1]. 
 *  
 *  If a second argument arg2 is provided, returns expr[t] - expr[t-arg2].
 *  
 *  @see L_lag() for more details on parameters.
 */
double L_diff(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    int lag = 1;
    if(nargs == 2)
    {
        double x = stack.back();
        stack.pop_back();
        lag = L_intlag(x);
    }
    
    double v1 = L_exec_sub(expr, len, t);
    if(!IODE_IS_A_NUMBER(v1)) 
        return IODE_NAN;
    
    double v2 = L_exec_sub(expr, len, t - lag);
    if(!IODE_IS_A_NUMBER(v2)) 
        return IODE_NAN;
    
    return v1 - v2;
}


/**
 *  Computes expr[t] / expr[t-1].
 *  If a second argument arg2 is provided, returns expr[t] / expr[t-arg2].
 *  
 *  @see L_lag() for more details on parameters.
 */
double L_rapp(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    int lag = 1;
    if(nargs == 2)
    {
        double x = stack.back();
        stack.pop_back();
        lag = L_intlag(x);
    }
    
    double v1 = L_exec_sub(expr, len, t);
    if(!IODE_IS_A_NUMBER(v1)) 
        return IODE_NAN;
    
    double v2 = L_exec_sub(expr, len, t - lag);
    if(!IODE_IS_A_NUMBER(v2)) 
        return IODE_NAN;

    double result = L_divide(v1, v2);
    return result;
}


/**
 *  Computes ln(expr[t]) - ln(expr[t-1]).
 *  If a second argument arg2 is provided, returns ln(expr[t]) - ln(expr[t-arg2]).
 *  
 *  @see L_lag() for more details on parameters.
 */
double L_dln(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    int lag = 1;
    if(nargs == 2)
    {
        double x = stack.back();
        stack.pop_back();
        lag = L_intlag(x);
    }

    double v1 = L_exec_sub(expr, len, t);
    if(!IODE_IS_A_NUMBER(v1)) 
        return IODE_NAN;
    
    double v2 = L_exec_sub(expr, len, t - lag);
    if(!IODE_IS_A_NUMBER(v2)) 
        return IODE_NAN;
    
    return L_logn(L_divide(v1, v2));
}


/**
 *  Computes 100 * (expr[t] / expr[t-1] - 1).
 *  If a second argument arg2 is provided, returns 100 * (expr[t] / expr[t - arg2] - 1).
 *  
 *  @see L_lag() for more details on parameters.
 */
double L_grt(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    double v1 = L_rapp(expr, len, from, to, t, stack, nargs);
    if(!IODE_IS_A_NUMBER(v1)) 
        return IODE_NAN;
    return (v1 - 1.0) * 100.0;
}


/**
 *  Computes the moving average of an expression. If no second argument is provided, returns expr[t].
 *  If a second argument arg2 is provided, the moving average on arg2 periods.
 *  
 *  @see L_lag() for more details on parameters.
 */
double L_mavg(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    int lag = 1;
    if(nargs == 2)
    {
        double x = stack.back();
        stack.pop_back();
        lag = L_intlag(x);
    }

    lag = (lag < 1) ? 1 : lag;

    double tmp;
    double v1 = 0.0;
    for(int j = 0; j < lag; j++) 
    {
        tmp = L_exec_sub(expr, len, t - j);
        if(!IODE_IS_A_NUMBER(tmp)) 
            return IODE_NAN;
        v1 += tmp;
    }

    return v1 / lag;
}


/**
 *  Computes the maximum of an expression on a sub sample. The boundaries of the sub sample [p1, p2] 
 *  defined below: 
 *      - If nargs = 1, 'from' is the beginning of the KDB sample and 'to'=t
 *      - If nargs = 2, 'from' is on the stack and 'to'=t
 *      - If nargs = 3, 'from' and 'to' are on the stack
 *  
 *  @param [in] expr    unsigned char*  compiled clec sub-expression to compute
 *  @param [in] len     short           length of expr
 *  @param [in] t       int             position in the current KDB Sample for which the expression will be calculated
 *  @param [in] stack   double*         top of the stack of values
 *  @param [in] nargs   int             number of arguments of the function (2 or 3)
 *  @return             double          the maximum of expr on the period [p1, p2]
 *  
 */
double L_vmax(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    double value = L_exec_sub(expr, len, from);
    if(!IODE_IS_A_NUMBER(value)) 
        return IODE_NAN;
 
    double max = value;
    for(int j = from + 1; j <= to; j++) 
    {
        value = L_exec_sub(expr, len, j);
        if(!IODE_IS_A_NUMBER(value)) 
            return IODE_NAN;
        if(value > max) 
            max = value;
    }

    return max;
}


/**
 *  Computes the minimum of an expression on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 */
double L_vmin(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    double value = L_exec_sub(expr, len, from);
    if(!IODE_IS_A_NUMBER(value)) 
        return IODE_NAN;

    double min = value;
    for(int j = from + 1; j <= to; j++) 
    {
        value = L_exec_sub(expr, len, j);
        if(!IODE_IS_A_NUMBER(value)) 
            return IODE_NAN;
        if(value < min) 
            min = value;
    }

    return min;
}


/**
 *  Computes the sum of an expression on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 */
double L_sum(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{   
    double value;
    double sum = 0.0;
    for(int j = from; j <= to; j++) 
    {
        value = L_exec_sub(expr, len, j);
        if(!IODE_IS_A_NUMBER(value)) 
            return IODE_NAN;
        sum += value;
    }

    return sum;
}


/**
 *  Computes the product of an expression on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 */
double L_prod(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{
    double value;
    double prod = 1.0;
    for(int j = from; j <= to; j++) 
    {
        value = L_exec_sub(expr, len, j);
        if(!IODE_IS_A_NUMBER(value)) 
            return IODE_NAN;
        prod *= value;
    }

    return prod;
}


/**
 *  Computes the average an expression on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 *  
 *  @note not a function because it is also used by MTFN functions
 */

double L_mean(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{   
    double sum = L_sum(expr, len, from, to, t, stack, nargs);
    if(!IODE_IS_A_NUMBER(sum)) 
        return IODE_NAN;

    int nb = 1 + to - from;
    return sum / nb;
}


/**
 *  Computes the standard error an expression on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 *  
 */
double L_stderr(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{   
    double value;
    double sum = 0.0, sum2 = 0.0;
    for(int j = from; j <= to; j++) 
    {
        value = L_exec_sub(expr, len, j);
        if(!IODE_IS_A_NUMBER(value)) 
            return IODE_NAN;
        sum += value;
        sum2 += value * value;
    }

    int nb = 1 + to - from;
    double x = (sum2 - sum * sum / nb);
    if(nb <= 1 || x < 0) 
        return IODE_NAN;
    
    return sqrt(x / (nb - 1));
}


/**
 *  Computes the last value of an expression which is not IODE_NAN on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 *  
 */
double L_lastobs(unsigned char* expr, short len, int from, int to, int t, std::deque<double>& stack, int nargs)
{   
    double value;
    for(int j = to; j >= from; j--) 
    {
        value = L_exec_sub(expr, len, j);
        if(IODE_IS_A_NUMBER(value)) 
            return value;
    }

    return IODE_NAN;
}

/**
 * @header4iode
 *
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
 *      L_REAL <fnname>(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *  
 *  where:
 *      - expr points to the current position in the CLEC expression (in "sum(A + B))", expr points to "A + B")
 *      - len is the length of the (sub-) expression expr (here the length of "A+B" compiled)
 *      - t is the current execution time (index in the SAMPLE) 
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
 *  
 *  List of functions
 *  -----------------
 *  Note that these functions are all called by L_exec_sub() (@see l_exec.c).
 *  
 *      L_REAL L_diff(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_rapp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_dln(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_grt(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_mavg(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_vmax(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_vmin(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_sum(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_prod(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *             L_REAL L_mean(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_stderr(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *      L_REAL L_lastobs(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *  
 */
#include <math.h>
#include "api/lec/lec.h"

/* ============ TFN ============ */

/**
 *  Computes a lagged CLEC sub expression.
 *  
 *  If narg == 1, the argument is the expression 
 *  If narg == 2, the first argument (lag) is on the stack
 *  
 *  @param [in] expr    unsigned char*  compiled clec sub-expression to compute
 *  @param [in] len     short           length of expr
 *  @param [in] t       int             position in the current KDB SAMPLE for which the expression will be calculated
 *  @param [in] stack   L_REAL*         top of the stack of values
 *  @param [in] nargs   int             number of arguments of the function (1 or 2)
 *  @return             L_REAL          result of "expr in t"
 *  
 */

L_REAL L_lag(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    return(L_exec_sub(expr, len, t - lag, stack));
}


/**
 *  Computes expr[t] - expr[t-1]. 
 *  
 *  If a second argument arg2 is provided, returns expr[t] - expr[t-arg2].
 *  
 *  @see L_lag() for more details on parameters.
 */
L_REAL L_diff(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1, v2;
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    v1 = L_exec_sub(expr, len, t, stack);
    if(!IODE_IS_A_NUMBER(v1)) return(IODE_NAN);
    v2 = L_exec_sub(expr, len, t - lag, stack);
    if(!IODE_IS_A_NUMBER(v2)) return(IODE_NAN);
    return(v1 - v2);
}


/**
 *  Computes expr[t] / expr[t-1].
 *  If a second argument arg2 is provided, returns expr[t] / expr[t-arg2].
 *  
 *  @see L_lag() for more details on parameters.
 */
L_REAL L_rapp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1, v2;
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    v1 = L_exec_sub(expr, len, t, stack);
    if(!IODE_IS_A_NUMBER(v1)) return(IODE_NAN);
    v2 = L_exec_sub(expr, len, t - lag, stack);
    if(!IODE_IS_A_NUMBER(v2)) return(IODE_NAN);
    return(L_divide(v1, v2));
}


/**
 *  Computes ln(expr[t]) - ln(expr[t-1]).
 *  If a second argument arg2 is provided, returns ln(expr[t]) - ln(expr[t-arg2]).
 *  
 *  @see L_lag() for more details on parameters.
 */
L_REAL L_dln(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1, v2;
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    v1 = L_exec_sub(expr, len, t, stack);
    if(!IODE_IS_A_NUMBER(v1)) return(IODE_NAN);
    v2 = L_exec_sub(expr, len, t - lag, stack);
    if(!IODE_IS_A_NUMBER(v2)) return(IODE_NAN);
    return(L_logn(L_divide(v1, v2)));
}


/**
 *  Computes 100 * (expr[t] / expr[t-1] - 1).
 *  If a second argument arg2 is provided, returns 100 * (expr[t] / expr[t - arg2] - 1).
 *  
 *  @see L_lag() for more details on parameters.
 */
L_REAL L_grt(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1;

    v1 = L_rapp(expr, len, t, stack, nargs);
    if(!IODE_IS_A_NUMBER(v1)) return(IODE_NAN);
    return((L_REAL)((v1 - 1.0) * 100.0));
}


/**
 *  Computes the moving average of an expression. If no second argument is provided, returns expr[t].
 *  If a second argument arg2 is provided, the moving average on arg2 periods.
 *  
 *  @see L_lag() for more details on parameters.
 */
L_REAL L_mavg(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1 = 0.0, tmp;
    int     n = 1, j;

    if(nargs == 2) n = L_intlag(*stack); /* JMP38 27-10-92 */
    n = (n < 1) ? 1 : n;
    for(j = 0 ; j < n ; j++) {
        tmp = L_exec_sub(expr, len, t - j, stack);
        if(!IODE_IS_A_NUMBER(tmp)) return(IODE_NAN);
        v1 += tmp;
    }
    return((L_REAL)(v1 / n));
}


/**
 *  Computes the maximum of an expression on a sub sample. The boundaries of the sub sample [p1, p2] 
 *  defined below: 
 *      - If nargs = 1, p1 is the beginning of the KDB sample and p2=t
 *      - If nargs = 2, p1 is on the stack and p2=t
 *      - If nargs = 3, p1 and p2 are on the stack
 *  
 *  @param [in] expr    unsigned char*  compiled clec sub-expression to compute
 *  @param [in] len     short           length of expr
 *  @param [in] t       int             position in the current KDB SAMPLE for which the expression will be calculated
 *  @param [in] stack   L_REAL*         top of the stack of values
 *  @param [in] nargs   int             number of arguments of the function (2 or 3)
 *  @return             L_REAL          the maximum of expr on the period [p1, p2]
 *  
 */
L_REAL L_vmax(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1, v2;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    v1 = L_exec_sub(expr, len, from, stack);
    if(!IODE_IS_A_NUMBER(v1)) return(IODE_NAN);
    for(j = from + 1 ; j <= to ; j++) {
        v2 = L_exec_sub(expr, len, j, stack);
        if(!IODE_IS_A_NUMBER(v2)) return(IODE_NAN);
        if(v2 > v1) v1 = v2;
    }
    return(v1);
}


/**
 *  Computes the minimum of an expression on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 */
L_REAL L_vmin(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1, v2;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    v1 = L_exec_sub(expr, len, from, stack);
    for(j = from + 1 ; j <= to ; j++) {
        v2 = L_exec_sub(expr, len, j, stack);
        if(!IODE_IS_A_NUMBER(v2)) return(IODE_NAN);
        if(v2 < v1) v1 = v2;
    }
    return(v1);
}


/**
 *  Computes the sum of an expression on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 */
L_REAL L_sum(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL    v1 = 0.0, tmp;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = from ; j <= to ; j++) {
        tmp = L_exec_sub(expr, len, j, stack);
        if(!IODE_IS_A_NUMBER(tmp)) return(IODE_NAN);
        v1 += tmp;
    }
    return(v1);
}


/**
 *  Computes the product of an expression on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 */
L_REAL L_prod(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1 = 1.0, tmp;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = from ; j <= to ; j++) {
        tmp = L_exec_sub(expr, len, j, stack);
        if(!IODE_IS_A_NUMBER(tmp)) return(IODE_NAN);
        v1 *= tmp;
    }
    return(v1);
}


/**
 *  Computes the average an expression on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 *  
 *  @note not a function because it is also used by MTFN functions
 */

L_REAL L_mean(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1;
    int     from, to;

    L_tfn_args(t, stack, nargs, &from, &to);
    v1 = L_sum(expr, len, t, stack, nargs);
    if(!IODE_IS_A_NUMBER(v1)) return(IODE_NAN);
    v1 /= (1 + to - from);
    return(v1);
}


/**
 *  Computes the standard error an expression on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 *  
 */
L_REAL L_stderr(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  s = 0.0, s2 = 0.0, v1;
    int     from, to, j, n;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = from ; j <= to ; j++) {
        v1  = L_exec_sub(expr, len, j, stack);
        if(!IODE_IS_A_NUMBER(v1)) return(IODE_NAN);
        s  += v1;
        s2 += v1 * v1;
    }
    n = 1 + to - from;
    if(n <= 1 || (s2 - s * s / n) < 0) return(IODE_NAN);  /* JMP 20-04-98 */
    v1 = sqrt((s2 - s * s / n) / (n - 1));             /* JMP 20-04-98 */

    return(v1);
}


/**
 *  Computes the last value of an expression which is not IODE_NAN on a sub sample. 
 *  
 *  @see L_vmax() for more details.
 *  
 */
L_REAL L_lastobs(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  tmp;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = to ; j >= from ; j--) {
        tmp = L_exec_sub(expr, len, j, stack);
        if(IODE_IS_A_NUMBER(tmp)) return(tmp);
    }
    return(IODE_NAN);
}

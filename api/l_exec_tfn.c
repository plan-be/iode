/**
 * @header4iode
 *
 * Functions to evaluate LEC "time functions". Time functions operate on "partial" samples. 
 * 
 * Example 
 *      sum(2000Y1, 2010Y1, A) computes the sum of elements of A between 2000 and 2010.
 *   
 * The table (*L_TFN_FN)[] (at the end of this file) contains the pointers 
 * to the functions from L_LAG to L_LASTOBS (see iode.h).
 *
 * Function signature:
 *
 *      L_REAL <fnname>(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
 *  
 *  where:
 *      - expr points to the current position in the CLEC expression (in "X + sum(A + B))", expr points to "A + B")
 *      - len is the length of the (sub-) expression expr (here the length of "A+B" compiled)
 *      - t is the current execution time (index in the SAMPLE) 
 *      - from receives the first position for the calculation 
 *      - to receives the last position for the calculation     
 *  
 */

#include "iode.h"

/* ============ TFN ============ */

extern void L_tfn_args(int t, L_REAL* stack, int nargs, int* from, int* to);
extern int  L_intlag(L_REAL);

static L_REAL L_lag(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    return(L_exec_sub(expr, len, t - lag, stack));
}

static L_REAL L_diff(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1, v2;
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    v1 = L_exec_sub(expr, len, t, stack);
    if(!L_ISAN(v1)) return(L_NAN);
    v2 = L_exec_sub(expr, len, t - lag, stack);
    if(!L_ISAN(v2)) return(L_NAN);
    return(v1 - v2);
}

static L_REAL L_rapp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1, v2;
    int     lag = 1;
    extern L_REAL L_divide();

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    v1 = L_exec_sub(expr, len, t, stack);
    if(!L_ISAN(v1)) return(L_NAN);
    v2 = L_exec_sub(expr, len, t - lag, stack);
    if(!L_ISAN(v2)) return(L_NAN);
    return(L_divide(v1, v2));
}

static L_REAL L_dln(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    extern L_REAL L_divide();
    L_REAL  v1, v2;
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    v1 = L_exec_sub(expr, len, t, stack);
    if(!L_ISAN(v1)) return(L_NAN);
    v2 = L_exec_sub(expr, len, t - lag, stack);
    if(!L_ISAN(v2)) return(L_NAN);
    return(L_logn(L_divide(v1,v2)));
}

static L_REAL L_grt(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1;

    v1 = L_rapp(expr, len, t, stack, nargs);
    if(!L_ISAN(v1)) return(L_NAN);
    return((L_REAL)((v1 - 1.0) * 100.0));
}

static L_REAL L_mavg(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1 = 0.0, tmp;
    int     n = 1, j;

    if(nargs == 2) n = L_intlag(*stack); /* JMP38 27-10-92 */
    n = (n < 1) ? 1 : n;
    for(j = 0 ; j < n ; j++) {
        tmp = L_exec_sub(expr, len, t - j, stack);
        if(!L_ISAN(tmp)) return(L_NAN);
        v1 += tmp;
    }
    return((L_REAL)(v1 / n));
}

static L_REAL L_vmax(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1, v2;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    v1 = L_exec_sub(expr, len, from, stack);
    if(!L_ISAN(v1)) return(L_NAN);
    for(j = from + 1 ; j <= to ; j++) {
        v2 = L_exec_sub(expr, len, j, stack);
        if(!L_ISAN(v2)) return(L_NAN);
        if(v2 > v1) v1 = v2;
    }
    return(v1);
}

static L_REAL L_vmin(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)

{
    L_REAL  v1, v2;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    v1 = L_exec_sub(expr, len, from, stack);
    for(j = from + 1 ; j <= to ; j++) {
        v2 = L_exec_sub(expr, len, j, stack);
        if(!L_ISAN(v2)) return(L_NAN);
        if(v2 < v1) v1 = v2;
    }
    return(v1);
}

static L_REAL L_sum(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL    v1 = 0.0, tmp;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = from ; j <= to ; j++) {
        tmp = L_exec_sub(expr, len, j, stack);
        if(!L_ISAN(tmp)) return(L_NAN);
        v1 += tmp;
    }
    return(v1);
}

static L_REAL L_prod(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1 = 1.0, tmp;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = from ; j <= to ; j++) {
        tmp = L_exec_sub(expr, len, j, stack);
        if(!L_ISAN(tmp)) return(L_NAN);
        v1 *= tmp;
    }
    return(v1);
}

// !! not static because used by MTFN functions
L_REAL L_mean(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  v1;
    int     from, to;

    L_tfn_args(t, stack, nargs, &from, &to);
    v1 = L_sum(expr, len, t, stack, nargs);
    if(!L_ISAN(v1)) return(L_NAN);
    v1 /= (1 + to - from);
    return(v1);
}


static L_REAL L_stderr(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  s = 0.0, s2 = 0.0, v1;
    int     from, to, j, n;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = from ; j <= to ; j++) {
        v1  = L_exec_sub(expr, len, j, stack);
        if(!L_ISAN(v1)) return(L_NAN);
        s  += v1;
        s2 += v1 * v1;
    }
    n = 1 + to - from;
    if(n <= 1 || (s2 - s * s / n) < 0) return(L_NAN);  /* JMP 20-04-98 */
    v1 = sqrt((s2 - s * s / n) / (n - 1));             /* JMP 20-04-98 */

    return(v1);
}

static L_REAL L_lastobs(unsigned char* expr, short len, int t, L_REAL* stack, int nargs)
{
    L_REAL  tmp;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = to ; j >= from ; j--) {
        tmp = L_exec_sub(expr, len, j, stack);
        if(L_ISAN(tmp)) return(tmp);
    }
    return(L_NAN);
}


/**
 *  Table of function pointers L_TFN_FN
 */
L_REAL(*L_TFN_FN[])() = { 
    L_lag,          // L_LAG       L_TFN + 0 
    L_diff,         // L_DIFF      L_TFN + 1 
    L_rapp,         // L_RAPP      L_TFN + 2 
    L_dln,          // L_DLN       L_TFN + 3 
    L_grt,          // L_GRT       L_TFN + 4 
    L_mavg,         // L_MAVG      L_TFN + 5 
    L_vmax,         // L_VMAX      L_TFN + 6 
    L_vmin,         // L_VMIN      L_TFN + 7 
    L_sum,          // L_SUM       L_TFN + 8 
    L_prod,         // L_PROD      L_TFN + 9 
    L_mean,         // L_MEAN      L_TFN + 10
    L_stderr,       // L_STDERR    L_TFN + 11
    0,              // L_DLAG      L_TFN + 12
    L_lastobs       // L_LASTOBS   L_TFN + 13
};


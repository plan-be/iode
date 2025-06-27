/**
 * @header4iode
 *
 * Functions to evaluate LEC "functions". 
 * The table (*L_FNS_FN)[] contains the pointers of the functions from L_UNIMUS to L_DIV0 (see iode.h).
 *
 * Function signatures:
 *
 *      L_REAL  <fnname>(L_REAL *stack, int nbargs);
 *  or 
 *      L_REAL  <fnname>(L_REAL *stack);
 *  
 *  where *stack* points to the top of the stack and *nbargs* indicates how many parameters have been 
 *  passed to the function in the LEC expression.
 *  
 *  Example: in the LEC expression
 *   
 *      max(A+2, B+1, C) 
 *  
 *  nbargs is 3 and the arguments are *stack, *(stack-1) and *(stack-2).
 *  
 *  
 *  List of functions
 *  -----------------
 *  Note that these functions are all called by L_exec_sub() (@see l_exec.c).
 *  
 *             L_REAL L_logn(L_REAL v)
 *      L_REAL L_uminus(L_REAL* stack)
 *      L_REAL L_uplus (L_REAL* stack)
 *      L_REAL L_log(L_REAL* stack, int nargs)
 *      L_REAL L_ln(L_REAL* stack)
 *      L_REAL L_not(L_REAL* stack)
 *      L_REAL L_expn(L_REAL* stack, int nargs)
 *      L_REAL L_max(L_REAL* stack, int nargs)
 *      L_REAL L_min(L_REAL* stack, int nargs)
 *      L_REAL L_sin (L_REAL* stack)
 *      L_REAL L_cos (L_REAL* stack)
 *      L_REAL L_acos (L_REAL* stack)
 *      L_REAL L_asin (L_REAL* stack)
 *      L_REAL L_tan (L_REAL* stack)
 *      L_REAL L_atan (L_REAL* stack)
 *      L_REAL L_tanh (L_REAL* stack)
 *      L_REAL L_sinh (L_REAL* stack)
 *      L_REAL L_cosh (L_REAL* stack)
 *      L_REAL L_abs (L_REAL* stack)
 *      L_REAL L_sqrt (L_REAL* stack)
 *      L_REAL L_int (L_REAL* stack)
 *      L_REAL L_rad (L_REAL* stack)
 *      L_REAL L_if(L_REAL* stack, int nargs)
 *      L_REAL L_lsum(L_REAL* stack, int nargs)
 *      L_REAL L_lmean(L_REAL* stack, int nargs)
 *      L_REAL L_fnisan(L_REAL* stack, int nargs)
 *      L_REAL L_lcount(L_REAL* stack, int nargs)
 *      L_REAL L_lprod(L_REAL* stack, int nargs)
 *      L_REAL L_sign(L_REAL* stack)
 *      L_REAL L_lstderr(L_REAL* stack, int nargs)
 *      L_REAL L_random(L_REAL* stack)
 *      L_REAL L_floor(L_REAL* stack)
 *      L_REAL L_ceil (L_REAL* stack)
 *      L_REAL L_round(L_REAL* stack, int nargs)
 *      L_REAL L_urandom(L_REAL* stack)
 *      double randBoxMuller(double rv_mean, double rv_sd)
 *      L_REAL L_grandom(L_REAL* stack)
 *      double dgamma(double x)
 *      L_REAL L_gamma(L_REAL* stack)
 *      L_REAL L_div0(L_REAL *stack, int nargs)
 *  
 */
#include <math.h>
#include <time.h>

#include "api/lec/lec.h"

#ifdef __GNUC__
    #define _isnan isnan
#endif

/* Global functions (not static!) */
L_REAL L_logn(L_REAL v)
{
    double  x;

    if(!IODE_IS_A_NUMBER(v) || v <= 0) {
        return((L_REAL)IODE_NAN);
        /*      L_errno = L_LOG_ERR;
        	longjmp(L_JMP, 1);
        */
    }

    x = log((double)v);
    if(_isnan(x)) x = IODE_NAN; /* JMP 18-01-02 */
    return((L_REAL)x);
}

/*------------- GROUP L_FNS_FN ---------------------*/

L_REAL L_uminus(L_REAL* stack) {return(- *stack);}
L_REAL L_uplus (L_REAL* stack) {return(*stack);}

L_REAL L_log(L_REAL* stack, int nargs)
{
    extern L_REAL L_divide();
    
    if(nargs == 2) return(L_divide(L_logn(*stack), L_logn(*(stack - 1))));
    return(L_logn(*stack));
}

L_REAL L_ln(L_REAL* stack)  {return(L_logn(*stack));}
L_REAL L_not(L_REAL* stack) {return((fabs((double)(*stack)) < 1e-15) ? 1.0: 0.0);}

/**
 *  Computes the exponential of the value on the stack.
 *  If nargs == 1, it's the neperian exponential
 *  If nargs == 2, the mantissa is the second to last value on the stack.
 *  
 *  @param [in] stack   L_REAL*     pointer to the top of the stack
 *  @param [in] nargs   int         number of arguments 
 *  @return 
 *  
 */
L_REAL L_expn(L_REAL* stack, int nargs)
{
    double  x, a, b;

    if(nargs == 1) {
        if(*stack >= DMAXEXP || *stack <= DMINEXP) return(IODE_NAN);
        x = exp(*stack);
        if(x >= MAXDOUBLE || x <= MINDOUBLE) return(IODE_NAN);
        return(x);
    }
    else {
        a = *(stack - 1);
        b = *stack;
        return(L_exp(a, b));
        /* ancienne version pre-2018
        if(a < 0 && b != (int)b) return(IODE_NAN);
        x = (L_REAL)pow((double)(*(stack - 1)), (double)(*stack)); // JMP 18-01-02
        if(_isnan(x)) x = IODE_NAN;
        if(x >= MAXDOUBLE || x <= MINDOUBLE) x = IODE_NAN;
        return((L_REAL)x);
        */
    }
}

L_REAL L_max(L_REAL* stack, int nargs)
{
    L_REAL  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        if(m < *(stack - j)) m = *(stack - j);
    return(m);
}

L_REAL L_min(L_REAL* stack, int nargs)
{
    L_REAL  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        if(m > *(stack - j)) m = *(stack - j);
    return(m);
}

L_REAL L_sin  (L_REAL* stack) {return((L_REAL)sin ((double)*stack));}
L_REAL L_cos  (L_REAL* stack) {return((L_REAL)cos ((double)*stack));}
L_REAL L_acos (L_REAL* stack) {return((L_REAL)acos((double)*stack));}
L_REAL L_asin (L_REAL* stack) {return((L_REAL)asin((double)*stack));}
L_REAL L_tan  (L_REAL* stack) {return((L_REAL)tan ((double)*stack));}
L_REAL L_atan (L_REAL* stack) {return((L_REAL)atan((double)*stack));}
L_REAL L_tanh (L_REAL* stack) {return((L_REAL)tanh((double)*stack));}
L_REAL L_sinh (L_REAL* stack) {return((L_REAL)sinh((double)*stack));}
L_REAL L_cosh (L_REAL* stack) {return((L_REAL)cosh((double)*stack));}
L_REAL L_abs  (L_REAL* stack) {return((L_REAL)fabs((double)*stack));}
L_REAL L_sqrt (L_REAL* stack) {return((*stack < 0) ? IODE_NAN: (L_REAL)sqrt((double)*stack));}
L_REAL L_int  (L_REAL* stack) {return((L_REAL)floor((double)(0.5 + *stack)));}
L_REAL L_rad  (L_REAL* stack) {return((L_REAL)(((double)(*stack)) * M_PI / 180.0));}

L_REAL L_if(L_REAL* stack, int nargs)
{
    double  cond = *(stack - 2);

    if(!IODE_IS_A_NUMBER(cond) || fabs(cond) < 1e-15)  return(*stack); /* JMP 14-06-07 */
    else                                     return(*(stack - 1));
}

L_REAL L_lsum(L_REAL* stack, int nargs)
{
    L_REAL  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        m += *(stack - j);
    return(m);
}

L_REAL L_lmean(L_REAL* stack, int nargs)
{
    L_REAL  m = 0.0;
    int     j, no = 0;

    for(j = 0 ; j < nargs ; j++) {
        if(IODE_IS_A_NUMBER(*(stack - j))) {             /* JMP 03-03-99 */
            m += *(stack - j);
            no++;
        }
    }
    if(no) return(m / no);
    else return(IODE_NAN);
}

L_REAL L_fnisan(L_REAL* stack, int nargs) {return((L_REAL)IODE_IS_A_NUMBER(*stack));}
L_REAL L_lcount(L_REAL* stack, int nargs) {return((L_REAL)nargs);}

L_REAL L_lprod(L_REAL* stack, int nargs)
{
    L_REAL  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        m *= *(stack - j);
    return(m);
}

L_REAL L_sign(L_REAL* stack)
{
    if(*stack < 0) return((L_REAL)-1.0);
    else           return((L_REAL)1.0);
}

L_REAL L_lstderr(L_REAL* stack, int nargs)
{
    L_REAL  m = 0, x, mean;
    int     j, no = 0;

    mean = L_lmean(stack, nargs);
    for(j = 0 ; j < nargs ; j++) {
        if(IODE_IS_A_NUMBER(*(stack - j))) {             /* JMP 03-03-99 */
            x = *(stack - j) - mean;
            m += x * x;
            no++;
        }
    }
    if(no > 1) return(sqrt(m / (no - 1)));
    else return(IODE_NAN);
}

L_REAL L_random(L_REAL* stack)
{
    static int init = 0;
    double  x, s = *stack;

    if(s == 0.0) return(0.0);
    if(init == 0) {
        srand((unsigned)time(NULL));
        init = 1;
    }

    x = (rand() - (double)RAND_MAX / 2) / (double)RAND_MAX;
    return(x * s);
}


L_REAL L_floor(L_REAL* stack) {return((L_REAL)floor((double)(*stack))); }
L_REAL L_ceil (L_REAL* stack) {return((L_REAL)(1.0 + floor((double)(*stack))));}

L_REAL L_round(L_REAL* stack, int nargs)
{
    double cf = 0, val = *stack;

    if(nargs > 1) {
        cf = (int)(*stack);
        val = *(stack - 1);
    }
    cf = pow(10, cf);
    val = floor(0.5 + val * cf);
    return((L_REAL)(val / cf));
}

L_REAL L_urandom(L_REAL* stack)
{
    static int init = 0;
    double  x, s = *stack;

    if(s == 0.0) return(0.0);
    if(init == 0) {
        srand((unsigned)time(NULL));
        init = 1;
    }

    x = (double) rand()/((double)RAND_MAX + 1.0);
    return(x * s);
}

/* grandom (gaussian random value) */
//#include <math.h>
//#include <stdlib.h>

#define u_rand() ((double) rand()/(1.0+(double) RAND_MAX))
static double  z2 =0.0;
static int  use_z2 = 1;

static double randBoxMuller(double rv_mean, double rv_sd)
{
    double  z1, x, y, w;
    if(use_z2 && z2 != 0.0) {
        z1 = z2;
        z2 = 0.0;
    }
    else {
        do {
            /* choose x,y in uniform square (-1,-1) to (+1,+1) */
            x = -1.0 + 2.0 * u_rand();
            y = -1.0 + 2.0 * u_rand();
            /* see if it is in the unit circle */
            w = x * x + y * y;
        }
        while(w > 1.0 || w == 0);
        /* Box-Muller transform */
        w = sqrt(-2.0 * log(w)/w);
        z2 = x * w;
        z1 = y * w;
    }
    return(rv_mean + rv_sd * z1);
}

L_REAL L_grandom(L_REAL* stack)
{
    extern double randBoxMuller(double m, double s);
    double  s = *stack, m = *(stack - 1);

    return(randBoxMuller(m, s));
}

/* Gamma function in double precision */
static double dgamma(double x)
{
    int k, n;
    double w, y;
    n = x < 1.5 ? -((int)(2.5 - x)) : (int)(x - 1.5);
    w = x - (n + 2);
    y = ((((((((((((-1.99542863674e-7 * w + 1.337767384067e-6) * w -
                   2.591225267689e-6) * w - 1.7545539395205e-5) * w +
                 1.45596568617526e-4) * w - 3.60837876648255e-4) * w -
               8.04329819255744e-4) * w + 0.008023273027855346) * w -
             0.017645244547851414) * w - 0.024552490005641278) * w +
           0.19109110138763841) * w - 0.233093736421782878) * w -
         0.422784335098466784) * w + 0.99999999999999999;
    if(n > 0) {
        w = x - 1;
        for(k = 2; k <= n; k++) {
            w *= x - k;
        }
    }
    else {
        w = 1;
        for(k = 0; k > n; k--) {
            y *= x - k;
        }
    }
    return w / y;
}

L_REAL L_gamma(L_REAL* stack)
{
    extern double dgamma(double x);
    double  s = *stack;

    return(dgamma(s));
}

L_REAL L_div0(L_REAL *stack, int nargs)
{
    double a = *(stack - 1);
    double b = *stack;

    if(!IODE_IS_A_NUMBER(b) || !IODE_IS_A_NUMBER(a)) return((L_REAL)IODE_NAN);
    else if(b == 0) return(0);
    return(a / b);
}


/**
 *  Table of function pointers L_FNS_FN
 */
L_REAL(*L_FNS_FN[])() = { 
    L_uminus,       // L_UMINUS    L_FN + 0
    L_uplus,        // L_UPLUS     L_FN + 1
    L_log,          // L_LOG       L_FN + 2  
    L_ln,           // L_LN        L_FN + 3
    L_not,          // L_NOT       L_FN + 4  
    L_expn,         // L_EXPN      L_FN + 5   
    L_max,          // L_MAX       L_FN + 6
    L_min,          // L_MIN       L_FN + 7  
    L_sin,          // L_SIN       L_FN + 8  
    L_cos,          // L_COS       L_FN + 9
    L_acos,         // L_ACOS      L_FN + 10   
    L_asin,         // L_ASIN      L_FN + 11   
    L_tan,          // L_TAN       L_FN + 12
    L_atan,         // L_ATAN      L_FN + 13   
    L_tanh,         // L_TANH      L_FN + 14   
    L_sinh,         // L_SINH      L_FN + 15
    L_cosh,         // L_COSH      L_FN + 16   
    L_abs,          // L_ABS       L_FN + 17  
    L_sqrt,         // L_SQRT      L_FN + 18
    L_int,          // L_INT       L_FN + 19  
    L_rad,          // L_RAD       L_FN + 20  
    L_if,           // L_IF        L_FN + 21
    L_lsum,         // L_LSUM      L_FN + 22   
    L_lmean,        // L_LMEAN     L_FN + 23    
    L_fnisan,       // L_FNISAN    L_FN + 24
    L_lcount,       // L_LCOUNT    L_FN + 25     
    L_lprod,        // L_LPROD     L_FN + 26    
    L_sign,         // L_SIGN      L_FN + 27
    L_lstderr,      // L_LSTDERR   L_FN + 28     
    L_random,       // L_RANDOM    L_FN + 29     
    L_floor,        // L_FLOOR     L_FN + 30
    L_ceil,         // L_CEIL      L_FN + 31     
    L_round,        // L_ROUND     L_FN + 32     
    L_urandom,      // L_URANDOM   L_FN + 33
    L_grandom,      // L_GRANDOM   L_FN + 34     
    L_gamma,        // L_GAMMA     L_FN + 35     
    L_div0          // L_DIV0      L_FN + 36
};

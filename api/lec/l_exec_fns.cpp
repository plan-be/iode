/**
 * @header4iode
 *
 * Functions to evaluate LEC "functions". 
 * The table (*L_FNS_FN)[] contains the pointers of the functions from L_UNIMUS to L_DIV0 (see iode.h).
 *
 * Function signatures:
 *
 *      double  <fnname>(double *stack, int nbargs);
 *  or 
 *      double  <fnname>(double *stack);
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
 *             double L_logn(double v)
 *      double L_uminus(double* stack)
 *      double L_uplus (double* stack)
 *      double L_log(double* stack, int nargs)
 *      double L_ln(double* stack)
 *      double L_not(double* stack)
 *      double L_expn(double* stack, int nargs)
 *      double L_max(double* stack, int nargs)
 *      double L_min(double* stack, int nargs)
 *      double L_sin (double* stack)
 *      double L_cos (double* stack)
 *      double L_acos (double* stack)
 *      double L_asin (double* stack)
 *      double L_tan (double* stack)
 *      double L_atan (double* stack)
 *      double L_tanh (double* stack)
 *      double L_sinh (double* stack)
 *      double L_cosh (double* stack)
 *      double L_abs (double* stack)
 *      double L_sqrt (double* stack)
 *      double L_int (double* stack)
 *      double L_rad (double* stack)
 *      double L_if(double* stack, int nargs)
 *      double L_lsum(double* stack, int nargs)
 *      double L_lmean(double* stack, int nargs)
 *      double L_fnisan(double* stack, int nargs)
 *      double L_lcount(double* stack, int nargs)
 *      double L_lprod(double* stack, int nargs)
 *      double L_sign(double* stack)
 *      double L_lstderr(double* stack, int nargs)
 *      double L_random(double* stack)
 *      double L_floor(double* stack)
 *      double L_ceil (double* stack)
 *      double L_round(double* stack, int nargs)
 *      double L_urandom(double* stack)
 *      double randBoxMuller(double rv_mean, double rv_sd)
 *      double L_grandom(double* stack)
 *      double dgamma(double x)
 *      double L_gamma(double* stack)
 *      double L_div0(double *stack, int nargs)
 *  
 */
#include <math.h>
#include <time.h>

#include "api/lec/lec.h"

#ifdef __GNUC__
    #define _isnan isnan
#endif

/* Global functions (not static!) */
double L_logn(double v)
{
    double  x;

    if(!IODE_IS_A_NUMBER(v) || v <= 0) {
        return((double)IODE_NAN);
        /*      L_errno = L_LOG_ERR;
        	longjmp(L_JMP, 1);
        */
    }

    x = log((double)v);
    if(_isnan(x)) x = IODE_NAN; /* JMP 18-01-02 */
    return((double)x);
}

/*------------- GROUP L_FNS_FN ---------------------*/

double L_uminus(double* stack, int unused) {return(- *stack);}
double L_uplus (double* stack, int unused) {return(*stack);}

double L_log(double* stack, int nargs)
{   
    if(nargs == 2) 
        return(L_divide(L_logn(*stack), L_logn(*(stack - 1))));
    return(L_logn(*stack));
}

double L_ln(double* stack, int unused)  {return(L_logn(*stack));}
double L_not(double* stack, int unused) {return((fabs((double)(*stack)) < 1e-15) ? 1.0: 0.0);}

/**
 *  Computes the exponential of the value on the stack.
 *  If nargs == 1, it's the neperian exponential
 *  If nargs == 2, the mantissa is the second to last value on the stack.
 *  
 *  @param [in] stack   double*     pointer to the top of the stack
 *  @param [in] nargs   int         number of arguments 
 *  @return 
 *  
 */
double L_expn(double* stack, int nargs)
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
        x = (double)pow((double)(*(stack - 1)), (double)(*stack)); // JMP 18-01-02
        if(_isnan(x)) x = IODE_NAN;
        if(x >= MAXDOUBLE || x <= MINDOUBLE) x = IODE_NAN;
        return((double)x);
        */
    }
}

double L_max(double* stack, int nargs)
{
    double  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        if(m < *(stack - j)) m = *(stack - j);
    return(m);
}

double L_min(double* stack, int nargs)
{
    double  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        if(m > *(stack - j)) m = *(stack - j);
    return(m);
}

double L_sin  (double* stack, int unused) {return((double)sin ((double)*stack));}
double L_cos  (double* stack, int unused) {return((double)cos ((double)*stack));}
double L_acos (double* stack, int unused) {return((double)acos((double)*stack));}
double L_asin (double* stack, int unused) {return((double)asin((double)*stack));}
double L_tan  (double* stack, int unused) {return((double)tan ((double)*stack));}
double L_atan (double* stack, int unused) {return((double)atan((double)*stack));}
double L_tanh (double* stack, int unused) {return((double)tanh((double)*stack));}
double L_sinh (double* stack, int unused) {return((double)sinh((double)*stack));}
double L_cosh (double* stack, int unused) {return((double)cosh((double)*stack));}
double L_abs  (double* stack, int unused) {return((double)fabs((double)*stack));}
double L_sqrt (double* stack, int unused) {return((*stack < 0) ? IODE_NAN: (double)sqrt((double)*stack));}
double L_int  (double* stack, int unused) {return((double)floor((double)(0.5 + *stack)));}
double L_rad  (double* stack, int unused) {return((double)(((double)(*stack)) * M_PI / 180.0));}

double L_if(double* stack, int nargs)
{
    double  cond = *(stack - 2);

    if(!IODE_IS_A_NUMBER(cond) || fabs(cond) < 1e-15)  return(*stack); /* JMP 14-06-07 */
    else                                     return(*(stack - 1));
}

double L_lsum(double* stack, int nargs)
{
    double  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        m += *(stack - j);
    return(m);
}

double L_lmean(double* stack, int nargs)
{
    double  m = 0.0;
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

double L_fnisan(double* stack, int nargs) {return((double)IODE_IS_A_NUMBER(*stack));}
double L_lcount(double* stack, int nargs) {return((double)nargs);}

double L_lprod(double* stack, int nargs)
{
    double  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        m *= *(stack - j);
    return(m);
}

double L_sign(double* stack, int unused)
{
    if(*stack < 0) return((double)-1.0);
    else           return((double)1.0);
}

double L_lstderr(double* stack, int nargs)
{
    double  m = 0, x, mean;
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

double L_random(double* stack, int unused)
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


double L_floor(double* stack, int unused) {return((double)floor((double)(*stack))); }
double L_ceil (double* stack, int unused) {return((double)(1.0 + floor((double)(*stack))));}

double L_round(double* stack, int nargs)
{
    double cf = 0, val = *stack;

    if(nargs > 1) {
        cf = (int)(*stack);
        val = *(stack - 1);
    }
    cf = pow(10, cf);
    val = floor(0.5 + val * cf);
    return((double)(val / cf));
}

double L_urandom(double* stack, int unused)
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

double L_grandom(double* stack, int unused)
{
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

double L_gamma(double* stack, int unused)
{
    double  s = *stack;

    return(dgamma(s));
}

double L_div0(double *stack, int unused)
{
    double a = *(stack - 1);
    double b = *stack;

    if(!IODE_IS_A_NUMBER(b) || !IODE_IS_A_NUMBER(a)) return((double)IODE_NAN);
    else if(b == 0) return 0;
    return(a / b);
}

/**
 *  @header4iode
 *
 *  Tries to find a root of a compiled LEC expression using a secant (bisection) method.
 *  
 *  List of functions
 *  -----------------
 *
 *  
 */

#include "iode.h"

#define LN_FACTOR   1.6             
#define LN_MAXIT    20

static int     LN_VARNB;            //
static double  LN_SHIFT = 0.0;      //
static CLEC    *LN_CLEC;            //
static KDB     *LN_DBV, *LN_DBS;    //


/**
 *  Computes the value of f(x) in time t where:
 *      f is a the LEC expression compiled in LN_CLEC
 *      x is a variable (in position LN_VARNB in KV_WS) present in the LEC expression
 *      t is the position of the computation period (0 for the 1st period of the KDB LN_DBV sample)
 *  
 *  @param   [in]   double  x   value for which the expression must be calculated
 *  @param   [in]   int     t   index in the variable for which the expression must be calculated
 *  @return         double      value of f(x, t)
 *
 *  @global  [in] int     LN_VARNBN position of the variable in the KDB of variables
 *  @global  [in] KDB*    LN_DBV    KDB of variables
 *  @global  [in] KDB*    LN_DBS    KDB of scalars
 *  @global  [in] CLEC*   LN_CLEC   compiled and linked LEC expression
 *  @global  [in] double  LN_SHIFT  ??
 */
static double L_fx(double x, int t)
{
    double  fx;
    IODE_REAL    *d_ptr, oldx;

    d_ptr = L_getvar(LN_DBV, LN_VARNB);
    oldx = d_ptr[t];
    d_ptr[t] = x;

    fx = L_exec(LN_DBV, LN_DBS, LN_CLEC, t);
    d_ptr[t] = oldx;
    return(fx - LN_SHIFT);
}


/**
 *  
 *  
 *  @param [in] double* x1 
 *  @param [in] double* x2 
 *  @param [in] int     t  
 *  @return 
 *  
 */
static int L_bracket(double* x1, double* x2, int t)
{
    int     i;
    double  f1, f2, ox1 = *x1, ox2 = *x2;

    if(*x1 == *x2) return(-1);

    *x1 = ox1 * 0.5;
    *x2 = ox1 * 1.5;

    f1 = L_fx(*x1, t);
    f2 = L_fx(*x2, t);

    for(i = 0; i < LN_MAXIT; i++) {
        if(f1 * f2 < 0.0) return(0);

        if(fabs(f1) < fabs(f2))
            f1 = L_fx(*x1 += LN_FACTOR*(*x1 - *x2), t);
        else
            f2 = L_fx(*x2 += LN_FACTOR*(*x2 - *x1), t);
    }

    return(-1);
}

/**
 *  
 *  
 *  @param [in, out] KDB*   dbv     
 *  @param [in]      KDB*   dbs     
 *  @param [in]      CLEC*  clec    
 *  @param [in]      int    t       
 *  @param [in]      int    varnb   
 *  @param [in]      int    eqvarnb 
 *  @return 
 *  
 *  @details 
 */
double L_secant(KDB* dbv, KDB* dbs, CLEC* clec, int t, int varnb, int eqvarnb)
{
    int     it = 0;
    double  x1, x2, xl, xh, xr,
            dx, tmp,
            fxl, fxh, fxr;
    IODE_REAL    *d_ptr;

    LN_DBV = dbv;
    LN_DBS = dbs;
    LN_CLEC = clec;
    LN_VARNB = varnb;

    d_ptr = L_getvar(dbv, varnb);
    x1 = d_ptr[t];
    if(!L_ISAN(x1)) {
        //return((double)L_NAN);
        x1 = 0.9;
    }

    if(varnb == eqvarnb || clec->dupendo) {   /* JMP 13-12-01 */
        LN_SHIFT = 0.0;
        x1 = fabs(x1);
    }

    else {
        LN_SHIFT = *(L_getvar(dbv, eqvarnb) + t);
        if(!L_ISAN(LN_SHIFT)) {
            return((double)L_NAN); /* GB 03-11-2003 */
        }
        x1 = fabs(LN_SHIFT);
    }

    if(fabs(L_fx(0.0, t)) < 1.0e-6) return(0.0);
    if(L_bracket(&x1, &x2, t) < 0) return((double)L_NAN);

    fxl = L_fx(x1, t);
    fxh = L_fx(x2, t);
    if(fxl < 0) {
        xl = x1;
        xh = x2;
    }
    else {
        xl = x2;
        xh = x1;
        tmp = fxl;
        fxl = fxh;
        fxh = tmp;
    }
    dx = xh - xl;

    while(it < LN_MAXIT) {
        xr = xl + dx * fxl/(fxl -fxh);
        fxr = L_fx(xr, t);
        if(fxr < 0.0) {
            tmp = xl - xr;
            xl = xr;
            fxl = fxr;
        }
        else {
            tmp = xh - xr;
            xh = xr;
            fxh = fxr;
        }
        dx = xh - xl;
        if(fabs(tmp) < 1.0e-6 * fabs(xr) || fabs(fxr) < 1.0e-6) return(xr);
        it++;
    }

    return((double)L_NAN);
}
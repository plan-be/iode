/**
 *  @header4iode
 *
 *  Tries to find a root of a compiled LEC expression using a secant (bisection) method.
 *  
 *  List of functions
 *  -----------------
 *
 *      static double L_fx(double x, int t)                                             Computes the value of f(x) in time t
 *      static int L_bracket(double* x1, double* x2, int t)                             Tries to find 2 values x1 and x2 such as the sign of L_fx(x1) is opposite to the sign of L_fx(x2).
 *      double L_secant(KDBVariables* dbv, KDBScalars* dbs, CLEC* clec, int t, int varnb, int eqvarnb)  Tries to find a solution to the equation clec by a secant method. 
 */
#include <math.h>
#include "api/lec/lec.h"

#define LN_FACTOR   1.6             
#define LN_MAXIT    20

static int     LN_VARNB;            // Current position of endo in LN_DBV
static double  LN_SHIFT = 0.0;      // Value of the endo[t] or 0 if the equation is not analytically solved (0 := lhs - rhs)
static CLEC*   LN_CLEC;             // Current CLEC expression
static KDBVariables* LN_DBV;       // Current KDB scalars
static KDBScalars*   LN_DBS;       // Current KDB of vars


/**
 *  Computes the value of f(x) in time t where:
 *      f is a the LEC expression compiled in LN_CLEC
 *      x is a variable (in position LN_VARNB in global_ws_var) present in the LEC expression
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
    double  *d_ptr, oldx;

    d_ptr = L_getvar(LN_DBV, LN_VARNB);
    oldx = d_ptr[t];
    d_ptr[t] = x;

    fx = L_exec(LN_DBV, LN_DBS, LN_CLEC, t);
    d_ptr[t] = oldx;
    return(fx - LN_SHIFT);
}


/**
 *  Tries to find an interval [x1, x2] where the is a root of L_fx() i.e.: (L_fx(x1) * L_fx(x2) < 0)
 *  (assuming that L_fx() is continuous).
 *  
 *      Start by setting 
 *          x1 = 0.5 * x1 
 *          x2 = 1.5 * x1
 *      If L_fx(x1) * L_fx(x2) < 0, then [x1, x2] meets the condition.
 *      Else, replace x1 and x2: 
 *          x1 = x1 + LN_FACTOR*(x1 - x2)
 *          x2 = x2 + LN_FACTOR*(x2 - x1)
 *      Check if [x1, x2] is now a suitable interval.
 *      If not, try again max LN_MAXIT times.
 *  
 *  
 *  @param [in, out]    double*     x1  left bound of the segment
 *  @param [out]        double*     x2  right bound of the segment
 *  @param [in]         int         t   current period of execution of L_fx()
 *  @return             int         0 if a solution is found, -1 otherwise
 *  
 *  TODO: replace LN_MAXIT and LN_FACTOR by global variables.
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
 *  Tries to find a solution to the equation clec by a secant method.
 *
 *  That basic secant method first requires to determine an interval [xl, xr] containing a root of 
 *  the equation clec (xl/xr stands for x-left/right).
 *  In other words, the sign of f(xl) must be opposite to that of f(xr).
 *  
 *  Then the size of that interval is decreased until |xl - xr| becomes << eps by applying the formula below:
 *      xr = xl + (xr - xl) * f(xl) / (f(xl) -f(xh))
 *      If the sign of f(xr) has changed, switch xr and xl. 
 *      Continue until |xl - xr| is << eps
 *   
 *      
 *  
 *  @param [in] KDB*    dbv     KDB of VAR with which the equation has been linked
 *  @param [in] KDB*    dbs     KDB of Scalar with which the equation has been linked
 *  @param [in] CLEC*   clec    compiled LEC expression 
 *  @param [in] int     t       time of calculation (index in dbv Sample)
 *  @param [in] int     varnb   position of the endogenous variable in dbv
 *  @param [in] int     eqvarnb position of the initial endogenous variable (i.e. equation name) in dbv
 *  
 *  @return     double          root of the equation (varnb value that solves the equation)
 *
 */
double L_secant(KDBVariables* dbv, KDBScalars* dbs, CLEC* clec, int t, int varnb, int eqvarnb)
{
    int     it = 0;
    double  x1, x2, xl, xh, xr,
            dx, tmp,
            fxl, fxh, fxr;
    double    *d_ptr;

    LN_DBV = dbv;
    LN_DBS = dbs;
    LN_CLEC = clec;
    LN_VARNB = varnb;

    d_ptr = L_getvar(dbv, varnb);
    x1 = d_ptr[t];
    if(!IODE_IS_A_NUMBER(x1)) {
        //return((double)IODE_NAN);
        x1 = 0.9;
    }

    // if endogenous is not changed and endo appears more than once in clec,
    // clec is of the form 0 := lhx - rhs. Hence, shift = 0 instead of endo(t)
    if(varnb == eqvarnb || clec->dupendo) {   /* JMP 13-12-01 */
        LN_SHIFT = 0.0;
        x1 = fabs(x1);
    }
    else {
        LN_SHIFT = *(L_getvar(dbv, eqvarnb) + t);
        if(!IODE_IS_A_NUMBER(LN_SHIFT)) {
            return((double)IODE_NAN); /* GB 03-11-2003 */
        }
        x1 = fabs(LN_SHIFT);
    }

    if(fabs(L_fx(0.0, t)) < 1.0e-6) return(0.0);            // Solution 0.0 reached 
    if(L_bracket(&x1, &x2, t) < 0) return((double)IODE_NAN);

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

    return((double)IODE_NAN);
}
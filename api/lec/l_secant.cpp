#pragma once
#include <math.h>
#include "api/lec/lec.h"

#define LN_FACTOR   1.6             
#define LN_MAXIT    20


/**
 *  Computes the value of f(x) in time t where:
 *      f is this LEC expression
 *      x is a variable (defined by var_name) present in the LEC expression
 *      t is the position of the computation period (0 for the 1st period of the KDB sample)
 *  
 *  @param [in]  x         double              value for which the expression must be calculated
 *  @param [in]  t         int                 index in the variable for which the expression must be calculated
 *  @param [in]  dbv       KDBVariablesPtr     KDB of variables
 *  @param [in]  dbs       KDBScalarsPtr       KDB of scalars
 *  @param [in]  var_name  string              name of the variable in the KDB of variables
 *  @param [in]  shift     double              shift value (endo[t] or 0)
 *  @return                double              value of f(x, t)
 */
double CLEC::fx(double x, int t, KDBVariablesPtr dbv, KDBScalarsPtr dbs, const std::string& var_name, double shift)
{
    if(!dbv || !dbs)
        return IODE_NAN;

    double* d_ptr = dbv->get_var_ptr(var_name);
    double oldx = d_ptr[t];
    d_ptr[t] = x;

    double fx = this->execute(dbv, dbs, t);
    d_ptr[t] = oldx;
    return fx - shift;
}


/**
 *  Tries to find an interval [x1, x2] where there is a root of fx() i.e.: (fx(x1) * fx(x2) < 0)
 *  (assuming that fx() is continuous).
 *  
 *      Start by setting 
 *          x1 = 0.5 * x1 
 *          x2 = 1.5 * x1
 *      If fx(x1) * fx(x2) < 0, then [x1, x2] meets the condition.
 *      Else, replace x1 and x2: 
 *          x1 = x1 + LN_FACTOR*(x1 - x2)
 *          x2 = x2 + LN_FACTOR*(x2 - x1)
 *      Check if [x1, x2] is now a suitable interval.
 *      If not, try again max LN_MAXIT times.
 *  
 *  @param [in,out]  x1        double*             left bound of the segment
 *  @param [out]     x2        double*             right bound of the segment
 *  @param [in]      t         int                 current period of execution
 *  @param [in]      dbv       KDBVariablesPtr     KDB of variables
 *  @param [in]      dbs       KDBScalarsPtr       KDB of scalars
 *  @param [in]      var_name  string              name of the variable in the KDB
 *  @param [in]      shift     double              shift value (endo[t] or 0)
 *  @return                    int                 0 if a solution is found, -1 otherwise
 *  
 *  TODO: replace LN_MAXIT and LN_FACTOR by global variables.
 */
int CLEC::bracket(double* x1, double* x2, int t, KDBVariablesPtr dbv, KDBScalarsPtr dbs, 
    const std::string& var_name, double shift)
{
    double ox1 = *x1;
    double ox2 = *x2;

    if(*x1 == *x2) 
        return -1;

    *x1 = ox1 * 0.5;
    *x2 = ox1 * 1.5;

    double f1 = fx(*x1, t, dbv, dbs, var_name, shift);
    double f2 = fx(*x2, t, dbv, dbs, var_name, shift);

    for(int i = 0; i < LN_MAXIT; i++) 
    {
        if(f1 * f2 < 0.0) 
            return 0;

        if(fabs(f1) < fabs(f2))
            f1 = fx(*x1 += LN_FACTOR*(*x1 - *x2), t, dbv, dbs, var_name, shift);
        else
            f2 = fx(*x2 += LN_FACTOR*(*x2 - *x1), t, dbv, dbs, var_name, shift);
    }

    return -1;
}


/**
 *  Tries to find a solution to this LEC equation by a secant method.
 *
 *  That basic secant method first requires to determine an interval [xl, xr] containing a root of 
 *  the equation (xl/xr stands for x-left/right).
 *  In other words, the sign of f(xl) must be opposite to that of f(xr).
 *  
 *  Then the size of that interval is decreased until |xl - xr| becomes << eps by applying the formula below:
 *      xr = xl + (xr - xl) * f(xl) / (f(xl) -f(xh))
 *      If the sign of f(xr) has changed, switch xr and xl. 
 *      Continue until |xl - xr| is << eps
 *   
 *      
 *  
 *  @param [in] KDBVariablesPtr dbv     KDB of VAR with which the equation has been linked
 *  @param [in] KDBScalarsPtr   dbs     KDB of Scalar with which the equation has been linked
 *  @param [in] int             t       time of calculation (index in dbv Sample)
 *  @param [in] int             varnb   position of the endogenous variable in dbv
 *  @param [in] int             eqvarnb position of the initial endogenous variable (i.e. equation name) in dbv
 *  
 *  @return     double                  root of the equation (var_name value that solves the equation)
 *
 */
double CLEC::secant(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, 
    const std::string& var_name, const std::string& eq_var_name)
{
    if(!dbv || !dbs)
        return IODE_NAN;

    double* d_ptr = dbv->get_var_ptr(var_name);
    double x1 = d_ptr[t];
    if(!IODE_IS_A_NUMBER(x1)) 
        x1 = 0.9;

    // if endogenous is not changed and endo appears more than once in clec,
    // clec is of the form 0 := lhx - rhs. Hence, shift = 0 instead of endo(t)
    double shift = 0.0;
    if(var_name == eq_var_name || this->duplicated_endo) 
        x1 = fabs(x1);
    else 
    {
        shift = dbv->get_value(eq_var_name, t);
        if(!IODE_IS_A_NUMBER(shift))
            return (double) IODE_NAN;
        x1 = fabs(shift);
    }

    // Solution 0.0 reached
    if(fabs(fx(0.0, t, dbv, dbs, var_name, shift)) < 1.0e-6) 
        return 0.0;

    double x2 = 0.0;
    if(bracket(&x1, &x2, t, dbv, dbs, var_name, shift) < 0) 
        return (double) IODE_NAN;

    double xl = 0.0;
    double xh = 0.0; 
    double dx = 0.0; 
    double tmp = 0.0;
    double fxl = fx(x1, t, dbv, dbs, var_name, shift);
    double fxh = fx(x2, t, dbv, dbs, var_name, shift);
    if(fxl < 0) 
    {
        xl = x1;
        xh = x2;
    }
    else 
    {
        xl = x2;
        xh = x1;
        tmp = fxl;
        fxl = fxh;
        fxh = tmp;
    }
    dx = xh - xl;

    int it = 0;
    double xr = 0.0; 
    double fxr = 0.0;
    while(it < LN_MAXIT) 
    {
        xr = xl + dx * fxl / (fxl - fxh);
        fxr = fx(xr, t, dbv, dbs, var_name, shift);
        if(fxr < 0.0) 
        {
            tmp = xl - xr;
            xl = xr;
            fxl = fxr;
        }
        else 
        {
            tmp = xh - xr;
            xh = xr;
            fxh = fxr;
        }
        dx = xh - xl;

        if(fabs(tmp) < 1.0e-6 * fabs(xr) || fabs(fxr) < 1.0e-6) 
            return xr;
        
        it++;
    }

    return (double) IODE_NAN;
}

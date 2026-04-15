/**
 * @header4iode
 *
 * Functions to evaluate LEC "operators". 
 * 
 * The table (*L_OPS_FN)[] contains the pointers to the operators from L_OR to L_EXP (see iode.h).
 *
 * Function signature:
 *
 *      double  <fnname>(double value1, double value2);
 *  
 *  where value1 and value2 are the parameters that have been passed to the function in the LEC expression.
 *  
 *  Example: in the LEC expression
 *   
 *      2 < 3
 *  
 *  value1 is 2 and value2 is 3
 *  
 *  List of functions
 *  -----------------
 *      double L_or (double a, double b)
 *      double L_and (double a, double b)
 *      double L_ge (double a, double b)
 *      double L_gt (double a, double b)
 *      double L_le (double a, double b)
 *      double L_lt (double a, double b)
 *      double L_eq (double a, double b)
 *      double L_ne (double a, double b)
 *      double L_plus (double a, double b)
 *      double L_minus(double a, double b)
 *      double L_times(double a, double b)
 *             double L_divide(double a, double b)
 *             double L_exp(double a, double b)
 *  
 */
#include <math.h>
#include "api/lec/lec.h"

#ifdef __GNUC__
    #define _isnan isnan
#endif

double L_or   (double a, double b) {return((a || b) ? (double)1.0 : (double)0.0);}
double L_and  (double a, double b) {return((a && b) ? (double)1.0 : (double)0.0);}
double L_ge   (double a, double b) {return((a >= b) ? (double)1.0 : (double)0.0);}
double L_gt   (double a, double b) {return((a > b)  ? (double)1.0 : (double)0.0);}
double L_le   (double a, double b) {return((a <= b) ? (double)1.0 : (double)0.0);}
double L_lt   (double a, double b) {return((a < b)  ? (double)1.0 : (double)0.0);}
double L_eq   (double a, double b) {return((a == b) ? (double)1.0 : (double)0.0);}
double L_ne   (double a, double b) {return((a != b) ? (double)1.0 : (double)0.0);}
double L_plus (double a, double b) {return(a + b);}
double L_minus(double a, double b) {return(a - b);}
double L_times(double a, double b) {return(a * b);}

/*  Note: L_divide() and L_exp() are used by other functions and must therefore be global. */

double L_divide(double a, double b) 
{
    if(!IODE_IS_A_NUMBER(b) || !IODE_IS_A_NUMBER(a) || b == 0) {
        return((double)IODE_NAN);
    }
    return(a / b);
}

double L_exp(double a, double b)
{
    double x, ib;

    if(a < 0 && b != (int)b) {
        // test 2018 : si (-a)^0.333 => faire -a^0.333
        ib = 1 / b;
        if(fabs(ib - (int) ib) > 1e-8) return(IODE_NAN);
        if((int)ib % 2 != 1)           return(IODE_NAN);
        x = -pow((double)-a, (double)b);
    }
    else
        x = pow((double)a, (double)b);
    if(_isnan(x)) x = IODE_NAN; /* JMP 18-01-02 */
    return((double)x);
}

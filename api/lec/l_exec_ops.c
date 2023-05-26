/**
 * @header4iode
 *
 * Functions to evaluate LEC "operators". 
 * 
 * The table (*L_OPS_FN)[] contains the pointers to the operators from L_OR to L_EXP (see iode.h).
 *
 * Function signature:
 *
 *      L_REAL  <fnname>(L_REAL value1, L_REAL value2);
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
 *      L_REAL L_or (L_REAL a, L_REAL b)
 *      L_REAL L_and (L_REAL a, L_REAL b)
 *      L_REAL L_ge (L_REAL a, L_REAL b)
 *      L_REAL L_gt (L_REAL a, L_REAL b)
 *      L_REAL L_le (L_REAL a, L_REAL b)
 *      L_REAL L_lt (L_REAL a, L_REAL b)
 *      L_REAL L_eq (L_REAL a, L_REAL b)
 *      L_REAL L_ne (L_REAL a, L_REAL b)
 *      L_REAL L_plus (L_REAL a, L_REAL b)
 *      L_REAL L_minus(L_REAL a, L_REAL b)
 *      L_REAL L_times(L_REAL a, L_REAL b)
 *             L_REAL L_divide(L_REAL a, L_REAL b)
 *             L_REAL L_exp(L_REAL a, L_REAL b)
 *  
 */

#include "iode.h"

L_REAL L_or   (L_REAL a, L_REAL b) {return((a || b) ? (L_REAL)1.0 : (L_REAL)0.0);}
L_REAL L_and  (L_REAL a, L_REAL b) {return((a && b) ? (L_REAL)1.0 : (L_REAL)0.0);}
L_REAL L_ge   (L_REAL a, L_REAL b) {return((a >= b) ? (L_REAL)1.0 : (L_REAL)0.0);}
L_REAL L_gt   (L_REAL a, L_REAL b) {return((a > b)  ? (L_REAL)1.0 : (L_REAL)0.0);}
L_REAL L_le   (L_REAL a, L_REAL b) {return((a <= b) ? (L_REAL)1.0 : (L_REAL)0.0);}
L_REAL L_lt   (L_REAL a, L_REAL b) {return((a < b)  ? (L_REAL)1.0 : (L_REAL)0.0);}
L_REAL L_eq   (L_REAL a, L_REAL b) {return((a == b) ? (L_REAL)1.0 : (L_REAL)0.0);}
L_REAL L_ne   (L_REAL a, L_REAL b) {return((a != b) ? (L_REAL)1.0 : (L_REAL)0.0);}
L_REAL L_plus (L_REAL a, L_REAL b) {return(a + b);}
L_REAL L_minus(L_REAL a, L_REAL b) {return(a - b);}
L_REAL L_times(L_REAL a, L_REAL b) {return(a * b);}

/*  Note: L_divide() and L_exp() are used by other functions and must therefore be global. */

L_REAL L_divide(L_REAL a, L_REAL b) 
{
    if(!L_ISAN(b) || !L_ISAN(a) || b == 0) {
        return((L_REAL)L_NAN);
    }
    return(a / b);
}

L_REAL L_exp(L_REAL a, L_REAL b)
{
    double x, ib;

    if(a < 0 && b != (int)b) {
        // test 2018 : si (-a)^0.333 => faire -a^0.333
        ib = 1 / b;
        if(fabs(ib - (int) ib) > 1e-8) return(L_NAN);
        if((int)ib % 2 != 1)           return(L_NAN);
        x = -pow((double)-a, (double)b);
    }
    else
        x = pow((double)a, (double)b);
    if(_isnan(x)) x = L_NAN; /* JMP 18-01-02 */
    return((L_REAL)x);
}


/**
 *  Table of function pointers L_OPS_FN
 */

L_REAL(*L_OPS_FN[])() = {
    L_or,           // L_OR        L_OP + 0
    L_and,          // L_AND       L_OP + 1 
    L_ge,           // L_GE        L_OP + 2
    L_gt,           // L_GT        L_OP + 3
    L_le,           // L_LE        L_OP + 4
    L_lt,           // L_LT        L_OP + 5
    L_eq,           // L_EQ        L_OP + 6
    L_ne,           // L_NE        L_OP + 7
    L_plus,         // L_PLUS      L_OP + 8
    L_minus,        // L_MINUS     L_OP + 9   
    L_divide,       // L_DIVIDE    L_OP + 10    
    L_times,        // L_TIMES     L_OP + 11
    L_exp           // L_EXP       L_OP + 12
};

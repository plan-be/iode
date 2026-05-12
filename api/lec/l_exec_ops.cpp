#include <math.h>
#include "api/lec/lec.h"

#ifdef __GNUC__
    #define _isnan isnan
#endif

double L_or   (double a, double b) { return (a || b) ? 1.0 : 0.0; }
double L_and  (double a, double b) { return (a && b) ? 1.0 : 0.0; }
double L_ge   (double a, double b) { return (a >= b) ? 1.0 : 0.0; }
double L_gt   (double a, double b) { return (a > b)  ? 1.0 : 0.0; }
double L_le   (double a, double b) { return (a <= b) ? 1.0 : 0.0; }
double L_lt   (double a, double b) { return (a < b)  ? 1.0 : 0.0; }
double L_eq   (double a, double b) { return (a == b) ? 1.0 : 0.0; }
double L_ne   (double a, double b) { return (a != b) ? 1.0 : 0.0; }
double L_plus (double a, double b) { return a + b; }
double L_minus(double a, double b) { return a - b; }
double L_times(double a, double b) { return a * b; }

double L_divide(double a, double b) 
{
    if(!IODE_IS_A_NUMBER(b) || !IODE_IS_A_NUMBER(a) || b == 0)
        return IODE_NAN;
    return a / b;
}

double L_exp(double a, double b)
{
    double x;

    // a^b for a < 0 and b not an integer
    if(a < 0 && b != (int) b) 
    {
        // check if a^b can be represented as a^(1/c) where 
        // 1) c is an integer  
        double c = 1.0 / b;
        bool is_fraction = fabs(c - (int) c) < 1e-8;
        if(!is_fraction) 
            return IODE_NAN;
        // 2) c is an odd integer 
        // (ex: (-8)^(1/3) = -(8^(1/3)) = -2, but (-8)^(1/2) is not a real number)
        bool is_odd_integer = (int) c % 2 == 1;
        if(!is_odd_integer)
            return IODE_NAN;

        // a^b = (-x)^b = (-)^b * x^b = -(x^b) if 1/b is an odd integer
        x = -pow(-a, b);
    }
    else
        x = pow(a, b);
    
    if(_isnan(x)) 
        x = IODE_NAN;
    
    return x;
}

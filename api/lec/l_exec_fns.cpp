#include <math.h>
#include <time.h>

#include "api/lec/lec.h"


#ifdef __GNUC__
    #define _isnan isnan
#endif

double L_log10(const double v)
{
    double x;
    if(!IODE_IS_A_NUMBER(v) || v <= 0) 
        return IODE_NAN;

    x = log(v);
    if(_isnan(x)) 
        x = IODE_NAN;
    return x;
}

/*------------- GROUP L_FNS_FN ---------------------*/

double L_uminus(LECStack& stack, int unused)
{
    double value = stack.back();
    stack.pop_back();
    return -value;
}

double L_uplus (LECStack& stack, int unused)
{
    double value = stack.back();
    stack.pop_back();
    return value;
}

double L_log(LECStack& stack, int nargs)
{   
    if(nargs == 2)
    {
        double base = stack.back();
        stack.pop_back();
        double log_b = L_log10(base);
        double x = stack.back();
        stack.pop_back();
        double log_x = L_log10(x);
        return L_divide(log_x, log_b);
    }
    else
    {
        double value = stack.back();
        stack.pop_back();
        return L_log10(value);
    }
}

double L_ln(LECStack& stack, int unused)
{
    double value = stack.back();
    stack.pop_back();
    return L_log10(value);
}

double L_not(LECStack& stack, int unused)
{
    double value = stack.back();
    stack.pop_back();
    return (fabs(value) < 1e-15) ? 1.0 : 0.0;
}

/**
 *  Computes the exponential of the value on the stack.
 *  If nargs == 1, it's the neperian exponential
 *  If nargs == 2, the mantissa is the second argument.
 */
double L_expn(LECStack& stack, int nargs)
{
    if(nargs == 1) 
    {
        double x = stack.back();
        stack.pop_back();
        if(x >= DMAXEXP || x <= DMINEXP)
            return IODE_NAN;
        
        double res = exp(x);
        if(res >= MAXDOUBLE || res <= MINDOUBLE)
            return IODE_NAN;
        
        return res;
    }
    else 
    {
        double n = stack.back();
        stack.pop_back();
        double x = stack.back();
        stack.pop_back();
        return L_exp(x, n);
    }
}

double L_max(LECStack& stack, int nargs)
{
    double max = stack.back();
    for(int j = 0; j < nargs; j++)
    {
        if(stack.back() > max) 
            max = stack.back();
        stack.pop_back();
    }

    return max;
}

double L_min(LECStack& stack, int nargs)
{
    double min = stack.back();
    for(int j = 0; j < nargs; j++)
    {
        if(stack.back() < min) 
            min = stack.back();
        stack.pop_back();
    }

    return min;
}

double L_sin(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return sin(x);
}

double L_cos(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return cos(x);
}

double L_acos(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return acos(x);
}

double L_asin(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return asin(x);
}

double L_tan(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return tan(x);
}

double L_atan(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return atan(x);
}

double L_tanh(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return tanh(x);
}

double L_sinh(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return sinh(x);
}

double L_cosh(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return cosh(x);
}

double L_abs(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return fabs(x);
}

double L_sqrt(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return (x < 0) ? IODE_NAN: sqrt(x);
}

double L_int(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return floor(0.5 + x);
}

double L_rad(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return (x * M_PI / 180.0);
}

double L_if(LECStack& stack, int nargs)
{
    double arg2 = stack.back();
    stack.pop_back();
    double arg1 = stack.back();
    stack.pop_back();
    double cond = stack.back();
    stack.pop_back();

    if(!IODE_IS_A_NUMBER(cond) || fabs(cond) < 1e-15)  
        return arg2;
    else                                     
        return arg1;
}

double L_lsum(LECStack& stack, int nargs)
{
    double sum = 0.0;
    for(int j = 0; j < nargs; j++)
    {
        sum += stack.back();
        stack.pop_back();
    }

    return sum;
}

double L_lmean(LECStack& stack, int nargs)
{
    int nb = 0;
    double mean = 0.0;
    double value;
    for(int j = 0; j < nargs; j++) 
    {
        value = stack.back();
        stack.pop_back();
        if(IODE_IS_A_NUMBER(value)) 
        {
            mean += value;
            nb++;
        }
    }
 
    return (nb > 0) ? mean / nb : IODE_NAN;
}

double L_fnisan(LECStack& stack, int nargs)
{
    double x = stack.back();
    stack.pop_back();
    return (double) IODE_IS_A_NUMBER(x);
}

double L_lcount(LECStack& stack, int nargs)
{
    return (double) nargs;
}

double L_lprod(LECStack& stack, int nargs)
{
    double prod = 1.0;
    for(int j = 0; j < nargs; j++)
    {
        if(IODE_IS_A_NUMBER(stack.back())) 
            prod *= stack.back();
        stack.pop_back();
    }

    return prod;
}

double L_sign(LECStack& stack, int unused)
{ 
    double x = stack.back();
    stack.pop_back();
    return (x < 0) ? -1.0 : 1.0;
}

double L_lstderr(LECStack& stack, int nargs)
{
    LECStack copy = stack;
    double mean = L_lmean(copy, nargs);

    double x;
    int nb = 0;
    double sum = 0.0;
    for(int j = 0; j < nargs; j++) 
    {
        x = stack.back();
        stack.pop_back();
        if(IODE_IS_A_NUMBER(x)) 
        {
            x = x - mean;
            sum += x * x;
            nb++;
        }
    }

    return (nb > 1) ? sqrt(sum / (nb - 1)) : IODE_NAN;
}

double L_random(LECStack& stack, int unused)
{
    static bool init = false;

    double value = stack.back();
    stack.pop_back();

    if(value == 0.0) 
        return 0.0;
    
    if(!init) 
    {
        srand((unsigned) time(NULL));
        init = true;
    }

    double factor = (rand() - (double) RAND_MAX / 2) / (double) RAND_MAX;
    return factor * value;
}

double L_floor(LECStack& stack, int unused)
{   
    double x = stack.back();
    stack.pop_back();
    return floor(x); 
}

double L_ceil(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return 1.0 + floor(x);
}

double L_round(LECStack& stack, int nargs)
{
    double value = 0.0;
    int nb_digits = 0;

    if(nargs == 2) 
    {
        nb_digits = (int) stack.back();
        stack.pop_back();
        value = stack.back();
        stack.pop_back();
    }
    else
    {
        value = stack.back();
        stack.pop_back();
    }

    double shift = pow(10, nb_digits);
    double res = floor(0.5 + (value * shift));
    return res / shift;
}

/**
 * @brief urandom(max): randomly and uniformly distributed within the interval [0, max[ 
 */
double L_urandom(LECStack& stack, int unused)
{
    static bool init = false;

    double value = stack.back();
    stack.pop_back();

    if(value == 0.0) 
        return 0.0;

    if(!init) 
    {
        srand((unsigned)time(NULL));
        init = true;
    }

    return u_rand() * value;
}

static double z2 = 0.0;
static bool use_z2 = true;

// TODO: check the algorithm
static double randBoxMuller(double rv_mean, double rv_sd)
{
    double z1 = 0.0;
    if(use_z2 && z2 != 0.0) 
    {
        z1 = z2;
        z2 = 0.0;
    }
    else 
    {
        double x, y, w = 2.0;
        while(w > 1.0 || w == 0.0)
        {
            // choose x,y in uniform square (-1,-1) to (+1,+1)
            x = -1.0 + 2.0 * u_rand();
            y = -1.0 + 2.0 * u_rand();
            // see if it is in the unit circle
            w = x * x + y * y;
        }

        // Box-Muller transform
        w = sqrt(-2.0 * log(w) / w);
        z2 = x * w;
        z1 = y * w;
    }

    return rv_mean + rv_sd * z1;
}

/**
 * @brief grandom(m, sd): random variable with a normal distribution 
 * with mean 'm' and standard deviation 'sd'.
 */
double L_grandom(LECStack& stack, int unused)
{
    double sd = stack.back();
    stack.pop_back();
    double mean = stack.back();
    stack.pop_back();

    return randBoxMuller(mean, sd);
}

// Gamma function in double precision
static double dgamma(double x)
{
    int n = x < 1.5 ? -((int) (2.5 - x)) : (int) (x - 1.5);
    double w = x - (n + 2);
    std::vector<double> coeffs = {
        -1.99542863674e-7, 1.337767384067e-6, -2.591225267689e-6, -1.7545539395205e-5,
        1.45596568617526e-4, -3.60837876648255e-4, -8.04329819255744e-4, 0.008023273027855346,
        -0.017645244547851414, -0.024552490005641278, 0.19109110138763841, -0.233093736421782878,
        -0.422784335098466784
    };

    double y = 0.0;
    for(const double& coeff : coeffs)
    {
        y += coeff;
        y *= w;
    }
    y += 0.99999999999999999;

    if(n > 0) 
    {
        w = x - 1;
        for(int k = 2; k <= n; k++) 
            w *= x - k;
    }
    else 
    {
        w = 1;
        for(int k = 0; k > n; k--) 
            y *= x - k;
    }

    return w / y;
}

double L_gamma(LECStack& stack, int unused)
{
    double x = stack.back();
    stack.pop_back();
    return dgamma(x);
}

double L_div0(LECStack& stack, int unused)
{
    double b = stack.back();
    stack.pop_back();
    double a = stack.back();
    stack.pop_back();

    double res;
    if(!IODE_IS_A_NUMBER(b) || !IODE_IS_A_NUMBER(a)) 
        res = IODE_NAN;
    else if(b == 0) 
        res = 0.0;
    else
        res = a / b;

    return res;
}

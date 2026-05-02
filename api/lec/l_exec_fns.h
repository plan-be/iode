#pragma once
#include "api/lec/l_common.h"


/* LEC:FUNCTIONS */
enum LecFunction
{
    L_UMINUS = L_FN,
    L_UPLUS,
    L_LOG,
    L_LN,
    L_NOT,
    L_EXPN,
    L_MAX,
    L_MIN,
    L_SIN,
    L_COS,
    L_ACOS,
    L_ASIN,
    L_TAN,
    L_ATAN,
    L_TANH,
    L_SINH,
    L_COSH,
    L_ABS,
    L_SQRT,
    L_INT,
    L_RAD,
    L_IF,
    L_LSUM,
    L_LMEAN,
    L_FNISAN,
    L_LCOUNT,
    L_LPROD,
    L_SIGN,
    L_LSTDERR,
    L_RANDOM,
    L_FLOOR,
    L_CEIL,
    L_ROUND,
    L_URANDOM,
    L_GRANDOM,
    L_GAMMA,
    L_DIV0,
};

// min nb of functions args {uminus, uplus, log, ln, not, expn, max, min, sin, cos, acos, asin, tan, atan, tanh, sinh, cosh, 
inline int L_MIN_FARGS[]  = {     1,     1,   1,  1,   1,    1,   1,   1,   1,   1,    1,    1,   1,    1,    1,    1,    1, 
                            // abs, sqrt, int, rad, if, lsum, lmean, fnisan, lcount, lprod, sign, lstderr, random, floor, ceil, 
                                 1,    1,   1,   1,  3,    1,     1,      1,      1,     1,    1,       2,      1,     1,    1, 
                            // round, urandom, grandom, gamma, div0}
                                   1,       1,       2,     1,    2};

// max nb of functions args {uminus, uplus, log, ln, not, expn, max, min, sin, cos, acos, asin, tan, atan, tanh, sinh, cosh, 
inline int L_MAX_FARGS[]  = {     1,     1,   2,  1,   1,    2, 255, 255,   1,   1,    1,    1,   1,    1,    1,    1,    1, 
                            // abs, sqrt, int, rad, if, lsum, lmean, fnisan, lcount, lprod, sign, lstderr, random, floor, ceil, 
                                 1,    1,   1,   1,  3,  255,   255,      1,    255,   255,    1,     255,      1,     1,    1, 
                            // round, urandom, grandom, gamma, div0}
                                   2,       1,       2,     1,    2};


constexpr int L_FN_LAST = 69;

inline bool is_fn(const int op)       
{ 
    return op >= L_FN  && op <= L_FN_LAST; 
}

double L_logn(double v);

double L_uminus(double* stack, int nbargs);
double L_uplus(double* stack, int nbargs);
double L_log(double* stack, int nbargs);
double L_ln(double* stack, int nbargs);
double L_not(double* stack, int nbargs);
double L_expn(double* stack, int nargs);
double L_max(double* stack, int nbargs);
double L_min(double* stack, int nbargs);
double L_sin(double* stack, int nbargs);
double L_asin(double* stack, int nbargs);
double L_cos(double* stack, int nbargs);
double L_acos(double* stack, int nbargs);
double L_tan(double* stack, int nbargs);
double L_atan(double* stack, int nbargs);
double L_sinh(double* stack, int nbargs);
double L_cosh(double* stack, int nbargs);
double L_tanh(double* stack, int nbargs);
double L_sqrt(double* stack, int nbargs);
double L_int(double* stack, int nbargs);
double L_abs(double* stack, int nbargs);
double L_sign(double* stack, int nbargs);
double L_rad(double* stack, int nbargs);
double L_if(double* stack, int nbargs);
double L_fnisan(double* stack, int nbargs);
double L_lcount(double* stack, int nbargs);
double L_lprod(double* stack, int nbargs);
double L_lsum(double* stack, int nbargs);
double L_lstderr(double* stack, int nbargs);
double L_lmean(double* stack, int nbargs);
double L_random(double* stack, int nbargs);
double L_floor(double* stack, int nbargs);
double L_ceil (double* stack, int nbargs);
double L_round(double* stack, int nbargs);
double L_urandom(double* stack, int nbargs);
double L_grandom(double* stack, int nbargs);
double L_gamma(double* stack, int nbargs);
double L_div0(double* stack, int nbargs);

inline double(*L_FNS_FN[])(double* stack, int nbargs) = 
{ 
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

inline std::vector<std::string> L_FN_NAMES = 
{
    "uminus",       // L_UMINUS    L_FN + 0
    "uplus",        // L_UPLUS     L_FN + 1
    "log",          // L_LOG       L_FN + 2  
    "ln",           // L_LN        L_FN + 3
    "not",          // L_NOT       L_FN + 4  
    "expn",         // L_EXPN      L_FN + 5   
    "max",          // L_MAX       L_FN + 6
    "min",          // L_MIN       L_FN + 7  
    "sin",          // L_SIN       L_FN + 8  
    "cos",          // L_COS       L_FN + 9
    "acos",         // L_ACOS      L_FN + 10   
    "asin",         // L_ASIN      L_FN + 11   
    "tan",          // L_TAN       L_FN + 12
    "atan",         // L_ATAN      L_FN + 13   
    "tanh",         // L_TANH      L_FN + 14   
    "sinh",         // L_SINH      L_FN + 15
    "cosh",         // L_COSH      L_FN + 16   
    "abs",          // L_ABS       L_FN + 17  
    "sqrt",         // L_SQRT      L_FN + 18
    "int",          // L_INT       L_FN + 19  
    "rad",          // L_RAD       L_FN + 20  
    "if",           // L_IF        L_FN + 21
    "lsum",         // L_LSUM      L_FN + 22   
    "lmean",        // L_LMEAN     L_FN + 23    
    "fnisan",       // L_FNISAN    L_FN + 24
    "lcount",       // L_LCOUNT    L_FN + 25     
    "lprod",        // L_LPROD     L_FN + 26    
    "sign",         // L_SIGN      L_FN + 27
    "lstderr",      // L_LSTDERR   L_FN + 28     
    "random",       // L_RANDOM    L_FN + 29
    "floor",        // L_FLOOR     L_FN + 30
    "ceil",         // L_CEIL      L_FN + 31
    "round",        // L_ROUND     L_FN + 32
    "urandom",      // L_URANDOM   L_FN + 33
    "grandom",      // L_GRANDOM   L_FN + 34
    "gamma",        // L_GAMMA     L_FN + 35
    "div0"          // L_DIV0      L_FN + 36
};

struct LEC_FN: public LEC_EXECUTABLE
{
public:
    LEC_FN(const int type, const int nb_args) : LEC_EXECUTABLE(type, nb_args)
    {
        if(!is_fn(type))
            throw std::invalid_argument("Invalid function type for LEC FUNC: " + std::to_string(type));
        pos = type - L_FN;
        representation = L_FN_NAMES[pos];
    }

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_FN(const unsigned char* buffer, int& pos_buffer) : LEC_EXECUTABLE(L_FN, 0)
    {
        type = (int) buffer[pos_buffer];
        pos_buffer++;
        nb_args = (int) buffer[pos_buffer];
        pos_buffer++;
        pos = type - L_FN;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);
        buffer[pos_buffer] = nb_args;
        pos_buffer++;
    }

    int get_length() const override 
    {
        return 2;   // 1 byte for type and 1 byte for nb_args
    }

    // executes the function with the given arguments on the stack
    void execute(double* stack, int& pos_stack)
     {
        // some functions (e.g. L_IF) can accept NaN as argument without returning NaN
        bool fn_accept_nan = (type == L_IF || type == L_FNISAN || type == L_LMEAN || 
                              type == L_LSTDERR || type == L_LCOUNT);

        int shift = nb_args - 1; 
        if(!fn_accept_nan && L_stack_is_nan(stack, pos_stack, nb_args))
            stack[pos_stack - shift] = IODE_NAN;
        else
            stack[pos_stack - shift] = (L_FNS_FN[pos])(stack + pos_stack, nb_args);
        
        pos_stack -= shift;
     }
};

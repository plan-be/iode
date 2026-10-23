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

inline double u_rand()
{
    return ((double) rand() / (1.0 + (double) RAND_MAX));
}

double L_log10(const double v);

double L_uminus(LECStack& stack, int nbargs);
double L_uplus(LECStack& stack, int nbargs);
double L_log(LECStack& stack, int nbargs);
double L_ln(LECStack& stack, int nbargs);
double L_not(LECStack& stack, int nbargs);
double L_expn(LECStack& stack, int nargs);
double L_max(LECStack& stack, int nbargs);
double L_min(LECStack& stack, int nbargs);
double L_sin(LECStack& stack, int nbargs);
double L_asin(LECStack& stack, int nbargs);
double L_cos(LECStack& stack, int nbargs);
double L_acos(LECStack& stack, int nbargs);
double L_tan(LECStack& stack, int nbargs);
double L_atan(LECStack& stack, int nbargs);
double L_sinh(LECStack& stack, int nbargs);
double L_cosh(LECStack& stack, int nbargs);
double L_tanh(LECStack& stack, int nbargs);
double L_sqrt(LECStack& stack, int nbargs);
double L_int(LECStack& stack, int nbargs);
double L_abs(LECStack& stack, int nbargs);
double L_sign(LECStack& stack, int nbargs);
double L_rad(LECStack& stack, int nbargs);
double L_if(LECStack& stack, int nbargs);
double L_fnisan(LECStack& stack, int nbargs);
double L_lcount(LECStack& stack, int nbargs);
double L_lprod(LECStack& stack, int nbargs);
double L_lsum(LECStack& stack, int nbargs);
double L_lstderr(LECStack& stack, int nbargs);
double L_lmean(LECStack& stack, int nbargs);
double L_random(LECStack& stack, int nbargs);
double L_floor(LECStack& stack, int nbargs);
double L_ceil (LECStack& stack, int nbargs);
double L_round(LECStack& stack, int nbargs);
double L_urandom(LECStack& stack, int nbargs);
double L_grandom(LECStack& stack, int nbargs);
double L_gamma(LECStack& stack, int nbargs);
double L_div0(LECStack& stack, int nbargs);

inline double(*L_FNS_FN[])(LECStack& stack, int nbargs) =
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

struct LEC_FN: public LEC_ABSTRACT_EXECUTABLE
{
public:
    LEC_FN(const int type, const int nb_args) : LEC_ABSTRACT_EXECUTABLE(type, nb_args)
    {
        if(!is_fn(type))
            throw std::invalid_argument("Invalid function type for LEC FUNC: " + std::to_string(type));
        pos = type - L_FN;
        fn_name = L_FN_NAMES[pos];
        representation = fn_name;
    }

    LEC_FN(const LEC_FN& other) = default;

    ~LEC_FN() override = default;

    std::unique_ptr<LEC_ABSTRACT> clone() const override 
    { 
        return std::make_unique<LEC_FN>(*this); 
    }

    bool equals(const LEC_ABSTRACT& other) const override
    {
        const auto* rhs = dynamic_cast<const LEC_FN*>(&other);
        return rhs && *this == *rhs;
    }
    
    bool operator==(const LEC_FN& other) const
    {
        return is_same_type(other);
    }

    /**
     *  Checks if at least one active value on the stack is IODE_NAN.
     *
     *  @param [in] stack       execution stack
     *  @return bool            true if an active value is IODE_NAN, false otherwise
     */
    bool check_nan_in_stack(const LECStack& stack) const
    {
        for(int shift = 0; shift < nb_args; shift++)
        {
            if(stack.is_nan(shift))
                return true;
        }
    
        return false;
    }

    // executes the function with the given arguments on the stack
    double execute(LECStack& stack, int t, int& expr_pos, AbstractCLEC& clec) const override
     {
        // some functions (e.g. L_IF) can accept NaN as argument without returning NaN
        bool fn_accept_nan = (type == L_IF || type == L_FNISAN || type == L_LMEAN || 
                              type == L_LSTDERR || type == L_LCOUNT);

        double result;
        if(!fn_accept_nan && check_nan_in_stack(stack))
        {
            for(int i = 0; i < nb_args; i++)
                stack.pop_back();
            result = IODE_NAN;
        }
        else
            result = (L_FNS_FN[pos])(stack, nb_args);
        
        return result;
     }
};

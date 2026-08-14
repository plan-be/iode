
#pragma once
#include "api/lec/l_exec_tfn.h"

#ifndef M_E
    #define M_E         2.71828182845904523536
    #define M_PI        3.14159265358979323846
#endif


constexpr int L_VAL = L_TFN_LAST + 1;

/* LEC: CONST VALUES */
enum LecConstValue
{
    L_PI = L_VAL,
    L_E,
    L_TIME,
    L_I,
    L_EURO
};

constexpr int L_VAL_LAST = 139;

inline bool is_val(const int op)      
{ 
    return op >= L_VAL && op <= L_VAL_LAST; 
}


double L_pi(int t, int exec_t);
double L_euro(int t, int exec_t);
double L_e(int t, int exec_t);
double L_time(int t, int exec_t);
double L_i(int t, int exec_t);

// l_exec_val.c
inline double(*L_VAL_FN[])(int t, int exec_t) = 
{ 
    L_pi,           // L_PI        L_VAL + 0
    L_e,            // L_E         L_VAL + 1
    L_time,         // L_TIME      L_VAL + 2
    L_i,            // L_I         L_VAL + 3
    L_euro          // L_EURO      L_VAL + 4
};

inline std::vector<std::string> L_VAL_FN_NAMES = 
{
    "pi",           // L_PI        L_VAL + 0
    "e",            // L_E         L_VAL + 1
    "time",         // L_TIME      L_VAL + 2
    "i",            // L_I         L_VAL + 3
    "euro"          // L_EURO      L_VAL + 4
};

struct LEC_VAL_FN: public TP_LEC_EXECUTABLE<int, int>
{
public:
    LEC_VAL_FN(const int type) : TP_LEC_EXECUTABLE<int, int>(type, 1)
    {
        if(!is_val(type))
            throw std::invalid_argument("Invalid operator type for LEC VALUE FUNCTION: " + std::to_string(type));
        pos = type - L_VAL;
        fn_name = L_VAL_FN_NAMES[pos];
        representation = fn_name;
    }

    LEC_VAL_FN(const LEC_VAL_FN& other) = default;

    bool operator==(const LEC_VAL_FN& other) const
    {
        return is_same_type(other);
    }

    // executes the function with the given arguments on the stack
    void execute(std::deque<double>& stack, int t, int exec_t) override
    {
        double value = (L_VAL_FN[pos])(t, exec_t);
        stack.push_back(value);
    }
};

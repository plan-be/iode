#pragma once
#include "api/lec/l_exec_fns.h"

constexpr int L_OP = L_FN_LAST + 1;

/* LEC:OPERATORS */
enum LecOperator
{
    L_OR = L_OP,    // logical OR
    L_AND,          // logical AND
    L_GE,           // greater or equal
    L_GT,           // greater than
    L_LE,           // less or equal
    L_LT,           // less than
    L_EQ,           // equal
    L_NE,           // not equal
    L_PLUS,         // addition '+'
    L_MINUS,        // subtraction '-'
    L_DIVIDE,       // division '/'
    L_TIMES,        // multiplication '*'
    L_EXP           // exponentiation '^'
};

// PRIORITY OF OPERATORS {or, and, ge, gt, le, lt, eq, ne, plus, minus, divide, times, exp}
inline int L_PRIOR[13] = { 2,   3,  4,  4,  4,  4,  4,  4,    5,     5,      6,     6,   7};

constexpr int L_OP_LAST = 99;

inline bool is_op(const int op)       
{ 
    return op >= L_OP  && op <= L_OP_LAST; 
}

double L_or(double a, double b);
double L_and(double a, double b);
double L_ge(double a, double b);
double L_gt(double a, double b);
double L_eq(double a, double b);
double L_ne(double a, double b);
double L_le(double a, double b);
double L_lt(double a, double b);
double L_plus(double a, double b);
double L_minus(double a, double b);
double L_divide(double a, double b);
double L_times(double a, double b);
double L_exp(double a, double b);

inline double(*L_OPS_FN[])(double val1, double val2) = 
{ 
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

inline std::vector<std::string> L_OPS_NAMES = 
{
    "or",           // L_OR        L_OP + 0
    "and",          // L_AND       L_OP + 1 
    ">=",           // L_GE        L_OP + 2
    ">",            // L_GT        L_OP + 3
    "<=",           // L_LE        L_OP + 4
    "<",            // L_LT        L_OP + 5
    "==",           // L_EQ        L_OP + 6
    "!=",           // L_NE        L_OP + 7
    "+",            // L_PLUS      L_OP + 8
    "-",            // L_MINUS     L_OP + 9   
    "/",            // L_DIVIDE    L_OP + 10    
    "*",            // L_TIMES     L_OP + 11
    "^"             // L_EXP       L_OP + 12
};

struct LEC_OP: public LEC_EXECUTABLE
{
public:
    LEC_OP(const int type) : LEC_EXECUTABLE(type, 2)
    {
        if(!is_op(type))
            throw std::invalid_argument("Invalid operator type for LEC OPERATOR: " + std::to_string(type));
        pos = type - L_OP;
        fn_name = L_OPS_NAMES[pos];
    }

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_OP(const unsigned char* buffer, int& pos_buffer) : LEC_EXECUTABLE(L_OP, 2)
    {
        type = (int) buffer[pos_buffer];
        pos_buffer++;
        pos = type - L_OP;
        representation = L_OPS_NAMES[pos];
    }

    int get_length() const override 
    {
        return 1;
    }

    // executes the operator with the given arguments on the stack
    void execute(unsigned char* expr, int j, int t, std::deque<double>& stack) override
    {
        double b = stack.back();
        stack.pop_back();
        double a = stack.back();
        stack.pop_back();

        double result;
        if(!IODE_IS_A_NUMBER(a) || !IODE_IS_A_NUMBER(b))
            result = IODE_NAN;
        else
            result = (L_OPS_FN[pos])(a, b);
        stack.push_back(result);
    }
};

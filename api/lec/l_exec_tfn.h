#pragma once
#include "api/lec/l_exec_ops.h"

constexpr int L_TFN = L_OP_LAST + 1;

/* LEC:TFN (time related functions) */
enum LecTimeFunction
{
    L_LAG = L_TFN,      // l([n,] expr)
    L_DIFF,             // d([n,] expr)
    L_RAPP,             // r([n,] expr)
    L_DLN,              // dln([n,] expr)
    L_GRT,              // grt([n,] expr)
    L_MAVG,             // ma([n,] expr)
    L_VMAX,             // vmax([from, [to,]] expr)
    L_VMIN,             // vmin([from, [to,]] expr)
    L_SUM,              // sum([from, [to,]] expr)
    L_PROD,             // prod([from, [to,]] expr)
    L_MEAN,             // mean([from, [to,]] expr)
    L_STDERR,           // stderr([from, [to,]] expr)
    L_DLAG,             // dlag(n, coef, expr)
    L_LASTOBS           // lastobs([from, [to,]] expr)
};

// min nb of functions args {lag, diff, rapp, dln, grt, mavg, vmax, vmin, sum, prod, mean, stderr, dlag, lastobs}
inline int L_MIN_TARGS[]  = {  1,    1,    1,   1,   1,    1,    1,    1,   1,    1,    1,      1,    0,       1};

// max nb of functions args {lag, diff, rapp, dln, grt, mavg, vmax, vmin, sum, prod, mean, stderr, dlag, lastobs}
inline int L_MAX_TARGS[]  = {  2,    2,    2,   2,   2,    2,    3,    3,   3,    3,    3,      3,    0,       3};

constexpr int L_TFN_LAST = 119;

inline bool is_tfn(const int op)      
{ 
    return op >= L_TFN && op <= L_TFN_LAST; 
}

void L_extract_time_range(int t, std::deque<double>& stack, int nargs, int& from, int& to);

double L_lag(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_diff(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_rapp(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_dln(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_grt(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_mavg(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_vmax(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_vmin(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_sum(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_prod(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_mean(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_stderr(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_lastobs(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs);

inline double(*L_TFN_FN[])(AbstractCLEC& clec, const int expr_pos, const int length, int from, int to, int t, std::deque<double>& stack, int nargs) = 
{ 
    L_lag,          // L_LAG       L_TFN + 0 
    L_diff,         // L_DIFF      L_TFN + 1 
    L_rapp,         // L_RAPP      L_TFN + 2 
    L_dln,          // L_DLN       L_TFN + 3 
    L_grt,          // L_GRT       L_TFN + 4 
    L_mavg,         // L_MAVG      L_TFN + 5 
    L_vmax,         // L_VMAX      L_TFN + 6 
    L_vmin,         // L_VMIN      L_TFN + 7 
    L_sum,          // L_SUM       L_TFN + 8 
    L_prod,         // L_PROD      L_TFN + 9 
    L_mean,         // L_MEAN      L_TFN + 10
    L_stderr,       // L_STDERR    L_TFN + 11
    0,              // L_DLAG      L_TFN + 12
    L_lastobs       // L_LASTOBS   L_TFN + 13 
};

inline std::vector<std::string> L_TFN_NAMES = 
{ 
    "lag",          // L_LAG       L_TFN + 0 
    "diff",         // L_DIFF      L_TFN + 1 
    "rapp",         // L_RAPP      L_TFN + 2 
    "dln",          // L_DLN       L_TFN + 3 
    "grt",          // L_GRT       L_TFN + 4 
    "mavg",         // L_MAVG      L_TFN + 5 
    "vmax",         // L_VMAX      L_TFN + 6 
    "vmin",         // L_VMIN      L_TFN + 7 
    "sum",          // L_SUM       L_TFN + 8 
    "prod",         // L_PROD      L_TFN + 9 
    "mean",         // L_MEAN      L_TFN + 10
    "stderr",       // L_STDERR    L_TFN + 11
    "dlag",         // L_DLAG      L_TFN + 12
    "lastobs"       // L_LASTOBS   L_TFN + 13 
};

struct LEC_TFN: public TP_LEC_EXECUTABLE<int, int&, AbstractCLEC&>
{
    // length as number of atomic lec elements of the sub-expression argument of the function
    int length_expr = 0;
    
public:
    LEC_TFN(const int type, const int nb_args) 
        : TP_LEC_EXECUTABLE<int, int&, AbstractCLEC&>(type, nb_args)
    {
        if(!is_tfn(type))
            throw std::invalid_argument("Invalid time function type for LEC TIME FUNC: " + std::to_string(type));
        pos = type - L_TFN;
        fn_name = L_TFN_NAMES[pos];
        representation = fn_name;
    }

    LEC_TFN(const LEC_TFN& other) = default;

    bool operator==(const LEC_TFN& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->length_expr != other.length_expr)
            return false;

        return true;
    }

    // executes the function with the given arguments on the stack
    // NOTE: expr_pos is the position of the first atomic lec element of the sub expression
    void execute(std::deque<double>& stack, int t, int& expr_pos, AbstractCLEC& clec) override
    {
        int from = -1, to = -1;
        // NOTE: for time functions:
        //       - If nargs = 1, 'from' is the beginning of the KDB sample and 'to'=t
        //       - If nargs = 2, 'from' is on the stack and 'to'=t
        //       - If nargs = 3, 'from' and 'to' are on the stack
        if(type == L_VMAX || type == L_VMIN || type == L_SUM || type == L_PROD || type == L_MEAN || 
           type == L_STDERR || type == L_LASTOBS)
            L_extract_time_range(t, stack, nb_args, from, to);

        double result = (L_TFN_FN[pos])(clec, expr_pos, length_expr, from, to, t, stack, nb_args);
        stack.push_back(result);

        // update the position of the expression to the next atomic lec 
        // at the end of the function's argument
        expr_pos += length_expr;
    }
};

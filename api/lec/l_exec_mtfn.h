#pragma once
#include "api/lec/l_exec_val.h"


constexpr int L_MTFN = L_VAL_LAST + 1;

/* LEC:MTFN (multiple LEC sub-expr possible in argument list) */
enum LecMultiTimeFunction
{
    L_CORR = L_MTFN,    // corr([from, [to,]] expr, expr)
    L_COVAR,            // covar([from, [to,]] expr, expr)
    L_COVAR0,           // covar0([from, [to,]] expr, expr)
    L_VARIANCE,         // var([from, [to,]] expr)
    L_STDDEV,           // stddev([from, [to,]] expr)
    L_INDEX,            // index([from, [to,]] expr, expr)
    L_ACF,              // acf([from, [to,]] expr, expr)
    L_INTERPOL,         // mkval(expr)
    L_APP,              // mkval(expr, app)
    L_HP,               // hp(([from, [to,]] lambda, expr)
    L_DAPP,             // hp(([from, [to,]] lambda, expr)
    L_HPSTD             // hpstd(([from, [to,]] lambda, expr)
};

// min nb of functions args {corr, covar, covar0, var, stddev, index, acf, interpol, app, hp, dapp, hpstd}
inline int L_MIN_MTARGS[] = {   2,     2,      2,   1,      1,     2,   2,        1,   2,  2,    2,     2};

// max nb of functions args {corr, covar, covar0, var, stddev, index, acf, interpol, app, hp, dapp, hpstd}
inline int L_MAX_MTARGS[] = {   4,     4,      4,   3,      3,     4,   4,        1,   2,  4,    2,     4};

inline bool is_mtfn(const int op)     
{ 
    return op >= L_MTFN; 
}

int L_calcvals(AbstractCLEC& clec, int start, int end, int t, std::deque<double>& stack, int* p_nargs, double* res, int nbvals);
double L_calccorr(AbstractCLEC& clec, const int pos1, const int length1, const int pos2, const int length2, int from, int to, int t, 
    std::deque<double>& stack, int nargs);
double L_calccovar(AbstractCLEC& clec, const int pos1, const int length1, const int pos2, const int length2, int from, int to, int t, 
    std::deque<double>& stack, int nargs, int center);
double L_hpall(AbstractCLEC& clec, const int pos1, const int length1, const int pos2, const int length2, int from, int to, int t, 
    std::deque<double>& stack, int nargs, int std);

double L_corr(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_covar(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_covar0(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_var(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_stddev(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_index(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_acf(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_interpol(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_app(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_hp(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_dapp(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_hpstd(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs);

inline double(*L_MTFN_FN[])(AbstractCLEC& clec, const int expr_pos, const std::deque<int>& v_length, int from, int to, int t, std::deque<double>& stack, int nargs) = 
{ 
    L_corr,         // L_CORR      L_M
    L_covar,        // L_COVAR     L_M
    L_covar0,       // L_COVAR0    L_M
    L_var,          // L_VARIANCE  L_M
    L_stddev,       // L_STDDEV    L_M
    L_index,        // L_INDEX     L_M
    L_acf,          // L_ACF       L_M
    L_interpol,     // L_INTERPOL  L_M
    L_app,          // L_APP       L_M
    L_hp,           // L_HP        L_M
    L_dapp,         // L_DAPP      L_M
    L_hpstd         // L_HPSTD     L_M
};

inline std::vector<std::string> L_MTFN_NAMES = 
{ 
    "corr",         // L_CORR      L_M
    "covar",        // L_COVAR     L_M
    "covar0",       // L_COVAR0    L_M
    "var",          // L_VARIANCE  L_M
    "stddev",       // L_STDDEV    L_M
    "index",        // L_INDEX     L_M
    "acf",          // L_ACF       L_M
    "interpol",     // L_INTERPOL  L_M
    "app",          // L_APP       L_M
    "hp",           // L_HP        L_M
    "dapp",         // L_DAPP      L_M
    "hpstd"         // L_HPSTD     L_M
};

struct LEC_MTFN: public TP_LEC_EXECUTABLE<int, int&, AbstractCLEC&>
{
    // number of variables arguments (i.e. variadic arguments)
    int nv_args;

    // vector of lengths as number of atomic lec elements for each sub-expression 
    // argument of the function
    std::deque<int> v_length_expr;
    
public:
    LEC_MTFN(const int type, const int nb_args, const int nv_args) 
        : TP_LEC_EXECUTABLE<int, int&, AbstractCLEC&>(type, nb_args), nv_args(nv_args)
    {
        if(!is_mtfn(type))
            throw std::invalid_argument("Invalid multi-time function type for LEC MULTI-TIME FUNC: " + std::to_string(type));
        
        pos = type - L_MTFN;
        fn_name = L_MTFN_NAMES[pos];
        representation = fn_name;
        v_length_expr.resize(nv_args, 0);
    }

    LEC_MTFN(const LEC_MTFN& other) = default;

    bool operator==(const LEC_MTFN& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->nv_args != other.nv_args)
            return false;

        if(this->v_length_expr != other.v_length_expr)
            return false;

        return true;
    }

    // executes the function with the given arguments on the stack
    // NOTE: expr_pos is the position of the first atomic lec element of the first sub expression
    void execute(std::deque<double>& stack, int t, int& expr_pos, AbstractCLEC& clec) override
    {
        int from = -1, to = -1;
        // NOTE: in function L_extract_time_range():
        //       - If nargs = 1, 'from' is the beginning of the KDB sample and 'to'=t
        //       - If nargs = 2, 'from' is on the stack and 'to'=t
        //       - If nargs = 3, 'from' and 'to' are on the stack
        if(type != L_INTERPOL && type != L_APP)
            L_extract_time_range(t, stack, nb_args - 1, from, to);

        double result = (L_MTFN_FN[pos])(clec, expr_pos, v_length_expr, from, to, t, stack, nb_args);
        stack.push_back(result);

        // update the position of the expression to the next atomic lec 
        // at the end of the function's arguments
        for(const int& length_expr : v_length_expr)
            expr_pos += length_expr;
    }
};

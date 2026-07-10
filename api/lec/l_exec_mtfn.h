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

int L_calcvals(unsigned char* expr, short lg, int t, std::deque<double>& stack, int* p_nargs, double* res, int nbvals);
double L_calccorr(unsigned char* expr1, short lg1, unsigned char* expr2, short lg2, int from, int to, int t, 
    std::deque<double>& stack, int nargs);
double L_calccovar(unsigned char* expr1, short lg1, unsigned char* expr2, short lg2, int from, int to, int t, 
    std::deque<double>& stack, int nargs, int center);

double L_corr(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_covar(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_covar0(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_var(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_stddev(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_index(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_acf(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_interpol(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_app(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_hp(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_dapp(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);
double L_hpstd(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs);

inline double(*L_MTFN_FN[])(unsigned char* expr, short nvargs, int from, int to, int t, std::deque<double>& stack, int nargs) = 
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

struct LEC_MTFN: public LEC_EXECUTABLE
{
    int nv_args;        // number of variables arguments (i.e. variadic arguments)
    short len_args;     // length in bytes of the function arguments (i.e. the sub-expression in the buffer)
    std::vector<short> v_len_args; // length in bytes of each function argument (i.e. the sub-expression in the buffer)

public:
    LEC_MTFN(const int type, const int nb_args, const int nv_args) 
        : LEC_EXECUTABLE(type, nb_args), nv_args(nv_args)
    {
        if(!is_mtfn(type))
            throw std::invalid_argument("Invalid multi-time function type for LEC MULTI-TIME FUNC: " + std::to_string(type));
        pos = type - L_MTFN;
        fn_name = L_MTFN_NAMES[pos];
        // minimum length in bytes of the function arguments (i.e. the sub-expression in the buffer)
        len_args = (short) (sizeof(short) * L_MIN_MTARGS[pos]);
    }

    LEC_MTFN(const LEC_MTFN& other) = default;

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_MTFN(const unsigned char* buffer, int& pos_buffer) : LEC_EXECUTABLE(L_MTFN, 0), nv_args(0)
    {
        type = (int) buffer[pos_buffer];
        pos_buffer++;
        nb_args = (int) buffer[pos_buffer];
        pos_buffer++;
        nv_args = (int) buffer[pos_buffer];
        pos_buffer++;
        memcpy(&len_args, buffer + pos_buffer, sizeof(short));  
        pos_buffer += sizeof(short) + len_args;      

        pos = type - L_MTFN;
        representation = L_MTFN_NAMES[pos];
    }

    bool operator==(const LEC_MTFN& other) const
    {
        if(!is_same_type(other))
            return false;
        
        if(this->nv_args != other.nv_args)
            return false;

        if(this->len_args != other.len_args)
            return false;

        return true;
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);

        // move function variadic arguments in the buffer to put type, nb_args 
        // and nv_args before them after the loop
        long tot_len = 0;
        unsigned char* tmp = nullptr;
        for(int j = 0; j < nv_args; j++) 
        {
            long len = v_len_args[j];
            tot_len += len;
            // a) go 'len + 1' bytes backward
            int new_pos_buffer = pos_buffer - (tot_len + 1);
            tmp = buffer + new_pos_buffer;
            // b) move tmp[0:len] to tmp[shift:shift+sub_len] 
            int shift = 3 + (nv_args - j + 1) * sizeof(short);
            for(int i = len - 1; i >= 0; i--)
                tmp[i + shift] = tmp[i];
            // save the length in bytes of the function ?? arguments
            memcpy(tmp + shift - sizeof(short), &len, sizeof(short));
        }

        tmp[0] = type;
        tmp[1] = nb_args;
        tmp[2] = nv_args;
        tot_len += nv_args * sizeof(short);
        memcpy(tmp + 3, &tot_len, sizeof(short));

        pos_buffer += 2 + (1 + nv_args) * sizeof(short);
    }

    int get_length() const override 
    {
        // 3 bytes for type, nb_args and nv_args + sizeof(short) for len_args + 
        // len_args for the arguments
        return 3 + sizeof(short) + len_args;
    }

    // executes the function with the given arguments on the stack
    void execute(unsigned char* expr, int j, int t, std::deque<double>& stack) override
    {
        int from = -1, to = -1;
        // NOTE: in function L_extract_time_range():
        //       - If nargs = 1, 'from' is the beginning of the KDB sample and 'to'=t
        //       - If nargs = 2, 'from' is on the stack and 'to'=t
        //       - If nargs = 3, 'from' and 'to' are on the stack
        if(type != L_INTERPOL && type != L_APP)
            L_extract_time_range(t, stack, nb_args - 1, from, to);

        // NOTE: 'j + 3 + sizeof(short)' corresponds to the position of the sub-expression
        //       in the buffer (after type, nb_args, nv_args and len_args)
        j += 3 + sizeof(short);
        double result = (L_MTFN_FN[pos])(expr + j, nv_args, from, to, t, stack, nb_args);
        stack.push_back(result);
    }
};

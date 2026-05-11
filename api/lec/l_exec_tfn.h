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
    L_VMAX,             // vmax([n,[m,]] expr)
    L_VMIN,             // vmin([n,[m,]] expr)
    L_SUM,              // sum([n,[m,]] expr)
    L_PROD,             // prod([n,[m,]] expr)
    L_MEAN,             // mean([n,[m,]] expr)
    L_STDERR,           // stderr([n,[m,]] expr)
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

void L_tfn_args(int nvargs, double* stack, int nargs, int* from, int* to);

double L_lag(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_diff(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_rapp(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_dln(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_grt(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_mavg(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_vmax(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_vmin(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_sum(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_prod(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_mean(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_stderr(unsigned char* expr, short lg, int t, double* stack, int nargs);
double L_lastobs(unsigned char* expr, short lg, int t, double* stack, int nargs);

inline double(*L_TFN_FN[])(unsigned char* expr, short length, int t, double *stack, int nargs) = 
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

struct LEC_TFN: public LEC_EXECUTABLE
{
    short len_args;    // length in bytes of the function arguments (i.e. the sub-expression in the buffer)
     
public:
    LEC_TFN(const int type, const int nb_args) : LEC_EXECUTABLE(type, nb_args), len_args(0)
    {
        if(!is_tfn(type))
            throw std::invalid_argument("Invalid time function type for LEC TIME FUNC: " + std::to_string(type));
        pos = type - L_TFN;
        representation = L_TFN_NAMES[pos];
    }

    // extract from the buffer starting at pos_buffer and update pos_buffer
    LEC_TFN(const unsigned char* buffer, int& pos_buffer) : LEC_EXECUTABLE(L_TFN, 0)
    {
        type = (int) buffer[pos_buffer];
        pos_buffer++;
        nb_args = (int) buffer[pos_buffer];
        pos_buffer++;
        memcpy(&len_args, buffer + pos_buffer, sizeof(short));
        pos_buffer += sizeof(short) + len_args; 

        pos = type - L_TFN;
        representation = L_TFN_NAMES[pos];
    }

    void link_sample_expr(KDBVariablesPtr dbv, unsigned char* buffer, int pos_buffer)
    {
        // NOTE: 'j + 2 + sizeof(short)' corresponds to the position of the sub-expression 
        //       in the buffer (after type, nb_args and len_args)
        pos_buffer += 2 + sizeof(short);
        L_link_sample_expr(dbv, buffer + pos_buffer , len_args);
    }

    void add_to_buffer(unsigned char* buffer, int& pos_buffer) const override
    {
        LEC_ABSTRACT::add_to_buffer(buffer, pos_buffer);

        // move function arguments in the buffer to put type and nb of args before them
        // a) go 'len_args + 1' bytes backward
        int new_pos_buffer = pos_buffer - (len_args + 1);
        unsigned char* tmp = buffer + new_pos_buffer;
        // b) move tmp[0:len_args] to tmp[shift:shift+len_args] 
        int shift = 2 + sizeof(short);
        for(int i = len_args - 1; i >= 0; i--)
            tmp[i + shift] = tmp[i];

        tmp[0] = type;
        tmp[1] = nb_args;
        memcpy(tmp + 2, &len_args, sizeof(short));

        pos_buffer += sizeof(short) + 1;
    }

    int get_length() const override 
    {
        // 2 for type and nb_args + sizeof(short) for len_args + 
        // len_args for the arguments
        return 2 + sizeof(short) + len_args;
    }

    // executes the function with the given arguments on the stack
    void execute(unsigned char* expr, int j, int t, double* stack, int& pos_stack)
    {
        // NOTE: 'j + 2 + sizeof(short)' corresponds to the position of the sub-expression 
        //       in the buffer (after type, nb_args and len_args)
        j += 2 + sizeof(short);

        // QUESTION: why '- 2' ?
        int shift = nb_args - 2;
        stack[pos_stack - shift] = (L_TFN_FN[pos])(expr + j, len_args, t, stack + pos_stack, nb_args);
        pos_stack -= shift;
    }
};

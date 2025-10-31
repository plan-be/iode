#pragma once

#include "scr4/s_yy.h"          // YYKEYS

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/utils/utils.h"

/* ---------------------- DEFINE ---------------------- */

#ifndef M_E
#define M_E         2.71828182845904523536
#define M_PI        3.14159265358979323846
#endif

#define L_REAL  double
#define LECREAL float

#define L_MAX_NAME  K_MAX_NAME
#define L_MAX_EXPR  512
#define L_MAX_STACK 512
#define L_MAX_NAMES 100
#define L_MAX_COEFS 999

/* ---------------------- ENUMS ---------------------- */

enum LecError
{
    L_SYNTAX_ERR = 1,
    L_PAR_ERR,
    L_ARGS_ERR,
    L_STACK_ERR,
    L_MEMORY_ERR,
    L_Period_ERR,
    L_LAG_ERR,
    L_PeriodY_ERR,
    L_BOUNDS_ERR,
    L_LINK_ERR,
    L_DIVIDE_ERR,
    L_LOG_ERR,
    L_COEF_ERR,
    L_ASSIGN_ERR,
    L_DUP_ERR,
    L_INVERT_ERR,
    L_NOT_FOUND_ERR,
    L_MACRO_ERR
};

/* LEC:SPECIAL */
enum LecSpecial
{
    L_EOE = 10,
    L_OPENP,
    L_CLOSEP,
    L_VAR,
    L_COEF,
    L_OPENB,
    L_CLOSEB,
    L_COMMA,
    L_OCPAR,  /* () */
    L_Period,
    L_VART,   /* VARIABLE [time] */
    L_COLON,
    L_LCONST,
    L_DCONST,
    L_SQUARE
};

#define L_FN        30

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

#define L_OP        70

/* LEC:OPERATORS */
enum LecOperator
{
    L_OR = L_OP,
    L_AND,
    L_GE,
    L_GT,
    L_LE,
    L_LT,
    L_EQ,
    L_NE,
    L_PLUS,
    L_MINUS,
    L_DIVIDE,
    L_TIMES,
    L_EXP   
};

#define L_TFN       100

/* LEC:TFN (time related functions) */
enum LecTimeFunction
{
    L_LAG = L_TFN,      /* l    ([n,] expr) */
    L_DIFF,             /* d    ([n,] expr) */
    L_RAPP,             /* r    ([n,] expr) */
    L_DLN,              /* dln  ([n,] expr) */
    L_GRT,              /* grt  ([n,] expr) */
    L_MAVG,             /* ma   ([n,] expr) */
    L_VMAX,             /* vmax ([n,[m,]] expr) */
    L_VMIN,             /* vmin ([n,[m,]] expr) */
    L_SUM,              /* sum  ([n,[m,]] expr) */
    L_PROD,             /* prod ([n,[m,]] expr) */
    L_MEAN,             /* mean ([n,[m,]] expr) */
    L_STDERR,           /* stderr([n,[m,]] expr) */
    L_DLAG,             /* dlag (n, coef, expr) */ /* !! not implemented */
    L_LASTOBS           /* lastobs([from, [to,]] expr) */
};

#define L_VAL       120

/* LEC: CONST VALUES */
enum LecConstValue
{
    L_PI = L_VAL,
    L_E,
    L_TIME,
    L_I,
    L_EURO
};

#define L_MTFN      140

/* LEC:MTFN (multiple LEC sub-expr possible in argument list) */
enum LecMultiTimeFunction
{
    L_CORR = L_MTFN,    /* corr([n, [m,]] expr, expr) */
    L_COVAR,            /* covar([n, [m,]] expr, expr) */
    L_COVAR0,           /* covar0([n, [m,]] expr, expr) */
    L_VARIANCE,         /* var([n, [m,]] expr) */
    L_STDDEV,           /* stddev([n, [m,]] expr) */
    L_INDEX,            /* index([from, [to,]] expr, expr) */
    L_ACF,              /* acf([from, [to,]] expr, expr) */
    L_INTERPOL,         /* mkval(expr) */
    L_APP,              /* mkval(expr, app) */
    L_HP,               /* hp(([from, [to,]] lambda, expr) */
    L_DAPP,             /* hp(([from, [to,]] lambda, expr) */
    L_HPSTD             /* hpstd(([from, [to,]] lambda, expr) */
};

/*---------------- STRUCTS ------------------------*/

// LEC atomic element
struct ALEC 
{
    int     al_type;        // type : L_VAR, L_COEF, L_CONST ...
    union {
        LECREAL v_real;         // constant value (float)
        long    v_long;         // constant values (integer)
        int     v_nb_args;      // nb of args for functions
        struct {
            short   pos;        // coef or series pos in ??
            Period  per;        // Period if any
            short   lag;        // lag if any
        } v_var;                // variable
        short   v_coef;         // coef number
        Period  v_per;          // period
    } al_val;
};

struct CVAR {
    short   pos,  // SWHDL ?? NON
            lag,
            ref,
            pad;
    Period  per;
};

struct TOKEN {
    LECREAL tk_real;
    long    tk_long;
    Period  tk_period;
    int     tk_def;
    char    tk_name[L_MAX_NAME + 1];
};

// stack of operators used by L_analyse 
struct LSTACK 
{        
    unsigned ls_op      : 8;    // operator 
    //unsigned ls_nb_args : 8;  // nb of arguments 
    unsigned ls_nb_args;        // nb of arguments : 16 bits instead of 8 to allow checking max 255 args
};

struct LNAME {
    ONAME   name;   // scalar or variable name
    char    pad[3];
    long    pos;    // SWHDL 
};

struct CLEC {
    long    tot_lg,      
            exec_lg;       
    short   nb_names;   // number of scalar and variables names
    char    dupendo;    // duplicate endogenous variable in the LEC expression
    char    pad;
    LNAME   lnames[1];  // list of of scalar and variable names
};

/*---------------- MACROS ------------------------*/

#define s_dbl   sizeof(LECREAL)
#define s_ptr   sizeof(double *)
#define s_short sizeof(short)
#define s_long  sizeof(long)

#define L_open_file(filename)   L_open_all(filename, YY_FILE)
#define L_open_string(str)      L_open_all(str, YY_MEM)
#define L_open_stdin()          L_open_all((char *)0, YY_STDIN)

#define is_fn(op)       ((op) >= L_FN  && (op) < L_OP)
#define is_op(op)       ((op) >= L_OP  && (op) < L_TFN)
#define is_tfn(op)      ((op) >= L_TFN && (op) < L_VAL)
#define is_val(op)      ((op) >= L_VAL && (op) < L_MTFN)  /* JMP 20-04-98 */
#define is_mtfn(op)     ((op) >= L_MTFN)                  /* JMP 20-04-98 */

#define LYYTEXT   (L_YY->yy_text)
#define LYYLONG   (L_YY->yy_long)
#define LYYREAD   (L_YY->yy_type)
#define LYYDOUBLE (L_YY->yy_double)

/*----------------- GLOBALS ----------------------*/

inline int      L_curt = 0;         // current value of t
inline int      L_errno = 0;        // LEC error number (during compilation)
inline ALEC     *L_EXPR = 0;        // Table of all ALEC atomic elements (see iode.h)
inline char     **L_NAMES = 0;      // Table of names encountered in the current LEC expression (vars and scalars)
inline int      L_NB_EXPR = 0;      // Current number of elements (ALEC) in L_EXPR
inline int      L_NB_AEXPR = 0;     // Number of allocated elements in L_EXPR (multiple of 100) TODO: repl 100 by a define
inline int      L_NB_NAMES = 0;     // Current number of names in L_NAMES
inline int      L_NB_ANAMES = 0;    // Number of allocated names in L_NAMES (multiple of 10) TODO: repl 10 by a define

inline KDB*     L_EXEC_DBV = nullptr;   
inline KDB*     L_EXEC_DBS = nullptr; 

// --- PRIORITY OF OPERATORS  and number of functions args ---
// See iode.h FOR POSITIONS in the vectors
inline int L_PRIOR[13]    = {2, 3, 4, 4, 4, 4, 4, 4, 5, 5, 6, 6, 7};
inline int L_MIN_FARGS[]  = {1, 1, 1, 1, 1, 1,  1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 2};
inline int L_MAX_FARGS[]  = {1, 1, 2, 1, 1, 2, 255, 255, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 255, 255, 1, 255, 255, 1, 255, 1, 1, 1, 2, 1, 2, 1, 2};
inline int L_MAX_TARGS[]  = {2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 0, 3}; /* JMP 17-04-98 */
inline int L_MIN_TARGS[]  = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}; /* JMP 17-04-98 */
inline int L_MAX_MTARGS[] = {4, 4, 4, 3, 3, 4, 4, 1, 2, 4, 2, 4}; /* GB 14-11-00 */ // JMP 12/4/2019
inline int L_MIN_MTARGS[] = {2, 2, 2, 1, 1, 2, 2, 1, 2, 2, 2, 2}; /* GB 14-11-00 */ // JMP 12/4/2019

// --- LEC tokens ---
inline YYKEYS L_TABLE[] = 
{
    (unsigned char*) "!",            L_NOT,
    (unsigned char*) "!=",           L_NE,
    (unsigned char*) "(",            L_OPENP,
    (unsigned char*) "()",           L_OCPAR,
    (unsigned char*) ")",            L_CLOSEP,
    (unsigned char*) "*",            L_TIMES,
    (unsigned char*) "**",           L_EXP,
    (unsigned char*) "+",            L_PLUS,
    (unsigned char*) "+ ",           L_UPLUS,
    (unsigned char*) ",",            L_COMMA,
    (unsigned char*) "/",            L_DIVIDE,
    (unsigned char*) ":",            L_COLON,
    (unsigned char*) ";",            L_EOE,
    (unsigned char*) /*    ";",            L_COMMA,*/
    (unsigned char*) "<",            L_LT,
    (unsigned char*) ">=",           L_GE,
    (unsigned char*) "<=",           L_LE,
    (unsigned char*) "<>",           L_NE,
    (unsigned char*) "=",            L_EQ,
    (unsigned char*) ">",            L_GT,
    (unsigned char*) "-",            L_MINUS,
    (unsigned char*) "- ",           L_UMINUS,
    (unsigned char*) "abs",          L_ABS,
    (unsigned char*) "acf",          L_ACF,
    (unsigned char*) "acos",         L_ACOS,
    (unsigned char*) "and",          L_AND,
    (unsigned char*) "app",          L_APP,
    (unsigned char*) "appdif",       L_DAPP,
    (unsigned char*) "asin",         L_ASIN,
    (unsigned char*) "atan",         L_ATAN,
    (unsigned char*) "ceil",         L_CEIL,  /* JMP 18-10-2004 */
    (unsigned char*) "cos",          L_COS,
    (unsigned char*) "cosh",         L_COSH,
    (unsigned char*) "corr",         L_CORR, /* JMP 17-04-98 */
    (unsigned char*) "covar",        L_COVAR, /* JMP 17-04-98 */
    (unsigned char*) "covar0",       L_COVAR0, /* JMP 17-04-98 */
    (unsigned char*) "d",            L_DIFF,
    (unsigned char*) "dapp",         L_DAPP,
    (unsigned char*) "dlag",         L_DLAG,
    (unsigned char*) "dln",          L_DLN,
    (unsigned char*) "e",            L_E,
    (unsigned char*) "euro",         L_EURO,
    (unsigned char*) "exp",          L_EXPN,
    (unsigned char*) "gamma",        L_GAMMA,
    (unsigned char*) "div0",         L_DIV0,
    (unsigned char*) "grandom",      L_GRANDOM,
    (unsigned char*) "grt",          L_GRT,
    (unsigned char*) "hp",           L_HP,
    (unsigned char*) "hpstd",        L_HPSTD, // JMP 12/4/2019
    (unsigned char*) "if",           L_IF,
    (unsigned char*) "index",        L_INDEX,
    (unsigned char*) "int",          L_INT,
    (unsigned char*) "interpol",     L_INTERPOL,
    (unsigned char*) "isan",         L_FNISAN,
    (unsigned char*) "floor",        L_FLOOR, /* JMP 18-10-2004 */
    (unsigned char*) "l",            L_LAG,
    (unsigned char*) "lastobs",      L_LASTOBS,
    (unsigned char*) "ln",           L_LN,
    (unsigned char*) "lcount",       L_LCOUNT,
    (unsigned char*) "lmean",        L_LMEAN,
    (unsigned char*) "log",          L_LOG,
    (unsigned char*) "lprod",        L_LPROD,
    (unsigned char*) "lstderr",      L_LSTDERR,
    (unsigned char*) "lsum",         L_LSUM,
    (unsigned char*) "ma",           L_MAVG,
    (unsigned char*) "mavg",         L_MAVG,
    (unsigned char*) "max",          L_MAX,
    (unsigned char*) "mean",         L_MEAN,
    (unsigned char*) "min",          L_MIN,
    (unsigned char*) "not",          L_NOT,
    (unsigned char*) "or",           L_OR,
    (unsigned char*) "pi",           L_PI,
    (unsigned char*) "prod",         L_PROD,
    (unsigned char*) "r",            L_RAPP,
    (unsigned char*) "rad",          L_RAD,
    (unsigned char*) "random",       L_RANDOM,
    (unsigned char*) "round",        L_ROUND, /* JMP 18-10-2004 */
    (unsigned char*) "sign",         L_SIGN,
    (unsigned char*) "sin",          L_SIN,
    (unsigned char*) "sinh",         L_SINH,
    (unsigned char*) "sqrt",         L_SQRT,
    (unsigned char*) "stddev",       L_STDDEV,
    (unsigned char*) "stderr",       L_STDERR,
    (unsigned char*) "sum",          L_SUM,
    (unsigned char*) "t",            L_TIME,
    (unsigned char*) "tan",          L_TAN,
    (unsigned char*) "tanh",         L_TANH,
    (unsigned char*) "urandom",      L_URANDOM,
    (unsigned char*) "var",          L_VARIANCE,
    (unsigned char*) "vmax",         L_VMAX,
    (unsigned char*) "vmin",         L_VMIN,
    (unsigned char*) "[",            L_OPENB,
    (unsigned char*) "]",            L_CLOSEB,
    (unsigned char*) "^",            L_EXP,
    (unsigned char*) "i",            L_I
};

inline  TOKEN   L_TOKEN;            // Global containing the last read token
inline  YYFILE* L_YY = nullptr;     // LEC stream the compiler is reading from

inline char    **KEXEC_VFILES = NULL;
inline char    **KEXEC_SFILES = NULL;
inline int     KEXEC_TRACE = 0;

/* ---------------------- FUNCS ---------------------- */

inline int YY_compare(const void *a, const void *b)
{
    return YY_strcmp((const char*) a, (const char*) b);
}

inline CLEC* clec_deep_copy(const CLEC* other)
{
    if(other == NULL) 
        return NULL;
    
    CLEC* copy = NULL;
    if(other->tot_lg == 0)
    {
        copy = (CLEC*) SW_nalloc(sizeof(CLEC));
        copy->tot_lg = 0;
        copy->exec_lg = 0;
        copy->nb_names = 0;
        copy->dupendo = 0;
        copy->pad = '\0';    
    }
    else
    {
        // NOTE : see end of function L_cc2() from l_cc2.c to calculate tot_lg (= len)
        copy = (CLEC*) SW_nalloc(other->tot_lg);
    
        copy->tot_lg = other->tot_lg,      
        copy->exec_lg = other->exec_lg;       
        copy->nb_names = other->nb_names;
        copy->dupendo = other->dupendo;
        copy->pad = '\0';
        for(int i = 0; i < other->nb_names; i++)
        {
            strncpy(copy->lnames[i].name, other->lnames[i].name, sizeof(ONAME) / sizeof(char));
            memset(copy->lnames[i].pad, '\0', sizeof(LNAME::pad) / sizeof(char));
            copy->lnames[i].pos = other->lnames[i].pos;
        }
    }
    
    return copy;
}

inline bool clec_equal(const CLEC* clec_1, const CLEC* clec_2)
{
    if(clec_1->tot_lg != clec_2->tot_lg) return false;
    if(clec_1->exec_lg != clec_2->exec_lg) return false;
    if(clec_1->nb_names != clec_2->nb_names) return false;
    if(clec_1->dupendo != clec_2->dupendo) return false;
    if(clec_1->pad != clec_2->pad) return false;
    for(int i = 0; i < clec_1->nb_names; i++)
    {
        if(strcmp(clec_1->lnames[i].name, clec_2->lnames[i].name)) return false;
        if(strcmp(clec_1->lnames[i].pad, clec_2->lnames[i].pad)) return false;
        if(clec_1->lnames[i].pos, clec_2->lnames[i].pos) return false;
    }
    return true;
}

inline std::vector<std::string> get_scalars_from_clec(CLEC* clec)
{
    std::vector<std::string> list;

    if(clec == NULL)
        throw std::runtime_error("Cannot get list of scalars.\nClec structure not defined.");
    
    char* item_name;
    for(int i = 0; i < clec->nb_names; i++)
    { 
        item_name = clec->lnames[i].name;
        if(is_coefficient(item_name))
            list.push_back(std::string(item_name));
    }

    return list;
}

inline std::vector<std::string> get_variables_from_clec(CLEC* clec)
{
    std::vector<std::string> list;

    if(clec == NULL)
        throw std::runtime_error("Cannot get list of variables.\nClec structure not defined.");
    
    char* item_name;
    for(int i = 0; i < clec->nb_names; i++)
    {
        item_name = clec->lnames[i].name;
        if(!is_coefficient(item_name))
            list.push_back(std::string(item_name));
    }

    return list;
}

/* l_token.c */
int L_nb_tokens(void);
int L_open_all(char *,int );
void L_close(void);
//int L_lex(void);
//int L_read(void);
//int L_unread(void);
//int L_macro(void);
//int L_string(void);
//int L_read_string(void);
//int L_getc(void);
//void L_ungetc(int );
//void L_skip(void);
int L_get_token(void);
//int L_include(char *,char *);
//int L_get_int(void);
//int L_get_period(YYFILE *,Period *);

/* l_cc1.c */
void L_alloc_expr(int );
int L_cc1(int );
// int L_add_new_series(char *);
void L_free_anames(void);
//int L_save_var(void);
//int L_save_op(void);
//int L_priority_sup(int );
//int L_add_stack(int );
//int L_clear_stack(void);
//int L_anal_lag(void);
int L_sub_expr(ALEC *,int );
//int L_time_expr(void);
//int L_lag_expr(int );

/* ode.c */
// char    *L_expand(char *);

/* l_alloc.c */
char *L_malloc(int );
//int L_free(char *);
void L_free(void *);

/* l_err.c */
char *L_error(void);

/* l_cc2.c */
CLEC *L_cc2(ALEC *);
void L_move_arg(char *,char *,int );
//int L_calc_len(ALEC *,int ,int );
CLEC *L_cc_stream(void);
CLEC *L_cc(char *);

/* l_link.c */
int L_link(KDB *,KDB *,CLEC *);
// int L_link1(KDB *,KDB *,CLEC *);
//int L_link2(KDB *,CLEC *);
//int L_link_sub(KDB *,char *,int );
void L_link_endos(KDB* kde, CLEC* cl);

/* l_exec.c */
#ifdef _MSC_VER
        int matherr(struct _exception *e);
#else
        // Define the exception structure
        struct exception {
        int type;       // Type of error
        char *name;     // Name of the function where the error occurred
        double arg1;    // First argument to the function
        double arg2;    // Second argument to the function (if applicable)
        double retval;  // Return value
        };
        
        int matherr(struct exception *e);
#endif
void L_fperror(void);
double L_exec(KDB *,KDB *,CLEC *,int );
double L_exec_sub(unsigned char *,int ,int ,double *);
L_REAL* L_cc_link_exec(char* lec, KDB* dbv, KDB* dbs);
int L_intlag(double );
double L_uminus(double* stack, int nbargs=-1);
double L_uplus(double* stack, int nbargs=-1);
double L_logn(double );
double L_log(double* stack, int nbargs);
double L_ln(double* stack, int nbargs=-1);
double L_not(double* stack, int nbargs=-1);
double L_expn(double *, int);
double L_max(double* stack, int nbargs);
double L_min(double* stack, int nbargs);
double L_sin(double* stack, int nbargs=-1);
double L_asin(double* stack, int nbargs=-1);
double L_cos(double* stack, int nbargs=-1);
double L_acos(double* stack, int nbargs=-1);
double L_tan(double* stack, int nbargs=-1);
double L_atan(double* stack, int nbargs=-1);
double L_sinh(double* stack, int nbargs=-1);
double L_cosh(double* stack, int nbargs=-1);
double L_tanh(double* stack, int nbargs=-1);
double L_sqrt(double* stack, int nbargs=-1);
double L_int(double* stack, int nbargs=-1);
double L_abs(double* stack, int nbargs=-1);
double L_sign(double* stack, int nbargs=-1);
double L_rad(double* stack, int nbargs=-1);
double L_if(double* stack, int nbargs);
double L_fnisan(double* stack, int nbargs);
double L_lcount(double* stack, int nbargs);
double L_lprod(double* stack, int nbargs);
double L_lsum(double* stack, int nbargs);
double L_lstderr(double* stack, int nbargs);
double L_lmean(double* stack, int nbargs);
double L_random(double* stack, int nbargs=-1);
double L_or(double ,double );
double L_and(double ,double );
double L_ge(double ,double );
double L_gt(double ,double );
double L_eq(double ,double );
double L_ne(double ,double );
double L_le(double ,double );
double L_lt(double ,double );
double L_plus(double ,double );
double L_minus(double ,double );
double L_divide(double ,double );
double L_times(double ,double );
double L_exp(double ,double );
double L_pi(int t=-1);
double L_euro(int t=-1);
double L_e(int t=-1);
double L_time(int t);
double L_i(int t);
double L_lag(unsigned char *,short ,int ,double *,int );
double L_diff(unsigned char *,short,int ,double *,int );
double L_rapp(unsigned char *,short,int ,double *,int );
double L_dln(unsigned char *,short,int ,double *,int );
double L_grt(unsigned char *,short,int ,double *,int );
double L_mavg(unsigned char *,short,int ,double *,int );
void L_tfn_args(int ,double *,int ,int *,int *);
double L_vmax(unsigned char *,short,int ,double *,int );
double L_vmin(unsigned char *,short,int ,double *,int );
double L_sum(unsigned char *,short,int ,double *,int );
double L_prod(unsigned char *,short,int ,double *,int );
double L_mean(unsigned char *,short ,int ,double *,int );
double L_stderr(unsigned char *,short,int ,double *,int );
double L_lastobs(unsigned char *,short,int ,double *,int );
double L_calccorr(unsigned char *,short ,unsigned char *,short ,int ,double *,int );
double L_corr(unsigned char *,short ,int ,double *,int );
double L_calccovar(unsigned char *,short ,unsigned char *,short ,int ,double *,int ,int );
double L_covar(unsigned char *,short ,int ,double *,int );
double L_covar0(unsigned char *,short ,int ,double *,int );
double L_var(unsigned char *,short ,int ,double *,int );
double L_stddev(unsigned char *,short ,int ,double *,int );
double L_index(unsigned char *,short ,int ,double *,int );
double L_acf(unsigned char *,short ,int ,double *,int );
int L_stackna(double **,int );
int L_calcvals(unsigned char *,short ,int ,double *,int *,double *, int);
double L_interpol(unsigned char *,short ,int ,double *,int );
double L_app(unsigned char *,short ,int ,double *,int );

/* l_hodrick.c */
int HP_calc(double *f_vec, double *t_vec, int nb, double lambda, int std);
void HP_test(double *f_vec, double *t_vec, int nb, int *beg, int *dim);

/* l_eqs.c */
CLEC *L_solve(char *,char *);
int L_split_eq(char *);

/* l_newton.c */
double L_zero(KDB *,KDB *,CLEC *,int ,int ,int );
double L_newton(KDB *,KDB *,CLEC *,int ,int ,int );
//double L_newton_1(int ,KDB *,KDB *,CLEC *,int ,int ,int );

/* l_secant.c */
//double L_fx(double ,int );
//int L_bracket(double *,double *,int );
double L_secant(KDB *,KDB *,CLEC *,int ,int ,int );

/* l_debug.c */
void L_debug(char *,...);

/* k_lec.c */
inline char *(*L_expand_super)(char* list_name) = nullptr;

double *L_getvar(KDB *,int );
double L_getscl(KDB *,int );
Sample *L_getsmpl(KDB *);
int L_findscl(KDB *,char *);
int L_findvar(KDB *,char *);
char* L_expand(char* list_name);

// l_exec_val.c
inline L_REAL(*L_VAL_FN[])(int t) = 
{ 
    L_pi,           // L_PI        L_VAL + 0
    L_e,            // L_E         L_VAL + 1
    L_time,         // L_TIME      L_VAL + 2
    L_i,            // L_I         L_VAL + 3
    L_euro          // L_EURO      L_VAL + 4
};

// l_exec_ops.c
inline L_REAL(*L_OPS_FN[])(L_REAL val1, L_REAL val2) = 
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

// l_exec_fns.cpp
L_REAL L_floor(L_REAL* stack, int nbargs=-1);
L_REAL L_ceil (L_REAL* stack, int nbargs=-1);
L_REAL L_round(L_REAL* stack, int nbargs);
L_REAL L_urandom(L_REAL* stack, int nbargs=-1);
L_REAL L_grandom(L_REAL* stack, int nbargs=-1);
L_REAL L_gamma(L_REAL* stack, int nbargs=-1);
L_REAL L_div0(L_REAL* stack, int nbargs);

inline L_REAL(*L_FNS_FN[])(L_REAL* stack, int nbargs) = 
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

// l_exec_tfn.c
inline L_REAL(*L_TFN_FN[])(unsigned char* expr, short length, int t, L_REAL *stack, int nargs) = 
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

// l_exec_mtfn.c
L_REAL L_hp(unsigned char* expr, short len, int t, L_REAL* stack, int nargs);
L_REAL L_dapp(unsigned char* expr, short nvargs, int t, L_REAL* stack, int nargs);
L_REAL L_hpstd(unsigned char* expr, short len, int t, L_REAL* stack, int nargs);

inline L_REAL(*L_MTFN_FN[])(unsigned char* expr, short nvargs, int t, L_REAL *stack, int nargs) = 
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

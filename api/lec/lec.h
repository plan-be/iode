#pragma once

#include "scr4/s_yy.h"          // YYKEYS

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/kdb.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    L_PERIOD_ERR,
    L_LAG_ERR,
    L_PERIODY_ERR,
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
    L_PERIOD,
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

typedef struct _alec {      /* LEC atomic element */
	int     al_type;        /* type : L_VAR, L_COEF, L_CONST ... */
    union {
	LECREAL v_real;         /* constant values double */
	long    v_long;         /* constant values long int */
	int     v_nb_args;      /* nb of args for fn */
	struct {
	    short   pos;        /* coef or series pos in table ?? */
	    PERIOD  per;        /* PERIOD if any */
	    short   lag;        /* lag if any */
	} v_var;                /* variable  */
	short   v_coef;         /* coef number */
	PERIOD  v_per;          /* period */
    } al_val;
} ALEC;

typedef struct _cvar_ {
    short   pos,  /* SWHDL ?? NON */
	    lag,
	    ref,
	    pad;
    PERIOD  per;
} CVAR;

typedef struct _token {
    LECREAL tk_real;
    long    tk_long;
    PERIOD  tk_period;
	int     tk_def;
	char    tk_name[L_MAX_NAME + 1];
} TOKEN;

typedef struct _lstack {        // stack of operators used by L_analyse 
    unsigned ls_op      : 8;    // operator 
    //unsigned ls_nb_args : 8;  // nb of arguments 
    unsigned ls_nb_args;        // nb of arguments : 16 bits instead of 8 to allow checking max 255 args
} LSTACK;

typedef struct _lname_ {
    ONAME   name;   // scalar or variable name
	char    pad[3];
    long    pos;    // SWHDL 
} LNAME;

typedef struct _clec_ {
    long    tot_lg,      
		exec_lg;       
    short   nb_names;   // number of scalar and variables names
    char    dupendo;
    char    pad;
    LNAME   lnames[1];  // list of of scalar and variable names
} CLEC;

/*---------------- MACROS ------------------------*/

#define L_ISCOEF(x) ((x)[0] >= 'a')

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

extern  int     L_errno;
extern  int     L_LEVEL;
extern  SAMPLE  L_SAMPLE;
extern  ALEC    *L_EXPR;
extern  char    **L_NAMES;
extern  int     L_NB_EXPR, L_NB_AEXPR, L_NB_ANAMES;
extern  int     L_NB_NAMES;
extern  int     L_PRIOR[],
	            L_MAX_FARGS[],
	            L_MIN_FARGS[],
	            L_MAX_TARGS[],
	            L_MIN_TARGS[],
	            L_MAX_MTARGS[],
	            L_MIN_MTARGS[];
extern YYKEYS   L_TABLE[];

extern  TOKEN   L_TOKEN;
extern  YYFILE  *L_YY;
extern  char    L_PERIOD_CH[];
extern  int     L_PERIOD_NB[];

extern int      G_errno;

extern  char    **KEXEC_VFILES, 
                **KEXEC_SFILES;
extern  int     KEXEC_TRACE;

/* ---------------------- FUNCS ---------------------- */

/* l_token.c */
extern int L_nb_tokens(void);
extern int L_open_all(char *,int );
extern void L_close(void);
//extern int L_lex(void);
//extern int L_read(void);
//extern int L_unread(void);
//extern int L_macro(void);
//extern int L_string(void);
//extern int L_read_string(void);
//extern int L_getc(void);
//extern void L_ungetc(int );
//extern void L_skip(void);
extern int L_get_token(void);
//extern int L_include(char *,char *);
//extern int L_get_int(void);
//extern int L_get_period(YYFILE *,PERIOD *);

/* l_cc1.c */
extern void L_alloc_expr(int );
extern int L_cc1(int );
// extern int L_add_new_series(char *);
extern void L_free_anames(void);
//extern int L_save_var(void);
//extern int L_save_op(void);
//extern int L_priority_sup(int );
//extern int L_add_stack(int );
//extern int L_clear_stack(void);
//extern int L_anal_lag(void);
extern int L_sub_expr(ALEC *,int );
//extern int L_time_expr(void);
//extern int L_lag_expr(int );

/* ode.c */
//extern  char    *L_expand(char *);

/* l_alloc.c */
extern char *L_malloc(int );
//extern int L_free(char *);
extern void L_free(void *);

/* l_err.c */
extern char *L_error(void);

/* l_cc2.c */
extern CLEC *L_cc2(ALEC *);
extern void L_move_arg(char *,char *,int );
//extern int L_calc_len(ALEC *,int ,int );
extern CLEC *L_cc_stream(void);
extern CLEC *L_cc(char *);

/* l_link.c */
extern int L_link(KDB *,KDB *,CLEC *);
// extern int L_link1(KDB *,KDB *,CLEC *);
//extern int L_link2(KDB *,CLEC *);
//extern int L_link_sub(KDB *,char *,int );
extern void L_link_endos(KDB* kde, CLEC* cl);

/* l_exec.c */
#ifdef _MSC_VER
        extern int matherr(struct _exception *e);
#else
        // Define the exception structure
        struct exception {
        int type;       // Type of error
        char *name;     // Name of the function where the error occurred
        double arg1;    // First argument to the function
        double arg2;    // Second argument to the function (if applicable)
        double retval;  // Return value
        };
        
        extern int matherr(struct exception *e);
#endif
extern void L_fperror(void);
extern double L_exec(KDB *,KDB *,CLEC *,int );
extern double L_exec_sub(unsigned char *,int ,int ,double *);
extern L_REAL* L_cc_link_exec(char* lec, KDB* dbv, KDB* dbs);
extern int L_intlag(double );
extern double L_uminus(double *);
extern double L_uplus(double *);
extern double L_logn(double );
extern double L_log(double *,int );
extern double L_ln(double *);
extern double L_not(double *);
extern double L_expn(double *, int);
extern double L_max(double *,int );
extern double L_min(double *,int );
extern double L_sin(double *);
extern double L_asin(double *);
extern double L_cos(double *);
extern double L_acos(double *);
extern double L_tan(double *);
extern double L_atan(double *);
extern double L_sinh(double *);
extern double L_cosh(double *);
extern double L_tanh(double *);
extern double L_sqrt(double *);
extern double L_int(double *);
extern double L_abs(double *);
extern double L_sign(double *);
extern double L_rad(double *);
extern double L_if(double *,int );
extern double L_fnisan(double *,int );
extern double L_lcount(double *,int );
extern double L_lprod(double *,int );
extern double L_lsum(double *,int );
extern double L_lstderr(double *,int );
extern double L_lmean(double *,int );
extern double L_random(double *);
extern double L_or(double ,double );
extern double L_and(double ,double );
extern double L_ge(double ,double );
extern double L_gt(double ,double );
extern double L_eq(double ,double );
extern double L_ne(double ,double );
extern double L_le(double ,double );
extern double L_lt(double ,double );
extern double L_plus(double ,double );
extern double L_minus(double ,double );
extern double L_divide(double ,double );
extern double L_times(double ,double );
extern double L_exp(double ,double );
extern double L_pi(void);
extern double L_euro(void);
extern double L_e(void);
extern double L_time(int );
extern double L_i(int );
extern double L_lag(unsigned char *,short ,int ,double *,int );
extern double L_diff(unsigned char *,short,int ,double *,int );
extern double L_rapp(unsigned char *,short,int ,double *,int );
extern double L_dln(unsigned char *,short,int ,double *,int );
extern double L_grt(unsigned char *,short,int ,double *,int );
extern double L_mavg(unsigned char *,short,int ,double *,int );
extern void L_tfn_args(int ,double *,int ,int *,int *);
extern double L_vmax(unsigned char *,short,int ,double *,int );
extern double L_vmin(unsigned char *,short,int ,double *,int );
extern double L_sum(unsigned char *,short,int ,double *,int );
extern double L_prod(unsigned char *,short,int ,double *,int );
extern double L_mean(unsigned char *,short ,int ,double *,int );
extern double L_stderr(unsigned char *,short,int ,double *,int );
extern double L_lastobs(unsigned char *,short,int ,double *,int );
extern double L_calccorr(unsigned char *,short ,unsigned char *,short ,int ,double *,int );
extern double L_corr(unsigned char *,short ,int ,double *,int );
extern double L_calccovar(unsigned char *,short ,unsigned char *,short ,int ,double *,int ,int );
extern double L_covar(unsigned char *,short ,int ,double *,int );
extern double L_covar0(unsigned char *,short ,int ,double *,int );
extern double L_var(unsigned char *,short ,int ,double *,int );
extern double L_stddev(unsigned char *,short ,int ,double *,int );
extern double L_index(unsigned char *,short ,int ,double *,int );
extern double L_acf(unsigned char *,short ,int ,double *,int );
extern int L_stackna(double **,int );
extern int L_calcvals(unsigned char *,short ,int ,double *,int *,double *, int);
extern double L_interpol(unsigned char *,short ,int ,double *,int );
extern double L_app(unsigned char *,short ,int ,double *,int );

/* l_hodrick.c */
extern int HP_calc(double *f_vec, double *t_vec, int nb, double lambda, int std);
extern void HP_test(double *f_vec, double *t_vec, int nb, int *beg, int *dim);

/* l_eqs.c */
extern CLEC *L_solve(char *,char *);
extern int L_split_eq(char *);

/* l_newton.c */
extern double L_zero(KDB *,KDB *,CLEC *,int ,int ,int );
extern double L_newton(KDB *,KDB *,CLEC *,int ,int ,int );
//extern double L_newton_1(int ,KDB *,KDB *,CLEC *,int ,int ,int );

/* l_secant.c */
//extern double L_fx(double ,int );
//extern int L_bracket(double *,double *,int );
extern double L_secant(KDB *,KDB *,CLEC *,int ,int ,int );

/* l_debug.c */
extern void L_debug(char *,...);

/* k_lec.c */
extern char *(*L_expand_super)(char* list_name);

extern double *L_getvar(KDB *,int );
extern double L_getscl(KDB *,int );
extern SAMPLE *L_getsmpl(KDB *);
extern int L_findscl(KDB *,char *);
extern int L_findvar(KDB *,char *);
extern char* L_expand(char* list_name);

#ifdef __cplusplus
}
#endif

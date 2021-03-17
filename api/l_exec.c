#include "iode.h"
#include <setjmp.h>
#include <signal.h>
#include <time.h>

#define L_REAL double

#ifdef UNIX
#define _isnan isnan
#endif

L_REAL L_exec_sub();
/*NH*/
extern L_REAL L_uminus(),
       L_uplus (),              L_log   (),              L_ln    (),
       L_not   (),              L_expn  (),              L_max   (),
       L_min   (),              L_sin   (),              L_cos   (),
       L_acos  (),              L_asin  (),              L_tan   (),
       L_atan  (),              L_tanh  (),              L_sinh  (),
       L_cosh  (),              L_abs   (),              L_sqrt  (),
       L_int   (),              L_rad   (),              L_if    (),

       L_or    (),              L_and   (),              L_ge    (),
       L_gt    (),              L_le    (),              L_lt    (),
       L_eq    (),              L_ne    (),              L_plus  (),
       L_minus (),              L_divide(),              L_times (),
       L_exp   (),              L_pi    (),              L_e     (),
       L_time  (),
       L_lag   (),              L_diff  (),              L_rapp  (),
       L_dln   (),              L_grt   (),              L_mavg  (),
       L_vmax  (),              L_vmin  (),              L_sum   (),
       L_prod  (),              L_mean  (),              L_stderr(),
       L_covar (),              L_covar0(),              L_corr  (),
       L_logn  (),              L_i     (),              L_var(),
       L_stddev(),              L_lsum(),                L_index(),
       L_acf(),                 L_lmean(),               L_fnisan(),
       L_lcount(),              L_lprod(),               L_sign(),
       L_euro(),                L_lstderr(),             L_lastobs(),
       L_interpol(),            L_app(),                 L_random(),
       L_hp(),                  L_dapp(),                L_floor(),
       L_ceil(),                L_round(),               L_urandom(),
       L_grandom(),             L_gamma(),               L_div0(),
       L_hpstd()
       ;

L_REAL (*L_FNS_FN[])() = { L_uminus,
                           L_uplus ,              L_log   ,              L_ln    ,
                           L_not   ,              L_expn  ,              L_max   ,
                           L_min   ,              L_sin   ,              L_cos   ,
                           L_acos  ,              L_asin  ,              L_tan   ,
                           L_atan  ,              L_tanh  ,              L_sinh  ,
                           L_cosh  ,              L_abs   ,              L_sqrt  ,
                           L_int   ,              L_rad   ,              L_if    ,
                           L_lsum  ,              L_lmean ,              L_fnisan,
                           L_lcount,              L_lprod ,              L_sign  ,
                           L_lstderr,             L_random,              L_floor ,
                           L_ceil,                L_round,               L_urandom,
                           L_grandom,             L_gamma,               L_div0
                         };

L_REAL (*L_OPS_FN[])() = {
    L_or    ,              L_and   ,              L_ge    ,
    L_gt    ,              L_le    ,              L_lt    ,
    L_eq    ,              L_ne    ,              L_plus  ,
    L_minus ,              L_divide,              L_times ,
    L_exp
};

L_REAL (*L_TFN_FN[])() = { L_lag, L_diff, L_rapp, L_dln, L_grt, L_mavg,
                           L_vmax, L_vmin, L_sum, L_prod, L_mean, L_stderr, 0, L_lastobs
                         };

L_REAL (*L_VAL_FN[])() = { L_pi, L_e, L_time, L_i, L_euro};

L_REAL (*L_MTFN_FN[])() = { L_corr, L_covar, L_covar0, L_var,
                            L_stddev, L_index, L_acf, L_interpol, L_app, L_hp, L_dapp, L_hpstd
                          };


jmp_buf L_JMP;
static KDB      *L_EXEC_DBV, *L_EXEC_DBS;
static LNAME    *L_EXEC_NAMES;
static int      L_SIG = 0, L_curt;

/*NH*/

#ifdef VC
int _matherr(a)
struct _exception *a;
{
    a->retval = (double)L_NAN;
    return(1);
}
#else
int matherr(a)
struct exception *a;
{
    a->retval = (double)L_NAN;
    return(1);
}
#endif

/*NH*/
void L_fperror()
{
    L_SIG = 0;
#ifdef DOS
    _fpreset();
#endif
    longjmp(L_JMP, 1);
}

/*
    Execution of a compiled LEC expression.

    Input : 3 pointers which are passed to the user defined functions
    L_getvar and L_getscl. These functions have the following syntax :

    IODE_REAL *L_getvar(dbv, pos) : returns the pointer to the variable
	    pos of dbv.

    IODE_REAL L_getscl(dbs, pos) : returns the value of the scalar nb pos
	    in dbs.

    SMPL  *L_getsmp(dbv)  returns a pointer to the sample of
    dbv, the database of variables

    expr is a pointer to a CLEC structure.

    t is the index for which the expression is to be computed.

    In case of success, L_exec returns the computed value.
    If an error occurs, L_exec sets L_errno and returns L_NAN (see <lec.h>).
*/

double L_exec(dbv, dbs, expr, t)
KDB     *dbv, *dbs;
CLEC    *expr;
int     t;
{
    int	pos; //short   pos;
    // L_REAL  stack[300];
    L_REAL  stack[1000]; // Pour reculer certains plantages (non solutionné...)

    L_EXEC_DBV = dbv;
    L_EXEC_DBS = dbs;
    L_EXEC_NAMES = &(expr->lnames[0]);
    L_errno = 0;
    if(expr == 0) return((double)L_NAN);
    if(L_SIG == 0) {
        signal(SIGFPE, L_fperror);
        L_SIG = 1;
    }
    if(setjmp(L_JMP))
        return((double)L_NAN);
    pos = sizeof(CLEC) + (expr->nb_names - 1) * sizeof(LNAME);
    L_curt = t;
    return(L_exec_sub((char *)expr + pos, expr->tot_lg - pos, t, stack));
}

/*NH*/
double L_exec_sub(expr, lg, t, stack)
unsigned char    *expr;
int	lg;//short   lg;
int     t;
L_REAL  *stack;
/*register L_REAL  *stack;*/
{
    IODE_REAL    *d_ptr;
    LECREAL r;
    int     j, nargs, keyw, nvargs;
    long    l;
    short   len, len1, s;
    CVAR    cvar;

    for(j = 0 ; j < lg ; ) {
        keyw = expr[j++];
        if(keyw == L_VAR || keyw == L_VART) {
            memcpy(&cvar, expr + j, sizeof(CVAR));
            d_ptr = L_getvar(L_EXEC_DBV, L_EXEC_NAMES[cvar.pos].pos);
            j += sizeof(CVAR);
            len = cvar.ref;
            if(cvar.per.p_y == 0)  len += t;
            stack++;
            if(len < 0 || len >= (L_getsmpl(L_EXEC_DBV))->s_nb) {
                *stack = L_NAN;
                /*                L_errno = L_BOUNDS_ERR;
                		longjmp(L_JMP, 1);
                */
            }
            else {
                *stack = d_ptr[len];
            }
        }

        else switch(keyw) {
            case L_PLUS  :
                if(L_stackna(&stack, 2)) break;
                stack--;
                *stack += *(stack + 1);
                break;
            case L_MINUS :
                if(L_stackna(&stack, 2)) break;
                stack --;
                *stack -= *(stack + 1);
                break;
            case L_TIMES :
                if(L_stackna(&stack, 2)) break;
                stack --;
                *stack *= *(stack + 1);
                break;
            case L_DCONST :
                stack++;
                memcpy(&r, expr + j, sizeof(LECREAL)); /* FLOAT 11-04-98 */
                *stack = r;
                j += s_dbl;
                break;
            case L_LCONST :
                stack++;
                memcpy(&l, expr + j, sizeof(long));
                *stack = l;
                j += s_long;
                break;
            case L_COEF :
                stack++;
                memcpy(&cvar, expr + j, sizeof(CVAR));
                *stack = L_getscl(L_EXEC_DBS, L_EXEC_NAMES[cvar.pos].pos);
                j += sizeof(CVAR);
                break;
            case L_PERIOD :
                j += sizeof(PERIOD);
                stack++;
                memcpy(&s, expr + j, sizeof(short));
                *stack = s;
                j += s_short;
                break;
            default :
                if(is_op(keyw)) {
                    if(L_stackna(&stack, 2)) break;
                    stack--;
                    *stack = (L_OPS_FN[keyw - L_OP])(*stack, *(stack + 1));
                    break;
                }
                if(is_fn(keyw)) {
                    nargs = expr[j];
                    j++;
                    if(keyw != L_IF && keyw != L_FNISAN &&
                            keyw != L_LMEAN && keyw != L_LSTDERR &&  /* JMP 03-03-99 */
                            keyw != L_LCOUNT && L_stackna(&stack, nargs))
                        break;
                    *(stack - nargs + 1) = (L_FNS_FN[keyw - L_FN])(stack, nargs);
                    stack -= nargs - 1;
                    break;
                }

                if(is_tfn(keyw)) {
                    nargs = expr[j];
                    memcpy(&len, expr + j + 1, sizeof(short));
                    j += 1 + sizeof(short);
                    *(stack - nargs + 2) =
                        (L_TFN_FN[keyw - L_TFN])(expr + j,
                                                 len, t, stack, nargs);
                    j += len;
                    stack -= nargs - 2;
                    break;
                }

                if(is_mtfn(keyw)) {
                    nargs = expr[j];
                    nvargs = expr[j + 1];
                    memcpy(&len, expr + j + 2, sizeof(short));
                    j += 2 + sizeof(short);
                    *(stack - nargs + nvargs + 1) =
                        (L_MTFN_FN[keyw - L_MTFN])(expr + j,
                                                   nvargs, t, stack, nargs);
                    j += len;
                    stack -= nargs - nvargs - 1;
                    break;
                }

                if(is_val(keyw)) {
                    stack++;
                    *stack = (L_VAL_FN[keyw - L_VAL])(t);
                    break;
                }
                kerror(0, "Internal error (L_exec)");
                return((double)L_NAN);
            }
        /*        if(*stack <= L_NAN) return((double)L_NAN); */
    }
    return((double)*stack);
}

/*NH*/
int L_intlag(lag)
L_REAL      lag;
{
    int intlag;

    if(lag < 0) intlag = -0.5 + lag;
    else        intlag = 0.5 + lag;

    return(intlag);
}
/*NH*/
L_REAL L_uminus(stack)
L_REAL  *stack;
{
    return(- *stack);
}
/*NH*/
L_REAL L_uplus(stack)
L_REAL  *stack;
{
    return(*stack);
}
/*NH*/
L_REAL L_logn(v)
L_REAL  v;
{
    double  x;

    if(!L_ISAN(v) || v <= 0) {
        return((L_REAL)L_NAN);
        /*      L_errno = L_LOG_ERR;
        	longjmp(L_JMP, 1);
        */
    }

    x = log((double)v);
    if(_isnan(x)) x = L_NAN; /* JMP 18-01-02 */
    return((L_REAL)x);
}
/*NH*/
L_REAL L_log(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    if(nargs == 2) return(L_divide(L_logn(*stack), L_logn(*(stack - 1))));
    return(L_logn(*stack));
}
/*NH*/
L_REAL L_ln(stack)
L_REAL  *stack;
{
    return(L_logn(*stack));
}
/*NH*/
L_REAL L_not(stack)
L_REAL  *stack;
{
    if(fabs((double)(*stack)) < 1e-15) return(1.0);   /* JMP38 27-10-92 */
    return(0.0);                                      /* JMP38 27-10-92 */
    /*    return((*stack == 0) ? 1.0 : 0.0); */
}

/*NH*/
L_REAL L_max(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    L_REAL  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        if(m < *(stack - j)) m = *(stack - j);
    return(m);
}
/*NH*/
L_REAL L_min(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    L_REAL  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        if(m > *(stack - j)) m = *(stack - j);
    return(m);
}
/*NH*/
L_REAL L_sin(stack)
L_REAL  *stack;
{
    return((L_REAL)sin((double)*stack));
}
/*NH*/
L_REAL L_asin(stack)
L_REAL  *stack;
{
    return((L_REAL)asin((double)*stack));
}
/*NH*/
L_REAL L_cos(stack)
L_REAL  *stack;
{
    return((L_REAL)cos((double)*stack));
}
/*NH*/
L_REAL L_acos(stack)
L_REAL  *stack;
{
    return((L_REAL)acos((double)*stack));
}
/*NH*/
L_REAL L_tan(stack)
L_REAL  *stack;
{
    return((L_REAL)tan((double)*stack));
}
/*NH*/
L_REAL L_atan(stack)
L_REAL  *stack;
{
    return((L_REAL)atan((double)*stack));
}
/*NH*/
L_REAL L_sinh(stack)
L_REAL  *stack;
{
    return((L_REAL)sinh((double)*stack));
}
/*NH*/
L_REAL L_cosh(stack)
L_REAL  *stack;
{
    return((L_REAL)cosh((double)*stack));
}
/*NH*/
L_REAL L_tanh(stack)
L_REAL  *stack;
{
    return((L_REAL)tanh((double)*stack));
}
/*NH*/
L_REAL L_sqrt(stack)
L_REAL  *stack;
{
    if(*stack < 0) return(L_NAN);
    return((L_REAL)sqrt((double)*stack));
}
/*NH*/
L_REAL L_int(stack)
L_REAL  *stack;
{
    return((L_REAL)floor((double)(0.5 + *stack))); /* JMP38 27-10-92 */
}
/*NH*/
L_REAL L_floor(stack)
L_REAL  *stack;
{
    return((L_REAL)floor((double)(*stack))); /* JMP 18-10-2004 */
}
L_REAL L_ceil(stack)
L_REAL  *stack;
{
    return((L_REAL)(1.0 + floor((double)(*stack)))); /* JMP 18-10-2004 */
}
/*NH*/
L_REAL L_abs(stack)
L_REAL  *stack;
{
    return((L_REAL)fabs((double)*stack));
}
/*NH*/
L_REAL L_round(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    double cf = 0, val = *stack;

    if(nargs > 1) {
        cf = (int) (*stack);
        val = *(stack - 1);
    }
    cf = pow(10, cf);
    val = floor(0.5 + val * cf);
    return((L_REAL)(val / cf));
}
/*NH*/
L_REAL L_sign(stack)
L_REAL  *stack;
{
    if(*stack < 0) return((L_REAL)-1.0);
    else           return((L_REAL)1.0);
}
/*NH*/
L_REAL L_rad(stack)
L_REAL  *stack;
{
    return((L_REAL)(((double)(*stack)) * M_PI / 180.0));
}
/*NH*/
L_REAL L_if(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    double  cond = *(stack - 2);

    if(!L_ISAN(cond) || fabs(cond) < 1e-15)  return(*stack); /* JMP 14-06-07 */
    else                                     return(*(stack - 1));
}

/*NH*/
L_REAL L_fnisan(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    return((L_REAL)L_ISAN(*stack));
}

/*NH*/
L_REAL L_lcount(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    return((L_REAL)nargs);
}

/*NH*/
L_REAL L_lprod(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    L_REAL  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        m *= *(stack - j);
    return(m);
}

/*NH*/
L_REAL L_lsum(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    L_REAL  m;
    int     j;

    m = *stack;
    for(j = 1 ; j < nargs ; j++)
        m += *(stack - j);
    return(m);
}

/*NH*/
L_REAL L_lstderr(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    L_REAL  m = 0, x, mean;
    int     j, no = 0;

    mean = L_lmean(stack, nargs);
    for(j = 0 ; j < nargs ; j++) {
        if(L_ISAN(*(stack - j))) {             /* JMP 03-03-99 */
            x = *(stack - j) - mean;
            m += x * x;
            no++;
        }
    }
    if(no > 1) return(sqrt(m / (no - 1)));
    else return(L_NAN);
}

/*NH*/
L_REAL L_lmean(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    L_REAL  m = 0.0;
    int     j, no = 0;

    for(j = 0 ; j < nargs ; j++) {
        if(L_ISAN(*(stack - j))) {             /* JMP 03-03-99 */
            m += *(stack - j);
            no++;
        }
    }
    if(no) return(m / no);
    else return(L_NAN);
}
/*NH*/
L_REAL L_random(stack)
L_REAL  *stack;
{
    static int init = 0;
    double  x, s = *stack;

    if(s == 0.0) return(0.0);
    if(init == 0) {
        srand((unsigned)time(NULL));
        init = 1;
    }

    x = (rand() - (double)RAND_MAX / 2) / (double)RAND_MAX;
    return(x * s);
}

L_REAL L_urandom(stack)
L_REAL  *stack;
{
    static int init = 0;
    double  x, s = *stack;

    if(s == 0.0) return(0.0);
    if(init == 0) {
        srand((unsigned)time(NULL));
        init = 1;
    }

    x = (double) rand()/((double)RAND_MAX + 1.0);
    return(x * s);
}

extern double randBoxMuller(double m, double s);

L_REAL L_grandom(stack)
L_REAL  *stack;
{
    double  s = *stack, m = *(stack - 1);

    return(randBoxMuller(m, s));
}

extern double dgamma(double x);

L_REAL L_gamma(stack)
L_REAL  *stack;
{
    double  s = *stack;

    return(dgamma(s));
}

/*NH*/
L_REAL L_or(a, b)
L_REAL a, b;
{
    return((a || b) ? (L_REAL)1.0 : (L_REAL)0.0);
}
/*NH*/
L_REAL L_and(a, b)
L_REAL a, b;
{
    return((a && b) ? (L_REAL)1.0 : (L_REAL)0.0);
}
/*NH*/
L_REAL L_ge(a, b)
L_REAL a, b;
{
    return((a >= b) ? (L_REAL)1.0 : (L_REAL)0.0);
}
/*NH*/
L_REAL L_gt(a, b)
L_REAL a, b;
{
    return((a > b) ? (L_REAL)1.0 : (L_REAL)0.0);
}
/*NH*/
L_REAL L_eq(a, b)
L_REAL a, b;
{
    return((a == b) ? (L_REAL)1.0 : (L_REAL)0.0);
}
/*NH*/
L_REAL L_ne(a, b)
L_REAL a, b;
{
    return((a != b) ? (L_REAL)1.0 : (L_REAL)0.0);
}
/*NH*/
L_REAL L_le(a, b)
L_REAL a, b;
{
    return((a <= b) ? (L_REAL)1.0 : (L_REAL)0.0);
}
/*NH*/
L_REAL L_lt(a, b)
L_REAL a, b;
{
    return((a < b) ? (L_REAL)1.0 : (L_REAL)0.0);
}
/*NH*/
L_REAL L_plus(a, b)
L_REAL a, b;
{
    return(a + b);
}
/*NH*/
L_REAL L_minus(a, b)
L_REAL a, b;
{
    return(a - b);
}
/*NH*/
L_REAL L_divide(a, b)
L_REAL a, b;
{
    if(!L_ISAN(b) || !L_ISAN(a) || b == 0) {
        return((L_REAL)L_NAN);
        /*        L_errno = L_DIVIDE_ERR;
        	longjmp(L_JMP, 1);
        */
    }
    return(a / b);
}
/*NH*/
L_REAL L_div0(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    double a = *(stack - 1);
    double b = *stack;

    if(!L_ISAN(b) || !L_ISAN(a)) return((L_REAL)L_NAN);
    else if(b == 0) return(0);
    return(a / b);
}
/*NH*/
L_REAL L_times(a, b)
L_REAL a, b;
{
    return(a * b);
}

/*NH*/
L_REAL L_exp(a, b)
L_REAL a, b;
{
    double x, ib;
    
    if(a < 0 && b != (int)b) {
    // test 2018 : si (-a)^0.333 => faire -a^0.333
        ib = 1 / b;
        if(fabs(ib - (int) ib) > 1e-8) return(L_NAN);
        if((int)ib % 2 != 1)           return(L_NAN);
        x = -pow((double)-a, (double)b);
    }
     else 
        x = pow((double)a, (double)b);
    if(_isnan(x)) x = L_NAN; /* JMP 18-01-02 */
    return((L_REAL)x);
}

L_REAL L_expn(stack, nargs)
L_REAL  *stack;
int     nargs;
{
    double  x, a, b;

    if(nargs == 1) {
        if(*stack >= DMAXEXP || *stack <= DMINEXP) return(L_NAN);
        x = exp(*stack);
        if(x >= MAXDOUBLE || x <= MINDOUBLE) return(L_NAN);
        return(x);
    }
    else {
        a = *(stack - 1);
        b = *stack;
        return(L_exp(a, b));
        /* ancienne version pre-2018
        if(a < 0 && b != (int)b) return(L_NAN);
        x = (L_REAL)pow((double)(*(stack - 1)), (double)(*stack)); // JMP 18-01-02 
        if(_isnan(x)) x = L_NAN;
        if(x >= MAXDOUBLE || x <= MINDOUBLE) x = L_NAN;
        return((L_REAL)x);
        */
    }
}

/*NH*/
L_REAL L_pi()
{
    return((L_REAL)M_PI);
}
/*NH*/
L_REAL L_euro()
{
    return((L_REAL)40.3399); /* JMP 04-01-99 */
}
/*NH*/
L_REAL L_e()
{
    return((L_REAL)M_E);
}
/*NH*/
L_REAL L_time(t)
int t;
{
    return((L_REAL)t);
}

L_REAL L_i(t)
int t;
{
    return((L_REAL)t - (L_REAL)L_curt);
}

/*NH*/
/* ============ TFN ============ */

L_REAL L_lag(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    return(L_exec_sub(expr, len, t - lag, stack));
}

/*NH*/
L_REAL L_diff(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  v1, v2;
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    v1 = L_exec_sub(expr, len, t, stack);
    if(!L_ISAN(v1)) return(L_NAN);
    v2 = L_exec_sub(expr, len, t - lag, stack);
    if(!L_ISAN(v2)) return(L_NAN);
    return(v1 - v2);
}

/*NH*/
L_REAL L_rapp(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  v1, v2;
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    v1 = L_exec_sub(expr, len, t, stack);
    if(!L_ISAN(v1)) return(L_NAN);
    v2 = L_exec_sub(expr, len, t - lag, stack);
    if(!L_ISAN(v2)) return(L_NAN);
    return(L_divide(v1, v2));
}

/*NH*/
L_REAL L_dln(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  v1, v2;
    int     lag = 1;

    if(nargs == 2) lag = L_intlag(*stack); /* JMP38 27-10-92 */
    v1 = L_exec_sub(expr, len, t, stack);
    if(!L_ISAN(v1)) return(L_NAN);
    v2 = L_exec_sub(expr, len, t - lag, stack);
    if(!L_ISAN(v2)) return(L_NAN);
    return(L_logn(L_divide(v1,v2)));
}

/*NH*/
L_REAL L_grt(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  v1;

    v1 = L_rapp(expr, len, t, stack, nargs);
    if(!L_ISAN(v1)) return(L_NAN);
    return((L_REAL)((v1 - 1.0) * 100.0));
}

/*NH*/
L_REAL L_mavg(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  v1 = 0.0, tmp;
    int     n = 1, j;

    if(nargs == 2) n = L_intlag(*stack); /* JMP38 27-10-92 */
    n = (n < 1) ? 1 : n;
    for(j = 0 ; j < n ; j++) {
        tmp = L_exec_sub(expr, len, t - j, stack);
        if(!L_ISAN(tmp)) return(L_NAN);
        v1 += tmp;
    }
    return((L_REAL)(v1 / n));
}

/*NH*/
L_tfn_args(t, stack, nargs, from, to)
int     t;
L_REAL  *stack;
int     nargs,
        *from, *to;
{
    int     j;

    *from = 0;
    *to = t;
    if(nargs == 1) return(0);
    *from = L_intlag(*stack);
    if(nargs == 3) *to = L_intlag(*(stack - 1));

    if(*from > *to) {
        j = *from;
        *from = *to;
        *to = j;
    }
    return(0);
}

/*NH*/
L_REAL L_vmax(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  v1, v2;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    v1 = L_exec_sub(expr, len, from, stack);
    if(!L_ISAN(v1)) return(L_NAN);
    for(j = from + 1 ; j <= to ; j++) {
        v2 = L_exec_sub(expr, len, j, stack);
        if(!L_ISAN(v2)) return(L_NAN);
        if(v2 > v1) v1 = v2;
    }
    return(v1);
}

/*NH*/
L_REAL L_vmin(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  v1, v2;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    v1 = L_exec_sub(expr, len, from, stack);
    for(j = from + 1 ; j <= to ; j++) {
        v2 = L_exec_sub(expr, len, j, stack);
        if(!L_ISAN(v2)) return(L_NAN);
        if(v2 < v1) v1 = v2;
    }
    return(v1);
}

/*NH*/
L_REAL L_sum(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL    v1 = 0.0, tmp;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = from ; j <= to ; j++) {
        tmp = L_exec_sub(expr, len, j, stack);
        if(!L_ISAN(tmp)) return(L_NAN);
        v1 += tmp;
    }
    return(v1);
}

/*NH*/
L_REAL L_prod(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  v1 = 1.0, tmp;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = from ; j <= to ; j++) {
        tmp = L_exec_sub(expr, len, j, stack);
        if(!L_ISAN(tmp)) return(L_NAN);
        v1 *= tmp;
    }
    return(v1);
}

/*NH*/
L_REAL L_mean(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  v1;
    int     from, to;

    L_tfn_args(t, stack, nargs, &from, &to);
    v1 = L_sum(expr, len, t, stack, nargs);
    if(!L_ISAN(v1)) return(L_NAN);
    v1 /= (1 + to - from);
    return(v1);
}


/*NH*/
L_REAL L_stderr(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  s = 0.0, s2 = 0.0, v1;
    int     from, to, j, n;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = from ; j <= to ; j++) {
        v1  = L_exec_sub(expr, len, j, stack);
        if(!L_ISAN(v1)) return(L_NAN);
        s  += v1;
        s2 += v1 * v1;
    }
    n = 1 + to - from;
    if(n <= 1 || (s2 - s * s / n) < 0) return(L_NAN);  /* JMP 20-04-98 */
    v1 = sqrt((s2 - s * s / n) / (n - 1));             /* JMP 20-04-98 */

    return(v1);
}

/*NH*/
L_REAL L_lastobs(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  tmp;
    int     from, to, j;

    L_tfn_args(t, stack, nargs, &from, &to);
    for(j = to ; j >= from ; j--) {
        tmp = L_exec_sub(expr, len, j, stack);
        if(L_ISAN(tmp)) return(tmp);
    }
    return(L_NAN);
}


/*NH*/
L_REAL L_calccorr(expr1, len1, expr2, len2, t, stack, nargs)  /* JMP 17-04-98 */
unsigned char    *expr1, *expr2;
short   len1, len2;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  sxx = 0.0, syy = 0.0, sxy = 0.0, vx, vy, meanx, meany;
    int     from, to, j, n;

    meanx = L_mean(expr1, len1, t, stack, nargs - 1);
    if(!L_ISAN(meanx)) return(L_NAN);
    meany = L_mean(expr2, len2, t, stack, nargs - 1);
    if(!L_ISAN(meany)) return(L_NAN);

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    for(j = from ; j <= to ; j++) {
        vx = L_exec_sub(expr1, len1, j, stack);
        if(!L_ISAN(vx)) return(L_NAN);
        vy = L_exec_sub(expr2, len2, j, stack);
        if(!L_ISAN(vy)) return(L_NAN);

        sxx += (vx - meanx) * (vx - meanx);
        syy += (vy - meany) * (vy - meany);
        sxy += (vx - meanx) * (vy - meany);
    }

    if(sxx * syy <= 0) return(L_NAN);
    return(sxy / sqrt(sxx * syy));
}

/*NH*/
L_REAL L_corr(expr, nvargs, t, stack, nargs)  /* JMP 17-04-98 */
unsigned char    *expr;
short   nvargs;
int     t;
L_REAL  *stack;
int     nargs;
{
    char    *expr1, *expr2;
    short   len1, len2;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    return(L_calccorr(expr + sizeof(short), len1,
                      expr + len1 + 2 * sizeof(short), len2,
                      t, stack, nargs));
}

/*NH*/
L_REAL L_calccovar(expr1, len1, expr2, len2, t, stack, nargs, orig)  /* JMP 17-04-98 */
unsigned char    *expr1, *expr2;
short   len1, len2;
int     t;
L_REAL  *stack;
int     nargs, orig;
{
    L_REAL  sxx = 0.0, syy = 0.0, sxy = 0.0, vx, vy, meanx, meany;
    int     from, to, j, n;

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    n = 1 + to - from;
    if(n == 0) return(L_NAN);

    meanx = L_mean(expr1, len1, t, stack, nargs - 1);
    if(!L_ISAN(meanx)) return(L_NAN);
    meany = L_mean(expr2, len2, t, stack, nargs - 1);
    if(!L_ISAN(meany)) return(L_NAN);

    for(j = from ; j <= to ; j++) {
        vx = L_exec_sub(expr1, len1, j, stack);
        if(!L_ISAN(vx)) return(L_NAN);
        vy = L_exec_sub(expr2, len2, j, stack);
        if(!L_ISAN(vy)) return(L_NAN);

        if(orig) sxy += vx * vy;
        else     sxy += (vx - meanx) * (vy - meany);
    }

    return(sxy / n);
}


/*NH*/
L_REAL L_covar(expr, nvargs, t, stack, nargs)  /* JMP 17-04-98 */
unsigned char    *expr;
short   nvargs;
int     t;
L_REAL  *stack;
int     nargs;
{
    char    *expr1, *expr2;
    short   len1, len2;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    return(L_calccovar(expr + sizeof(short), len1,
                       expr + len1 + 2 * sizeof(short), len2,
                       t, stack, nargs, 0));
}

/*NH*/
L_REAL L_covar0(expr, nvargs, t, stack, nargs)  /* JMP 17-04-98 */
unsigned char    *expr;
short   nvargs;
int     t;
L_REAL  *stack;
int     nargs;
{
    char    *expr1, *expr2;
    short   len1, len2;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    return(L_calccovar(expr + sizeof(short), len1,
                       expr + len1 + 2 * sizeof(short), len2,
                       t, stack, nargs, 1));
}

/*NH*/
L_REAL L_var(expr, nvargs, t, stack, nargs)  /* JMP 17-04-98 */
unsigned char    *expr;
short   nvargs;
int     t;
L_REAL  *stack;
int     nargs;
{
    char    *expr1;
    short   len1;

    memcpy(&len1, expr, sizeof(short));
    return(L_calccovar(expr + sizeof(short), len1,
                       expr + sizeof(short), len1,
                       t, stack, nargs + 1, 0));
}

/*NH*/
L_REAL L_stddev(expr, nvargs, t, stack, nargs)  /* JMP 17-04-98 */
unsigned char    *expr;
short   nvargs;
int     t;
L_REAL  *stack;
int     nargs;
{
    L_REAL  v1;

    v1 = L_var(expr, nvargs, t, stack, nargs);
    if(!L_ISAN(v1) || v1 < 0) return(L_NAN);
    else return(sqrt(v1));
}

/*NH*/
L_REAL L_index(expr, nvargs, t, stack, nargs)  /* JMP 17-04-98 */
unsigned char    *expr;
short   nvargs;
int     t;
L_REAL  *stack;
int     nargs;
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     from, to, j, n;
    L_REAL  vx, vy;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr1 = expr + sizeof(short);
    expr2 = expr + len1 + 2 * sizeof(short);

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    n = 1 + to - from;
    if(n == 0) return(L_NAN);

    vx = L_exec_sub(expr1, len1, t, stack);
    if(!L_ISAN(vx)) return(L_NAN);

    for(j = from ; j <= to ; j++) {
        vy = L_exec_sub(expr2, len2, j, stack);
        if(!L_ISAN(vy)) return(L_NAN);

        if(fabs(vx - vy) < 1e-30) return((L_REAL)j);
    }

    return(L_NAN);
}

/*NH*/
L_REAL L_acf(expr, nvargs, t, stack, nargs)  /* JMP 17-04-98 */
unsigned char    *expr;
short   nvargs;
int     t;
L_REAL  *stack;
int     nargs;
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     from, to, j, n, k;
    L_REAL  vx, vy, meanx, sxy = 0.0, sxy0 = 0.0;

    memcpy(&len1, expr, sizeof(short));
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr1 = expr + sizeof(short);
    expr2 = expr + len1 + 2 * sizeof(short);

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    n = 1 + to - from;
    if(n == 0) return(L_NAN);

    vx = L_exec_sub(expr1, len1, t, stack);
    if(!L_ISAN(vx)) return(L_NAN);
    k = (int)vx;
    if(k < 0 || k > n / 4) return(L_NAN);

    meanx = L_mean(expr2, len2, t, stack, nargs - 1);
    if(!L_ISAN(meanx)) return(L_NAN);

    for(j = from ; j <= to - k ; j++) {
        vx = L_exec_sub(expr2, len2, j, stack);
        if(!L_ISAN(vx)) return(L_NAN);
        vy = L_exec_sub(expr2, len2, j + k, stack);
        if(!L_ISAN(vy)) return(L_NAN);

        sxy += (vx - meanx) * (vy - meanx);
    }

    for(j = from ; j <= to ; j++) {
        vx = L_exec_sub(expr2, len2, j, stack);
        if(!L_ISAN(vx)) return(L_NAN);
        sxy0 += (vx - meanx) * (vx - meanx);
    }

    return(sxy / sxy0);
}


L_stackna(p_stack, nargs)
L_REAL  **p_stack;
int     nargs;
{
    int     i;
    L_REAL  *stack = *p_stack;

    for(i = 0 ; i < nargs ; i++)
        if(!L_ISAN(*(stack - i))) {
            (*p_stack) -= nargs - 1;
            **p_stack = L_NAN;
            return(1);
        }
    return(0);
}

/*NH*/
/* Retourne dans vy et dans vt les valeurs d‚finies les plus proches autour
    de expr[t].
*/
L_calcvals(expr1, len1, t, stack, vt, vy, notnul)
unsigned char   *expr1;
short           len1;
int             t;
L_REAL          *stack;
L_REAL          *vy;
int             *vt;
{
    int     j, nobs;

    /* 2. Calc val after t */
    vy[0] = vy[1] = L_NAN;
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;
    for(vt[1] = t + 1 ; vt[1] < nobs ; vt[1]++) {
        vy[1] = L_exec_sub(expr1, len1, vt[1], stack);
        if(L_ISAN(vy[1]) && (notnul == 0 || fabs(vy[1]) > 1e-15)) break;
    }

    /* 3. Calc val before t */
    for(vt[0] = t - 1 ; vt[0] >= 0 ; vt[0]--) {
        vy[0] = L_exec_sub(expr1, len1, vt[0], stack);
        if(L_ISAN(vy[0]) && (notnul == 0 || fabs(vy[0]) > 1e-15)) break;
    }

    /* if NO value after AND before t, return L_NAN */
    if(!L_ISAN(vy[0]) && !L_ISAN(vy[1])) return(-1);
    if(L_ISAN(vy[0]) && L_ISAN(vy[1])) return(0);

    /* if no value after, calc before t0 */
    if(!L_ISAN(vy[1])) {
        vy[1] = vy[0];
        vt[1] = vt[0];
        vy[0] = L_NAN;
        for(vt[0] = vt[1] - 1 ; vt[0] >= 0 ; vt[0]--) {
            vy[0] = L_exec_sub(expr1, len1, vt[0], stack);
            if(L_ISAN(vy[0]) && (notnul == 0 || fabs(vy[0]) > 1e-15)) break;
        }
    }

    /* if no value before, calc after t1 */
    else {
        vy[0] = vy[1];
        vt[0] = vt[1];
        vy[1] = L_NAN;
        for(vt[1] = vt[0] + 1 ; vt[1] < nobs ; vt[1]++) {
            vy[1] = L_exec_sub(expr1, len1, vt[1], stack);
            if(L_ISAN(vy[1])) break;
        }
    }

    return(0);
}

/*NH*/
L_REAL L_interpol(expr, nvargs, t, stack, nargs)  /* JMP 17-04-98 */
unsigned char    *expr;
short   nvargs;
int     t;
L_REAL  *stack;
int     nargs;
{
    char    *expr1;
    short   len1;
    int     j, nobs, vt[2];
    L_REAL  vx, vy[2], itc;

    memcpy(&len1, expr, sizeof(short));
    expr1 = expr + sizeof(short);

    /* 1. Calc val in t */
    vx = L_exec_sub(expr1, len1, t, stack);
    if(L_ISAN(vx)) return(vx);

    /* 2. Calc values around t */
    L_calcvals(expr1, len1, t, stack, vt, vy, 0);
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;

    /* 3. Calc result */
    if(!L_ISAN(vy[0]) && !L_ISAN(vy[1])) return(L_NAN);
    if(vt[0] < 0) return(vy[1]);
    if(vt[1] >= nobs) return(vy[0]);
    itc = (vy[0] * vt[1] - vy[1] * vt[0]) / (vt[1] - vt[0]);
    return(itc + t * (vy[1] - vy[0]) / (vt[1] - vt[0]));
}

/*NH*/
L_REAL L_app(expr, nvargs, t, stack, nargs)  /* JMP 17-04-98 */
unsigned char    *expr;
short   nvargs;
int     t;
L_REAL  *stack;
int     nargs;
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     j, nobs, vt[2];
    L_REAL  vx, vy[2], itc, ayt, ay[2], delta;

    memcpy(&len1, expr, sizeof(short));
    expr1 = expr + sizeof(short);
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr2 = expr + len1 + 2 * sizeof(short);

    /* 1. Calc val in t */
    vx = L_exec_sub(expr1, len1, t, stack);
    if(L_ISAN(vx)) return(vx);

    /* 2. Calc values around t */
    L_calcvals(expr1, len1, t, stack, vt, vy, 1);
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;

    /* if NO value after AND before t, return L_NAN */
    if(!L_ISAN(vy[0]) && !L_ISAN(vy[1])) return(L_NAN);

    /* Valeurs apparent‚es */
    ayt = L_exec_sub(expr2, len2, t, stack);
    if(!L_ISAN(ayt)) return(L_NAN);
    ay[0] = ay[1] = L_NAN; /* JMP 19-07-07 */
    if(vt[0] >= 0)   ay[0] = L_exec_sub(expr2, len2, vt[0], stack);
    if(vt[1] < nobs) ay[1] = L_exec_sub(expr2, len2, vt[1], stack);

    // Deux valeurs trouv‚es dans la s‚rie initiale
    // !! Les deux valeurs doivent exister dans la s‚rie apparent‚e
    if(L_ISAN(ay[0]) && L_ISAN(ay[1])) {
        if(vt[0] < t && vt[1] > t) {
            /*            return(ayt * (vy[0] / ay[0] + vy[1] / ay[1]) / 2); */
            // delta = (vy[1]/vy[0])/(ay[1]/ay[0]);
            if(fabs(vy[0] * ay[1]) < 1e-15) return(L_NAN);
            delta = (vy[1] * ay[0]) / (vy[0] * ay[1]);
            if(delta < 0) return(L_NAN);
            delta = pow(delta, 1.0/(vt[1]-vt[0]));
            if(_isnan(delta)) return(L_NAN); /* JMP 18-01-02 */
            return(vy[0] * (ayt/ay[0] * pow(delta, t - vt[0]) ));
        }
        else {
            j = 0;
            if(vt[0] < t) j = 1;
            if(fabs(ay[j]) < 1e-15) return(L_NAN); /* JMP 19-07-07 */
            return(ayt * (vy[j] / ay[j]));
        }
    }
    // Seule la valeur en t0 est d‚finie : res(t) <- APP(t) * (ORIG(t0) / APP(t0))
    if(L_ISAN(ay[0])) {
        if(fabs(ay[0]) < 1e-15) return(L_NAN); /* JMP 19-07-07 */
        return(ayt * (vy[0] / ay[0]));
    }
    // Seule la valeur en t1 est d‚finie : res(t) <- APP(t) * (ORIG(t1) / APP(t1))
    if(L_ISAN(ay[1])) {
        if(fabs(ay[1]) < 1e-15) return(L_NAN); /* JMP 19-07-07 */
        return(ayt * (vy[1] / ay[1]));
    }
    return(L_NAN);
}

/*NH*/
L_REAL L_dapp(expr, nvargs, t, stack, nargs)  /* GB 14-11-00 */
unsigned char    *expr;
short   nvargs;
int     t;
L_REAL  *stack;
int     nargs;
{
    char    *expr1, *expr2;
    short   len1, len2;
    int     j, nobs, vt[2];
    L_REAL  vx, vy[2], itc, ayt, ay[2], delta;

    memcpy(&len1, expr, sizeof(short));
    expr1 = expr + sizeof(short);
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr2 = expr + len1 + 2 * sizeof(short);

    /* 1. Calc val in t */
    vx = L_exec_sub(expr1, len1, t, stack);
    if(L_ISAN(vx)) return(vx);

    /* 2. Calc values around t */
    L_calcvals(expr1, len1, t, stack, vt, vy, 0);
    nobs = (L_getsmpl(L_EXEC_DBV))->s_nb;

    /* if NO value after AND before t, return L_NAN */
    if(!L_ISAN(vy[0]) && !L_ISAN(vy[1])) return(L_NAN);

    /* Valeurs apparent‚es */
    ayt = L_exec_sub(expr2, len2, t, stack);
    if(!L_ISAN(ayt)) return(L_NAN);
    if(vt[0] >= 0)   ay[0] = L_exec_sub(expr2, len2, vt[0], stack);
    if(vt[1] < nobs) ay[1] = L_exec_sub(expr2, len2, vt[1], stack);

    if(L_ISAN(ay[0]) && L_ISAN(ay[1])) {
        if(vt[0] < t && vt[1] > t) {
            delta = (ay[1] - ay[0] + vy[0] - vy[1]) / (vt[1]-vt[0]);
            return(ayt + vy[0] - ay[0] - (t - vt[0]) * delta);
        }
        else {
            j = 0;
            if(vt[0] < t) j = 1;
            return(ayt + (vy[j] - ay[j]));
        }
    }
    if(L_ISAN(ay[0]))                return(ayt + (vy[0] - ay[0]));
    if(L_ISAN(ay[1]))                return(ayt + (vy[1] - ay[1]));
    return(L_NAN);
}

/*NH*/
L_REAL L_hpall(expr, len, t, stack, nargs, std)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs, std;
{
    char    *expr1, *expr2;
    short   len1, len2;

    IODE_REAL    v, *itmp = NULL, *otmp = NULL; 
    //int     from, to, j, lg, lambda, nbna, dim;      /* JMP 7-3-2019 */
    int     from, to, j, lg, nbna, dim;         /* JMP 7-3-2019 */
    IODE_REAL   lambda;                         /* JMP 7-3-2019 */
    
    memcpy(&len1, expr, sizeof(short));
    expr1 = expr + sizeof(short);
    memcpy(&len2, expr + len1 + sizeof(short), sizeof(short));
    expr2 = expr + len1 + 2 * sizeof(short);

    v = L_exec_sub(expr1, len1, t, stack);
    if(!L_ISAN(v)) return(L_NAN);
    lambda = v;

    L_tfn_args(t, stack, nargs - 1, &from, &to);
    if(t < from || t > to) goto err;
    lg = to - from + 1;
    if(lg < 5) goto err;
    itmp = SCR_malloc(lg * sizeof(IODE_REAL));
    otmp = SCR_malloc(lg * sizeof(IODE_REAL));

    if(itmp == NULL || otmp == NULL) goto err;
    for(j = from ; j <= to ; j++) {
        itmp[j - from] = L_exec_sub(expr2, len2, j, stack);
        // if(!L_ISAN(itmp[j - from])) goto err;            /* JMP 26-07-11 */
    }

    /*
        for(j = from, nbna = 0 ; j <= to ; j++, nbna++) {
    	if(L_ISAN(itmp[j - from])) break;
        }
        if(lg - nbna < 5) goto err;
    */

    HP_test(itmp, otmp, lg, &nbna, &dim);           /* JMP 26-07-11 */
    if(dim < 5) goto err;                           /* JMP 26-07-11 */
    HP_calc(itmp + nbna, otmp + nbna, dim, lambda, std); /* JMP 26-07-11 */ // JMP 12/4/2019 

    SCR_free(itmp);
    v = otmp[t - from];
    SCR_free(otmp);
    //DebugActif = 1;
    //Debug("t=%d, lambda=%lf, v=%lf, from=%d, to=%d\n", t, lambda, v, from, to);
    return(v);

err:
    SCR_free(itmp);
    SCR_free(otmp);
    return(L_NAN);
}

/*NH*/
L_REAL L_hp(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    return(L_hpall(expr, len, t, stack, nargs, 0));
}

/*NH*/
L_REAL L_hpstd(expr, len, t, stack, nargs)
unsigned char    *expr;
short   len;
int     t;
L_REAL  *stack;
int     nargs;
{
    return(L_hpall(expr, len, t, stack, nargs, 1));
}






/**
 * @header4iode
 *
 * Functions to evaluate a compiled and linked LEC expression. 
 *  
 *   - L_REAL L_exec_sub(unsigned char* expr, int lg, int t, L_REAL* stack)     Execution of a CLEC sub expression.
 *   - L_REAL L_exec(KDB* dbv, KDB* dbs, CLEC* expr, int t)                     Execution of a compiled and linked CLEC expression.
 *   - L_REAL* L_cc_link_exec(char* lec, KDB* dbv, KDB* dbs)                    Compiles, links and executes a LEC expression.
 */

#include "iode.h"
#include <setjmp.h>
#include <signal.h>
#include <time.h>

#ifdef __GNUC__
    #define _isnan isnan
#endif

/**
 *  Tables of LEC sub-functions pointers. Each function returns a L_REAL.
 *  
 *  The number and type of the function parameters depend on the group to which they belong.
 *  Five groups of functions are defined. For each group, a table of pointers to these functions
 *  is created. 
 *  
 *      - L_OPS_FN[]:  operators                   Syntax: L_REAL fn(L_REAL val1, L_REAL val2)
 *      - L_FNS_FN[]:  simple functions            Syntax: L_REAL fn(L_REAL *stack, int nargs])
 *      - L_TFN_FN[]:  time functions              Syntax: L_REAL fn(char* expr, short length, int t, L_REAL *stack, int nargs)
 *      - L_MTFN_FN[]: multi-args time functions   Syntax: L_REAL fn(char* expr, int nvargs, int t, L_REAL *stack, int nargs)
 *      - L_VAL_FN[]:  values                      Syntax: L_REAL fn(int t)
 */
extern L_REAL(*L_VAL_FN[])();       // l_exec_val.c
extern L_REAL(*L_OPS_FN[])();       // l_exec_ops.c
extern L_REAL(*L_FNS_FN[])();       // l_exec_fns.c
extern L_REAL(*L_TFN_FN[])();       // l_exec_tfn.c
extern L_REAL(*L_MTFN_FN[])();      // l_exec_mtfn.c    

// Globals
static LNAME    *L_EXEC_NAMES;
static int      L_SIG = 0;
static jmp_buf  L_JMP;

// Globals used in L_* fns
KDB     *L_EXEC_DBV,    
        *L_EXEC_DBS; 
int     L_curt;         // current value of t

// Math exceptions trap
#ifdef _MSC_VER
int _matherr(struct _exception *e)
{
    e->retval = (double) IODE_NAN;
    return(1);
}
#else
int matherr(struct exception *e)
{
    e->retval = (double) IODE_NAN;
    return(1);
}
#endif

// Function called on floating point exception (SIGFPE)
void L_fperror()
{
    L_SIG = 0;
#ifdef DOS
    _fpreset();
#endif
    longjmp(L_JMP, 1);
}

/**
 *  Checks if at least one of the last nargs values on the stack is IODE_NAN.
 *  If it is the case, go back nargs-1 steps on the stack and set IODE_NAN on the new top of the stack
 *  Ex.: 
 *      if the stack is {IODE_NAN, 2, 3...}
 *  after the call to L_stackna(&stack, 2)
 *      the stack will be {IODE_NAN, 3...}
 *      
 *  @param [in, out] p_stack     L_REAL**    pointer to the pointer to the stack
 *  @param [in]      nargs       int         number of arguments
 *  @return                      int         1 if IODE_NAN has been found and the stack is modified
 *                                           0 otherwise
 */
int L_stackna(L_REAL** p_stack, int nargs)
{
    int     i;
    L_REAL  *stack = *p_stack;

    for(i = 0 ; i < nargs ; i++)
        if(!IODE_IS_A_NUMBER(*(stack - i))) {
            (*p_stack) -= nargs - 1;
            **p_stack = IODE_NAN;
            return(1);
        }
    return(0);
}


/**
 *  Execution of a CLEC sub expression.
 * 
 *  The variable, scalar and sample values are retrieved by calling 3 user defined functions (see also k_lec.c):
 * 
 *      double*  L_getvar(dbv, pos) : returns the pointer to the variable at position pos in dbv.
 *      double   L_getscl(dbs, pos) : returns the value of the scalar at position pos in dbs.
 *      SMPL*       L_getsmpl(dbv)     : returns a pointer to the sample of dbv, the database of variables
 *
 *  The process iterates on the compiled LEC expression. 
 *      - the first byte (keyw) is an identifier (+, ln, VAR, ...)
 *      - according to the identifier, one or more values are read from the stack, 
 *          the function/operator is called and the result is placed on the stack
 *      
 *  @param [in] expr    unsigned char*  pointer to the beginning of the sub expression (heterogeneous container)
 *  @param [in] lg      int             length of the sub expression
 *  @param [in] t       int             time (index in dbv) of execution
 *  @param [in] stack   L_REAL*         execution stack
 *  @return             L_REAL          result of the computation
 *  
 */
L_REAL L_exec_sub(unsigned char* expr, int lg, int t, L_REAL* stack)
{
    double    *d_ptr;
    LECREAL r;
    int     j, nargs, keyw, nvargs;
    long    l;
    short   len, s;
    CVAR    cvar;

    for(j = 0 ; j < lg ;) {
        keyw = expr[j++];
        if(keyw == L_VAR || keyw == L_VART) {
            memcpy(&cvar, expr + j, sizeof(CVAR));
            d_ptr = L_getvar(L_EXEC_DBV, L_EXEC_NAMES[cvar.pos].pos);
            j += sizeof(CVAR);
            len = cvar.ref;
            if(cvar.per.p_y == 0)  len += t;
            stack++;
            if(len < 0 || len >= (L_getsmpl(L_EXEC_DBV))->s_nb) {
                *stack = IODE_NAN;
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
                    return((double)IODE_NAN);
            }
        /*        if(*stack <= IODE_NAN) return((double)IODE_NAN); */
    }
    return((double)*stack);
}


/**
 *  Execution of a compiled and linked CLEC expression.
 *  
 *  - Assign global variables that could be used by calculation functions.
 *  - initiate the exception handling on floating point errors
 *  - call L_exec_sub() which is the real (recursive) calculator
 *  
 *  @param [in] dbv  KDB*       input variable KDB
 *  @param [in] dbs  KDB*       input scalars KDB    
 *  @param [in] expr CLEC*      compiled and linked (with dbv and dbs) LEC
 *  @param [in] t    int        time of calculation (index in dbv sample)
 *  @return          L_REAL     result of the calculation 
 *                              IODE_NAN on error (and L_errno is set)
 */
L_REAL L_exec(KDB* dbv, KDB* dbs, CLEC* expr, int t)
{
    int	pos; //short   pos;
    L_REAL  stack[1000];    // 1000 pour reculer certains plantages (non solutionné...) TODO: manage the stack overflow ?

    // Use globals to limit the number of parameters in function calls
    L_EXEC_DBV = dbv;   
    L_EXEC_DBS = dbs;
    L_EXEC_NAMES = &(expr->lnames[0]);
    L_curt = t;         // Global with the current t of execution
    
    // Reset _errno
    L_errno = 0;
    
    // Leave if empty CLEC
    if(expr == 0) return((double)IODE_NAN);
    
    // Set the handle on floating point exception to L_fperror
    if(L_SIG == 0) {
        signal(SIGFPE, L_fperror);
        L_SIG = 1;
    }
    
    // Set the point of return on FPE
    if(setjmp(L_JMP))
        return((double)IODE_NAN); // On FPE, return IODE_NAN
    
    pos = sizeof(CLEC) + (expr->nb_names - 1) * sizeof(LNAME);
    
    
    return(L_exec_sub((char *)expr + pos, expr->tot_lg - pos, t, stack));
}


/**
 *  Tranforms a value L_REAL in an integer by adding or substracting 0.5 and keeping the integer part.
 *  
 *  @param [in] lag L_REAL  real to transform into an integer
 *  @return         int     result
 *
 */
int L_intlag(L_REAL lag)
{
    int intlag;

    if(lag < 0) intlag = (int)(-0.5 + lag);
    else        intlag = (int)(0.5 + lag);

    return(intlag);
}

/**
 *  Utility function to retrieve one or 2 optional time values on the stack.
 *  Ex.: 
 *      vmax(1990Y1, 2000Y1, A+B):  from=1990Y1, to=2000Y1
 *      vmax(1990Y1, A+B):          from=1990Y1, to=t
 *      vmax(A+B):                  from=0,      to=t
 *  
 *  @param [in]  t      int     current value of t (in a simulation loop for example)
 *  @param [in]  stack  L_REAL  top of the stack
 *  @param [in]  nargs  int     nb of args passed to the function
 *  @param [out] from   int*    position in the sample of the "from" arg
 *  @param [out] to     int*    position in the sample of the "to" arg
 */
void L_tfn_args(int t, L_REAL* stack, int nargs, int* from, int* to)
{
    int     j;

    *from = 0;
    *to = t;
    if(nargs == 1) return;
    *from = L_intlag(*stack);
    if(nargs == 3) *to = L_intlag(*(stack - 1));

    if(*from > *to) {
        j = *from;
        *from = *to;
        *to = j;
    }
}


/**
 *  Compiles, links and executes a LEC expression.
 *  
 *  @param [in] char*       lec     LEC expression 
 *  @param [in] KDB*        dbv     KDB of variables used in lec 
 *  @param [in] KDB*        dbs     KDB of SCL used in lec
 *  @return     double*          calculated lec expression on the whole dbv sample    
 *                                  NULL on error (error can be retrieved via a call to L_error()
 */
L_REAL* L_cc_link_exec(char* lec, KDB* dbv, KDB* dbs)
{
    int      t, nb;
    CLEC     *clec = 0;
    L_REAL   *vec = NULL;

    
    if(lec == 0 || lec[0] == 0) return(vec);
    clec = L_cc(lec);
    if(clec != 0 && !L_link(dbv, dbs, clec)) {
        nb = KSMPL(dbv)->s_nb;
        vec = (double*) SW_nalloc(nb * sizeof(double));
        for(t = 0 ; t < nb ; t++) {
            vec[t] = L_exec(dbv, dbs, clec, t);
        }
    }

    SW_nfree(clec);
    return(vec);
}
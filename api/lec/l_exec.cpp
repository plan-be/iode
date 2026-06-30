#include <setjmp.h>
#include <signal.h>
#include <time.h>

#include "api/constants.h"
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/objs/variables.h"
#include "api/lec/lec.h"

#ifdef __GNUC__
    #define _isnan isnan
    #define _exception exception
#endif


/**
 *  Tables of LEC sub-functions pointers. Each function returns a double.
 *  
 *  The number and type of the function parameters depend on the group to which they belong.
 *  Five groups of functions are defined. For each group, a table of pointers to these functions
 *  is created. 
 *  
 *      - L_OPS_FN[]:  operators                   Syntax: double fn(double val1, double val2)
 *      - L_FNS_FN[]:  simple functions            Syntax: double fn(double *stack, [int nargs])
 *      - L_TFN_FN[]:  time functions              Syntax: double fn(char* expr, short length, int t, double *stack, int nargs)
 *      - L_MTFN_FN[]: variadic time functions     Syntax: double fn(char* expr, int nvargs, int t, double *stack, int nargs)
 *      - L_VAL_FN[]:  values                      Syntax: double fn(int t)
 */  

// Globals
static int L_SIG = 0;
static jmp_buf L_JMP;

// Math exceptions trap
#ifdef _MSC_VER
#include <math.h>
int _matherr(struct _exception *e)
{
    e->retval = (double) IODE_NAN;
    return(1);
}
#else
#include <math.h>
int matherr(struct exception *e)
{
    e->retval = (double) IODE_NAN;
    return(1);
}
#endif

// Function called on floating point exception (SIGFPE)
void L_fperror(int)
{
    L_SIG = 0;
#ifdef DOS
    _fpreset();
#endif
    longjmp(L_JMP, 1);
}

/**
 *  Checks if at least one of the last nargs values on the stack is IODE_NAN.
 *      
 *  @param [in, out] stack       double**    pointer to the pointer to the stack
 *  @return                      int         1 if IODE_NAN has been found and the stack is modified
 *                                           0 otherwise
 */
bool L_stack_is_nan(const std::deque<double>& stack)
{
    for(const double& value : stack)
    {
        if(!IODE_IS_A_NUMBER(value)) 
            return true;
    }

    return false;
}


static std::string get_l_exec_sub_error_message(unsigned char* expr, int t)
{
    std::string sub_expression = std::string((char*) expr);
    std::string error_msg = "Could not execute compiled LEC sub expression ";
    if(sub_expression.empty())
        error_msg += "at period position " + std::to_string(t) + ": empty expression";
    else
    {
        error_msg += "'" + sub_expression + "' at period position ";
        error_msg += std::to_string(t) + ": invalid expression";
    }
    return error_msg;
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
 *      - the first byte (type) is an identifier (+, ln, VAR, ...)
 *      - according to the identifier, one or more values are read from the stack, 
 *          the function/operator is called and the result is placed on the stack
 *      
 *  @param [in] expr    unsigned char*  pointer to the beginning of the sub expression (heterogeneous container)
 *  @param [in] lg      int             length of the sub expression
 *  @param [in] t       int             time (index in dbv) of execution
 *  @return             double          result of the computation
 *  
 */
double L_exec_sub(unsigned char* expr, int lg, int t)
{
    int j;
    int type;
    int previous_j;
    int pos_stack = 0;  
    std::deque<double> stack;
    for(j = 0; j < lg ;) 
    {
        type = expr[j];

        if(type == L_LCONST)
        {
            // extract LEC long constant from the buffer -> update j
            LEC_CONST_LONG al_lconst((unsigned char*) expr, j);
            // add the constant to the stack
            al_lconst.add_to_stack(stack);
        }
        else if(type == L_DCONST)
        {
            // extract LEC double constant from the buffer -> update j
            LEC_CONST_REAL al_dconst((unsigned char*) expr, j);
            // add the constant to the stack
            al_dconst.add_to_stack(stack); 
        }
        else if(type == L_COEF)
        {
            // extract LEC coefficient from the buffer -> update j
            LEC_COEF al_coef((unsigned char*) expr, j);
            // add the coefficient value to the stack
            bool ok = al_coef.add_to_stack(stack);
            if(!ok) 
            {
                std::string error_msg = get_l_exec_sub_error_message(expr, t);
                kerror(0, (char*) error_msg.c_str());
                return (double) IODE_NAN;
            }
        }
        else if(type == L_PERIOD)
        {
            // extract LEC Period from the buffer -> update j
            LEC_PERIOD al_period((unsigned char*) expr, j);
            // add the period position to the stack
            al_period.add_to_stack(stack);
        }
        // key = variable or variable[period] 
        else if(type == L_VAR || type == L_VART) 
        {
            // extract LEC Variable from the buffer -> update j
            LEC_VAR al_var((unsigned char*) expr, j);
            // add the variable value to the stack
            bool ok = al_var.add_to_stack(stack, t);
            if(!ok) 
            {
                std::string error_msg = get_l_exec_sub_error_message(expr, t);
                kerror(0, (char*) error_msg.c_str());
                return (double) IODE_NAN;
            }
        }

        // ---- executable LEC elements ----

        else if(is_fn(type)) 
        {
            // extract LEC function from the buffer -> update j
            LEC_FN al_fn((unsigned char*) expr, j);
            // execute the function on the stack
            al_fn.execute(stack);
        }
        else if(is_op(type))
        {
            // extract LEC operator from the buffer -> update j
            LEC_OP al_op((unsigned char*) expr, j);
            // execute the operator on the stack
            al_op.execute(stack);
        }
        else if(is_tfn(type)) 
        {
            previous_j = j;
            // extract LEC time function from the buffer -> update j
            LEC_TFN al_tfn((unsigned char*) expr, j);
            // execute the time function on the stack
            al_tfn.execute(expr, previous_j, t, stack);
        }
        else if(is_val(type)) 
        {
            // extract LEC value from the buffer -> update j
            LEC_VAL_FN al_val((unsigned char*) expr, j);
            // execute the value function on the stack
            al_val.execute(t, stack);
        }
        else if(is_mtfn(type)) 
        {
            previous_j = j;
            // extract LEC multi-time function from the buffer -> update j
            LEC_MTFN al_mtfn((unsigned char*) expr, j);
            // execute the multi-time function on the stack
            al_mtfn.execute(expr, previous_j, t, stack);
        }
        else 
        {
            std::string error_msg = "Could not execute compiled LEC sub expression: invalid atomic lec type " + std::to_string(type);
            kerror(0, (char*) error_msg.c_str());
            return (double) IODE_NAN;
        }
    }

    return stack.back();
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
 *  @return          double     result of the calculation 
 *                              IODE_NAN on error (and L_errno is set)
 */
double L_exec(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const std::shared_ptr<CLEC> clec, const int t)
{
    if(!clec) 
        return IODE_NAN;

    // leave if empty CLEC expression
    if(clec->expression == nullptr || clec->len_expr == 0)
        return IODE_NAN;

    // Use globals to limit the number of parameters in function calls
    L_EXEC_DBV = dbv;   
    L_EXEC_DBS = dbs;

    V_EXEC_POS.clear();
    for(const auto& [_, pos] : clec->objs)
        V_EXEC_POS.push_back(pos);
    
    L_curt = t;         // Global with the current t of execution
    
    // Reset _errno
    L_errno = 0;
    
    // Set the handle on floating point exception to L_fperror
    if(L_SIG == 0) 
    {
        signal(SIGFPE, L_fperror);
        L_SIG = 1;
    }
    
    // Set the point of return on FPE
    if(setjmp(L_JMP))
        return (double) IODE_NAN; // On FPE, return IODE_NAN
    
    double value = L_exec_sub(clec->expression, clec->len_expr, t);
    return value;
}


/**
 *  Tranforms a value double in an integer by adding or substracting 0.5 and keeping the integer part.
 *  
 *  @param [in] lag double  real to transform into an integer
 *  @return         int     result
 *
 */
int L_intlag(double lag)
{
    int intlag;
    if(lag < 0) 
        intlag = (int) (-0.5 + lag);
    else        
        intlag = (int) (0.5 + lag);
    return intlag;
}


/**
 *  Compiles, links and executes a LEC expression.
 *  
 *  @param [in] char*       lec     LEC expression 
 *  @param [in] KDB*        dbv     KDB of variables used in lec 
 *  @param [in] KDB*        dbs     KDB of Scalar used in lec
 *  @return     double*          calculated lec expression on the whole dbv sample    
 *                                  NULL on error (error can be retrieved via a call to L_error()
 */
double* L_cc_link_exec(char* lec, KDBVariablesPtr dbv, KDBScalarsPtr dbs)
{
    double* vec = NULL;
    if(lec == NULL || lec[0] == 0) 
        return vec;
    
    std::shared_ptr<CLEC> clec = L_cc(lec);
    if(clec != 0 && !L_link(dbv, dbs, clec)) 
    {
        int nb = dbv->get_sample()->nb_periods;
        vec = (double*) SW_nalloc(nb * sizeof(double));
        for(int t = 0 ; t < nb ; t++)
            vec[t] = L_exec(dbv, dbs, clec, t);
    }

    return vec;
}

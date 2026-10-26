#include <setjmp.h>
#include <signal.h>
#include <time.h>

#include "api/pch.h"
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
 *  Execution of a CLEC sub expression.
 * 
 *  The process iterates on the compiled LEC expression. 
 *      - the first byte (type) is an identifier (+, ln, VAR, ...)
 *      - according to the identifier, one or more values are read from the stack, 
 *          the function/operator is called and the result is placed on the stack
 *  
 *  @param [in] expr_pos int            position of the sub expression in the CLEC->expression buffer
 *  @param [in] length   int            length of the sub expression
 *  @param [in] t        int            time (index in dbv) of execution
 *  @return              double         result of the computation
 *  
 */
double CLEC::execute_sub_expression(const int start, const int length, const int t) const
{
    if(start < 0 || start >= this->v_expression.size())
    {
        std::string error_msg = "Invalid start position for sub expression: " + std::to_string(start);
        kwarning(error_msg.c_str());
        return (double) IODE_NAN;
    }

    int end = start + length - 1;
    if(end >= this->v_expression.size())
    {
        std::string error_msg = "Invalid end position for sub expression: " + std::to_string(end);
        kwarning(error_msg.c_str());
        return (double) IODE_NAN;
    }
    
    int type;
    double value;
    LECStack stack;
    for(int expr_pos = start; expr_pos <= end;)
    {
        const ATOMIC_LEC& al = this->v_expression[expr_pos];
        expr_pos++;

        type = al->type;

        switch(al->generic_type)
        {
            // ---- atomic LEC of type value ----
            case L_VALUE:
            {
                if(al->type == L_VAR || al->type == L_VART)
                {
                    const LEC_VAR& al_var = static_cast<LEC_VAR&>(*al);
                    if(!smpl_ptr)
                    {
                        kwarning("LEC: No attached sample");
                        return IODE_NAN;
                    }
                    value = al_var.get_value(t);
                }
                else
                {
                    const LEC_ABSTRACT_VALUE& al_value = static_cast<LEC_ABSTRACT_VALUE&>(*al);
                    value = al_value.get_value(t);
                }
                stack.push_back(value);
                break;
            }
            // ---- atomic LEC of type executable ----
            case L_EXECUTABLE:
            {
                const LEC_ABSTRACT_EXECUTABLE& al_exec = static_cast<LEC_ABSTRACT_EXECUTABLE&>(*al);
                value = al_exec.execute(stack, t, expr_pos, *this);
                stack.push_back(value);
                break;
            }
            // ---- atomic LEC of type other ----
            case L_OTHER:
            default:
                break;
        }
    }

    return stack.back();
}

/**
 *  Execution of a compiled and linked CLEC expression.
 *  
 *  - Assign global variables that could be used by calculation functions.
 *  - initiate the exception handling on floating point errors
 *  - call execute_sub_expression() which is the real (recursive) calculator
 *  
 *  @param [in] t    int              time of calculation (index in dbv sample)
 *  @return          double           result of the calculation, IODE_NAN on error (L_errno is set)
 */
double CLEC::execute(const int t)
{
    // leave if empty CLEC expression
    if(this->v_expression.size() == 0)
        return IODE_NAN;

    exec_t = t;
    
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
    
    double value = this->execute_sub_expression(0, (int) v_expression.size(), t);
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
    
    std::shared_ptr<CLEC> clec = nullptr; 
    try
    {
        clec = std::make_shared<CLEC>(lec);
    }
    catch(const std::exception& e) 
    {
        std::string error_msg = "Syntax error in LEC expression '" + std::string(lec) + "': ";
        error_msg += e.what();
        kwarning(error_msg.c_str());
        return vec;
    }

    int rc = clec->link(dbv, dbs);
    if(rc > 0)
        return vec;

    int nb = dbv->get_sample()->nb_periods;
    vec = (double*) SW_nalloc(nb * sizeof(double));
    for(int t = 0 ; t < nb ; t++)
        vec[t] = clec->execute(t);

    return vec;
}

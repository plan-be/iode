#pragma once

#include "api/pch.h"
#include "api/lec/l_common.h"
#include "api/lec/l_value.h"
#include "api/lec/l_exec_fns.h"
#include "api/lec/l_exec_ops.h"
#include "api/lec/l_exec_tfn.h"
#include "api/lec/l_exec_val.h"
#include "api/lec/l_exec_mtfn.h"
#include "api/lec/l_err.h"
#include "api/lec/l_token.h"


/*----------------- GLOBALS ----------------------*/

using ATOMIC_LEC = std::variant<LEC_CONST_REAL, LEC_CONST_LONG, LEC_COEF, LEC_VAR, LEC_PERIOD, 
                                LEC_OTHER, LEC_OP, LEC_FN, LEC_TFN, LEC_VAL_FN, LEC_MTFN>;

inline std::vector<ATOMIC_LEC> L_EXPR;      // Table of pairs <type, atomic elements>  

/*---------------- STRUCTS ------------------------*/

struct CLEC: public AbstractCLEC
{
    // duplicate endogenous variable in the LEC expression
    bool duplicated_endo = false;

    // original LEC expression as a string (for debugging purposes)
    std::string lec;

    // 'executable' LEC expression as a vector of atomic expressions 
    // (for execution purposes)
    std::vector<ATOMIC_LEC> v_expression;

    // list of pairs <scalar and variable name, positions in database>
    // NOTE: we don't use a std::map as we need to keep the order of the 
    //       names as they appear in the expression (for the link step) 
    std::vector<std::pair<std::string, int>> objs;

private:
    /**
     * Tries to solve this LEC equation by the Newton-Raphson method (internal helper).
     * 
     * Subfunction used by newton() method. The convergence threshold eps is set to 1e-6 by default.
     * However, if algo is set to 1 and the absolute value of the endogenous variable before the 
     * first iteration is > 1.0, then eps is multiplied by the endogenous value.
     *
     * @param  [in]    int  algo              if not null, the convergence criteria eps is multiplied by the 
     *                                         value of f(x) if ||f(x)|| > 1.0.
     * @param [in]    KDBVariablesPtr dbv     KDB of VAR with which the equation has been linked
     * @param [in]    KDBScalarsPtr   dbs     KDB of Scalar with which the equation has been linked
     * @param [in]    int             t       time of calculation (index in dbv Sample)
     * @param [in]    int             varnb   position of the endogenous variable in dbv
     * @param [in]    int             eqvarnb position of the initial endogenous variable (i.e. equation name) in dbv
     * @return        double                  approximated root or NaN if no solution found
     */
    double newton_sub(const int algo, KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, 
        const int varnb, const int eqvarnb);

    /**
     * First step of linking CLEC to KDBs: each variable and scalar name is searched in the KDB's
     * and their positions are saved in this CLEC->objs.
     *
     * If a name is not found, an error message is added to the stack via IodeErrorManager::append_error 
     * and L_errno is set to L_NOT_FOUND_ERR.
     *
     * @param [in]      dbv     KDB*    KDB of variables
     * @param [in]      dbs     KDB*    KDB of scalars
     * @return                  int     0 on success,  L_NOT_FOUND_ERR on error
    */
    int link_names(KDBVariablesPtr dbv, KDBScalarsPtr dbs);

    /**
     * Second step of linking CLEC. Each time displacement in this CLEC struct is aligned to the dbv's Sample.
     * For example, the position of A[1970Y1] in the vector A depends on the sample of dbv.
     *
     * @param [in]      dbv     KDB*    KDB of variables
     */
    void link_sample(KDBVariablesPtr dbv, const int start, const int length);

    /**
     *  First step of LEC compilation. L_YY (see l_token.c) is the open stream containing the analyzed LEC expression.
     *  
     *  At the end of this function, 2 tables are created: L_EXPR and L_NAMES:
     *      - L_EXPR contains atomic expressions in the execution order including references to L_NAMES 
     *      - L_NAMES contains the names included in the lec expression
     *  
     *  @return int error code: 0 on success or L_PAR_ERR, L_SYNTAX_ERR...
     */
    int initialize(const bool side_of_eq);

    // WARNING: to be run AFTER initialize() in order to fill L_EXPR and L_NAMES first
    void reorder_expression(std::vector<ATOMIC_LEC>& expr);
    
    void initialize_names()
    {
        // initialize all names with position -1 (not found)
        for(const std::string& name : L_NAMES)
            this->objs.push_back({name, -1});
    } 

public:
    CLEC(const std::string& lec, const bool side_of_eq = false);

    CLEC(const std::string& eq, const std::string& endo);

    CLEC(const CLEC& other) 
    {
        this->lec = other.lec;
        this->duplicated_endo = other.duplicated_endo;
        this->v_expression = other.v_expression;
        this->objs = other.objs;
    }

    ~CLEC() 
    { 
        v_expression.clear();
    }

    // assignment operator (deep copy)
    CLEC& operator=(const CLEC& other) 
    {
        if(this == &other)
            return *this;
    
        this->lec = other.lec;
        this->duplicated_endo = other.duplicated_endo;
        this->v_expression = other.v_expression;
        this->objs = other.objs;

        return *this;
    }

    // NOTE: we don't compare the lec attributes as they can be different but lead to the  
    // same executable expression (e.g. "D*2+ACAF" and "D * 2 + ACAF" are the same) 
    bool operator==(const CLEC& other) const
    {
        if(this->duplicated_endo != other.duplicated_endo)
            return false;

        if(this->v_expression.size() != other.v_expression.size())
            return false;

        for(size_t i = 0; i < this->v_expression.size(); ++i)
        {
            if(this->v_expression[i] != other.v_expression[i])
                return false;
        }

        if(this->objs.size() != other.objs.size())
            return false;

        return this->objs == other.objs;
    }

    // Links this CLEC expression to KDB's of variables and of scalars.
    // If some Period's are present in CLEC, they are aligned to the Sample of dbv.
    // The CLEC, although modified by link(), can be relinked later with other KDB's.
    // @param [in]      dbv  KDB*        KDB of variables to link to CLEC
    // @param [in]      dbs  KDB*        KDB of scalars to link to CLEC
    // @return               int         0 on success, L_errno on error
    int link(KDBVariablesPtr dbv, KDBScalarsPtr dbs);

    std::vector<std::string> get_scalars() const
    {
        std::vector<std::string> list;
        for(const auto& [name, _] : this->objs)
        {
            if(is_coefficient(name))
                list.push_back(name);
        }
        return list;
    }

    std::vector<std::string> get_variables() const
    {
        std::vector<std::string> list;
        for(const auto& [name, _] : this->objs)
        {
            if(!is_coefficient(name))
                list.push_back(name);
        }
        return list;
    }

    /**
     * Solves numerically this LEC equation for one period of time with respect to a given variable (varnb).
     * If the Newton-Raphson method does not reach a solution, tries a bisection (secant) method.
     *
     * @param [in] KDBVariablesPtr dbv     KDB of VAR with which the equation has been linked
     * @param [in] KDBScalarsPtr   dbs     KDB of Scalar with which the equation has been linked
     * @param [in] int             t       time of calculation (index in dbv Sample)
     * @param [in] int             varnb   position of the endogenous variable in dbv
     * @param [in] int             eqvarnb position of the initial endogenous variable (i.e. equation name) in dbv
     * @return     double                  root of the equation (varnb value that solves the equation)
     */
    double zero(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, const int varnb, const int eqvarnb);

    /**
     * Tries to solve this LEC equation by the Newton-Raphson method.
     *
     * Calls first the Newton-Raphson method with convergence criterion set to 1e-6.
     * If no solution is found, calls again with a multiplied epsilon by the endogenous value.
     *
     * @param [in] KDBVariablesPtr dbv     KDB of VAR with which the equation has been linked
     * @param [in] KDBScalarsPtr   dbs     KDB of Scalar with which the equation has been linked
     * @param [in] int             t       time of calculation (index in dbv Sample)
     * @param [in] int             varnb   position of the endogenous variable in dbv
     * @param [in] int             eqvarnb position of the initial endogenous variable (i.e. equation name) in dbv
     * @return     double                  approximated root of the equation
     */
    double newton(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, const int varnb, const int eqvarnb);

    /**
     * Tries to find a solution to this LEC equation by a secant (bisection) method.
     *
     * The basic secant method first requires to determine an interval [xl, xr] containing a root of
     * the equation (xl/xr stands for x-left/right). Then the size of that interval is decreased
     * until convergence.
     *
     * @param [in] KDBVariablesPtr dbv     KDB of VAR with which the equation has been linked
     * @param [in] KDBScalarsPtr   dbs     KDB of Scalar with which the equation has been linked
     * @param [in] int             t       time of calculation (index in dbv Sample)
     * @param [in] int             varnb   position of the endogenous variable in dbv
     * @param [in] int             eqvarnb position of the initial endogenous variable (i.e. equation name) in dbv
     * @return     double                  root of the equation (varnb value that solves the equation)
     */
    double secant(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, const int varnb, const int eqvarnb);

    /**
     * Prints this LEC expression. Sets the endogenous variable (name) in bold.
     *
     * @param [in] name   std::string&  endogenous name
     * @param [in] eqlec  std::string&  LEC expression
     * @param [in] coefs  int           if 1: replace scalars by their value
     *                                  if 2: replace scalars by their value and their t-test
     * @return            bool          true on success, false on failure
     */
    bool print_definition(const std::string& name, const std::string& eqlec, const int coefs);

    /**
     * Executes this compiled and linked CLEC expression.
     *
     * Assigns global variables that could be used by calculation functions and initiates 
     * the exception handling on floating point errors, then calls the expression evaluator.
     *
     * @param [in] dbv    KDBVariablesPtr     input variable KDB
     * @param [in] dbs    KDBScalarsPtr       input scalars KDB
     * @param [in] t      int                 time of calculation (index in dbv sample)
     * @return            double              result of the calculation, IODE_NAN on error
     */
    double execute(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t);

    /**
     * @brief Execution of a CLEC sub expression.
     * 
     * @param pos       first position of the sub expression in the CLEC expression
     * @param length    length of the sub expression
     * @param t         time of calculation (index in dbv sample)
     * @return double 
     */
    double execute_sub_expression(const int start, const int length, const int t);
};

/* ---------------------- FUNCS ---------------------- */

/* l_compile.cpp */
int L_sub_expr(const std::vector<ATOMIC_LEC>& v_alec, int close = -1);

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
void L_fperror(int sig);
double* L_cc_link_exec(char* lec, KDBVariablesPtr dbv, KDBScalarsPtr dbs);

/* l_hodrick.c */
int HP_calc(double *f_vec, double *t_vec, int nb, double lambda, int std);
void HP_test(double *f_vec, double *t_vec, int nb, int *beg, int *dim);

/* l_eqs.c */
int L_split_eq(const std::string& eq);
int L_invert(const std::string& eq, const std::string& endo, bool& duplicated_endo);

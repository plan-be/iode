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

enum EQ_HAND_SIDE
{
    EQ_LHS = 0,
    EQ_RHS = 1
};

using ATOMIC_LEC = std::variant<LEC_CONST_REAL, LEC_CONST_LONG, LEC_COEF, LEC_VAR, LEC_PERIOD, 
                                LEC_OTHER, LEC_OP, LEC_FN, LEC_TFN, LEC_VAL_FN, LEC_MTFN>;

/*---------------- STRUCTS ------------------------*/


struct CLEC: public AbstractCLEC
{
    // duplicate endogenous variable in the LEC expression
    bool duplicated_endo = false;

    // original LEC expression as a string (for debugging purposes)
    std::string lec;

    // parser for the LEC expression
    LecParser parser;

    // 'executable' LEC expression as a vector of atomic expressions 
    // (for execution purposes)
    std::vector<ATOMIC_LEC> v_expression;

    // map of scalar and variable names to their positions in database
    std::map<std::string, int> map_objs;

    // vector of scalar and variable names in order of appearance in the LEC expression
    // NOTE: we need this to fulfill the *RULE 4* when editing the cell of 
    //       an IODE computed table -> (rule = only the first variable found 
    //       in the LEC expression is updated)
    std::vector<std::string> v_objs;
    
private:
    /**
     * Adds a series or scalar name in map_objs.
     * 
     * @param [in]     name    std::string   VAR or Scalar name
     */
    void add_coef_or_var_name(const std::string& name);

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
     * Tries to solve this LEC equation by the Newton-Raphson method (internal helper).
     * 
     * Subfunction used by newton() method. The convergence threshold eps is set to 1e-6 by default.
     * However, if algo is set to 1 and the absolute value of the endogenous variable before the 
     * first iteration is > 1.0, then eps is multiplied by the endogenous value.
     *
     * @param  [in]    int  algo              if not null, the convergence criteria eps is multiplied by the 
     *                                         value of f(x) if ||f(x)|| > 1.0.
     * @param [in]    KDBVariablesPtr dbv         KDB of VAR with which the equation has been linked
     * @param [in]    KDBScalarsPtr   dbs         KDB of Scalar with which the equation has been linked
     * @param [in]    int             t           time of calculation (index in dbv Sample)
     * @param [in]    std::string&    var_name    name of the endogenous variable in dbv
     * @param [in]    std::string&    eq_var_name name of the initial endogenous variable (i.e. equation name) in dbv
     * @return        double                      approximated root or NaN if no solution found
     */
    double newton_sub(const int algo, KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, 
        const std::string& var_name, const std::string& eq_var_name);

    /**
     * Computes the value of f(x) in time t where f is this LEC expression.
     * 
     * @param [in]  x         double            value for which the expression must be calculated
     * @param [in]  t         int               index in the variable for which the expression must be calculated
     * @param [in]  dbv       KDBVariablesPtr   KDB of variables
     * @param [in]  dbs       KDBScalarsPtr     KDB of scalars
     * @param [in]  var_name  string            name of the variable in the KDB of variables
     * @param [in]  shift     double            shift value (endo[t] or 0)
     * @return                double            value of f(x, t)
     */
    double fx(double x, int t, KDBVariablesPtr dbv, KDBScalarsPtr dbs, const std::string& var_name, double shift);

    /**
     * Tries to find an interval [x1, x2] where there is a root of fx().
     * 
     * @param [in,out]  x1        double*           left bound of the segment
     * @param [out]     x2        double*           right bound of the segment
     * @param [in]      t         int               current period of execution
     * @param [in]      dbv       KDBVariablesPtr   KDB of variables
     * @param [in]      dbs       KDBScalarsPtr     KDB of scalars
     * @param [in]      var_name  string            name of the variable in the KDB
     * @param [in]      shift     double            shift value (endo[t] or 0)
     * @return                    int               0 if a solution is found, -1 otherwise
     */
    int bracket(double* x1, double* x2, int t, KDBVariablesPtr dbv, KDBScalarsPtr dbs, const std::string& var_name, double shift);

    /**
    * Saves the current variable or constant token to v_expression.
     * 
     * @return int  0 on success, -1 on error
     */
    int save_var(TOKEN& token);

    /**
     * Check if the operator op has a lower execution priority than the last operator on the stack L_OPS.
     * 
     * @param [in]  op  int     operator to compare with last operator in L_OPS
     * @return          bool    true if last entry in L_OPS is not an operator or priority(op) <= priority(last op)
     *                          false if priority(op) > priority(last op) or no op on stack
     */
    bool priority_sup(int op);

    /**
    * Adds the last "operator" on top of L_OPS to v_expression, as well as the number of parameters.
     * Checks if the number of arguments are in line with the definitions.
     * 
     * @return  int     0 on success, L_ARGS_ERR if the number of args does not follow the syntax
     */
    int save_op();

    /**
     * Adds the current operator (stored in token.type) to L_OPS, the stack of operators.
     * 
     * @param [in]      op_group  int   group the operator to be added belongs to (L_OP, L_FN, L_TFN, L_MTFN, L_OPENP, COMMA...)
     * @param [in]      func_type int   type of the function (ex. L_SIN, L_COS...).
     * @param [in,out]  L_PAR     int&  current parenthesis depth (used for checking balanced parentheses)
     * @return                    int   0 on success, L_errno on error
     */
    int add_stack(int op_group, int func_type, int& L_PAR);

    /**
    * Empties the stack of operators L_OPS by adding all operators and the number of their arguments to v_expression.
     * 
     * @return  int     0 on success, L_errno on error
     */
    int empty_ops_stack();

    /**
     * Applies a lag on each variable in the last sub expression.
     * 
     * @param [in]  lag int     lag to add to each var
     * @return          int     0 on success, L_errno if the sub expression cannot be identified
     */
    int lag_expr(int lag);

    /**
     * Applies a time expression (e.g. "1960Y1") on each variable in the last sub expression.
     * 
     * @return  int     0 on success, L_errno if the sub expression cannot be identified
     */
    int time_expr(TOKEN& token);

    /**
     * Analyses a lag expression between [], like in A[2021Y1] or VAR[-2].
     * 
     * @return  int     0 on success, L_LAG_ERR on error
     */
    int analyze_lag();

    /**     *  First step of LEC compilation. L_YY (see l_token.c) is the open stream containing the analyzed LEC expression.
     *  
     *  At the end of this function, 2 tables are created: v_expression and map_objs:
     *      - v_expression contains atomic expressions in the execution order including references to map_objs 
     *      - map_objs contains the names included in the lec expression
     *  
     *  @return int error code: 0 on success or L_PAR_ERR, L_SYNTAX_ERR...
     */
    int parse(const bool side_of_eq);

    // WARNING: to be run AFTER parse() in order to fill v_expression and map_objs first
    void reorder_expression();

public:
    CLEC(const std::string& lec, const bool side_of_eq = false);

    CLEC(const std::string& eq, const std::string& endo);

    CLEC(const CLEC& other) 
    {
        this->lec = other.lec;
        this->duplicated_endo = other.duplicated_endo;
        this->v_expression = other.v_expression;
        this->v_objs = other.v_objs;
        this->map_objs = other.map_objs;
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
        this->v_objs = other.v_objs;
        this->map_objs = other.map_objs;

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

        if(this->v_objs.size() != other.v_objs.size())
            return false;

        if(this->v_objs != other.v_objs)
            return false;

        if(this->map_objs.size() != other.map_objs.size())
            return false;

        return this->map_objs == other.map_objs;
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
        for(const std::string& name : this->v_objs)
        {
            if(is_coefficient(name))
                list.push_back(name);
        }
        return list;
    }

    std::vector<std::string> get_variables() const
    {
        std::vector<std::string> list;
        for(const std::string& name : this->v_objs)
        {
            if(!is_coefficient(name))
                list.push_back(name);
        }
        return list;
    }

    /**
     * Solves numerically this LEC equation for one period of time with respect to a given variable.
     * If the Newton-Raphson method does not reach a solution, tries a bisection (secant) method.
     *
     * @param [in] KDBVariablesPtr dbv         KDB of VAR with which the equation has been linked
     * @param [in] KDBScalarsPtr   dbs         KDB of Scalar with which the equation has been linked
     * @param [in] int             t           time of calculation (index in dbv Sample)
     * @param [in] std::string&    var_name    name of the endogenous variable in dbv
     * @param [in] std::string&    eq_var_name name of the initial endogenous variable (i.e. equation name) in dbv
     * @return     double                      root of the equation (var_name value that solves the equation)
     */
    double zero(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, const std::string& var_name, const std::string& eq_var_name);

    /**
     * Tries to solve this LEC equation by the Newton-Raphson method.
     *
     * Calls first the Newton-Raphson method with convergence criterion set to 1e-6.
     * If no solution is found, calls again with a multiplied epsilon by the endogenous value.
     *
     * @param [in] KDBVariablesPtr dbv         KDB of VAR with which the equation has been linked
     * @param [in] KDBScalarsPtr   dbs         KDB of Scalar with which the equation has been linked
     * @param [in] int             t           time of calculation (index in dbv Sample)
     * @param [in] std::string&    var_name    name of the endogenous variable in dbv
     * @param [in] std::string&    eq_var_name name of the initial endogenous variable (i.e. equation name) in dbv
     * @return     double                      approximated root of the equation
     */
    double newton(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, const std::string& var_name, const std::string& eq_var_name);

    /**
     * Tries to find a solution to this LEC equation by a secant (bisection) method.
     *
     * The basic secant method first requires to determine an interval [xl, xr] containing a root of
     * the equation (xl/xr stands for x-left/right). Then the size of that interval is decreased
     * until convergence.
     *
     * @param [in] KDBVariablesPtr dbv         KDB of VAR with which the equation has been linked
     * @param [in] KDBScalarsPtr   dbs         KDB of Scalar with which the equation has been linked
     * @param [in] int             t           time of calculation (index in dbv Sample)
     * @param [in] std::string&    var_name    name of the endogenous variable in dbv
     * @param [in] std::string&    eq_var_name name of the initial endogenous variable (i.e. equation name) in dbv
     * @return     double                      root of the equation (var_name value that solves the equation)
     */
    double secant(KDBVariablesPtr dbv, KDBScalarsPtr dbs, const int t, const std::string& var_name, const std::string& eq_var_name);

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


struct SLEC 
{
    // LHS equation's member as a compiled LEC object
    std::shared_ptr<CLEC> left_clec;
    
    // RHS equation's member as a compiled LEC object
    std::shared_ptr<CLEC> right_clec;

    EQ_HAND_SIDE    side_with_endo;         // side containing endo (will be inverted if possible)
    EQ_HAND_SIDE    side_without_endo;      // side not containing endo (will be left unchanged)
    int             func_index;             // last operator or function
    int             nb_args;                // number of args of the last op|fn
    std::string     endo;                   // name of the endogenous variable

private:
    /**
     * Splits an equation side expression if it contains the endogenous variable in a non-trivial way.
     * 
     * @return                  bool    false if nothing to split, true if split or error occurred
     */
    bool split_expr();

    /**
     * Counts the number of occurrences of the endogenous variable of an equation 
     * in a vector of atomic expressions. Its purpose is to check if the endogenous 
     * variable is duplicated in the equation.
     * 
     * @param [in]  mbr     EQ_HAND_SIDE    position of the member to analyze (LHS or RHS)
     * @return              int             number of occurrences of the the endogenous variable
     */
    int count_endo(const EQ_HAND_SIDE mbr);

    /**
    * Appends the member (LHS or RHS) contained in slec to expr.
     * 
     * @param [in]  mbr     EQ_HAND_SIDE    position of the member to append
     */
    void append_member(const EQ_HAND_SIDE mbr, std::vector<ATOMIC_LEC>& expr);

    /**
    * Prepends the member (LHS or RHS) contained in slec to expr.
     * 
     * @param [in]  mbr     EQ_HAND_SIDE    member to prepend
     */
    void prepend_member(const EQ_HAND_SIDE mbr, std::vector<ATOMIC_LEC>& expr);

    /**
    * Appends an LEC_OTHER element to expr.
     * 
     * @param [in]  type    int     type of LEC_OTHER element
     */
    void append_other(int type, std::vector<ATOMIC_LEC>& expr);

    /**
    * Appends an operator to expr.
     * 
     * @param [in]  op  int     operator
     */
    void append_op(int op, std::vector<ATOMIC_LEC>& expr);

    /**
    * Appends a function and its number of arguments to expr.
     * 
     * @param [in]  op      int     function id
     * @param [in]  nargs   int     number of arguments of the function
     */
    void append_fn(int op, int nargs, std::vector<ATOMIC_LEC>& expr);

    /**
    * Appends a long constant to expr.
     * 
     * @param [in]  a   int     value of the constant
     */
    void append_const(int a, std::vector<ATOMIC_LEC>& expr);

public:
    /**   
     * Splits an equation in LHS (left hand side) and RHS (right hand side) and 
     * compiles each element separately.
     * 
     * @param [in]      lec     std::string     LEC expression of the equation
     * @param [in]      endo    std::string     Name of the endogenous variable of the equation
     */
    SLEC(const std::string& lec, const std::string& endo);

    /**
     * Tries to analytically solve an equation with respect to a specified endogenous variable.
     * The result is a LEC expression (not equation) that can be used in simulations.
     * 
    * @param [out]  duplicated_endo    bool&           false if inverted, true if endo appears multiple times
    * @param [out]  clec               CLEC&           target CLEC receiving the generated expression in v_expression
     */
    int invert_equation(bool& duplicated_endo, CLEC& clec);

    /**
     * @brief Moves the objects' names (variables and scalars) to the clec attribute of 
     *        an equation.
     * 
     * @param clec 
     */
    void merge_names(CLEC& clec);
};


/* ---------------------- FUNCS ---------------------- */

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

/* l_compile.cpp */

/**
 * Computes the position of the beginning of a sub-expression.
 * Browses backwards all elements of the expression until having reached a level 0 of parentheses or 
 * all arguments of an operator or function.
 * 
 * @param [in]  v_alec  std::vector<ATOMIC_LEC>&   vector of atomic LEC elements
 * @param [in]  close   int                         position where the expression is "closed" (-1 for end)
 * @return              int                         position where the expression starts, -1 on error
 */
int find_sub_expr_start(const std::vector<ATOMIC_LEC>& v_alec, int close = -1);

/* l_eqs.cpp */

/**
 * Returns the position of ":=" in an equation or -1 if not found.
 * 
 * @param [in]  eq  std::string     equation to analyze
 * @return          int             position of := in the equation or -1 if not found
 */
int split_eq(const std::string& eq);

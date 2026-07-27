/**
 * @header4iode
 *
 * Functions to compile LEC *equations*.
 * 
 * Context
 * -------
 *      LEC *equations* are made up of 2 LEC *expressions* separated by ":=":  
 *           LHS := RHS
 *      
 *      Equations are used in 2 different contexts: 
 *           - estimation of coefficients 
 *           - model simulation
 *      
 *      The way LHS need to be compiled differs according to the context.
 *
 *      To estimate (the coefficients of) the equation "LHS := RHS", where RHS contains the coefficients, one needs (roughly) to
 *      calculate LHS and then to find the values of the coefficients to minimize |LHS - RHS|. LHS may be a simple endogenous variable 
 *      or a complex expression: "PIB" and "dln PIB" are both valid LHS in the case of an estimation.
 *      
 *      A model simulation using a Gauss-Seidel-like method, in contast, requires the evaluation of the endogenous for each equation
 *      in an iterative process. In the example above, the equation "dln PIB := RHS" should ideally be transformed in "PIB := exp(ln PIB[-1] + RHS)".
 *      When this is impossible, another numerical (sub-iterative) method is to be used to find the value of the endogenous variable.
 * 
 * 
 */

#include "api/pch.h"
#include "api/lec/lec.h"


enum EQ_HAND_SIDE
{
    EQ_LHS = 0,
    EQ_RHS = 1
};


struct SLEC 
{
    // Tables of ALEC's for LHS equations members
    std::vector<ATOMIC_LEC> sl_left_expr;
    
    // Tables of ALEC's for RHS equations members
    std::vector<ATOMIC_LEC> sl_right_expr;

    EQ_HAND_SIDE    sl_side_with_endo;      // side containing sl_endo (will be inverted if possible)
    EQ_HAND_SIDE    sl_side_without_endo;   // side not containing sl_endo (will be left unchanged)
    int             sl_op;                  // last operator|function
    int             sl_nargs;               // number of args of the last op|fn
    std::string     sl_endo;                // name of the endogenous variable
};


/**
 * Counts the number of occurrences of a variable in a ALEC table of atomic expressions (result of l_cc1()).
 * The lagged (A[-1]) and timed (A[2000Y1]) variables are not taken into account.
 * 
 * @param [in]  v_al    std::vector<ATOMIC_LEC>&   vector of ALEC elements
 * @param [in]  last    int     last position in ALEC to take into account in the count
 * @param [in]  endo    char*   name of the variable to search
 * @return              int     number of occurence of endo in ALEC
*/
static int L_count_endo(const std::vector<ATOMIC_LEC>& v_al, const std::string& endo)
{
    if(L_NAMES.empty())
        return 0;

    int count = 0;
    for(const ATOMIC_LEC& al : v_al)
    {
        if(!std::holds_alternative<LEC_VAR>(al))
            continue;

        const LEC_VAR& al_var = std::get<LEC_VAR>(al);

        // the lagged (A[-1]) and timed (A[2000Y1]) variables are not taken into account.
        if(al_var.type != L_VAR || al_var.per.step != 0 || al_var.lag != 0)
            continue;

        if(L_NAMES[al_var.pos] == endo)
            count++;
    }

    return count;
}

/**
 * If equation side with endo is not trivial, splits it into two parts 
 * and replaces left side and right side of the equation with the two parts.
 * 
 * ------------------------------------------------------
 * Example: 
 *
 * -> equation side containing the endogenous : "EXO + ln ENDO"
 * -> at the end of the process, the SLEC struct will contain:
 * left side:  "EXO" 
 * right side: "ln ENDO"
 * 
 * @param [in, out] sl  SLEC&   current state of the equation
 * @return              bool    false if there was nothing to split, true if the  
 *                              expression was split or if an error occurred
*/
static bool L_split_expr(SLEC& slec)
{
    int start_sub_expr = -1;

    // vector of atomic lec containing the endogenous variable
    std::vector<ATOMIC_LEC> v_al;
    if(slec.sl_side_with_endo == EQ_LHS)
        v_al = slec.sl_left_expr;
    else
        v_al = slec.sl_right_expr;
    
ag:
    if(v_al.size() == 0)
    {
        L_errno = L_DUP_ERR;
        return true; 
    }

    const ATOMIC_LEC& al = v_al.back();

    int type = std::visit([](auto&& arg) -> int { return arg.type; }, al);

    if(type == L_CLOSEP)
    {
        // if the last element is a close parenthesis, the first must be an open parenthesis
        // -> drop the first element of v_al (open parenthesis)
        v_al.erase(v_al.begin());
        // -> drop the last element of v_al (close parenthesis)
        v_al.pop_back();
        // try the next element 
        goto ag;
    }

    // if the last element is L_VAR, we have only one item and the process is terminated (?)
    if(type == L_VAR)
        return false;

    if(!is_executable(type))
    {
        L_errno = L_DUP_ERR;
        return true; 
    }

    // cast to LEC_EXECUTABLE to get the number of arguments
    const LEC_EXECUTABLE& al_exec = reinterpret_cast<const LEC_EXECUTABLE&>(al);

    slec.sl_op = type;
    slec.sl_nargs = al_exec.nb_args;

    // remove the last element of v_al (the operator/function) 
    // -> previously stored in slec.sl_op and slec.sl_nargs
    v_al.pop_back();

    // Search the beginning of the sub expression
    start_sub_expr = L_sub_expr(v_al, (int) v_al.size() - 1);
    if(start_sub_expr < 0)
    {
        L_errno = L_DUP_ERR;
        return true; 
    }

    slec.sl_left_expr.clear();
    slec.sl_right_expr.clear();

    auto it_sub_expr = v_al.begin() + start_sub_expr;

    // We move the elements before the sub expression to slec.sl_left_expr
    if(it_sub_expr != v_al.begin())
        slec.sl_left_expr.insert(slec.sl_left_expr.end(), v_al.begin(), it_sub_expr);

    // and the elements after the sub expression to slec.sl_right_expr
    if(it_sub_expr != v_al.end())
        slec.sl_right_expr.insert(slec.sl_right_expr.end(), it_sub_expr, v_al.end());

    v_al.clear(); // we clear v_al to avoid confusion (we won't use it anymore)

    // find the member where endo is present
    if(L_count_endo(slec.sl_left_expr, slec.sl_endo) > 0) 
    {       
        slec.sl_side_with_endo = EQ_LHS;                                   
        slec.sl_side_without_endo = EQ_RHS;                               
    }
    else 
    {
        slec.sl_side_with_endo = EQ_RHS;
        slec.sl_side_without_endo = EQ_LHS;
    }

    return true;
}


/**
 * Splits an equation in LHS and RHS and compiles (step 1 only) each element separately. 
 * The error L_ASSIGN_ERR is returned if ":=" is not found in the equation.
 * 
 * The result is stored in a SLEC struct (see above).
 * Only the step 1 of the compilation is performed at this stage.
 * 
 * @param [in, out]     sl    SLEC&   struct where the A
 * @param [in]          lec   char*   LEC expression of the equation
 * @return                    int     error code or 0 on success
 *                                      
*/
static int initialize_eq(SLEC& slec, const std::string& lec)
{
    L_NAMES.clear();

    // split the equation into LHS and RHS according to :=
    std::size_t pos = lec.find(":=");
    if(pos == std::string::npos) 
    {
        L_errno = L_ASSIGN_ERR;
        return L_errno;
    }
    // make a copy of the equation to modify it
    std::string lhs_lec = lec.substr(0, pos);
    std::string rhs_lec = lec.substr(pos + 2);

    // Compiles left member
    CLEC lhs(lhs_lec, true);
    slec.sl_left_expr = lhs.v_expression;
    slec.sl_left_expr.pop_back();                // drop the last element (L_EOE)        
    if(slec.sl_left_expr.empty()) 
        return L_errno;
    
    // Compiles the right member
    CLEC rhs(rhs_lec, true);
    slec.sl_right_expr = rhs.v_expression;
    slec.sl_right_expr.pop_back();               // drop the last element (L_EOE)
    if(slec.sl_right_expr.empty()) 
        return L_errno;

    return 0;
}


/**
 * Appends the member mbr (LHS or RHS) contained in sl to L_EXPR. 
 * 
 * @param [in] sl   SLEC&   
 * @param [in] mbr  int     position of the member to append 
*/
static void L_append(const SLEC& slec, const EQ_HAND_SIDE mbr)
{
    const std::vector<ATOMIC_LEC>& v_al = (mbr == EQ_LHS) ? slec.sl_left_expr : slec.sl_right_expr;
    L_EXPR.insert(L_EXPR.end(), v_al.begin(), v_al.end());
}


/**
 * Adds L_EXPR the member mbr (LHS or RHS) contained in sl at the beginning of L_EXPR.
 * 
 * @param [in] sl   SLEC&   container of the compiled equation
 * @param [in] mbr  int     member of sl to copy to L_EXPR
*/
static void L_front(const SLEC& slec, const EQ_HAND_SIDE mbr)
{
    const std::vector<ATOMIC_LEC>& v_al = (mbr == EQ_LHS) ? slec.sl_left_expr : slec.sl_right_expr;
    L_EXPR.insert(L_EXPR.begin(), v_al.begin(), v_al.end());
}


static void L_append_other(int type)
{
    LEC_OTHER al_other(type);
    L_EXPR.push_back(al_other);
}


/**
 * Appends an operator to L_EXPR.
 * 
 * @param [in]  op  int     operator
*/
static void L_append_op(int op)
{
    LEC_OP al_op(op);
    L_EXPR.push_back(al_op);
}


/**
 * Appends a function and its number of arguments to L_EXPR.
 *
 * @param [in] op     int   function id
 * @param [in] nargs  int   number of arguments of the function
*/
static void L_append_fn(int op, int nargs)
{
    if(is_fn(op))
    {
        LEC_FN al_fn(op, nargs);
        L_EXPR.push_back(al_fn);
        return;
    }

    if(is_tfn(op))
    {
        LEC_TFN al_tfn(op, nargs);
        L_EXPR.push_back(al_tfn);
        return;
    }

    if(is_val(op))
    {
        LEC_VAL_FN al_val_fn(op);
        L_EXPR.push_back(al_val_fn);
        return;
    }

    if(is_mtfn(op))
    {
        LEC_MTFN al_mtfn(op, nargs, 0);
        L_EXPR.push_back(al_mtfn);
        return;
    }

    throw std::invalid_argument("L_append_fn(): Invalid function type for atomic LEC element: " + std::to_string(op));
}


/**
 * Appends a long constant to L_EXPR.
 * 
 * @param [in] a    int     value of the constant
*/
static void L_append_const(int a)
{
    LEC_CONST_LONG al(a);
    L_EXPR.push_back(al);
}


/**
 * Tries to analytically solve an equation with respect to a specified endogenous variable. The result
 * is thus the equivalent of a LEC *expression*, not *equation*. That transformed expression is used in simuations.
 * 
 * The result is stored in L_EXPR (table of ALEC's).
 * 
 * If the endogenous variable is found more than once, the equation (say "LHS := RHS") is replaced by 
 * "0 := LHS - RHS" and duplicated_endo is true.
 *
 * For example,
 *   L_invert("ln X := a + b * ln Y", "X", &duplicated_endo) => "exp(a + b * ln Y)"    with duplicated_endo = false
 *   L_invert("ln X := a + b * X ",   "X", &duplicated_endo) => "ln X - (a + b * X)"   with duplicated_endo = true
 * 
 * 
 * L_errno can take the values below:
 *      - L_DUP_ERR    : duplicate endo in equation (cannot be solved)
 *		- L_INVERT_ERR : cannot invert an operator  (ex "(X<t)*2 := Y")
 *		- L_ASSIGN_ERR : sign := not found
 *      - other        : standard compilation errors or 0 on success
 *    
 * @param [in]   eq                 char*       text of the LEC equation
 * @param [in]   endo               char*       name of the endogenous variable 
 * @param [out]  duplicated_endo    bool&       false if the equation has been inverted, 
 *                                              true if endo is present more than once.
 * @return                          int         0 on success and L_errno on error
*/
int L_invert(const std::string& eq, const std::string& endo, bool& duplicated_endo)
{ 
    SLEC slec;

    // Compiles the 2 members of eq and put the result in slec
    if(initialize_eq(slec, eq)) 
        return L_errno;
    
    // FIND MEMBER CONTAINING ENDO AND SET INFO IN slec
    duplicated_endo = false;
    slec.sl_endo = endo;
    int count0 = L_count_endo(slec.sl_left_expr, slec.sl_endo);
    int count1 = L_count_endo(slec.sl_right_expr, slec.sl_endo);

    // If endo is not present in the equation, we cannot invert it
    if(count0 + count1 == 0) 
    {
        L_errno = L_DUP_ERR;
        return L_errno;
    }

    // If endo is present more than once, we cannot determine the sub expression 
    // to invert and we just move all the equation in L_EXPR and return
    if(count0 + count1 >= 2) 
    {
        // Result = {slec.sl_left_expr, slec.sl_right_expr, L_MINUS, L_EOE} i.e. F(x) = LHS - RHS 
        duplicated_endo = true;
        L_EXPR.clear();
        L_append(slec, EQ_LHS);    
        L_append(slec, EQ_RHS);
        L_append_op(L_MINUS);
        L_append_other(L_EOE);
        return 0;
    }

    // endo present in the left hand side
    if(count0 == 0) 
    {
        slec.sl_side_with_endo = EQ_RHS;
        slec.sl_side_without_endo = EQ_LHS;
    }
    // endo present in the right hand side
    else 
    {
        slec.sl_side_with_endo = EQ_LHS;
        slec.sl_side_without_endo = EQ_RHS;
    }

    // Create an empty L_EXPR and move the member not containing the endo into L_EXPR
    L_EXPR.clear();
    L_append(slec, slec.sl_side_without_endo);
    if(slec.sl_side_without_endo == EQ_LHS) 
        slec.sl_left_expr.clear();
    else 
        slec.sl_right_expr.clear();

    // if the equation side containing the endogenous variable is not trivial 
    // -> it must thus be inverted
    // NOTE: L_split_expr will split the equation side containing the endogenous variable 
    //       into 2 parts and store them in slec.sl_left_expr and slec.sl_right_expr
    while(L_split_expr(slec))
    {
        switch(slec.sl_op) 
        {
            // x + y = z  =>  x = z - y
            case L_PLUS   :             
                L_append(slec, slec.sl_side_without_endo);
                L_append_op(L_MINUS);
                break;
            // x * y = z  =>  x = z / y
            case L_TIMES  :
                L_append(slec, slec.sl_side_without_endo);
                L_append_op(L_DIVIDE);
                break;
            case L_MINUS  :
                // y - x = z  =>  x = y - z
                if(slec.sl_side_with_endo == EQ_RHS)     // x is after the minus sign
                {
                    L_front(slec, slec.sl_side_without_endo);
                    L_append_op(L_MINUS);
                }
                // x - y = z  =>  x = z + y
                else 
                {
                    L_append(slec, slec.sl_side_without_endo);
                    L_append_op(L_PLUS);
                }
                break;
            case L_DIVIDE :
                // y / x = z  =>  x = y / z
                if(slec.sl_side_with_endo == EQ_RHS)     // x is after the divide sign
                {
                    L_front(slec, slec.sl_side_without_endo);
                    L_append_op(L_DIVIDE);
                }
                // x / y = z  =>  x = z * y
                else 
                {
                    L_append(slec, slec.sl_side_without_endo);
                    L_append_op(L_TIMES);
                }
                break;
            // ln(x) = z  =>  x = exp(z)
            case L_LN :
                L_append_fn(L_EXPN, 1);
                break;
            // exp(x) = z  =>  x = ln(z)
            case L_EXPN :
                L_append_fn(L_LN, 1);
                break;
            // +x = z  =>  x = z
            case L_UPLUS  :
                break;
            // -x = z  =>  x = -z
            case L_UMINUS :
                L_append_fn(L_UMINUS, 1);
                break;
            // cos(x) = z  =>  x = arccos(z)
            case L_COS :
                L_append_fn(L_ACOS, 1);
                break;
            // arccos(x) = z  =>  x = cos(z)
            case L_ACOS :
                L_append_fn(L_COS, 1);
                break;
            // sin(x) = z  =>  x = arcsin(z)
            case L_SIN :
                L_append_fn(L_ASIN, 1);
                break;
            // arcsin(x) = z  =>  x = sin(z)
            case L_ASIN :
                L_append_fn(L_SIN, 1);
                break;
            // tan(x) = z  =>  x = arctan(z)
            case L_TAN :
                L_append_fn(L_ATAN, 1);
                break;
            // arctan(x) = z  =>  x = tan(z)
            case L_ATAN :
                L_append_fn(L_TAN, 1);
                break;
            // sqrt(x) = z  =>  x = z ^ 2
            case L_SQRT :
                L_append_const(2);
                L_append_fn(L_EXP, 2);
                break;
            case L_EXP :
                // NOTE: log_b(a) = ln(a) / ln(b)
                // y ^ x = z
                // y = log_x(z) = ln(z) / ln(x)
                // ln(x) = ln(z) / y
                // x = exp(ln(z) / y)
                if(slec.sl_side_with_endo == EQ_LHS)     // x is after the exponentiation operator
                {
                    L_append_fn(L_LN, 1);
                    L_append(slec, slec.sl_side_without_endo);
                    L_append_op(L_DIVIDE);
                    L_append_fn(L_EXPN, 1);
                }
                // x ^ y = z  =>  x = log_y(z)
                else 
                {
                    L_front(slec, slec.sl_side_without_endo);
                    L_append_fn(L_LOG, 2);
                }
                break;
            // log(x) = z  =>  x = 10 ^ z
            case L_LOG :
                L_append_fn(L_EXP, 1);
                break;
            // d(x) = x[t] - x[t-lag] = z[t]  =>  x[t] = z[t] + x[t-lag]
            case L_DIFF :
                if(slec.sl_nargs == 2) L_append(slec, slec.sl_side_without_endo);
                L_append(slec, slec.sl_side_with_endo);
                L_append_fn(L_LAG, slec.sl_nargs);
                L_append_op(L_PLUS);
                break;
            // dln(x) = ln(x[t]) - ln(x[t-lag]) = z[t]  
            // ln(x[t]) = z[t] + ln(x[t-lag])
            // x[t] = exp(z[t] + ln(x[t-lag]))    
            case L_DLN :
                if(slec.sl_nargs == 2) L_append(slec, slec.sl_side_without_endo);
                L_append(slec, slec.sl_side_with_endo);
                L_append_fn(L_LAG, slec.sl_nargs);
                L_append_fn(L_LN, 1);
                L_append_op(L_PLUS);
                L_append_fn(L_EXPN, 1);
                break;
            // grt(x) = 100 * (x[t] / x[t-lag] - 1) = z[t]
            // (x[t] / x[t-lag] - 1) = z[t] / 100
            // x[t] / x[t-lag] = (z[t] / 100) + 1
            // x[t] = ((z[t] / 100) + 1) * x[t-lag]
            case L_GRT :
                L_append_const(100);
                L_append_op(L_DIVIDE);
                L_append_const(1);
                L_append_op(L_PLUS);
                if(slec.sl_nargs == 2) L_append(slec, slec.sl_side_without_endo);
                L_append(slec, slec.sl_side_with_endo);
                L_append_fn(L_LAG, slec.sl_nargs);
                L_append_op(L_TIMES);
                break;
            // rapp(x) = x[t] / x[t-lag] = z[t]
            // x[t] = z[t] * x[t-lag]
            case L_RAPP :
                if(slec.sl_nargs == 2) L_append(slec, slec.sl_side_without_endo);
                L_append(slec, slec.sl_side_with_endo);
                L_append_fn(L_LAG, slec.sl_nargs);
                L_append_op(L_TIMES);
                break;
            default :
                slec.sl_left_expr.clear();
                slec.sl_right_expr.clear();
                L_errno = L_INVERT_ERR;
                return L_errno;
        }
    }

    slec.sl_left_expr.clear();
    slec.sl_right_expr.clear();
    L_append_other(L_EOE);
    
    return L_errno;
}


/**
 * Returns the position of ":=" in an equation or -1 if not found.
 * 
 * @param [in] eq   char*   equation to analyse
 * @return          int     position of := in the equation or -1 if not found.
*/
int L_split_eq(const std::string& eq)
{
    for(int i = 0 ; eq[i] != 0 ; i++)
    {
        if(eq[i] == ':' && eq[i + 1] == '=') 
            return i;
    }

    return -1;
}

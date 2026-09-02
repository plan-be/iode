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


int SLEC::count_endo(const EQ_HAND_SIDE mbr)
{
    std::shared_ptr<CLEC> clec_ptr = (mbr == EQ_LHS) ? left_clec : right_clec;
    if(!clec_ptr)
        return 0;
    
    int count = 0;
    for(const ATOMIC_LEC& al : clec_ptr->v_expression)
    {
        if(!std::holds_alternative<LEC_VAR>(al))
            continue;

        const LEC_VAR& al_var = std::get<LEC_VAR>(al);

        // the lagged (A[-1]) and timed (A[2000Y1]) variables are not taken into account.
        if(al_var.type != L_VAR || al_var.per.step != 0 || al_var.lag != 0)
            continue;

        if(al_var.name == endo)
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
 * @return              bool    false if there was nothing to split, true if the  
 *                              expression was split or if an error occurred
*/
bool SLEC::split_expr()
{
    int start_sub_expr = -1;

    if(!left_clec || !right_clec)
        return false;

    // create a copy of the vector of atomic LEC elements from the equation's side 
    // containing the endogenous variable of the equation (LHS or RHS)
    std::shared_ptr<CLEC> clec_ptr = (side_with_endo == EQ_LHS) ? left_clec : right_clec;
    std::vector<ATOMIC_LEC> v_al = clec_ptr->v_expression;
    
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

    func_index = type;
    nb_args = al_exec.nb_args;

    // remove the last element of v_al (the operator/function) 
    // -> previously stored in func_index and nb_args
    v_al.pop_back();

    // Search the beginning of the sub expression
    start_sub_expr = find_sub_expr_start(v_al, (int) v_al.size() - 1);
    if(start_sub_expr < 0)
    {
        L_errno = L_DUP_ERR;
        return true; 
    }

    left_clec->v_expression.clear();
    right_clec->v_expression.clear();

    auto it_sub_expr = v_al.begin() + start_sub_expr;

    // We move the elements before the sub expression to left_clec
    if(it_sub_expr != v_al.begin())
        left_clec->v_expression.insert(left_clec->v_expression.end(), v_al.begin(), it_sub_expr);

    // and the elements after the sub expression to right_clec
    if(it_sub_expr != v_al.end())
        right_clec->v_expression.insert(right_clec->v_expression.end(), it_sub_expr, v_al.end());

    v_al.clear(); // we clear v_al to avoid confusion (we won't use it anymore)

    // find the member where endo is present
    if(count_endo(EQ_LHS) > 0) 
    {       
        side_with_endo = EQ_LHS;                                   
        side_without_endo = EQ_RHS;                               
    }
    else 
    {
        side_with_endo = EQ_RHS;
        side_without_endo = EQ_LHS;
    }

    return true;
}


SLEC::SLEC(const std::string& lec, const std::string& endo)
{
    this->endo = endo;

    // split the equation into LHS and RHS according to :=
    std::size_t pos = lec.find(":=");
    if(pos == std::string::npos) 
    {
        L_errno = L_ASSIGN_ERR;
        return;
    }

    // make a copy of the equation to modify it
    std::string lhs_lec = lec.substr(0, pos);
    std::string rhs_lec = lec.substr(pos + 2);

    lhs_lec = trim(lhs_lec);
    rhs_lec = trim(rhs_lec);

    // Compiles left member
    left_clec = std::make_shared<CLEC>(lhs_lec, true);
    if(!left_clec) 
        return;
    // drop the last element (L_EOE)
    left_clec->v_expression.pop_back();
    
    // Compiles the right member
    right_clec = std::make_shared<CLEC>(rhs_lec, true);
    if(!right_clec) 
        return;
    // drop the last element (L_EOE)
    right_clec->v_expression.pop_back();
}


/**
 * Appends the member mbr (LHS or RHS) contained in sl to expr. 
 * 
 * @param [in] mbr  int     position of the member to append 
*/
void SLEC::append_member(const EQ_HAND_SIDE mbr, std::vector<ATOMIC_LEC>& expr)
{
    std::shared_ptr<CLEC> clec_ptr = (mbr == EQ_LHS) ? left_clec : right_clec;
    if(!clec_ptr)
        return;
    expr.insert(expr.end(), clec_ptr->v_expression.begin(), clec_ptr->v_expression.end());
}


/**
 * Adds expr the member mbr (LHS or RHS) contained in sl at the beginning of expr.
 * 
 * @param [in] mbr  int     member of sl to copy to expr
*/
void SLEC::prepend_member(const EQ_HAND_SIDE mbr, std::vector<ATOMIC_LEC>& expr)
{
    std::shared_ptr<CLEC> clec_ptr = (mbr == EQ_LHS) ? left_clec : right_clec;
    if(!clec_ptr)
        return;
    expr.insert(expr.begin(), clec_ptr->v_expression.begin(), clec_ptr->v_expression.end());
}


void SLEC::append_other(int type, std::vector<ATOMIC_LEC>& expr)
{
    LEC_OTHER al_other(type);
    expr.push_back(al_other);
}


/**
 * Appends an operator to expr.
 * 
 * @param [in]  op  int     operator
*/
void SLEC::append_op(int op, std::vector<ATOMIC_LEC>& expr)
{
    LEC_OP al_op(op);
    expr.push_back(al_op);
}


/**
 * Appends a function and its number of arguments to expr.
 *
 * @param [in] op     int   function id
 * @param [in] nargs  int   number of arguments of the function
*/
void SLEC::append_fn(int op, int nargs, std::vector<ATOMIC_LEC>& expr)
{
    if(is_fn(op))
    {
        LEC_FN al_fn(op, nargs);
        expr.push_back(al_fn);
        return;
    }

    if(is_tfn(op))
    {
        LEC_TFN al_tfn(op, nargs);
        expr.push_back(al_tfn);
        return;
    }

    if(is_val(op))
    {
        LEC_VAL_FN al_val_fn(op);
        expr.push_back(al_val_fn);
        return;
    }

    if(is_mtfn(op))
    {
        LEC_MTFN al_mtfn(op, nargs, 0);
        expr.push_back(al_mtfn);
        return;
    }

    throw std::invalid_argument("append_fn(): Invalid function type for atomic LEC element: " + std::to_string(op));
}


/**
 * Appends a long constant to expr.
 * 
 * @param [in] a    int     value of the constant
*/
void SLEC::append_const(int a, std::vector<ATOMIC_LEC>& expr)
{
    LEC_CONST_LONG al(a);
    expr.push_back(al);
}


/**
 * Tries to analytically solve an equation with respect to a specified endogenous variable. The result
 * is thus the equivalent of a LEC *expression*, not *equation*. That transformed expression is used in simuations.
 * 
 * The result is stored in clec.v_expression.
 * 
 * If the endogenous variable is found more than once, the equation (say "LHS := RHS") is replaced by 
 * "0 := LHS - RHS" and duplicated_endo is true.
 *
 * For example,
 *   invert_equation("ln X := a + b * ln Y", "X", &duplicated_endo) => "exp(a + b * ln Y)"    with duplicated_endo = false
 *   invert_equation("ln X := a + b * X ",   "X", &duplicated_endo) => "ln X - (a + b * X)"   with duplicated_endo = true
 * 
 * 
 * L_errno can take the values below:
 *      - L_DUP_ERR    : duplicate endo in equation (cannot be solved)
 *		- L_INVERT_ERR : cannot invert an operator  (ex "(X<t)*2 := Y")
 *		- L_ASSIGN_ERR : sign := not found
 *      - other        : standard compilation errors or 0 on success
 * 
 * @param [out]  duplicated_endo    bool&       false if the equation has been inverted, 
 *                                              true if endo is present more than once.
 * @return                          int         0 on success and L_errno on error
*/
int SLEC::invert_equation(bool& duplicated_endo, CLEC& clec)
{
    if(L_errno != 0)
        return L_errno;
    
    // FIND MEMBER CONTAINING ENDO
    duplicated_endo = false;
    this->endo = endo;
    int count0 = count_endo(EQ_LHS);
    int count1 = count_endo(EQ_RHS);

    // If endo is not present in the equation, we cannot invert it
    if(count0 + count1 == 0) 
    {
        L_errno = L_DUP_ERR;
        return L_errno;
    }

    // If endo is present more than once, we cannot determine the sub expression 
    // to invert and we just move all the equation in clec.v_expression and return
    if(count0 + count1 >= 2) 
    {
        // Result = {left_clec, right_clec, L_MINUS, L_EOE} i.e. F(x) = LHS - RHS 
        duplicated_endo = true;
        clec.v_expression.clear();
        append_member(EQ_LHS, clec.v_expression);    
        append_member(EQ_RHS, clec.v_expression);
        append_op(L_MINUS, clec.v_expression);
        append_other(L_EOE, clec.v_expression);
        return 0;
    }

    // endo present in the left hand side
    if(count0 == 0) 
    {
        side_with_endo = EQ_RHS;
        side_without_endo = EQ_LHS;
    }
    // endo present in the right hand side
    else 
    {
        side_with_endo = EQ_LHS;
        side_without_endo = EQ_RHS;
    }

    // Create an empty expression and move the member not containing the endo into it.
    clec.v_expression.clear();
    append_member(side_without_endo, clec.v_expression);
    if(side_without_endo == EQ_LHS) 
        left_clec->v_expression.clear();
    else 
        right_clec->v_expression.clear();

    // if the equation side containing the endogenous variable is not trivial 
    // -> it must thus be inverted
    // NOTE: split_expr will split the equation side containing the endogenous variable 
    //       into 2 parts and store them in left_clec and right_clec
    while(split_expr())
    {
        switch(func_index) 
        {
            // x + y = z  =>  x = z - y
            case L_PLUS   :             
                append_member(side_without_endo, clec.v_expression);
                append_op(L_MINUS, clec.v_expression);
                break;
            // x * y = z  =>  x = z / y
            case L_TIMES  :
                append_member(side_without_endo, clec.v_expression);
                append_op(L_DIVIDE, clec.v_expression);
                break;
            case L_MINUS  :
                // y - x = z  =>  x = y - z
                if(side_with_endo == EQ_RHS)     // x is after the minus sign
                {
                    prepend_member(side_without_endo, clec.v_expression);
                    append_op(L_MINUS, clec.v_expression);
                }
                // x - y = z  =>  x = z + y
                else 
                {
                    append_member(side_without_endo, clec.v_expression);
                    append_op(L_PLUS, clec.v_expression);
                }
                break;
            case L_DIVIDE :
                // y / x = z  =>  x = y / z
                if(side_with_endo == EQ_RHS)     // x is after the divide sign
                {
                    prepend_member(side_without_endo, clec.v_expression);
                    append_op(L_DIVIDE, clec.v_expression);
                }
                // x / y = z  =>  x = z * y
                else 
                {
                    append_member(side_without_endo, clec.v_expression);
                    append_op(L_TIMES, clec.v_expression);
                }
                break;
            // ln(x) = z  =>  x = exp(z)
            case L_LN :
                append_fn(L_EXPN, 1, clec.v_expression);
                break;
            // exp(x) = z  =>  x = ln(z)
            case L_EXPN :
                append_fn(L_LN, 1, clec.v_expression);
                break;
            // +x = z  =>  x = z
            case L_UPLUS  :
                break;
            // -x = z  =>  x = -z
            case L_UMINUS :
                append_fn(L_UMINUS, 1, clec.v_expression);
                break;
            // cos(x) = z  =>  x = arccos(z)
            case L_COS :
                append_fn(L_ACOS, 1, clec.v_expression);
                break;
            // arccos(x) = z  =>  x = cos(z)
            case L_ACOS :
                append_fn(L_COS, 1, clec.v_expression);
                break;
            // sin(x) = z  =>  x = arcsin(z)
            case L_SIN :
                append_fn(L_ASIN, 1, clec.v_expression);
                break;
            // arcsin(x) = z  =>  x = sin(z)
            case L_ASIN :
                append_fn(L_SIN, 1, clec.v_expression);
                break;
            // tan(x) = z  =>  x = arctan(z)
            case L_TAN :
                append_fn(L_ATAN, 1, clec.v_expression);
                break;
            // arctan(x) = z  =>  x = tan(z)
            case L_ATAN :
                append_fn(L_TAN, 1, clec.v_expression);
                break;
            // sqrt(x) = z  =>  x = z ^ 2
            case L_SQRT :
                append_const(2, clec.v_expression);
                append_fn(L_EXP, 2, clec.v_expression);
                break;
            case L_EXP :
                // NOTE: log_b(a) = ln(a) / ln(b)
                // y ^ x = z
                // y = log_x(z) = ln(z) / ln(x)
                // ln(x) = ln(z) / y
                // x = exp(ln(z) / y)
                if(side_with_endo == EQ_LHS)     // x is after the exponentiation operator
                {
                    append_fn(L_LN, 1, clec.v_expression);
                    append_member(side_without_endo, clec.v_expression);
                    append_op(L_DIVIDE, clec.v_expression);
                    append_fn(L_EXPN, 1, clec.v_expression);
                }
                // x ^ y = z  =>  x = log_y(z)
                else 
                {
                    prepend_member(side_without_endo, clec.v_expression);
                    append_fn(L_LOG, 2, clec.v_expression);
                }
                break;
            // log(x) = z  =>  x = 10 ^ z
            case L_LOG :
                append_fn(L_EXP, 1, clec.v_expression);
                break;
            // d(x) = x[t] - x[t-lag] = z[t]  =>  x[t] = z[t] + x[t-lag]
            case L_DIFF :
                if(nb_args == 2) append_member(side_without_endo, clec.v_expression);
                append_member(side_with_endo, clec.v_expression);
                append_fn(L_LAG, nb_args, clec.v_expression);
                append_op(L_PLUS, clec.v_expression);
                break;
            // dln(x) = ln(x[t]) - ln(x[t-lag]) = z[t]  
            // ln(x[t]) = z[t] + ln(x[t-lag])
            // x[t] = exp(z[t] + ln(x[t-lag]))    
            case L_DLN :
                if(nb_args == 2) append_member(side_without_endo, clec.v_expression);
                append_member(side_with_endo, clec.v_expression);
                append_fn(L_LAG, nb_args, clec.v_expression);
                append_fn(L_LN, 1, clec.v_expression);
                append_op(L_PLUS, clec.v_expression);
                append_fn(L_EXPN, 1, clec.v_expression);
                break;
            // grt(x) = 100 * (x[t] / x[t-lag] - 1) = z[t]
            // (x[t] / x[t-lag] - 1) = z[t] / 100
            // x[t] / x[t-lag] = (z[t] / 100) + 1
            // x[t] = ((z[t] / 100) + 1) * x[t-lag]
            case L_GRT :
                append_const(100, clec.v_expression);
                append_op(L_DIVIDE, clec.v_expression);
                append_const(1, clec.v_expression);
                append_op(L_PLUS, clec.v_expression);
                if(nb_args == 2) append_member(side_without_endo, clec.v_expression);
                append_member(side_with_endo, clec.v_expression);
                append_fn(L_LAG, nb_args, clec.v_expression);
                append_op(L_TIMES, clec.v_expression);
                break;
            // rapp(x) = x[t] / x[t-lag] = z[t]
            // x[t] = z[t] * x[t-lag]
            case L_RAPP :
                if(nb_args == 2) append_member(side_without_endo, clec.v_expression);
                append_member(side_with_endo, clec.v_expression);
                append_fn(L_LAG, nb_args, clec.v_expression);
                append_op(L_TIMES, clec.v_expression);
                break;
            default :
                L_errno = L_INVERT_ERR;
                return L_errno;
        }
    }

    append_other(L_EOE, clec.v_expression);    
    return L_errno;
}

void SLEC::merge_names(CLEC& clec)
{
    if(L_errno != 0)
        return;

    if(!left_clec || !right_clec)
        return;

    clec.v_obj_names.clear();

    // Copy from left_clec
    clec.v_obj_names = left_clec->v_obj_names;
    
    // Copy from right_clec (skip duplicates)
    std::set<std::string> s_objs(clec.v_obj_names.begin(), clec.v_obj_names.end());
    for(const std::string& name : right_clec->v_obj_names)
    {
        if(!s_objs.contains(name))
            clec.v_obj_names.push_back(name);
    }
}


/**
 * Returns the position of ":=" in an equation or -1 if not found.
 * 
 * @param [in] eq   char*   equation to analyse
 * @return          int     position of := in the equation or -1 if not found.
*/
int split_eq(const std::string& eq)
{
    for(int i = 0 ; eq[i] != 0 ; i++)
    {
        if(eq[i] == ':' && eq[i + 1] == '=') 
            return i;
    }

    return -1;
}

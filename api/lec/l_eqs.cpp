/**
 * @header4iode
 *
 * Functions to compile LEC *equations*.
 * 
 * Main functions
 * --------------
 *    CLEC* L_solve(char* eq, char* endo)  Compiles a LEC equation and tries to analytically solve the equation with respect to endo.
 *    int L_split_eq(char* eq)             Returns the position of ":=" in an equation or -1 if not found.
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

#include "api/lec/lec.h"
#include <algorithm>        // for std::move


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
 * If sl->sl_expr[sl->sl_side_with_endo] is not trivial, splits 
 * sl->sl_expr[sl->sl_side_with_endo] into two parts and replaces 
 * sl->sl_expr[sl->sl_side_with_endo] and sl->sl_expr[sl->sl_side_without_endo] 
 * by these 2 expressions.
 * 
 * ------------------------------------------------------
 * Example: 
 *
 * Expression containing the endogenous : "EXO + ln ENDO"
 * 
 * At the beginning of the function:
 * 
 *  sl->sl_expr[sl->sl_side_with_endo]:  | EXO  |
 *              | (    |
 *              | ENDO |
 *              | )    |
 *              | ln   |
 *              | +    |
 * 
 *  At the end of the function:
 *     sl->sl_left_expr =   EXO
 *     sl->sl_right_expr =  | (    |
 *                | ENDO |
 *                | )    |
 *                | ln   |
 *     sl->sl_side_with_endo = EQ_RHS
 *     sl->sl_side_without_endo = EQ_LHS
 * -------------------------------------------------------
 * 
 * @param [in, out]     sl  SLEC*   current state of the equation
 * @return                  int     0 on success, L_DUP_ERR if the sub expression cannot be determined
*/
static int L_split_expr(SLEC* sl)
{
    int start_sub_expr = -1;

    // ALEC containing the sl->sl_endo
    std::vector<ATOMIC_LEC> v_al;
    if(sl->sl_side_with_endo == EQ_LHS)
        v_al = sl->sl_left_expr;
    else
        v_al = sl->sl_right_expr;
    
ag:
    if(v_al.size() == 0)
    {
        L_errno = L_DUP_ERR;
        return L_errno; 
    }

    const ATOMIC_LEC& al = v_al.back();

    int type = std::visit([](auto&& arg) -> int { return arg.type; }, al);

    // if the last element is L_CLOSEP, the first on must be L_OPENP (?)
    if(type == L_CLOSEP)
    {
        // drop the first element of v_al
        v_al.erase(v_al.begin());
        // drop the last element of v_al
        v_al.pop_back();
        // we try the next element 
        goto ag;
    }

    // if the last element is L_VAR, we have only one item and the process is terminated (?)
    if(type == L_VAR)
        return 1;

    if(!is_executable(type))
    {
        L_errno = L_DUP_ERR;
        return L_errno; 
    }

    // cast to LEC_EXECUTABLE to get the number of arguments
    const LEC_EXECUTABLE& al_exec = reinterpret_cast<const LEC_EXECUTABLE&>(al);

    sl->sl_op = type;
    sl->sl_nargs = al_exec.nb_args;

    // QUESTION: why ?
    v_al.pop_back();

    // Search the beginning of the sub expression
    start_sub_expr = L_sub_expr(v_al, (int) v_al.size() - 1);
    if(start_sub_expr < 0)
    {
        L_errno = L_DUP_ERR;
        return L_errno; 
    }

    sl->sl_left_expr.clear();
    sl->sl_right_expr.clear();

    auto it_sub_expr = v_al.begin() + start_sub_expr;

    // We move the elements before the sub expression to sl->sl_left_expr
    if(it_sub_expr != v_al.begin())
        sl->sl_left_expr.insert(sl->sl_left_expr.end(), v_al.begin(), it_sub_expr);

    // and the elements after the sub expression to sl->sl_right_expr
    if(it_sub_expr != v_al.end())
        sl->sl_right_expr.insert(sl->sl_right_expr.end(), it_sub_expr, v_al.end());

    v_al.clear(); // we clear v_al to avoid confusion (we won't use it anymore)

    // find the member where endo is present
    if(L_count_endo(sl->sl_left_expr, sl->sl_endo) > 0) 
    {       
        sl->sl_side_with_endo = EQ_LHS ;                                   
        sl->sl_side_without_endo = EQ_RHS;                               
    }
    else 
    {
        sl->sl_side_with_endo = EQ_RHS ;
        sl->sl_side_without_endo = EQ_LHS;
    }

    return 0;
}

/**
 * Compiles a LEC expression and allocates the ALEC table resulting from the compilation. 
 * 
 * As this function is used to compile *equations*, it is called twice, once for the left member (LHS)
 * and once for the right member (RHS). 
 * For that reason:
 *      nb_names = 0 for first call (no names are already known) 
 *      nb_names = L_NB_NAMES for second member (all names in LHS are already in L_NAMES).
      
 * L_errno contains 0 or the error number.
 * 
 * @param [in] lec           char*   LEC expression to compile
 * @return                   ALEC*   allocated table of ALEC's or NULL on error
*/
static std::vector<ATOMIC_LEC> L_cc1_alloc(char* lec)
{
    std::vector<ATOMIC_LEC> v_al;
    if(L_open_string(lec)) 
        return v_al;
    
    if(L_cc1() != 0) 
        return v_al;
    
    L_close();

    v_al = L_EXPR;
    return v_al;
}


/**
 * Splits an equation in LHS and RHS and compiles (step 1 only) each element separately. 
 * The error L_ASSIGN_ERR is returned if ":=" is not found in the equation.
 * 
 * The result is stored in a SLEC struct (see above).
 * Only the step 1 of the compilation is performed at this stage.
 * 
 * @param [in, out]     sl    SLEC*   struct where the A
 * @param [in]          eq    char*   text of the equation
 * @return                    int     error code or 0 on success
 *                                      
*/
static int L_cc1_eq(SLEC* sl, char* eq)
{
    L_NAMES.clear();

    /* SLIT EQ AND COMPILE EACH MEMBER */
    int pos = L_split_eq(eq);
    if(pos < 0)
    {
        L_errno = L_ASSIGN_ERR;
        return L_errno;
    }

    // Compiles left member
    eq[pos] = 0;                                // close the left member
    sl->sl_left_expr = L_cc1_alloc(eq);
    sl->sl_left_expr.pop_back();                // drop the last element (L_EOE)        
    L_EXPR.clear();                       // Clean up L_EXPR
    eq[pos] = ':';                              // Reset the original text
    if(sl->sl_left_expr.empty()) 
        return L_errno;
    
    // Compiles the right member
    sl->sl_right_expr = L_cc1_alloc(eq + pos + 2);
    sl->sl_right_expr.pop_back();               // drop the last element (L_EOE)
    L_EXPR.clear();
    if(sl->sl_right_expr.empty()) 
        return L_errno;

    return 0;
}


/**
 * Appends the member mbr (LHS or RHS) contained in sl to L_EXPR. 
 * 
 * @param [in] sl   SLEC*   
 * @param [in] mbr  int     position of the member to append 
*/
static void L_append(SLEC* sl, const EQ_HAND_SIDE mbr)
{
    std::vector<ATOMIC_LEC>& v_al = (mbr == EQ_LHS) ? sl->sl_left_expr : sl->sl_right_expr;
    L_EXPR.insert(L_EXPR.end(), v_al.begin(), v_al.end());
}


/**
 * Adds L_EXPR the member mbr (LHS or RHS) contained in sl at the beginning of L_EXPR.
 * 
 * @param [in] sl   SLEC*   container of the compiled equation
 * @param [in] mbr  int     member of sl to copy to L_EXPR
*/
static void L_front(SLEC* sl, const EQ_HAND_SIDE mbr)
{
    std::vector<ATOMIC_LEC>& v_al = (mbr == EQ_LHS) ? sl->sl_left_expr : sl->sl_right_expr;
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
 * If the endogenous variable is found more than once, the equation (say "LHS := RHS") is replaced by "0 := LHS - RHS" and duplicated_endo is set to 1.
 *
 * For example,
 *   L_invert("ln X := a + b * ln Y", "X", &duplicated_endo) => "exp(a + b * ln Y)"    with duplicated_endo = 0
 *   L_invert("ln X := a + b * X ",   "X", &duplicated_endo) => "ln X - (a + b * X)"   with duplicated_endo = 1
 * 
 * 
 * L_errno can take the values below:
 *      - L_DUP_ERR    : duplicate endo in equation (cannot be solved)
 *		- L_INVERT_ERR : cannot invert an operator  (ex "(X<t)*2 := Y")
 *		- L_ASSIGN_ERR : sign := not found
 *      - other        : standard compilation errors or 0 on success
 *    
 * @param [in]   eq         char*   text of the LEC equation
 * @param [in]   endo       char*   name of the endogenous variable 
 * @param [out]  duplicated_endo    int*    0 if the equation has been inverted, 1 if endo is present more than once.
 * @return                  int     0 on success and L_errno on error
*/
static int L_invert(char* eq, char* endo, int *duplicated_endo)
{ 
    SLEC slec, *sl = &slec;

    // Compiles the 2 members of eq and put the result in slec
    if(L_cc1_eq(sl, eq)) 
        return L_errno;
    
    /* FIND MEMBER CONTAINING ENDO AND SET INFO IN sl */
    *duplicated_endo = 0;
    sl->sl_endo = endo;
    int count0 = L_count_endo(sl->sl_left_expr, sl->sl_endo);
    int count1 = L_count_endo(sl->sl_right_expr, sl->sl_endo);

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
        // Result = {sl->sl_left_expr, sl->sl_right_expr, L_MINUS, L_EOE} i.e. F(x) = LHS - RHS 
        *duplicated_endo = 1;
        L_EXPR.clear();
        L_append(sl, EQ_LHS);    
        L_append(sl, EQ_RHS);
        L_append_op(L_MINUS);
        L_append_other(L_EOE);
        return 0;
    }

    // endo present in the left hand side
    if(count0 == 0) 
    {
        sl->sl_side_with_endo = EQ_RHS;
        sl->sl_side_without_endo = EQ_LHS;
    }
    // endo present in the right hand side
    else 
    {
        sl->sl_side_with_endo = EQ_LHS;
        sl->sl_side_without_endo = EQ_RHS;
    }

    // Create an empty L_EXPR and move the member not containing the endo into L_EXPR
    L_EXPR.clear();
    L_append(sl, sl->sl_side_without_endo);
    if(sl->sl_side_without_endo == EQ_LHS) 
        sl->sl_left_expr.clear();
    else 
        sl->sl_right_expr.clear();

    // sl->sl_expr[sl->sl_side_with_endo] is the expression containing endo 
    // and must thus be inverted
    /* LOOP ON OPERATORS */
    while(L_split_expr(sl) == 0)
    {
        switch(sl->sl_op) 
        {
            case L_PLUS   :             
                L_append(sl, sl->sl_side_without_endo);
                L_append_op(L_MINUS);
                break;
            case L_TIMES  :
                L_append(sl, sl->sl_side_without_endo);
                L_append_op(L_DIVIDE);
                break;
            case L_MINUS  :
                if(sl->sl_side_with_endo == EQ_RHS) 
                {
                    L_front(sl, sl->sl_side_without_endo);
                    L_append_op(L_MINUS);
                }
                else 
                {
                    L_append(sl, sl->sl_side_without_endo);
                    L_append_op(L_PLUS);
                }
                break;
            case L_DIVIDE :
                if(sl->sl_side_with_endo == EQ_RHS) 
                {
                    L_front(sl, sl->sl_side_without_endo);
                    L_append_op(L_DIVIDE);
                }
                else 
                {
                    L_append(sl, sl->sl_side_without_endo);
                    L_append_op(L_TIMES);
                }
                break;
            case L_LN     :
                L_append_fn(L_EXPN, 1);
                break;
            case L_EXPN   :
                L_append_fn(L_LN, 1);
                break;
            case L_UPLUS  :
                break;
            case L_UMINUS :
                L_append_fn(L_UMINUS, 1);
                break;
            case L_COS    :
                L_append_fn(L_ACOS, 1);
                break;
            case L_ACOS   :
                L_append_fn(L_COS, 1);
                break;
            case L_SIN    :
                L_append_fn(L_ASIN, 1);
                break;
            case L_ASIN   :
                L_append_fn(L_SIN, 1);
                break;
            case L_TAN    :
                L_append_fn(L_ATAN, 1);
                break;
            case L_ATAN   :
                L_append_fn(L_TAN, 1);
                break;
            case L_SQRT   :
                L_append_const(2);
                L_append_fn(L_EXP, 2);
                break;
            case L_EXP    :
                if(sl->sl_side_with_endo == EQ_LHS) 
                {
                    L_append_fn(L_LN, 1);
                    L_append(sl, sl->sl_side_without_endo);
                    L_append_op(L_DIVIDE);
                    L_append_fn(L_EXPN, 1);
                }
                else 
                {
                    L_front(sl, sl->sl_side_without_endo);
                    L_append_fn(L_LOG, 2);
                }
                break;
            case L_LOG    :
                L_append_fn(L_EXP, 1);
                break; /* JMP 22-06-00 */
            case L_DIFF   :
                if(sl->sl_nargs == 2) L_append(sl, sl->sl_side_without_endo);
                L_append(sl, sl->sl_side_with_endo);
                L_append_fn(L_LAG, sl->sl_nargs);
                L_append_op(L_PLUS);
                break;
            case L_DLN    :
                if(sl->sl_nargs == 2) L_append(sl, sl->sl_side_without_endo);
                L_append(sl, sl->sl_side_with_endo);
                L_append_fn(L_LAG, sl->sl_nargs);
                L_append_fn(L_LN, 1);
                L_append_op(L_PLUS);
                L_append_fn(L_EXPN, 1);
                break;
            case L_GRT    :
                L_append_const(100);
                L_append_op(L_DIVIDE);
                L_append_const(1);
                L_append_op(L_PLUS);
                if(sl->sl_nargs == 2) L_append(sl, sl->sl_side_without_endo);
                L_append(sl, sl->sl_side_with_endo);
                L_append_fn(L_LAG, sl->sl_nargs);
                L_append_op(L_TIMES);
                break;
            case L_RAPP   :
                if(sl->sl_nargs == 2) L_append(sl, sl->sl_side_without_endo);
                L_append(sl, sl->sl_side_with_endo);
                L_append_fn(L_LAG, sl->sl_nargs);
                L_append_op(L_TIMES);
                break;
            default :
                sl->sl_left_expr.clear();
                sl->sl_right_expr.clear();
                L_errno = L_INVERT_ERR;
                return L_errno;
        }
    }

    sl->sl_left_expr.clear();
    sl->sl_right_expr.clear();
    L_append_other(L_EOE);
    
    return L_errno;
}


/**
 * Compiles a LEC equation and tries to analytically solve the equation with respect to endo.
 * 
 * Generates a CLEC form with the result and set clec->duplicated_endo to 1 if the
 * generated form is of the form "0 := LHS - RHS")
 * 
 * @param [in] eq       char*
 * @param [in] endo     char* 
 * @return              CLEC*
*/
CLEC* L_solve(char* eq, char* endo)
{
    CLEC* clec = nullptr;
    int duplicated_endo = 0;
    std::string lec = std::string(eq);

    L_invert(eq, endo, &duplicated_endo);
    switch(L_errno) 
    {
        case 0              :
            clec = L_cc2(L_EXPR, lec);
            if(clec) 
                clec->duplicated_endo = (char) duplicated_endo;
            break;
        case L_DUP_ERR      :
        case L_INVERT_ERR   :
        default             :
            break;
    }
    
    L_EXPR.clear();
    return clec;
}



/**
 * Returns the position of ":=" in an equation or -1 if not found.
 * 
 * @param [in] eq   char*   equation to analyse
 * @return          int     position of := in the equation or -1 if not found.
*/
int L_split_eq(char* eq)
{
    for(int i = 0 ; eq[i] != 0 ; i++)
    {
        if(eq[i] == ':' && eq[i + 1] == '=') 
            return(i);
    }

    return -1;
}

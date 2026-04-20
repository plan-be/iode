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
#include <variant>          // for std::variant


/**
 * Struct specific for equation compilation.
*/

enum EQ_HAND_SIDE
{
    EQ_LHS = 0,
    EQ_RHS = 1
};


struct SLEC 
{
    std::string sl_lec;                         // original LEC equation as a string (for debugging purposes)          
    std::vector<ATOMIC_LEC> sl_left_expr;       // Tables of ALEC's for LHS equations members 
    std::vector<ATOMIC_LEC> sl_right_expr;      // Tables of ALEC's for RHS equations members 
    EQ_HAND_SIDE     sl_side_with_endo;         // side containing sl_endo (will be inverted if possible)
    EQ_HAND_SIDE     sl_side_without_endo;      // side not containing sl_endo (will be left unchanged)
    int     sl_op;              // last operator|function
    int     sl_nargs;           // number of args of the last op|fn
    char*   sl_endo;            // name of the endogenous variable
};


/**
 * Counts the number of occurrences of a variable in a ALEC table of atomic expressions 
 * (result of l_cc1()). The lagged (A[-1]) and timed (A[2000Y1]) variables are not taken 
 * into account.
 * 
 * @param [in]  al      ALEC*   table of ALEC elements
 * @param [in]  endo    char*   name of the variable to search
 * @return              int     number of occurence of endo in ALEC
*/
static int L_count_endo(const std::vector<ATOMIC_LEC>& v_al, char* endo)
{
    int count = 0;
    for(const ATOMIC_LEC& al : v_al)
    {
        // test that al is of type ALEC_VAR
        if(std::holds_alternative<ALEC_VAR>(al)) 
        {
            const ALEC_VAR& al_var = std::get<ALEC_VAR>(al);

            if(al_var.type == L_VAR && al_var.per.step == 0 && al_var.lag == 0 &&
                strcmp(L_NAMES[al_var.pos], endo) == 0)
                    count++;
        }
    }

    return count;
}

/**
 * If sl->sl_expr[sl->sl_side_with_endo] is not trivial, splits sl->sl_expr[sl->sl_side_with_endo] into two parts and replaces sl->sl_expr[sl->sl_side_with_endo] and sl->sl_expr[sl->sl_side_without_endo] by these 2 expressions.
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
    int type;
    int split_pos;

    // vector of ATOMIC LEC from the equation side containing the endogenous variable
    std::vector<ATOMIC_LEC> v_al;
    if(sl->sl_side_with_endo == EQ_LHS) 
        v_al = sl->sl_left_expr;
    else 
        v_al = sl->sl_right_expr;

ag:
    const ATOMIC_LEC& al = v_al.back();   // last element of the equation side expression

    // get the common part of all ALEC types (type) to test the type of the last element
    type = get_alec_type(al);
    sl->sl_op = type;

    if(std::holds_alternative<ALEC_FUNC>(al)) 
    {
        const ALEC_FUNC& al_func = std::get<ALEC_FUNC>(al);
        sl->sl_nargs = al_func.nb_args;
    }
    else
        sl->sl_nargs = 0;

    // if the last element is L_VAR, we have only one item and the process is terminated
    if(sl->sl_op == L_VAR)                    
        return 1; 
    
    // if the last element is L_CLOSEP, the first on must be L_OPENP
    // -> we drop the first and last elements of v_al and continue to parse v_al                         
    if(sl->sl_op == L_CLOSEP)
    {
        v_al.erase(v_al.begin());   // drop the first element (L_OPENP)
        v_al.pop_back();            // drop the last element (L_CLOSEP)
        goto ag;                    // we try the next element                       
    }

    // Search the beginning of the sub expression
    split_pos = L_sub_expr(v_al, (int) v_al.size() - 1);
    if(split_pos < 0)
    {
        L_errno = L_DUP_ERR;
        return L_errno; 
    }
    
    // We move the elements BEFORE the sub expression to sl->sl_left_expr
    sl->sl_left_expr.clear();
    for(int i = 0; i < split_pos; i++)
        sl->sl_left_expr.push_back(v_al[i]);
    // and the elements AFTER the sub expression to sl->sl_right_expr
    sl->sl_right_expr.clear();
    for(int i = split_pos + 1; i < v_al.size(); i++)
        sl->sl_right_expr.push_back(v_al[i]);

    // find the side where the endogenous variable is present
    if(L_count_endo(sl->sl_left_expr, sl->sl_endo) > 0) 
    {       
        sl->sl_side_with_endo = EQ_LHS;                                   
        sl->sl_side_without_endo = EQ_RHS;                               
    }
    else 
    {
        sl->sl_side_with_endo = EQ_RHS;
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
 * @param [in] nb_names      int     number of names in L_NAMES before compilation
 * @return                   ALEC*   allocated table of ALEC's or NULL on error
*/
static std::vector<ATOMIC_LEC> L_cc1_alloc(char* lec, int nb_names)
{
    std::vector<ATOMIC_LEC> expression;

    if(L_open_string(lec)) 
        return expression;
    
    if(L_cc1(nb_names) != 0) 
        return expression;
    
    L_close();

    // move the content of L_EXPR to expression
    std::move(L_EXPR.begin(), L_EXPR.end(), std::back_inserter(expression));
    return expression;
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
    // store the original equation as a string (for debugging purposes)
    sl->sl_lec = std::string(eq);

    /* SLIT EQ AND COMPILE EACH MEMBER */
    int split_pos = L_split_eq(eq);
    if(split_pos < 0)
    {
        L_errno = L_ASSIGN_ERR;
        return L_errno;
    }

    // Compiles left member
    eq[split_pos] = 0;        // close the left member
    sl->sl_left_expr = L_cc1_alloc(eq, 0);
    eq[split_pos] = ':';      // Reset the original text
    if(sl->sl_left_expr.size() == 0) 
        return L_errno;

    // Compiles the right member
    sl->sl_right_expr = L_cc1_alloc(eq + split_pos + 2, L_NB_NAMES);
    if(sl->sl_right_expr.size() == 0) 
    {
        sl->sl_left_expr.clear();
        return L_errno;
    }

    return 0;
}


/**
 * Appends to L_EXPR the member mbr (LHS or RHS) contained in sl. 
 * 
 * @param [in] sl   SLEC*   
 * @param [in] mbr  int     position of the member to append 
*/
static void L_append(SLEC* sl, const EQ_HAND_SIDE mbr)
{
    if(mbr == EQ_LHS)
    {
        for(const ATOMIC_LEC& al : sl->sl_left_expr)
            L_EXPR.push_back(al);   
    }
    else
    {
        for(const ATOMIC_LEC& al : sl->sl_right_expr)
            L_EXPR.push_back(al);   
    }
}


/**
 * Add at the beginning of L_EXPR the member mbr of the SLEC struct sl.
 * 
 * @param [in] sl   SLEC*   container of the compiled equation
 * @param [in] mbr  int     member of sl to copy to L_EXPR
*/
static void L_front(SLEC* sl, const EQ_HAND_SIDE mbr)
{
    if(mbr == EQ_LHS)
    {
        // L_EXPR = {sl->sl_left_expr, L_EXPR} (i.e. we add the member at the beginning of L_EXPR)
        for(auto it = sl->sl_left_expr.rbegin(); it != sl->sl_left_expr.rend(); ++it)
            L_EXPR.insert(L_EXPR.begin(), *it);   
    }
    else
    {
        // L_EXPR = {sl->sl_right_expr, L_EXPR} (i.e. we add the member at the beginning of L_EXPR)
        for(auto it = sl->sl_right_expr.rbegin(); it != sl->sl_right_expr.rend(); ++it)
            L_EXPR.insert(L_EXPR.begin(), *it);   
    }
}


/**
 * Appends an operator to L_EXPR.
 * 
 * @param [in]  op  int     operator (defined in iode.h)
*/
static void L_append_op(const int op)
{
    L_EXPR.push_back(ALEC_OPERATOR(op));
}

/**
 * Appends a function and its number of arguments to L_EXPR.
 *
 * @param [in] func   int   function id
 * @param [in] nargs  int   number of arguments of the function
*/
static void L_append_fn(const int func, const int nargs)
{
    L_EXPR.push_back(ALEC_FUNC(func, nargs));
}

/**
 * Appends a long constant to L_EXPR.
 * 
 * @param [in] a    int     value of the constant
*/
static void L_append_const(const long value)
{
    L_EXPR.push_back(ALEC_CONST_LONG(value));
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
    SLEC slec;
    SLEC* sl = &slec;

    // Compiles the 2 members of eq and put the result in slec
    if(L_cc1_eq(sl, eq)) 
        return L_errno;

    /* FIND MEMBER CONTAINING ENDO AND SET INFO IN sl */
    *duplicated_endo = 0;
    sl->sl_endo = endo;
    int count_left = L_count_endo(sl->sl_left_expr, sl->sl_endo);
    int count_right = L_count_endo(sl->sl_right_expr, sl->sl_endo);

    // endogenous variable not found in the equation
    if(count_left + count_right == 0) 
    {
        sl->sl_left_expr.clear();
        sl->sl_right_expr.clear();
        L_errno = L_DUP_ERR;
        return L_errno;
    }

    // if endo is present more than once in the equation, we cannot solve it. 
    // We transform the equation in "0 := LHS - RHS" and set duplicated_endo to 1.
    if(count_left + count_right >= 2) 
    {
        // Result = {sl->sl_left_expr, sl->sl_right_expr, L_MINUS, L_EOE} 
        // i.e. F(x) = LHS - RHS 
        *duplicated_endo = 1;
        L_append(sl, EQ_LHS);    
        L_append(sl, EQ_RHS);
        L_append_op(L_MINUS);
        L_append_op(L_EOE);
        sl->sl_left_expr.clear();
        sl->sl_right_expr.clear();
        return 0;
    }

    // if endo not present in LHS
    if(count_left == 0) 
    {
        sl->sl_side_with_endo = EQ_RHS;
        sl->sl_side_without_endo = EQ_LHS;
    }
    // if endo not present in RHS
    else 
    {
        sl->sl_side_with_endo = EQ_LHS;
        sl->sl_side_without_endo = EQ_RHS;
    }

    // Create an empty L_EXPR and move the member nmbr into L_EXPR
    L_EXPR.clear();
    L_append(sl, sl->sl_side_without_endo);
    if(sl->sl_side_without_endo == EQ_LHS) 
        sl->sl_left_expr.clear();
    else
        sl->sl_right_expr.clear();

    // invert the expression containing endo
    /* LOOP ON OPERATORS */
    while(L_split_expr(sl) == 0)
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
                if(sl->sl_side_with_endo == 1) 
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
                if(sl->sl_side_with_endo == 0) 
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
                if(sl->sl_nargs == 2) 
                    L_append(sl, sl->sl_side_without_endo);
                L_append(sl, sl->sl_side_with_endo);
                L_append_fn(L_LAG, sl->sl_nargs);
                L_append_op(L_PLUS);
                break;
            case L_DLN    :
                if(sl->sl_nargs == 2) 
                    L_append(sl, sl->sl_side_without_endo);
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
                if(sl->sl_nargs == 2) 
                    L_append(sl, sl->sl_side_without_endo);
                L_append(sl, sl->sl_side_with_endo);
                L_append_fn(L_LAG, sl->sl_nargs);
                L_append_op(L_TIMES);
                break;
            case L_RAPP   :
                if(sl->sl_nargs == 2) 
                    L_append(sl, sl->sl_side_without_endo);
                L_append(sl, sl->sl_side_with_endo);
                L_append_fn(L_LAG, sl->sl_nargs);
                L_append_op(L_TIMES);
                break;
            default :
            {
                if(sl->sl_side_with_endo == EQ_LHS) 
                    sl->sl_left_expr.clear();
                else 
                    sl->sl_right_expr.clear();
                L_errno = L_INVERT_ERR;
                return L_errno;
            }
        }

    L_append_op(L_EOE);

    if(sl->sl_side_with_endo == EQ_LHS) 
        sl->sl_left_expr.clear();
    else 
        sl->sl_right_expr.clear();
    
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
            return i;
    }

    return -1;
}

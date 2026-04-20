/**
 *  @header4iode
 *  
 *  First step of LEC compilation. 
 *  
 *  During this stage, 2 tables, one with atomic expressions (ALEC *L_EXPR) and 
 *  one with names found in the expression (char** L_NAMES) will be
 *  filled. L_EXPR is filled in the order in which the execution must take place. 
 *  
 *  The 2 tables are reallocated by blocks according to the size of the LEC expression. 
 *  
 *  The second step is described in l_cc2.c and generates a "CLEC expression", 
 *  which is basically the serialization of the combination of L_EXPR and L_NAMES.
 *  
 *  Main functions
 * 
 *      - int L_cc1(int nb_names)           First step of LEC compilation: creates 
 *                                              - L_EXPR  = ordered list of atomic expressions with references to L_NAMES
 *                                              - L_NAMES = list of names in the LEC expression
 *      - void L_alloc_expr(int nb)         Allocates or reallocates L_EXPR by blocks of 100 elements.
 *      - int L_sub_expr(const ATOMIC_LEC& al, const int i)   Computes the position of the beginning of a sub-expression
 *  
 */

#include "api/lec/lec.h"

#define last_ls         (L_OPS[L_NB_OPS - 1])   // Last element on the stack
#define last_op         (last_ls.ls_op)         // Operator of the last element on the stack

// Global variables
LSTACK  L_OPS[L_MAX_STACK]; // Stack of operators encountered during compilation process. Limited to a max depth of L_MAX_STACK.
int     L_NB_OPS = 0,       // Current operator stack depth
        L_PAR = 0;          // Current parenthesis depth

// TODO: move LSTACK here + L_OPS... static


/**
 *  Adds a series or scalar name in L_NAMES.
 *  
 *  If necessary, reallocates L_NAMES by blocks of 10 elements at a time.
 *  
 *  @param [in]     name    char*   VAR of Scalar name
 *  @return                 int     position of name in L_NAMES
 *  
 */
static int L_add_coef_or_var_name(char* name)
{
    int i;
    for(i = 0 ; i < L_NB_NAMES ; i++)
    {
        if(strcmp(name, L_NAMES[i]) == 0) 
            return i;
    }

    if(L_NB_NAMES >= L_NB_ANAMES) 
    {
        L_NAMES = (char **) SCR_realloc(L_NAMES, sizeof(char *),
                                        L_NB_ANAMES, L_NB_ANAMES + 10);
        for(int j = 0 ; j < 10 ; j++)
            L_NAMES[L_NB_ANAMES + j] = SCR_malloc(L_MAX_NAME + 1);

        L_NB_ANAMES += 10;
    }

    strcpy(L_NAMES[i], name);
    L_NB_NAMES++;
    return i;
}


/**
 *  Adds the last read token (in L_TOKEN) in L_EXPR. That token can be a VAR name +
 *  but also a Scalar, a Period or a numerical constant.
 *  
 *  The token is saved in the union al_val (see iode.h).
 *  
 *  @return     int     0 TODO: check if L_alloc_expr() succeeded
 *  
 */
static int L_save_var()
{
    int type = L_TOKEN.tk_def;
    switch(type) 
    {
        case L_PERIOD :     // Period
        {
            Period period(L_TOKEN.tk_period.year, L_TOKEN.tk_period.periodicity, 
                          L_TOKEN.tk_period.step);
            ALEC_PERIOD al(period);
            L_EXPR.push_back(al);
            break;
        }
        case L_DCONST:      // constant double
        {
            ALEC_CONST_REAL al(L_TOKEN.tk_real);
            L_EXPR.push_back(al);
            break;
        }
        case L_LCONST:      // constant long
        {
            ALEC_CONST_LONG al(L_TOKEN.tk_long);
            L_EXPR.push_back(al);
            break;
        }
        default :           // scalar or variable
        {
            int pos = L_add_coef_or_var_name(L_TOKEN.tk_name);
            // if represents a value -> coefficient
            if(is_val(type))
            {
                ALEC_COEF al(pos);
                L_EXPR.push_back(al);
            }
            // else it is a variable
            {
                short lag = 0;
                ALEC_VAR al(type, pos, lag, Period());
                L_EXPR.push_back(al);
            }
            break;
        }
    }

    return 0;
}


/**
 *  Check if the operator op has a lower execution priority than the last operator on the stack L_OPS;
 *  
 *  @param [in]     op  int     operator to compare with last_op?
 *  @return             int     1 if last entry in L_OPS is not an operator (parenthesis for ex.)
 *                              1 if priority(op) <= priority(last_op) 
 *                              0 if priority(op) > priority(last_op) 
 *                              0 if there is no op on the stack or if the last op is an open parenthesis
 */
static int L_priority_sup(int op)
{
    if(L_NB_OPS <= 0) 
        return 0;
    
    if(last_op == L_OPENP) 
        return 0;
    
    if(!is_op(last_op)) 
        return 1;
    
    if(L_PRIOR[op - L_OP] <= L_PRIOR[last_op - L_OP]) 
        return 1;
    
    return 0;
}


/**
 *  Adds the last "operator" on top of L_OPS to L_EXPR, as well as the number of parameters (in last_ls.ls_nb_args). 
 *  Checks if the number of arguments are in line with the definitions.
 *  
 *  The operator is saved in type. 
 *  The nb_args is saved in the union al_val (see iode.h).
 *  
 *  @return     int     0 on success
 *                      L_ARGS_ERR if the number of args does not follow the syntax (L_MAX_* and L_MIN_*).
 *  
 *  TODO: check if L_alloc_expr() succeeded
 */
static int L_save_op()
{
    int pos;
    int op = last_op;
    int type = op;
    int nb_args = (int) last_ls.ls_nb_args;

    if(is_fn(op)) 
    {
        pos = op - L_FN;
        if(L_MAX_FARGS[pos] < nb_args || L_MIN_FARGS[pos] > nb_args)
        {
            L_errno = L_ARGS_ERR;
            return L_errno;
        }

        ALEC_FUNC al(type, nb_args);
        L_EXPR.push_back(al);
        return 0;
    }

    if(is_tfn(op)) 
    {
        pos = op - L_TFN;
        if(L_MAX_TARGS[pos] < nb_args || L_MIN_TARGS[pos] > nb_args)
        {
            L_errno = L_ARGS_ERR;
            return L_errno;
        }

        ALEC_FUNC al(type, nb_args);
        L_EXPR.push_back(al);
        return 0;
    }

    if(is_mtfn(op)) 
    {
        pos = op - L_MTFN;
        if(L_MAX_MTARGS[pos] < nb_args || L_MIN_MTARGS[pos] > nb_args)
        {
            L_errno = L_ARGS_ERR;
            return L_errno;
        }

        ALEC_FUNC al(type, nb_args);
        L_EXPR.push_back(al);
        return 0;
    }

    return 0;
}


/**
 *  Adds the current operator (stored in L_TOKEN.tk_def) to L_OPS, the stack of operators. If needed, reallocates L_OPS.
 *  Note that *op_group* does represent a group of operators (L_OP, L_FNS...), not a specific operators. 
 *  The last read *operator* is in L_TOKEN.tk_def.
 *  
 *  First, saves in L_EXPR the operator(s) of lower priorities that are on the top of L_OPS.
 *  
 *  Example: if op is '+' and last_op is '*': 
 *              '*' if moved to L_EXPR because '+' has a lower priority.
 *              '+' is put on the top of L_OPS
 *  
 *  @param [in] op_group  int   group the operator to be added belongs to (L_OP, L_FN, L_TFN, L_MTFN, L_OPENP, COMMA...).
 *                              The operator itself is in L_TOKEN.
 *  @return                     0 on success
 *                              L_errno on error
 */
static int L_add_stack(int op_group)
{
    if(L_NB_OPS >= L_MAX_STACK)
    {
        L_errno = L_STACK_ERR;
        return L_errno;
    } 

    switch(op_group) 
    {
        case L_OP :
            while(L_priority_sup(L_TOKEN.tk_def))
            {
                if(L_save_op() != 0) 
                    return L_errno;
            }
            // NO BREAK!
        case L_FN :
        case L_TFN :
        case L_MTFN :
            L_OPS[L_NB_OPS].ls_nb_args = 1;
            L_OPS[L_NB_OPS].ls_op = L_TOKEN.tk_def;
            break;
        case L_OPENP :
            L_PAR++;
            L_OPS[L_NB_OPS++].ls_op = L_OPENP;
            if(L_save_op() != 0) 
                return L_errno;
            L_OPS[L_NB_OPS].ls_op = L_OPENP;
            break;
        case L_CLOSEP :
            L_PAR--;
            while(L_NB_OPS > 0) 
            {
                if(last_op == L_OPENP) 
                {
                    last_op = L_CLOSEP;
                    return L_save_op();
                }
                if(L_save_op() != 0) 
                    return L_errno;
            }
            L_errno = L_PAR_ERR;
            return L_errno;
        case L_COMMA :
            if(L_add_stack(L_CLOSEP)) 
                return L_errno;
            if(L_NB_OPS <= 0 || !(is_fn(last_op) || is_tfn(last_op) || is_mtfn(last_op)))
            {
                L_errno = L_SYNTAX_ERR;
                return L_errno;
            }
            last_ls.ls_nb_args++;
            return L_add_stack(L_OPENP);
        case L_OCPAR :
            if(L_NB_OPS <= 0 || !is_fn(last_op))
            {
                L_errno = L_SYNTAX_ERR;
                return L_errno;
            }
            last_ls.ls_nb_args = 0;
            return 0;
        default :
            break;
    }

    L_NB_OPS++;
    return 0;
}


/**
 *  Empties the stack of operators L_OPS by adding all operators and the 
 *  number of their arguments to L_EXPR.
 *  
 *  @return     int 0 on success
 *                  L_errno on error
 */
static int L_empty_ops_stack()
{
    while(L_NB_OPS > 0)
    {
        if(L_save_op() != 0) 
            return L_errno;
    }
    
    ALEC_OPERATOR al(L_EOE);
    L_EXPR.push_back(al);
    return 0;
}


/**
 *  Applies a lag on each variable in the last sub expression. 
 *  The last expression on L_EXPR is either an atomic expression (e.g. "A" or "A[1960Y1]") 
 *  or an expression between parentheses (e.g. "(A+B+2)").
 *  
 *  Ex. 
 *       (A1+A2[-1])[-2]             == A1[-2] + A2[-3]
 *       A1[1965Y1][-2]              == A1[1965Y1] 
 *  
 *  @param [in]  lag int    lag to add to each var (for ex when treating an sub- sub- expression).
 *  @return          int    0 on success, L_errno if the sub expression cannot be identified          
 */
static int L_lag_expr(int lag)
{
    int start_pos = L_sub_expr(L_EXPR, (int) L_EXPR.size() - 1);
    if(start_pos < 0) 
        return L_errno;

    int type;
    for(int pos = start_pos; pos < L_EXPR.size(); pos++) 
    {
        ATOMIC_LEC al = L_EXPR[pos];
        type = get_alec_type(al);

        if(type != L_VAR) 
            continue;

        ALEC_VAR al_var = std::get<ALEC_VAR>(al);
        if(al_var.per.step != 0) 
            continue;
        
        al_var.lag += lag;
        L_EXPR[start_pos] = al_var;
    }

    return 0;
}


/**
 *  Applies a time expression (for ex. "1960Y1") on each variable in the last sub expression. 
 *  The last expression on L_EXPR is either an atomic expression (e.g. "A" or "A[1960Y1]") or 
 *  an expression between parentheses (e.g. "(A+B+2)").
 *  
 *  Ex. 
 *       (A1+A2[1965Y1])[1962Y1]     == A1[1962Y1] + A2[1965Y1]
 *       (A1[-2]+1)[1965Y1]          == A1[1963Y1]+1
 *  
 *  @return          int    0 on success, L_errno if the sub expression cannot be identified          
 */
static int L_time_expr()
{
    int start_pos = L_sub_expr(L_EXPR, (int) L_EXPR.size() - 1);
    if(start_pos < 0) 
        return L_errno;
    
    int type;
    for(int pos = start_pos; pos < L_EXPR.size(); pos++) 
    {
        ATOMIC_LEC al = L_EXPR[pos];
        type = get_alec_type(al);

        if(type != L_VAR) 
            continue;
        
        ALEC_VAR al_var = std::get<ALEC_VAR>(al);
        if(al_var.per.step != 0) 
            continue;

        al_var.per = L_TOKEN.tk_period;
        L_EXPR[start_pos] = al_var;
    }

    return 0;
}


/**
 *  Analyses a lag expression between [], like in A[2021Y1] or VAR[-2]. 
 *  Two expressions are accepted:
 *      - a fixed period like in A[2001M3]
 *      - a lag or a lead like in A[-1]
 *  
 *  The expression preceding the lag is modified by the function. 
 *  The variables that are fixed in the time (A[2000Y1] for ex.) remain untouched, like a scalar.
 *  Other variables, including those already lagged, are "moved" accordingly.
 *  
 *  Ex.
 *      (A1[2000Y1] + A2[-1] + A3)[-2] => (A[2000Y1] + B[-3] + C[-2])
 *        
 *  @return     int     0 on success
 *                      L_LAG_ERR on error
 *  
 */
static int L_anal_lag()
{
    int type = L_get_token();
    switch(type) 
    {
        // case 1: lag or lead like in A[-2]
        case L_OP :
        {
            // check that the operator is a '+' or a '-'
            int op = L_TOKEN.tk_def;
            if(op != L_MINUS && op != L_PLUS)
            {
                L_errno = L_LAG_ERR;
                return L_errno;
            }

            // check that the next token is a long constant (the lag)
            if(L_get_token() != L_LCONST)
            {
                L_errno = L_LAG_ERR;
                return L_errno;
            }
            
            // apply the lag
            int lag = LYYLONG;
            if(op == L_MINUS) 
                lag = -lag;
            L_lag_expr(lag);
            break;
        }  
        // case 2: time expression like in A[2000Y1]
        case L_PERIOD :
            // apply the time expression
            L_time_expr();
            break;
        // default: error
        default :
            L_errno = L_LAG_ERR;
            return L_errno;
    }

    // return the error if any
    if(L_errno != 0) 
        return L_errno;
    
    // check that the lag expression is closed by a ']'
    type = L_get_token();
    if(type == L_CLOSEB) 
        return 0;

    L_errno = L_LAG_ERR;
    return L_errno;
}


/**
 *  First step of LEC compilation. L_YY (see l_token.c) is the open stream containing the analysed LEC expression.
 *  
 *  At the end of this function, 2 tables are created: L_EXPR and L_NAMES. They are the input of L_cc2() which will 
 *  serialize L_EXPR into a CLEC (Compiled LEC) structure.
 *      - L_EXPR contains atomic expressions in the execution order including references to L_NAMES 
 *      - L_NAMES contains the names included in the lec expression
 *  
 *  @param [in] nb_names    int number of variable and/or scalar names already in L_NAMES. 
 *                              Normally 0 but can be > 0 for equations
 *  
 *  @return                 int error code: 0 on success or L_PAR_ERR, L_SYNTAX_ERR...
 */
int L_cc1(int nb_names)
{
    int type;
    int start = 1;
    int beg = 1;    /* indicate if next token is an oper or an expr */

    L_NB_OPS = L_PAR = L_errno = 0;
    L_NB_NAMES = nb_names;

    /* LOOP ON TOKEN */
    while(true) 
    {
        type = L_get_token();       // Group of operators, not the operator itself
        if(L_errno) 
            return L_errno;
        
again:
        switch(type) 
        {
            case L_PERIOD:      // period
            case L_LCONST :     // long constant
            case L_DCONST :     // double constant
            case L_VAR :        // variable
            case L_COEF :       // coefficient (scalar)
            case L_VAL :        // value function (pi, e, time...)
                if(beg == 0) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(L_save_var()) 
                    return L_errno;
                beg = 0;
                break;
            case L_OP :         // Operator
                if(beg != 0) 
                {
                    switch(L_TOKEN.tk_def) 
                    {
                        case L_MINUS :
                            L_TOKEN.tk_def = L_UMINUS;
                            break;
                        case L_PLUS :
                            L_TOKEN.tk_def = L_UPLUS;
                            break;
                        default :
                        {
                            L_errno = L_SYNTAX_ERR;
                            return L_errno;
                        }
                    }
                    type = L_FN;
                    goto again;
                }
                beg = 1;
                if(L_add_stack(keyw)) 
                    return L_errno;
                break;
            case L_FN :         // not time function
            case L_TFN:         // time function
            case L_MTFN:        // multi-args time function
            case L_OPENP :      // open parenthesis
                if(beg == 0) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(L_add_stack(type)) 
                    return L_errno;
                break;
            case L_OCPAR :      // ??
                if(beg == 0) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(L_add_stack(type)) 
                    return L_errno;
                beg = 0;
                break;
            case L_CLOSEP :     // Close parenthesis
                if(beg == 1) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(L_add_stack(type)) 
                    return L_errno;
                break;
            case L_COMMA :      // Comma
                if(beg == 1) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(L_add_stack(type)) 
                    return L_errno;
                beg = 1;
                break;
            case L_OPENB:
                if(beg == 1) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(L_anal_lag()) 
                    return L_errno;
                beg = 0;
                break;
            case YY_EOF:
            case L_EOE :        // End of expression
                if(start) 
                {
                    L_EXPR.clear();
                    L_EXPR.push_back(ALEC_OPERATOR(L_EOE));
                    return L_errno;
                }
                if(beg == 1) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(L_PAR != 0) 
                    L_errno = L_PAR_ERR;
                else 
                    L_empty_ops_stack();
                return L_errno;
            case YY_ERROR :
                return L_errno;
            default :
            {
                L_errno = L_SYNTAX_ERR;
                return L_errno;
            }
        }
        start = 0;
    }
}


/**
 *  Resets (frees) L_NAMES.
 */
void L_free_anames()
{
    int     j;


    for(j = 0 ; j < L_NB_ANAMES ; j++)
        SCR_free(L_NAMES[j]);

    SCR_free(L_NAMES);
    L_NB_ANAMES = 0;
    L_NAMES = 0;
}




/**
 *  Computes the position of the beginning of the sub-expression starting at expr[i] (expr comes from L_EXPR).
 *  Browses backwards all elements of the expression until having reached a level 0 of parentheses or 
 *  all arguments of an operator or function.
 *    
 *  @param [in] expr        ALEC*   pointer to an element of L_EXPR
 *  @param [in] close_pos   int     position in expr of the element where the expression is "closed"
 *  @return                         position in expr of the element where the expression starts
 */
int L_sub_expr(const std::vector<ATOMIC_LEC>& expr, const int close_pos)
{
    int type;
    int nb_parents = 0;

    // browse backwards all elements of the expression
    for(int pos = close_pos; pos >= 0; pos--) 
    {
        ATOMIC_LEC& al = expr[pos];
        type = get_alec_type(al);

        // if open or close parenthesis, update the number of parents
        switch(type) 
        {
            // open parenthesis -> decrease number of parents
            case L_OPENP  :
                nb_parents--;
                break;
            // close parenthesis -> increase number of parents
            case L_CLOSEP :
                nb_parents++;
                break;
            default :
                break;
        }

        if(nb_parents > 0) 
            continue;
        
        // QUESTION: why calling L_sub_expr twice ?
        if(is_op(type)) 
        {
            pos = L_sub_expr(expr, pos - 1);
            pos = L_sub_expr(expr, pos - 1);
            return pos;
        }

        // if not a function, we have reached the beginning of the expression
        if(!is_fn(type) && !is_tfn(type) && !is_mtfn(type)) 
            return pos;
    }

    L_errno = L_LAG_ERR;
    return -1;
}

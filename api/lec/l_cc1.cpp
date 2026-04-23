/**
 *  @header4iode
 *  
 *  First step of LEC compilation. 
 *  
 *  During this stage, 2 tables, one with atomic expressions (ALEC *L_EXPR) and 
 *  one with names found in the expression (L_NAMES) will be filled. 
 *  L_EXPR is filled in the order in which the execution must take place. 
 *  
 *  The 2 tables are reallocated by blocks according to the size of the LEC expression. 
 *  
 *  The second step is described in l_cc2.c and generates a "CLEC expression", 
 *  which is basically the serialization of the combination of L_EXPR and L_NAMES.
 *  
 *  Main functions
 * 
 *      - int L_cc1()               First step of LEC compilation: creates 
 *                                              - L_EXPR  = ordered list of atomic expressions with references to L_NAMES
 *                                              - L_NAMES = list of names in the LEC expression
 *      - int L_sub_expr(const std::vector<ALEC>& v_alec, int close)   Computes the position of the beginning of a sub-expression
 */

#include "api/lec/lec.h"

#define last_ls         (L_OPS[L_NB_OPS - 1])   // Last element on the stack
#define last_op         (last_ls.ls_op)         // Operator of the last element on the stack

// Global variables
LSTACK  L_OPS[L_MAX_STACK]; // Stack of operators encountered during compilation process. Limited to a max depth of L_MAX_STACK.
int     L_NB_OPS = 0,       // Current operator stack depth
        L_PAR = 0;          // Current parenthesis depth


/**
 *  Adds a series or scalar name in L_NAMES.
 *  
 *  @param [in]     name    char*   VAR of Scalar name
 *  @return                 int     position of name in L_NAMES
 *  
 */
static int L_add_coef_or_var_name(const std::string& name)
{
    // Check if name already exists
    for(int i = 0; i < (int) L_NAMES.size(); i++)
    {
        if(L_NAMES[i] == name)
            return i;
    }

    // Add new name to the vector
    L_NAMES.push_back(name);
    return (int) L_NAMES.size() - 1;
}

static int L_save_var()
{
    ALEC al;
    al.type = L_TOKEN.tk_def;

    switch(L_TOKEN.tk_def) 
    {
        case L_PERIOD :     // period
            al.content.period.year = L_TOKEN.tk_period.year;
            al.content.period.periodicity = L_TOKEN.tk_period.periodicity;
            al.content.period.step = L_TOKEN.tk_period.step;
            break;
        case L_DCONST:      // double constant
            al.content.const_float = L_TOKEN.tk_real;
            break;
        case L_LCONST:      // long constant
            al.content.const_long = L_TOKEN.tk_long;
            break;
        default :
            // if special constant (pi, e, etc.) -> nothing to do
            if(is_val(L_TOKEN.tk_def)) 
                break;
            // Variable or Scalar
            al.content.variable.pos = L_add_coef_or_var_name(L_TOKEN.tk_name);
            al.content.variable.lag  = 0;
            al.content.variable.per.year = 0;
            al.content.variable.per.periodicity= 0;
            al.content.variable.per.step = 0;
            break;
    }

    L_EXPR.push_back(al);
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
static bool L_priority_sup(int op)
{
    if(L_NB_OPS <= 0) 
        return false;
    
    if(last_op == L_OPENP) 
        return false;
    
    if(!is_op(last_op)) 
        return true;
    
    if(L_PRIOR[op - L_OP] <= L_PRIOR[last_op - L_OP]) 
        return true;
    
    return false;
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
 */
static int L_save_op()
{
    int pos;
    int op = last_op;
    int nb_args = (int) last_ls.ls_nb_args;

    if(is_fn(op)) 
    {
        pos = op - L_FN;
        if(L_MAX_FARGS[pos] < nb_args || L_MIN_FARGS[pos] > nb_args)
        {            
            L_errno = L_ARGS_ERR;
            return L_errno;
        }
    }

    if(is_tfn(op)) 
    {
        pos = op - L_TFN;
        if(L_MAX_TARGS[pos] < nb_args || L_MIN_TARGS[pos] > nb_args)
        {            
            L_errno = L_ARGS_ERR;
            return L_errno;
        }
    }

    if(is_mtfn(op)) 
    {
        pos = op - L_MTFN;
        if(L_MAX_MTARGS[pos] < nb_args || L_MIN_MTARGS[pos] > nb_args)
        {            
            L_errno = L_ARGS_ERR;
            return L_errno;
        }
    }

    ALEC al;
    al.type = op;
    al.content.func_nb_args = nb_args;
    L_EXPR.push_back(al);

    L_NB_OPS--;
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

    ALEC al;
    al.type = L_EOE;
    L_EXPR.push_back(al);

    return 0;
}


/**
 *  Applies a lag on each variable in the last sub expression. 
 *  The last expression on L_EXPR is either an atomic expression (e.g. "A" or "A[1960Y1]") 
 *  or an expression between parentheses (e.g. "(A + B + 2)").
 *  
 *  Ex. 
 *       (A1 + A2[-1])[-2]           == A1[-2] + A2[-3]
 *       A1[1965Y1][-2]              == A1[1965Y1] 
 *  
 *  @param [in]  lag int    lag to add to each var (for ex when treating an sub- sub- expression).
 *  @return          int    0 on success, L_errno if the sub expression cannot be identified          
 */
static int L_lag_expr(int lag)
{
    int start_sub_expr = L_sub_expr(L_EXPR);
    if(start_sub_expr < 0) 
        return L_errno;

    for(auto it = L_EXPR.begin() + start_sub_expr; it != L_EXPR.end(); it++)
    {
        ALEC& al = *it;

        // only applies lag to non-timed variables. 
        if(al.type != L_VAR) 
            continue;

        // skip if a period is associated to the variable (ex. A[1960Y1]) because 
        // in that case, the variable is fixed in time and cannot be lagged.
        if(al.content.variable.per.step != 0) 
            continue;
        
        // applies lag
        al.content.variable.lag += lag;
    }

    return 0;
}


/**
 *  Applies a time expression (for ex. "1960Y1") on each variable in the last sub expression. 
 *  The last expression on L_EXPR is either an atomic expression (e.g. "A" or "A[1960Y1]") or 
 *  an expression between parentheses (e.g. "(A + B + 2)").
 *  
 *  Ex. 
 *       (A1 + A2[1965Y1])[1962Y1]     == A1[1962Y1] + A2[1965Y1]
 *       (A1[-2] + 1)[1965Y1]          == A1[1963Y1] + 1
 *  
 *  @return          int    0 on success, L_errno if the sub expression cannot be identified          
 */
static int L_time_expr()
{
    int start_sub_expr = L_sub_expr(L_EXPR);
    if(start_sub_expr < 0) 
        return L_errno;

    for(auto it = L_EXPR.begin() + start_sub_expr; it != L_EXPR.end(); it++)
    {
        ALEC& al = *it;

        // only applies lag to non-timed variables.
        if(al.type != L_VAR) 
            continue;

        // skip if a period is associated to the variable (ex. A[1960Y1]) because 
        // in that case, the variable is fixed in time and cannot be lagged.
        if(al.content.variable.per.step != 0) 
            continue;
        
        // applies time expression
        memcpy(&(al.content.variable.per), &(L_TOKEN.tk_period), sizeof(Period));
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
 *  Other variables, including thoses already lagged, are "moved" accordingly.
 *  
 *  Ex.
 *      (A1[2000Y1] + A2[-1] + A3)[-2] => (A[2000Y1] + B[-3] + C[-2])
 *        
 *  @return     int     0 on success
 *                      L_LAG_ERR on error
 *  
 */
static int L_analyze_lag()
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
            int lag = L_YY->yy_long;
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
 *  First step of LEC compilation. L_YY (see l_token.c) is the open stream containing the analyzed LEC expression.
 *  
 *  At the end of this function, 2 tables are created: L_EXPR and L_NAMES. 
 *  They are the input of L_cc2() which will serialize L_EXPR into a CLEC (Compiled LEC) structure.
 *      - L_EXPR contains atomic expressions in the execution order including references to L_NAMES 
 *      - L_NAMES contains the names included in the lec expression
 *  
 *  @return int     error code: 0 on success or L_PAR_ERR, L_SYNTAX_ERR...
 */
int L_cc1()
{
    int type;
    int start = 1;
    int beg = 1;    /* indicate if next token is an oper or an expr */

    L_EXPR.clear();
    L_NB_OPS = L_PAR = L_errno = 0;

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
                if(L_add_stack(type)) 
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
            case L_OCPAR :      // open-close parentheses "()""
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
                if(L_analyze_lag()) 
                    return L_errno;
                beg = 0;
                break;
            case YY_EOF:
            case L_EOE :        // End of expression
                if(start) 
                {
                    ALEC al;
                    al.type = L_EOE;
                    L_EXPR.push_back(al);

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
 *  Computes the position of the beginning of the sub-expression starting at ALEC al + i (al comes from L_EXPR).
 *  Browses backwards all elements of the expression until having reached a level 0 of parentheses or 
 *  all arguments of an operator or function.
 *    
 *  @param [in] al  ALEC*   pointer to an element of L_EXPR
 *  @param [in] i   int     position in al of the element where the expression is "closed"
 *  @return                 position in al of the element where the expression starts
 */
int L_sub_expr(const std::vector<ALEC>& v_alec, int close)
{
    if(v_alec.empty())
        return -1;

    if(close < -1 || close >= (int) v_alec.size())
    {
        std::string error_msg = "Invalid position for close in L_sub_expr: " + std::to_string(close) + ".\n";
        error_msg += "Valid values are between -1 and " + std::to_string(v_alec.size() - 1) + ".";
        throw std::out_of_range(error_msg);
    }

    int type;
    int nb_parents = 0;
    int end = (close >= 0) ? close : (int) v_alec.size() - 1;

    // browse backwards all elements of the expression
    for(int pos = end; pos >= 0; pos--) 
    {
        type = v_alec[pos].type;

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
            pos = L_sub_expr(v_alec, pos - 1);
            pos = L_sub_expr(v_alec, pos - 1);
            return pos;
        }

        // if not a function, we have reached the beginning of the expression
        if(!is_fn(type) && !is_tfn(type) && !is_mtfn(type)) 
            return pos;
    }

    L_errno = L_LAG_ERR;
    return -1;
}

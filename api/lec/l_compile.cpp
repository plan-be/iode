#include "api/lec/lec.h"


struct LSTACK 
{        
    int type;           // operator or function type 
    int nb_args;        // nb of arguments

public:
    LSTACK(int type, int nb_args=0) : type(type), nb_args(nb_args) {}
};

std::vector<LSTACK> L_OPS;      // Stack of operators and functions


void CLEC::add_coef_or_var_name(const std::string& name)
{
    // check if the name is already in v_objs
    for(const auto& obj_name : v_objs)
    {
        if (obj_name == name)
            return; // name already exists, do not add it again
    }

    v_objs.push_back(name);
    map_objs[name] = -1;
}

int CLEC::save_var(TOKEN& token)
{
    int type = token.type;
    std::string name(token.name);

    bool success = true;
    switch(type) 
    {
        case L_PERIOD :     // period
        {
            // the position of the period in the Variables sample will be set later
            LEC_PERIOD al(token.period, -1);
            L_EXPR.push_back(al);
            break;
        }
        case L_DCONST:      // double constant
        {
            LEC_CONST_REAL al(token.real_value);
            L_EXPR.push_back(al);
            break;
        }
        case L_LCONST:      // long constant
        {
            LEC_CONST_LONG al(token.long_value);
            L_EXPR.push_back(al);
            break;
        }
        case L_COEF:         // coefficient
        {
            add_coef_or_var_name(name);
            LEC_COEF al(name);
            L_EXPR.push_back(al);
            break;
        }
        default :
        {
            // if special constant (pi, e, etc.)
            if(is_val(type))
            {
                LEC_VAL_FN al(type);
                L_EXPR.push_back(al);
                break;
            }

            // IODE Variable
            if(type == L_VAR || type == L_VART)
            {
                add_coef_or_var_name(name);
                LEC_VAR al(type, name, 0, Period());
                L_EXPR.push_back(al);
                break;
            }

            // if the token is not recognized, return a warning and skip it
            success = false;
            std::string msg = "save_var(): Unexpected token type '" + std::to_string(type) + "' in LEC expression. ";
            msg += "This token will be ignored.";
            kwarning(msg.c_str());
            break;
        }
    }

    return success ? 0 : -1;
}


/**
 *  Check if the operator op has a lower execution priority than the last operator on the stack L_OPS;
 *  
 *  @param [in]     op  int     operator to compare with last operator in L_OPS
 *  @return             int     1 if last entry in L_OPS is not an operator (parenthesis for ex.)
 *                              1 if priority(op) <= priority(last op) 
 *                              0 if priority(op) > priority(last op) 
 *                              0 if there is no op on the stack or if the last op is an open parenthesis
 */
bool CLEC::priority_sup(int op)
{
    if(L_OPS.size() <= 0) 
        return false;

    int last_op = L_OPS.back().type;
    
    if(last_op == L_OPENP) 
        return false;
    
    if(!is_op(last_op)) 
        return true;
    
    if(L_PRIOR[op - L_OP] <= L_PRIOR[last_op - L_OP]) 
        return true;
    
    return false;
}


/**
 *  Adds the last "operator" on top of L_OPS to L_EXPR, as well as the number of parameters. 
 *  Checks if the number of arguments are in line with the definitions.
 *  
 *  The operator is saved in type. 
 *  The nb_args is saved in the union al_val (see iode.h).
 *  
 *  @return     int     0 on success
 *                      L_ARGS_ERR if the number of args does not follow the syntax (L_MAX_* and L_MIN_*).
 */
int CLEC::save_op()
{
    int pos = -1;
    LSTACK ls = L_OPS.back();
    L_OPS.pop_back();
    int op = ls.type;
    int nb_args = ls.nb_args;

    if(op == L_OPENP || op == L_CLOSEP)
    {
        LEC_OTHER al(op);
        L_EXPR.push_back(al);
        return 0;
    }

    if(is_fn(op)) 
    {
        pos = op - L_FN;
        if(L_MAX_FARGS[pos] < nb_args || L_MIN_FARGS[pos] > nb_args)
        {            
            L_errno = L_ARGS_ERR;
            return L_errno;
        }

        LEC_FN al(op, nb_args);
        L_EXPR.push_back(al);
        return 0;
    }

    if(is_op(op)) 
    {
        LEC_OP al(op);
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

        LEC_TFN al(op, nb_args);
        L_EXPR.push_back(al);
        return 0;
    }

    if(is_val(op)) 
    {
        LEC_VAL_FN al(op);
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

        LEC_MTFN al(op, nb_args, 0);
        L_EXPR.push_back(al);
        return 0;
    }

    // if the function/operator is not recognized, return a warning and skip it
    std::string msg = "save_op(): Unexpected function or operator '" + std::to_string(op) + "' in LEC expression. ";
    msg += "This function or operator will be ignored.";
    kwarning(msg.c_str());
    return -1;
}


/**
 *  Adds the current operator (stored in token.type) to L_OPS, the stack of operators. If needed, reallocates L_OPS.
 *  Note that *op_group* does represent a group of operators (L_OP, L_FNS...), not a specific operators. 
 *  The last read *operator* is in token.type.
 *  
 *  First, saves in L_EXPR the operator(s) of lower priorities that are on the top of L_OPS.
 *  
 *  Example: if op is '+' and last op is '*': 
 *              '*' if moved to L_EXPR because '+' has a lower priority.
 *              '+' is put on the top of L_OPS
 *  
 *  @param [in] op_group  int   group the operator to be added belongs to (L_OP, L_FN, L_TFN, L_MTFN, L_OPENP, COMMA...).
 *                              The operator itself is in token.
 *  @param [in] func_type int   type of the function (ex. L_SIN, L_COS...).
 *  @param [in] L_PAR     int   current parenthesis depth (used for checking balanced parentheses)
 *  @return                     0 on success
 *                              L_errno on error
 */
int CLEC::add_stack(int op_group, int func_type, int& L_PAR)
{
    if(op_group == L_OP)
    {
        while(priority_sup(func_type))
        {
            if(save_op() != 0) 
                return L_errno;
        }
    }

    switch(op_group) 
    {
        case L_OP :
        case L_FN :
        case L_TFN :
        case L_MTFN :
        {
            LSTACK ls(func_type, 1);
            L_OPS.push_back(ls);
            break;
        }

        case L_OPENP :
        {
            L_PAR++;
            L_OPS.push_back(LSTACK(L_OPENP));
            if(save_op() != 0) 
                return L_errno;
            
            // NOTE: we put the open parenthesis on L_OPS as a marker to be found in case 
            // of a closing parenthesis (i.e. next case) and to know when to stop emptying 
            // the stack of operators
            L_OPS.push_back(LSTACK(L_OPENP));
            break;
        }

        case L_CLOSEP :
        {
            L_PAR--;
            while(L_OPS.size() > 0) 
            {
                LSTACK& last_op = L_OPS.back();
                if(last_op.type == L_OPENP) 
                {
                    last_op.type = L_CLOSEP;
                    return save_op();
                }

                if(save_op() != 0) 
                    return L_errno;
            }

            L_errno = L_PAR_ERR;
            return L_errno;
        }

        case L_COMMA :
        {
            if(add_stack(L_CLOSEP, func_type, L_PAR)) 
                return L_errno;

            LSTACK& last_op = L_OPS.back();
            if(!(is_fn(last_op.type) || is_tfn(last_op.type) || is_mtfn(last_op.type)))
            {
                L_errno = L_SYNTAX_ERR;
                return L_errno;
            }

            last_op.nb_args++;
            return add_stack(L_OPENP, func_type, L_PAR);
        }

        case L_OCPAR :      // open-close parentheses () 
        {
            LSTACK& last_op = L_OPS.back();
            if(!is_fn(last_op.type))
            {
                L_errno = L_SYNTAX_ERR;
                return L_errno;
            }

            last_op.nb_args = 0;
            return 0;
        }

        default :
            break;
    }

    return 0;
}


/**
 *  Empties the stack of operators L_OPS by adding all operators and the 
 *  number of their arguments to L_EXPR.
 *  
 *  @return     int 0 on success
 *                  L_errno on error
 */
int CLEC::empty_ops_stack()
{
    while(L_OPS.size() > 0)
    {
        if(save_op() != 0) 
            return L_errno;
    }

    LEC_OTHER al(L_EOE);     // end of expression
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
int CLEC::lag_expr(int lag)
{
    int start_sub_expr = find_sub_expr_start(L_EXPR);
    if(start_sub_expr < 0) 
        return L_errno;

    for(auto it = L_EXPR.begin() + start_sub_expr; it != L_EXPR.end(); it++)
    {
        ATOMIC_LEC& al = *it;

        // check that the atomic lec represents a variable
        if(!std::holds_alternative<LEC_VAR>(al)) 
            continue;

        // only applies lag to non-timed variables.
        LEC_VAR& lvar = std::get<LEC_VAR>(al);
        if(lvar.type != L_VAR) 
            continue;

        // skip if a period is associated to the variable (ex. A[1960Y1]) because 
        // in that case, the variable is fixed in time and cannot be lagged.
        if(lvar.per.step != 0) 
            continue;
        
        // applies lag
        lvar.lag += lag;
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
int CLEC::time_expr(TOKEN& token)
{
    int start_sub_expr = find_sub_expr_start(L_EXPR);
    if(start_sub_expr < 0) 
        return L_errno;

    for(auto it = L_EXPR.begin() + start_sub_expr; it != L_EXPR.end(); it++)
    {
        ATOMIC_LEC& al = *it;

        // check that the atomic lec represents a variable
        if(!std::holds_alternative<LEC_VAR>(al)) 
            continue;

        // only applies lag to non-timed variables.
        LEC_VAR& lvar = std::get<LEC_VAR>(al);
        if(lvar.type != L_VAR) 
            continue;

        // skip if a period is associated to the variable (ex. A[1960Y1]) because 
        // in that case, the variable is fixed in time and cannot be lagged.
        if(lvar.per.step != 0) 
            continue;
        
        // applies time expression
        lvar.per = token.period;
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
int CLEC::analyze_lag()
{
    int group = 0;
    TOKEN token = parser.read_next_token(group);
    switch(group) 
    {
        // case 1: lag or lead like in A[-2]
        case L_OP :
        {
            // check that the operator is a '+' or a '-'
            int op = token.type;
            if(op != L_MINUS && op != L_PLUS)
            {
                L_errno = L_LAG_ERR;
                return L_errno;
            }

            // check that the next token is a long constant (the lag)
            token = parser.read_next_token(group);
            if(group != L_LCONST)
            {
                L_errno = L_LAG_ERR;
                return L_errno;
            }
            
            // apply the lag
            int lag = L_YY->yy_long;
            if(op == L_MINUS) 
                lag = -lag;
            lag_expr(lag);
            break;
        }  
        // case 2: time expression like in A[2000Y1]
        case L_PERIOD :
            // apply the time expression
            time_expr(token);
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
    token = parser.read_next_token(group);
    if(group == L_CLOSEB) 
        return 0;

    L_errno = L_LAG_ERR;
    return L_errno;
}


/**
 *  First step of LEC compilation. L_YY (see l_token.c) is the open stream containing the analyzed LEC expression.
 *  
 *  At the end of this function, 2 tables are created: L_EXPR and map_objs:
 *      - L_EXPR contains atomic expressions in the execution order including references to map_objs 
 *      - map_objs contains the names included in the lec expression
 *  
 *  @return int     error code: 0 on success or L_PAR_ERR, L_SYNTAX_ERR...
 */
int CLEC::parse(const bool side_of_eq)
{
    int group;
    int start = 1;
    int beg = 1;        // indicate if next token is an oper or an expr
    int L_PAR = 0;      // Current parenthesis depth

    // reset global variables
    L_errno = 0;
    L_EXPR.clear();

    // NOTE: if the LEC expression is the left or right side of an equation, 
    //       we don't reset map_objs
    if(!side_of_eq)
        map_objs.clear();

    /* LOOP ON TOKEN */
    TOKEN token;
    while(true) 
    {
        // Group of operators, not the operator itself
        token = parser.read_next_token(group);
        if(L_errno) 
            return L_errno;
        
again:
        switch(group) 
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
                if(save_var(token)) 
                    return L_errno;
                beg = 0;
                break;
            case L_OP :         // Operator
                if(beg != 0) 
                {
                    switch(token.type) 
                    {
                        case L_MINUS :
                            token.type = L_UMINUS;
                            break;
                        case L_PLUS :
                            token.type = L_UPLUS;
                            break;
                        default :
                        {
                            L_errno = L_SYNTAX_ERR;
                            return L_errno;
                        }
                    }
                    group = L_FN;
                    goto again;
                }
                beg = 1;
                if(add_stack(group, token.type, L_PAR)) 
                    return L_errno;
                break;
            case L_FN :         // not time function
            case L_TFN:         // time function
            case L_MTFN:        // variadic time function
            case L_OPENP :      // open parenthesis
                if(beg == 0) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(add_stack(group, token.type, L_PAR)) 
                    return L_errno;
                break;
            case L_OCPAR :      // open-close parentheses "()""
                if(beg == 0) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(add_stack(group, token.type, L_PAR)) 
                    return L_errno;
                beg = 0;
                break;
            case L_CLOSEP :     // Close parenthesis
                if(beg == 1) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(add_stack(group, token.type, L_PAR)) 
                    return L_errno;
                break;
            case L_COMMA :      // Comma
                if(beg == 1) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(add_stack(group, token.type, L_PAR)) 
                    return L_errno;
                beg = 1;
                break;
            case L_OPENB:
                if(beg == 1) 
                {
                    L_errno = L_SYNTAX_ERR;
                    return L_errno;
                }
                if(analyze_lag()) 
                    return L_errno;
                beg = 0;
                break;
            case YY_EOF:
            case L_EOE :        // End of expression
                if(start) 
                {
                    LEC_OTHER al(L_EOE);
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
                    empty_ops_stack();
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
 *  @param [in] v_alec  std::vector<ATOMIC_LEC>&    vector of atomic LEC elements
 *  @param [in] close   int                         position in v_alec where the expression is "closed"
 *  @return                                         position in v_alec where the expression starts
 */
int find_sub_expr_start(const std::vector<ATOMIC_LEC>& v_alec, int close)
{
    if(v_alec.empty())
        return -1;

    if(close < -1 || close >= (int) v_alec.size())
    {
        std::string error_msg = "Invalid position for close in sub_expr: " + std::to_string(close) + ".\n";
        error_msg += "Valid values are between -1 and " + std::to_string(v_alec.size() - 1) + ".";
        throw std::out_of_range(error_msg);
    }

    int nb_parents = 0;
    int end = (close >= 0) ? close : (int) v_alec.size() - 1;

    // browse backwards all elements of the expression
    for(int pos = end; pos >= 0; pos--) 
    {
        const ATOMIC_LEC& al = v_alec[pos];

        // if open or close parenthesis, update the number of parents
        if(std::holds_alternative<LEC_OTHER>(al)) 
        {
            const LEC_OTHER& other = std::get<LEC_OTHER>(al);
            // open parenthesis -> decrease number of parents
            if(other.type == L_OPENP)
                nb_parents--;
            // close parenthesis -> increase number of parents
            if(other.type == L_CLOSEP)
                nb_parents++;
        }
        
        if(nb_parents > 0) 
            continue;
        
        // QUESTION: why calling sub_expr twice ?
        if(std::holds_alternative<LEC_OP>(al))
        {
            pos = find_sub_expr_start(v_alec, pos - 1);
            pos = find_sub_expr_start(v_alec, pos - 1);
            return pos;
        }

        // if not a function, we have reached the beginning of the expression
        bool b_is_fn = std::holds_alternative<LEC_FN>(al);
        bool b_is_tfn = std::holds_alternative<LEC_TFN>(al);
        bool b_is_mtfn = std::holds_alternative<LEC_MTFN>(al);
        if(!b_is_fn && !b_is_tfn && !b_is_mtfn) 
            return pos;
    }

    L_errno = L_LAG_ERR;
    return -1;
}

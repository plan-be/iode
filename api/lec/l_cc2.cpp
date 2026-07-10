/**
 *  @header4iode
 *  
 *  Second step of the LEC compilation process. During this step, a CLEC structure is created based on the content of L_EXPR and L_NAMES. 
 *
 *  A CLEC struct is the "executable" form a the LEC expression. This struct contains (see also iode.h):
 *      - (short)   : length of the "executable" expression 
 *      - (short)   : number of variables 
 *      - (LNAME)   : list of LNAME structs where LNAME = [variable name, position in KDB defined JIT at "link"] (see iode.h)
 *      - (char*)   : executable expression (following the last LNAME above). Each element of the expression contains:
 *                      a type (1 byte) = type of the ALEC expression (can be L_VAR, L_COEF, op...)
 *                      the optional parameter(s) whose content depends on type. For example:
 *                           - CVAR struct for type variables or scalars (see iode.h):
 *                                  - position of the variable or scalar in the list of names LNAME's
 *                                  - lag (for variables)
 *                                  - ref (used only at execution time)
 *                                  - Period (for variables)
 *                           - Period for type "period"
 *                           - long for type "integer constant"
 *                           - char for the nb of args in case of type "function"
 *                           - more complex (recursive) struct for types "TFN's" or "MTFN's"
 */

#include "api/lec/lec.h"

/**
 * Calculates the number of bytes required to save a sequence of ALEC atomic expressions into a CLEC struct by
 * adding the size reclaimed by each ALEC atomic expression.
 * 
 * @param [in] expr     ALEC*   pointer to a table of ALEC atomic expressions
 * @param [in] from     int     starting position in ALEC of the expression whose size is to be evaluated
 * @param [in] to       int     ending position in ALEC of that expression
 * @return              int     size in bytes of the expression in CLEC form
*/
static int L_calc_len(const std::vector<ATOMIC_LEC>& expr, const int from, const int to)
{
    if(expr.empty()) 
        return 0;
    
    int len = 0;
    for(int i = from; i < to; i++) 
    {
        const ATOMIC_LEC& al = expr[i];
        len += std::visit([](auto& arg) { return arg.get_length(); }, al);
        if(std::holds_alternative<LEC_TFN>(al))
        {
            const LEC_TFN& al_tfn = std::get<LEC_TFN>(al);
            len -= al_tfn.len_args;
        }
        if(std::holds_alternative<LEC_MTFN>(al))
        {
            const LEC_MTFN& al_mtfn = std::get<LEC_MTFN>(al);
            len -= al_mtfn.len_args;
        }
    }
    
    return len;
}


/**
 * Second stage of LEC compilation. Generates an "executable" LEC expression (heterogenous container).
 * 
 * @param [in]  expr    ALEC*   pointer to the first atomic element of the expression (result of L_cc1(), normally L_EXPR)
 * @return              CLEC*   pointer to a compiled LEC (see above for details on the contents of a CLEC)
*/
std::shared_ptr<CLEC> L_cc2(std::vector<ATOMIC_LEC>& expr, const std::string& lec) 
{
    std::shared_ptr<CLEC> clec;
    if(expr.empty()) 
        return clec;

    int pos_buffer = 0;             // position (in number of bytes) in the buffer
    int pos_expr = 0;               // position in the vector of atomic lec elements
    int alen = 0;                   // current allocated size of the buffer (in bytes)
    unsigned char* buffer = NULL;   // buffer to save the "executable" expression (will be copied in the CLEC struct at the end)
    for(ATOMIC_LEC& al : expr) 
    {
        // end of expression -> stop
        if(std::holds_alternative<LEC_OTHER>(al) && std::get<LEC_OTHER>(al).type == L_EOE)
            break;

        // if the current size of the buffer is not sufficient to save the next element, 
        // reallocate it by chunks of 512 bytes
        if(pos_buffer + 30 >= alen) 
        {
            buffer = (unsigned char*) SW_nrealloc(buffer, alen, alen + 512);
            alen += 512;
        }

        if(std::holds_alternative<LEC_TFN>(al))
        {
            LEC_TFN& al_tfn = std::get<LEC_TFN>(al);

            // compute the length in bytes of the function arguments 
            // (i.e. the sub-expression in the buffer) 
            int start_pos = L_sub_expr(expr, pos_expr - 1);
            short len = (short) L_calc_len(expr, start_pos, pos_expr);
            if(len < 0 || len > pos_buffer) 
            {
                std::string error_msg = "L_cc2(): Could not compute the length in bytes of ";
                error_msg += "arguments of function '" + al_tfn.representation + "'.";
                kwarning(error_msg.c_str());
                SW_nfree(buffer);
                return clec;
            }

            al_tfn.len_args = (short) len;
        }
        
        if(std::holds_alternative<LEC_MTFN>(al))
        {
            LEC_MTFN& al_mtfn = std::get<LEC_MTFN>(al);

            int nv_args = L_MIN_MTARGS[al_mtfn.pos];
            al_mtfn.nv_args = nv_args;

            al_mtfn.v_len_args.clear();
            int new_pos = pos_expr - 1;
            for(int j = 0; j < nv_args; j++) 
            {
                // compute the length in bytes of the function argument 
                // representing a sub-expression in the buffer 
                int start_pos = L_sub_expr(expr, new_pos);
                short len = (short) L_calc_len(expr, start_pos, new_pos + 1);
                if(len < 0) 
                {
                    std::string error_msg = "L_cc2(): Could not compute the length in bytes of ";
                    error_msg += "arguments of function '" + al_mtfn.representation + "'.";
                    kwarning(error_msg.c_str());
                    SW_nfree(buffer);
                    return clec;
                }
                al_mtfn.v_len_args.push_back(len);
                new_pos = start_pos - 1;
            }
        }

        // for each variant type -> save the executable representation of the atomic lec in the buffer 
        // and update the position in the buffer
        std::visit([&buffer, &pos_buffer](auto& arg) { arg.add_to_buffer(buffer, pos_buffer); }, al);

        pos_expr++;
    }

    if(pos_buffer == 0)
    {
        SW_nfree(buffer);
        return clec;
    }
    
    clec = std::make_shared<CLEC>(lec, expr, buffer, pos_buffer);
    SW_nfree(buffer);
    L_NAMES.clear();

    return clec;
}


/**
 * Compiles L_YY, the open YY stream containing a LEC expression (see l_cc1.c).
 *  
 * @return  CLEC*  compiled and serialized LEC expression.
 */
std::shared_ptr<CLEC> L_cc_stream(const std::string& lec)
{
    std::shared_ptr<CLEC> clec;

    if(L_cc1() != 0)
        return clec;
    
    clec = L_cc2(L_EXPR, lec);
    L_EXPR.clear();
    return clec;
}


/**
 *  Compiles a LEC string. Returns an allocated CLEC struct pointer.
 *  
 *  @param [in]     lec     char*   LEC expression
 *  @return                 CLEC*   compiled and serialized LEC expression
 *  
 */
std::shared_ptr<CLEC> L_cc(const std::string& lec)
{
    std::shared_ptr<CLEC> clec;
    if(L_open_string((char*) lec.c_str())) 
        return clec;

    clec = L_cc_stream(lec);
    L_close();
    return clec;
}


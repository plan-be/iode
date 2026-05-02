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
 * 
 * Main functions:
 * 
 *      CLEC *L_cc2(ALEC* expr, std::string& lec)        Second stage of LEC compilation. Generates an "executable" LEC expression.
 *      CLEC *L_cc_stream(std::string& lec)              Compiles L_YY, the open YY stream containing a LEC expression.
 *      CLEC *L_cc(char* lec)                            Compiles a LEC string. 
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
    }
    
    return len;
}


/**
 * Second stage of LEC compilation. Generates an "executable" LEC expression (heterogenous container).
 * 
 * @param [in]  expr    ALEC*   pointer to the first atomic element of the expression (result of L_cc1(), normally L_EXPR)
 * @return              CLEC*   pointer to a compiled LEC (see above for details on the contents of a CLEC)
*/
CLEC* L_cc2(const std::vector<ATOMIC_LEC>& expr, const std::string& lec) 
{
    if(expr.empty()) 
        return nullptr;

    int pos_buffer = 0;             // position (in number of bytes) in the buffer
    int pos_expr = 0;               // position in the vector of atomic lec elements
    int alen = 0;                   // current allocated size of the buffer (in bytes)
    unsigned char* buffer = NULL;   // buffer to save the "executable" expression (will be copied in the CLEC struct at the end)
    for(const ATOMIC_LEC& al : expr) 
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
            const LEC_TFN& al_tfn = std::get<LEC_TFN>(al);

            // save the type of the expression element (1 byte)
            buffer[pos_buffer] = al_tfn.type;
            pos_buffer++;

            // move function arguments in the buffer to put type and nb of args before them
            int start_pos = L_sub_expr(expr, pos_expr - 1);
            long len = L_calc_len(expr, start_pos, pos_expr);
            if(len < 0) 
            {
                std::string error_msg = "L_cc2(): Could not compute the length in bytes of ";
                error_msg += "arguments of function '" + al_tfn.representation + "'.";
                kwarning(error_msg.c_str());
                SW_nfree(buffer);
                return nullptr;
            }
            // a) go 'len + 1' bytes backward
            int new_pos_buffer = pos_buffer - (len + 1);
            unsigned char* tmp = buffer + new_pos_buffer;
            // b) move tmp[0:len] to tmp[shift:shift+len] 
            int shift = 2 + sizeof(short);
            for(int i = len - 1; i >= 0; i--)
                tmp[i + shift] = tmp[i];
            // save the length in bytes of the function arguments
            memcpy(tmp + shift - sizeof(short), &len, sizeof(short));

            tmp[0] = al_tfn.type;
            tmp[1] = al_tfn.nb_args;
            pos_buffer += sizeof(short) + 1;
        }
        else if(std::holds_alternative<LEC_MTFN>(al))
        {
            const LEC_MTFN& al_mtfn = std::get<LEC_MTFN>(al);

            // save the type of the expression element (1 byte)
            buffer[pos_buffer] = al_mtfn.type;
            pos_buffer++;

            long tot_len = 0;
            int new_pos = pos_expr - 1;
            unsigned char* tmp = nullptr;
            int nv_args = L_MIN_MTARGS[al_mtfn.pos];
            for(int j = 0; j < nv_args; j++) 
            {
                // move function ?? arguments in the buffer to put type, nb_args and nv_args before them after the loop
                int start_pos = L_sub_expr(expr, new_pos);
                long len = L_calc_len(expr, start_pos, new_pos + 1);
                if(len < 0) 
                {
                    std::string error_msg = "L_cc2(): Could not compute the length in bytes of ";
                    error_msg += "arguments of function '" + al_mtfn.representation + "'.";
                    kwarning(error_msg.c_str());
                    SW_nfree(buffer);
                    return nullptr;
                }
                tot_len += len;
                new_pos = start_pos - 1;
                // a) go 'len + 1' bytes backward
                int new_pos_buffer = pos_buffer - (tot_len + 1);
                tmp = buffer + new_pos_buffer;
                // b) move tmp[0:len] to tmp[shift:shift+sub_len] 
                int shift = 3 + (nv_args - j + 1) * sizeof(short);
                for(int i = len - 1; i >= 0; i--)
                    tmp[i + shift] = tmp[i];
                // save the length in bytes of the function ?? arguments
                memcpy(tmp + shift - sizeof(short), &len, sizeof(short));
            }

            tot_len += nv_args * sizeof(short);
            memcpy(tmp + 3, &tot_len, sizeof(short));

            tmp[0] = al_mtfn.type;
            tmp[1] = al_mtfn.nb_args;
            tmp[2] = nv_args;
            pos_buffer += 2 + (1 + nv_args) * sizeof(short);
        }
        else
        {
            // for other variant types (LEC_CONST_REAL, LEC_CONST_LONG, LEC_COEF, LEC_VAR, LEC_PERIOD, LEC_OP, LEC_FN, LEC_VAL_FN)
            std::visit([&buffer, &pos_buffer](auto& arg) { arg.add_to_buffer(buffer, pos_buffer); }, al);
        }

        pos_expr++;
    }

    if(pos_buffer == 0)
    {
        SW_nfree(buffer);
        return nullptr;
    }
    
    CLEC* clec = new CLEC(lec, buffer, pos_buffer);
    SW_nfree(buffer);
    L_NAMES.clear();

    return clec;
}


/**
 * Compiles L_YY, the open YY stream containing a LEC expression (see l_cc1.c).
 *  
 * @return  CLEC*  compiled and serialized LEC expression.
 */
CLEC* L_cc_stream(const std::string& lec)
{
    CLEC* cl;

    if(L_cc1() != 0)
        return NULL;
    
    cl = L_cc2(L_EXPR, lec);
    L_EXPR.clear();
    return cl;
}


/**
 *  Compiles a LEC string. Returns an allocated CLEC struct pointer.
 *  
 *  @param [in]     lec     char*   LEC expression
 *  @return                 CLEC*   compiled and serialized LEC expression
 *  
 */
CLEC *L_cc(const std::string& lec)
{
    if(L_open_string((char*) lec.c_str())) 
        return nullptr;

    CLEC* clec = L_cc_stream(lec);
    L_close();
    return clec;
}


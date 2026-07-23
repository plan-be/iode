/**
 * @header4iode
 *
 * Functions implementing the interface between the LEC functions and the IODE KDB's.
 *  
 *      double *L_getvar(KDBVariablesPtr kdb, int pos)    Retrieves a pointer to the first element of a VAR.
 *      double L_getscl(KDBScalarsPtr kdb, int pos)       Retrieves a scalar value.
 *      Sample *L_getsmpl(KDBVariablesPtr kdb)            Retrieves the sample of a KDB.
 *      int L_findscl(KDBScalarsPtr kdb, char *name)      Retrieves a scalar position.
 *      int L_findvar(KDBVariablesPtr kdb, char* name)    Retrieves a variable position.
 */

#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/print/print.h"

#include <algorithm>    // for std::max


/**
 *  Implementation of L_getvar() in the context of IODE objects. Retrieves a pointer to the first element of a VAR.
 *  
 *  @param [in] kdb     KDB*        KDB of variables from which the variable value has to be retrieved
 *  @param [in] pos     int         pos of the var in kdb
 *  @return             IODE_READ   pointer to the first value of the variable
 *  
 */
double* L_getvar(KDBVariablesPtr kdb, int pos)
{
    std::string name = kdb->get_name(pos);
    return kdb->get_var_ptr(name);
}


/**
 *  Implementation of L_getscl() in the context of IODE objects. Retrieves a scalar value.
 *  
 *  @param [in] kdb     KDB*        KDB of Scalar from which the scalar value has to be retrieved
 *  @param [in] pos     int         pos of the Scalar in kdb
 *  @return             IODE_READ   value of the scalar
 *  
 */
double L_getscl(KDBScalarsPtr kdb, int pos)
{
    std::string name = kdb->get_name(pos);
    std::shared_ptr<Scalar> scl = kdb->get_obj_ptr(name);
    return(scl->value);
}


/**
 *  Implementation of L_getsmpl() in the context of IODE objects. 
 *  Retrieves the pointer to the sample a the KDB.
 *  
 *  @param [in] kdb     KDB*    KDB of VAR from which the Period has to be retrieved
 *  @return             Sample* pointer to the Sample struct (not allocated)
 *  
 */
Sample* L_getsmpl(KDBVariablesPtr kdb)
{
    std::shared_ptr<Sample> sample = kdb->get_sample();
    return sample ? sample.get() : nullptr;
}


/**
 *  Implementation of L_findscl() in the context of IODE objects. Retrieves a scalar position.
 *  
 *  @param [in] kdb     KDB*        KDB of Scalar from which the scalar postion has to be retrieved
 *  @param [in] name    char*       name of the scalar
 *  @return             int         position of name in KDB 
 *  
 */
int L_findscl(KDBScalarsPtr kdb, const std::string& name)
{
    return kdb->index_of(name);
}


/**
 *  Implementation of L_findvar() in the context of IODE objects. Retrieves a variable position.
 *  
 *  @param [in] kdb     KDB*        KDB of VAR from which the position has to be retrieved
 *  @param [in] name    char*       name of the VAR
 *  @return             int         position of name in KDB 
 *  
 */
int L_findvar(KDBVariablesPtr kdb, const std::string& name)
{
    return kdb->index_of(name);
}

/**
 *  Implentation of L_expand()
 *  
 *  @param [in] name 
 *  @return 
 *  
 */
 
char* L_expand(char* list_name)
{
    if(L_expand_super) 
        return((*L_expand_super)(list_name));
    else 
    {
        if(!global_ws_lst->contains(list_name)) 
            return NULL;
        std::shared_ptr<List> lst_ptr = global_ws_lst->get_obj_ptr(list_name);
        if(!lst_ptr) 
            return NULL;
        return (char*) lst_ptr->c_str();
    }    
}

/**
 *  Print a LEC expression. Set the engogenous (name) in bold.
 *  
 *  @param [in] name   char* name    endogenous name   
 *  @param [in] eqlec  char* eqlec   LEC expression 
 *  @param [in] eqclec CLEC* eqclec  CLEC (compiled LEC) equivalent to LEC
 *  @param [in] coefs  int   coefs   if 1: replace scalars by their value   
 *                                   if 2: replace scalars by their value and their t-test   
 *  @return            bool
 */
bool print_lec_definition(const std::string& name, const std::string& eqlec, 
    const std::shared_ptr<CLEC> eqclec, const int coefs)
{
    if(!eqclec) 
        return false;

    // create a char* array containing a copy of the string eqlec
    int lg = (int) eqlec.size();
    lg = std::max(512, 4 * lg);
    char* c_lec = new char[lg];
    strcpy(c_lec, eqlec.c_str());
    
    // create a copy of the CLEC* eqclec
    std::shared_ptr<CLEC> clec = std::make_shared<CLEC>(*eqclec);

    char buf[80];
    sprintf(buf, "%cb%s%cB", A2M_ESCCH, name.c_str(), A2M_ESCCH);
    SCR_replace_gnl((unsigned char*) c_lec, (unsigned char*) name.c_str(), 
                    (unsigned char*) buf, (unsigned char*) "_\\");
    
    std::string sname;
    std::shared_ptr<Scalar> scl = nullptr;
    char tcoef[128], ttest[128];
    for(const auto& [sname, _] : clec->objs) 
    {
        buf[0] = 0;
        if(coefs && is_coefficient(sname)) 
        {
            if(global_ws_scl->contains(sname)) 
            {
                scl = global_ws_scl->get_obj_ptr(sname);
                T_fmt_val(tcoef, scl->value, 15, K_NBDEC);
                T_fmt_val(ttest, scl->calculate_t_test(), 15, K_NBDEC);
                if(coefs == 1) 
                    sprintf(buf, "%ci%s%cI", A2M_ESCCH, tcoef, A2M_ESCCH);
                if(coefs == 2) 
                    sprintf(buf, "%ci%s(%s)%cI", A2M_ESCCH, tcoef, ttest, A2M_ESCCH);
            }
        }
        if(buf[0] == 0) 
            sprintf(buf, "%ci%s%cI", A2M_ESCCH, sname.c_str(), A2M_ESCCH);
        SCR_replace_gnl((unsigned char*) c_lec, (unsigned char*) sname.c_str(), 
                        (unsigned char*) buf, (unsigned char*) "_\\");
    }

    dump_string((char*) " ", c_lec);

    delete[] c_lec;
    return true;
}

/**
 * Second stage of LEC compilation. Generates an "executable" LEC expression (heterogenous container).
 * 
 * @param [in]  expr    std::vector<ATOMIC_LEC>&   vector of atomic lec elements (result of generate_lec_expression(), normally L_EXPR)
 * @param [in]  lec     std::string&               original LEC expression string
*/
void CLEC::initialize(std::vector<ATOMIC_LEC>& expr, const std::string& lec)
{
    if(expr.empty())
        throw std::invalid_argument("CLEC constructor: empty expression vector.");

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
            short len = this->calculate_length(expr, start_pos, pos_expr);
            if(len < 0 || len > pos_buffer) 
            {
                std::string error_msg = "CLEC constructor: Could not compute the length in bytes of ";
                error_msg += "arguments of function '" + al_tfn.representation + "'.";
                SW_nfree(buffer);
                throw std::runtime_error(error_msg);
            }

            al_tfn.len_args = len;
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
                short len = this->calculate_length(expr, start_pos, new_pos + 1);
                if(len < 0) 
                {
                    std::string error_msg = "CLEC constructor: Could not compute the length in bytes of ";
                    error_msg += "arguments of function '" + al_mtfn.representation + "'.";
                    SW_nfree(buffer);
                    throw std::runtime_error(error_msg);
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
        std::string error_msg = "CLEC constructor: Could not compile the LEC expression: ";
        error_msg += "'" + lec + "'.";
        SW_nfree(buffer);
        throw std::runtime_error(error_msg);
    }
    
    this->v_expression = expr;
    this->len_expr = pos_buffer;
    this->expression = new unsigned char[pos_buffer];
    memset(this->expression, 0, pos_buffer);
    memcpy(this->expression, buffer, pos_buffer);

    // initialize all names with position -1 (not found)
    for(const std::string& name : L_NAMES)
        this->objs.push_back({name, -1});

    SW_nfree(buffer);
    L_EXPR.clear();
}

CLEC::CLEC(std::vector<ATOMIC_LEC>& expr, const std::string& lec)
{
    initialize(L_EXPR, lec);
}

CLEC::CLEC(const std::string& lec)
{
    if(L_open_string((char*) lec.c_str()) != 0) 
        throw std::runtime_error("Error opening LEC string");

    if(generate_lec_expression() != 0)
        throw std::runtime_error("Error generating LEC expression");
    
    initialize(L_EXPR, lec);
    L_close();
}

/**
 * Compiles a LEC equation and tries to analytically solve the equation with respect to endo.
 * 
 * Generates a CLEC form with the result and set clec->duplicated_endo to 1 if the
 * generated form is of the form "0 := LHS - RHS")
*/
CLEC::CLEC(const std::string& eq, const std::string& endo)
{
    int duplicated_endo = 0;
    std::string lec = eq;

    L_invert(eq, endo, &duplicated_endo);
    if(L_errno != 0) 
    {
        L_EXPR.clear();
        std::string error_msg = "Equation: cannot invert LEC expression '" + eq + "'\n";
        error_msg += "with respect to endogenous variable '" + endo + "' -> " + L_error();
        throw std::runtime_error(error_msg);
    }

    initialize(L_EXPR, lec);
    this->duplicated_endo = (char) duplicated_endo;
}

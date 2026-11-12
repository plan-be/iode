#pragma once
#include "api/pch.h"
#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/objs/old_structs.h"
#include "api/print/print.h"


/**
 *  Implementation of L_expand()
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
 *  Prints this LEC expression. Sets the endogenous variable (name) in bold.
 *  
 *  @param [in] name   std::string&  endogenous name   
 *  @param [in] eqlec  std::string&  LEC expression 
 *  @param [in] coefs  int           if 1: replace scalars by their value   
 *                                   if 2: replace scalars by their value and their t-test   
 *  @return            bool          true on success, false on failure
 */
bool CLEC::print_definition(const std::string& name, const std::string& eqlec, const int coefs)
{
    // create a char* array containing a copy of the string eqlec
    int lg = (int) eqlec.size();
    lg = std::max(512, 4 * lg);
    char* c_lec = new char[lg];
    strcpy(c_lec, eqlec.c_str());
    
    // create a copy of this CLEC
    std::shared_ptr<CLEC> clec = std::make_shared<CLEC>(*this);

    char buf[80];
    sprintf(buf, "%cb%s%cB", A2M_ESCCH, name.c_str(), A2M_ESCCH);
    SCR_replace_gnl((unsigned char*) c_lec, (unsigned char*) name.c_str(), 
                    (unsigned char*) buf, (unsigned char*) "_\\");
    
    std::string sname;
    std::string tcoef, ttest;
    std::shared_ptr<Scalar> scl = nullptr;
    for(const auto& sname : clec->v_coeff_names)
    {
        buf[0] = 0;
        if(coefs)
        {
            if(global_ws_scl->contains(sname)) 
            {
                scl = global_ws_scl->get_obj_ptr(sname);
                tcoef = format_double_value(scl->value, 15, tbl_nb_decimals);
                ttest = format_double_value(scl->calculate_t_test(), 15, tbl_nb_decimals);
                if(coefs == 1) 
                    sprintf(buf, "%ci%s%cI", A2M_ESCCH, tcoef.c_str(), A2M_ESCCH);
                if(coefs == 2) 
                    sprintf(buf, "%ci%s(%s)%cI", A2M_ESCCH, tcoef.c_str(), ttest.c_str(), A2M_ESCCH);
            }
        }

        if(buf[0] == 0) 
            sprintf(buf, "%ci%s%cI", A2M_ESCCH, sname.c_str(), A2M_ESCCH);
        SCR_replace_gnl((unsigned char*) c_lec, (unsigned char*) sname.c_str(), 
                        (unsigned char*) buf, (unsigned char*) "_\\");
    };

    for(const auto& sname : clec->v_var_names)
    {
        buf[0] = 0;
        sprintf(buf, "%ci%s%cI", A2M_ESCCH, sname.c_str(), A2M_ESCCH);
        SCR_replace_gnl((unsigned char*) c_lec, (unsigned char*) sname.c_str(), 
                        (unsigned char*) buf, (unsigned char*) "_\\");
    }

    dump_string((char*) " ", c_lec);

    delete[] c_lec;
    return true;
}

short CLEC::calculate_binary_length(const std::vector<ATOMIC_LEC>& expr, const int from, const int to)
{
    if(expr.empty())
        return 0;

    short len = 0;
    for(int i = from; i < to; i++)
    {
        const ATOMIC_LEC& al = expr[i];
        len += std::visit([](auto& arg) { return arg.get_binary_length(); }, al);
    }

    return len;
}

void CLEC::reorder_expression()
{
    // nothing to reorder
    if(this->v_expression.empty())
        return;

    // create a copy of the expression vector to reorder
    std::vector<ATOMIC_LEC> expr;
    expr.swap(this->v_expression);

    // reorder the expression vector to ensure that the arguments of each function 
    // are placed after the function in the vector of atomic LEC elements
    for(ATOMIC_LEC& al : expr)
    {
        if(std::holds_alternative<LEC_OTHER>(al))
        {
            LEC_OTHER& al_other = std::get<LEC_OTHER>(al);
            // end of expression -> stop
            if(al_other.type == L_EOE)
                break;
        }
        
        this->v_expression.push_back(al);

        if(std::holds_alternative<LEC_TFN>(al))
        {
            LEC_TFN& al_tfn = std::get<LEC_TFN>(al);
            al_tfn.binary_length_args = -1;

            // compute the start and end positions of the arguments of the function 
            // NOTE: - 2 because the last element of v_expression is always (should be) 
            //       of type L_EOE (End Of Expression)
            int args_end = (int) this->v_expression.size() - 2;  
            int args_start = find_sub_expr_start(this->v_expression, args_end);
            if(args_start < 0 || args_end < args_start) 
            {
                std::string error_msg = "CLEC constructor: Could not determine the start and end ";
                error_msg += "positions of the arguments of function '" + L_TFN_NAMES[al_tfn.pos] + "'.";
                throw std::runtime_error(error_msg);
            }
            al_tfn.length_expr = args_end - args_start + 1;
            al_tfn.binary_length_args = this->calculate_binary_length(expr, args_start, args_end + 1);

            // insert current LEC_TFN element before its arguments in the vector
            this->v_expression.pop_back();
            this->v_expression.insert(this->v_expression.begin() + args_start, al);
        }

        if(std::holds_alternative<LEC_MTFN>(al))
        {
            LEC_MTFN& al_mtfn = std::get<LEC_MTFN>(al);

            int nv_args = L_MIN_MTARGS[al_mtfn.pos];
            al_mtfn.nv_args = nv_args;
            al_mtfn.v_length_expr.clear();
            al_mtfn.v_length_expr.reserve(nv_args);
            al_mtfn.v_binary_length_args.clear();
            al_mtfn.v_binary_length_args.reserve(nv_args);

            short arg_binary_length;

            // compute the start and end positions of the arguments of the function
            int args_start;
            // NOTE: - 2 because the last element of v_expression is always (should be) 
            //       of type L_EOE (End Of Expression)
            int args_end = (int) this->v_expression.size() - 2;
            for(int j = 0; j < nv_args; j++) 
            {
                // compute the start and end positions of the nth sub-expression of the function 
                args_start = find_sub_expr_start(this->v_expression, args_end);
                if(args_start < 0 || args_end < args_start) 
                {
                    std::string error_msg = "CLEC constructor: Could not determine the start and end ";
                    error_msg += "positions of the arguments of function '" + L_MTFN_NAMES[al_mtfn.pos] + "'.";
                    throw std::runtime_error(error_msg);
                }
                int length_expr = args_end - args_start + 1;
                al_mtfn.v_length_expr.push_back(length_expr);
                arg_binary_length = this->calculate_binary_length(expr, args_start, args_end + 1);
                al_mtfn.v_binary_length_args.push_back(arg_binary_length);

                args_end = args_start - 1;
            }
            // NOTE: the arguments have been processed in reverse order
            std::reverse(al_mtfn.v_length_expr.begin(), al_mtfn.v_length_expr.end());

            // insert current LEC_MTFN element before its arguments in the vector
            this->v_expression.pop_back();
            this->v_expression.insert(this->v_expression.begin() + args_start, al);
        }
    }
}

/**
 * Serializes a CLEC object.
 *
 * @param [out] pack    (char **)   placeholder for the pointer to the serialized object
 * @return                          true if the serialization succeeded, false otherwise
 */
bool CLEC::to_binary(char** pack) const
{
    int pos_buffer = 0;             // position (in number of bytes) in the buffer
    int pos_expr = 0;               // position in the vector of atomic lec elements
    int allocated_len = 0;          // current allocated size of the buffer (in bytes)
    unsigned char* buffer = NULL;   // buffer to save the "executable" expression (will be copied in the CLEC struct at the end)
    CLEC_6_68* fake_clec = nullptr; // CLEC struct (IODE version 6.68 -> for backward compatibility)

    for(const ATOMIC_LEC& al : this->v_expression)
    {
        // end of expression -> stop
        if(std::holds_alternative<LEC_OTHER>(al) && std::get<LEC_OTHER>(al).type == L_EOE)
            break;

        // if the current size of the buffer is not sufficient to save the next element, 
        // reallocate it by chunks of 512 bytes
        if(pos_buffer + 30 >= allocated_len)
        {
            // NOTE: realloc(nullptr, size) behaves like malloc(size)
            unsigned char* new_buffer = (unsigned char*) realloc(buffer, allocated_len + 512);
            if(new_buffer == nullptr)
            {
                free(buffer);
                std::string error_msg = "CLEC::to_binary(): could not allocate memory to serialize ";
                error_msg += "the LEC expression '" + lec + "'.";
                kwarning((char*) error_msg.c_str());
                return false;
            }
            buffer = new_buffer;
            // fill the newly allocated bytes with nulls (as SW_nrealloc did)
            memset(buffer + allocated_len, 0, 512);
            allocated_len += 512;
        }

        // for each variant type -> save the executable representation of the atomic lec in the buffer 
        // and update the position in the buffer
        std::visit([&buffer, &pos_buffer](auto& arg) { arg.append_to_buffer(buffer, pos_buffer); }, al);

        pos_expr++;
    }

    long exec_lg = (long) pos_buffer;
    if(exec_lg == 0)
    {
        free(buffer);
        std::string error_msg = "CLEC constructor: Could not compile the LEC expression: ";
        error_msg += "'" + lec + "'.";
        kwarning((char*) error_msg.c_str());
        return false;
    }

    std::string log = "\t\t\t SAVE CLEC: ";
    for(int k = 0; k < exec_lg; k++)
        log += std::to_string((int) buffer[k]) + " ";
    logger.info(log);
    
    // NOTE: In binary files, compiled LEC are stored as a single memory block. 
    //       This is the old C idiom called the "struct hack". 
    //       C99 later made it official as the flexible array member ( LNAME lnames[]; ).
    //       In binary files, a CLEC_6_68 object will be saved as as:
    //       [ tot_lg | exec_lg | nb_names | dupendo | pad | lnames[0] ... lnames[nb_names-1] | exec bytecode (exec_lg bytes) ]
    //       <------------------------------------------- tot_lg bytes ------------------------------------------------------->

    int nb_names = (int) v_names.size();
    long tot_lg = sizeof(CLEC_6_68) + (nb_names - 1) * sizeof(LNAME) + exec_lg;
    // Allocates tot_lg bytes and fills them with nulls.
    // NOTE: the memory returned by operator new[] is suitably aligned
    //       for any fundamental type, hence for CLEC_6_68.
    char* fake_clec_bytes = new char[tot_lg];
    memset(fake_clec_bytes, 0, tot_lg);
    fake_clec = reinterpret_cast<CLEC_6_68*>(fake_clec_bytes);
    fake_clec->dupendo = this->duplicated_endo ? 1 : 0;
    fake_clec->tot_lg  = tot_lg;
    fake_clec->exec_lg  = exec_lg;
    // copy the coefficients and variables names
    fake_clec->nb_names = nb_names;
    LNAME* _names_ = fake_clec->lnames;
    for(int i = 0; i < nb_names; i++)
        strcpy(_names_[i].name, v_names[i].c_str());
    // copy the buffer
    char* tmp = (char*) fake_clec;
    memcpy(tmp + tot_lg - exec_lg, buffer, exec_lg);
    free(buffer);

    log = "\t\t\t SAVE CLEC STRUCT: (size = " + std::to_string(fake_clec->tot_lg) + ") ";
    for(int k = 0; k < fake_clec->tot_lg; k++)
        log += std::to_string((int) tmp[k]) + " ";
    logger.info(log);

    // Serializing
    *pack = (char*) P_add(*pack, (char*) fake_clec_bytes, fake_clec->tot_lg);

    delete[] fake_clec_bytes;
    return true;
}


/**
 * @brief Construct a new CLEC object
 * 
 * @param lec           std::string&  LEC expression to compile
 * @param side_of_eq    bool          if true, the LEC expression is the left or right side of an equation
 */
CLEC::CLEC(const std::string& lec, const bool side_of_eq) : AbstractCLEC()
{
    this->lec = lec;

    if(parser.open_string((char*) lec.c_str()) != 0) 
        throw std::runtime_error("Error opening LEC string");

    if(parse(side_of_eq) != 0)
        throw std::runtime_error("Error computing LEC expression " + lec);
    
    if(!side_of_eq)
        reorder_expression();

    parser.close();
}

/**
 * Compiles a LEC equation and tries to analytically solve the equation with respect to endo.
 * 
 * Generates a CLEC form with the result and set clec->duplicated_endo to 1 if the
 * generated form is of the form "0 := LHS - RHS")
*/
CLEC::CLEC(const std::string& eq, const std::string& endo) : AbstractCLEC()
{
    this->lec = eq;
    this->duplicated_endo = false;
    
    SLEC slec(eq, endo);
    slec.invert_equation(this->duplicated_endo, *this);
    slec.merge_names(*this);
    if(L_errno != 0) 
    {
        this->v_expression.clear();
        std::string error_msg = "Equation: cannot invert LEC expression '" + eq + "'\n";
        error_msg += "with respect to endogenous variable '" + endo + "' -> " + L_error();
        throw std::runtime_error(error_msg);
    }

    reorder_expression();
}

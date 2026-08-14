#pragma once
#include "api/pch.h"
#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
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
    std::shared_ptr<Scalar> scl = nullptr;
    char tcoef[128], ttest[128];
    for(const auto& [sname, _] : clec->map_objs) 
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

void CLEC::reorder_expression(std::vector<ATOMIC_LEC>& expr)
{
    if(expr.empty())
        throw std::invalid_argument("CLEC constructor: empty expression vector.");

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

            // compute the start and end positions of the arguments of the function 
            // NOTE: - 2 because the last element of v_expression is always (should be) 
            //       of type L_EOE (End Of Expression)
            int args_end = (int) this->v_expression.size() - 2;  
            int args_start = find_sub_expr_start(this->v_expression, args_end);
            if(args_start < 0 || args_end < args_start) 
            {
                std::string error_msg = "CLEC constructor: Could not determine the start and end ";
                error_msg += "positions of the arguments of function '" + al_tfn.representation + "'.";
                throw std::runtime_error(error_msg);
            }
            al_tfn.length_expr = args_end - args_start + 1;

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
                    error_msg += "positions of the arguments of function '" + al_mtfn.representation + "'.";
                    throw std::runtime_error(error_msg);
                }
                int length_expr = args_end - args_start + 1;
                // NOTE: push front because the arguments are processed in reverse order
                al_mtfn.v_length_expr.push_front(length_expr);
                args_end = args_start - 1;
            }

            // insert current LEC_MTFN element before its arguments in the vector
            this->v_expression.pop_back();
            this->v_expression.insert(this->v_expression.begin() + args_start, al);
        }
    }
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
        throw std::runtime_error("Error generating LEC expression");
    
    // copy the vector of atomic lec as is
    if(side_of_eq)
        this->v_expression = L_EXPR;
    else
        reorder_expression(L_EXPR);

    L_EXPR.clear();
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
    slec.invert_equation(this->duplicated_endo);
    slec.merge_names(*this);
    if(L_errno != 0) 
    {
        L_EXPR.clear();
        std::string error_msg = "Equation: cannot invert LEC expression '" + eq + "'\n";
        error_msg += "with respect to endogenous variable '" + endo + "' -> " + L_error();
        throw std::runtime_error(error_msg);
    }

    reorder_expression(L_EXPR);
    L_EXPR.clear();
}

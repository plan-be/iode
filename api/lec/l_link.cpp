#pragma once
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/lec/lec.h"
#include "api/objs/equations.h"


/**
 * First step of linking CLEC to KDBs: each variable and scalar name is searched in the KDB's
 * and their positions are saved in this CLEC->objs.
 *
 * If a name is not found, an error message is added to the stack via IodeErrorManager::append_error 
 * and L_errno is set to L_NOT_FOUND_ERR.
 *
 * @param [in]      dbv     KDB*    KDB of variables
 * @param [in]      dbs     KDB*    KDB of scalars
 * @return                  int     0 on success,  L_NOT_FOUND_ERR on error
*/
int CLEC::link_names(KDBVariablesPtr dbv, KDBScalarsPtr dbs)
{
    std::string name;
    int nb_vars = (int) this->v_var_names.size();
    int nb_coeffs = (int) this->v_coeff_names.size();

    std::shared_ptr<Variable> var_ptr;
    std::unordered_map<std::string, std::shared_ptr<Variable>> map_vars;
    for(int i = 0; i < nb_vars; i++)
    {
        name = this->v_var_names[i];

        try
        {
            var_ptr = dbv->get_obj_ptr(name);
        }
        catch(const std::invalid_argument&)
        {
            std::string msg = "linking LEC failed: Variable '" + name + "' not found";
            error_manager.append_error(msg);
            L_errno = L_NOT_FOUND_ERR;
            return L_errno;
        }

        if(!var_ptr)
        {
            std::string msg = "linking LEC failed: Variable '" + name + "' empty";
            error_manager.append_error(msg);
            L_errno = L_LINK_ERR;
            return L_errno;   
        }

        map_vars[name] = var_ptr;
    }
    
    std::shared_ptr<Scalar> scl_ptr;
    std::unordered_map<std::string, std::shared_ptr<Scalar>> map_coeffs;
    for(int i = 0; i < nb_coeffs; i++)
    {
        name = this->v_coeff_names[i];

        try
        {
            scl_ptr = dbs->get_obj_ptr(name);
        }
        catch(const std::invalid_argument&)
        {
            std::string msg = "linking LEC failed: Scalar '" + name + "' not found";
            error_manager.append_error(msg);
            L_errno = L_NOT_FOUND_ERR;
            return L_errno;
        }

        if(!scl_ptr)
        {
            std::string msg = "linking LEC failed: Scalar '" + name + "' empty";
            error_manager.append_error(msg);
            L_errno = L_LINK_ERR;
            return L_errno;   
        }

        map_coeffs[name] = scl_ptr;
    }

    for(ATOMIC_LEC& al : this->v_expression)
    {
        if(al->type == L_COEF)
        {
            LEC_COEF& al_coef = static_cast<LEC_COEF&>(*al);
            name = al_coef.name;
            al_coef.link(map_coeffs[name]);
        }
        else if(al->type == L_VAR || al->type == L_VART)
        {
            LEC_VAR& al_var = static_cast<LEC_VAR&>(*al);
            name = al_var.name;
            al_var.link(map_vars[name]);
        }
    }

    return 0;
}


/**
 * Links a CLEC *sub-expression* to the sample of a KDB. 
 * 
 * For example, assume dbv's sample is 2000Y1:2030Y1. 
 * In the LEC expression "A[2001Y1] + 2002Y1":
 *      - A[2001Y1] will be interpreted as the value of the 2d element of A
 *      - 2002Y1 will be replaced by 2 (2000Y1 == 0, 2001Y1 == 1,...)
 *
 * @param [in]      dbv     KDB*    KDB of variables
 */
void CLEC::link_sample(KDBVariablesPtr dbv, const int start, const int length)
{
    if(this->v_expression.size() == 0) 
        return;

    if(start < 0 || start >= this->v_expression.size())
    {
        std::string msg = "Invalid start position for linking CLEC sub-expression: " + std::to_string(start);
        kwarning(msg.c_str());
        return;
    }

    int end = start + length - 1;
    if(end >= this->v_expression.size())
    {
        std::string msg = "Invalid end position for linking CLEC sub-expression: " + std::to_string(end);
        kwarning(msg.c_str());
        return;
    }
    
    smpl_ptr = dbv->get_sample();
    if(!smpl_ptr)
    {
        std::string msg = "Cannot link a LEC sub-expression because the sample ";
        msg += "of the passed Variables database is empty";
        kwarning(msg.c_str());
        return;
    }

    for(int expr_pos = start; expr_pos <= end;)
    {
        ATOMIC_LEC& al = this->v_expression[expr_pos];
        expr_pos++;

        if(al->type == L_VAR || al->type == L_VART)
        {
            LEC_VAR& al_var = dynamic_cast<LEC_VAR&>(*al);
            // calculate the reference of the variable
            al_var.calculate_ref(*smpl_ptr);
        }
        else if(al->type == L_PERIOD)
        {
            LEC_PERIOD& al_period = dynamic_cast<LEC_PERIOD&>(*al);
            // calculate the position of the period in the sample
            al_period.link(*smpl_ptr);
        }
        else if(is_tfn(al->type))
        {
            LEC_TFN& al_tfn = dynamic_cast<LEC_TFN&>(*al);
            // recursive call to link_sample()
            int tfn_length = al_tfn.length_expr;
            if(tfn_length > 0)
            {
                this->link_sample(dbv, expr_pos, tfn_length);
                expr_pos += tfn_length;
            }
        }
        else if(is_mtfn(al->type))
        {
            LEC_MTFN& al_mtfn = dynamic_cast<LEC_MTFN&>(*al);
            // recursive call to link_sample()
            for(const int& mtfn_length : al_mtfn.v_length_expr)
            {
                this->link_sample(dbv, expr_pos, mtfn_length);
                expr_pos += mtfn_length;
            }
        }
    }
}


/**
 * Links this CLEC expression to KDB's of variables and of scalars. If some Period's are present in CLEC,
 * they are aligned to the Sample of dbv.
 * 
 * The CLEC, although modified by link(), can be relinked later with other KDB's. 
 *
 * @param [in]      dbv  KDB*        KDB of variables to link to CLEC
 * @param [in]      dbs  KDB*        KDB of scalars to link to CLEC
 * @return               int         0 on success, L_errno on error
*/
int CLEC::link(KDBVariablesPtr dbv, KDBScalarsPtr dbs)
{
    if(this->link_names(dbv, dbs)) 
        return L_errno;
    
    this->link_sample(dbv, 0, (int) this->v_expression.size());
    return 0;
}

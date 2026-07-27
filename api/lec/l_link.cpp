/**
 *  @header4iode
 *
 *  Functions to link CLEC variables and scalars to KDB's of variables and scalars.
 *  This process is required before computing the value of a CLEC instance.
 *
 *  The positions of variables and scalars returned by these functions will be used at execution time by L_getvar() and L_getscl().
 */
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
    for(auto& [name, pos] : this->objs) 
    {
        if(is_coefficient(name))
            pos = dbs->index_of(name);
        else
            pos = dbv->index_of(name);
        
        if(pos < 0) 
        {
            std::string msg = "linking LEC failed: '" + name + "' not found";
            error_manager.append_error(msg);
            L_errno = L_NOT_FOUND_ERR;
            return L_errno;
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
    
    std::shared_ptr<Sample> smpl_ptr = dbv->get_sample();
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

        if(std::holds_alternative<LEC_VAR>(al))
        {
            LEC_VAR& al_var = std::get<LEC_VAR>(al);
            // calculate the reference of the variable
            al_var.calculate_ref(*smpl_ptr);
        }
        else if(std::holds_alternative<LEC_PERIOD>(al))
        {
            LEC_PERIOD& al_period = std::get<LEC_PERIOD>(al);
            // calculate the position of the period in the sample
            al_period.calculate_pos(*smpl_ptr);
        }
        else if(std::holds_alternative<LEC_TFN>(al))
        {
            LEC_TFN& al_tfn = std::get<LEC_TFN>(al);
            // recursive call to link_sample()
            int tfn_length = al_tfn.length_expr;
            if(tfn_length > 0)
            {
                this->link_sample(dbv, expr_pos, tfn_length);
                expr_pos += tfn_length;
            }
        }
        else if(std::holds_alternative<LEC_MTFN>(al))
        {
            LEC_MTFN& al_mtfn = std::get<LEC_MTFN>(al);
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


/*---- PSEUDO LINKING FOR THE CONSTRUCTION OF THE MODEL SCC ----*/

/**
 * Pseudo linking used to calculate the strong connex components of a model (SCC).
 * 
 * The endogenous variables are assigned the position of their equation in dbe. 
 * This process constructs a sort of incidence matrix of the model by assigning to the position of the endogenous variables 
 * in the CLEC l_names of each equation (therefore, the KDB of vars and scalars are not needed here).
 * 
 * @param [in]      dbe     KDB*    KDB of equations
 * @param [in, out] clec    CLEC*   pointer to the CLEC to be linked
 */
void L_link_endos(const KDBEquationsPtr dbe, std::shared_ptr<CLEC>& clec)
{   
    for(auto& [name, pos] : clec->objs) 
    {
        if(is_coefficient(name))
            pos = 0;  // For the SCC construction, we do not need the coefficients (scalars)
        else
            pos = dbe->index_of(name);

        if(pos < 0)  // Not found => exogenous var
            pos = -1; // For the SCC construction, we do not need the exogenous vars positions 
    }
}

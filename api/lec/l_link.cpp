/**
 *  @header4iode
 *
 *  Functions to link CLEC variables and scalars to KDB's of variables and scalars.
 *  This process is required before computing the value of a CLEC instance.
 *
 *  Three functions, implemented in k_lec.c, are called during the link process: L_findvar(), L_findscl() L_getsmpl(). 
 *  The positions of variables and scalars returned by these functions will be used at execution time by L_getvar() and L_getscl().
 *   
 *  Main functions
 * 
 *      int L_link(KDBVariablesPtr dbv, KDBScalarsPtr dbs, CLEC* cl)    Links a CLEC expression to KDB's of variables and scalars. Aligns Period's to the Sample of dbv.
 *      void L_link_endos(const KDBEquationsPtr dbe, CLEC *cl)       Pseudo linking used to calculate the strong connex components of a model (SCC).
 */
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/lec/lec.h"
#include "api/objs/equations.h"


/**
 * First step of linking CLEC to KDBs: each variable and scalar name is searched in the KDB's
 * and their positions are saved in CLEC->objs.
 *
 * If a name is not found, an error message is added to the stack via IodeErrorManager::append_error 
 * and L_errno is set to L_NOT_FOUND_ERR.
 *
 * @param [in]      dbv     KDB*    KDB of variables
 * @param [in]      dbs     KDB*    KDB of scalars
 * @param [in, out] cl      CLEC*   Compiled LEC
 * @return                  int     0 on success,  L_NOT_FOUND_ERR on error
*/
static int L_link_names(KDBVariablesPtr dbv, KDBScalarsPtr dbs, CLEC* cl)
{
    for(auto& [name, pos] : cl->objs) 
    {
        if(is_coefficient(name))
            pos = L_findscl(dbs, name);
        else
            pos = L_findvar(dbv, name);
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
 * @param [in]      dbv     KDB*    KDB of variables (only its Sample is needed here)
 * @param [in, out] expr    char*   pointer to the CLEC (sub-)expression
 * @param [in]      lg      int     length of expr
*/
void L_link_sample_expr(KDBVariablesPtr dbv, unsigned char* expr, short lg)
{
    Sample* smpl = L_getsmpl(dbv);
    if(!smpl)
    {
        std::string msg = "Cannot link a LEC sub-expression because the sample ";
        msg += "of the passed Variables database is empty";
        kwarning(msg.c_str());
        return;
    }

    int j;
    int type;
    int previous_j;
    for(j = 0 ; j < lg ;) 
    {
        type = expr[j];
        switch(type) 
        {
            case L_VAR :
            {
                // extract LEC Variable from the buffer -> update j
                LEC_VAR al_var(expr, j);
                // calculate the reference of the variable
                al_var.calculate_ref(*smpl);
                // move back to the beginning of the LEC_VAR in the buffer
                j -= al_var.get_length();
                // save the updated LEC Variable back to the buffer
                al_var.add_to_buffer(expr, j);
                break;
            }
            case L_COEF :
            {
                // extract LEC Coefficient from the buffer -> update j
                LEC_COEF al_coef(expr, j);
                break;
            }
            case L_DCONST :
            {
                // extract LEC Double Constant from the buffer -> update j
                LEC_CONST_REAL al_real(expr, j);
                break;
            }
            case L_LCONST :
            {
                // extract LEC Long Constant from the buffer -> update j
                LEC_CONST_LONG al_long(expr, j);
                break;
            }
            case L_PERIOD :
            {
                // extract LEC Period from the buffer -> update j
                LEC_PERIOD al_period(expr, j);
                // calculate the position of the period in the sample
                al_period.calculate_pos(*smpl);
                // move back to the beginning of the LEC_PERIOD in the buffer
                j -= al_period.get_length();
                // save the updated LEC Period back to the buffer
                al_period.add_to_buffer(expr, j);
                break;
            }
            default :
            {
                if(is_special_lec_elem(type)) 
                {
                    j++;
                    break; 
                }

                if(is_fn(type)) 
                {
                    // extract LEC Function from the buffer -> update j
                    LEC_FN al_fn(expr, j);
                    break; 
                }

                if(is_op(type)) 
                {
                    // extract LEC Operator from the buffer -> update j
                    LEC_OP al_op(expr, j);
                    break; 
                }

                if(is_tfn(type)) 
                {
                    previous_j = j;
                    // extract LEC TFN from the buffer -> update j
                    LEC_TFN al_tfn(expr, j);
                    // recursive call to L_link_sample_expr()
                    al_tfn.link_sample_expr(dbv, expr, previous_j);
                    break;
                }

                if(is_val(type)) 
                {
                    // extract LEC VAL_FN from the buffer -> update j
                    LEC_VAL_FN al_val_fn(expr, j);
                    break; 
                }

                if(is_mtfn(type)) 
                {
                    // extract LEC TFN from the buffer -> update j
                    LEC_MTFN al_mtfn(expr, j);
                    // TODO : find a way to link sample in multi-arg functions
                    // ...
                    break;
                }

                throw std::runtime_error("Unexpected type in L_link_sample_expr: " + std::to_string(type));
            }
        }
    }
}


/**
 * Second step of linking CLEC. Each time displacement in the CLEC struct is aligned to the dbv's Sample.
 * For example, the position of A[1970Y1] in the vector A depends on the sample of dbv.
 *
 * @param [in]      dbv     KDB*    KDB of variables
 * @param [in, out] cl      CLEC*   Compiled LEC
 */
static void L_link_sample(KDBVariablesPtr dbv, CLEC* cl)
{
    if(!cl) 
        return;

    L_link_sample_expr(dbv, cl->expression, (short) cl->len_expr);
    return;
}


/**
 * Links a CLEC expression to KDB's of variables and of scalars. If some Period's are present in CLEC,
 * they are aligned to the Sample of dbv.
 * 
 * The CLEC, although modified by L_link(), can be relinked later with other KDB's. 
 *
 * @param [in]      dbv  KDB*        KDB of variables to link to CLEC
 * @param [in]      dbs  KDB*        KDB of scalars to link to CLEC
 * @param [in, out] cl   CLEC*       CLEC expression whose content must be linked to dbv and dbs
 * @return               int         0 on success, L_errno on error
*/
int L_link(KDBVariablesPtr dbv, KDBScalarsPtr dbs, CLEC* cl)
{
    if(!cl) 
        return 0;
    
    if(L_link_names(dbv, dbs, cl)) 
        return L_errno;
    
    L_link_sample(dbv, cl);
    return 0;
}


/*---- PSEUDO LINKING FOR THE CONSTRUCTION OF THE MODEL SCC ----*/

/**
 * For each variable in CLEC (v_names) that exists as equation in dbe, set its position (v_names.pos) to the position in dbe.
 *
 * @param [in]      dbe     KDB*     KDB of equations
 * @param [in, out] cl      CLEC*    Compiled lec
 *
 */
static void L_link1_endos(const KDBEquationsPtr dbe, CLEC* cl)
{
    for(auto& [name, pos] : cl->objs) 
    {
        if(is_coefficient(name))
            pos = 0;  // For the SCC construction, we do not need the coefficients (scalars)
        else
            pos = dbe->index_of(name);

        if(pos < 0)  // Not found => exogenous var
            pos = -1; // For the SCC construction, we do not need the exogenous vars positions 
    }
}

/**
 * Pseudo linking used to calculate the strong connex components of a model (SCC).
 * 
 * The endogenous variables are assigned the position of their equation in dbe. 
 * This process constructs a sort of incidence matrix of the model by assigning to the position of the endogenous variables 
 * in the CLEC l_names of each equation (therefore, the KDB of vars and scalars are not needed here).
 *  
 * @param [in]      dbe     KDB*    KDB of equations
 * @param [in, out] cl      CLEC*   compiled LEC expression
 * @return                  int     0 on success, L_errno on error
 * 
 * @author JMP 16/3/2012
 */
void L_link_endos(const KDBEquationsPtr dbe, CLEC *cl)
{
    if (cl == NULL) 
        return;
    
    L_link1_endos(dbe, cl);
}

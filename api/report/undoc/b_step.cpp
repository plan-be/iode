/**
 *  @header4iode
 * 
 *  Report function that estimates a block of equations and finds the best possible tests 
 *  for all possible combinations of coefficients.
 *  
 *  List of functions 
 *  -----------------
 *      int B_EqsStepWise(char* arg, int unused) | $EqsStepWise from to eqname leccond {r2|fstat}
 */
#include "api/pch.h"
#include "api/b_args.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/estimation/estimation.h"
#include "api/report/commands/commands.h"
#include "api/report/undoc/undoc.h"


/**
 *  Analyses an equation and checks that all variables exist in global_ws_var. 
 *  Initializes to 0.9 the scalars that are not yet defined in global_ws_scl.
 *  
 *  Sub-function of B_EqsStepWise().
 *  
 *  @param [in] string   eqs    equation name
 *  @return     int             1 on success, -1 if some variable present in eqs does not exist in global_ws_var
 */
static int check_scl_var(const std::string& name)
{
    if(!global_ws_eqs->contains(name)) 
        return -1;
    
    std::shared_ptr<Equation> eq_ptr = global_ws_eqs->get_obj_ptr(name) ;
    if(!eq_ptr) 
        return -1;
    
    char buf[1024];
    std::shared_ptr<CLEC> cl = eq_ptr->clec;
    for(const std::string& cl_name : cl->v_obj_names) 
    {
        if(is_coefficient(cl_name)) 
        {
            // create scalar with default value 0.9 if not existing
            if(!global_ws_scl->contains(cl_name))
            {
                sprintf(buf, "%s 0.9 1", cl_name.c_str());
                B_DataUpdate(buf, SCALARS);
            }
        }
        else 
        {
            if(!global_ws_var->contains(cl_name))
            {
                kerror(0, "Var %s from %s not found", cl_name.c_str(), name.c_str());
                return -1;
            }
        }
    }
    
    return 1;
}


/**
 *  This function estimates a block of equations and finds the best possible tests for all possible 
 *  combinations of coefficients.
 *  
 *  $EqsStepWise from to eqname leccond {r2|fstat}
 *       from to : estimation period
 *       eqname  : equation to estimate
 *       leccond : condition of eligibility
 */
int B_EqsStepWise(char* arg, int unused)                                                 
{
    std::vector<std::string> v_args = split_multi_quoted(std::string(arg), " ,\n\t", '"');
    if(v_args.empty()) 
        return 1;

    if(v_args.size() != 5)
    {
        std::string error_msg = "EqsStepWise: expected 5 arguments but got ";
        error_msg += std::to_string(v_args.size()) + " arguments instead.";
        error_manager.append_error(error_msg);
        return 1;
    }

    std::shared_ptr<Sample> smpl = nullptr;
    std::string from = v_args[0];                                              
    std::string to = v_args[1];
    try
    {
        smpl = std::make_shared<Sample>(from, to);
    }
    catch(const std::exception& e)
    {   
        kerror(0, e.what());
        return 1;
    }

    std::string eq_name = v_args[2]; 
    eq_name = global_ws_eqs->to_key(eq_name);                                         
    if(!global_ws_eqs->contains(eq_name)) 
    {                            
        kerror(0, "Eqs %s not found", eq_name.c_str());
        return 1;
    }

    std::string cond = v_args[3]; 
    double value = C_evallec((char*) cond.c_str(), 0); 
    // manage errors from the lec condition                                   
    if(int(value) == -1)
        return 1;

    std::string test = v_args[4];
    // manage errors from the r2 and fstat tests
    if(test != "r2" && test != "fstat")
    {
        std::string error_msg = "Incorrect test name '" + test + "'. ";
        error_msg += "Expected 'r2' or 'fstat'.";
        kerror(0, (char*) error_msg.c_str());
        return 1;
    }

    int res = check_scl_var(eq_name);
    // case where some scalars and/or variables declared in the equation 
    // are not present in the global workspaces
    if(res == -1)
        return 1;                      

    estimate_step_wise(smpl, (char*) eq_name.c_str(), (char*) cond.c_str(), (char*) test.c_str());
    
    return 0;
}

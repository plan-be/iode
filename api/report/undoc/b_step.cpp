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
#include "api/constants.h"
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
 *  @param [in] char*   eqs     equation name
 *  @return     int             1 on success, -1 if some variable present in eqs does not exist in global_ws_var
 */
static int check_scl_var(char *eqs)
{
    char buf[1024];
    std::string name = std::string(eqs);

    if(!global_ws_eqs->contains(name)) 
        return -1;
    
    std::shared_ptr<Equation> eq_ptr = global_ws_eqs->get_obj_ptr(name) ;
    if(!eq_ptr) 
        return -1;

    std::shared_ptr<CLEC> cl = eq_ptr->clec;
    for(auto& [cl_name, _]: cl->objs) 
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
                kerror(0, "Var %s from %s not found", cl_name.c_str(), eqs);
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
    char** args = B_vtom_chk(arg, 5);
    if(args == NULL) 
        return 1;

    std::shared_ptr<Sample> smpl = nullptr;
    std::string from = std::string(args[0]);                                              
    std::string to = std::string(args[1]);
    try
    {
        /*Calcule le sample*/
        smpl = std::make_shared<Sample>(from, to);
    }
    catch(const std::exception& e)
    {   
        /*Gère les erreurs de sample */                                    
        SCR_free_tbl((unsigned char**) args);
        kerror(0,e.what());
        return(1);
    }

    char* c_eq_name = args[2];
    std::string eq_name = std::string(c_eq_name);                                               
    if(!global_ws_eqs->contains(eq_name)) 
    {                            
        kerror(0,"Eqs %s not found", c_eq_name);
        SCR_free_tbl((unsigned char**) args);
        return 1;
    }

    char* cond = args[3]; 
    double value = C_evallec(cond, 0); 
    // manage errors from the lec condition                                   
    if(int(value) == -1)
    {
        SCR_free_tbl((unsigned char**) args);
        return(1);                          
    }

    char* tmp = args[4];
    char* test = (char*) SCR_stracpy(SCR_lower((unsigned char*) tmp));
    // manage errors from the r2 and fstat tests
    if(strcmp(test,"r2")!=0 && strcmp(test,"fstat")!=0)
    {
        kerror(0,"Incorrect test name");
        SCR_free_tbl((unsigned char**) args);
        return(1);
    }

    int res = check_scl_var(c_eq_name);
    // case where some scalars and/or variables declared in the equation 
    // are not present in the global workspaces
    if(res == -1)
    {
        SCR_free_tbl((unsigned char**) args);
        return(1);                      
    }

    estimate_step_wise(smpl, c_eq_name, cond, test);            /* Effectue les estimations */
    
    SCR_free_tbl((unsigned char**) args);
    return 0;
}

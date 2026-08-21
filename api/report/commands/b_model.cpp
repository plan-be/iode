/**
 *  @header4iode
 * 
 *  Report functions related to model simulations. 
 *  
 *  See b_data.c for more details on the report function syntax and logic.
 *  
 *  List of functions 
 *  -----------------
 *      int B_ModelSimulate(char *arg)                              $ModelSimulate per_from per_to equation_list
 *      int B_ModelSimulateParms(char* arg, int unused)                         $ModelSimulateParms eps relax maxit {Connex | Triang | None } 0 - 4 (starting values) {Yes | no } nbtri {yes | No } 
 *      int B_ModelExchange(char* arg, int unused)                              $ModelExchange eqname1-varname1,eqname2-varname2,...
 *      int KE_compile(KDBEquations& dbe)                                    Recompiles a KDB of equations. Tests and other informations saved in the equation object are left unchanged.
 *      int B_ModelCompile(char* arg, int unused)                               $ModelCompile  [eqname1, eqname2, ... ]
 *      int B_ModelCalcSCC(char *arg)                               $ModelCalcSCC nbtris prename intername postname [eqs]
 *      int B_ModelSimulateSCC(char *arg)                           $ModelSimulateSCC from to pre inter post
 *      int B_ModelSimulateSaveNIters(char *arg)                    $ModelSimulateSaveNiters varname
 *      int B_ModelSimulateSaveNorms(char *arg)                     $ModelSimulateSaveNorms varname
 */
#include "api/pch.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/equations.h"
#include "api/objs/lists.h"
#include "api/objs/variables.h"
#include "api/simulation/simulation.h"

#include "api/report/commands/commands.h"


/* Model Simulation */

/**
 *  Simulates a model (i.e. a system of equations). If eqs is NULL or empty, uses the full global_ws_eqs as 
 *  system of equations.
 *  
 *  Sub-function of B_ModelSimulate().
 *  
 *  @param [in] Sample* smpl    simulation Sample
 *  @param [in] char**  eqs     NULL or list of equations defining the model 
 *  @return     int             0 on success, return code of simulate() on error.    
 */
static int B_ModelSimulateEqs(Sample* smpl, char** c_eqs)
{
    std::string s_eqs;
    std::vector<std::string> v_eqs;
    if(c_eqs != NULL) 
    {
        int nb_eqs = SCR_tbl_size((unsigned char**) c_eqs);
        v_eqs.reserve(nb_eqs);

        std::string eq_name;
        for(int i = 0; i < nb_eqs; i++)
        {
            eq_name = std::string(c_eqs[i]);
            v_eqs.push_back(eq_name);
            s_eqs += eq_name + ";";
        } 
    }

    bool success;
    if(v_eqs.size() == 0)
        success = global_simu->simulate(global_ws_eqs, global_ws_var, global_ws_scl, smpl);
    else 
    {
        KDBEquationsPtr tdbe = global_ws_eqs->get_subset(s_eqs, false);
        if(tdbe->size() > 0)
            success = global_simu->simulate(tdbe, global_ws_var, global_ws_scl, smpl, v_eqs);
    }

    return success ? 0 : -1;
}


/**
 *  Syntax: $ModelSimulate per_from per_to equation_list
 *  
 *
 */
int B_ModelSimulate(char *const_arg, int unused)
{
    int     lg1, lg2;
    int     rc = -1;
    char    from[16], to[16], **eqs = 0;
    Sample  *smpl = nullptr;
    char    *arg;

    // Copy for C++ strings = read only (const)
    arg = (char*) SCR_stracpy((unsigned char*) const_arg);
    
    lg1 = B_get_arg0(from, arg, 15);
    lg2 = B_get_arg0(to, arg + lg1, 15);
    try
    {
        smpl = new Sample(std::string((char*) from), std::string((char*) to));
    }
    catch(const std::exception& e)
    {
        std::string error_msg = "ModelSimulate: invalid sample\n" + std::string(e.what()); 
        error_manager.append_error(error_msg);
        goto err;
    }

    eqs = B_ainit_chk(arg + lg1 + lg2, NULL, 0);
    if(eqs == 0) 
        goto err;
    
    rc = B_ModelSimulateEqs(smpl, eqs);

err:
    if(smpl) delete smpl;
    smpl = nullptr;
    SCR_free_tbl((unsigned char**) eqs);
    SCR_free(arg);
    return rc;
}


/**
 *  Defines simulation parameters.
 *  
 *  Syntax: $ModelSimulateParms eps relax maxit {Connex | Triang | None } 0 - 4 (starting values) {Yes | no } nbtri {yes | No } 
 *          where:
 *              eps  := convergence threshold
 *              0.1 <= relax <= 1.0
 *          
 *
 */
int B_ModelSimulateParms(char* arg, int unused)
{
    char    **args;
    int     rc = 0, nargs;

    args = (char **) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
    nargs = SCR_tbl_size((unsigned char**) args);
    if(nargs < 6) {
        error_manager.append_error("ModelSimulateParms: incorrect number of parameters");
        rc = -1;
        goto fin;
    }

    global_simu->epsilon = atof(args[0]);
    global_simu->relax = atof(args[1]);
    global_simu->max_iter = atoi(args[2]);
    if(U_is_in(args[3][0], "bB")) 
        args[3][0] = 'T'; // JMP 20/02/2023 
    global_simu->sorting_algo = B_argpos("CTN", args[3][0]);
    global_simu->init_algo = args[4][0] - '0';
    global_simu->debug = B_argpos("NY", args[5][0]);
    if(nargs > 6) global_simu->nb_passes = atoi(args[6]); //JMP 14/3/2012

    if(nargs > 7)
        newton_debug = B_argpos("NY", args[7][0]); // JMP 6/3/2012
    else
        newton_debug = 0;

fin :
    SCR_free_tbl((unsigned char**) args);
    return rc;
}


/**
 *  Defines goalSeeking parameters.
 *   
 *  Syntax: $ModelExchange eqname1-varname1,eqname2-varname2,...
 *           where eqname1, eqname2 are equation names (thus also endogenous vars)
 *                 varname1, varname2 are exogenous variables
 *  
 *
 */
int B_ModelExchange(char* const_arg, int unused)
{
    // Copy for C++ strings = read only (const)
    char* arg = (char*) SCR_stracpy((unsigned char*) const_arg);
    
    global_simu->v_endo_exo.clear();
    if(arg && SCR_strip((unsigned char*) arg)[0])
    {
        char** lst = B_ainit_chk(arg, NULL, 0);
        if(lst != NULL)
        {
            int nb = SCR_tbl_size((unsigned char**) lst);
            for(int i = 0; i < nb; i++)
                global_simu->v_endo_exo.push_back(std::string(lst[i]));
            SCR_free_tbl((unsigned char**) lst);
        }
    } 
    
    SCR_free(arg);
    return 0;
}


/**
 *  Syntax: $ModelCompile [eqname1, eqname2, ... ]
 *  
 *
 */
int B_ModelCompile(char* arg, int unused)
{
    int rc = -1;

    /* EndoExo whole WS */
    if(arg == NULL || arg[0] == 0) 
        rc = KE_compile(*global_ws_eqs);
    else 
    {
        std::string eqs = std::string(arg);
        if(eqs.empty())
            rc = KE_compile(*global_ws_eqs);
        else 
        {
            KDBEquationsPtr tdbe = global_ws_eqs->get_subset(eqs, false);
            if(tdbe->size() > 0)
                rc = KE_compile(*tdbe);
        }
    }

    return rc;
}


/**
 *  Syntax: $ModelCalcSCC nbtris prename intername postname [eqs]
 */
int B_ModelCalcSCC(char *const_arg, int unused)
{
    // Copy for C++ strings = read only (const)
    char* arg = (char*) SCR_stracpy((unsigned char*) const_arg);
    
    // Tri
    char buf[256];
    int lg1 = B_get_arg0(buf, arg, 15);
    int tris = atoi(buf); 
    if(tris < 0) 
        tris = 0;

    // result list names
    char pre[64], inter[64], post[64];
    lg1 += B_get_arg0(pre,   arg + lg1, 20);
    lg1 += B_get_arg0(inter, arg + lg1, 20);
    lg1 += B_get_arg0(post,  arg + lg1, 20);
    if(strlen(pre) == 0 || strlen(inter) == 0 || strlen(post) == 0)
    {
        SCR_free(arg);
        return -1;
    } 

    KDBEquationsPtr tdbe = nullptr;
    std::string list_eqs = std::string(arg + lg1);
    list_eqs = trim(list_eqs);
    if(list_eqs.empty())
        tdbe = global_ws_eqs;
    else
        tdbe = global_ws_eqs->get_subset(list_eqs, false);

    bool success = global_simu->calculate_SCC(tdbe, tris, pre, inter, post);
    return success ? 0 : -1;
}


/**
 *  Simulates a model based on 3 precalculated lists pre, post and inter.
 *  
 *  Syntax: $ModelSimulateSCC from to pre inter post
 */
int B_ModelSimulateSCC(char *const_arg, int unused)
{
    // Copy for C++ strings = read only (const)
    char* arg = (char*) SCR_stracpy((unsigned char*) const_arg);

    Sample* smpl = nullptr;
    char from[16], to[16];
    int lg1 = B_get_arg0(from, arg, 15);
    int lg2 = B_get_arg0(to, arg + lg1, 15);
    try
    {
        smpl = new Sample(std::string((char*) from), std::string((char*) to));
    }
    catch(const std::exception& e)
    {
        std::string error_msg = "ModelSimulateSCC: invalid sample\n" + std::string(e.what());
        error_manager.append_error(error_msg);
        SCR_free(arg);
        return -1;
    }

    // Extrait les listes restantes
    char** lsts = B_ainit_chk(arg + lg1 + lg2, NULL, 0);
    if(lsts == 0 || SCR_tbl_size((unsigned char**) lsts) != 3) 
    {
        error_manager.append_error("ModelSimulateSCC: syntax error in lists");
        SCR_free(arg);
        SCR_free_tbl((unsigned char**) lsts);
        if(smpl) delete smpl;
        return -1;
    }

    if(!(global_ws_lst->contains(lsts[0]) && global_ws_lst->contains(lsts[1]) && global_ws_lst->contains(lsts[2]))) 
    {
        error_manager.append_error("ModelSimulateSCC: pre, post or inter list not found in the Lists workspace");
        SCR_free(arg);
        SCR_free_tbl((unsigned char**) lsts);
        if(smpl) delete smpl;
        return -1;
    }

    int nb;

    std::shared_ptr<List> pre_lst = global_ws_lst->get_obj_ptr(lsts[0]);
    char** c_pre = (char**) KL_expand((char*) pre_lst->c_str());
    // convert to std::vector<std::string>
    std::vector<std::string> pre;
    nb = SCR_tbl_size((unsigned char**) c_pre);
    for(int i = 0; i < nb; i++) 
        pre.push_back(std::string(c_pre[i]));
    SCR_free_tbl((unsigned char**) c_pre);

    std::shared_ptr<List> inter_lst = global_ws_lst->get_obj_ptr(lsts[1]);
    char** c_inter = (char**) KL_expand((char*) inter_lst->c_str());
    // convert to std::vector<std::string>
    std::vector<std::string> inter;
    nb = SCR_tbl_size((unsigned char**) c_inter);
    for(int i = 0; i < nb; i++) 
        inter.push_back(std::string(c_inter[i]));
    SCR_free_tbl((unsigned char**) c_inter);

    std::shared_ptr<List> post_lst = global_ws_lst->get_obj_ptr(lsts[2]);
    char** c_post = (char**) KL_expand((char*) post_lst->c_str());
    // convert to std::vector<std::string>
    std::vector<std::string> post;
    nb = SCR_tbl_size((unsigned char**) c_post);
    for(int i = 0; i < nb; i++) 
        post.push_back(std::string(c_post[i]));
    SCR_free_tbl((unsigned char**) c_post);

    SCR_free_tbl((unsigned char**) lsts);

    // union of pre, inter and post -> to create a subset of global_ws_eqs to simulate
    std::set<std::string> v_eqs;
    v_eqs.insert(pre.begin(), pre.end());
    v_eqs.insert(inter.begin(), inter.end());
    v_eqs.insert(post.begin(), post.end());
    std::string list_eqs;
    for(const auto& eq : v_eqs) 
        list_eqs += eq + ";";
    KDBEquationsPtr tdbe = global_ws_eqs->get_subset(list_eqs, false);

    // Run the simulation
    bool success = global_simu->simulate_SCC(tdbe, global_ws_var, global_ws_scl, smpl, pre, inter, post);

    return success ? 0 : -1;
}


/**
 *  Set all values of the variable name to NaN. The variable is created if it does not exist.
 *  
 *  @param [in] char*   name    var name
 *  @return     int             0 always    
 */
static int B_CreateEmptyVar(char *name)
{
    char        uname[1024], buf[4096];

    // Create var
    strcpy(uname, name);
    SCR_sqz(SCR_upper((unsigned char*) uname));
    sprintf(buf, "%s 1/0", uname);
    B_DataCalcVar(buf);
    return 0;
}


/**
 *  Retrieves the pointer to the first value of a variable data.
 *  
 *  @param [in] char*       name    variable name
 *  @return     double*          pointer to the var "name"
 */
static double *B_GetVarPtr(char* c_name)
{
    std::string name = std::string(c_name);
    name = trim(name);
    name = to_upper(name);
    if(!global_ws_var->contains(name))
        return NULL;
    else
        return global_ws_var->get_var_ptr(name);
}


/**
 *  Saves the values of the vector of double vec in the variable name. 
 *  Creates name if it does not exist.
 *  
 *  @param [in, out] char*       name   variable name
 *  @param [in]      double*  vec    pointer to the values to copy in name
 */
static int B_CreateVarFromVecOfDoubles(char *name, double *vec)
{
    if(!global_ws_var)
        return -1;

    // Create var and get Ptr
    B_CreateEmptyVar(name);
    double* x = B_GetVarPtr(name);
    if(x == 0) 
    {
        std::string error_msg = "B_CreateVarFromVecOfDoubles: failed to create the variable '";
        error_msg += std::string(name) + "'";
        error_manager.append_error(error_msg);
        return -1;
    }

    // Copy values
    if(vec) 
    {
        for(int t = 0; t < global_ws_var->get_sample()->nb_periods; t++)
            x[t] = vec[t];
    }
    return 0;
}

/**
 *  Saves into the variable "name" the values of the vector vec of integers.
 *  Creates name if it does not exist.
 *  
 *  @param [in, out] char*      name   variable name
 *  @param [in]      int*       vec    pointer to the values to copy in name
 */
static int B_CreateVarFromVecOfInts(char *name, int *vec)
{
    if(!global_ws_var)
        return -1;

    // Create var and get Ptr
    B_CreateEmptyVar(name);
    double* x = B_GetVarPtr(name);
    if(x == 0) 
    {
        std::string error_msg = "B_CreateVarFromVecOfInts: failed to create the variable '";
        error_msg += std::string(name) + "'";
        error_manager.append_error(error_msg);
        return -1;
    }

    // Copy values
    if(vec) 
    {
        for(int t = 0; t < global_ws_var->get_sample()->nb_periods; t++)
            x[t] = vec[t];
    }
    return 0;
}


/**
 *  Saves the number of iterations needed to complete each simulated period in a variable.
 *  
 *  Syntax: $ModelSimulateSaveNiters varname
 *  
 *
 */
int B_ModelSimulateSaveNIters(char *arg, int unused)
{
    return(B_CreateVarFromVecOfInts(arg, global_simu->v_nb_iterations.empty() ? NULL : global_simu->v_nb_iterations.data()));
}


/**
 *  Saves the convergence threshold reached to complete each simulated period in a variable.
 *  
 *  Syntax: $ModelSimulateSaveNorms varname
 *  
 *
 */
int B_ModelSimulateSaveNorms(char *arg, int unused)
{
    return(B_CreateVarFromVecOfDoubles(arg, global_simu->v_norm.empty() ? NULL : global_simu->v_norm.data()));
}

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
 *      int B_ModelSimulateParms(const std::string& arg, int unused)                         $ModelSimulateParms eps relax maxit {Connex | Triang | None } 0 - 4 (starting values) {Yes | no } nbtri {yes | No } 
 *      int B_ModelExchange(const std::string& arg, int unused)                              $ModelExchange eqname1-varname1,eqname2-varname2,...
 *      int KE_compile(KDBEquations& dbe)                                    Recompiles a KDB of equations. Tests and other informations saved in the equation object are left unchanged.
 *      int B_ModelCompile(const std::string& arg, int unused)                               $ModelCompile  [eqname1, eqname2, ... ]
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
static int B_ModelSimulateEqs(Sample* smpl, const std::vector<std::string>& v_eqs)
{
    std::string s_eqs;
    for(const std::string& eq_name : v_eqs)
        s_eqs += eq_name + ";";

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
int B_ModelSimulate(const std::string& arg, int unused)
{
    int     rc = -1;
    std::string from, to;
    Sample  *smpl = nullptr;
    std::vector<std::string> v_eqs;

    // Copy (get_next_arg() modifies its argument)
    std::string args(arg);

    from = get_next_arg(args, 15);
    to = get_next_arg(args, 15);
    try
    {
        smpl = new Sample(from, to);
    }
    catch(const std::exception& e)
    {
        std::string error_msg = "ModelSimulate: invalid sample\n" + std::string(e.what()); 
        error_manager.append_error(error_msg);
        goto err;
    }

    v_eqs = expand_args(args, 0);
    rc = B_ModelSimulateEqs(smpl, v_eqs);

err:
    if(smpl) delete smpl;
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
int B_ModelSimulateParms(const std::string& arg, int unused)
{
    std::vector<std::string> args = split_skip_empty(std::string(arg), " ,\n\t");
    int nargs = (int) args.size();
    if(nargs < 6) {
        error_manager.append_error("ModelSimulateParms: incorrect number of parameters");
        return -1;
    }

    global_simu->epsilon = atof(args[0].c_str());
    global_simu->relax = atof(args[1].c_str());
    global_simu->max_iter = atoi(args[2].c_str());
    if(U_is_in(args[3][0], "bB"))
        args[3][0] = 'T'; // JMP 20/02/2023
    global_simu->sorting_algo = B_argpos("CTN", args[3][0]);
    global_simu->init_algo = args[4][0] - '0';
    global_simu->debug = B_argpos("NY", args[5][0]);
    if(nargs > 6) global_simu->nb_passes = atoi(args[6].c_str()); //JMP 14/3/2012

    if(nargs > 7)
        newton_debug = B_argpos("NY", args[7][0]); // JMP 6/3/2012
    else
        newton_debug = 0;

    return 0;
}


/**
 *  Defines goalSeeking parameters.
 *   
 *  Syntax: $ModelExchange eqname1-varname1,eqname2-varname2,...
 *           where eqname1, eqname2 are equation names (thus also endogenous vars)
 *                 varname1, varname2 are exogenous variables
 */
int B_ModelExchange(const std::string& arg, int unused)
{
    std::string list_exo = arg;
    global_simu->set_exchange(list_exo);
    return 0;
}


/**
 *  Syntax: $ModelCompile [eqname1, eqname2, ... ]
 *  
 *
 */
int B_ModelCompile(const std::string& arg, int unused)
{
    int rc = -1;

    /* EndoExo whole WS */
    if(arg.empty() || arg[0] == 0)
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
int B_ModelCalcSCC(const std::string& arg, int unused)
{
    // Copy (get_next_arg() modifies its argument)
    std::string args(arg);

    // Tri
    std::string buf = get_next_arg(args, 15);
    int tris = atoi(buf.c_str());
    if(tris < 0)
        tris = 0;

    // result list names
    std::string pre = get_next_arg(args, 20);
    std::string inter = get_next_arg(args, 20);
    std::string post = get_next_arg(args, 20);
    if(pre.empty() || inter.empty() || post.empty())
        return -1;

    KDBEquationsPtr tdbe = nullptr;
    std::string list_eqs = args;
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
int B_ModelSimulateSCC(const std::string& arg, int unused)
{
    // Copy (get_next_arg() modifies its argument)
    std::string args(arg);

    Sample* smpl = nullptr;
    std::string from = get_next_arg(args, 15);
    std::string to = get_next_arg(args, 15);
    try
    {
        smpl = new Sample(from, to);
    }
    catch(const std::exception& e)
    {
        std::string error_msg = "ModelSimulateSCC: invalid sample\n" + std::string(e.what());
        error_manager.append_error(error_msg);
        return -1;
    }

    // Extrait les listes restantes
    std::vector<std::string> v_lst = expand_args(args, 0);
    if(v_lst.size() != 3)
    {
        error_manager.append_error("ModelSimulateSCC: syntax error in lists");
        if(smpl) delete smpl;
        return -1;
    }

    std::string pre_list_name = global_ws_lst->to_key(v_lst[0]);
    std::string inter_list_name = global_ws_lst->to_key(v_lst[1]);
    std::string post_list_name = global_ws_lst->to_key(v_lst[2]);
    if(!(global_ws_lst->contains(pre_list_name) && global_ws_lst->contains(inter_list_name) 
        && global_ws_lst->contains(post_list_name))) 
    {
        error_manager.append_error("ModelSimulateSCC: pre, post or inter list not found in the Lists workspace");
        if(smpl) delete smpl;
        return -1;
    }

    std::shared_ptr<List> pre_lst = global_ws_lst->get_obj_ptr(pre_list_name);
    std::vector<std::string> pre = global_ws_lst->expand_lists(*pre_lst);

    std::shared_ptr<List> inter_lst = global_ws_lst->get_obj_ptr(inter_list_name);
    std::vector<std::string> inter = global_ws_lst->expand_lists(*inter_lst);

    std::shared_ptr<List> post_lst = global_ws_lst->get_obj_ptr(post_list_name);
    std::vector<std::string> post = global_ws_lst->expand_lists(*post_lst);

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
 *  @param [in] const std::string&  name    var name
 *  @return     int                         0 always
 */
static int B_CreateEmptyVar(const std::string& name)
{
    // Create var: name in uppercase (CP850 aware), without spaces
    std::string uname = name;
    SCR_upper((unsigned char*) uname.data());
    uname.erase(std::remove(uname.begin(), uname.end(), ' '), uname.end());
    B_DataCalcVar(uname + " 1/0");
    return 0;
}


/**
 *  Retrieves the pointer to the first value of a variable data.
 *  
 *  @param [in] const std::string&  var_name    variable name
 *  @return     double*                         pointer to the var "name"
 */
static double *B_GetVarPtr(const std::string& var_name)
{
    std::string name = global_ws_var->to_key(var_name);
    if(!global_ws_var->contains(name))
        return NULL;
    else
        return global_ws_var->get_var_ptr(name);
}


/**
 *  Saves the values of the vector of double vec in the variable name. 
 *  Creates name if it does not exist.
 *  
 *  @param [in, out] const std::string&  name   variable name
 *  @param [in]      double*             vec    pointer to the values to copy in name
 */
static int B_CreateVarFromVecOfDoubles(const std::string& name, double *vec)
{
    if(!global_ws_var)
        return -1;

    // Create var and get Ptr
    B_CreateEmptyVar(name);
    double* x = B_GetVarPtr(name);
    if(x == 0) 
    {
        std::string error_msg = "B_CreateVarFromVecOfDoubles: failed to create the variable '";
        error_msg += name + "'";
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
 *  @param [in, out] const std::string&  name   variable name
 *  @param [in]      int*                vec    pointer to the values to copy in name
 */
static int B_CreateVarFromVecOfInts(const std::string& name, int *vec)
{
    if(!global_ws_var)
        return -1;

    // Create var and get Ptr
    B_CreateEmptyVar(name);
    double* x = B_GetVarPtr(name);
    if(x == 0) 
    {
        std::string error_msg = "B_CreateVarFromVecOfInts: failed to create the variable '";
        error_msg += name + "'";
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
int B_ModelSimulateSaveNIters(const std::string& arg, int unused)
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
int B_ModelSimulateSaveNorms(const std::string& arg, int unused)
{
    return(B_CreateVarFromVecOfDoubles(arg, global_simu->v_norm.empty() ? NULL : global_simu->v_norm.data()));
}

/** 
 *  @header4iode
 *
 *  Strongly Connex Components (SCC) API: alternative functions to reorder and simulate very large models.
 *  
 *  The reordering algorithm can be CPU intensive for very large models. (Large) Models can be split into 
 *  Stronly Connex Components (**SCC**) and each SCC is reordered. The optimized order 
 *  is then saved in IODE lists. 
 *  
 *  The simulation can later be run multiple times without having to reorder the model for each simulation.
 */

#include "api/pch.h"
#include "api/k_super.h"
#include "api/b_errors.h"
#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/equations.h"
#include "api/objs/lists.h"
#include "api/objs/variables.h"
#include "api/simulation/simulation.h"


/**
 *  Reorders the model defined by dbe and saves 3 lists with prolog, epilog and interdependent blocks.
 *  
 *  @param [in] KDB*    dbe     KDB of equations defining the model
 *  @param [in] int     tris    nombre of pseudo-triangulation passes
 *  @param [in] char*   pre     name of the prolog list 
 *  @param [in] char*   inter   name of the interdependent list 
 *  @param [in] char*   post    name of the epilog list 
 *  @return     bool                
 *  
 */
bool CSimulation::calculate_SCC(KDBEquationsPtr dbe, int tris, const std::string& pre, 
    const std::string& inter, const std::string& post)
{
    int opasses = nb_passes;
    int osort = sorting_algo;

    if(dbe->size() == 0) 
    {
        std::string error_msg = "Empty set of equations";
        error_manager.append_error(error_msg);
        return false;
    }

    sim_dbe = dbe;
    nb_passes = tris;

    // to build the PRE, INTER and POST lists in build_lists_order()
    sim_dbv = KDBVariables::Create(false);
    for(const auto& [var_name, _] : dbe->k_objs)
        sim_dbv->add_obj_ptr(var_name, std::shared_ptr<Variable>());

    if(tris > 0) 
        sorting_algo = SORT_BOTH;
    else         
        sorting_algo = SORT_CONNEX;

    map_exchange.clear();
    map_exchange_rev.clear();
    
    // PSEUDO LINK EQUATIONS ie set num endo = num eq
    std::string eq_name;
    std::shared_ptr<Equation> eq_ptr;
    kmsg("Pseudo-linking equations ....");
    for(const auto& [eq_name, eq_ptr] : dbe->k_objs) 
    {
        if(!eq_ptr)
        {
            std::string error_msg = "'" + eq_name + "': cannot find equation";
            error_manager.append_error(error_msg);
            return false;
        }

        // try to compile the equation if it has not been compiled yet
        if(!eq_ptr->clec)
            eq_ptr->compile();
         
        if(!eq_ptr->clec)
        {
            std::string error_msg = "'" + eq_name + "': cannot link equation";
            error_manager.append_error(error_msg);
            return false;
        }
    }

    /* ORDERING EQUATIONS */
    order(dbe);
    build_lists_order(pre, inter, post);

    v_ordered_eqs.clear();
    map_exchange.clear();
    map_exchange_rev.clear();

    nb_passes = opasses;
    sorting_algo = osort;
    return true;
}


/**
 *  Initialize the function simulate_SCC by allocating the needed global vars and linking the equations.
 *  
 *  @param [in]         KDB*    dbe         global_ws_eqs or subset of global_ws_eqs containing all the model equations
 *  @param [in, out]    KDB*    dbv         KDB containing at minimum the model variables (endo + exo)
 *  @param [in]         KDB*    dbs         KDB containing the model scalars
 *  @param [in]         Sample* smpl        simulation Sample
 *  @return             bool                true on success, false if dbe is empty or smpl in incompatible with that of dbs 
 *                                          or the link is impossible or the simulation does not succeed
 */
bool CSimulation::simulate_SCC_init(KDBEquationsPtr dbe, KDBVariablesPtr dbv, KDBScalarsPtr dbs, Sample* smpl)
{
    if(dbe->size() == 0) 
    {
        std::string error_msg = "Empty set of equations";
        error_manager.append_error(error_msg);
        return false;
    }

    sim_dbv = dbv;
    sim_dbe = dbe;
    sim_dbs = dbs;

    // Check Sample dans les bornes du WS
    int t = smpl->start_period.difference(dbv->get_sample()->start_period);
    int at = dbv->get_sample()->end_period.difference(smpl->end_period);
    if(t < 0 || at < 0) 
    {
        std::string error_msg = "Simulation sample out of the Variables sample boundaries";
        error_manager.append_error(error_msg);
        return false;
    }

    map_exchange.clear();
    map_exchange_rev.clear();

    // Initialise les nouvelles vars pour conserver les résultats de sim
    // WARNING: DO NOT FREE v_norm and v_nb_iterations later because they are used 
    //          for reporting afterwards!
    v_norm.clear();
    v_nb_iterations.clear();
    v_cpu_time.clear();
    v_norm.resize(dbv->get_sample()->nb_periods, 0.0);
    v_nb_iterations.resize(dbv->get_sample()->nb_periods, 0);
    v_cpu_time.resize(dbv->get_sample()->nb_periods, 0);

    /* LINK EQUATIONS + SAVE ENDO POSITIONS */
    int rc = 0;
    bool success = true;
    std::string eq_name;
    std::shared_ptr<Equation> eq_ptr = nullptr;
    kmsg("Linking equations ....");
    for(const auto& [eq_name, eq_ptr] : dbe->k_objs) 
    {
        if(!dbv->contains(eq_name)) 
        {
            std::string error_msg = "'" + eq_name + "': cannot find variable";
            error_manager.append_error(error_msg);
            return false;
        }
        
        eq_ptr->compile();
        rc = eq_ptr->clec->link(dbv, dbs);
        if(rc) 
        {
            std::string error_msg = "'" + eq_name + "': cannot link equation";
            error_manager.append_error(error_msg);
            return false;
        }
    }

    v_endo_values.clear();
    v_endo_values_1.clear();
    v_endo_values.resize(nb_inter, 0.0);
    v_endo_values_1.resize(nb_inter, 0.0);
    return true;
}


/**
 *  Simulates a model in the order given by 3 lists of tables of equation names: pre, inter and post.
 *  
 *  @param [in]         KDB*    dbe         global_ws_eqs or subset of global_ws_eqs containing all the model equations
 *  @param [in, out]    KDB*    dbv         KDB containing at minimum the model variables (endo + exo)
 *  @param [in]         KDB*    dbs         KDB containing the model scalars
 *  @param [in]         Sample* smpl        simulation Sample
 *  @param [in]         char*   pre         name of the prolog list 
 *  @param [in]         char*   inter       name of the interdependent list
 *  @param [in]         char*   post        name of the epilog list 
 *  @return             int                 0 on success, -1 on error
 *  
 */
bool CSimulation::simulate_SCC(KDBEquationsPtr dbe, KDBVariablesPtr dbv, KDBScalarsPtr dbs, Sample* smpl, 
    const std::vector<std::string>& pre, const std::vector<std::string>& inter, const std::vector<std::string>& post)
{
    nb_pre = (int) pre.size();
    nb_inter = (int) inter.size();
    nb_post = (int) post.size();

    bool success = simulate_SCC_init(dbe, dbv, dbs, smpl);
    if(!success) 
        return false;

    // Fixe l'ordre d'exécution dans v_order
    v_ordered_eqs.clear();
    v_ordered_eqs.reserve(nb_pre + nb_inter + nb_post);
    for(int i = 0; i < nb_pre; i++)   
        v_ordered_eqs.push_back(pre[i]);
    for(int i = 0; i < nb_inter; i++) 
        v_ordered_eqs.push_back(inter[i]);
    for(int i = 0; i < nb_post; i++)  
        v_ordered_eqs.push_back(post[i]);

    // Simulation
    int t = smpl->start_period.difference(dbv->get_sample()->start_period);

    int rc = 0;
    for(int i = 0; i < smpl->nb_periods; i++, t++)
    {
        rc = sub_simulate(t);
        if(rc > 0)
        {
            clear();
            return false;
        } 
    }

    clear();
    return true;
}

/**
 * Same as IodeModelCalcSCC() (defined in b_api.c from iode_dos repository).
 */
bool CSimulation::calculate_SCC(const int nb_iterations, const std::string& pre_name, 
    const std::string& inter_name, const std::string& post_name, const std::string& list_eqs)
{
    std::string error_msg = "Cannot simulate SCC";
    if(!list_eqs.empty()) 
        error_msg += " for the equations list '" + list_eqs + "'";
    error_msg += ":\n";

    // clear C API errors stack
    error_manager.clear();

    // result list names
    bool lists_ok = true;
    if(pre_name.empty())
    {
        lists_ok = false;
        error_msg += "\tPre-recursive list name is empty\n";
    }
    if(inter_name.empty())
    {
        lists_ok = false;
        error_msg += "\tRecursive list name is empty\n";
    }
    if(post_name.empty())
    {
        lists_ok = false;
        error_msg += "\tPost-recursive list name is empty\n";
    }
    if(!lists_ok)
    {
        kwarning(error_msg.c_str());
        return false;
    }

    char* c_pre = to_char_array(pre_name);
    char* c_inter = to_char_array(inter_name);
    char* c_post = to_char_array(post_name);

    int rc = -1;
    KDBEquationsPtr tdbe = nullptr;
    if(list_eqs.empty())
    {
        tdbe = global_ws_eqs;
        rc = calculate_SCC(tdbe, nb_iterations, c_pre, c_inter, c_post);
    }
    else
    {
        try
        {
            tdbe = global_ws_eqs->get_subset(list_eqs, false);
            if(tdbe->size() > 0)
                rc = calculate_SCC(tdbe, nb_iterations, c_pre, c_inter, c_post);
        }
        catch(const std::exception& e)
        {
            error_msg += "\t" + std::string(e.what());
            kwarning(error_msg.c_str());
            return false;
        }
    }
    
    if(rc < 0)
    {
        std::string error_msg = "Could not calculate SCC for the model";
        error_manager.prepend_error(error_msg);
        kwarning(error_manager.get_all_errors().c_str());
        return false;
    }

    return true;
}

/**
 * Mostly same function as IodeModelSimulateSCC() (defined in b_api.c from iode_dos repository).
 * Unlike in IodeModelSimulateSCC(), the following global parameters are not passed to this function: 
 *    eps, relax, maxit,  init_values, debug, newton_eps, newton_maxit, newton_debug.
 * 
 * TODO: add these parameters as optional arguments
 */
bool CSimulation::simulate_SCC(const std::string& from, const std::string& to, 
    const std::string& pre_name, const std::string& inter_name, const std::string& post_name)
{
    // clear C API errors stack
    error_manager.clear();

    std::string error_msg = "Cannot simulate SCC:\n";

    Sample* sample = nullptr;
    try
    {
        // throw exception if wrong parameters
        sample = new Sample(from, to);
    }
    catch(const std::exception& e)
    {
        error_msg += "\t" + std::string(e.what());
        kwarning(error_msg.c_str());
        return false;
    }

    // result list names
    bool lists_ok = true;
    if(!global_ws_lst->contains(pre_name))
    {
        lists_ok = false;
        error_msg += "\tPre-recursive list '" + pre_name + "' not found!\n";
    }  
    if(!global_ws_lst->contains(inter_name))
    {
        lists_ok = false;
        error_msg += "\tRecursive list '" + inter_name + "' not found!\n";
    }
    if(!global_ws_lst->contains(post_name))
    {
        lists_ok = false;
        error_msg += "\tPost-recursive list '" + post_name + "' not found!\n";
    }
    if(!lists_ok)
    {
        kwarning(error_msg.c_str());
        if (sample) delete sample;
        return false;
    }

    std::string list_pre = global_ws_lst->get(pre_name);
    char** c_pre = (char**) KL_expand(to_char_array(list_pre));
    // convert to std::vector<std::string>
    std::vector<std::string> pre;
    int nb = SCR_tbl_size((unsigned char**) c_pre);
    for(int i = 0; i < nb; i++) 
        pre.push_back(std::string(c_pre[i]));
    SCR_free_tbl((unsigned char**) c_pre); 
    
    std::string list_inter = global_ws_lst->get(inter_name);
    char** c_inter = (char**) KL_expand(to_char_array(list_inter));
    // convert to std::vector<std::string>
    std::vector<std::string> inter;
    nb = SCR_tbl_size((unsigned char**) c_inter);
    for(int i = 0; i < nb; i++) 
        inter.push_back(std::string(c_inter[i]));
    SCR_free_tbl((unsigned char**) c_inter);

    std::string list_post = global_ws_lst->get(post_name);
    char** c_post = (char**) KL_expand(to_char_array(list_post));
    // convert to std::vector<std::string>
    std::vector<std::string> post;
    nb = SCR_tbl_size((unsigned char**) c_post);
    for(int i = 0; i < nb; i++) 
        post.push_back(std::string(c_post[i]));
    SCR_free_tbl((unsigned char**) c_post);  

    // union of pre, inter and post -> to create a subset of global_ws_eqs to simulate
    std::set<std::string> v_eqs;
    v_eqs.insert(pre.begin(), pre.end());
    v_eqs.insert(inter.begin(), inter.end());
    v_eqs.insert(post.begin(), post.end());
    std::string list_eqs;
    for(const auto& eq : v_eqs) 
        list_eqs += eq + ";";
    if(list_eqs.empty())
    {
        error_msg += "\tEmpty list of equations";
        kwarning(error_msg.c_str());
        if (sample) delete sample;
        return false;
    }

    bool success = false;
    try
    {
        KDBEquationsPtr tdbe = global_ws_eqs->get_subset(list_eqs, false);
        if(tdbe->size() > 0)
            success = simulate_SCC(tdbe, global_ws_var, global_ws_scl, sample, pre, inter, post);
    }
    catch(const std::exception& e)
    {
        error_msg += "\t" + std::string(e.what());
        kwarning(error_msg.c_str());
        if(sample) delete sample;
        return false;
    }

    delete sample;

    if(!success)
    {
        std::string error_msg = "Could not simulate SCC for the model"; 
        error_manager.prepend_error(error_msg);
        kwarning(error_manager.get_all_errors().c_str());
        return false;
    }

    return true;
}

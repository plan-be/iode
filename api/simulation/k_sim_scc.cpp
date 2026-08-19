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
bool CSimulation::calculate_SCC(KDBEquationsPtr dbe, int tris, char* pre, char* inter, char* post)
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
    max_depth = dbe->size();
    nb_passes = tris;

    // to build the PRE, INTER and POST lists in build_lists_order() via KSIM_NAME
    std::shared_ptr<Variable> var_ptr;
    sim_dbv = KDBVariables::Create(false);
    for(const std::string& var_name : dbe->get_names())
        sim_dbv->k_objs[var_name] = var_ptr;

    if(tris > 0) 
        sorting_algo = SORT_BOTH;
    else         
        sorting_algo = SORT_CONNEX;

    // v_pos_endo_in_dbv[i] = num dans dbv de la var endogène de l'équation i
    // v_pos_endo_in_dbe[i] = pos in sim_dbe of the eq whose endo is var[i] 
    v_pos_endo_in_dbv = (int *) SW_nalloc((int)(sizeof(int) * dbe->size()));
    v_pos_endo_in_dbe = (int *) SW_nalloc((int)(sizeof(int) * dbe->size()));
    for(int i = 0 ; i < dbe->size(); i++) 
        v_pos_endo_in_dbe[i] = -1;  
    
    // PSEUDO LINK EQUATIONS ie set num endo = num eq
    std::string eq_name;
    std::shared_ptr<Equation> eq_ptr;
    kmsg("Pseudo-linking equations ....");
    for(int i = 0 ; i < dbe->size(); i++) 
    {
        v_pos_endo_in_dbv[i] = i;
        v_pos_endo_in_dbe[i] = i;
        eq_name = dbe->get_name(i);
        eq_ptr = dbe->get_obj_ptr(eq_name);
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

        L_link_endos(dbe, eq_ptr->clec);
    }

    /* ORDERING EQUATIONS */
    order(dbe);
    build_lists_order(pre, inter, post);

    SW_nfree(v_pos_endo_in_dbv);
    v_pos_endo_in_dbv = NULL;
    SW_nfree(v_pos_endo_in_dbe);
    v_pos_endo_in_dbe = NULL;
    SW_nfree(v_order);
    v_order = NULL;

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
    max_depth = dbe->size();
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

    // v_pos_endo_in_dbv[i] = num dans dbv de la var endogène de l'équation i
    v_pos_endo_in_dbv = (int *) SW_nalloc(sizeof(int) * dbe->size());

    // Initialise les nouvelles vars pour conserver les résultats de sim
    // WARNING: DO NOT FREE v_norm and v_nb_iterations later because they are used 
    //          for reporting afterwards!
    SCR_free(v_norm);
    SCR_free(v_nb_iterations);
    SCR_free(v_cpu_time);
    v_norm = (double *) SCR_malloc(sizeof(double) * dbv->get_sample()->nb_periods);
    v_nb_iterations = (int *) SCR_malloc(sizeof(int) * dbv->get_sample()->nb_periods);
    v_cpu_time = (long *) SCR_malloc(sizeof(long) * dbv->get_sample()->nb_periods);

    /* LINK EQUATIONS + SAVE ENDO POSITIONS */
    int rc = 0;
    bool success = true;
    std::string eq_name;
    std::shared_ptr<Equation> eq_ptr = nullptr;
    kmsg("Linking equations ....");
    for(int i = 0 ; i < dbe->size(); i++) 
    {
        eq_name = dbe->get_name(i);

        v_pos_endo_in_dbv[i] = dbv->index_of(eq_name);
        if(v_pos_endo_in_dbv[i] < 0) 
        {
            std::string error_msg = "'" + eq_name + "': cannot find variable";
            error_manager.append_error(error_msg);
            success = false;
            goto fin;
        }
        
        eq_ptr = dbe->get_obj_ptr(eq_name);
        eq_ptr->compile();
        rc = eq_ptr->clec->link(dbv, dbs);
        if(rc) 
        {
            std::string error_msg = "'" + eq_name + "': cannot link equation";
            error_manager.append_error(error_msg);
            success = false;
            goto fin;
        }
    }

    v_endo_values = (double *) SW_nalloc(sizeof(double) * nb_inter);
    v_endo_values_1 = (double *) SW_nalloc(sizeof(double) * nb_inter);
    return true;

fin:
    SW_nfree(v_pos_endo_in_dbv);
    v_pos_endo_in_dbv = NULL;
    return success;
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
bool CSimulation::simulate_SCC(KDBEquationsPtr dbe, KDBVariablesPtr dbv, KDBScalarsPtr dbs, Sample* smpl, char** pre, char** inter, char** post)
{
    nb_pre = SCR_tbl_size((unsigned char**) pre);
    nb_inter = SCR_tbl_size((unsigned char**) inter);
    nb_post = SCR_tbl_size((unsigned char**) post);

    bool success = simulate_SCC_init(dbe, dbv, dbs, smpl);
    if(!success) 
        return false;

    // Fixe l'ordre d'exécution dans v_order
    int j = 0;
    v_order = (int *)  SW_nalloc(sizeof(int) * (nb_pre + nb_inter + nb_post));
    for(int i = 0; i < nb_pre; i++)   
        v_order[j++] = dbe->index_of(std::string(pre[i]));
    for(int i = 0; i < nb_inter; i++) 
        v_order[j++] = dbe->index_of(std::string(inter[i]));
    for(int i = 0; i < nb_post; i++)  
        v_order[j++] = dbe->index_of(std::string(post[i]));

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
    
    std::string list_inter = global_ws_lst->get(inter_name);
    char** c_inter = (char**) KL_expand(to_char_array(list_inter));
    
    std::string list_post = global_ws_lst->get(post_name);
    char** c_post = (char**) KL_expand(to_char_array(list_post));
    
    if(!(list_pre.back() == ';')) list_pre += ";";
    if(!(list_inter.back() == ';')) list_inter += ";";
    std::string list_eqs = list_pre + list_inter + list_post;

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
            success = simulate_SCC(tdbe, global_ws_var, global_ws_scl, sample, c_pre, c_inter, c_post);
    }
    catch(const std::exception& e)
    {
        error_msg += "\t" + std::string(e.what());
        kwarning(error_msg.c_str());
        if(sample) delete sample;
        return false;
    }

    SCR_free_tbl((unsigned char**) c_pre);
    SCR_free_tbl((unsigned char**) c_inter);
    SCR_free_tbl((unsigned char**) c_post);

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

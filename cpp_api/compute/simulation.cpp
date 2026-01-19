#include "simulation.h"


/**
 * Same as B_ModelExchange()
 * 
 * This "exchange" bw endos and exos variables allows to solve the model with respect to an alternate set of variables (some endos being replaced by exos).
 */
bool Simulation::model_exchange(const std::string& list_endo_exo)
{
    if(KSIM_EXO) 
    {
        SCR_free_tbl((unsigned char**) KSIM_EXO);
        KSIM_EXO = NULL;
    }

    if(list_endo_exo.empty())
        return false;
    
    char* c_list_endo_exo = to_char_array(list_endo_exo);
    char** c_exo = B_ainit_chk(c_list_endo_exo, NULL, 0);
    if(c_exo == NULL && !list_endo_exo.empty()) 
    {
        std::string error_msg = "Cannot exchange the model variables:\n";
        error_msg += "Invalid list of endogenous-exogenous pairs: " + list_endo_exo;
        kwarning(error_msg.c_str());
        return false;
    }

    KSIM_EXO = c_exo;
    return true;
}

/**
 * Same as B_ModelCompile()
 * 
 * Equations containing lists in their LEC forms (eg: "Y := a + b * $LIST") must be "recompiled" when the value of $LIST is modified.
 * 
 * Note: rarely, if ever, used.
 */
bool Simulation::model_compile(const std::string& list_eqs)
{
    // clear C API errors stack
    error_manager.clear();

    int rc = -1;

    std::string error_msg = "Could not compile the model";
    if(!list_eqs.empty()) 
        error_msg += " for the equations list '" + list_eqs + "'";
    error_msg += ":\n";

    // EndoExo whole WS
    if(list_eqs.empty()) 
        rc = KE_compile(global_ws_eqs.get());
    else 
    {        
        // EndoExo whole WS
        if(list_eqs.empty()) 
            rc = KE_compile(global_ws_eqs.get());
        else 
        {
            try
            {
                KDBEquations* tdbe = new KDBEquations(global_ws_eqs.get(), list_eqs, false);
                if(tdbe->size() > 0)
                    rc = KE_compile(tdbe);
                delete tdbe;
            }
            catch(const std::exception& e)
            {
                error_msg += "\t" + std::string(e.what());
                kwarning(error_msg.c_str());
                return false;
            }

            if(rc < 0)
            {
                std::string last_error = error_manager.get_last_error();
                if(!last_error.empty()) 
                    error_msg += "\t" + last_error;
                kwarning(error_msg.c_str());
                return false;
            }
        }
    }

    return true;
}


/**
 * Same as B_ModelSimulate()
 */
bool Simulation::model_simulate(const std::string& from, const std::string& to, 
    const std::string& list_eqs)
{
    // clear C API errors stack
    error_manager.clear();

    std::string error_msg = "Cannot simulate the model";

    Sample* sample = nullptr;
    try
    {
        // throw exception if wrong parameters
        sample = new Sample(from, to);
    }
    catch(const std::exception& e)
    {
        error_msg += ":\n" + std::string(e.what());
        kwarning(error_msg.c_str());
        return false;
    }

    error_msg += " for the sample ";
    error_msg += "'" + from + ":" + to + "'";
    if(!list_eqs.empty()) 
        error_msg += " and for the equations list '" + list_eqs + "'";
    error_msg += ":\n";

    int rc = -1;
    if(list_eqs.empty())
        rc = K_simul(global_ws_eqs.get(), global_ws_var.get(), global_ws_scl.get(), 
                     sample, KSIM_EXO, NULL);
    else 
    {
        try
        {
            KDBEquations* tdbe = new KDBEquations(global_ws_eqs.get(), list_eqs, false);
            if(tdbe->size() > 0)
            {
                char** c_eqs = B_ainit_chk((char*) list_eqs.c_str(), NULL, 0);
                rc = K_simul(tdbe, global_ws_var.get(), global_ws_scl.get(), sample, 
                                KSIM_EXO, c_eqs);
                SCR_free_tbl((unsigned char**) c_eqs);
            }
            delete tdbe;
        }
        catch(const std::exception& e)
        {
            error_msg += "\t" + std::string(e.what());
            kwarning(error_msg.c_str());
            if(sample) delete sample;
            return false;
        }
        
    }

    delete sample;

    if(rc < 0)
    {
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
            error_msg += last_error;
        kwarning(error_msg.c_str());
        return false;
    }

    return true;
}

/**
 * Same as IodeModelCalcSCC() (defined in b_api.c from iode_dos repository).
 */
bool Simulation::model_calculate_SCC(const int nb_iterations, const std::string& pre_name, 
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
    KDBEquations* tdbe = nullptr;
    if(list_eqs.empty())
    {
        tdbe = global_ws_eqs.get();
        rc = KE_ModelCalcSCC(tdbe, nb_iterations, c_pre, c_inter, c_post);
    }
    else
    {
        try
        {
            tdbe = new KDBEquations(global_ws_eqs.get(), list_eqs, false);
            if(tdbe->size() > 0)
                rc = KE_ModelCalcSCC(tdbe, nb_iterations, c_pre, c_inter, c_post);
            delete tdbe;
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
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
            error_msg += "\t" + last_error;
        kwarning(error_msg.c_str());
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
bool Simulation::model_simulate_SCC(const std::string& from, const std::string& to, 
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

    int rc = -1;
    try
    {
        KDBEquations* tdbe = new KDBEquations(global_ws_eqs.get(), list_eqs, false);
        if(tdbe->size() > 0)
            rc = K_simul_SCC(tdbe, global_ws_var.get(), global_ws_scl.get(), sample, 
                             c_pre, c_inter, c_post);
        delete tdbe;
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

    if(rc < 0)
    {
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
            error_msg += "\t" + last_error;
        kwarning(error_msg.c_str());
        return false;
    }

    return true;
}

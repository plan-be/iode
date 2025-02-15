#include "simulation.h"


Simulation::Simulation()
{
    // reset global variable KSIM_EXO
    model_exchange();
}

/**
 * Same as B_ModelExchange()
 * 
 * This "exchange" bw endos and exos variables allows to solve the model with respect to an alternate set of variables (some endos being replaced by exos).
 */
void Simulation::model_exchange(const std::string& list_exo)
{
    if (KSIM_EXO) 
    {
        SCR_free_tbl((unsigned char**) KSIM_EXO);
        KSIM_EXO = NULL;
    }

    if (list_exo.empty()) return;
    char* c_list_exo = to_char_array(list_exo);
    KSIM_EXO = B_ainit_chk(c_list_exo, NULL, 0);
}

/**
 * Same as B_ModelCompile()
 * 
 * Equations containing lists in their LEC forms (eg: "Y := a + b * $LIST") must be "recompiled" when the value of $LIST is modified.
 * 
 * Note: rarely, if ever, used.
 */
void Simulation::model_compile(const std::string& list_eqs)
{
    // clear C API errors stack
    B_clear_last_error();

    if (list_eqs.empty()) 
        KE_compile(K_WS[EQUATIONS]);            // EndoExo whole WS
    else 
    {
        char* c_list_eqs = to_char_array(list_eqs);
        char** eqs = B_ainit_chk(c_list_eqs, NULL, 0);
        int nb_eqs = SCR_tbl_size((unsigned char**) eqs);
        if (eqs == NULL || nb_eqs == 0) KE_compile(K_WS[EQUATIONS]);   // EndoExo whole WS
        else 
        {
            KDB* tdbe = K_refer(K_WS[EQUATIONS], nb_eqs, eqs);
            int rc = KE_compile(tdbe);
            K_free_kdb(tdbe);
            SCR_free_tbl((unsigned char**) eqs);
            if (rc < 0)
            {
                std::string error_msg = "Could not compile the model";
                if(!list_eqs.empty()) 
                    error_msg += " for the equations list " + list_eqs + "\n";
                error_msg += get_last_error();
                throw std::runtime_error(error_msg);
            }
        }
    }
}


/**
 * Same as B_ModelSimulate()
 */
void Simulation::model_simulate(const std::string& from, const std::string& to, const std::string& list_eqs)
{
    // clear C API errors stack
    B_clear_last_error();

    Sample* sample = nullptr;
    try
    {
        // throw exception if wrong parameters
        sample = new Sample(from, to);
    }
    catch(const std::exception& e)
    {
         throw std::runtime_error("Cannot simulate the model:\n" + std::string(e.what()));
    }

    char** c_eqs = B_ainit_chk(to_char_array(list_eqs), NULL, 0);
    if (!list_eqs.empty() && c_eqs == NULL) 
    {
        if (sample) delete sample;
        if (SCR_tbl_size((unsigned char**)c_eqs) > 0) SCR_free_tbl((unsigned char**) c_eqs);
        throw std::invalid_argument("Cannot simulate the model:\nthe equations list is empty or null");
    }

    int rc;
    if (SCR_tbl_size((unsigned char**) c_eqs) == 0)
        rc = K_simul(K_WS[EQUATIONS], K_WS[VARIABLES], K_WS[SCALARS], sample, KSIM_EXO, NULL);
    else 
    {
        KDB* tdbe = K_refer(K_WS[EQUATIONS], SCR_tbl_size((unsigned char**) c_eqs), c_eqs);
        rc = K_simul(tdbe, K_WS[VARIABLES], K_WS[SCALARS], sample, KSIM_EXO, c_eqs);
        K_free_kdb(tdbe);
        SCR_free_tbl((unsigned char**) c_eqs);
    }

    if (rc < 0)
    {
        std::string c_api_error = get_last_error();
        std::string error_msg = "Could not simulate the model: the simulation did not converged\n";
        if (!c_api_error.empty()) 
            error_msg += c_api_error + "\n";
        error_msg += "Sample: " + sample->to_string() + "\n";
        error_msg += "Equations list: " + list_eqs + "\n";
        error_msg += "Max iterations: " + std::to_string(get_max_nb_iterations()) + "\n";
        error_msg += "Convergence threshold: " + std::to_string(get_convergence_threshold()) + "\n";
        error_msg += "Initialization method: " + get_initialization_method_as_string() + "\n";
        error_msg += "Sort algorithm: " + get_sort_algorithm_as_string() + "\n";
        delete sample;
        throw std::runtime_error(error_msg);
    }

    delete sample;
}

/**
 * Same as IodeModelCalcSCC() (defined in b_api.c)
 */
void Simulation::model_calculate_SCC(const int nb_iterations, const std::string& pre_name, const std::string& inter_name, const std::string& post_name, const std::string& list_eqs)
{
    std::string error_msg;

    // clear C API errors stack
    B_clear_last_error();

    // result list names
    if (pre_name.empty())   
        error_msg += "Pre-recursive list name is empty\n";
    if (inter_name.empty()) 
        error_msg += "Recursive list name is empty\n";
    if (post_name.empty())  
        error_msg += "Post-recursive list name is empty\n";

    char* c_pre = to_char_array(pre_name);
    char* c_inter = to_char_array(inter_name);
    char* c_post = to_char_array(post_name);

    char** c_eqs = B_ainit_chk(to_char_array(list_eqs), NULL, 0);
    if (!list_eqs.empty() && c_eqs == NULL) 
        error_msg += "Equations list is empty or null\n";

    if(!error_msg.empty())
    {
        if (SCR_tbl_size((unsigned char**)c_eqs) > 0) SCR_free_tbl((unsigned char**)c_eqs);
        throw std::invalid_argument("Cannot calculate SCC:\n" + error_msg);
    }

    int rc;
    KDB* tdbe;
    if (SCR_tbl_size((unsigned char**) c_eqs) == 0)
    {
        tdbe = K_WS[EQUATIONS];
        rc = KE_ModelCalcSCC(tdbe, nb_iterations, c_pre, c_inter, c_post);
    }
    else
    {
        tdbe = K_refer(K_WS[EQUATIONS], SCR_tbl_size((unsigned char**) c_eqs), c_eqs);
        rc = KE_ModelCalcSCC(tdbe, nb_iterations, c_pre, c_inter, c_post);
        K_free_kdb(tdbe);
        SCR_free_tbl((unsigned char**) c_eqs);
    }
    
    if (rc < 0)
    {
        std::string c_api_error = get_last_error();
        std::string error_msg = "Could not not calculate SCC\n";
        if(!c_api_error.empty()) 
            error_msg += c_api_error + "\n";
        error_msg += "Pre-recursive list name: " + pre_name + "\n";
        error_msg += "Recursive list name: " + inter_name + "\n";
        error_msg += "Post-recursive list name: " + post_name + "\n";
        error_msg += "Equations list: " + list_eqs + "\n";
        error_msg += "Nb passes: " + std::to_string(get_nb_passes());
        throw std::runtime_error(error_msg);
    }
}

/**
 * Mostly same function as IodeModelSimulateSCC() (defined in b_api.c).
 * Unlike in IodeModelSimulateSCC(), the following global parameters are not passed to this function: 
 *    eps, relax, maxit,  init_values, debug, newton_eps, newton_maxit, newton_debug.
 * 
 * TODO: add these parameters as optional arguments
 */
void Simulation::model_simulate_SCC(const std::string& from, const std::string& to, const std::string& pre_name, const std::string& inter_name, const std::string& post_name)
{
    // clear C API errors stack
    B_clear_last_error();

    Sample* sample = nullptr;
    try
    {
        // throw exception if wrong parameters
        sample = new Sample(from, to);
    }
    catch(const std::exception& e)
    {
        throw std::runtime_error("Cannot simulate SCC" + std::string(e.what()));
    }

    // result list names
    std::string error_msg;
    if(!Lists.contains(pre_name))   
        error_msg += "Pre-recursive list \"" + pre_name + "\" not found!\n";
    if(!Lists.contains(inter_name)) 
        error_msg += "Recursive list \"" + inter_name + "\" not found!\n";
    if(!Lists.contains(post_name))  
        error_msg += "Post-recursive list \"" + post_name + "\" not found!\n";
    if(!error_msg.empty())
    {
        if (sample) delete sample;
        throw std::invalid_argument("Cannot simulate SCC:\n" + error_msg);
    }

    std::string list_pre = Lists.get(pre_name);
    char** c_pre = (char**) KL_expand(to_char_array(list_pre));
    if (!(list_pre.back() == ';')) list_pre += ";";
    std::string list_inter = Lists.get(inter_name);
    char** c_inter = (char**) KL_expand(to_char_array(list_inter));
    if (!(list_inter.back() == ';')) list_inter += ";";
    std::string list_post = Lists.get(post_name);
    char** c_post = (char**) KL_expand(to_char_array(list_post));
    std::string list_eqs = list_pre + list_inter + list_post;
    char** c_eqs = B_ainit_chk(to_char_array(list_eqs), NULL, 0);

    KDB* tdbe = K_refer(K_WS[EQUATIONS], SCR_tbl_size((unsigned char**) c_eqs), c_eqs);
    int rc = K_simul_SCC(tdbe, K_WS[VARIABLES], K_WS[SCALARS], sample, c_pre, c_inter, c_post);

    K_free_kdb(tdbe);
    SCR_free_tbl((unsigned char**) c_eqs);
    SCR_free_tbl((unsigned char**) c_pre);
    SCR_free_tbl((unsigned char**) c_inter);
    SCR_free_tbl((unsigned char**) c_post);

    if (rc < 0)
    {
        std::string c_api_error = get_last_error();
        std::string error_msg = "Could not simulate SCC\n";
        if(!c_api_error.empty()) 
            error_msg += c_api_error + "\n";
        error_msg += "Sample: " + sample->to_string() + "\n";
        error_msg += "Equations list: " + list_eqs + "\n";
        error_msg += "Pre-recursive list name: " + pre_name + "\n";
        error_msg += "Recursive list name: " + inter_name + "\n";
        error_msg += "Post-recursive list name: " + post_name + "\n";
        error_msg += "Max iterations: " + std::to_string(get_max_nb_iterations()) + "\n";
        error_msg += "Convergence threshold: " + std::to_string(get_convergence_threshold()) + "\n";
        error_msg += "Initialization method: " + get_initialization_method_as_string() + "\n";
        error_msg += "Sort algorithm: ", get_sort_algorithm_as_string();
        delete sample;
        throw std::runtime_error(error_msg);
    }
    delete sample;
}

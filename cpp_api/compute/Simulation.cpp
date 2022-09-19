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
    if (list_eqs.empty()) KE_compile(K_WS[I_EQUATIONS]);            // EndoExo whole WS
    else 
    {
        char* c_list_eqs = to_char_array(list_eqs);
        char** eqs = B_ainit_chk(c_list_eqs, NULL, 0);
        int nb_eqs = SCR_tbl_size((unsigned char**) eqs);
        if (eqs == NULL || nb_eqs == 0) KE_compile(K_WS[K_EQS]);   // EndoExo whole WS
        else 
        {
            KDB* tdbe = K_refer(K_WS[I_EQUATIONS], nb_eqs, eqs);
            int rc = KE_compile(tdbe);
            K_free_kdb(tdbe);
            SCR_free_tbl((unsigned char**) eqs);
            if (rc < 0)
            {
                IodeExceptionFunction error("Cannot compile model", "Unknown");
                error.add_argument("Equation List", list_eqs);
                throw error;
            }
        }
    }
}


/**
 * Same as B_ModelSimulate()
 */
void Simulation::model_simulate(const std::string& from, const std::string& to, const std::string& list_eqs)
{
    IodeExceptionInvalidArguments error("Cannot simulate model");

    Sample* sample = nullptr;
    try
    {
        // throw exception if wrong parameters
        sample = new Sample(from, to);
    }
    catch (IodeException& e)
    {
        error.add_argument("Sample From -> To", from + " -> " + to);
    }

    char** c_eqs = B_ainit_chk(to_char_array(list_eqs), NULL, 0);
    if (!list_eqs.empty() && c_eqs == NULL) error.add_argument("Equations list", list_eqs);
    
    if (error.invalid_args())
    {
        if (sample) delete sample;
        if (SCR_tbl_size((unsigned char**)c_eqs) > 0) SCR_free_tbl((unsigned char**) c_eqs);
        throw error;
    }

    int rc;
    if (SCR_tbl_size((unsigned char**) c_eqs) == 0)
        rc = K_simul(K_WS[I_EQUATIONS], K_WS[I_VARIABLES], K_WS[I_SCALARS], sample->c_sample, KSIM_EXO, NULL);
    else 
    {
        KDB* tdbe = K_refer(K_WS[I_EQUATIONS], SCR_tbl_size((unsigned char**) c_eqs), c_eqs);
        rc = K_simul(tdbe, K_WS[I_VARIABLES], K_WS[I_SCALARS], sample->c_sample, KSIM_EXO, c_eqs);
        K_free_kdb(tdbe);
        SCR_free_tbl((unsigned char**) c_eqs);
    }

    if (rc < 0)
    {
        IodeExceptionFunction error_func("Cannot simulate model", "Simulation did not converged.");
        error_func.add_argument("Sample", sample->to_string());
        error_func.add_argument("Equations list", list_eqs);
        error_func.add_argument("Max iterations", std::to_string(get_max_nb_iterations()));
        error_func.add_argument("Convergence threshold", std::to_string(get_convergence_threshold()));
        error_func.add_argument("Initialization method", get_initialization_method_as_string());
        error_func.add_argument("Sort algorithm", get_sort_algorithm_as_string());
        delete sample;
        throw error_func;
    }

    delete sample;
}

/**
 * Same as B_ModelCalcSCC()
 */
void Simulation::model_calculate_SCC(const int nb_iterations, const std::string& pre_name, const std::string& inter_name, const std::string& post_name, const std::string& list_eqs)
{
    IodeExceptionInvalidArguments error("Cannot calculate SCC");

    // result list names
    KDBLists kdb_list;
    if (pre_name.empty())   error.add_argument("Pre-recursive list name", "empty");
    if (inter_name.empty()) error.add_argument("Recursive list name", "empty");
    if (post_name.empty())  error.add_argument("Post-recursive list name", "empty");

    char* c_pre = to_char_array(pre_name);
    char* c_inter = to_char_array(inter_name);
    char* c_post = to_char_array(post_name);

    char** c_eqs = B_ainit_chk(to_char_array(list_eqs), NULL, 0);
    if (!list_eqs.empty() && c_eqs == NULL) error.add_argument("Equations list", list_eqs);

    if (error.invalid_args())
    {
        if (SCR_tbl_size((unsigned char**)c_eqs) > 0) SCR_free_tbl((unsigned char**)c_eqs);
        throw error;
    }

    int rc;
    KDB* tdbe;
    if (SCR_tbl_size((unsigned char**) c_eqs) == 0)
    {
        tdbe = K_WS[I_EQUATIONS];
        rc = KE_ModelCalcSCC(tdbe, nb_iterations, c_pre, c_inter, c_post);
    }
    else
    {
        tdbe = K_refer(K_WS[I_EQUATIONS], SCR_tbl_size((unsigned char**) c_eqs), c_eqs);
        rc = KE_ModelCalcSCC(tdbe, nb_iterations, c_pre, c_inter, c_post);
        K_free_kdb(tdbe);
        SCR_free_tbl((unsigned char**) c_eqs);
    }
    
    if (rc < 0)
    {
        IodeExceptionFunction error_func("Cannot calculate SCC", "Unknown");
        error_func.add_argument("Pre-recursive list name", pre_name);
        error_func.add_argument("Recursive list name", inter_name);
        error_func.add_argument("Post-recursive list name", post_name);
        error_func.add_argument("Equations list", list_eqs);
        error_func.add_argument("Nb passes", std::to_string(get_nb_passes()));
        throw error_func;
    }
}

/**
 * Same as B_ModelSimulateSCC()
 */
void Simulation::model_simulate_SCC(const std::string& from, const std::string& to, const std::string& pre_name, const std::string& inter_name, const std::string& post_name)
{
    IodeExceptionInvalidArguments error("Cannot simulate SCC");

    Sample* sample = nullptr;
    try
    {
        // throw exception if wrong parameters
        sample = new Sample(from, to);
    }
    catch (IodeException& e)
    {
        error.add_argument("Sample From -> To", from + " -> " + to);
    }

    // result list names
    KDBLists kdb_list;
    if (!kdb_list.contains(pre_name))   error.add_argument("Pre-recursive list not found! ", pre_name);
    if (!kdb_list.contains(inter_name)) error.add_argument("Recursive list not found! ", inter_name);
    if (!kdb_list.contains(post_name))  error.add_argument("Post-recursive list not found! ", post_name);

    if (error.invalid_args())
    {
        if (sample) delete sample;
        throw error;
    }

    std::string list_pre = kdb_list.get(pre_name);
    char** c_pre = (char**) KL_expand(to_char_array(list_pre));
    if (!(list_pre.back() == ';')) list_pre += ";";
    std::string list_inter = kdb_list.get(inter_name);
    char** c_inter = (char**) KL_expand(to_char_array(list_inter));
    if (!(list_inter.back() == ';')) list_inter += ";";
    std::string list_post = kdb_list.get(post_name);
    char** c_post = (char**) KL_expand(to_char_array(list_post));
    std::string list_eqs = list_pre + list_inter + list_post;
    char** c_eqs = B_ainit_chk(to_char_array(list_eqs), NULL, 0);

    KDB* tdbe = K_refer(K_WS[I_EQUATIONS], SCR_tbl_size((unsigned char**) c_eqs), c_eqs);
    int rc = K_simul_SCC(tdbe, K_WS[I_VARIABLES], K_WS[I_SCALARS], sample->c_sample, c_pre, c_inter, c_post);

    K_free_kdb(tdbe);
    SCR_free_tbl((unsigned char**) c_eqs);
    SCR_free_tbl((unsigned char**) c_pre);
    SCR_free_tbl((unsigned char**) c_inter);
    SCR_free_tbl((unsigned char**) c_post);

    if (rc < 0)
    {
        IodeExceptionFunction error_func("simulate SCC", "Unknown");
        error_func.add_argument("Sample", sample->to_string());
        error_func.add_argument("Pre-recursive list name", pre_name);
        error_func.add_argument("Recursive list name", inter_name);
        error_func.add_argument("Post-recursive list name", post_name);
        error_func.add_argument("Equations list", list_eqs);
        error_func.add_argument("Max iterations", std::to_string(get_max_nb_iterations()));
        error_func.add_argument("Convergence threshold", std::to_string(get_convergence_threshold()));
        error_func.add_argument("Initialization method", get_initialization_method_as_string());
        error_func.add_argument("Sort algorithm", get_sort_algorithm_as_string());
        delete sample;
        throw error_func;
    }
    delete sample;
}

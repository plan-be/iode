#pragma once

#include "common.h"
#include "utils.h"
#include "sample.h"
#include "./KDB/kdb_lists.h"


enum EnumSimulationSortAlgorithm
{
    IS_SORT_CONNEX = SORT_CONNEX,
    IS_SORT_BOTH = SORT_BOTH,
    IS_SORT_NONE = SORT_NONE
};

const static std::array<std::string, 3> v_simulation_sort_algorithm =
{
    "Connex compon. decomposition",
    "Connex compon. + Triangulation",
    "None"
};

enum EnumSimulationInitialization
{
    IV_INIT_TM1 = KV_INIT_TM1,
    IV_INIT_TM1_A = KV_INIT_TM1_A,
    IV_INIT_EXTRA = KV_INIT_EXTRA,
    IV_INIT_EXTRA_A = KV_INIT_EXTRA_A,
    IV_INIT_ASIS = KV_INIT_ASIS,
    IV_INIT_TM1_NA = KV_INIT_TM1_NA,
    IV_INIT_EXTRA_NA = KV_INIT_EXTRA_NA
};

const static std::array<std::string, 7> v_simulation_initialization
{
    "Y := Y[-1], if Y null or NA",                  // TM1
    "Y := Y[-1], always",                           // TM1_A
    "Y := extrapolation, if Y null or NA",          // EXTRA    -> Y := fn(Y[-1], Y[-2]), 
    "Y := extrapolation, always"                    // EXTRA_A  -> Y := fn(Y[-1], Y[-3])
    "Y unchanged",                                  // ASIS
    "Y := Y[-1], if Y = NA",                        // TM1_NA
    "Y := extrapolation, if Y = NA",                // EXTRA_NA -> Y := fn(Y[-1], Y[-3])
};


class Simulation
{
public:
    Simulation();

    // ==== GETTER AND SETTER ====

    IODE_REAL get_convergence_threshold()
    {
        return KSIM_EPS;
    }

    void set_convergence_threshold(IODE_REAL threshold)
    {
        KSIM_EPS = threshold;
    }

    IODE_REAL get_relax()
    {
        return KSIM_RELAX;
    }

    void set_relax(IODE_REAL relax)
    {
        KSIM_RELAX = relax;
    }

    int get_max_nb_iterations()
    {
        return KSIM_MAXIT;
    }

    void set_max_nb_iterations(int nb_iterations)
    {
        KSIM_MAXIT = nb_iterations;
    }

    int get_max_nb_iterations_newton()
    {
        return KSIM_NEWTON_MAXIT;
    }

    void set_max_nb_iterations_newton(int nb_iterations)
    {
        KSIM_NEWTON_MAXIT = nb_iterations;
    }

    EnumSimulationSortAlgorithm get_sort_algorithm()
    {
        return (EnumSimulationSortAlgorithm) KSIM_SORT;
    }

    std::string get_sort_algorithm_as_string()
    {
        return v_simulation_sort_algorithm[KSIM_SORT];
    }

    void set_sort(EnumSimulationSortAlgorithm sort_algorithm)
    {
        KSIM_SORT = sort_algorithm;
    }

    EnumSimulationInitialization get_initialization_method()
    {
        return (EnumSimulationInitialization) KSIM_START;
    }

    std::string get_initialization_method_as_string()
    {
        return v_simulation_initialization[KSIM_START];
    }

    void set_initialization_method(EnumSimulationInitialization method)
    {
        KSIM_START = method;
    }

    bool is_debug_active()
    {
        return (bool) KSIM_DEBUG;
    }

    void set_debug(bool debug)
    {
        KSIM_DEBUG = (int) debug;
    }

    bool is_debug_newton_active()
    {
        return (bool) KSIM_NEWTON_DEBUG;
    }

    void set_debug_newton(bool debug)
    {
        KSIM_NEWTON_DEBUG = (int) debug;
    }

    // Number of passes for the heuristic triangulation algorithm
    int get_nb_passes()
    {
        return KSIM_PASSES;
    }

    void set_nb_passes(int nb_passes)
    {
        KSIM_PASSES = nb_passes;
    }

    // ==== MODEL SIMULATION ====

    void model_exchange(const std::string& list_exo = "");

    void model_compile(const std::string& list_eqs = "");

    void model_simulate(const std::string& from, const std::string& to, const std::string& list_eqs = "");

    void model_calculate_SCC(const int nb_iterations, const std::string& pre_name = "_PRE", const std::string& inter_name = "_INTER", 
        const std::string& post_name = "_POST", const std::string& list_eqs = "");

    void model_simulate_SCC(const std::string& from, const std::string& to, const std::string& pre_name = "_PRE", 
        const std::string& inter_name = "_INTER", const std::string& post_name = "_POST");
};

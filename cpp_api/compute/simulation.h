#pragma once

#include "cpp_api/common.h"
#include "cpp_api/utils/utils.h"
#include "cpp_api/time/sample.h"
#include "cpp_api/KDB/kdb_lists.h"


const static std::vector<std::string> v_simulation_sort_algorithm =
{
    "Connex compon. decomposition",
    "Connex compon. + Triangulation",
    "None"
};


class Simulation
{
public:
    Simulation();

    // ==== GETTER AND SETTER ====

    double get_convergence_threshold()
    {
        return KSIM_EPS;
    }

    void set_convergence_threshold(double threshold)
    {
        KSIM_EPS = threshold;
    }

    double get_relax()
    {
        return KSIM_RELAX;
    }

    void set_relax(double relax)
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

    SimuSortAlgorithm get_sort_algorithm()
    {
        return (SimuSortAlgorithm) KSIM_SORT;
    }

    std::string get_sort_algorithm_as_string()
    {
        return v_simulation_sort_algorithm[KSIM_SORT];
    }

    void set_sort(SimuSortAlgorithm sort_algorithm)
    {
        KSIM_SORT = sort_algorithm;
    }

    VariablesInitialization get_initialization_method()
    {
        return (VariablesInitialization) KSIM_START;
    }

    std::string get_initialization_method_as_string()
    {
        return v_simulation_initialization[KSIM_START];
    }

    void set_initialization_method(VariablesInitialization method)
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

    // TODO: add the functions below (giving simulation/reordering informations (see b_api.c)
    //  double IodeModelSimNorm(char* period); 
    //  int IodeModelSimNIter(char* period);
    //  int IodeModelSimCpu(char* period);
    // TODO: consider adding new fns to return the elapsed time for the last SCC decomposition (KSIM_CPU_SCC) and Reordering (KSIM_CPU_SORT)


};

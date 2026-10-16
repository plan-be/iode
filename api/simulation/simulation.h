#pragma once
#include "api/iode_scr4.h"
#include "scr4/scr4.h"

#include "api/pch.h"
#include "api/b_args.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/equations.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"


/* ---------------------- ENUMS ---------------------- */

enum SimuSortAlgorithm
{
    SORT_CONNEX,
    SORT_BOTH,
    SORT_NONE
};

const static std::vector<std::string> v_simulation_sort_algorithm =
{
    "Connex compon. decomposition",
    "Connex compon. + Triangulation",
    "None"
};

const static std::vector<std::string> v_simulation_initialization
{
    "Y := Y[-1], if Y null or NA",                  // TM1
    "Y := Y[-1], always",                           // TM1_A
    "Y := extrapolation, if Y null or NA",          // EXTRA    -> Y := fn(Y[-1], Y[-2]), 
    "Y := extrapolation, always",                   // EXTRA_A  -> Y := fn(Y[-1], Y[-3])
    "Y unchanged",                                  // ASIS
    "Y := Y[-1], if Y = NA",                        // TM1_NA
    "Y := extrapolation, if Y = NA"                 // EXTRA_NA -> Y := fn(Y[-1], Y[-3])
};

/* ---------------------- CLASSES ---------------------- */

class CSimulation
{
public:
	double  epsilon;          	    // Model simulation convergence threshold
	double  relax;        	        // Model relaxation parameter ("damping")
	int     max_iter;               // Maximum number of iteration to reach a solution
	int     debug;        	        // if not null : save 3 list _PRE, _INTER and _POST
	int     nb_passes;       	    // number of passes for the heuristic triangulation algorithm
	int     sorting_algo;           // reordering option : SORT_NONE, SORT_CONNEX or SORT_BOTH
	int     init_algo;              // endogenous initial values
	
    std::set<int> path_examined;
	std::vector<std::string> v_endo_exo;    // Allow exchange exogenous <-> endogenous roles in equations
    std::vector<double> v_norm;             // Convergence threshold reached at the end of each simulation period
    std::vector<int> v_nb_iterations;       // Numbers of iterations needed for each simulation period
    
	int cpu_time_scc;                       // Elapsed time to compute SCC
	int cpu_time_sorting;                   // Elapsed time to sort interdep block
    std::vector<long> v_cpu_time;           // Elapsed time for each simulation period

    double  newton_epsilon;                 // Newton-Raphson: max number of iterations of the Newton-Raphson sub algorithm.
    double  newton_step;                    // Newton-Raphson: save a trace of the sub-iterations
    int     newton_max_iter;                // Newton-Raphson convergence threshold

private:
    std::vector<std::shared_ptr<Equation>> v_eqs_ptr;   // vector of equations defining the model to simulation. 
                                                        // Can be global_ws_eqs or a subset.
    std::vector<std::shared_ptr<Variable>> v_var_ptr;   // vector of variables' pointers. 
    
    KDBEquationsPtr  sim_dbe;               // KDB of equations used for the simulation. Normally global_ws_eqs
    KDBScalarsPtr    sim_dbs;               // KDB of scalars used for the simulation. Normally global_ws_scl
    KDBVariablesPtr  sim_dbv;               // KDB of variables used for the simulation. Normally global_ws_var

	double  norm = 0.0;                     // Error measure: maximum difference between 2 iterations 

    std::vector<std::string> v_eq_names;    // vector of equations names
    std::vector<std::string> v_var_names;   // vector of variables names

    std::unordered_map<std::string, int> map_eq_name_index;     // Cache of equation name to index mapping for fast lookups
    std::unordered_map<std::string, int> map_var_name_index;    // Cache of variable name to index mapping for fast lookups

	// EQUATION ORDERING
	int nb_pre = 0;                         // number of equations in the "prolog" block
	int nb_inter = 0;                       // number of equations in the "interdep" block
	int nb_post = 0;                        // number of equations in the "epilog"
    std::vector<int> v_ordered_eqs;         // positions of the equations in sim_dbe stored in the execution order.
    std::vector<int> v_ordered_vars;        // variable position in sim_dbv for each equation in execution order
    
    std::unordered_map<std::string, std::string> map_exchange;      // pairs <endo, exo> for which the exchanges are defined
    std::unordered_map<std::string, std::string> map_exchange_rev;  // reverse of map_exchange

    std::vector<double> v_endo_values;      // Values of the endogenous variables (in the interdep block) at the end of the previous iteration
    std::vector<double> v_endo_values_1;    // Values of the endogenous variables (in the interdep block) during the current iteration

    std::vector<int> v_permut;              // vector of permutation   
    std::vector<bool> v_ordered;            // indicates if equation i is already in a block

private:
    void clear()
    {
        norm = 0.0;
        nb_pre = 0;     
        nb_inter = 0;   
        nb_post = 0;          
        cpu_time_scc = 0;    
        cpu_time_sorting = 0;

        sim_dbe.reset();
        sim_dbs.reset();
        sim_dbv.reset();

        path_examined.clear();
        v_endo_values.clear();
        v_endo_values_1.clear();

        map_eq_name_index.clear();
        map_var_name_index.clear();

        v_ordered_eqs.clear();
        v_ordered_vars.clear();
        v_permut.clear();
        v_ordered.clear();

        v_eq_names.clear();
        v_var_names.clear();
        v_eqs_ptr.clear();
        v_var_ptr.clear();

        v_endo_exo.clear();
        map_exchange.clear();
        map_exchange_rev.clear();
    }

    // Constructors are private - use Create() factory method instead
    // global or standalone database
    CSimulation()
    {   
        reset();
    }

public:
    /**
     * Factory method to create a managed instance with std::shared_ptr
     * 
     * Usage: auto simu = CSimulation::Create();
     * 
     * @return std::shared_ptr<CSimulation> pointing to the newly created instance
     */
    static std::shared_ptr<CSimulation> Create()
    {
        return std::shared_ptr<CSimulation>(new CSimulation());
    }

	~CSimulation()
	{
		reset();

        // WARNING: reset v_norm, v_nb_iterations and v_cpu_time here and 
        //          NOT in reset() because they are used for reporting after 
        //          the end of the simulation and should not be reset before that.
        v_norm.clear();
        v_nb_iterations.clear();
        v_cpu_time.clear();
	}

    void reset()
    {
        clear();

        epsilon = 0.001;
        relax = 1.0;
        max_iter = 100;
        debug = 0;
        nb_passes = 5;
        sorting_algo = SORT_BOTH;
        init_algo = VAR_INIT_TM1;

        newton_debug = 0;
        newton_epsilon = 1e-6; 
        newton_step = 1e-6;    
        newton_max_iter = 50;   
    }

    // ==== GETTER AND SETTER ====

    SimuSortAlgorithm get_sort_algorithm()
    {
        return (SimuSortAlgorithm) sorting_algo;
    }

    std::string get_sort_algorithm_as_string()
    {
        return v_simulation_sort_algorithm[sorting_algo];
    }

    void set_sort(SimuSortAlgorithm sort_algorithm)
    {
        sorting_algo = sort_algorithm;
    }

    VariablesInitialization get_initialization_method()
    {
        return (VariablesInitialization) init_algo;
    }

    std::string get_initialization_method_as_string()
    {
        return v_simulation_initialization[init_algo];
    }

    void set_initialization_method(VariablesInitialization method)
    {
        init_algo = method;
    }

    bool is_debug_active()
    {
        return debug > 0;
    }

    void set_debug(bool debug)
    {
        this->debug = debug ? 1 : 0;
    }

    bool is_debug_newton_active()
    {
        return newton_debug > 0;
    }

    void set_debug_newton(bool debug)
    {
        newton_debug = debug ? 1 : 0;
    }

    // ==== MODEL SIMULATION ====

	/* k_sim_main.c */
    bool set_exchange(const std::string& list_exo = "");

    bool compile(const std::string& list_eqs = "");

	bool simulate(KDBEquationsPtr dbe, KDBVariablesPtr dbv, KDBScalarsPtr dbs, Sample* smpl, 
		const std::vector<std::string>& eqs = std::vector<std::string>());

    bool simulate(const std::string& from, const std::string& to, const std::string& list_eqs = "");

	/* k_sim_scc.c */
	bool calculate_SCC(KDBEquationsPtr dbe, int tris, const std::string& pre, const std::string& inter, const std::string& post);

    bool calculate_SCC(const int nb_iterations = 5, const std::string& pre_name = "_PRE", const std::string& inter_name = "_INTER", 
        const std::string& post_name = "_POST", const std::string& list_eqs = "");

    bool simulate_SCC(KDBEquationsPtr dbe, KDBVariablesPtr dbv, KDBScalarsPtr dbs, Sample* smpl, 
            const std::vector<std::string>& pre, const std::vector<std::string>& inter, const std::vector<std::string>& post);

    bool simulate_SCC(const std::string& from, const std::string& to, const std::string& pre_name = "_PRE", 
        const std::string& inter_name = "_INTER", const std::string& post_name = "_POST");

protected:
	/* k_sim_main.c */
	double calculate_CLEC(const int eq_pos, const int var_pos, int t, int msg);
	void build_lists_order(const std::string& pre, const std::string& inter, const std::string& post);

	/* k_sim_order.c */
	int find_eq_pos(const std::string& var);
	void order(const std::vector<std::string>& eqs = std::vector<std::string>());
	void compute_tri(std::vector<std::vector<int>>& predecessors, int passes);

	/* k_sim_exo2endo.c */
	bool exo_to_endo(const std::string& endo, const std::string& exo);

    std::vector<std::string> eqs_to_vector(KDBEquationsPtr dbe, const std::string& list_eqs)
    {
        std::vector<std::string> v_eqs;
        if(list_eqs.empty())
            return v_eqs;

        std::vector<std::string> v_tmp_eqs = expand_args(list_eqs, 0);
        if(v_tmp_eqs.empty())
        {
            std::string error_msg = "Invalid equations list: " + list_eqs;
            throw std::invalid_argument(error_msg);
        }

        std::string eq_name;
        v_eqs.reserve(dbe->size());
        for(const std::string& item : v_tmp_eqs)
        {
            eq_name = item;
            if(!dbe->contains(eq_name))
            {
                std::string error_msg = "Equation '" + eq_name + "' not found in the model\n";
                throw std::invalid_argument(error_msg);
            }
            v_eqs.push_back(eq_name);
        }

        return v_eqs;
    }

private:
    void build_ordered_variable_positions()
    {
        int var_pos;
        v_ordered_vars.clear();
        v_ordered_vars.reserve(v_ordered_eqs.size());
        for(const int eq_pos : v_ordered_eqs)
        {
            const std::string& eq_name = v_eq_names[eq_pos];
            // check if endogenous variable (= eq_name) has been exchanged with an exogenous variable
            auto exchange_it = map_exchange.find(eq_name);
            const std::string& var_name = exchange_it != map_exchange.end() ? exchange_it->second : eq_name;
            // push the position of the corresponding variable in sim_dbv into the vector
            var_pos = map_var_name_index.at(var_name);
            // ordered equation 'i' points to 'var_pos'
            v_ordered_vars.push_back(var_pos);
        }
    }

    double* get_variable_values(const int var_pos)
    {
        return v_var_ptr[var_pos]->data();
    }

    double get_variable_value(const int var_pos, const int t)
    {
        return (*v_var_ptr[var_pos])[t];
    }

    void set_variable_value(const int var_pos, const int t, const double value)
    {
        (*v_var_ptr[var_pos])[t] = value;
    }

    bool set_simulation_workspaces(KDBEquationsPtr dbe, KDBScalarsPtr dbs, KDBVariablesPtr dbv)
    {
        std::string error_msg = "Simulation (initialization): ";

        if(dbe->size() == 0) 
        {
            error_msg += "Empty workspace of equations";
            error_manager.append_error(error_msg);
            return false;
        }

        if(dbs->size() == 0) 
        {
            error_msg += "Empty workspace of scalars";
            error_manager.append_error(error_msg);
            return false;
        }

        if(dbv->size() == 0) 
        {
            error_msg += "Empty workspace of variables";
            error_manager.append_error(error_msg);
            return false;
        }

        sim_dbe = dbe;
        sim_dbs = dbs;
        sim_dbv = dbv;

        int eq_pos = 0;
        map_eq_name_index.clear();
        v_eqs_ptr.reserve(dbe->size());
        v_eq_names.reserve(dbe->size());
        std::shared_ptr<Equation> eq_ptr = nullptr;
        for(const std::string& name : dbe->get_names()) 
        {
            eq_ptr = dbe->get_obj_ptr(name);
            if(!eq_ptr)
                throw std::runtime_error(error_msg + "Equation '" + name + "' is null");
            v_eqs_ptr.push_back(eq_ptr);
            v_eq_names.push_back(name);
            map_eq_name_index[name] = eq_pos;
            eq_pos++;
        }

        int var_pos = 0;
        map_var_name_index.clear();
        v_var_ptr.reserve(sim_dbv->size());
        v_var_names.reserve(sim_dbv->size());
        std::shared_ptr<Variable> var_ptr = nullptr;
        for(const std::string& name : sim_dbv->get_names())
        {
            var_ptr = sim_dbv->get_obj_ptr(name);
            if(!var_ptr)
                throw std::runtime_error(error_msg + "Variable '" + name + "' is null");
            v_var_ptr.push_back(var_ptr);
            v_var_names.push_back(name);
            map_var_name_index[name] = var_pos;
            var_pos++;
        }

        return true;
    }

	/* k_sim_main.c */
	void init_values(int t);
	void restore_values(int t);
	int prolog(int t);
	int interdep(int t);
	int sub_interdep_1(int t);
	int sub_interdep_2(int t);
	int epilog(int t);
	int diverge(int t, char* c_name, double eps);
	int sub_simulate(int t);
	void sub_build_lists_order(const std::string& lstname, int eq1, int eqn);

	/* k_sim_order.c */
	int pre_order(std::vector<std::vector<int>>& predecessors, std::vector<std::vector<int>>& successors);
	int build_pre_post_list(std::vector<std::vector<int>>& v_eq_vars, int from);
	int build_inter_list(std::vector<std::vector<int>>& predecessors);
	void compute_tri_perm1(int i, std::vector<int>& v_vars);
	int compute_tri_begin();
	int compute_tri_end();

	/* k_sim_exo2endo.c */
    bool prepare_exchanges();
	bool find_path(const std::string& endo, const std::string& exo, int& depth);

	/* k_sim_scc.c */
	bool simulate_SCC_init(Sample& smpl);
};

/*----------------------- GLOBALS ----------------------------*/

using SimulationPtr = std::shared_ptr<CSimulation>;

// global shared pointer containing an instance of the CSimulation class
// shared_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline SimulationPtr global_simu = CSimulation::Create();

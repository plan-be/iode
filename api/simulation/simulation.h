#pragma once
#include "scr4/s_mat.h"
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
		
	char*   path;
	char**  v_endo_exo;             // Allow exchange exogenous <-> endogenous roles in equations
	double* v_norm;     		    // Convergence threshold reached at the end of each simulation period
	int*    v_nb_iterations;        // Numbers of iterations needed for each simulation period
	long*   v_cpu_time;      	    // Elapsed time for each simulation period
	int     cpu_time_scc;           // Elapsed time to compute SCC
	int     cpu_time_sorting;       // Elapsed time to sort interdep block

    double  newton_epsilon;         // Newton-Raphson: max number of iterations of the Newton-Raphson sub algorithm.
    double  newton_step;            // Newton-Raphson: save a trace of the sub-iterations
    int     newton_max_iter;        // Newton-Raphson convergence threshold

protected:
	KDBVariablesPtr  sim_dbv;       // KDB of variables used for the simulation. Normally global_ws_var
	KDBScalarsPtr    sim_dbs;       // KDB of scalars used for the simulation. Normally global_ws_scl
	KDBEquationsPtr  sim_dbe; 	    // KDB of equations defining the model to simulation. Can global_ws_eqs or a subset.

	double  norm = 0.0;             // Error measure: maximum difference between 2 iterations 

	// EQUATION ORDERING
	int  	nb_pre = 0;             // number of equations in the "prolog" block
	int  	nb_inter = 0;           // number of equations in the "interdep" block
	int  	nb_post = 0;            // number of equations in the "epilog"
	int*    v_order;                // position in dbe of the equations (to simulate) in the execution order.
	int  	max_depth = 0;          // Number of equations in the model
	int*    v_pos_endo_in_dbv;      // Position in sim_dbv of the endo variable of equation "sim_dbe[i]"
	int*    v_pos_endo_in_dbe;      // Position in sim_dbe of the equation whose endo is "sim_dbv[i]" (reverse of v_pos_endo_in_dbv)

	double* v_endo_values;          // Values of the endogenous variables (in the interdep block) at the end of the previous iteration               	
	double* v_endo_values_1;        // Values of the endogenous variables (in the interdep block) during the current iteration

private:
  	int*	v_permut;      	        // vector of permutation 
  	char*	v_ordered;   	        // indicates if equation i is already in a block

private:
    void clear()
    {
        norm = 0.0;
        nb_pre = 0;     
        nb_inter = 0;   
        nb_post = 0;        
        max_depth = 0;  
        cpu_time_scc = 0;    
        cpu_time_sorting = 0;

        if(path != NULL) SW_nfree(path);
        if(v_endo_exo != NULL) SW_nfree(v_endo_exo);
        if(v_endo_values != NULL) SW_nfree(v_endo_values);
        if(v_endo_values_1 != NULL) SW_nfree(v_endo_values_1);	
        if(v_pos_endo_in_dbv != NULL) SW_nfree(v_pos_endo_in_dbv);
        if(v_pos_endo_in_dbe != NULL) SW_nfree(v_pos_endo_in_dbe);
        if(v_order != NULL) SW_nfree(v_order);
        if(v_permut != NULL) SW_nfree(v_permut);
        if(v_ordered != NULL) SW_nfree(v_ordered);

        path = NULL;
        v_endo_exo = NULL;
        v_endo_values = NULL;
        v_endo_values_1 = NULL;
        v_pos_endo_in_dbe = NULL;
        v_pos_endo_in_dbv = NULL;
        v_order = NULL;
        v_permut = NULL;
        v_ordered = NULL;	
    }

    // Constructors are private - use Create() factory method instead
    // global or standalone database
    CSimulation()
    {   
        path = NULL;
        v_norm = NULL;
        v_endo_exo = NULL;
        v_nb_iterations = NULL;	
        v_endo_values = NULL;
        v_endo_values_1 = NULL;
        v_pos_endo_in_dbe = NULL;
        v_pos_endo_in_dbv = NULL;
        v_order = NULL;
        v_permut = NULL;
        v_ordered = NULL;	
        v_cpu_time = NULL;

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
        SCR_free(v_norm);
        SCR_free(v_nb_iterations);
        SCR_free(v_cpu_time);

        v_norm = NULL;
        v_nb_iterations = NULL;
        v_cpu_time = NULL;
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
    bool exchange(const std::string& list_exo = "");

    bool compile(const std::string& list_eqs = "");

	bool simulate(KDBEquationsPtr dbe, KDBVariablesPtr dbv, KDBScalarsPtr dbs, Sample* smpl, 
		const std::vector<std::string>& eqs = std::vector<std::string>());

    bool simulate(const std::string& from, const std::string& to, const std::string& list_eqs = "");

	/* k_sim_scc.c */
	bool calculate_SCC(KDBEquationsPtr dbe, int tris, char* pre, char* inter, char* post);

    bool calculate_SCC(const int nb_iterations, const std::string& pre_name = "_PRE", const std::string& inter_name = "_INTER", 
        const std::string& post_name = "_POST", const std::string& list_eqs = "");

    bool simulate_SCC(KDBEquationsPtr dbe, KDBVariablesPtr dbv, KDBScalarsPtr dbs, Sample* smpl, 
            char** pre, char** inter, char** post);

    bool simulate_SCC(const std::string& from, const std::string& to, const std::string& pre_name = "_PRE", 
        const std::string& inter_name = "_INTER", const std::string& post_name = "_POST");

protected:
	/* k_sim_main.c */
	double calculate_CLEC(int eqnb, int t, int varnb, int msg);
	void build_lists_order(char* pre, char* inter, char* post);

	/* k_sim_order.c */
	void order(KDBEquationsPtr dbe, const std::vector<std::string>& eqs = std::vector<std::string>());
	int get_eq_position(int posendo);
	void compute_tri(KDBEquationsPtr dbe, std::vector<std::vector<int>>& predecessors, int passes);

	/* k_sim_exo2endo.c */
	int exo_to_endo(int posendo, int posexo);

	///< Name of the endogenous of equation i (possibly after endo-exo)
	std::string KSIM_NAME(const int i) const
	{
		return sim_dbv->get_name(v_pos_endo_in_dbv[i]);
	}

	///< get value of endo[i] in period nb t
	double KSIM_VAL(const int i, const int t) const
	{
		std::string name = sim_dbv->get_name(v_pos_endo_in_dbv[i]);
		return sim_dbv->get_value(name, t);
	}

	///< set value of endo[i] in period nb t
	void KSIM_SET_VAL(const int i, const int t, const double value)
	{
		std::string name = sim_dbv->get_name(v_pos_endo_in_dbv[i]);
		*sim_dbv->get_var_ptr(name, t) = value;
	}

    std::vector<std::string> eqs_to_vector(const std::string& list_eqs)
    {
        std::vector<std::string> v_eqs;
        if(list_eqs.empty())
            return v_eqs;

        char** c_eqs = B_ainit_chk((char*) list_eqs.c_str(), NULL, 0);
        if(c_eqs == NULL) 
        {
            std::string error_msg = "Invalid equations list: " + list_eqs;
            throw std::invalid_argument(error_msg);
        }

        std::string eq_name;
        v_eqs.reserve(sim_dbe->size());
        for(int i = 0; c_eqs[i] != NULL; i++)
        {
            eq_name = std::string(c_eqs[i]);
            if(!sim_dbe->contains(eq_name))
            {
                std::string error_msg = "Equation '" + eq_name + "' not found in the model\n";
                throw std::invalid_argument(error_msg);
            }
            v_eqs.push_back(eq_name);
        }

        return v_eqs;
    }

private:
	/* k_sim_main.c */
	void init_values(int t);
	void restore_XK(int t);
	int prolog(int t);
	int interdep(int t);
	int sub_interdep_1(int t);
	int sub_interdep_2(int t);
	int epilog(int t);
	int diverge(int t, char* c_name, double eps);
	int sub_simulate(int t);
	void sub_build_lists_order(char* lstname, int eq1, int eqn);

	/* k_sim_order.c */
	int pre_order(KDBEquationsPtr dbe, std::vector<std::vector<int>>& predecessors, std::vector<std::vector<int>>& successors);
	int add_post(std::vector<std::vector<int>>& successors, int i, int pos);
	int post_order(KDBEquationsPtr dbe, std::vector<std::vector<int>>& predecessors, std::vector<std::vector<int>>& successors);
	int build_pre_post_list(KDBEquationsPtr dbe, std::vector<std::vector<int>>& predecessors, int from);
	int build_inter_list(KDBEquationsPtr dbe, std::vector<std::vector<int>>& predecessors);
	void compute_tri_perm1(KDBEquationsPtr dbe, int i, std::vector<int>& vars);
	int compute_tri_begin(KDBEquationsPtr dbe);
	int compute_tri_end(KDBEquationsPtr dbe);

	/* k_sim_exo2endo.c */
	int find_path(int posendo, int posexo, int* depth);

	/* k_sim_scc.c */
	bool simulate_SCC_init(KDBEquationsPtr dbe, KDBVariablesPtr dbv, KDBScalarsPtr dbs, Sample* smpl);
};

/*----------------------- GLOBALS ----------------------------*/

using SimulationPtr = std::shared_ptr<CSimulation>;

// global shared pointer containing an instance of the CSimulation class
// shared_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline SimulationPtr global_simu = CSimulation::Create();

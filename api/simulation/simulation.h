#pragma once

#include "scr4/s_mat.h"
#include "scr4/scr4.h"

#include "api/constants.h"
#include "api/b_args.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/equations.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"

#include <vector>
#include <string>

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
	static double  KSIM_EPS;          	// Model simulation convergence threshold
	static double  KSIM_RELAX;        	// Model relaxation parameter ("damping")
	static int     KSIM_MAXIT;     		// Maximum number of iteration to reach a solution
	static int     KSIM_DEBUG;        	// Simulation: if not null : save 3 list _PRE, _INTER and _POST
	static int     KSIM_PASSES;       	// Simulation: number of passes for the heuristic triangulation algorithm
	static int     KSIM_SORT;         	// Simulation: reordering option : SORT_NONE, SORT_CONNEX or SORT_BOTH
	static int     KSIM_START;        	// Simulation: endogenous initial values
	
	static double  KSIM_NEWTON_EPS;    	// Newton-Raphson convergence threshold
	static double  KSIM_NEWTON_STEP;   	// Newton-Raphson step to calculate the local derivative (f(x+h) - f(x)) / h
	static int     KSIM_NEWTON_MAXIT; 	// Newton-Raphson: max number of iterations of the Newton-Raphson sub algorithm.
	static int     KSIM_NEWTON_DEBUG; 	// Newton-Raphson: save a trace of the sub-iterations
	
	static char**  KSIM_EXO;
	static char*   KSIM_PATH;
	static double* KSIM_NORMS;     		// Convergence threshold reached at the end of each simulation period
	static int*    KSIM_NITERS;    		// Numbers of iterations needed for each simulation period
	static long*   KSIM_CPUS;      		// Elapsed time for each simulation period
	static int     KSIM_CPU_SCC;   		// Elapsed time to compute SCC
	static int     KSIM_CPU_SORT;  		// Elapsed time to sort interdep block

protected:
	KDBVariables*  KSIM_DBV;         	// KDB of variables used for the simulation. Normally global_ws_var
	KDBScalars*    KSIM_DBS;         	// KDB of scalars used for the simulation. Normally global_ws_scl
	KDBEquations*  KSIM_DBE;         	// KDB of equations defining the model to simulation. Can global_ws_eqs or a subset.

	double  KSIM_NORM;              	// Error measure: maximum difference between 2 iterations 

	// EQUATION ORDERING
	int  	KSIM_PRE;               	// number of equations in the "prolog" block
	int  	KSIM_INTER;             	// number of equations in the "interdep" block
	int  	KSIM_POST;              	// number of equations in the "epilog"
	int*    KSIM_ORDER;             	// position in dbe of the equations (to simulate) in the execution order.
	int  	KSIM_MAXDEPTH;          	// Number of equations in the model
	int*    KSIM_POSXK;             	// Position in KSIM_DBV of the endo variable of equation "KSIM_DBE[i]"
	int*    KSIM_POSXK_REV;         	// Position in KSIM_DBE of the equation whose endo is "KSIM_DBV[i]" (reverse of KSIM_POSXK)

	double* KSIM_XK;                	// Values of the endogenous variables (in the interdep block) at the end of the previous iteration 
	double* KSIM_XK1;               	// Values of the endogenous variables (in the interdep block) during the current iteration

private:
  	int*	KSIM_PERM;      			// vector of permutation 
  	char*	KSIM_ORDERED;   			// indicates if equation i is already in a block

public:
	CSimulation()
	{
		KSIM_PATH = NULL;
		KSIM_XK = NULL;
		KSIM_XK1 = NULL;
		KSIM_POSXK = NULL;
		KSIM_POSXK_REV = NULL;
		KSIM_ORDER = NULL;	
	}

	~CSimulation()
	{
		clear();
	}

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

	/* k_sim_main.c */
	int simulate(KDBEquations* dbe, KDBVariables* dbv, KDBScalars* dbs, Sample* smpl, 
		char** endo_exo, const std::vector<std::string>& eqs = std::vector<std::string>());

	/* k_sim_scc.c */
	int calculate_SCC(KDBEquations* dbe, int tris, char* pre, char* inter, char* post);
	int simulate_SCC(KDBEquations* dbe, KDBVariables* dbv, KDBScalars* dbs, Sample* smpl, 
		             char** pre, char** inter, char** post);

protected:
	/* k_sim_main.c */
	double calculate_CLEC(int eqnb, int t, int varnb, int msg);
	void build_lists_order(char* pre, char* inter, char* post);

	/* k_sim_order.c */
	void order(KDBEquations* dbe, const std::vector<std::string>& eqs = std::vector<std::string>());
	int get_eq_position(int posendo);
	void compute_tri(KDBEquations* dbe, int** predecessors, int passes);

	/* k_sim_exo2endo.c */
	int exo_to_endo(int posendo, int posexo);

	///< Name of the endogenous of equation i (possibly after endo-exo)
	std::string KSIM_NAME(const int i) const
	{
		return KSIM_DBV->get_name(KSIM_POSXK[i]);
	}

	///< get value of endo[i] in period nb t
	double KSIM_VAL(const int i, const int t) const
	{
		std::string name = KSIM_DBV->get_name(KSIM_POSXK[i]);
		return KSIM_DBV->get_value(name, t);
	}

	///< set value of endo[i] in period nb t
	void KSIM_SET_VAL(const int i, const int t, const double value)
	{
		std::string name = KSIM_DBV->get_name(KSIM_POSXK[i]);
		*KSIM_DBV->get_var_ptr(name, t) = value;
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
        v_eqs.reserve(KSIM_DBE->size());
        for(int i = 0; c_eqs[i] != NULL; i++)
        {
            eq_name = std::string(c_eqs[i]);
            if(!KSIM_DBE->contains(eq_name))
            {
                std::string error_msg = "Equation '" + eq_name + "' not found in the model\n";
                throw std::invalid_argument(error_msg);
            }
            v_eqs.push_back(eq_name);
        }

        return v_eqs;
    }

private:
	void clear()
	{
		SW_nfree(KSIM_XK);
		SW_nfree(KSIM_XK1);
		SW_nfree(KSIM_POSXK);
		SW_nfree(KSIM_POSXK_REV);
		SW_nfree(KSIM_ORDER);
		SW_nfree(KSIM_PATH);

		KSIM_PATH = NULL;
		KSIM_XK = NULL;
		KSIM_XK1 = NULL;
		KSIM_POSXK = NULL;
		KSIM_POSXK_REV = NULL;
		KSIM_ORDER = NULL;	
	}

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
	int pre_order(KDBEquations* dbe, int** predecessors, int** successors);
	int add_post(int** successors, int i, int pos);
	int post_order(KDBEquations* dbe, int** predecessors, int** successors);
	int build_pre_post_list(KDBEquations* dbe, int** predecessors, int from);
	int build_inter_list(KDBEquations* dbe, int** predecessors);
	void compute_tri_perm1(KDBEquations* dbe, int i, int* vars);
	int compute_tri_begin(KDBEquations* dbe);
	int compute_tri_end(KDBEquations* dbe);

	/* k_sim_exo2endo.c */
	int find_path(int posendo, int posexo, int* depth);

	/* k_sim_scc.c */
	int simulate_SCC_init(KDBEquations* dbe, KDBVariables* dbv, KDBScalars* dbs, Sample* smpl);
};

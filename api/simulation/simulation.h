#pragma once

#include "scr4/s_mat.h"
#include "scr4/scr4.h"

#include "api/constants.h"
#include "api/b_args.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
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
	KDB*    KSIM_DBV;            		// KDB of variables used for the simulation. Normally K_WS[VARIABLES]
	KDB*    KSIM_DBS;            		// KDB of scalars used for the simulation. Normally K_WS[SCALARS]
	KDB*    KSIM_DBE;            		// KDB of equations defining the model to simulation. Can K_WS[EQUATIONS] or a subset.

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
		K_simul_free();
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
	int K_simul(KDB *,KDB *,KDB *,Sample *,char **, char **);

	/* k_sim_scc.c */
	int KE_ModelCalcSCC(KDB* dbe, int tris, char* pre, char* inter, char* post);
	int K_simul_SCC(KDB* dbe, KDB* dbv, KDB* dbs, Sample* smpl, 
		                   char** pre, char** inter, char** post);

protected:
	/* k_sim_main.c */
	double K_calc_clec(int ,int ,int ,int);
	void K_lstorder(char *,char *,char *);

	/* k_sim_order.c */
	void KE_order(KDB *, char **);
	int KE_poseq(int );
	void KE_tri(KDB *,int **,int );

	/* k_sim_exo2endo.c */
	int KE_exo2endo(int ,int );

	///< Name of the endogenous of equation i (possibly after endo-exo)
	std::string KSIM_NAME(const int i) const
	{
		return KSIM_DBV->get_name(KSIM_POSXK[i]);
	}

	///< get value of endo[i] in period nb t
	double KSIM_VAL(const int i, const int t) const
	{
		std::string name = KSIM_DBV->get_name(KSIM_POSXK[i]);
		return *KVVAL(KSIM_DBV, name, t);
	}

	///< set value of endo[i] in period nb t
	void KSIM_SET_VAL(const int i, const int t, const double value)
	{
		std::string name = KSIM_DBV->get_name(KSIM_POSXK[i]);
		*KVVAL(KSIM_DBV, name, t) = value;
	}

private:
	void K_simul_free()
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
	void K_init_values(int );
	void K_restore_XK(int );
	int K_prolog(int );
	int K_interdep(int );
	int K_interdep_1(int );
	int K_interdep_2(int );
	int K_epilog(int );
	int K_diverge(int ,char *,double );
	int K_simul_1(int t);
	void K_lstorder_1(char* lstname, int eq1, int eqn);

	/* k_sim_order.c */
	int KE_preorder(KDB *,int **,int **);
	int KE_add_post(int **,int ,int );
	int KE_postorder(KDB *,int **,int **);
	int KE_pre(KDB *,int **,int );
	int KE_interdep(KDB *,int **);
	void KE_tri_perm1(KDB *,int ,int *);
	int KE_tri_begin(KDB *);
	int KE_tri_end(KDB *);

	/* k_sim_exo2endo.c */
	int KE_findpath(int posendo, int posexo, int* depth);

	/* k_sim_scc.c */
	int K_simul_SCC_init(KDB* dbe, KDB* dbv, KDB* dbs, Sample* smpl);
};

#pragma once

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/kdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------- ENUMS ---------------------- */

enum SimuSortAlgorithm
{
    SORT_CONNEX,
    SORT_BOTH,
    SORT_NONE
};

/* ---------------------- GLOBALS ---------------------- */

extern double   KSIM_EPS,               // Model simulation convergence threshold
		        KSIM_RELAX,             // Model relaxation parameter ("damping")
		        KSIM_NEWTON_EPS,        // Newton-Raphson convergence threshold
		        KSIM_NEWTON_STEP;       // Newton-Raphson step to calculate the local derivative (f(x+h) - f(x)) / h

extern int      KSIM_MAXIT,             // Simulation: max number of iterations
		        KSIM_DEBUG,             // Simulation: if not null : save 3 list _PRE, _INTER and _POST
		        KSIM_PASSES,            // Simulation: number of passes for the heuristic triangulation algorithm
		        KSIM_NEWTON_DEBUG,      // Newton-Raphson: save a trace of the sub-iterations
		        KSIM_NEWTON_MAXIT,      // Newton-Raphson: max number of iterations of the Newton-Raphson sub algorithm.
		        KSIM_SORT,              // Simulation: reordering option : SORT_NONE, SORT_CONNEX or SORT_BOTH
		        KSIM_START;             // Simulation: endogenous initial values

extern char   **KSIM_EXO;

extern int      KSIM_MAXIT;     		// Maximum number of iteration to reach a solution
extern int     *KSIM_NITERS;    		// Numbers of iterations needed for each simulation period
extern long    *KSIM_CPUS;      		// Elapsed time for each simulation period
extern double  *KSIM_NORMS;     		// Convergence threshold reached at the end of each simulation period

extern int      KSIM_CPU_SCC;   		// Elapsed time to compute SCC
extern int      KSIM_CPU_SORT;  		// Elapsed time to sort interdep block

// EQUATION ORDERING
extern int  	KSIM_PRE;               // number of equations in the "prolog" block
extern int  	KSIM_INTER;             // number of equations in the "interdep" block
extern int  	KSIM_POST;              // number of equations in the "epilog"
//extern int  *KSIM_ORDER;
//extern char *KSIM_ORDERED;
extern int  	KSIM_MAXDEPTH;          // Number of equations in the model
extern int     *KSIM_POSXK;             // Position in KSIM_DBV of the endo variable of equation "KSIM_DBE[i]"
extern int     *KSIM_POSXK_REV;         // Position in KSIM_DBE of the equation whose endo is "KSIM_DBV[i]" (reverse of KSIM_POSXK)

/* ---------------------- FUNCS ---------------------- */

/* k_sim_main.c */
extern int K_simul(KDB *,KDB *,KDB *,SAMPLE *,char **, char **);
extern int K_simul_1(int );
extern void K_simul_free();
//extern void K_init_values(int );
//extern void K_restore_XK(int );
extern double K_calc_clec(int ,int ,int ,int);
//extern void K_prolog(int );
//extern int K_interdep(int );
//extern int K_interdep(int );
//extern void K_epilog(int );
extern void K_lstorder(char *,char *,char *);
// extern int K_diverge(int ,char *,double );

/* k_sim_order.c */
extern void KE_order(KDB *, char **);
//extern int KE_preorder(KDB *,int **,int **);
extern int KE_poseq(int );
//extern int KE_add_post(int **,int ,int );
//extern int KE_postorder(KDB *,int **,int **);
//extern int KE_pre(KDB *,int **,int );
//extern int KE_interdep(KDB *,int **);
extern void KE_tri(KDB *,int **,int );
//extern int KE_tri_perm1(KDB *,int ,int *);
//extern int KE_tri_begin(KDB *);
//extern int KE_tri_end(KDB *);

/* k_sim_exo2endo.c */
extern int KE_exo2endo(int ,int );
//extern int KE_findpath(int ,int ,int *);

/* k_sim_scc.c */
extern int KE_ModelCalcSCC(KDB* dbe, int tris, char* pre, char* inter, char* post);
//extern int K_simul_SCC_init(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl);
extern int K_simul_SCC(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** pre, char** inter, char** post);

#ifdef __cplusplus
}
#endif

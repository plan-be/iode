/** 
 * @header4iode
 *
 *   The Gauss-Seidel algorithm
 *   ��������������������������
 *  
 *  A macroeconomic model is a system of nonlinear equations that must be solved with respect 
 *  to its endogenous variables, say {yi}. 
 *  
 *  Most macroeconometric models are solved using the Gauss-Seidel technique. It is a
 *  simple technique and in most cases works remarkably well. This technique is
 *  used in IODE to simulate models. The Gauss-Seidel algorithm is based on the fixed-point theorem
 *  (see https://en.wikipedia.org/wiki/Fixed-point_theorems) and is easy
 *  to describe by means of an example.
 *  
 *  Assume that the model consists of n equations: 
 *  
 *    f1(y1, y2, y3..., yn) = 0
 *    f2(y1, y2, y3..., yn) = 0
 *    f3(y1, y2, y3..., yn) = 0
 *      ...                   
 *    fn(y1, y2, y3..., yn) = 0
 *  
 *  
 *  The Gauss-Seidel technique requires to rewrite the equations in such a way that each endogenous
 *  variable is placed on the left member of the equations. 
 *  
 *  For example, if the equation is 
 *  
 *      ln(y1 / y2) : = y3 + x1
 *    
 *  then y2 can be moved to the right side of the equation:
 *  
 *      y1 = y2 * exp(y3 + x1)
 *        
 *  IODE tries to do that for each equation, provided that the endogenous variable appears only once in
 *  the equation. The endogenous variable can appear either on the left or on the right.
 *  
 *  When these transformations of the equations are done, the model can be rewritten as follows :
 *  
 *    y1 = f1(y2, y3..., yn)
 *    y2 = f2(y1, y3..., yn)
 *    y3 = f3(y1, y2..., yn)
 *     ...                 
 *    yn = fn(y1, y2..., yn-1) 
 *   
 *  Note: when an equation cannot be transformed in the form yn = fn(...), a "sub-algorithm" based on the Newton-Raphson 
 *  or secant method is used to numerically solve the equation with respect to its endogenous variable. 
 *  See below for more information on that sub-algorithm.
 *  
 *  The Gauss-Seidel algorithm then proceeds as follows. 
 *  
 *  Initial values of the endogenous variables are selected. These are either the
 *  actual values or extrapolations from the previous period according to some rules. IODE
 *  allow various strategies. 
 *  
 *  Given these values the equations can be computed successively and produce a new set of values for the endogenous variables {y1...yn}.
 *  With this new set of values, the equations can again be calculated to get another set {y1...yn}, etc.
 *  
 *  Convergence is reached if, for each endogenous variable, the values of successive
 *  iterations are within some predefined tolerance level.
 *  
 *  There is no guarantee that the Gauss-Seidel method will converge. The advantage of
 *  the technique, however, is that it can usually be made to converge (assuming an actual
 *  solution exists) with sufficient "damping", as described below.
 *  
 *  Let
 *      yi[k-1] denote the solution value of endogenous yi for iteration k-1 
 *      yi[k]   denote the value computed by solving the equation on iteration k. 
 *        
 *  Instead of using yi[k] as the new value for iteration k, one can adjust yi[k] by combining yi[k-1] and yi[k]:
 *  
 *      y1[k] = y1[k-1] + lambda * (y[k] - y[k-1]) where 0 << lamda <<= 1
 *  
 *  
 *  If lambda == 1, there is no damping.
 *  
 *  The solution is reached when the difference between 2 iterations is under a defined threshold for each endogenous {y1...yn}.
 *  
 *   
 * 
 * List of functions 
 * -----------------
 *
 *      int K_simul(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** endo_exo, char** eqs) Simulates a model defined by a set of equations and optional replacements endo-exo.
 *      void K_simul_free()                                             Frees all temporary allocated memory for the simulation.
 *      IODE_REAL K_calc_clec(int eqnb, int t, int varnb, int msg)      Tries to find a value for varnb[t] that satifies the equality in the equation eqnb. 
 */
 
#include "iode.h"

#define KSIM_VAL(i, t)      *(KVVAL(KSIM_DBV, KSIM_POSXK[i], t))  // Value of endo[i] in period nb t
#define KSIM_NAME(i)        (KONAME(KSIM_DBV, KSIM_POSXK[i]))     // Name  of the endogenous of equation i (possibly after endo-exo)  

// Global variables
IODE_REAL   *KSIM_NORMS = 0;	      // Convergence threshold reached at the end of each simulation period 
IODE_REAL   KSIM_NORM;                // Error measure: maximum difference between 2 iterations 
int         *KSIM_NITERS = 0; 	      // Numbers of iterations needed for each simulation period
IODE_REAL   KSIM_EPS = 0.001;         // Required max convergence threshold
IODE_REAL   KSIM_RELAX = 1.0;         // Relaxation parameter
IODE_REAL   *KSIM_XK;                 // Values of the endogenous variables (in the interdep block) at the end of the previous iteration 
IODE_REAL   *KSIM_XK1;                // Values of the endogenous variables (in the interdep block) during the current iteration
int         KSIM_MAXIT= 100;          // Maximum number of iteration to reach a solution
int         KSIM_DEBUG = 0;           // Debug level: 0 = no debugging output
int         KSIM_MAXDEPTH;            // Number of equations in the model
int         *KSIM_POSXK;              // Position in KSIM_DBV of the endo variable of equation "KSIM_DBE[i]"
int         KSIM_PASSES = 5;          // Number of passes for the heuristic triangulation algorithm
int         KSIM_SORT = SORT_BOTH;    // Reordering option : SORT_NONE, SORT_CONNEX or SORT_BOTH  
int         KSIM_START = KV_INIT_TM1; // Endogenous initial values: @see KV_init_values_1() for available options
char        **KSIM_EXO = NULL;        // 
char        *KSIM_PATH = NULL;        //
KDB         *KSIM_DBV;                // KDB of variables used for the simulation. Normally K_WS[K_VAR]
KDB         *KSIM_DBS;                // KDB of scalars used for the simulation. Normally K_WS[K_SCL]
KDB         *KSIM_DBE;                // KDB of equations defining the model to simulation. Can K_WS[K_EQS] or a subset.

extern int  KSIM_PRE,                 // number of equations in the "prolog" block 
            KSIM_INTER,               // number of equations in the "interdep" block
            KSIM_POST,                // number of equations in the "epilog"
            *KSIM_ORDER;              // position in dbe of the equations (to simulate) in the execution order

/**
 *  Initialises the values of the interdependent endogenous variables for one period before starting 
 *  the solver on that period.
 *  
 *  @see KV_init_values_1() for the available initialisation methods.
 *  
 *  @param [in] int     t           period to initialise
 *  @global     int     KSIM_START  initialisation method 
 *  
 */
static void K_init_values(int t)
{
    int         i;
    IODE_REAL        *val;

    if(KSIM_START == KV_INIT_ASIS) return;

    for(i = 0 ; i < KSIM_PRE + KSIM_INTER + KSIM_POST; i++) {
        val = KVVAL(KSIM_DBV, KSIM_POSXK[KSIM_ORDER[i]], 0);
        KV_init_values_1(val, t, KSIM_START);
    }
}

/**
 *  Restore the endo values of the interdependent block by setting their values before the last iteration
 *  (saved in KSIM_XK):
 *  
 *      ENDO[i,t]=KSIM_XK[i]
 *  
 *  @param  [in]  int           t         index of the period where the data must be copied
 *  @global [in]  IODE_REAL*    KSIM_XK   result of the last Gauss-Seidel iteration 
 *  
 */
static void K_restore_XK(int t)
{
    int         i, j;

    for(i = KSIM_PRE, j = 0; j < KSIM_INTER; i++, j++)
        KSIM_VAL(KSIM_ORDER[i], t) = KSIM_XK[j];
    return(0);
}


/**
 *  Frees all temporary allocated memory for the simulation.
 */
void K_simul_free()
{
    SW_nfree(KSIM_XK);
    SW_nfree(KSIM_XK1);
    SW_nfree(KSIM_POSXK);
    SW_nfree(KSIM_ORDER);
    SW_nfree(KSIM_PATH);
    KSIM_PATH = NULL;
    KSIM_XK = KSIM_XK1 = NULL;
    KSIM_POSXK = KSIM_ORDER = NULL;
}

/**
 *  Calculates the first non interdependent part of the model (the "prolog").
 *  The solution of each equation is saved in the global KSIM_DBV.
 *  
 *  @param [in] int     t   period to calculated
 *  @return     int         0 (should be L_NAN on error ?)
 *  
 */
static int K_prolog(int t)
{
    int     i;
    IODE_REAL    x;

    for(i = 0; i < KSIM_PRE; i++)  {
        x = K_calc_clec(KSIM_ORDER[i], t, KSIM_POSXK[KSIM_ORDER[i]], 0);
        KSIM_VAL(KSIM_ORDER[i], t) = x;
        // if(!L_ISAN(x)) return(-1); /* JMP 13-04-00 */
    }
    return(0);
}


/**
 *  Tries to solve the interdependent block of the model using a modified Gauss-Seidel algorithm.
 *  
 *  For eq nb i:
 *    - saves the previous iteration value of the endogenous variable in KSIM_XK[i]
 *    - computes the new value of the endo var and saves it into KSIM_DBV (via KSIM_VAL) 
 *    - if required, modifies the resulting value by "relaxing" it (multiply by KSIM_RELAX)
 *    - computes the ||f(x)|| = diff between the new endo value and the value of the previous iteration 
 *      and saves that value in KSIM_NORM. 
 *  
 *  @param [in]     int         t            simulated period
 *  @return         int                      0 if the equation return a real value
 *                                           -1 if the equation returns L_NAN
 *  @global [out]   IODE_REAL   KSIM_NORM    maximum difference bw endos before and after iteration
 *  
 */
 
static int K_interdep_1(int t)
{
    int     i, j;
    IODE_REAL    x;
    double  d, pd, fpd1, fpd2;


    KSIM_NORM = 0.0;
    for(i = KSIM_PRE, j = 0; j < KSIM_INTER; i++, j++)  {
        /* save XK first */
        KSIM_XK[j] = KSIM_VAL(KSIM_ORDER[i], t);

        /* execute lec */
        x = K_calc_clec(KSIM_ORDER[i], t, KSIM_POSXK[KSIM_ORDER[i]], 1);
        if(!L_ISAN(x)) return(-1);

        /* Check convergence */
        if(L_ISAN(KSIM_XK[j])) {
            d = (KSIM_XK[j] - x);   // d = diff between 2 iterations
            if(!L_IS0(KSIM_XK[j]))  
                pd = min(fabs(1 - x / KSIM_XK[j]), fabs(d)); // if ||endo|| != 0, norm = relative difference
            else 
                pd = fabs(d);                                // else norm = |d| 

            pd *= KSIM_RELAX; 
            if(pd > KSIM_NORM) KSIM_NORM = pd;

            // Stores the new endo value and "relaxes" it 
            KSIM_VAL(KSIM_ORDER[i], t) = KSIM_RELAX * (x - KSIM_XK[j]) +
                                         KSIM_XK[j];
        }
        else {
            // if NaN, set KSIM_NORM to a huge value 
            KSIM_NORM = 10;
            KSIM_VAL(KSIM_ORDER[i], t) = x;
        }
    }

    return(0);
}


/**
 *  Tries to solve the nonlinear equation system defined by the interdependent block of the model.
 *  
 *  This version applies the relaxation parameter at the end of a complete iteration of the model, 
 *  instead of directly after each equation calculation.
 *  
 *  A solution is reached if the difference between 2 iterations k and k+1 is less that KSIM_NORM. 
 *   
 *  @param [in]     int         t  index of the calculation period
 *  @return         int         -1 if the result of an equation is L_NAN
 *                              0 otherwise
 *  @global [out]   IODE_REAL   KSIM_NORM    maximum difference between 2 iterations
 */
 
static int K_interdep_2(int t)
{
    int     i, j;
    IODE_REAL    x;
    double  d, pd, fpd1, fpd2;

    // Stage 1
    for(i = KSIM_PRE, j = 0; j < KSIM_INTER; i++, j++)  {
        /* save XK for further use */
        KSIM_XK[j] = KSIM_VAL(KSIM_ORDER[i], t);

        /* execute lec and save in KSIM_XK1 */
        KSIM_XK1[j] = K_calc_clec(KSIM_ORDER[i], t, KSIM_POSXK[KSIM_ORDER[i]], 1);
        if(!L_ISAN(KSIM_XK1[j])) return(-1); // NaN value --> stop simulation
    }

    // Stage 2
    KSIM_NORM = 0.0;
    for(i = KSIM_PRE, j = 0; j < KSIM_INTER; i++, j++)  {
        if(L_ISAN(KSIM_XK[j])) { // Valeur pr�c�dente d�finie
            d = KSIM_XK[j] - KSIM_XK1[j]; // Diff between iterations

            // Calcule la 'norme' = fabs de la diff�rence relative entre 2 it.
            //   ou de la diff entre 2 it.
            if(!L_IS0(KSIM_XK[j]))
                pd = min(fabs(1 - KSIM_XK1[j] / KSIM_XK[j]), fabs(d));
            else
                pd = fabs(d);

            pd *= KSIM_RELAX;
            if(pd > KSIM_NORM) KSIM_NORM = pd; // norme : la plus grande pour le mod.

            /* Store new value and relax it */
            KSIM_VAL(KSIM_ORDER[i], t) =
                KSIM_RELAX * KSIM_XK1[j] + (1 - KSIM_RELAX) * KSIM_XK[j];
        }
        else { // If previous iteation value is L-NAN, set KSIM_NORM to 10 and assing new calc value to endo
            KSIM_NORM = 10;
            KSIM_VAL(KSIM_ORDER[i], t) = KSIM_XK1[j];
        }
    }

    return(0);
}


/**
 *  Solves the interdependent part of the model by a modified Gauss-Seidel algorithm.
 *  
 *  Two methods are available depending on KSIM_RELAX sign: 
 *      - if KSIM_RELAX > 0: K_interdep_1() that "relaxes" each endo directly after its calculation
 *      - else:              K_interdep_2() that waits for the end of the iteration before "relaxing" 
 *                              all endos at the same time
 *  
 *  @param [in] int     t   period to solve (=simulate)
 *  @return     int         0 on success, -1 on error
 *  
 */
static int K_interdep(int t)
{
    int         rc;
    double      relax = KSIM_RELAX;

    KSIM_RELAX = fabs(relax);
    if(relax >= 0)      rc = K_interdep_1(t);
    else                rc = K_interdep_2(t);

    KSIM_RELAX = relax;
    return(rc);
}


/**
 *  Calculates the last non interdependent (post recursive) part of the model.
 *  
 *  @param [in] int     t   period to calculated
 *  @return     int         0 (should be L_NAN on error ?)
 *  
 */
static int K_epilog(int t)
{
    int     i, j;
    IODE_REAL    x;


    for(i = KSIM_PRE + KSIM_INTER, j = 0; j < KSIM_POST; i++, j++)  {
        x = K_calc_clec(KSIM_ORDER[i], t, KSIM_POSXK[KSIM_ORDER[i]], 0);
        KSIM_VAL(KSIM_ORDER[i], t) = x;
        // if(!L_ISAN(x)) return(-1);  
    }
    return(0);
}


/**
 *  At the end of a failed simulation, K_diverge() creates a list containing all the equations 
 *  whose difference between the 2 last iterations exceeds KSIM_EPS.
 *  
 *  @param [in] int         t       current simulation period
 *  @param [in] char*       lst     name of the list to create
 *  @param [in] IODE_REAL   eps     convergence threshold
 *  @return     int                 0 on success, -1 if at least one equation returns L_NAN. 
 *  TODO: 
 *      Why returning -1 
 *      if NA, stop creating _DIVER ?
 *      replace B_ fns by K_ fns (see comments)
 *  
 */
static int K_diverge(int t, char* lst, IODE_REAL eps)
{
    char        buf[81];
    char        *diverg = NULL;
    int         i, j, pos;
    IODE_REAL   x;
    double      d, pd, fpd1, fpd2;

    // Delete lst 
    //if(B_DataExist(lst, K_LST) >= 0) B_DataDelete(lst, K_LST);
    pos = K_find(KL_WS, lst);
    if(pos >= 0) K_del(KL_WS, pos);
    
    for(i = KSIM_PRE, j = 0; j < KSIM_INTER; i++, j++)  {
        /* save XK first */
        KSIM_XK[j] = KSIM_VAL(KSIM_ORDER[i], t);

        /* execute lec */
        x = K_calc_clec(KSIM_ORDER[i], t, KSIM_POSXK[KSIM_ORDER[i]], 1);
        if(!L_ISAN(x)) return(-1); // TODO: Add to _DIVER instead ?

        /* Check convergence */
        if(L_ISAN(KSIM_XK[j])) {
            /* ?????????????
            d = (KSIM_XK[j] - x) * KSIM_RELAX;
            */

            d = (KSIM_XK[j] - x);
            if(!L_IS0(KSIM_XK[j]))
                pd = min(fabs(1 - x / KSIM_XK[j]), fabs(d));
            else pd = fabs(d);

            pd *= KSIM_RELAX;
            if(pd > eps)  {
                if(diverg) diverg = SCR_strafcat(diverg, ",");
                diverg = SCR_strafcat(diverg, KSIM_NAME(KSIM_ORDER[i]));
                //sprintf(buf, "%s %s", lst, KSIM_NAME(KSIM_ORDER[i]));
                //B_DataAppend(buf, K_LST);
            }
        }
    }
    if(diverg) K_add(KL_WS, lst, diverg);
    return(0);
}


/**
 *  Simulates one period of the current model (see K_simul() for more details on the model definition).
 *  
 *  The initial values of the endogenous variables are set before starting the process.
 *  
 *  At the end of the function, the KSIM_NITERS[t] and KSIM_NORMS[t] are set to
 *  memorize the number of iterations and the level of convergence reached. These values can be
 *  saved via the report functions $ModelSimulateSaveNiters and $ModelSimulateSaveNorms.
 *  
 *  The super-functions ktermvkey(), khitkey() and kconfirm() are called to allow user interruptions.
 *  The super-function kmsg() is called to display the result at the end of each iteration.
 *  
 *  @param [in] int     t   index of the period to simulated
 *  @return     int         0 no success, 
 *                          -1 on error (the algorithm does not reach a solution or a NaN value is generated).
 *  
 *  @see K_simul() for the global input and output globlals.
 */
int K_simul_1(int t)
{
    extern  int SCR_vtime;
    int     it = 0, rc, conv = 0, ovtime = SCR_vtime; /* JMP 27-09-96 */
    char    buf[10], msg[80];

    K_init_values(t);
    KSIM_NITERS[t] = 0; 
    KSIM_NORMS[t] = 0;  
    if(K_prolog(t)) return(-1);
    
    ktermvkey(0); // Force the interval between 2 keyboard readings to 0 ms
    while(conv == 0 && it++ < KSIM_MAXIT) {
        rc = K_interdep(t);
        KSIM_NITERS[t]++; 			
        KSIM_NORMS[t] = KSIM_NORM;	
        if(rc) {
            ktermvkey(ovtime);  // Resets the interval between 2 keyboard readings
            return(-1);
        }
        PER_pertoa(PER_addper(&(KSMPL(KSIM_DBV)->s_p1), t), buf);
        sprintf(msg, "%s %d iterations ; error = %8.4lg", buf, it, KSIM_NORM);
        kmsg("%.80s", msg);
        conv = (KSIM_NORM <= KSIM_EPS) ? 1 : 0;
        if(khitkey() != 0) {                    // Checks the keyboard for a buffered key 
            kgetkey();                          // Reads the keyboard buffer
            if(!kconfirm("Stop Simulation")) {  
                K_restore_XK(t);
                ktermvkey(ovtime); 
                return(-1);
            }
        }
    }
    ktermvkey(ovtime);  // Resets the interval between 2 keyboard readings

    if(conv) {
        K_restore_XK(t);
        if(K_epilog(t)) return(-1);
        return(0);
    }
    else {
        B_seterrn(114, KSIM_MAXIT, "_DIVER");
        K_restore_XK(t);
        K_diverge(t, "_DIVER", KSIM_EPS); // Saves the list of non convergent eqs in the list _DIVER
    }

    return(-1);
}


/**
 *  Simulates a model defined by a set of equations and optionaly a list of replacements endo-exo. 
 *  
 *  If a list of couples endo-exo is defined in the table char** endo_exo, the model is solved with respect 
 *  to a new set of variables = all endogenous variables except the endos in the list endo_exo 
 *  plus the list of exogenous specified in the list endo-exo.
 *  
 *  The algorithm used to solve the model for one period is Gauss-Seidel or a secant method.
 *  
 *  @param [in] KDB*    dbe         KE_WS or subset of KE_WS containing all the model equations
 *  @param [in] KDB*    dbv         KDB containing the model variables (endo + exo)
 *  @param [in] KDB*    dbs         KDB containing the model scalars
 *  @param [in] SAMPLE* smpl        simulation SAMPLE
 *  @param [in] char**  endo_exo    optional goal-seeking specification: list of strings in the form "endo1-exo1,endo2-exo2..." 
 *                                      where endo1 and endo2 become exogenous (instead of endogenous) 
 *                                      and exo1, exo2 replace endo1 and endo2 as endogenous to the model.
 *                                      NULL for a normal simulation (model solved with respect to default endogenous) 
 *  @param [in] char**  eqs         set of equations defining the model to simulate, not necessarily in alphabetic order
 *                                  NULL if the order must be calculated by K_simul()
 *  @return     int                 0 on succes, -1 on error            
 *  
 *  @global [out] IODE_REAL   *KSIM_NORMS     convergence threshold reached at the end of each simulation period
 *  @global [out] int		  *KSIM_NITERS    Numbers of iterations needed for each simulation period
 *  @global [in]  IODE_REAL   KSIM_EPS        Required max convergence threshold
 *  @global [in]  IODE_REAL   KSIM_RELAX      Relaxation parameter
 *  @global [in]  int         KSIM_MAXIT      Maximum number of iteration to reach a solution   
 *  @global [in]  int         KSIM_DEBUG      Debug level: 0 = no debugging output
 *  @global [in]  int         KSIM_SORT       reordering option : SORT_NONE, SORT_CONNEX or SORT_BOTH  
 *  
 *  @note Objects in a KDB are stored in alphabetic order, which is not efficient for the Gauss-Seidel solver.
 *        When the parameter eqs is not NULL, and the global KSIM_SORT == SORT_NONE, 
 *        the simulation order is left untouched before starting the Gauss-Seidel iterations.
 *
 */
int K_simul(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** endo_exo, char** eqs)
{
    int     i, t, j, k, endo_exonb, lg,
            posendo, posexo,
            rc = -1;
    char    **var = NULL;
    IODE_REAL    *x;

    if(KNB(dbe) == 0) {
        B_seterrn(110);
        return(rc);
    }

    // Assign static global variables to avoid passing to many parameters to sub functions
    KSIM_DBV = dbv;
    KSIM_DBE = dbe;
    KSIM_MAXDEPTH = KNB(dbe);
    KSIM_DBS = dbs;

    // Find in the KSIM_DBV sample the position t of the first period to simulate
    // and check that the simulation sample is included in KSIM_DBV sample
    t = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));
    if(t < 0 || PER_diff_per(&(KSMPL(dbv)->s_p2), &(smpl->s_p2)) < 0) {
        B_seterrn(111);
        return(rc);
    }

    // KSIM_POSXK[i] = pos in KSIM_DBV of the endo of equation i (endo var = eq name)
    KSIM_POSXK = (int *) SW_nalloc((int)(sizeof(int) * KNB(dbe)));

    // Initialize KSIM_NORMS and KSIM_NITERS (see definitions above) 
    SCR_free(KSIM_NORMS);
    SCR_free(KSIM_NITERS);
    KSIM_NORMS = (IODE_REAL *) SCR_malloc(sizeof(IODE_REAL) * KSMPL(dbv)->s_nb);
    KSIM_NITERS = (int *) SCR_malloc(sizeof(int) * KSMPL(dbv)->s_nb);

    // LINK EQUATIONS + SAVE ENDO POSITIONS 
    kmsg("Linking equations ....");
    for(i = 0 ; i < KNB(dbe); i++) {
        KSIM_POSXK[i] = K_find(dbv, KONAME(dbe,i));
        if(KSIM_POSXK[i] < 0) {
            B_seterrn(112, KONAME(dbe, i));
            rc = -1;
            goto fin;
        }

        rc = L_link(dbv, dbs, KECLEC(dbe, i));
        if(rc) {
            B_seterrn(113, KONAME(dbe, i));
            rc = -1;
            goto fin;
        }
    }

    // Optional goal seeking = exchange exo and endo roles in equations
    // Each couple endo-exo
    if(endo_exo != NULL) {
        endo_exonb = SCR_tbl_size(endo_exo);
        KSIM_PATH = SW_nalloc(KSIM_MAXDEPTH);
        for(i = 0; i < endo_exonb; i ++) {
            var = SCR_vtom(endo_exo[i], '-');
            if(var == NULL || SCR_tbl_size(var) != 2) {
                B_seterrn(115, endo_exo[i]);
                rc = -1;
                goto fin;
            }

            posendo = K_find(KSIM_DBV, var[0]); // Position of the endogenous var in dbv
            posexo = K_find(KSIM_DBV, var[1]);  // Position of the exogenous var in dbv
            //         fprintf(stdout, "\n====Exchanging %s %s\n====", var[0], var[1]);
            if(KE_exo2endo(posendo, posexo) < 0) {
                rc = -1;
                goto fin;
            }

            //          fprintf(stdout, "\nDone");
            SCR_free_tbl(var);
            var = NULL;
        }

    }

    // ORDERING EQUATIONS 
    KE_order(dbe, eqs);
    if(KSIM_DEBUG) K_lstorder("_PRE", "_INTER", "_POST");

    // SIMULATE 
    KSIM_XK  = (IODE_REAL *) SW_nalloc(sizeof(IODE_REAL) * KSIM_INTER);
    KSIM_XK1 = (IODE_REAL *) SW_nalloc(sizeof(IODE_REAL) * KSIM_INTER);

    for(i = 0; i < smpl->s_nb; i++, t++) {
        if(rc = K_simul_1(t)) goto fin;
        
        // In case of exchange ENDO-EXO, initialises the future EXO's => exo[t+i] = exo[t] i=t+1..end of sample
        if(endo_exo != NULL) {
            for(k = 0; k < endo_exonb; k ++) {
                var = SCR_vtom(endo_exo[k], '-');
                posexo = K_find(KSIM_DBV, var[1]);

                x = KVVAL(KSIM_DBV, posexo, 0);
                for(j = t + 1; j < KSMPL(dbv)->s_nb; j++)  x[j] = x[t];

                SCR_free_tbl(var);
                var = NULL;
            }
        }
    }

fin:
    SCR_free_tbl(var);
    K_simul_free();
    return(rc);
}

 
/**
 *  Tries to find a value for varnb[t] that satifies the equality in the equation eqnb. 
 *  
 *  If the varnb is not the endogenous variable of the equation eqnb or if the equation was not analytically 
 *  solved with respect to its endogenous, the funtion L_zero() is called to (try to) solve the equation numerically.
 *  
 *  The equation must be linked before the call to K_calc_clec().
 *  
 *  If no solution can be found, the function kerror() is called to display an error message
 *  
 *  @param [in] int         eqnb    position of the equation in KSIM_DBE (the model KDB = subset of KE_WS)
 *  @param [in] int         t       index of the period to be calculated
 *  @param [in] int         varnb   position of the variable to calculate in the global KV_DB
 *  @param [in] int         msg     indicated if the function kerror() must be called on error (no solution found)
 *  @return     IODE_REAL           on succes: the equation solution
 *                                  on error of if no solution can be found: L_NAN 
 *  
 *  TODO: find a quicker solution (avoid CLEC allocation for example)
 */

IODE_REAL K_calc_clec(int eqnb, int t, int varnb, int msg)
{
    int     lg, eqvarnb = -1;
    CLEC    *clec;
    IODE_REAL    x;
    char    buf[10];

//Debug("Eq %s\n", KONAME(KSIM_DBE, eqnb));
    lg = KECLEC(KSIM_DBE, eqnb)->tot_lg;
    clec = (CLEC*) SW_nalloc(lg);
    memcpy(clec, KECLEC(KSIM_DBE, eqnb), lg);
    eqvarnb = K_find(KSIM_DBV, KONAME(KSIM_DBE, eqnb));
    if(clec->dupendo || varnb != eqvarnb)
        x = L_zero(KSIM_DBV, KSIM_DBS, clec, t, varnb, eqvarnb);
    else
        x = L_exec(KSIM_DBV, KSIM_DBS, clec, t);
    if(!L_ISAN(x) && msg)
        kerror(0, "%s : becomes unavailable at %s%s",
               KONAME(KSIM_DBV, varnb), /* JMP 16-06-99 a la place de eqvarnb */
               PER_pertoa(PER_addper(&(KSMPL(KSIM_DBV)->s_p1), t), buf),
               ((clec->dupendo || varnb != eqvarnb) ? "(Newton)" : "")
              );
    SW_nfree(clec);
    return(x);
}


/**
 *  Creates or updates a list of equations from equation KSIM_ORDER[eq1] to equation KSIM_ORDER[eqn].
 *  
 *  Sub-function  of K_lstorder().
 *  
 *  @param [in] char*   lstname     name of the list to be created / updated    
 *  @param [in] int     eq1         first equation position in KSIM_ORDER (name = KSIM_NAME[KSIM_ORDER[eq1]])
 *  @param [in] int     eqn         last equation pos in KSIM_ORDER (name = KSIM_NAME[KSIM_ORDER[eqn]])
 *  
 */

static void K_lstorder_1(char* lstname, int eq1, int eqn)
{
    U_ch 	        **lst = 0,                     
                    **tbl_todel;
    U_ch            *lst_todel,
                    buf[256];
    int   	        i, 
                    nlst = 0, 
                    nb = eqn - eq1 + 1,  
                    maxl = 1000;
    extern  KDB*    KSIM_DBV;

    // D�truit la liste cible et les sous-listes
    sprintf(buf, "%s*", lstname);
    //B_DataDelete(buf, K_LST); // Old version usign B_*() fns
    lst_todel = K_expand(K_LST, NULL, buf, '*');
    if(lst_todel) {
        tbl_todel = SCR_vtom(lst_todel, ';');
        for(i = 0; tbl_todel[i] ; i++)
            K_del_by_name(KL_WS, tbl_todel[i]);
    }
    SCR_free(lst_todel);
    SCR_free_tbl(tbl_todel);
    
    // Creates a table of strings containing all the name to set in the list
    for(i = 0; i < nb; i++)
        SCR_add_ptr(&lst, &nlst, KSIM_NAME(KSIM_ORDER[i + eq1]));

    SCR_add_ptr(&lst, &nlst, 0); 

    // Creates the list lstname (and possibly sub-lists lstname1,...) 
    KL_lst(lstname, lst, maxl);
    SCR_free_tbl(lst);
}


/**
 *  Creates 3 lists of equations: the prolog, the epilog and the interdependent part of the model.
 *  
 *  Called by K_simul() if KSIM_DEBUG is not null: creates the lists _PRE, _INTER and _POST.
 *   
 *  @param [in] char*   pre     name of the list containing the prolog  
 *  @param [in] char*   inter   name of the list containing the interdep
 *  @param [in] char*   post    name of the list containing the epilog
 *  
 */
void K_lstorder(char* pre, char* inter, char *post)
{
    K_lstorder_1(pre,   0, 					   KSIM_PRE - 1);
    K_lstorder_1(inter, KSIM_PRE, 			   KSIM_PRE + KSIM_INTER - 1);
    K_lstorder_1(post,  KSIM_PRE + KSIM_INTER, KSIM_PRE + KSIM_INTER + KSIM_POST - 1);
}


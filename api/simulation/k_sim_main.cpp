/** 
 * @header4iode
 *
 *   The Gauss-Seidel algorithm
 *   ==========================
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
 */

#include "api/pch.h"
#include "api/k_super.h"
#include "api/b_errors.h"
#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/grep.h"
#include "api/objs/equations.h"
#include "api/objs/lists.h"
#include "api/objs/variables.h"
#include "api/simulation/simulation.h"


extern "C" int SCR_vtime;

/**
 * 
 *  Initialises the values of the interdependent endogenous variables for one period before starting 
 *  the solver on that period.
 *  
 *  @see KV_init_values_1() for the available initialisation methods.
 *  
 *  @param [in] int     t           period to initialise
 *  @global     int     init_algo  initialisation method 
 *  
 */
void CSimulation::init_values(int t)
{
    if(init_algo == VAR_INIT_ASIS) 
        return;

    double* val;
    std::string name;
    for(int i = 0 ; i < nb_pre + nb_inter + nb_post; i++) 
    {
        name = sim_dbv->get_name(v_pos_endo_in_dbv[v_order[i]]);
        val = sim_dbv->get_var_ptr(name);
        KV_init_values_1(val, t, init_algo);
    }
}

/**
 *  Restore the endo values of the interdependent block by setting their values before the last iteration
 *  (saved in v_endo_values):
 *  
 *      ENDO[i,t]=v_endo_values[i]
 *  
 *  @param  [in]  int  t   index of the period where the data must be copied
 *  
 */
void CSimulation::restore_XK(int t)
{
    int i, j;
    for(i = nb_pre, j = 0; j < nb_inter; i++, j++)
        KSIM_SET_VAL(v_order[i], t, v_endo_values[j]);
}

/**
 *  Calculates the first non interdependent part of the model (the "prolog").
 *  The solution of each equation is saved in the global sim_dbv.
 *  
 *  @param [in] int     t   period to calculated
 *  @return     int         0 (should be IODE_NAN on error ?)
 *  
 */
int CSimulation::prolog(int t)
{
    double x;
    for(int i = 0; i < nb_pre; i++)  
    {
        x = calculate_CLEC(v_order[i], t, v_pos_endo_in_dbv[v_order[i]], 0);
        KSIM_SET_VAL(v_order[i], t, x);
    }

    return 0;
}


/**
 *  Tries to solve the interdependent block of the model using a modified Gauss-Seidel algorithm.
 *  
 *  For eq nb i:
 *    - saves the previous iteration value of the endogenous variable in v_endo_values[i]
 *    - computes the new value of the endo var and saves it into sim_dbv (via KSIM_SET_VAL) 
 *    - if required, modifies the resulting value by "relaxing" it (multiply by relax)
 *    - computes the ||f(x)|| = diff between the new endo value and the value of the previous iteration 
 *      and saves that value in norm. 
 *  
 *  @param [in]     int         t            simulated period
 *  @return         int                      0 if the equation return a real value
 *                                           -1 if the equation returns IODE_NAN
 *  @global [out]   double   norm    maximum difference bw endos before and after iteration
 *  
 */
int CSimulation::sub_interdep_1(int t)
{
    int     i, j;
    double  x;
    double  d, pd;


    norm = 0.0;
    for(i = nb_pre, j = 0; j < nb_inter; i++, j++)  
    {
        /* save XK first */
        v_endo_values[j] = KSIM_VAL(v_order[i], t);

        /* execute lec */
        x = calculate_CLEC(v_order[i], t, v_pos_endo_in_dbv[v_order[i]], 1);
        if(!IODE_IS_A_NUMBER(x)) 
            return -1;

        /* Check convergence */
        if(IODE_IS_A_NUMBER(v_endo_values[j])) 
        {
            d = (v_endo_values[j] - x);   // d = diff between 2 iterations
            if(!IODE_IS_0(v_endo_values[j]))  
                pd = std::min(fabs(1 - x / v_endo_values[j]), fabs(d));   // if ||endo|| != 0, norm = relative difference
            else 
                pd = fabs(d);                                       // else norm = |d| 

            pd *= relax; 
            if(pd > norm) norm = pd;

            // Stores the new endo value and "relaxes" it 
            KSIM_SET_VAL(v_order[i], t, relax * (x - v_endo_values[j]) + v_endo_values[j]);
        }
        else 
        {
            // if NaN, set norm to a huge value 
            norm = 10;
            KSIM_SET_VAL(v_order[i], t, x);
        }
    }

    return 0;
}


/**
 *  Tries to solve the nonlinear equation system defined by the interdependent block of the model.
 *  
 *  This version applies the relaxation parameter at the end of a complete iteration of the model, 
 *  instead of directly after each equation calculation.
 *  
 *  A solution is reached if the difference between 2 iterations k and k+1 is less that norm. 
 *   
 *  @param [in]     int         t  index of the calculation period
 *  @return         int         -1 if the result of an equation is IODE_NAN
 *                              0 otherwise
 *  @global [out]   double   norm    maximum difference between 2 iterations
 */
int CSimulation::sub_interdep_2(int t)
{
    int     i, j;
    double  d, pd;

    // Stage 1
    for(i = nb_pre, j = 0; j < nb_inter; i++, j++)  
    {
        /* save XK for further use */
        v_endo_values[j] = KSIM_VAL(v_order[i], t);

        /* execute lec and save in v_endo_values_1 */
        v_endo_values_1[j] = calculate_CLEC(v_order[i], t, v_pos_endo_in_dbv[v_order[i]], 1);
        // NaN value --> stop simulation
        if(!IODE_IS_A_NUMBER(v_endo_values_1[j])) 
            return -1;
    }

    // Stage 2
    norm = 0.0;
    for(i = nb_pre, j = 0; j < nb_inter; i++, j++)  
    {
        // Valeur précédente définie
        if(IODE_IS_A_NUMBER(v_endo_values[j])) 
        {
            d = v_endo_values[j] - v_endo_values_1[j]; // Diff between iterations

            // Calcule la 'norme' = fabs de la différence relative entre 2 it.
            //   ou de la diff entre 2 it.
            if(!IODE_IS_0(v_endo_values[j]))
                pd = std::min(fabs(1 - v_endo_values_1[j] / v_endo_values[j]), fabs(d));
            else
                pd = fabs(d);

            // norme : la plus grande pour le mod.
            pd *= relax;
            if(pd > norm) 
                norm = pd;

            /* Store new value and relax it */
            KSIM_SET_VAL(v_order[i], t, relax * v_endo_values_1[j] + (1 - relax) * v_endo_values[j]);
        }
        // If previous iteation value is L-NAN, set norm to 10 and assing new calc value to endo
        else 
        {
            norm = 10;
            KSIM_SET_VAL(v_order[i], t, v_endo_values_1[j]);
        }
    }

    return 0;
}


/**
 *  Solves the interdependent part of the model by a modified Gauss-Seidel algorithm.
 *  
 *  Two methods are available depending on relax sign: 
 *      - if relax > 0: sub_interdep_1() that "relaxes" each endo directly after its calculation
 *      - else:              sub_interdep_2() that waits for the end of the iteration before "relaxing" 
 *                              all endos at the same time
 *  
 *  @param [in] int     t   period to solve (=simulate)
 *  @return     int         0 on success, -1 on error
 *  
 */
int CSimulation::interdep(int t)
{
    int rc = 0;
    double signed_relax = relax;

    relax = fabs(signed_relax);
    if(signed_relax >= 0)      
        rc = sub_interdep_1(t);
    else                
        rc = sub_interdep_2(t);

    relax = signed_relax;
    return rc;
}


/**
 *  Calculates the last non interdependent (post recursive) part of the model.
 *  
 *  @param [in] int     t   period to calculated
 *  @return     int         0 (should be IODE_NAN on error ?)
 *  
 */
int CSimulation::epilog(int t)
{
    int     i, j;
    double  x;

    for(i = nb_pre + nb_inter, j = 0; j < nb_post; i++, j++)  
    {
        x = calculate_CLEC(v_order[i], t, v_pos_endo_in_dbv[v_order[i]], 0);
        KSIM_SET_VAL(v_order[i], t, x);  
    }

    return 0;
}


/**
 *  At the end of a failed simulation, diverge() creates a list containing all the equations 
 *  whose difference between the 2 last iterations exceeds epsilon.
 *  
 *  @param [in] int         t       current simulation period
 *  @param [in] char*       lst     name of the list to create
 *  @param [in] double   eps     convergence threshold
 *  @return     int                 0 on success, -1 if at least one equation returns IODE_NAN. 
 *  TODO: 
 *      Why returning -1 
 *      if NA, stop creating _DIVER ?
 *      replace B_ fns by K_ fns (see comments)
 *  
 */
int CSimulation::diverge(int t, char* c_name, double eps)
{
    char        *diverg = NULL;
    int         i, j;
    double      x;
    double      d, pd;

    // Delete name 
    std::string name(c_name);
    if(global_ws_lst->contains(name))
        global_ws_lst->remove(name);
    
    for(i = nb_pre, j = 0; j < nb_inter; i++, j++)  
    {
        /* save XK first */
        v_endo_values[j] = KSIM_VAL(v_order[i], t);

        /* execute lec */
        x = calculate_CLEC(v_order[i], t, v_pos_endo_in_dbv[v_order[i]], 1);
        if(!IODE_IS_A_NUMBER(x)) return -1; // TODO: Add to _DIVER instead ?

        /* Check convergence */
        if(IODE_IS_A_NUMBER(v_endo_values[j])) {
            /* ?????????????
            d = (v_endo_values[j] - x) * relax;
            */

            d = (v_endo_values[j] - x);
            if(!IODE_IS_0(v_endo_values[j]))
                pd = std::min(fabs(1 - x / v_endo_values[j]), fabs(d));
            else pd = fabs(d);

            pd *= relax;
            if(pd > eps)  
            {
                if(diverg) 
                    diverg = (char*) SCR_strafcat((unsigned char*) diverg, (unsigned char*) ",");
                diverg = (char*) SCR_strafcat((unsigned char*) diverg, (unsigned char*) KSIM_NAME(v_order[i]).c_str());
            }
        }
    }
    
    if(diverg)
    {
        List lst(diverg);
        global_ws_lst->set(name, lst);
    }
    
    return 0;
}


/**
 *  Simulates one period of the current model (see simulate() for more details on the model definition).
 *  
 *  The initial values of the endogenous variables are set before starting the process.
 *  
 *  At the end of the function, the v_nb_iterations[t], v_norm[t], ... are set to
 *  memorize the number of iterations, the level of convergence reached... These values can be
 *  saved via the report functions $ModelSimulateSaveNiters and $ModelSimulateSaveNorms.
 *  
 *  The super-functions ktermvkey(), khitkey() and kconfirm() are called to allow user interruptions.
 *  The super-function kmsg() is called to display the result at the end of each iteration.
 *  
 *  @param [in] int     t   index of the period to simulated
 *  @return     int         0 no success, 
 *                          -1 on error (the algorithm does not reach a solution or a NaN value is generated).
 */
int CSimulation::sub_simulate(int t)
{
    int     it = 0, rc, conv = 0, ovtime = SCR_vtime;
    char    msg[80];
    long    ms_iter;

    init_values(t);
    v_nb_iterations[t] = 0; 
    v_norm[t] = 0;  
    v_cpu_time[t] = 0;  
    if(prolog(t)) 
        return -1;
    
    ktermvkey(0); // Force the interval between 2 keyboard readings to 0 ms
    while(conv == 0 && it++ < max_iter) 
    {
        ms_iter = WscrGetMS();
        rc = interdep(t);
        v_nb_iterations[t]++; 			
        v_norm[t] = norm;	
        if(rc) 
        {
            ktermvkey(ovtime);  // Resets the interval between 2 keyboard readings
            return -1;
        }
        Period period = sim_dbv->get_sample()->start_period.shift(t);
        sprintf(msg, "%s: %d iters - error = %8.4lg - cpu=%ldms", 
                      (char*) period.to_string().c_str(), it, norm, 
                      WscrGetMS() - ms_iter);
        kmsg("%.80s", msg);
        conv = (norm <= epsilon) ? 1 : 0;
        if(khitkey() != 0) 
        {   // Checks the keyboard for a buffered key 
            kgetkey();      // Reads the keyboard buffer
            if(!kconfirm("Stop Simulation")) 
            {  
                restore_XK(t);
                ktermvkey(ovtime); 
                return -1;
            }
        }
    }
    ktermvkey(ovtime);  // Resets the interval between 2 keyboard readings

    if(conv) 
    {
        restore_XK(t);
        if(epilog(t)) 
            return -1;
        return 0;
    }
    else 
    {
        std::string err_msg = "Model does not converge after ";
        err_msg += std::to_string(max_iter);
        err_msg += " iterations";
        error_manager.append_error(err_msg);
        restore_XK(t);
        diverge(t, "_DIVER", epsilon); // Saves the list of non convergent eqs in the list _DIVER
    }

    return -1;
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
 *  @param [in] KDB*    dbe         global_ws_eqs or subset of global_ws_eqs containing all the model equations
 *  @param [in] KDB*    dbv         KDB containing the model variables (endo + exo)
 *  @param [in] KDB*    dbs         KDB containing the model scalars
 *  @param [in] Sample* smpl        simulation Sample 
 *  @param [in] char**  eqs         set of equations defining the model to simulate, not necessarily in alphabetic order
 *                                  NULL if the order must be calculated by simulate()
 *  @return     bool                            
 *  
 *  @global [out] double   *v_norm     convergence threshold reached at the end of each simulation period
 *  @global [out] int		  *v_nb_iterations    Numbers of iterations needed for each simulation period
 *  @global [out] int		  *cpu_time      CPU needed for each simulation period
 *  @global [in]  double   epsilon        Required max convergence threshold
 *  @global [in]  double   relax      Relaxation parameter
 *  @global [in]  int         max_iter      Maximum number of iteration to reach a solution   
 *  @global [in]  int         debug      Debug level: 0 = no debugging output
 *  @global [in]  int         sorting_algo       reordering option : SORT_NONE, SORT_CONNEX or SORT_BOTH  
 *  
 *  @note Objects in a KDB are stored in alphabetic order, which is not efficient for the Gauss-Seidel solver.
 *        When the parameter eqs is not NULL, and the global sorting_algo == SORT_NONE, 
 *        the simulation order is left untouched before starting the Gauss-Seidel iterations.
 */
bool CSimulation::simulate(KDBEquationsPtr dbe, KDBVariablesPtr dbv, KDBScalarsPtr dbs, Sample* smpl, 
    const std::vector<std::string>& eqs)
{
    bool success = true;
    int     i, t, bt, at, j, k, res, endo_exonb,
            posendo, posexo, posvar, cpu_iter;
    double  *x;
    std::string var_name, var_exo;
    std::vector<std::string> pair_endo_exo;

    if(dbe->size() == 0) 
    {
        std::string err_msg = "Empty set of equations";
        error_manager.append_error(err_msg);
        return false;
    }

    // Assign static global variables to avoid passing to many parameters to sub functions
    sim_dbv = dbv;
    sim_dbe = dbe;
    max_depth = dbe->size();
    sim_dbs = dbs;

    // Find in the sim_dbv sample the position t of the first period to simulate
    // and check that the simulation sample is included in sim_dbv sample
    at = smpl->start_period.difference(dbv->get_sample()->start_period);
    bt = dbv->get_sample()->end_period.difference(smpl->end_period);
    if(bt < 0 || at < 0) 
    {
        std::string err_msg = "Simulation sample out of the Variables sample boundaries";
        error_manager.append_error(err_msg);
        return false;
    }
    t = at; // t = index of the first period to simulate

    // v_pos_endo_in_dbv[i] = pos in sim_dbv of the endo of equation i (endo var = eq name)
    // v_pos_endo_in_dbe[i] = pos in sim_dbe of the eq whose endo is var[i] 
    v_pos_endo_in_dbv.clear();
    v_pos_endo_in_dbe.clear();
    v_pos_endo_in_dbv.resize(dbe->size(), -1);
    v_pos_endo_in_dbe.resize(dbv->size(), -1);
    for(i = 0 ; i < dbv->size(); i++) 
        v_pos_endo_in_dbe[i] = -1;

    // Initialize v_norm and v_nb_iterations (see definitions above) 
    v_norm.clear();
    v_nb_iterations.clear();
    v_cpu_time.clear();
    v_norm.resize(dbv->get_sample()->nb_periods, 0.0);
    v_nb_iterations.resize(dbv->get_sample()->nb_periods, 0);
    v_cpu_time.resize(dbv->get_sample()->nb_periods, 0);

    // LINK EQUATIONS + SAVE ENDO POSITIONS 
    kmsg("Linking equations ....");
    
    int rc = 0;
    std::string eq_name;
    std::shared_ptr<Equation> eq_ptr;
    for(i = 0 ; i < dbe->size(); i++) 
    {
        eq_name = dbe->get_name(i);   
        posvar = dbv->index_of(eq_name);
        v_pos_endo_in_dbv[i] = posvar;
        if(posvar < 0) 
        {
            std::string err_msg = std::string("'") + eq_name + "': cannot find variable";
            error_manager.append_error(err_msg);
            success = false;
            goto fin;
        }
        v_pos_endo_in_dbe[posvar] = i; // Position of equation with endo nb posvar = i
        
        eq_ptr = dbe->get_obj_ptr(eq_name);
        eq_ptr->compile();
        rc = eq_ptr->clec->link(dbv, dbs);
        if(rc) 
        {
            std::string err_msg = std::string("'") + eq_name + "': cannot link equation";
            error_manager.append_error(err_msg);
            success = false;
            goto fin;
        }
    }

    // Optional goal seeking = exchange exo and endo roles in equations
    // Each couple endo-exo
    if(!v_endo_exo.empty()) 
    {
        v_path.clear();
        v_path.resize(max_depth, false);
        endo_exonb = (int) v_endo_exo.size();
        for(i = 0; i < endo_exonb; i ++) 
        {
            pair_endo_exo = split(v_endo_exo[i], '-');
            if(pair_endo_exo.size() != 2) 
            {
                std::string err_msg = v_endo_exo[i] + ": syntax error in goal seeking parameter";
                error_manager.append_error(err_msg);
                success = false;
                goto fin;
            }

            var_name = pair_endo_exo[0];
            posendo = sim_dbv->index_of(var_name);   // Position of the endogenous var in dbv
            if(posendo < 0) 
            {
                std::string err_msg = "Goal Seeking: '";
                err_msg += var_name;
                err_msg += "': no such equation in the Equations workspace";
                error_manager.append_error(err_msg);
                success = false;
                goto fin;
            }

            var_name = pair_endo_exo[1];
            posexo = sim_dbv->index_of(var_name);  // Position of the exogenous var in dbv
            if(posexo < 0) 
            {
                std::string err_msg = std::string("'") + var_name + "': cannot find variable";
                error_manager.append_error(err_msg);
                success = false;
                goto fin;
            }
            
            res = exo_to_endo(posendo, posexo);
            if(res < 0) 
            {
                success = false;
                goto fin;
            }
        }
    }

    // ORDERING EQUATIONS 
    order(dbe, eqs);
    if(debug) 
        build_lists_order("_PRE", "_INTER", "_POST");

    // SIMULATE 
    v_endo_values.clear();
    v_endo_values_1.clear();
    v_endo_values.resize(nb_inter, 0.0);
    v_endo_values_1.resize(nb_inter, 0.0);

    for(i = 0; i < smpl->nb_periods; i++, t++) 
    {
        cpu_iter = WscrGetMS();

        rc = sub_simulate(t);
        success = (rc == 0) ? true : false;
        if(!success) 
            goto fin;
        
        v_cpu_time[t] = WscrGetMS() - cpu_iter;
        // In case of exchange ENDO-EXO, initialises the future EXO's => exo[t+i] = exo[t] i=t+1..end of sample
        if(!v_endo_exo.empty()) 
        {
            for(k = 0; k < endo_exonb; k ++) 
            {
                pair_endo_exo = split(v_endo_exo[k], '-');
                var_name = pair_endo_exo[1];
                posexo = sim_dbv->index_of(var_name);  // Position of the exogenous var in dbv

                var_exo = sim_dbv->get_name(posexo);
                x = sim_dbv->get_var_ptr(var_exo);
                for(j = t + 1; j < dbv->get_sample()->nb_periods; j++)  
                    x[j] = x[t];
            }
        }
    }

fin:
    clear();
    return success;
}

 
/**
 *  Tries to find a value for varnb[t] that satifies the equality in the equation eqnb. 
 *  
 *  If the varnb is not the endogenous variable of the equation eqnb or if the equation was not analytically 
 *  solved with respect to its endogenous, the funtion zero() is called to (try to) solve the equation numerically.
 *  
 *  The equation must be linked before the call to calculate_CLEC().
 *  
 *  If no solution can be found, the function kerror() is called to display an error message
 *  
 *  @param [in] int         eqnb    position of the equation in sim_dbe (the model KDB = subset of global_ws_eqs)
 *  @param [in] int         t       index of the period to be calculated
 *  @param [in] int         varnb   position of the variable to calculate in the global KV_DB
 *  @param [in] int         msg     indicated if the function kerror() must be called on error (no solution found)
 *  @return     double           on succes: the equation solution
 *                                  on error of if no solution can be found: IODE_NAN 
 *  
 *  TODO: find a quicker solution (avoid CLEC allocation for example)
 */
double CSimulation::calculate_CLEC(int eqnb, int t, int varnb, int msg)
{
    int eqvarnb = -1;
    double x;

    std::string eq_name = sim_dbe->get_name(eqnb);
    std::shared_ptr<Equation> eq = sim_dbe->get_obj_ptr(eq_name);
    if(!eq)
        return IODE_NAN;
    
    std::shared_ptr<CLEC> eq_clec = eq->clec;
    if(!eq_clec)
        return IODE_NAN;
    
    std::shared_ptr<CLEC> clec = std::make_shared<CLEC>(*eq_clec);
    eqvarnb = sim_dbv->index_of(eq_name);
    if(clec->duplicated_endo || varnb != eqvarnb)
    {
        std::string var_name = sim_dbv->get_name(varnb);
        std::string eq_var_name = sim_dbv->get_name(eqvarnb);
        x = clec->zero(sim_dbv, sim_dbs, newton_step, newton_epsilon, newton_max_iter, 
                       t, var_name, eq_var_name);
    }
    else
        x = clec->execute(sim_dbv, sim_dbs, t);
    
    if(!IODE_IS_A_NUMBER(x) && msg)
    {
        Period period = sim_dbv->get_sample()->start_period.shift(t);
        kerror(0, "%s : becomes unavailable at %s%s",
               sim_dbv->get_name(varnb), /* JMP 16-06-99 a la place de eqvarnb */
               (char*) period.to_string().c_str(),
               ((clec->duplicated_endo || varnb != eqvarnb) ? "(Newton)" : "")
              );
    }
    
    return x;
}


/**
 *  Creates or updates a list of equations from equation v_order[eq1] to equation v_order[eqn].
 *  
 *  Sub-function  of build_lists_order().
 *  
 *  @param [in] char*   lstname     name of the list to be created / updated    
 *  @param [in] int     eq1         first equation position in v_order (name = KSIM_NAME[v_order[eq1]])
 *  @param [in] int     eqn         last equation pos in v_order (name = KSIM_NAME[v_order[eqn]])
 *  
 */
void CSimulation::sub_build_lists_order(const std::string& lstname, int eq1, int eqn)
{
    U_ch** lst = NULL;                     
    U_ch** tbl_todel = NULL;
    U_ch* lst_todel = NULL;
    U_ch buf[256];
    int i = 0; 
    int nlst = 0; 
    int nb = eqn - eq1 + 1;  
    int maxl = 1000;

    // delete the list 'lstname' and all sub-lists
    sprintf((char*) buf, "%s*", (char*) lstname.c_str());
    lst_todel = (unsigned char*) K_expand(LISTS, NULL, (char*) buf, '*');
    if(lst_todel) 
    {
        tbl_todel = SCR_vtom(lst_todel, ';');
        for(i = 0; tbl_todel[i] ; i++)
            global_ws_lst->remove(std::string((char*) tbl_todel[i]));
    }
    SCR_free(lst_todel);
    SCR_free_tbl(tbl_todel);
    
    // Creates a table of strings containing all the name to set in the list
    for(i = 0; i < nb; i++)
        SCR_add_ptr(&lst, &nlst, (unsigned char*) KSIM_NAME(v_order[i + eq1]).c_str());

    SCR_add_ptr(&lst, &nlst, 0); 

    // Creates the list lstname (and possibly sub-lists lstname1,...) 
    KL_lst((char*) lstname.c_str(), (char**) lst, maxl);
    SCR_free_tbl(lst);
}


/**
 *  Creates 3 lists of equations: the prolog, the epilog and the interdependent part of the model.
 *  
 *  Called by simulate() if debug is not null: creates the lists _PRE, _INTER and _POST.
 *   
 *  @param [in] char*   pre     name of the list containing the prolog  
 *  @param [in] char*   inter   name of the list containing the interdep
 *  @param [in] char*   post    name of the list containing the epilog
 *  
 */
void CSimulation::build_lists_order(const std::string& pre, const std::string& inter, const std::string& post)
{
    sub_build_lists_order(pre, 0, nb_pre - 1);
    sub_build_lists_order(inter, nb_pre, nb_pre + nb_inter - 1);
    sub_build_lists_order(post, nb_pre + nb_inter, nb_pre + nb_inter + nb_post - 1);
}

/**
 * Same as B_ModelExchange()
 * 
 * This "exchange" bw endos and exos variables allows to solve the model with respect to an alternate set of variables (some endos being replaced by exos).
 */
bool CSimulation::exchange(const std::string& list_endo_exo)
{
    v_endo_exo.clear();

    if(list_endo_exo.empty())
        return false;

    char** c_endo_exo = B_ainit_chk((char*) list_endo_exo.c_str(), NULL, 0);
    if(c_endo_exo == NULL && !list_endo_exo.empty()) 
    {
        std::string error_msg = "Cannot exchange the model variables:\n";
        error_msg += "Invalid list of endogenous-exogenous pairs: " + list_endo_exo;
        kwarning(error_msg.c_str());
        return false;
    }

    int nb = SCR_tbl_size((unsigned char**) c_endo_exo);
    for(int i = 0; i < nb; i++) 
        v_endo_exo.push_back(std::string(c_endo_exo[i]));
    SCR_free_tbl((unsigned char**) c_endo_exo);
    
    return true;
}

/**
 * Same as B_ModelCompile()
 * 
 * Equations containing lists in their LEC forms (eg: "Y := a + b * $LIST") must be "recompiled" when the value of $LIST is modified.
 * 
 * Note: rarely, if ever, used.
 */
bool CSimulation::compile(const std::string& list_eqs)
{
    // clear C API errors stack
    error_manager.clear();

    int rc = -1;

    std::string error_msg = "Could not compile the model";
    if(!list_eqs.empty()) 
        error_msg += " for the equations list '" + list_eqs + "'";
    error_msg += ":\n";

    // EndoExo whole WS
    if(list_eqs.empty()) 
        rc = KE_compile(*global_ws_eqs);
    else 
    {        
        // EndoExo whole WS
        if(list_eqs.empty()) 
            rc = KE_compile(*global_ws_eqs);
        else 
        {
            try
            {
                KDBEquationsPtr tdbe = global_ws_eqs->get_subset(list_eqs, false);
                if(tdbe->size() > 0)
                    rc = KE_compile(*tdbe);
            }
            catch(const std::exception& e)
            {
                error_msg += "\t" + std::string(e.what());
                kwarning(error_msg.c_str());
                return false;
            }

            if(rc < 0)
            {
                std::string error_msg = "Could not compile the model";
                error_manager.prepend_error(error_msg);
                kwarning(error_manager.get_all_errors().c_str());
                return false;
            }
        }
    }

    return true;
}

/**
 * Same as B_ModelSimulate()
 */
bool CSimulation::simulate(const std::string& from, const std::string& to, 
    const std::string& list_eqs)
{
    // clear C API errors stack
    error_manager.clear();

    std::string error_msg = "Cannot simulate the model";

    Sample* sample = nullptr;
    try
    {
        // throw exception if wrong parameters
        sample = new Sample(from, to);
    }
    catch(const std::exception& e)
    {
        error_msg += ":\n" + std::string(e.what());
        kwarning(error_msg.c_str());
        return false;
    }

    error_msg += " for the sample ";
    error_msg += "'" + from + ":" + to + "'";
    if(!list_eqs.empty()) 
        error_msg += " and for the equations list '" + list_eqs + "'";
    error_msg += ":";

    bool success = false;
    if(list_eqs.empty())
        success = simulate(global_ws_eqs, global_ws_var, global_ws_scl, sample);
    else 
    {
        try
        {
            KDBEquationsPtr tdbe = global_ws_eqs->get_subset(list_eqs, false);
            if(tdbe->size() > 0)
            {
                std::vector<std::string> v_eqs = eqs_to_vector(list_eqs);
                success = simulate(tdbe, global_ws_var, global_ws_scl, sample, v_eqs);
            }
        }
        catch(const std::exception& e)
        {
            error_msg += "\n\t" + std::string(e.what());
            kwarning(error_msg.c_str());
            if(sample) delete sample;
            return false;
        }
        
    }

    delete sample;

    if(!success)
    {
        error_manager.prepend_error(error_msg);
        kwarning(error_manager.get_all_errors().c_str());
        return false;
    }

    return true;
}

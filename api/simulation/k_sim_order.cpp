/** 
 * @header4iode
 *
 *  Functions to reorder a model to optimize the simulation algorithm. 
 *  
 *      1. Decomposes the model in 3 blocks:   
 *          - prolog (nb_pre), 
 *          - interdep (nb_inter)
 *          - epilog (nb_post)
 *      2. "pseudo-triangulates" the interdep block, i.e. inverts the equation order 
 *         to optimize the incidence matrix (i.e.: minimizing the nb of 1 above the diagonal).
 */
#include "api/pch.h"
#include "api/k_super.h"
#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/equations.h"
#include "api/simulation/simulation.h"


/**
 *  Adds the successor i to the list successors[pos] of successors of equation pos. 
 *  
 *  Let the equation eqi = A := B + C.
 *  If B is endo of equation  eqj
 *  Then add_post(successors, eqj, eqi) adds eqi to successors[eqj]
 *
 * At the end of the process, successors contains for each equation all dependent equations.
 *  
 *  @param [in, out]    int**   successors  vector of int*, one per equation containing the list of successors of eq nb i 
 *  @param [in]         int     i           successor position to add to the list successors[pos]
 *  @param [in]         int     pos         position of the predecessor of i
 *  
 */
int CSimulation::add_post(std::vector<std::vector<int>>& successors, int i, int pos)
{
    if(successors[pos].size() == 0)
        successors[pos].push_back(0);

    // NOTE : the first element of successors[pos] (successors[pos][0]) is 
    //        the number of successors already in the list
    successors[pos][0]++;
    successors[pos].push_back(i);

    return 0;
}


/**
 *  Computes the pre-recursive (prolog) or post-recursive (epilog) block of equations, i.e. the equations depending
 *  only on previously computed equations (endogenous).
 *  
 *  Loops on the equations. 
 *  If all endogenous in an equation are already in a block, then put the equation in the next available 
 *  position in v_order and mark the equation as ordered (v_ordered[i] = true);
 *  
 *  Method
 *  ------
 *       For each equation eq, loop on varj, the variables present in eq (stored in predecessors[i]):
 *			if varj == predecessors[i][j+1] or varj < 0 (exo) or v_ordered[varj] == true: 
 *               skip the equation
 *			else 
 *				v_ordered[varj] = true
 *				v_order[from + nb] = i
 *	
 *       If no new equation had been set in v_order during the loop, end of the process (the block is completed)
 *       Else restart the loop on the equations
 *  
 *  @param [in]         KDB*    dbe             KDB of equations
 *  @param [in]         int**   predecessors    vector of vectors containing the endogenous variables of each equation in dbe
 *  @param [in]         int     from            first available place in v_order
 *  @return             int                     number of equations in the computed block
 *
 *  @global [in, out]   int*    v_order      vector containing the order of execution of the model (after reordering)
 *  @global [in, out]   std::vector<bool> v_ordered    vector with true for the equations already placed in v_order
 *  
 */
int CSimulation::build_pre_post_list(KDBEquationsPtr dbe, std::vector<std::vector<int>>& predecessors, int from)
{
    int nb = 0;
    int nb_predecessors = 0;

    // We restart as long as we added an equation to the PRE or POST list (new_eq_added = true)
    // because we may have a new equation that only depended on those just added and which 
    // therefore is also part of the PRE or POST list
    bool new_eq_added = true;
    while(new_eq_added) 
    {
        int j = 0;
        new_eq_added = false;
        for(int i = 0; i < sim_dbe->size(); i++) 
        {
            // Equation already ordered (i.e. in the PRE or POST list)
            if(v_ordered[i]) 
                continue;
            
            std::vector<int>& eq_predecessors = predecessors[i];
            if(eq_predecessors.size() > 0) 
            {
                nb_predecessors = eq_predecessors[0];
                for(j = 0; j < nb_predecessors; j++) 
                {
                    // VAR[j+1] = endogenous of the current equation
                    if(eq_predecessors[j + 1] < 0) 
                        continue;
                    // Var[j+1] = already ordered
                    if(v_ordered[eq_predecessors[j + 1]]) 
                        continue;
                    // Var[j+1] non-exogenous and not ordered 
                    // -> equation cannot be added to the PRE or POST list 
                    break;
                }
            }

            // No variable in the equation OR all variables exogenous OR already ordered
            if(predecessors[i].size() == 0 || j == nb_predecessors) 
            {
                new_eq_added = true;
                v_ordered[i] = true;
                v_order[from + nb] = i;
                nb++;
            }
        }
    }

    return nb;
}


/**
 *  Builds the interdependent block and places the equation numbers at the end of v_order.
 *  The final execution order v_order is composed as follows:
 *  
 *      nb_pre eqs
 *  	nb_post eqs
 *  	nb_inter eqs
 *    
 *  @param [in] KDB*    dbe             model   
 *  @param [in] int**   predecessors    vector of vectors containing the endogenous variables of each equation in dbe
 *  @return     int                     number of equations in the interdep block
 *
 *  @global [in, out]   int*    v_order      vector containing the order of execution of the model (after reordering)
 *  
 */
int CSimulation::build_inter_list(KDBEquationsPtr dbe, std::vector<std::vector<int>>& predecessors)
{
    int nb = 0;
    for(int i = 0; i < dbe->size(); i++) 
    {
        if(v_ordered[i]) 
            continue;
        v_order[nb_pre + nb_post + nb] = i;
        nb++;
    }
    return nb;
}

/**
 *  Free all temporary vectors allocated for the model reordering.
 *  
 *  @param [in]         KDB*    dbe             KDB of the model (equations)
 *  @param [in, out]    int**   predecessors    see pre_order()
 *  @param [in, out]    int**   successors      see pre_order()
 *  
 */
int CSimulation::post_order(KDBEquationsPtr dbe, std::vector<std::vector<int>>& predecessors, std::vector<std::vector<int>>& successors)
{
    v_ordered.clear();
    return 0;
}


/**
 * Prepares the model reordering by creating 2 vectors (predecessors and successors) containing 
 * the predecessors and successors of each equation.
 *  
 *		predecessors[i][0]    = nb of predecessors (endos) in the equation i
 *		predecessors[i][1...] = positions in dbe of the predecessors found in equation i
 
 *      successors[i][0]   = nb of successors of the equation i = all equations containing endo[i]
 *		successors[i][1...] = positions of successors of endo[i] in dbe
 *
 *  
 *  @param [in]      KDB*    dbe            equations of the model 
 *  @param [in, out] int**   predecessors   vector of vectors of predecessors (1 vector for each eq (=endo))
 *  @param [in, out] int**   successors     vector of vectors of successors (1 vector for each endo)
 *
 *	@global [out]    std::vector<bool> v_ordered   allocated vector indicating which equations have already been placed in a block (pre, post, interdep).
 *                                              -> Contains nb bools where nb is the number of equations in dbe. 
 *                                              -> Note that v_ordered is not calculated in pre_order(), just allocated.
 *                                              -> v_ordered[i] = true if equation i has already been put in a block (PRE...)
 *	@global [out]    int*    v_order     allocated vector containing the order of execution of the equations in the model.
 *                                              -> nb integers where nb is the number of equations in dbe (not calculated here)
 *                                              -> Note that v_order is not calculated here, only allocated 
 *  
 */
int CSimulation::pre_order(KDBEquationsPtr dbe, std::vector<std::vector<int>>& predecessors, std::vector<std::vector<int>>& successors)
{
    int nb = dbe->size();
    
    v_order.clear();
    v_order.resize(nb, -1);

    v_ordered.clear();
    v_ordered.resize(nb, false);
    
    int eq_pos;
    int i = 0, j = 0;
    bool exchange = false;
    std::shared_ptr<CLEC> clec;
    std::string eq_name_resolved;
    for(const auto& [eq_name, eq_ptr] : dbe->k_objs) 
    {
        clec = eq_ptr->clec;
        std::vector<int>& eq_predecessors = predecessors[i];
        eq_predecessors.reserve(clec->map_objs.size() + 1);

        /* LOG NB AND POS OF ENDO VARS */
        // eq_predecessors[0] = (maximum) nb of (possible) predecessors of the ith equation
        eq_predecessors.push_back(0);

        j = 1;
        eq_pos = -1;
        exchange = map_exchange.contains(eq_name);
        for(const auto& [name, _]: clec->map_objs) 
        {
            if(is_coefficient(name)) 
                continue;

            eq_name_resolved = find_eq_name(name);
            
            // 'name' is not an endogenous variable of any equation
            if(eq_name_resolved.empty())
                eq_pos = -1;
            // 'name' is the endogenous variable of the current equation
            else if(eq_name_resolved == eq_name)
                eq_pos = -1;
            else 
                eq_pos = sim_dbe->index_of(eq_name_resolved);

            eq_predecessors[0]++;
            eq_predecessors.push_back(eq_pos);
            if(eq_pos >= 0) 
                add_post(successors, i, eq_pos);
        }

        i++;
    }

    return 0;
}


/**
 *  Reorders a model (i.e.: a list of equations) before the simulation to optimise the execution order of the set of equations.
 *  
 *  The prolog block consists of the first nb_pre equations in v_order.
 *  The interdep block consists of the nb_inter equations in v_order beginning at position nb_pre + 1.
 *  The epilog block consists of the KSIM_EPILOG equations in v_order beginning at position nb_pre + nb_inter + 1.
 *  
 *  If sorting_algo == SORT_NONE:
 *      - no reordering is performed: nb_pre = nb_post = 0 and nb_inter = nb
 *      - v_order = order defined by the positions of eqs in dbe
 *  
 *  If sorting_algo == SORT_CONVEX or SORT_BOTH:
 *      - 3 blocks are created, based on predecessors and successors of each equation.
 *      - each block is defined by its size (nb_pre, nb_post and nb_inter)
 *      - v_order contains the reordered equations: nb_pre, then nb_inter, then nb_post
 *  
 *  If sorting_algo == SORT_BOTH:
 *      - after decomposing in 3 blocks, a reordering is made inside the interdep block to minimize the distance 
 *        between each equation and its explanatory variables (i.e. contained in the eq formula)
 *  
 *  @param [in] KDB*    dbe     KDB containing the equations defining the model
 *  @param [in] char**  eqs     list of equations to simulate
 *  
 *  @global [in]    int  sorting_algo   reordering algorithm (SORT_NONE, SORT_BOTH)
 *  @global [out]   int* v_order  positions in dbe of the equations (to simulate) in the execution order
 *  @global [out]   int  nb_pre    number of equations in the "prolog" block 
 *  @global [out]   int  nb_inter  number of equations in the "interdep" block
 *  @global [out]   int  nb_post   number of equations in the "epilog"
 *  
 */
void CSimulation::order(KDBEquationsPtr dbe, const std::vector<std::string>& eqs)
{
    long cpu_order = 0; 
    cpu_time_sorting = 0;
    cpu_time_scc = 0;

    int nb = dbe->size();
    
    // No reordering : we keep the order of eqs and so everything is interdep
    if(sorting_algo == SORT_NONE) 
    {
        nb_pre = nb_post = 0;
        nb_inter = nb;
        v_order.clear();
        v_order.resize(nb, -1);
        if(eqs.size() == 0)
            for(int i = 0; i < nb_inter; i++) 
                v_order[i] = i;
        else
            for(int i = 0; i < nb_inter; i++) 
                v_order[i] = dbe->index_of(eqs[i]);
        return;
    }

    //kmsg("Sorting equations ....");
    cpu_order = WscrGetMS();
    kmsg("Calculating SCC...");

    // predecessors = liste de pointeur vers des vecteurs contenant la pos de toutes les vars de l'eq i
    // voir preorder
    std::vector<std::vector<int>> predecessors(nb);
    std::vector<std::vector<int>> successors(nb);
    pre_order(dbe, predecessors, successors);
    nb_pre = build_pre_post_list(dbe, predecessors, 0);
    nb_post = build_pre_post_list(dbe, successors, nb_pre);

    /* REVERSE FOR EXECUTION PURPOSE */
    int k;
    for(int i = 0; i < nb_post / 2; i++) 
    {
        k = v_order[nb_pre + i];
        v_order[nb_pre + i] = v_order[nb_pre + (nb_post - 1) - i];
        v_order[nb_pre + (nb_post - 1) - i] = k;
    }

    nb_inter = build_inter_list(dbe, predecessors);

    cpu_time_scc = WscrGetMS() - cpu_order;
    kmsg("Calculating SCC... %ld ms -> #PRE %d - #INTER %d - #POST %d", 
            cpu_time_scc, 
            nb_pre, 
            nb_inter, 
            nb_post);

    // NOTE: currently, v_order contains lists in the order *pre*, *post*, *inter*.
    // Rotate the [post, inter] subrange left by nb_post to obtain *pre*, *inter*, *post*.
    std::rotate(v_order.begin() + nb_pre,
                v_order.begin() + nb_pre + nb_post,
                v_order.begin() + nb_pre + nb_post + nb_inter);

    if(sorting_algo == SORT_BOTH) 
    {
        kmsg("Reordering interdependent block...");
        compute_tri(dbe, predecessors, nb_passes);
        kmsg("Reordering interdependent block... %ld ms", cpu_time_sorting);
    }

    post_order(dbe, predecessors, successors);
}


/**
 *  Tries to find the equation whose endogenous is the variable 'var'. 
 *  Browses therefore map_exchange which contains the (possibly modified) 
 *  endogenous variable 'var' after the first endo-exo exchanges.
 *  
 *  @param  [in]    string   var    variable to search for
 *  @return         string          name of the equation whose endogenous is 'var'
 */
std::string CSimulation::find_eq_name(const std::string& var)
{   
    // exchange for this variable -> the endogenous of the equation 
    // is the exchanged variable
    if(map_exchange_rev.contains(var))
        return map_exchange_rev[var];
    
    // no exchange for this variable -> endogenous of the equation 
    // is the variable itself 
    if(sim_dbe->contains(var))
        return var;
    else
        return "";
}


/**
 *  Initialise the pseudo-triangulation variables.
 *  
 *  @param [in]     KDB*    dbe         model    
 */
int CSimulation::compute_tri_begin(KDBEquationsPtr dbe)
{
    int nb = dbe->size();
    v_permut.clear();
    v_permut.resize(nb, -1);
    
    for(int i = 0 ; i < nb_inter ; i++)
        v_permut[v_order[nb_pre + i]] = i;

    return 0;
}


/**
 *  Saves int v_order the changes computed by the triangulation algorithm.
 *  
 *  @param [in]     KDB*    dbe         model    
 */
int CSimulation::compute_tri_end(KDBEquationsPtr dbe)
{
    for(int i = 0 ; i < dbe->size() ; i++)
        if(v_permut[i] >= 0)
            v_order[nb_pre + v_permut[i]] = i;

    v_permut.clear();
    return 0;
}


/**
 *  For each explanatory variable in equation  i, we look for the equation calculated the latest
 *  in the order of the model (after permutation as defined in the current state of v_permut).
 *
 *    Let m be this position.
 *    If m < v_permut[i], ok, the explanatory var is caclulated before equation i => no change in v_permut
 *    Otherwise, move everything forward from the current position of eqi to m and place eqi in place of m.
 *  
 *  @param [in]         KDB*    dbe     model   
 *  @param [in]         int     i       equation position in the dbe
 *  @param [in]         int*    vars    list of explanatory variables in equation i
 */
void CSimulation::compute_tri_perm1(KDBEquationsPtr dbe, int i, std::vector<int>& vars)
{
    int m = -1;

    // calcul de l'eq jm dont le numéro d'ordre de calcul est le plus grand
    int posj = -1;
    for(int j = 1 ; j <= vars[0] ; j++) 
    {
        // var endogène de l'eq
        if(vars[j] < 0) 
            continue;

        // position actuelle de l'eq j
        posj = v_permut[vars[j]];

        if(posj > m)
            m = posj;
    }

    // Si le numéro d'ordre max des eq dont dépend la courante est < numéro d'ordre de la courante : ok
    int ksim_permi = v_permut[i];
    if(m < ksim_permi) 
        return;

    int nbe = dbe->size();
    for(int j = 0 ; j < nbe ; j++)
        if(v_permut[j] > ksim_permi && v_permut[j] <= m)
            v_permut[j]--;

    v_permut[i] = m;
}


/**
 *  Sort the equations by making successive 'pseudo-triangulation' passes.
 *  Method applied "passes" times.

 *  
 *  @param [in]         KDB*    dbe             model
 *  @param [in]         int**   predecessors    table of vectors, 1 vector per equation with the list explanatory variables  
 *  @param [in]         int     passes          how many times the heuristic algorithm must be run 
 */
void CSimulation::compute_tri(KDBEquationsPtr dbe, std::vector<std::vector<int>>& predecessors, int passes)
{   
    int cpu_sort = WscrGetMS();

    compute_tri_begin(dbe);

    int var;
    for(int j = 0 ; j < passes ; j++) 
    {
        for(int i = 0 ; i < nb_inter ; i++) 
        {
            var = v_order[nb_pre + i];
            compute_tri_perm1(dbe, var, predecessors[var]);
        }
    }

    compute_tri_end(dbe);
    
    cpu_time_sorting = WscrGetMS() - cpu_sort;
}

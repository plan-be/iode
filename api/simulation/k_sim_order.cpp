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
#include <unordered_map>


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
 *				v_ordered_eqs[from + nb] = i
 *	
 *       If no new equation had been set in v_order during the loop, end of the process (the block is completed)
 *       Else restart the loop on the equations
 *  
 *  @param [in]         int**   v_eq_vars       vector of vectors containing the variables of each equation in dbe
 *  @param [in]         int     from            first available place in v_order
 *  @return             int                     number of equations in the computed block  
 */
int CSimulation::build_pre_post_list(std::vector<std::vector<int>>& v_eq_vars, const int from, const bool pre)
{
    auto start_time = std::chrono::steady_clock::now();

    std::string list_name = pre ? "PRE" : "POST";

    // We restart as long as we added an equation to the PRE or POST list (new_eq_added = true)
    // because we may have a new equation that only depended on those just added and which 
    // therefore is also part of the PRE or POST list
    int nb = 0;
    bool new_eq_added = true;
    while(new_eq_added) 
    {
        new_eq_added = false;
        for(int i = 0; i < sim_dbe->size(); i++) 
        {
            // Equation already ordered (i.e. in the PRE or POST list)
            if(v_ordered[i]) 
                continue;
            
            bool found_exo = false;
            std::vector<int>& v_vars = v_eq_vars[i];
            if(v_vars.size() > 0) 
            {
                for(const int var: v_vars) 
                {
                    // 'var' = endogenous of the current equation (see method pre_order())
                    // -> OK so move to next name
                    if(var < 0) 
                        continue;
                    // 'var' already ordered -> OK so move to next name
                    if(v_ordered[var]) 
                        continue;
                    // 'var' not endogenous and not ordered 
                    // -> equation cannot be added to the PRE or POST list 
                    found_exo = true;
                    break;
                }
            }

            // No variable in the equation OR no exogenous variable
            // -> equation can be added to the PRE or POST list
            if(v_vars.size() == 0 || !found_exo) 
            {
                new_eq_added = true;
                v_ordered[i] = true;
                v_ordered_eqs[from + nb] = i;
                nb++;
            }
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    double cpu_time = std::chrono::duration<double>(end_time - start_time).count();
    std::string msg = "\tBuilding " + list_name + " list done in " + std::to_string(cpu_time) + " sec";
    kmsg((char*) msg.c_str());

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
 *  @param [in] int**   predecessors    vector of vectors containing the endogenous variables of each equation in dbe
 *  @return     int                     number of equations in the interdep block
 *
 *  @global [in, out]   int*    v_order      vector containing the order of execution of the model (after reordering)
 *  
 */
int CSimulation::build_inter_list(std::vector<std::vector<int>>& predecessors)
{
    auto start_time = std::chrono::steady_clock::now();

    int nb = 0;
    for(int i = 0; i < sim_dbe->size(); i++) 
    {
        if(v_ordered[i]) 
            continue;
        v_ordered_eqs[nb_pre + nb_post + nb] = i;
        nb++;
    }

    auto end_time = std::chrono::steady_clock::now();
    double cpu_time = std::chrono::duration<double>(end_time - start_time).count();
    std::string msg = "\tBuilding INTER list done in " + std::to_string(cpu_time) + " sec";
    kmsg((char*) msg.c_str());

    return nb;
}


/**
 * Prepares the model reordering by creating 2 vectors (predecessors and successors) containing 
 * the predecessors and successors of each equation.
 *  
 *		predecessors[i][...] = positions in dbe of the predecessors found in equation i
 *		successors[i][...]   = positions in dbe of the successors found in equation i
 * 
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
int CSimulation::pre_order(std::vector<std::vector<int>>& predecessors, std::vector<std::vector<int>>& successors)
{
    int nb_eqs = sim_dbe->size();
    
    v_ordered_eqs.clear();
    v_ordered_eqs.resize(nb_eqs, -1);

    v_ordered.clear();
    v_ordered.resize(nb_eqs, false);

    auto start_time = std::chrono::steady_clock::now();
    
    int eq_pos;
    int i = 0, j = 0;
    std::string eq_name;
    std::string endo_name;
    bool exchange = false;
    std::shared_ptr<CLEC> clec;
    std::unordered_map<std::string, int> cache_eq_pos;
    for(std::shared_ptr<Equation>& eq_ptr : sim_dbe->v_obj_ptr) 
    {
        clec = eq_ptr->clec;
        std::vector<int>& eq_predecessors = predecessors[i];
        eq_predecessors.reserve(clec->v_var_names.size());

        eq_name = sim_dbe->get_name_from_vector(i);

        for(const auto& name : clec->v_var_names)
        {
            // try to find the position of the equation whose endogenous 
            // variable is 'name'
            if(cache_eq_pos.contains(name))
                eq_pos = cache_eq_pos[name];
            else
            {
                eq_pos = find_eq_pos(name);
                cache_eq_pos[name] = eq_pos;
            }
            
            // 'name' is not an endogenous variable of any equation 
            // -> check next name is CLEC
            if(eq_pos < 0)
                continue;
            
            // 'name' is the endogenous variable of the current equation
            // -> put tag '-1' as predecessor and move to next name is CLEC
            endo_name = sim_dbe->get_name_from_vector(eq_pos);
            if(eq_name == endo_name)
            {
                eq_predecessors.push_back(-1);
                continue;
            }

            // push the position of the equation corresponding to 'name' 
            eq_predecessors.push_back(eq_pos);

            // push the position of the equation corresponding to 'eq_name'
            // (current equation) in the vector of successors
            successors[eq_pos].push_back(i);
        }

        i++;
    }

    auto end_time = std::chrono::steady_clock::now();
    double cpu_time = std::chrono::duration<double>(end_time - start_time).count();
    std::string msg = "\tBuild predecessors and successors vectors done in " + std::to_string(cpu_time) + " sec";
    kmsg((char*) msg.c_str());

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
 *  @param [in] char**  eqs     list of equations to simulate
 *  
 *  @global [in]    int  sorting_algo   reordering algorithm (SORT_NONE, SORT_BOTH)
 *  @global [out]   int* v_order  positions in dbe of the equations (to simulate) in the execution order
 *  @global [out]   int  nb_pre    number of equations in the "prolog" block 
 *  @global [out]   int  nb_inter  number of equations in the "interdep" block
 *  @global [out]   int  nb_post   number of equations in the "epilog"
 *  
 */
void CSimulation::order(const std::vector<std::string>& eqs)
{
    std::string msg;
    double cpu_time = 0.0;
    int nb_eqs = (eqs.empty()) ? sim_dbe->size() : (int) eqs.size();

    // No reordering : we keep the order of eqs and so everything is interdep
    if(sorting_algo == SORT_NONE) 
    {
        auto start_time = std::chrono::steady_clock::now();

        nb_pre = nb_post = 0;
        nb_inter = nb_eqs;

        v_ordered_eqs.clear();
        if(eqs.size() == 0)
        {
            v_ordered_eqs.reserve(nb_eqs);
            for(int i = 0; i < nb_eqs; i++) 
                v_ordered_eqs.push_back(i);
        }
        else
        {
            int eq_pos;
            v_ordered_eqs.reserve(nb_eqs);
            for(const std::string& eq_name : eqs)
            {
                eq_pos = sim_dbe->get_vector_position(eq_name);
                if(eq_pos < 0)
                {
                    msg = "Ordering: equation '" + eq_name + "' not found in the ";
                    msg += "simulation Equations workspace";
                    kwarning((char*) msg.c_str());
                    clear();
                    return;
                }
                v_ordered_eqs.push_back(eq_pos);
            }
        }

        auto end_time = std::chrono::steady_clock::now();
        cpu_time = std::chrono::duration<double>(end_time - start_time).count();
        std::string msg = "Ordering done in " + std::to_string(cpu_time) + " sec";
        kmsg((char*) msg.c_str());

        return;
    }
    
    auto start_time = std::chrono::steady_clock::now();
    kmsg("--- Calculating SCC ---");

    // predecessors = liste de pointeur vers des vecteurs contenant la pos de toutes les vars de l'eq i
    // voir preorder
    std::vector<std::vector<int>> predecessors(nb_eqs);
    std::vector<std::vector<int>> successors(nb_eqs);
    pre_order(predecessors, successors);
    nb_pre = build_pre_post_list(predecessors, 0, true);
    nb_post = build_pre_post_list(successors, nb_pre, false);

    // REVERSE FOR EXECUTION PURPOSE
    int k;
    for(int i = 0; i < nb_post / 2; i++) 
    {
        k = v_ordered_eqs[nb_pre + i];
        v_ordered_eqs[nb_pre + i] = v_ordered_eqs[nb_pre + (nb_post - 1) - i];
        v_ordered_eqs[nb_pre + (nb_post - 1) - i] = k;
    }

    nb_inter = build_inter_list(predecessors);

    auto end_time = std::chrono::steady_clock::now();
    cpu_time = std::chrono::duration<double>(end_time - start_time).count();
    msg = "Calculating SCC done in " + std::to_string(cpu_time) + " sec -> ";
    msg += "#PRE " + std::to_string(nb_pre) + " - #INTER " + std::to_string(nb_inter); 
    msg += " - #POST " + std::to_string(nb_post);
    kmsg((char*) msg.c_str());
    cpu_time_scc = (int) cpu_time * 1000;

    // NOTE: currently, v_ordered_eqs contains lists in the order *pre*, *post*, *inter*.
    // Rotate the [post, inter] subrange left by nb_post to obtain *pre*, *inter*, *post*.
    std::rotate(v_ordered_eqs.begin() + nb_pre,
                v_ordered_eqs.begin() + nb_pre + nb_post,
                v_ordered_eqs.begin() + nb_pre + nb_post + nb_inter);

    if(sorting_algo == SORT_BOTH) 
        compute_tri(predecessors, nb_passes);

    end_time = std::chrono::steady_clock::now();
    cpu_time = std::chrono::duration<double>(end_time - start_time).count();
    msg = "Ordering done in " + std::to_string(cpu_time) + " sec";
    kmsg((char*) msg.c_str());

    v_ordered.clear();
}


/**
 *  Tries to find the equation whose endogenous is the variable 'var'. 
 *  Browses therefore map_exchange which contains the (possibly modified) 
 *  endogenous variable 'var' after the first endo-exo exchanges.
 *  
 *  @param  [in]    string   var    variable to search for
 *  @return         int             position of the equation whose endogenous is 'var'
 */
int CSimulation::find_eq_pos(const std::string& var)
{   
    // check if an exchange 'endo <-> exo' exist for the variable 'var'.
    // Otherwise, assume 'var' is the endogenous variable of the equation to be found
    std::string endo = map_exchange_rev.contains(var) ? map_exchange_rev[var] : var;
    
    // no exchange for this variable -> endogenous of the equation 
    // is the variable itself 
    return sim_dbe->get_vector_position(endo);
}


/**
 *  Initialise the pseudo-triangulation variables.
 */
int CSimulation::compute_tri_begin()
{
    int nb_eqs = sim_dbe->size();

    v_permut.clear();
    v_permut.resize(nb_eqs, -1);
    
    for(int i = 0 ; i < nb_inter ; i++)
        v_permut[v_ordered_eqs[nb_pre + i]] = i;

    return 0;
}


/**
 *  Saves int v_order the changes computed by the triangulation algorithm.
 */
int CSimulation::compute_tri_end()
{
    for(int i = 0 ; i < sim_dbe->size() ; i++)
        if(v_permut[i] >= 0)
            v_ordered_eqs[nb_pre + v_permut[i]] = i;

    v_permut.clear();
    return 0;
}


/**
 *  For each explanatory variable in equation i, we look for the equation calculated the latest
 *  in the order of the model (after permutation as defined in the current state of v_permut).
 *
 *    Let last_pos be this position.
 *    If last_pos < v_permut[i], ok, the explanatory var is calculated before equation i => no change in v_permut
 *    Otherwise, move everything forward from the current position of eqi to last_pos and place eqi in place of last_pos.
 *  
 *  @param [in]         int     i       equation position in the dbe
 *  @param [in]         int*    v_vars  list of explanatory variables in equation i
 */
void CSimulation::compute_tri_perm1(int i, std::vector<int>& v_vars)
{
    int last_pos = -1;
    int current_pos = v_permut[i];

    // look for the equation calculated the latest in the order of the model
    int posj = -1;
    for(const int var: v_vars) 
    {
        // var = endogenous of equation i
        if(var < 0) 
            continue;

        // permuted position of var
        posj = v_permut[var];

        if(posj > last_pos)
            last_pos = posj;
    }

    // all equations associated with the explanatory variables of 
    // the current equation are calculated BEFORE the current equation
    if(last_pos < current_pos) 
        return;

    // move all equation positions calculated between the current equation + 1 
    // and the last equation position associated with the explanatory variables
    for(int j = 0; j < sim_dbe->size(); j++)
        if(v_permut[j] > current_pos && v_permut[j] <= last_pos)
            v_permut[j]--;

    // current equation is permuted with the equation associated with the 
    // explanatory variable which is the latest computed
    v_permut[i] = last_pos;
}


/**
 *  Sort the equations by making successive 'pseudo-triangulation' passes.
 *  Method applied "passes" times.
 *  
 *  @param [in]         int**   predecessors    table of vectors, 1 vector per equation with the list explanatory variables  
 *  @param [in]         int     passes          how many times the heuristic algorithm must be run 
 */
void CSimulation::compute_tri(std::vector<std::vector<int>>& predecessors, int passes)
{
    std::string msg;
    double cpu_time = 0.0;

    msg = "--- Reordering interdependent block (" + std::to_string(nb_inter) + " equations) ---";
    kmsg((char*) msg.c_str());

    auto start_time = std::chrono::steady_clock::now();
    compute_tri_begin();

    int eq_pos;
    for(int j = 0 ; j < passes ; j++) 
    {   
        auto start_tri = std::chrono::steady_clock::now();
        for(int i = 0 ; i < nb_inter ; i++) 
        {
            eq_pos = v_ordered_eqs[nb_pre + i];
            compute_tri_perm1(eq_pos, predecessors[eq_pos]);
        }
        auto end_tri = std::chrono::steady_clock::now();
        cpu_time = std::chrono::duration<double>(end_tri - start_tri).count();
        msg = "\tPass " + std::to_string(j) + " : " + std::to_string(cpu_time) + " sec";
        kmsg((char*) msg.c_str());
    }

    compute_tri_end();

    auto end_time = std::chrono::steady_clock::now();
    cpu_time = std::chrono::duration<double>(end_time - start_time).count();
    msg = "Reordering interdependent block done in " + std::to_string(cpu_time) + " sec";
    kmsg((char*) msg.c_str());
    cpu_time_sorting = (int) cpu_time * 1000;
}

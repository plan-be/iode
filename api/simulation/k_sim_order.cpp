/** 
 * @header4iode
 *
 *  Functions to reorder a model to optimize the simulation algorithm. 
 *  
 *      1. Decomposes the model in 3 blocks:   
 *          - prolog (KSIM_PRE), 
 *          - interdep (KSIM_INTER)
 *          - epilog (KSIM_POST)
 *      2. "pseudo-triangulates" the interdep block, i.e. inverts the equation order 
 *         to optimize the incidence matrix (i.e.: minimizing the nb of 1 above the diagonal).
 *  
 * List of functions 
 * -----------------
 *      int order(std::shared_ptr<KDBEquations> dbe, char** eqs)              Reorders a model before the simulation to optimise the execution order of the set of equations.
 *      int get_eq_position(int posendo)                       Searches the equation whose endogenous is the variable posendo. 
 *      void compute_tri(std::shared_ptr<KDBEquations> dbe, std::vector<std::vector<int>>& predecessors, int passes)    Sort the equations by making successive 'pseudo-triangulation' passes.
 */
#include "api/constants.h"
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
 *  position in KSIM_ORDER and mark the equation as ordered (KSIM_ORDERED[i] = 1;
 *  
 *  Method
 *  ------
 *       For each equation eq, loop on varj, the variables present in eq (stored in predecessors[i]):
 *			if varj == predecessors[i][j+1] or varj < 0 (exo) or KSIM_ORDERED[varj] == 1: 
 *               skip the equation
 *			else 
 *				KSIM_ORDERED[varj] = 1
 *				KSIM_ORDER[from + nb] = i
 *	
 *       If no new equation had been set in KSIM_ORDER during the loop, end of the process (the block is completed)
 *       Else restart the loop on the equations
 *  
 *  @param [in]         KDB*    dbe             KDB of equations
 *  @param [in]         int**   predecessors    vector of vectors containing the endogenous variables of each equation in dbe
 *  @param [in]         int     from            first available place in KSIM_ORDER
 *  @return             int                     number of equations in the computed block
 *
 *  @global [in, out]   int*    KSIM_ORDER      vector containing the order of execution of the model (after reordering)
 *  @global [in, out]   int*    KSIM_ORDERED    vector with 1 for the equations already placed in KSIM_ORDER
 *  
 */
int CSimulation::build_pre_post_list(std::shared_ptr<KDBEquations> dbe, std::vector<std::vector<int>>& predecessors, int from)
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
        for(int i = 0; i < KSIM_DBE->size(); i++) 
        {
            // Equation already ordered (i.e. in the PRE or POST list)
            if(KSIM_ORDERED[i] == 1) 
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
                    if(KSIM_ORDERED[eq_predecessors[j + 1]] == 1) 
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
                KSIM_ORDERED[i] = 1;
                KSIM_ORDER[from + nb] = i;
                nb++;
            }
        }
    }

    return nb;
}


/**
 *  Builds the interdependent block and places the equation numbers at the end of KSIM_ORDER.
 *  The final execution order KSIM_ORDER is composed as follows:
 *  
 *      KSIM_PRE eqs
 *  	KSIM_POST eqs
 *  	KSIM_INTER eqs
 *    
 *  @param [in] KDB*    dbe             model   
 *  @param [in] int**   predecessors    vector of vectors containing the endogenous variables of each equation in dbe
 *  @return     int                     number of equations in the interdep block
 *
 *  @global [in, out]   int*    KSIM_ORDER      vector containing the order of execution of the model (after reordering)
 *  
 */
int CSimulation::build_inter_list(std::shared_ptr<KDBEquations> dbe, std::vector<std::vector<int>>& predecessors)
{
    int nb = 0;
    for(int i = 0; i < dbe->size(); i++) 
    {
        if(KSIM_ORDERED[i]) 
            continue;
        KSIM_ORDER[KSIM_PRE + KSIM_POST + nb] = i;
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
int CSimulation::post_order(std::shared_ptr<KDBEquations> dbe, std::vector<std::vector<int>>& predecessors, std::vector<std::vector<int>>& successors)
{
    SW_nfree(KSIM_ORDERED);
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
 *	@global [out]    char*   KSIM_ORDERED   allocated vector indicating which equations have already been placed in a block (pre, post, interdep).
 *                                              -> Contains nb chars where nb is the number of equations in dbe. 
 *                                              -> Note that KSIM_ORDERED is not calculated in pre_order(), just allocated.
 *                                              -> KSIM_ORDERED[i] = 1 if equation i has already been put in a block (PRE...)
 *	@global [out]    int*    KSIM_ORDER     allocated vector containing the order of execution of the equations in the model.
 *                                              -> nb integers where nb is the number of equations in dbe (not calculated here)
 *                                              -> Note that KSIM_ORDER is not calculated here, only allocated 
 *  
 */
int CSimulation::pre_order(std::shared_ptr<KDBEquations> dbe, std::vector<std::vector<int>>& predecessors, std::vector<std::vector<int>>& successors)
{
    int     pos, posj, nb;
    CLEC    *clec;

    nb = dbe->size();
    KSIM_ORDER    = (int *)  SW_nalloc(sizeof(int) * nb);
    KSIM_ORDERED  = (char *) SW_nalloc(sizeof(char) * nb);

    int i = 0;
    for(const auto& [name, eq] : dbe->k_objs) 
    {
        clec = eq->clec;
        std::vector<int>& eq_predecessors = predecessors[i];
        eq_predecessors.resize(clec->nb_names + 1, 0);

        /* LOG NB AND POS OF ENDO VARS */
        // eq_predecessors[0] = (maximum) nb of (possible) predecessors of the ith equation
        eq_predecessors[0] = clec->nb_names;
        for(int j = 0; j < clec->nb_names; j++) 
        {
            if(is_coefficient(clec->lnames[j].name)) 
                continue;
            
            // Recherche l'eq dont la variable j est endo
            posj = (clec->lnames[j]).pos;
            
            if(KSIM_POSXK[i] == posj)  // améliore les performances JMP 11/3/2012 -- CHECK!
                pos = -1; // Endo de l'eq courante
            else 
            {
                pos = get_eq_position(posj);
                // si var pos est l'endogène => -1
                if(pos >= 0 && pos == i) 
                    pos = -1;
            }
            eq_predecessors[j + 1] = pos;
            if(pos >= 0) 
                add_post(successors, i, pos);
        }
        i++;
    }

    return 0;
}


/**
 *  Reorders a model (i.e.: a list of equations) before the simulation to optimise the execution order of the set of equations.
 *  
 *  The prolog block consists of the first KSIM_PRE equations in KSIM_ORDER.
 *  The interdep block consists of the KSIM_INTER equations in KSIM_ORDER beginning at position KSIM_PRE + 1.
 *  The epilog block consists of the KSIM_EPILOG equations in KSIM_ORDER beginning at position KSIM_PRE + KSIM_INTER + 1.
 *  
 *  If KSIM_SORT == SORT_NONE:
 *      - no reordering is performed: KSIM_PRE = KSIM_POST = 0 and KSIM_INTER = nb
 *      - KSIM_ORDER = order defined by the positions of eqs in dbe
 *  
 *  If KSIM_SORT == SORT_CONVEX or SORT_BOTH:
 *      - 3 blocks are created, based on predecessors and successors of each equation.
 *      - each block is defined by its size (KSIM_PRE, KSIM_POST and KSIM_INTER)
 *      - KSIM_ORDER contains the reordered equations: KSIM_PRE, then KSIM_INTER, then KSIM_POST
 *  
 *  If KSIM_SORT == SORT_BOTH:
 *      - after decomposing in 3 blocks, a reordering is made inside the interdep block to minimize the distance 
 *        between each equation and its explanatory variables (i.e. contained in the eq formula)
 *  
 *  @param [in] KDB*    dbe     KDB containing the equations defining the model
 *  @param [in] char**  eqs     list of equations to simulate
 *  
 *  @global [in]    int  KSIM_SORT   reordering algorithm (SORT_NONE, SORT_BOTH)
 *  @global [out]   int* KSIM_ORDER  positions in dbe of the equations (to simulate) in the execution order
 *  @global [out]   int  KSIM_PRE    number of equations in the "prolog" block 
 *  @global [out]   int  KSIM_INTER  number of equations in the "interdep" block
 *  @global [out]   int  KSIM_POST   number of equations in the "epilog"
 *  
 */
void CSimulation::order(std::shared_ptr<KDBEquations> dbe, const std::vector<std::string>& eqs)
{
    int  *tmp2, i, k;
    long cpu_order = 0; 

    KSIM_CPU_SORT = 0;
    KSIM_CPU_SCC = 0;
    
    int nb = dbe->size();
    
    // Pas de réord : on garde l'ordre de eqs et donc tout est interdep
    if(KSIM_SORT == SORT_NONE) 
    {
        KSIM_PRE = KSIM_POST = 0;
        KSIM_INTER = nb;
        KSIM_ORDER = (int *) SW_nalloc(sizeof(int) * nb);
        if(eqs.size() == 0)
            for(i = 0; i < KSIM_INTER; i++) 
                KSIM_ORDER[i] = i;
        else
            for(i = 0; i < KSIM_INTER; i++) 
                KSIM_ORDER[i] = dbe->index_of(eqs[i]);
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
    KSIM_PRE = build_pre_post_list(dbe, predecessors, 0);
    KSIM_POST = build_pre_post_list(dbe, successors, KSIM_PRE);
    
    /* REVERSE FOR EXECUTION PURPOSE */
    for(i = 0; i < KSIM_POST / 2; i++) 
    {
        k = KSIM_ORDER[KSIM_PRE + i];
        KSIM_ORDER[KSIM_PRE + i] = KSIM_ORDER[KSIM_PRE + (KSIM_POST - 1) - i];
        KSIM_ORDER[KSIM_PRE + (KSIM_POST - 1) - i] = k;
    }

    KSIM_INTER = build_inter_list(dbe, predecessors);
    KSIM_CPU_SCC = WscrGetMS() - cpu_order;
    kmsg("Calculating SCC... %ld ms -> #PRE %d - #INTER %d - #POST %d", 
            KSIM_CPU_SCC, 
            KSIM_PRE, 
            KSIM_INTER, 
            KSIM_POST);

    /* A AMELIORER !! */
    tmp2 = (int *) SW_nalloc(sizeof(int) * (KSIM_INTER + KSIM_POST));
    memcpy(tmp2, KSIM_ORDER + KSIM_PRE, (KSIM_INTER + KSIM_POST) * sizeof(int));
    memcpy(KSIM_ORDER + KSIM_PRE, tmp2 + KSIM_POST, KSIM_INTER * sizeof(int));
    memcpy(KSIM_ORDER + KSIM_PRE + KSIM_INTER, tmp2, KSIM_POST * sizeof(int));
    SW_nfree(tmp2);

    if(KSIM_SORT == SORT_BOTH) 
    {
        kmsg("Reordering interdependent block...");
        compute_tri(dbe, predecessors, KSIM_PASSES);
        kmsg("Reordering interdependent block... %ld ms", KSIM_CPU_SORT);
    }
    
    post_order(dbe, predecessors, successors);
}


/**
 *  Tries to find the equation whose endogenous is the variable posendo. 
 *  Browses therefore the vector KSIM_POSXK which contains the (possibly modified) endogenous var positions 
 *  after the first endo-exo exchanges.
 *  
 *  @param  [in]    int     posendo   initial position of the endogenous variable
 *  @return         int               new position of that endo after the first endo-exo exchanges
 *                                    -1 if posendo is not found in KSIM_POSXK
 *  
 *  @global [in]    int     KSIM_MAXDEPTH   nb of eqs dans the model (KSIM_DBE)
 *  @global [in]    int*    KSIM_POSXK      KSIM_POSXK[i] = position in KSIM_DBV of the endo variable of equation "KSIM_DBE[i]"
 *  
 */
int CSimulation::get_eq_position(int posendo)
{
    if(posendo < 0) 
        return -1;
    
    if(posendo < KSIM_MAXDEPTH && KSIM_POSXK[posendo] == posendo) 
        return posendo;

    // Recherche l'eq avec pour endogène posendo ie i tq posendo = KSIM_POSXK[i]
    return KSIM_POSXK_REV[posendo]; 
}


/**
 *  Initialise the pseudo-triangulation variables.
 *  
 *  @param [in]     KDB*    dbe         model   
 *  @global [out]   int*    KSIM_PERM   vector of equations permutations. Filled with -1 at start.
 *  
 */
int CSimulation::compute_tri_begin(std::shared_ptr<KDBEquations> dbe)
{
    int nb = dbe->size();
    KSIM_PERM = (int *) SW_nalloc(sizeof(int) * nb);
    
    for(int i = 0 ; i < nb ; i++) 
        KSIM_PERM[i] = -1;
    
    for(int i = 0 ; i < KSIM_INTER ; i++)
        KSIM_PERM[KSIM_ORDER[KSIM_PRE + i]] = i;

    return 0;
}


/**
 *  Saves int KSIM_ORDER the changes computed by the triangulation algorithm.
 *  
 *  @param [in]     KDB*    dbe         model   
 *  @global [out]   int*    KSIM_PERM   vector of equations permutations.
 *  
 */
int CSimulation::compute_tri_end(std::shared_ptr<KDBEquations> dbe)
{
    for(int i = 0 ; i < dbe->size() ; i++)
        if(KSIM_PERM[i] >= 0)
            KSIM_ORDER[KSIM_PRE + KSIM_PERM[i]] = i;

    SW_nfree(KSIM_PERM);
    return 0;
}


/**
 *  For each explanatory variable in equation  i, we look for the equation calculated the latest
 *  in the order of the model (after permutation as defined in the current state of KSIM_PERM).
 *
 *    Let m be this position.
 *    If m < KSIM_PERM[i], ok, the explanatory var is caclulated before equation i => no change in KSIM_PERM
 *    Otherwise, move everything forward from the current position of eqi to m and place eqi in place of m.
 *  
 *  @param [in]         KDB*    dbe     model   
 *  @param [in]         int     i       equation position in the dbe
 *  @param [in]         int*    vars    list of explanatory variables in equation i
 *
 *  @global [in, out]   int*    KSIM_PERM   vector of equations permutations. 
 *  
 */
void CSimulation::compute_tri_perm1(std::shared_ptr<KDBEquations> dbe, int i, std::vector<int>& vars)
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
        posj = KSIM_PERM[vars[j]];

        if(posj > m)
            m = posj;
    }

    // Si le numéro d'ordre max des eq dont dépend la courante est < numéro d'ordre de la courante : ok
    int ksim_permi = KSIM_PERM[i];
    if(m < ksim_permi) 
        return;

    int nbe = dbe->size();
    for(int j = 0 ; j < nbe ; j++)
        if(KSIM_PERM[j] > ksim_permi && KSIM_PERM[j] <= m)
            KSIM_PERM[j]--;

    KSIM_PERM[i] = m;
}


/**
 *  Sort the equations by making successive 'pseudo-triangulation' passes.
 *  Method applied "passes" times.

 *  
 *  @param [in]         KDB*    dbe             model
 *  @param [in]         int**   predecessors    table of vectors, 1 vector per equation with the list explanatory variables  
 *  @param [in]         int     passes          how many times the heuristic algorithm must be run 
 *      
 *  @global [in, out]   int*    KSIM_PERM vector of equation positions after pseudo-triangulation
 *  
 */
void CSimulation::compute_tri(std::shared_ptr<KDBEquations> dbe, std::vector<std::vector<int>>& predecessors, int passes)
{   
    int cpu_sort = WscrGetMS();

    compute_tri_begin(dbe);

    int var;
    for(int j = 0 ; j < passes ; j++) 
    {
        for(int i = 0 ; i < KSIM_INTER ; i++) 
        {
            var = KSIM_ORDER[KSIM_PRE + i];
            compute_tri_perm1(dbe, var, predecessors[var]);
        }
    }

    compute_tri_end(dbe);
    
    KSIM_CPU_SORT = WscrGetMS() - cpu_sort;
}

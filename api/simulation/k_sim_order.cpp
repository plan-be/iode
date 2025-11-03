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
 *      int KE_order(KDB* dbe, char** eqs)              Reorders a model before the simulation to optimise the execution order of the set of equations.
 *      int KE_poseq(int posendo)                       Searches the equation whose endogenous is the variable posendo. 
 *      void KE_tri(KDB* dbe, int** predecessors, int passes)    Sort the equations by making successive 'pseudo-triangulation' passes.
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
 *  Then KE_add_post(successors, eqj, eqi) adds eqi to successors[eqj]
 *
 * At the end of the process, successors contains for each equation all dependent equations.
 *  
 *  @param [in, out]    int**   successors  vector of int*, one per equation containing the list of successors of eq nb i 
 *  @param [in]         int     i           successor position to add to the list successors[pos]
 *  @param [in]         int     pos         position of the predecessor of i
 *  
 */
int CSimulation::KE_add_post(int** successors, int i, int pos)
{
    int     nb = 1;

    if(successors[pos] == 0)
        successors[pos] = (int *) SW_nalloc(sizeof(int) * 2);
    else {
        nb = successors[pos][0] + 1;
        successors[pos] = (int *) SW_nrealloc(successors[pos], nb * sizeof(int),(nb + 1) * sizeof(int));
    }
    successors[pos][0]  = nb;
    successors[pos][nb] = i;

    return(0);
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
int CSimulation::KE_pre(KDB* dbe, int** predecessors, int from)
{
    int     i, j,
            c = 1, nb = 0;

    // On recommence tant qu'on a ajouté une éq dans le groupe PRE (c == 1)
    // car on a peut être une nouvelle éq qui ne dépendait que de celles qui viennent d'être
    // ajoutée et qui donc fait aussi partie du groupe
    while(c == 1) {
        c = 0;
        for(i = 0; i < dbe->size(); i ++) {
            if(KSIM_ORDERED[i] == 1) continue; // Equation déjà classée (i.e. dans le bloc PRE)
            if(predecessors[i]) {
                for(j = 0; j < predecessors[i][0]; j++) {  	// predecessors[i][0] = nb vars de l'éq i
                    if(predecessors[i][j + 1] < 0) continue; /* VAR j+1 = ENDO de l'eq courante */
                    if(KSIM_ORDERED[predecessors[i][j + 1]] == 1) continue;  /* Var j+1 = déjà classée (ORDERED) */
                    break; // Var j + 1 non EXO et non classée, donc eq ne peut être retenue dans le bloc
                }
            }
            if(predecessors[i] == 0 || j == predecessors[i][0]) { // Pas de var dans l'eq OU toutes exo OU classées
                c = KSIM_ORDERED[i] = 1;
                KSIM_ORDER[from + nb] = i;
                nb ++;
            }
        }
    }
    return(nb);
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
int CSimulation::KE_interdep(KDB* dbe, int** predecessors)
{
    int     i, nb = 0;

    for(i = 0; i < dbe->size(); i++) {
        if(KSIM_ORDERED[i]) continue;
        KSIM_ORDER[KSIM_PRE + KSIM_POST + nb] = i;
        nb ++;
    }
    return(nb);
}

/**
 *  Free all temporary vectors allocated for the model reordering.
 *  
 *  @param [in]         KDB*    dbe             KDB of the model (equations)
 *  @param [in, out]    int**   predecessors    see KE_preorder()
 *  @param [in, out]    int**   successors      see KE_preorder()
 *  
 */
int CSimulation::KE_postorder(KDB* dbe, int** predecessors, int** successors)
{
    int     i;

    for(i = 0; i < dbe->size(); i ++)  {
        SW_nfree(predecessors[i]);
        SW_nfree(successors[i]);
    }
    SW_nfree(predecessors);
    SW_nfree(successors);
    SW_nfree(KSIM_ORDERED);

    return(0);
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
 *                                              -> Note that KSIM_ORDERED is not calculated in KE_preorder(), just allocated.
 *                                              -> KSIM_ORDERED[i] = 1 if equation i has already been put in a block (PRE...)
 *	@global [out]    int*    KSIM_ORDER     allocated vector containing the order of execution of the equations in the model.
 *                                              -> nb integers where nb is the number of equations in dbe (not calculated here)
 *                                              -> Note that KSIM_ORDER is not calculated here, only allocated 
 *  
 */
int CSimulation::KE_preorder(KDB* dbe, int** predecessors, int** successors)
{
    int     i, j, pos, posj, nb;
    CLEC    *clec;

    nb = dbe->size();
    KSIM_ORDER    = (int *)  SW_nalloc(sizeof(int) * nb);
    KSIM_ORDERED  = (char *) SW_nalloc(sizeof(char) * nb);

    for(i = 0; i < nb; i ++) {
        clec = KECLEC(dbe, i);
        predecessors[i] = (int *) SW_nalloc(sizeof(int) * (clec->nb_names + 1)); // alloue (nb names + 1) long

        /* LOG ALL NB AND POS OF ENDO VARS */
        predecessors[i][0] = clec->nb_names; // Nbre max de noms (on pourrait améliorer en ne prenant que les vars)
        for(j = 0; j < clec->nb_names; j++) {
            if(is_coefficient(clec->lnames[j].name)) continue;
            // Recherche l'eq dont la variable j est endo
            posj = (clec->lnames[j]).pos;
            
            if(KSIM_POSXK[i] == posj)  // améliore les performances JMP 11/3/2012 -- CHECK!
                pos = -1; // Endo de l'eq courante
            else {
                pos = KE_poseq(posj);
                if(pos >= 0 && pos == i) pos = -1; // si var pos est l'endogène => -1
            }
            predecessors[i][j + 1] = pos;
            if(pos >= 0) KE_add_post(successors, i, pos);
        }
    }

    return(0);
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
void CSimulation::KE_order(KDB* dbe, char** eqs)
{
    int     **predecessors, **successors, *tmp2, i, k, nb;
    long    cpu_order = 0; 

    KSIM_CPU_SORT = 0;
    KSIM_CPU_SCC = 0;
    
    nb = dbe->size();
    
    // Pas de réord : on garde l'ordre de eqs et donc tout est interdep
    if(KSIM_SORT == SORT_NONE) {
        KSIM_PRE = KSIM_POST = 0;
        KSIM_INTER = nb;
        KSIM_ORDER = (int *)  SW_nalloc(sizeof(int) * nb);
        if(eqs == 0)
            for(i = 0; i < KSIM_INTER; i++) KSIM_ORDER[i] = i;
        else
            for(i = 0; i < KSIM_INTER; i++) KSIM_ORDER[i] = dbe->index_of(eqs[i]); /* JMP 02-02-2004 */

        return;
    }

    //kmsg("Sorting equations ....");
    cpu_order = WscrGetMS();
    kmsg("Calculating SCC...");

    // predecessors = liste de pointeur vers des vecteurs contenant la pos de toutes les vars de l'eq i
    // voir preorder
    predecessors = (int **) SW_nalloc(sizeof(int *) * nb);
    successors = (int **) SW_nalloc(sizeof(int *) * nb);
    KE_preorder(dbe, predecessors, successors);
    KSIM_PRE = KE_pre(dbe, predecessors, 0);
    KSIM_POST = KE_pre(dbe, successors, KSIM_PRE);
    
    /* REVERSE FOR EXECUTION PURPOSE */
    for(i = 0; i < KSIM_POST / 2; i++) {
        k = KSIM_ORDER[KSIM_PRE + i];
        KSIM_ORDER[KSIM_PRE + i] = KSIM_ORDER[KSIM_PRE + (KSIM_POST - 1) - i];
        KSIM_ORDER[KSIM_PRE + (KSIM_POST - 1) - i] = k;
    }

    KSIM_INTER = KE_interdep(dbe, predecessors);
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

    if(KSIM_SORT == SORT_BOTH) {
        kmsg("Reordering interdependent block...");
        KE_tri(dbe, predecessors, KSIM_PASSES);
        kmsg("Reordering interdependent block... %ld ms", KSIM_CPU_SORT);
    }
    
    KE_postorder(dbe, predecessors, successors);
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
int CSimulation::KE_poseq(int posendo)
{
    if(posendo < 0) return(-1);
    if(posendo < KSIM_MAXDEPTH && KSIM_POSXK[posendo] == posendo) return(posendo);

    // Recherche l'eq avec pour endogène posendo ie i tq posendo = KSIM_POSXK[i]
    return(KSIM_POSXK_REV[posendo]); 

}


/**
 *  Initialise the pseudo-triangulation variables.
 *  
 *  @param [in]     KDB*    dbe         model   
 *  @global [out]   int*    KSIM_PERM   vector of equations permutations. Filled with -1 at start.
 *  
 */
int CSimulation::KE_tri_begin(KDB* dbe)
{
    int     i, nb;

    nb = dbe->size();
    KSIM_PERM = (int *) SW_nalloc(sizeof(int) * nb);
    for(i = 0 ; i < nb ; i++) KSIM_PERM[i] = -1;
    for(i = 0 ; i < KSIM_INTER ; i++)
        KSIM_PERM[KSIM_ORDER[KSIM_PRE + i]] = i;

    return(0);
}


/**
 *  Saves int KSIM_ORDER the changes computed by the triangulation algorithm.
 *  
 *  @param [in]     KDB*    dbe         model   
 *  @global [out]   int*    KSIM_PERM   vector of equations permutations.
 *  
 */
 
int CSimulation::KE_tri_end(KDB* dbe)
{
    int     i;

    for(i = 0 ; i < dbe->size() ; i++)
        if(KSIM_PERM[i] >= 0)
            KSIM_ORDER[KSIM_PRE + KSIM_PERM[i]] = i;

    SW_nfree(KSIM_PERM);
    return(0);
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
void CSimulation::KE_tri_perm1(KDB* dbe, int i, int* vars)
{
    int     j, m = -1, posj, nbe = dbe->size(), ksim_permi = KSIM_PERM[i];

    // calcul de l'eq jm dont le numéro d'ordre de calcul est le plus grand
    for(j = 1 ; j <= vars[0] ; j++) {
        if(vars[j] < 0) continue; 	// var endogène de l'eq
        posj = KSIM_PERM[vars[j]];  // position actuelle de l'eq j

        if(posj > m) {
            m = posj;
        }
    }

    // Si le numéro d'ordre max des eq dont dépend la courante est < numéro d'ordre de la courante : ok
    if(m < ksim_permi) return;

    for(j = 0 ; j < nbe ; j++)
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
void CSimulation::KE_tri(KDB* dbe, int** predecessors, int passes)
{
    int     i, var, j;
    int     cpu_sort;
    
    cpu_sort = WscrGetMS();
    KE_tri_begin(dbe);

    for(j = 0 ; j < passes ; j++) {
        for(i = 0 ; i < KSIM_INTER ; i++) {
            var = KSIM_ORDER[KSIM_PRE + i];
            KE_tri_perm1(dbe, var, predecessors[var]);
        }
    }

    KE_tri_end(dbe);
    
    KSIM_CPU_SORT = WscrGetMS() - cpu_sort;
}

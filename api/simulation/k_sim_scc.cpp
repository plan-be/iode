/** 
 *  @header4iode
 *
 *  Strongly Connex Components (SCC) API: alternative functions to reorder and simulate very large models.
 *  
 *  The reordering algorithm can be CPU intensive for very large models. (Large) Models can be split into 
 *  Stronly Connex Components (**SCC**) and each SCC is reordered. The optimized order 
 *  is then saved in IODE lists. 
 *  
 *  The simulation can later be run multiple times without having to reorder the model for each simulation.
 *   
 *  List of functions 
 *  -----------------
 *  
 *   int KE_ModelCalcSCC(KDB* dbe, int tris, char* pre, char* inter, char* post)                         Reorders the model defined by dbe and saves 3 lists with prolog, epilog and interdependent blocks.     
 *   int K_simul_SCC(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** pre, char** inter, char** post)  Simulates a model in the order given by 3 lists of tables of equation names: pre, inter and post.
 *
 */
#include "api/constants.h"
#include "api/k_super.h"
#include "api/b_errors.h"
#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/equations.h"
#include "api/objs/variables.h"
#include "api/simulation/simulation.h"


/**
 *  Reorders the model defined by dbe and saves 3 lists with prolog, epilog and interdependent blocks.
 *  
 *  @param [in] KDB*    dbe     KDB of equations defining the model
 *  @param [in] int     tris    nombre of pseudo-triangulation passes
 *  @param [in] char*   pre     name of the prolog list 
 *  @param [in] char*   inter   name of the interdependent list 
 *  @param [in] char*   post    name of the epilog list 
 *  @return     int             0 if ok, -1 if dbe is empty    
 *  
 */
int CSimulation::KE_ModelCalcSCC(KDB* dbe, int tris, char* pre, char* inter, char* post)
{
    int i,
        opasses = KSIM_PASSES,
        osort = KSIM_SORT;

    if(KNB(dbe) == 0) {
        std::string error_msg = "Empty set of equations";
        error_manager.append_error(error_msg);
        return(-1);
    }

    KSIM_DBE = dbe;
    KSIM_DBV = dbe; // Pour reconstruire les listes dans K_lstorder via KSIM_NAME
    KSIM_MAXDEPTH = KNB(dbe);
    KSIM_PASSES = tris;

    if(tris > 0) KSIM_SORT = SORT_BOTH;
    else         KSIM_SORT = SORT_CONNEX;

    // KSIM_POSXK[i] = num dans dbv de la var endogène de l'équation i
    // KSIM_POSXK_REV[i] = pos in KSIM_DBE of the eq whose endo is var[i] 
    KSIM_POSXK = (int *) SW_nalloc((int)(sizeof(int) * KNB(dbe)));
    KSIM_POSXK_REV = (int *) SW_nalloc((int)(sizeof(int) * KNB(dbe)));
    for(i = 0 ; i < KNB(dbe); i++) {
        KSIM_POSXK_REV[i] = -1;  
    }
    
    // PSEUDO LINK EQUATIONS ie set num endo = num eq
    kmsg("Pseudo-linking equations ....");
    for(i = 0 ; i < KNB(dbe); i++) {
        KSIM_POSXK[i] = i;
        KSIM_POSXK_REV[i] = i;
        L_link_endos(dbe, KECLEC(dbe, i));
    }

    /* ORDERING EQUATIONS */
    KE_order(dbe, NULL);
    K_lstorder(pre, inter, post);

    SW_nfree(KSIM_POSXK);
    SW_nfree(KSIM_POSXK_REV);
    SW_nfree(KSIM_ORDER);
    KSIM_POSXK = KSIM_POSXK_REV = KSIM_ORDER = NULL;
    KSIM_PASSES = opasses;
    KSIM_SORT = osort;
    return(0);
}


/**
 *  Initialize the function K_simul_SCC by allocating the needed global vars and linking the equations.
 *  
 *  @param [in]         KDB*    dbe         KE_WS or subset of KE_WS containing all the model equations
 *  @param [in, out]    KDB*    dbv         KDB containing at minimum the model variables (endo + exo)
 *  @param [in]         KDB*    dbs         KDB containing the model scalars
 *  @param [in]         SAMPLE* smpl        simulation SAMPLE
 *  @return             int                 0 on success, -1 if dbe is empty or
 *                                                              smpl in incompatible with that of dbs or
 *                                                              le link is impossible
 *                                                              the simulation does not succeed
 *  
 */
int CSimulation::K_simul_SCC_init(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl)
{
    int     i, t, rc = 0;

    if(KNB(dbe) == 0) {
        std::string error_msg = "Empty set of equations";
        error_manager.append_error(error_msg);
        return(-1);
    }

    KSIM_DBV = dbv;
    KSIM_DBE = dbe;
    KSIM_MAXDEPTH = KNB(dbe);
    KSIM_DBS = dbs;

    // Check Sample dans les bornes du WS
    t = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));
    if(t < 0 || PER_diff_per(&(KSMPL(dbv)->s_p2), &(smpl->s_p2)) < 0) {
        std::string error_msg = "Simulation sample out of the Variables sample boundaries";
        error_manager.append_error(error_msg);
        return(-1);
    }

    // KSIM_POSXK[i] = num dans dbv de la var endogène de l'équation i
    KSIM_POSXK = (int *) SW_nalloc(sizeof(int) * KNB(dbe));

    // Initialise les nouvelles vars pour conserver les résultats de sim
    // NE PAS FREEER KSIM_NORMS ni KSIM_NITERS CAR UTILISES POUR LE REPORTING A POSTERIORI !!
    SCR_free(KSIM_NORMS);
    SCR_free(KSIM_NITERS);
    SCR_free(KSIM_CPUS);
    KSIM_NORMS = (double *) SCR_malloc(sizeof(double) * KSMPL(dbv)->s_nb);
    KSIM_NITERS = (int *) SCR_malloc(sizeof(int) * KSMPL(dbv)->s_nb);
    KSIM_CPUS = (long *) SCR_malloc(sizeof(long) * KSMPL(dbv)->s_nb);

    /* LINK EQUATIONS + SAVE ENDO POSITIONS */
    kmsg("Linking equations ....");
    for(i = 0 ; i < KNB(dbe); i++) {
        KSIM_POSXK[i] = K_find(dbv, KONAME(dbe,i));
        if(KSIM_POSXK[i] < 0) {
            std::string error_msg = "'" + std::string(KONAME(dbe, i)) + "': cannot find variable";
            error_manager.append_error(error_msg);
            rc = -1;
            goto fin;
        }
        
        rc = L_link(dbv, dbs, KECLEC(dbe, i));
        if(rc) {
            std::string error_msg = "'" + std::string(KONAME(dbe, i)) + "': cannot link equation";
            error_manager.append_error(error_msg);
            rc = -1;
            goto fin;
        }
    }

    KSIM_XK  = (double *) SW_nalloc(sizeof(double) * KSIM_INTER);
    KSIM_XK1 = (double *) SW_nalloc(sizeof(double) * KSIM_INTER);
    return(0);

fin:
    SW_nfree(KSIM_POSXK);
    KSIM_POSXK = 0;
    return(rc);
}


/**
 *  Simulates a model in the order given by 3 lists of tables of equation names: pre, inter and post.
 *  
 *  @param [in]         KDB*    dbe         KE_WS or subset of KE_WS containing all the model equations
 *  @param [in, out]    KDB*    dbv         KDB containing at minimum the model variables (endo + exo)
 *  @param [in]         KDB*    dbs         KDB containing the model scalars
 *  @param [in]         SAMPLE* smpl        simulation SAMPLE
 *  @param [in]         char*   pre         name of the prolog list 
 *  @param [in]         char*   inter       name of the interdependent list
 *  @param [in]         char*   post        name of the epilog list 
 *  @return             int                 0 on success, -1 on error
 *  
 */
int CSimulation::K_simul_SCC(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** pre, char** inter, char** post)
{
    int     i, t, j, rc = -1;

    KSIM_PRE = SCR_tbl_size((unsigned char**) pre);
    KSIM_INTER = SCR_tbl_size((unsigned char**) inter);
    KSIM_POST = SCR_tbl_size((unsigned char**) post);

    if(K_simul_SCC_init(dbe, dbv, dbs, smpl)) return(-1);

    // Fixe l'ordre d'exécution dans KSIM_ORDER
    KSIM_ORDER = (int *)  SW_nalloc(sizeof(int) * (KSIM_PRE + KSIM_INTER + KSIM_POST));
    for(i = j = 0; i < KSIM_PRE; i++)   KSIM_ORDER[j++] = K_find(dbe, pre[i]);
    for(i = 0;     i < KSIM_INTER; i++) KSIM_ORDER[j++] = K_find(dbe, inter[i]);
    for(i = 0;     i < KSIM_POST; i++)  KSIM_ORDER[j++] = K_find(dbe, post[i]);

    // Simulation
    t = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));

    for(i = 0; i < smpl->s_nb; i++, t++) {
        if(rc = K_simul_1(t)) goto fin;
    }

fin:
    K_simul_free();
    return(rc);
}

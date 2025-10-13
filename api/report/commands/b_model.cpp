/**
 *  @header4iode
 * 
 *  Report functions related to model simulations. 
 *  
 *  See b_data.c for more details on the report function syntax and logic.
 *  
 *  List of functions 
 *  -----------------
 *      int B_ModelSimulate(char *arg)                              $ModelSimulate per_from per_to equation_list
 *      int B_ModelSimulateParms(char* arg, int unused)                         $ModelSimulateParms eps relax maxit {Connex | Triang | None } 0 - 4 (starting values) {Yes | no } nbtri {yes | No } 
 *      int B_ModelExchange(char* arg, int unused)                              $ModelExchange eqname1-varname1,eqname2-varname2,...
 *      int KE_compile(KDB* dbe)                                    Recompiles a KDB of equations. Tests and other informations saved in the equation object are left unchanged.
 *      int B_ModelCompile(char* arg, int unused)                               $ModelCompile  [eqname1, eqname2, ... ]
 *      int B_ModelCalcSCC(char *arg)                               $ModelCalcSCC nbtris prename intername postname [eqs]
 *      int B_ModelSimulateSCC(char *arg)                           $ModelSimulateSCC from to pre inter post
 *      int B_ModelSimulateSaveNIters(char *arg)                    $ModelSimulateSaveNiters varname
 *      int B_ModelSimulateSaveNorms(char *arg)                     $ModelSimulateSaveNorms varname
 */
#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/equations.h"
#include "api/objs/lists.h"
#include "api/objs/variables.h"
#include "api/simulation/simulation.h"

#include "api/report/commands/commands.h"


/* Model Simulation */

/**
 *  Simulates a model (i.e. a system of equations). If eqs is NULL or empty, uses the full KE_WS as 
 *  system of equations.
 *  
 *  Sub-function of B_ModelSimulate().
 *  
 *  @param [in] Sample* smpl    simulation Sample
 *  @param [in] char**  eqs     NULL or list of equations defining the model 
 *  @return     int             0 on success, return code of K_simul() on error.    
 */
static int B_ModelSimulateEqs(Sample* smpl, char** eqs)
{
    KDB     *tdbe;
    int     rc;
    CSimulation simu;

    if(eqs == NULL || SCR_tbl_size((unsigned char**) eqs) == 0)
        rc = simu.K_simul(K_WS[EQUATIONS], K_WS[VARIABLES], K_WS[SCALARS], smpl, CSimulation::KSIM_EXO, NULL);
    else 
    {
        tdbe = K_refer(K_WS[EQUATIONS], SCR_tbl_size((unsigned char**) eqs), eqs);
        rc = simu.K_simul(tdbe, K_WS[VARIABLES], K_WS[SCALARS], smpl, CSimulation::KSIM_EXO, eqs);
        delete tdbe;
        tdbe = nullptr;
    }

    return rc;
}


/**
 *  Syntax: $ModelSimulate per_from per_to equation_list
 *  
 *  @see https://iode.plan.be/doku.php?id=modelsimulate
 */
int B_ModelSimulate(char *const_arg, int unused)
{
    int     lg1, lg2;
    int     rc = -1;
    char    from[16], to[16], **eqs = 0;
    Sample  *smpl = nullptr;
    char    *arg;

    // Copy for C++ strings = read only (const)
    arg = (char*) SCR_stracpy((unsigned char*) const_arg);
    
    lg1 = B_get_arg0(from, arg, 15);
    lg2 = B_get_arg0(to, arg + lg1, 15);
    try
    {
        smpl = new Sample(std::string((char*) from), std::string((char*) to));
    }
    catch(const std::exception& e)
    {
        std::string error_msg = "ModelSimulate: invalid sample\n" + std::string(e.what()); 
        error_manager.append_error(error_msg);
        goto err;
    }

    eqs = B_ainit_chk(arg + lg1 + lg2, NULL, 0);
    if(eqs == 0) 
        goto err;
    
    rc = B_ModelSimulateEqs(smpl, eqs);

err:
    if(smpl) delete smpl;
    smpl = nullptr;
    SCR_free_tbl((unsigned char**) eqs);
    SCR_free(arg);
    return rc;
}


/**
 *  Defines simulation parameters.
 *  
 *  Syntax: $ModelSimulateParms eps relax maxit {Connex | Triang | None } 0 - 4 (starting values) {Yes | no } nbtri {yes | No } 
 *          where:
 *              eps  := convergence threshold
 *              0.1 <= relax <= 1.0
 *          
 *  @see https://iode.plan.be/doku.php?id=modelsimulateparms
 */
int B_ModelSimulateParms(char* arg, int unused)
{
    char    **args;
    int     rc = 0, nargs;

    args = (char **) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
    nargs = SCR_tbl_size((unsigned char**) args);
    if(nargs < 6) {
        error_manager.append_error("ModelSimulateParms: incorrect number of parameters");
        rc = -1;
        goto fin;
    }

    CSimulation::KSIM_EPS   = atof(args[0]);
    CSimulation::KSIM_RELAX = atof(args[1]);
    CSimulation::KSIM_MAXIT = atoi(args[2]);
    if(U_is_in(args[3][0], "bB")) 
        args[3][0] = 'T'; // JMP 20/02/2023 
    CSimulation::KSIM_SORT  = B_argpos("CTN", args[3][0]);
    CSimulation::KSIM_START = args[4][0] - '0';
    CSimulation::KSIM_DEBUG = B_argpos("NY", args[5][0]);
    if(nargs > 6) CSimulation::KSIM_PASSES = atoi(args[6]); //JMP 14/3/2012

    if(nargs > 7)
        CSimulation::KSIM_NEWTON_DEBUG = B_argpos("NY", args[7][0]); // JMP 6/3/2012
    else
        CSimulation::KSIM_NEWTON_DEBUG = 0;

fin :
    SCR_free_tbl((unsigned char**) args);
    return(rc);
}


/**
 *  Defines goalSeeking parameters.
 *   
 *  Syntax: $ModelExchange eqname1-varname1,eqname2-varname2,...
 *           where eqname1, eqname2 are equation names (thus also endogenous vars)
 *                 varname1, varname2 are exogenous variables
 *  
 *  @see https://iode.plan.be/doku.php?id=modelexchange
 */
int B_ModelExchange(char* const_arg, int unused)
{
    char    *arg;

    // Copy for C++ strings = read only (const)
    arg = (char*) SCR_stracpy((unsigned char*) const_arg);
    
    if(CSimulation::KSIM_EXO) {
        SCR_free_tbl((unsigned char**) CSimulation::KSIM_EXO);
        CSimulation::KSIM_EXO = NULL;
    }

    if(arg && SCR_strip((unsigned char*) arg)[0]) 
        CSimulation::KSIM_EXO = B_ainit_chk(arg, NULL, 0);
    
    SCR_free(arg);
    return(0);
}


/**
 *  Recompiles a KDB of equations. Tests and other informations saved in the equation object are left unchanged.
 *  
 *  Useful to reflect new values of lists when equations formulas contain macros ($LISTNAME). 
 *  
 *  @param [in, out] KDB*   dbe     KDB of equations to recompile
 *  @return          int            0 on success, -1 of dbe is null or empty
 *  
 *  TODO: return -1 if K_upd_eqs() fails ?
 */
int KE_compile(KDB* dbe)
{
    if(dbe == NULL || dbe->size() == 0) {
        error_manager.append_error("Empty set of equations");
        return -1;
    }

    Equation* eq;
    for(int i = 0; i < dbe->size(); i++) 
    {
        eq = KEVAL(dbe, i);
        K_upd_eqs(KONAME(dbe, i), (char*) eq->lec.c_str(), NULL, 0, NULL, NULL, NULL, NULL, 0);
        delete eq;
        eq = nullptr;
    }
    
    return 0;
}


/**
 *  Syntax: $ModelCompile [eqname1, eqname2, ... ]
 *  
 *  @see https://iode.plan.be/doku.php?id=modelsimulate
 */
int B_ModelCompile(char* arg, int unused)
{
    char    **eqs = NULL;
    KDB     *tdbe = NULL;
    int     rc;

    if(arg == NULL || arg[0] == 0) 
    {
        /* EndoExo whole WS */
        return(KE_compile(K_WS[EQUATIONS]));
    }
    else 
    {
        eqs = B_ainit_chk(arg, NULL, 0);
        if(eqs == NULL || SCR_tbl_size((unsigned char**) eqs) == 0)
            return(KE_compile(K_WS[EQUATIONS]));
        else 
        {
            tdbe = K_refer(K_WS[EQUATIONS], SCR_tbl_size((unsigned char**) eqs), eqs);
            rc = KE_compile(tdbe);
            delete tdbe;
            tdbe = nullptr;
            SCR_free_tbl((unsigned char**) eqs);
        }
    }

    return rc;
}


/**
 *  Syntax: $ModelCalcSCC nbtris prename intername postname [eqs]
 *  
 *  @see https://iode.plan.be/doku.php?id=ModelCalcSCC
 */
int B_ModelCalcSCC(char *const_arg, int unused)
{
    char    **eqs, buf[256], pre[64], inter[64], post[64];
    int     rc = -1, lg1, tris;
    KDB		*tdbe = NULL;
    char    *arg;
    CSimulation simu;

    // Copy for C++ strings = read only (const)
    arg = (char*) SCR_stracpy((unsigned char*) const_arg);
    
    // Tri
    lg1 = B_get_arg0(buf, arg, 15);
    tris = atoi(buf); 
    if(tris < 0) tris = 0;

    // result list names
    lg1 += B_get_arg0(pre,   arg + lg1, 20);
    lg1 += B_get_arg0(inter, arg + lg1, 20);
    lg1 += B_get_arg0(post,  arg + lg1, 20);
    if(strlen(pre) == 0 || strlen(inter) == 0 || strlen(post) == 0) goto err;

    // eqs if given
    eqs = B_ainit_chk(arg + lg1, NULL, 0);
    if(SCR_tbl_size((unsigned char**) eqs) == 0)
        tdbe = K_WS[EQUATIONS];
    else
        tdbe = K_quick_refer(K_WS[EQUATIONS], eqs);

    rc = simu.KE_ModelCalcSCC(tdbe, tris, pre, inter, post);

    if(SCR_tbl_size((unsigned char**) eqs) != 0)
    {
        delete tdbe;
        tdbe = nullptr;
    }

err:
    SCR_free_tbl((unsigned char**) eqs);
    SCR_free(arg);
    return rc;
}


/**
 *  Simulates a model based on 3 precalculated lists pre, post and inter.
 *  
 *  Syntax: $ModelSimulateSCC from to pre inter post
 *  
 *  @see https://iode.plan.be/doku.php?id=ModelSimulateSCC
 */
int B_ModelSimulateSCC(char *const_arg, int unused)
{
    int     lg1, lg2, rc, prepos, interpos, postpos;
    char    from[16], to[16], **lsts = 0, **eqs, **eqs1, **pre, **post, **inter;
    Sample  *smpl = nullptr;
    KDB     *tdbe;
    char    *arg;
    CSimulation simu;

    // Copy for C++ strings = read only (const)
    arg = (char*) SCR_stracpy((unsigned char*) const_arg);

    lg1 = B_get_arg0(from, arg, 15);
    lg2 = B_get_arg0(to, arg + lg1, 15);
    try
    {
        smpl = new Sample(std::string((char*) from), std::string((char*) to));
    }
    catch(const std::exception& e)
    {
        SCR_free(arg);
        std::string error_msg = "ModelSimulateSCC: invalid sample\n" + std::string(e.what());
        error_manager.append_error(error_msg);
        return(-1);
    }

    // Extrait les listes restantes
    lsts = B_ainit_chk(arg + lg1 + lg2, NULL, 0);
    if(lsts == 0 || SCR_tbl_size((unsigned char**) lsts) != 3) 
    {
        error_manager.append_error("ModelSimulateSCC: syntax error in lists");
        SCR_free_tbl((unsigned char**) lsts);
        rc = -1;
        goto err;
    }

    prepos   = K_find(K_WS[LISTS], lsts[0]); 
    interpos = K_find(K_WS[LISTS], lsts[1]);
    postpos  = K_find(K_WS[LISTS], lsts[2]);
    SCR_free_tbl((unsigned char**) lsts);

    if(prepos < 0 || interpos < 0 || postpos < 0) 
    {
        rc = -1;
        error_manager.append_error("ModelSimulateSCC: pre, post or inter list not found in the Lists workspace");
        goto err;
    }

    pre   = (char**) KL_expand(KLVAL(K_WS[LISTS], prepos));
    inter = (char**) KL_expand(KLVAL(K_WS[LISTS], interpos));
    post  = (char**) KL_expand(KLVAL(K_WS[LISTS], postpos));

    // Regroupe les listes dans une seule avant de faire K_quick_refer
    eqs1 = (char**) SCR_union_quick((unsigned char**) pre, (unsigned char**) inter); // JMP 29/8/2012
    eqs = (char**) SCR_union_quick((unsigned char**) eqs1, (unsigned char**) post);  // JMP 29/8/2012
    SCR_free_tbl((unsigned char**) eqs1);                 // JMP 29/8/2012
    tdbe = K_quick_refer(K_WS[EQUATIONS], eqs);
    SCR_free_tbl((unsigned char**) eqs);

    // Lance la simulation
    rc = simu.K_simul_SCC(tdbe, K_WS[VARIABLES], K_WS[SCALARS], smpl, pre, inter, post);

    // Cleanup
    delete tdbe;
    tdbe = nullptr;
    SCR_free_tbl((unsigned char**) pre);
    SCR_free_tbl((unsigned char**) inter);
    SCR_free_tbl((unsigned char**) post);

err:
    if(smpl) delete smpl;
    smpl = nullptr;
    SCR_free(arg);
    return rc;
}


/**
 *  Set all values of the variable name to NaN. The variable is created if it does not exist.
 *  
 *  @param [in] char*   name    var name
 *  @return     int             0 always    
 */
static int B_CreateEmptyVar(char *name)
{
    char        uname[1024], buf[4096];

    // Create var
    strcpy(uname, name);
    SCR_sqz(SCR_upper((unsigned char*) uname));
    sprintf(buf, "%s 1/0", uname);
    B_DataCalcVar(buf);
    return(0);
}


/**
 *  Retrieves the pointer to the first value of a variable data.
 *  
 *  @param [in] char*       name    variable name
 *  @return     double*          pointer to the var "name"
 */
static double *B_GetVarPtr(char *name)
{
    int     pos;
    KDB     *dbv = K_WS[VARIABLES];
    char    uname[1024];

    // get var position in KDB
    strcpy(uname, name);
    SCR_sqz(SCR_upper((unsigned char*) uname));
    pos = K_find(dbv, uname);
    if(pos < 0)
        return(NULL);
    else
        return(KVVAL(dbv, pos, 0));
}


/**
 *  Saves the values of the vector of double vec in the variable name. 
 *  Creates name if it does not exist.
 *  
 *  @param [in, out] char*       name   variable name
 *  @param [in]      double*  vec    pointer to the values to copy in name
 */
static int B_CreateVarFromVecOfDoubles(char *name, double *vec)
{
    KDB         *dbv = K_WS[VARIABLES];
    double      *x;
    int         t;

    // Create var and get Ptr
    B_CreateEmptyVar(name);
    x = B_GetVarPtr(name);
    if(x == 0) return(-1);

    // Copy values
    if(vec) 
    {
        for(t = 0; t < dbv->sample->nb_periods; t++)
            x[t] = vec[t];
    }
    return(0);
}

/**
 *  Saves into the variable "name" the values of the vector vec of integers.
 *  Creates name if it does not exist.
 *  
 *  @param [in, out] char*      name   variable name
 *  @param [in]      int*       vec    pointer to the values to copy in name
 */
static int B_CreateVarFromVecOfInts(char *name, int *vec)
{
    KDB         *dbv = K_WS[VARIABLES];
    double      *x;
    int         t;

    // Create var and get Ptr
    B_CreateEmptyVar(name);
    x = B_GetVarPtr(name);
    if(x == 0) 
    {
        std::string error_msg = "B_CreateVarFromVecOfInts: failed to create the variable '";
        error_msg += std::string(name) + "'";
        error_manager.append_error(error_msg);
        return(-1);
    }

    // Copy values
    if(vec) 
    {
        for(t = 0; t < dbv->sample->nb_periods; t++)
            x[t] = vec[t];
    }
    return(0);
}


/**
 *  Saves the number of iterations needed to complete each simulated period in a variable.
 *  
 *  Syntax: $ModelSimulateSaveNiters varname
 *  
 *  @see https://iode.plan.be/doku.php?id=ModelSimulateSaveNIters
 */
int B_ModelSimulateSaveNIters(char *arg, int unused)
{
    return(B_CreateVarFromVecOfInts(arg, CSimulation::KSIM_NITERS));
}


/**
 *  Saves the convergence threshold reached to complete each simulated period in a variable.
 *  
 *  Syntax: $ModelSimulateSaveNorms varname
 *  
 *  @see https://iode.plan.be/doku.php?id=ModelSimulateSaveNorms
 */
int B_ModelSimulateSaveNorms(char *arg, int unused)
{
    return(B_CreateVarFromVecOfDoubles(arg, CSimulation::KSIM_NORMS));
}

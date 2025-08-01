/**
 *  @header4iode
 * 
 *  Report function that estimates a block of equations and finds the best possible tests 
 *  for all possible combinations of coefficients.
 *  
 *  List of functions 
 *  -----------------
 *      int B_EqsStepWise(char* arg, int unused) | $EqsStepWise from to eqname leccond {r2|fstat}
 */
#include "api/constants.h"
#include "api/b_args.h"
#include "api/k_super.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/estimation/estimation.h"
#include "api/report/commands/commands.h"
#include "api/report/undoc/undoc.h"


/**
 *  Analyses an equation and checks that all variables exist in KV_WS. 
 *  Initializes to 0.9 the scalars that are not yet defined in KS_WS.
 *  
 *  Sub-function of B_EqsStepWise().
 *  
 *  @param [in] char*   eqs     equation name
 *  @return     int             1 on success, -1 if some variable present in eqs does not exist in KV_WS
 *  
 *  TODO: éliminer les B_DataUpdate et autres fonctions de haut niveau
 */
static int check_scl_var(char *eqs)
{
    int     pos,j;
    EQ*     eq;
    CLEC*   cl;
    char    buf[1024];

    pos = K_find(K_WS[EQUATIONS], eqs);
    if(pos < 0) return(-1);             // JMP 04/07/2022
    eq = KEVAL(K_WS[EQUATIONS], pos);
    cl = eq->clec;

    for(j = 0 ; j < cl->nb_names ; j++) {
        if(L_ISCOEF(cl->lnames[j].name)) {
            if(K_find(K_WS[SCALARS],cl->lnames[j].name)== -1) {
                sprintf(buf, "%s 0.9 1", cl->lnames[j].name);
                B_DataUpdate(buf, SCALARS);
            }
        }
        else {
            if(K_find(K_WS[VARIABLES],cl->lnames[j].name)== -1) {
                kerror(0,"Var %s from %s not found",cl->lnames[j].name,eqs);
                E_free(eq); // JMP 08/07/2022
                return(-1);
            }
        }
    }
    
    E_free(eq);  // JMP 08/07/2022
    return(1);
}


/**
 *  This function estimates a block of equations and finds the best possible tests for all possible 
 *  combinations of coefficients.
 *  
 *  $EqsStepWise from to eqname leccond {r2|fstat}
 *       from to : estimation period
 *       eqname  : equation to estimate
 *       leccond : condition of eligibility
 */
int B_EqsStepWise(char* arg, int unused)                                                 
{
    char    *from, *to, *eqs, *cond, *test, *tmp;
    char**  args;
    SAMPLE  *smpl;

    args = B_vtom_chk(arg, 5);
    if(args == NULL) 
        return(1);

    from = args[0];                                              
    to = args[1];                                                
    smpl = PER_atosmpl(from, to);                                /*Calcule le sample*/
    if(smpl == NULL) {                                           /*Gère les erreurs de sample*/
        kerror(0,"Incorrect sample");
        SCR_free_tbl((unsigned char**) args);
        return(1);
    }

    eqs = args[2];                                               
    if(K_find(K_WS[EQUATIONS], eqs)== -1) {                            /*Gère les erreurs d'équation*/
        kerror(0,"Eqs %s not found",eqs);
        SCR_free_tbl((unsigned char**) args);
        return(1);
    }

    cond = args[3];                                              
    if(C_evallec(cond, 0)==-1)                                       /*Gère les erreurs de condition*/
    {
        SCR_free_tbl((unsigned char**) args);
        return(1);                          
    }

    // Sanitizer -> need to make a copy of str to avoid the 
    // strcpy-param-overlap error (from SCR_add_ptr())
    tmp = args[4];
    test = (char*) SCR_stracpy(SCR_lower((unsigned char*) tmp));
    if(strcmp(test,"r2")!=0 && strcmp(test,"fstat")!=0)         /*Gère les erreurs de test*/
    {
        kerror(0,"Incorrect test name");
        SCR_free_tbl((unsigned char**) args);
        return(1);
    }

    if(check_scl_var(eqs) == -1)                                /*Gère les erreurs de présence des scalaires et variables de l'équation*/
    {
        SCR_free_tbl((unsigned char**) args);
        return(1);                      
    }

    estimate_step_wise(smpl, eqs, cond, test);                          /*Effectue les estimations*/
    
    SCR_free_tbl((unsigned char**) args);
    return(0);
}

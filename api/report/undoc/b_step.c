/**
 *  @header4iode
 * 
 *  Report function that estimates a block of equations and finds the best possible tests 
 *  for all possible combinations of coefficients.
 *  
 *  List of functions 
 *  -----------------
 *      int B_EqsStepWise(char* arg) | $EqsStepWise from to eqname leccond {r2|fstat}
 */

#include "iode.h"


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
int B_EqsStepWise(char* arg)                                                 
{
    int     lg1, lg2,lg3,lg4,lg5;
    char    from[16], to[16], eqs[30], cond[1024], test[20];
    SAMPLE  *smpl;

    lg1 = B_get_arg0(from, arg, 15);                              /*Lit les arguments*/
    lg2 = B_get_arg0(to, arg + lg1, 15);
    lg3 = B_get_arg0(eqs, arg + lg1 + lg2, 29);
    lg4 = B_get_arg0(cond, arg + lg1 + lg2 + lg3, 1023);
    lg5 = B_get_arg0(test, arg + lg1 + lg2 + lg3 + lg4, 19);

    smpl = PER_atosmpl(from, to);                                /*Calcul le sample*/

    if(smpl == NULL) {                                           /*GŠre les erreurs de sample*/
        kerror(0,"Incorrect sample");
        return(1);
    }
    if(K_find(K_WS[EQUATIONS],eqs)== -1) {                            /*GŠre les erreurs d'‚quation*/
        kerror(0,"Eqs %s not found",eqs);
        return(1);
    }

    if(C_evallec(cond,0)==-1)return(1);                          /*GŠre les erreurs de condition*/

    strcpy(test,SCR_lower(test));
    if(strcmp(test,"r2")!=0 && strcmp(test,"fstat")!=0) {        /*GŠre les erreurs de test*/
        kerror(0,"Incorrect test name");
        return(1);
    }
    if(check_scl_var(eqs) == -1)return(1);                      /*GŠre les erreurs de pr‚sence des scalaires et variables de l'‚quation*/
    E_StepWise(smpl, eqs, cond, test);                          /*Effectue les estimations*/
    return(0);
}

//print_result(double F,double R,double cond,char **tab_scl,int nb_scls,int combi,char *cond_string)  /*Imprime le r‚sultat d'un estimation*/
//{
//    double res[3];
//    char *tmp;
//    FILE *output;
//    int j;
//
//    tmp=(char *)malloc(30);
//    output=fopen("stepwise.txt","a");
//    fprintf(output,"\nStep %d : (F_Stat = %f;R2 = %f;Condition(%s) = %f) ",combi,F,R,cond_string,cond);
//    for(j=0; j<nb_scls; j++) {
//        sprintf(tmp,"%s",tab_scl[j]);
//        E_SclToReal(tmp,res);
//        fprintf(output,"%s = %f ",tab_scl[j],res[0]);
//    }
//    SCR_free(tmp);
//    fclose(output);
//}


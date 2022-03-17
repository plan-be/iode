/** 
 *  @header4iode
 *  
 *  Exchange Endo-Exo
 *  -----------------
 *  
 *  Functions to invert equations in order to solve the equation system with respect to an 
 *  alternative set of variables.
 *
 *  It is possible to exchange the status of an exogenous variable with that of an endogenous variable. 
 *  This allows, when the value of an endogenous variable is known in advance, 
 *  for example during the first simulation period, to block this endogenous variable by letting an exogenous variable vary 
 *  so that the known value of the endogenous variable is preserved. 
 *  
 *  The method used is to look for a path between the exo and the endo in the equations of the model. 
 *  
 *  If exo appears in the equation endo, the path is trivial and we simply change the name of the endogeneous in the equation. 
 *  Note that this will involve using a Newton-Raphson method to solve the equation during the simulation. 
 *  
 *  Otherwise, we look for each other endo variable in the equation to see if their defining equation contains exo. 
 *  If so, the status in this equation is exchanged and the path between exo and endo is found in one step. 
 *  If not, we continue the process recursively until we have found a path between endo and exo.
 * 
 *
 *  List of functions 
 *  -----------------
 *  
 *      int KE_exo2endo(int posendo, int posexo)  Modify the model to solve it with respect to another set of variables
 */
 
#include "iode.h"


/**
 *  Modify the model to solve it with respect to another set of variables, 
 *  the variable posendo becoming exogenous and the variable posexo becoming endogenous.
 *  
 *  @param [in] int     posendo     position of the endogenous variable in KSIM_DBV
 *  @param [in] int     posexo      position of the exogenous variable in KSIM_DBV
 *  @return     int                 0 if the exchange is possible, 
 *                                  -1 otherwise: 
 *                                      endo or exo not found in equations, 
 *                                      path between endo and exo inexistent 
 *  
 */
 
int KE_exo2endo(int posendo, int posexo)
{
    int         i, endo, exo;
    int         depth = 0;
    extern char *KSIM_PATH;
    extern KDB  *KSIM_DBV;

    endo = KE_poseq(posendo);
    if(endo < 0) {
        B_seterrn(116, KONAME(KSIM_DBV, posendo));
        return(-1);
    }

    if(KE_poseq(posexo) >= 0) {
        B_seterrn(119, KONAME(KSIM_DBV, posexo));
        return(-1);
    }

    /* check if exo in equation */
    memset(KSIM_PATH, 0, KSIM_MAXDEPTH);
    exo = KE_findpath(posendo, posexo, &depth);
    if(exo < 0) {
        B_seterrn(117, KONAME(KSIM_DBV, posendo), KONAME(KSIM_DBV, posexo));
        return(-1);
    }
    return(0);
}


/**
 *  Recursive function to search a path between posexo and posendo. 
 *  
 *  See more explanation at the top of this module.
 *  
 *  @param [in] int     posendo     position of the endogenous variable in KSIM_DBV
 *  @param [in] int     posexo      position of the exogenous variable in KSIM_DBV
 *  @param [in] int*    depth       current level of recursivity (starts at 0 and increase each time KE)
 *  @return     int                 0 on success, -1 on error
 *  
 */
static int KE_findpath(int posendo, int posexo, int* depth)
{
    int         lg, j, poseq, posseq, rc = -1;
    CLEC        *clec = NULL, *eclec;
    extern char *KSIM_PATH;
    extern KDB  *KSIM_DBE;

    if(posexo < 0 || *depth > KSIM_MAXDEPTH) return(-1);

    // Endo and exo are in same equation 
    // => replace the endo var position in KSIM_POSXK by the exo var position 
    poseq = KE_poseq(posendo);
    clec = KECLEC(KSIM_DBE, poseq);
    for(j = 0; j < clec->nb_names; j++) {
        if(L_ISCOEF(clec->lnames[j].name)) continue;
        if((clec->lnames[j]).pos == posexo) {
            KSIM_POSXK[poseq] = posexo;
            return(0);
        }
    }

    // Endo and exo *not* in the same equation
    // => try to find a path between endo and exo and change endo / exo at each step
    for(j = 0; j < clec->nb_names; j++) {
        eclec = KECLEC(KSIM_DBE, poseq);            
        clec = (CLEC *)SW_nalloc(eclec->tot_lg);
        memcpy(clec, eclec, eclec->tot_lg);
        if(L_ISCOEF(clec->lnames[j].name)) continue;
        posseq = KE_poseq((clec->lnames[j]).pos);

        /* if same endo, variable exo or endo already exchanged continue */
        if(poseq == posseq || posseq < 0) continue;

        /* if path already examined continue */
        if(KSIM_PATH[posseq] == 1) continue;
        else KSIM_PATH[posseq] = 1;


        (*depth) ++;
        rc = KE_findpath((clec->lnames[j]).pos, posexo, depth);
        if(rc < 0) {
            (*depth) --;
            continue;
        }

        /*      fprintf(stdout, "%s <- ", clec->lnames[j].name); */
        KSIM_POSXK[poseq] = (clec->lnames[j]).pos;

        (*depth) --;
        SW_nfree(clec);
        return(rc);
    }

    SW_nfree(clec);
    return(-1);
}


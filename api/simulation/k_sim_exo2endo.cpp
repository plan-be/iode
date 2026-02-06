/** 
 *  @header4iode
 *  
 *  Functions to invert equations in order to solve the equation system with respect to an 
 *  alternative set of variables.
 *  
 *  List of functions 
 *  -----------------
 *  
 *      int KE_exo2endo(int posendo, int posexo)  Modify the model to solve it with respect to another set of variables
 *
 *
 *  Exchanges Endo-Exo
 *  ------------------
 *
 *  It is possible to exchange the status of an exogenous variable with that of an endogenous variable. 
 *  That allows, when the value of an endogenous variable is known in advance, 
 *  for example during the first simulation period, to block this endogenous variable by letting an exogenous variable vary 
 *  so that the known value of the endogenous variable is preserved. 
 *  
 *  The method used is to find a path between the exo and the endo by scanning the equations of the model and to modify 
 *  the "status" of the variables accordingly as explained below.
 *   
 *  If "exo" appears in the equation "endo", the path is trivial and we simply replace the endogeneous of that equation.
 *  Note that this requires the use of a Newton-Raphson method to solve the equation with respect 
 *  to the (new) endo (ex exo) variable during the simulation process. 
 *  
 *  If exo does not appear in the equation, we analyse each other endogenous variable in the equation to see 
 *  if their defining equation contains the variable "exo". 
 *  If so, the status in this equation is exchanged and the path between exo and endo is found in one step. 
 *  If not, we continue the process recursively until we have found a path between "endo" and "exo".
 * 
 *  The example below should clarify the process.
 *   
 *      EQ                              ENDO
 *      ------------------------------------
 *      y1 = f1(y2, x1)                 y1
 *      y2 = f2(y3, x1)                 y2
 *      y3 = f3(y1, y2, x1, x2)         y3
 *   
 *  Suppose the exchange y1-x2. 
 *  
 *    x2 does not appear in f1. The only endogenous var in f1 is y2.
 *    x2 does not appear in f2. The only endogenous var in f2 is y3. 
 *    x2 does appear in f3. 
 *      -> x2 becomes the new endogenous of the f3. But as y3 must keep its endogenous "status", we have to change f2 as well.
 *      -> y3 becomes the new endogenous of the f2. Again, y2 must keep its endogenous "status", thus it becomes endo of f1
 *      -> y2 becomes the new endogenous of the f1. 
 *  
 *  Finally, whe obtain the following reorganisation of the model:
 *  
 *      EQ                              ENDO
 *      ------------------------------------
 *      y1 = f1(y2, x1)                 y2
 *      y2 = f2(y3, x1)                 y3
 *      y3 = f3(y1, y2, x1, x2)         x2
 *  
 *  The model is thus solve with respect to {y2,y3,x2} instead of {y1,y2,y3}. The value of y1 if therefore left unchanged.
 *
 *  
 */
#include "api/constants.h"
#include "api/b_errors.h"
#include "api/lec/lec.h"
#include "api/objs/equations.h"
#include "api/simulation/simulation.h"


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
 *  TODO: check that eclec is freed
 *  
 */
int CSimulation::KE_findpath(int posendo, int posexo, int* depth)
{
    int j, poseq, posseq, posvar, rc = -1;

    if(posexo < 0 || *depth > KSIM_MAXDEPTH) 
        return -1;

    Equation* eq = nullptr;

    // Endo and exo are in same equation 
    // => replace the endo var position in KSIM_POSXK by the exo var position 
    std::string coef_name, eq_name;
    poseq = KE_poseq(posendo);
    eq_name = KSIM_DBV->get_name(posendo);
    eq = KSIM_DBE->get_obj_ptr(eq_name);
    CLEC* clec = eq->clec;
    for(j = 0; j < clec->nb_names; j++) 
    {
        coef_name = std::string(clec->lnames[j].name);
        if(is_coefficient(coef_name)) 
            continue;
        if((clec->lnames[j]).pos == posexo) 
        {
            KSIM_POSXK[poseq] = posexo;
            KSIM_POSXK_REV[posexo] = poseq;
            return 0;
        }
    }

    // Endo and exo *not* in the same equation
    // => try to find a path between endo and exo and change endo / exo at each step
    CLEC* eclec = nullptr;
    for(j = 0; j < clec->nb_names; j++) 
    {
        eq_name = KSIM_DBE->get_name(poseq);
        eq = KSIM_DBE->get_obj_ptr(eq_name);
        eclec = eq->clec;            
        clec = (CLEC *)SW_nalloc(eclec->tot_lg);
        memcpy(clec, eclec, eclec->tot_lg);
        coef_name = std::string(clec->lnames[j].name);
        if(is_coefficient(coef_name)) 
            continue;
        posseq = KE_poseq((clec->lnames[j]).pos);

        /* if same endo, variable exo or endo already exchanged continue */
        if(poseq == posseq || posseq < 0) 
            continue;

        /* if path already examined continue */
        if(KSIM_PATH[posseq] == 1) 
            continue;
        else 
            KSIM_PATH[posseq] = 1;

        (*depth) ++;
        rc = KE_findpath((clec->lnames[j]).pos, posexo, depth);
        // If not found, try the next variable in clec
        if(rc < 0) 
        {
            (*depth) --;
            continue;
        }

        /*      fprintf(stdout, "%s <- ", clec->lnames[j].name); */
        // Path found
        // Replace the endo of the KSIM_POSXK[poseq] by the j'd varname in current clec
        posvar = (clec->lnames[j]).pos;
        KSIM_POSXK[poseq] = posvar;
        KSIM_POSXK_REV[posvar] = poseq;
        
        // decrease the depth by 1
        (*depth) --;
        
        // Free the current clec
        SW_nfree(clec);
        
        return rc;
    }

    SW_nfree(clec);
    return -1;
}

/**
 *  Modify the model to solve it with respect to another set of variables, 
 *  the variable posendo becoming exogenous and the variable posexo becoming endogenous.
 *  
 *  If the function succeeds, the vector KSIM_POSXK is modified to reflect the new endogenous for each equation.
 *   
 *  Note that KSIM_POSXK[i] contains for each eq of the model (defined by KSIM_DBE) 
 *  the position in KSIM_DBV of the endo variable of equation KSIM_DBE[i].
 *  
 *  
 *  @param [in] int     posendo     position of the endogenous variable in KSIM_DBV
 *  @param [in] int     posexo      position of the exogenous variable in KSIM_DBV
 *  @return     int                 0 if the exchange is possible, 
 *                                  -1 otherwise: 
 *                                      endo or exo not found in equations, 
 *                                      path between endo and exo inexistent 
 *  
 */
int CSimulation::KE_exo2endo(int posendo, int posexo)
{
    int endo, exo;
    int depth = 0;

    endo = KE_poseq(posendo);
    if(endo < 0) 
    {
        std::string error_msg = "Goal Seeking: ";
        error_msg += std::string(KSIM_DBV->get_name(posendo));
        error_msg += " : no such equation in the Equations workspace";
        error_manager.append_error(error_msg);
        return -1;
    }

    exo = KE_poseq(posexo);
    if(exo >= 0) 
    {
        std::string error_msg = "Goal Seeking: ";
        error_msg += std::string(KSIM_DBV->get_name(posexo));
        error_msg += " already endogeneous";
        error_manager.append_error(error_msg);
        return -1;
    }

    /* check if exo in equation */
    memset(KSIM_PATH, 0, KSIM_MAXDEPTH);
    exo = KE_findpath(posendo, posexo, &depth);
    if(exo < 0) 
    {
        std::string error_msg = "Goal Seeking: ";
        error_msg += std::string(KSIM_DBV->get_name(posendo)) + "-"; 
        error_msg += std::string(KSIM_DBV->get_name(posexo));
        error_msg += " no exchange possible";
        error_manager.append_error(error_msg);
        return -1;
    }

    return 0;
}

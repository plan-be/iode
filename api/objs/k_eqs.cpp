/**
 * @header4iode
 *
 *  Functions to manipulate equation expressions and objects.
 *  
 *  List of functions 
 *  -----------------
 *      void E_free(EQ* eq)                                                     Frees an EQ struct and all its elements
 *      int E_split_eq(char* lec, char** lhs, char** rhs)                       Extracts the left and right sides of a lec equation
 *      int E_dynadj(int method, char* lec, char* c1, char* c2, char** adjlec)  Transforms a LEC equation to add dynamic adjustment
 *      E_DynamicAdjustment(int method, char** eqs, char*c1, char*c2)           Transforms a LEC equation "in place" to add a dynamic adjustment
 *
 */
#include "equations.h"


/**
 *  Frees an EQ struct and all its elements.
 *  
 *  @param [in, out]    EQ*     eq  pointer to the EQ struct to delete
 *  @return             void
 *  
 */
void E_free(EQ* eq)
{
    if(eq == NULL) return;

    SW_nfree(eq->endo);
    SW_nfree(eq->lec);
    SW_nfree(eq->clec);
    SW_nfree(eq->comment);
    SW_nfree(eq->block);
    SW_nfree(eq->instruments);

    SW_nfree(eq);
}


/**
 *  Extracts the left and right sides of a lec equation. 
 *  
 *  @param [in]     char*  lec  text of a LEC equation 
 *  @param [out]    char** lhs  allocated left member
 *  @param [out]    char** rhs  allocated right member
 *  @return         int         0 if ok, 
 *                              -1 if  ":=" is not found in lec  
 *  
 */
int E_split_eq(char* lec, char** lhs, char** rhs)
{
    int     pos = -1;

    *lhs = NULL;
    *rhs = NULL;

    if(lec == NULL) goto done;
    pos = L_split_eq(lec);
    if(pos < 0) goto done;

    lec[pos] = 0;
    *lhs = (char*) SCR_stracpy((unsigned char*) lec);
    *rhs = (char*) SCR_stracpy(((unsigned char*) lec) + pos + 2);

done :
    return(pos);
}


/**
 *  Transforms a LEC equation to add a dynamic adjustment.
 *  
 *  For the equation LHS := RHS:
 *  
 *      - method 0: *adjlec = "d(LHS) := c1 * (RHS - (LHS)[-1])"
 *      - method 1: *adjlec = "d(LHS) := c1 * d(RHS) + c2 * (RHS -LHS)[-1]"
 *    
 *  @param [in]  int    method    type of dyn adjust (0 or 1, see above)
 *  @param [in]  char*  lec       LEC equation
 *  @param [in]  char*  c1        name of the first coefficient
 *  @param [in]  char*  c2        name of the 2d coefficient
 *  @param [out] char** adjlec    pointer to the resulting LEC equation
 *  @return      int              0 if ok, -1 if no ":=" in lec
 *  
 */

int E_dynadj(int method, char* lec, char* c1, char* c2, char** adjlec)
{
    int     lg, rc = -1;
    char    *lhs = NULL, *rhs = NULL;

    *adjlec = NULL;
    if(E_split_eq(lec, &lhs, &rhs) < 0) goto done;
    SCR_strip((unsigned char*) lhs);
    SCR_strip((unsigned char*) rhs);

    if(method == 0) {
        if(c1 == NULL || c1[0] == 0) goto done;

        lg = 2 * (int) strlen(lhs) + (int) strlen(rhs) + 100;
        *adjlec = SCR_malloc(lg);
        sprintf(*adjlec,
                "d(%s) := %s * (%s -(%s)[-1])",
                lhs, c1, rhs, lhs);
    }
    else {
        if(c1 == NULL || c1[0] == 0
                || c2 == NULL || c2[0] == 0) goto done;

        lg = 2 * (int)strlen(lhs) + 2 * (int)strlen(rhs) + 100;
        *adjlec = SCR_malloc(lg);
        sprintf(*adjlec,
                "d(%s) := %s * d(%s) + %s * (%s -%s)[-1]",
                lhs, c1, rhs, c2, rhs, lhs);
    }

    rc = 0;

done :
    SCR_free(lhs);
    SCR_free(rhs);
    return(rc);
}


/**
 *  Transforms a LEC equation "in place" to add a dynamic adjustment. See E_dynadj for details.
 *  
 *    
 *  @param [in]      int    method    type of dyn adjust (0 or 1, see above)
 *  @param [in, out] char** lec       LEC equation before and after transform
 *  @param [in]      char*  c1        name of the first coefficient
 *  @param [in]      char*  c2        name of the 2d coefficient
 *  @return          int              0 if ok, -1 if no ":=" in lec
 *  
 */

int E_DynamicAdjustment(int method, char** eqs, char*c1, char*c2)
{
    char    *ae;

    SCR_strip((unsigned char*) *eqs);
    SCR_strip((unsigned char*) c1);
    SCR_strip((unsigned char*) c2);

    E_dynadj(method, *eqs, c1, c2, &ae);

    SCR_strfacpy((unsigned char**) eqs, (unsigned char*) ae);
    SCR_free(ae);

    return(0);
}

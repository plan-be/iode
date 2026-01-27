/**
 * @header4iode
 *
 * Function to compare 2 IODE objects.
 * -----------------------------------
 *
 *      int K_cmp(char* name, KDB* kdb1, KDB* kdb2) : compare object named name in 2 kdb.
 */ 
#include <math.h>

#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"
#include "api/objs/compare.h"


/**
 *  Compare the size (length) of 2 packed objects. 
 *  
 *  @note  The size of a packed object is stored in the first 4 bytes of the "pack".
 *  
 *  @param [in] p1      char*   pointer to the first packed object.
 *  @param [in] p2      char*   pointer to the second packed object.
 *  @param [in] name    char*   unused. Present to ensure the same function params as for K_cmpeqs()... See also K_cmpobjs[].
 *  @return             int     0 if equal, 1 otherwise.
 *  
 */
 
static int K_cmplg(char* p1, char* p2, char* name)
{
    int l1, l2;

    l1 = * (OSIZE *) p1;
    l2 = * (OSIZE *) p2;

    if(l1 != l2 || memcmp(p1, p2, l1) != 0) 
        return(1);
    else 
        return(0);
}

/**
 *  Compares 2 packed equations. 
 *  
 *  The function compares:
 *      - the compiled LEC form (the text of the LEC expression may differ as long as the compiled versions are equal).
 *      - the estimation method if present
 *      - the estimation Sample if defined
 *      - the estimation block of simultaneous equations
 *      - the metric instruments if any
 *  
 *  @param [in] p1      char*   pointer to the first packed EQ.
 *  @param [in] p2      char*   pointer to the second packed EQ.
 *  @param [in] name    char*   name of the equation.
 *  @return             int     0 if all elements are equal, 1 if at least one is different.
 *  
 *  @note the estimation results are not compared (statistical tests).
 */

static int K_cmpeqs(char* p1, char* p2, char* name)
{
    int rc = 1;     // not equal
    Equation* eq1 = K_eunpack(p1, name);
    Equation* eq2 = K_eunpack(p2, name);

    if(eq1 != NULL && eq2 != NULL) 
        rc = *eq1 == *eq2 ? 0 : 1;
    
    if(eq1) delete eq1;
    if(eq2) delete eq2;
    eq1 = nullptr;
    eq2 = nullptr;
    return rc;
}

/**
 *  Compares 2 packed identities. 
 *  
 *  The function compares the compiled LEC forms. The text of the LEC expression may differ as long as the compiled versions don't.
 *  
 *  @param [in] p1      char*   pointer to the first packed IDT.
 *  @param [in] p2      char*   pointer to the second packed IDT
 *  @param [in] name    char*   name of the identity.
 *  @return             int     0 if identities are equal, 1 otherwise.
 *  
 */

static int K_cmpidt(char* p1, char* p2, char* name)
{
    int     rc = 1;
    char    *i1 = NULL, *i2 = NULL;
    CLEC    *cl1 = NULL, *cl2 = NULL;
    i1 = (char*) P_get_ptr(p1, 0);
    cl1 = L_cc(i1);
    i2 = (char*) P_get_ptr(p2, 0);
    cl2 = L_cc(i2);

    if(cl1 == NULL || cl2 == NULL ||
            cl1->tot_lg != cl2->tot_lg ||
            memcmp(cl1, cl2, cl1->tot_lg) != 0) goto done;

    rc = 0;

done :
    SCR_free(cl1);
    SCR_free(cl2);

    return(rc);
}

int K_cmp_scl(Scalar* scl1, Scalar* scl2)
{
    if(scl1 == NULL || scl2 == NULL) 
        return 1;

    return *scl1 == *scl2;
}

int K_cmp_tbl(Table* tbl1, Table* tbl2)
{
    int rc;
    char* p1 = NULL; 
    char* p2 = NULL;

    if(tbl1 == NULL || tbl2 == NULL) 
        return 1;

    rc = K_tpack(&p1, (char*) tbl1);
    rc = K_tpack(&p2, (char*) tbl2);
    if(p1 == NULL || p2 == NULL || rc)
        return 1;
    
    rc = K_cmplg(p1, p2, NULL);

    SCR_free(p1);
    SCR_free(p2);
    return rc;
}

/**
 *  Compares 2 double values. 
 *  
 *  The values are considered equal if their relative difference is less than K_CMP_EPS.
 *  
 *  @param [in] v1      double   first value
 *  @param [in] v2      double   second value
 *  @return             int         0 if |(v1 - v2)/v1| < K_CMP_EPS, 1 otherwise.
 *  
 */

static int K_cmpvar_1(double v1, double v2)
{
    double  diff;
    if(v1 == v2) return(0);
    if(v1 == 0) {
        if(fabs(v2) < K_CMP_EPS) return(0);
        else return(1);
    }
    diff = fabs((v1 - v2) / v1);
    if(diff < K_CMP_EPS) return(0);
    return(1);
}

int K_cmp_var(VAR var1, VAR var2)
{
    int i;
    int nb = global_ws_var->sample->nb_periods;

    for(i = 0 ; i < nb ; i++)
        if(K_cmpvar_1(var1[i], var2[i])) 
            return(1);

    return(0);
}

/**
 *  Compares 2 variables on the current WS sample. 
 *  
 *  See also K_cmpvar_1().
 *  
 *  @param [in] p1      char*   pointer to the first variable (will be cast to double).
 *  @param [in] p2      char*   pointer to the second variable (will be cast to double).
 *  @param [in] name    char*   unused. Present to ensure the same function signature as K_cmpeqs()... See K_cmpobjs[].
 *  @return             int     0 if the 2 variables are equal, 1 otherwise.
 *  
 */

static int K_cmpvar(char* p1, char* p2, char* name)
{
    double *r1, *r2;
    int     i, nb = global_ws_var->sample->nb_periods;

    r1 = (double*) P_get_ptr(p1, 0);
    r2 = (double*) P_get_ptr(p2, 0);
    for(i = 0 ; i < nb ; i++) {
        if(K_cmpvar_1(r1[i], r2[i])) return(1);
    }

    return(0);
}


/**
 *  Compares IODE objects having the same name in two KDB. 
 *  Function used to compare an IODE file with the content of the current workspace.
 *  
 *  See above for more details on the comparison methods for each object type.
 *  
 *  @param [in] name    char*       object name
 *  @param [in] kdb1    KDB*        first KDB
 *  @param [in] kdb2    KDB*        second KDB
 *  @return             int         0 -> if name neither in kdb1 nor in kdb2
 *                                  1 -> if name in kdb1 but not in kdb2
 *                                  2 -> if name not in kdb1 but in kdb2
 *                                  3 -> if name in both kdb1 and kdb2, IODE object in kdb1 == IODE object in kdb2
 *                                  4 -> if name in both kdb1 and kdb2, IODE object in kdb1 != IODE object in kdb2
 */

int K_cmp(char* name, KDB* kdb1, KDB* kdb2)
{
    int res=0;

    if(kdb1->k_type != kdb2->k_type) 
        return -1;

    SWHDL handle_1 = kdb1->get_handle(name);
    SWHDL handle_2 = kdb2->get_handle(name);

    if(handle_1 == 0) 
    {
        if(handle_2 == 0) 
            return(0);          /* not kdb1 and not kdb2 */
        else 
            return(2);          /* not kdb1 but in kdb2 */
    }

    if(handle_2 == 0) 
        return(1);              /* in kdb1 but not in kdb2 */

    // ---- name in both kdb1 and kdb2 ----
    // res = 0 -> IODE object in kdb1 == IODE object in kdb2
    // res = 1 -> IODE object in kdb1 != IODE object in kdb2
    switch(kdb1->k_type)
    {
    case COMMENTS:
        res = K_cmplg(SW_getptr(handle_1), SW_getptr(handle_2), name);
        break;
    case EQUATIONS:
        res = K_cmpeqs(SW_getptr(handle_1), SW_getptr(handle_2), name);
        break;
    case IDENTITIES:
        res = K_cmpidt(SW_getptr(handle_1), SW_getptr(handle_2), name);
        break;
    case LISTS:
        res = K_cmplg(SW_getptr(handle_1), SW_getptr(handle_2), name);
        break;
    case SCALARS:
        res = K_cmplg(SW_getptr(handle_1), SW_getptr(handle_2), name);
        break;
    case TABLES:
        res = K_cmplg(SW_getptr(handle_1), SW_getptr(handle_2), name);
        break;
    case VARIABLES:
        res = K_cmpvar(SW_getptr(handle_1), SW_getptr(handle_2), name);
        break;
    default:
        break;
    }

    // 3 -> if name in both kdb1 and kdb2, IODE object in kdb1 == IODE object in kdb2
    // 4 -> if name in both kdb1 and kdb2, IODE object in kdb1 != IODE object in kdb2
    return 3 + res;
}

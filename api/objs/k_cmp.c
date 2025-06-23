/**
 * @header4iode
 *
 * Function to compare 2 IODE objects.
 * -----------------------------------
 *
 *      int K_cmp(char* name, KDB* kdb1, KDB* kdb2) : compare object named name in 2 kdb.
 */ 
 
#include "iode.h"

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

    if(l1 != l2 || memcmp(p1, p2, l1) != 0) return(1);
    else return(0);
}

int K_cmp_eqs(EQ* eq1, EQ* eq2, char* name)
{
    int     rc = 1;
    CLEC    *cl1 = NULL, *cl2 = NULL;

    if(eq1 == NULL || eq2 == NULL) goto done;

    /* same unlinked CLEC, method, sample, cmt, instr, blk, --plus tests -- */
    cl1 = L_solve(eq1->lec, name);
    cl2 = L_solve(eq2->lec, name);

    if(cl1 == NULL || cl2 == NULL
            || cl1->tot_lg != cl2->tot_lg
            || memcmp(cl1, cl2, cl1->tot_lg) != 0) goto done;

    if(eq1->method != eq2->method) goto done;
    if(memcmp(&(eq1->smpl), &(eq2->smpl), sizeof(SAMPLE)) != 0) goto done;
    if(strcmp(eq1->blk, eq2->blk) != 0) goto done;
    if(strcmp(eq1->instr, eq2->instr) != 0) goto done;

//    if(memcmp(eq1->tests, eq2->tests, EQS_NBTESTS * sizeof(float)) != 0) goto done; /* FLOAT 12-04-98 */
//        test 0 and 5 and 11..20 are not stored in ascii
    rc = 0;

done :
    SCR_free(cl1);
    SCR_free(cl2);

    return(rc);
}

/**
 *  Compares 2 packed equations. 
 *  
 *  The function compares:
 *      - the compiled LEC form (the text of the LEC expression may differ as long as the compiled versions are equal).
 *      - the estimation method if present
 *      - the estimation SAMPLE if defined
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
    int     rc;
    EQ      *eq1 = NULL, *eq2 = NULL;

    eq1 = K_eunpack(p1, name);
    eq2 = K_eunpack(p2, name);

    rc = K_cmp_eqs(eq1, eq2, name);

done :
    E_free(eq1);
    E_free(eq2);

    return(rc);
}

int K_cmp_idt(IDT* idt1, IDT* idt2)
{
    int     rc = 1;
    CLEC    *cl1 = NULL, *cl2 = NULL;

    cl1 = L_cc(idt1->lec);
    cl2 = L_cc(idt2->lec);

    if(cl1 == NULL || cl2 == NULL ||
            cl1->tot_lg != cl2->tot_lg ||
            memcmp(cl1, cl2, cl1->tot_lg) != 0) goto done;

    rc = 0;

done :
    SCR_free(cl1);
    SCR_free(cl2);

    return(rc);
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
    i1 = P_get_ptr(p1, 0);
    cl1 = L_cc(i1);
    i2 = P_get_ptr(p2, 0);
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

int K_cmp_scl(SCL* scl1, SCL* scl2)
{
    int rc;
    char* p1 = NULL; 
    char* p2 = NULL;

    if(scl1 == NULL || scl2 == NULL) 
        return 1;

    rc = K_spack(&p1, (char*) scl1);
    rc = K_spack(&p2, (char*) scl2);
    if(p1 == NULL || p2 == NULL || rc)
        return 1;
        
    rc = K_cmplg(p1, p2, NULL);

    SCR_free(p1);
    SCR_free(p2);
    return rc;
}

int K_cmp_tbl(TBL* tbl1, TBL* tbl2)
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


// Threshold for VAR comparisons 
double K_CMP_EPS = 1e-7;

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
    int nb = KSMPL(KV_WS)->s_nb;

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
    int     i, nb = KSMPL(KV_WS)->s_nb;


    r1 = P_get_ptr(p1, 0);
    r2 = P_get_ptr(p2, 0);
    for(i = 0 ; i < nb ; i++) {
        if(K_cmpvar_1(r1[i], r2[i])) return(1);
    }

    return(0);
}

/**
 *  Table of function pointers for IODE objects comparison: CMT, EQ, IDT, LST, SCL, TBL, VAR.
 */
static int (*K_cmpobj[])() = {
    K_cmplg,
    K_cmpeqs,
    K_cmpidt,
    K_cmplg,
    K_cmplg,
    K_cmplg,
    K_cmpvar
};


/*
    compares object name from 2 different kdb's
    returns 0 : if name not in 1, not in 2
	    1 ; if name in 1, not in 2
	    2 ; if name not in 1, in 2
	    3 ; if name in 1, in 2 and 1 = 2
	    4 ; if name in 1, in 2 and 1 != 2
*/

/**
 *  Compares IODE objects having the same name in two KDB. 
 *  Function used to compare an IODE file with the content of the current workspace.
 *  
 *  See above for more details on the comparison methods for each object type.
 *  
 *  @param [in] name    char*       object name
 *  @param [in] kdb1    KDB*        first KDB
 *  @param [in] kdb2    KDB*        second KDB
 *  @return             int         0 if objects are equal, 1 if not.
 *  
 */

int K_cmp(char* name, KDB* kdb1, KDB* kdb2)
{
    int     p1, p2;

    if(KTYPE(kdb1) != KTYPE(kdb2)) return(-1);

    p1 = K_find(kdb1, name);
    p2 = K_find(kdb2, name);

    if(p1 < 0) {
        if(p2 < 0) return(0);   /* not 1, not 2 */
        else return(2);         /* not 1, 2 */
    }

    if(p2 < 0) return(1);      /* 1, not 2 */

    /* 1, 2 */
    return(3 +
           K_cmpobj[KTYPE(kdb1)](KGOVAL(kdb1, p1), KGOVAL(kdb2, p2), name));
}

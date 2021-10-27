#include "iode.h"

K_cmplg(p1, p2, name)
char    *p1, *p2, *name;
{
    int l1, l2;

    l1 = * (OSIZE *) p1;
    l2 = * (OSIZE *) p2;

    if(l1 != l2 || memcmp(p1, p2, l1) != 0) return(1);
    else return(0);
}

K_cmpeqs(p1, p2, name)
char    *p1, *p2, *name;
{
    int     rc = 1;
    EQ      *eq1 = NULL, *eq2 = NULL;
    CLEC    *cl1 = NULL, *cl2 = NULL;

    eq1 = K_eunpack(p1);
    eq2 = K_eunpack(p2);

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
    E_free(eq1);
    E_free(eq2);
    SCR_free(cl1);
    SCR_free(cl2);

    return(rc);
}

K_cmpidt(p1, p2, name)
char    *p1, *p2, *name;
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

// JMP 26/3/2012 pour améliorer la comparaison (à eps près)

double K_CMP_EPS = 1e-7;

K_cmpvar_1(v1, v2) IODE_REAL    v1, v2;
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

K_cmpvar(p1, p2, name)
char    *p1, *p2;
char    *name;
{
    IODE_REAL *r1, *r2;
    int     i, nb = KSMPL(KV_WS)->s_nb;


    r1 = P_get_ptr(p1, 0);
    r2 = P_get_ptr(p2, 0);
    for(i = 0 ; i < nb ; i++) {
        if(K_cmpvar_1(r1[i], r2[i])) return(1);
    }

    return(0);
}

/* if p1 == p2 return 0 else 1 */
int (*K_cmpobj[])() = {
    K_cmplg,
    K_cmpeqs,
    K_cmpidt,
    K_cmplg,
    K_cmplg,
    K_cmplg,
//    K_cmplg
    K_cmpvar   // JMP 26/3/2012
};


/*
    compares object name from 2 different kdb's
    returns 0 : if name not in 1, not in 2
	    1 ; if name in 1, not in 2
	    2 ; if name not in 1, in 2
	    3 ; if name in 1, in 2 and 1 = 2
	    4 ; if name in 1, in 2 and 1 != 2
*/

int K_cmp(char* name, KDB* kdb1, KDB* kdb2)
{
    int     p1, p2, l1, l2;
    char    *pck1, *pck2;

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



#include "iode.h"

void E_free(eq)
EQ  *eq;
{
    if(eq == NULL) return;

    SW_nfree(eq->lec);
    SW_nfree(eq->clec);
    SW_nfree(eq->cmt);
    SW_nfree(eq->blk);
    SW_nfree(eq->instr);

    SW_nfree(eq);
}


int E_split_eq(lec, lhs, rhs)
char    *lec, **lhs, **rhs;
{
    int     pos = -1;

    *lhs = NULL;
    *rhs = NULL;

    if(lec == NULL) goto done;
    pos = L_split_eq(lec);
    if(pos < 0) goto done;

    lec[pos] = 0;
    *lhs = SCR_stracpy(lec);
    *rhs = SCR_stracpy(lec + pos + 2);

done :
    return(pos);
}


int E_dynadj(method, lec, c1, c2, adjlec)
int     method;
char    *lec, *c1, *c2, **adjlec;
{
    int     lg, rc = -1;
    char    *lhs = NULL, *rhs = NULL;

    *adjlec = NULL;
    if(E_split_eq(lec, &lhs, &rhs) < 0) goto done;
    SCR_strip(lhs);
    SCR_strip(rhs);

    if(method == 0) {
	if(c1 == NULL || c1[0] == 0) goto done;

	lg = 2 * strlen(lhs) + strlen(rhs) + 100;
	*adjlec = SCR_malloc(lg);
	sprintf(*adjlec,
	    "d(%s) := %s * (%s -(%s)[-1])",
	    lhs, c1, rhs, lhs);
    }
    else {
	if(c1 == NULL || c1[0] == 0
	    || c2 == NULL || c2[0] == 0) goto done;

	lg = 2 * strlen(lhs) + 2 * strlen(rhs) + 100;
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


E_DynamicAdjustment(method, eqs, c1, c2)
int     method;
char    **eqs, *c1, *c2;
{
    char    *ae;

    SCR_strip(*eqs);
    SCR_strip(c1);
    SCR_strip(c2);

    E_dynadj(method, *eqs, c1, c2, &ae);

    SCR_strfacpy(eqs, ae);
    SCR_free(ae);

    return(0);
}



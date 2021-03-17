#include "iode.h"

/*
    Link CLEC struct with respect to variables and scalar data bases.

    Input : 2 pointers which are passed to the user defined functions
    L_findvar and L_findscl. These functions have the following syntax :

    int L_findvar(dbv, name) : returns the position of the variable
	    name in dbv. This value will be used by L_getvar at
	    execution time.

    int L_findscl(dbs, name) : returns the position of the scalar name
	    in dbs. This value will be used by L_getscl at
	    execution time.

    smpl is a pointer to the sample of dbv, the database of variables

    cl is a pointer to a CLEC structure.

    As output, the structure CLEC is modified, but remains usable
    for a future link with other databases, or another sample.

    L_link returns 0 if no problem occurs. It set L_errno and returns
    L_errno otherwise.
*/

L_link(dbv, dbs, cl)
KDB   *dbv, *dbs;
CLEC    *cl;
{
    if(cl == 0) return(0);
    if(L_link1(dbv, dbs, cl)) return(L_errno);
    L_link2(dbv, cl);
    return(0);
}

/*NH*/
/* ASSIGN VARIABLES NB TO EACH VAR IN EXPRESSION */

L_link1(dbv, dbs, cl)
KDB   *dbv, *dbs;
CLEC    *cl;
{
    int     i;

    for(i = 0 ; i < cl->nb_names ; i++) {
	if(L_ISCOEF(cl->lnames[i].name))
	    cl->lnames[i].pos = L_findscl(dbs, cl->lnames[i].name);
	else
	    cl->lnames[i].pos = L_findvar(dbv, cl->lnames[i].name);
	if(cl->lnames[i].pos < 0) {
	    B_seterror("%s : not in WS", cl->lnames[i].name);
	    return(L_errno = L_NOT_FOUND_ERR);
	}
    }
    return(0);
}

/*NH*/
/* COMPUTE DISPLACEMENT FOR EACH VAR IN EXPRESSION AND SET INTO EXPR */

L_link2(dbv, expr)
KDB     *dbv;
CLEC    *expr;
{
    int     pos;

    if(expr == 0) return(0);
    pos = sizeof(CLEC) + (expr->nb_names - 1) * sizeof(LNAME);
    L_link_sub(dbv, (char *)expr + pos, expr->tot_lg - pos);
    return(0);
}

/*NH*/
L_link_sub(dbv, expr, lg)
KDB     *dbv;
char    *expr;
short   lg;
{
    int     j, keyw;
    short   len, ref, s;
    CVAR    cvar;
    SAMPLE  *smpl;
    PERIOD  per;

    smpl = L_getsmpl(dbv);
    for(j = 0 ; j < lg ; ) {
	keyw = expr[j++];
	switch(keyw) {
	    case L_VAR   :
		memcpy(&cvar, expr + j, sizeof(CVAR));
		cvar.ref = cvar.lag;
		if(cvar.per.p_s != 0)
		    cvar.ref += PER_diff_per(&(cvar.per), &(smpl->s_p1));
		memcpy(expr + j, &cvar, sizeof(CVAR));
		j += sizeof(CVAR);
		break;
	    case L_COEF      : j += sizeof(CVAR); break;
	    case L_DCONST    : j += s_dbl;   break;
	    case L_LCONST    : j += s_long;  break;
	    case L_PERIOD    :
		memcpy(&per, expr + j, sizeof(PERIOD));
		s = PER_diff_per(&per, &(smpl->s_p1));
		memcpy(expr + j + sizeof(PERIOD), &s, sizeof(short));
/*                *(short *)(expr + j + sizeof(PERIOD)) =
		    PER_diff_per((PERIOD *)(expr + j), &(smpl->s_p1));
*/
		j += s_short + sizeof(PERIOD);
		break;
	    default :
		if(is_fn(keyw)) {j++; break;}
		if(is_tfn(keyw)) {
		    memcpy(&len, expr + j + 1, sizeof(short));
/*                    len = *(short *)(expr + j + 1); */
		    L_link_sub(dbv, expr + j + 1 + s_short, len);
		    j += 1 + s_short + len;
		    break;
		    }
		break;
	    }
	}
    return(0);
}

/**************************************************************************
Pseudo link. Utilisé pour calculer les SCC.
Les db de var et de scl ne sont pas nécessaires.
Chaque variable endo reçoit le numero de l'équation
JMP 16/3/2012
**************************************************************************/

L_link_endos(KDB *dbe, CLEC *cl)
{
    if(cl == 0) return(0);
    if(L_link1_endos(dbe, cl)) return(L_errno);
//    L_link2(dbv, cl);
    return(0);
}

/*NH*/
/* ASSIGN VARIABLES NB TO EACH VAR IN EXPRESSION */

L_link1_endos(KDB *dbe, CLEC *cl)
{
    int     i;

    for(i = 0 ; i < cl->nb_names ; i++) {
		if(L_ISCOEF(cl->lnames[i].name))
			cl->lnames[i].pos = 0; // On s'en tape pour le calcul des SCC
		else
			cl->lnames[i].pos = K_find(dbe, cl->lnames[i].name);
			
		if(cl->lnames[i].pos < 0) // Non trouvé -> exo
			cl->lnames[i].pos = KNB(dbe); // exo : on s'en tape aussi
	}
    return(0);
}


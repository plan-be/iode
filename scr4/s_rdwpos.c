#include "scr.h"

typedef struct _wpos_ {
    short   wp_obj;
    short   wp_fld;
    short   wp_pos[4];
    U_ch    wp_rpos[4];
} WPOS;

int     SCR_WPOS_NB = 0;
int     SCR_WPOS_MAX = 0;
WPOS    *SCR_WPOS = 0;

SCR_WposReadAdd(iObj, iFld, v)
int     iObj, iFld, v;
{
    int     op = 0, type, x = 0;
ag:
    type = SCR_yylex();
    switch(type) {
	case YY_LONG  : x = YYLONG; if(op == 0) op = 1; break;
	case SCR_PLUS : op = 2; goto ag;
	case SCR_MINUS: op = 3; goto ag;
	default:
		SCR_yyerror(ERR_ERROR, ERR_SYNTAX, "(%s)", YYTEXT);
		return(0);
	}

    SCR_WposAddV(iObj, iFld, v, x, op);
    return(0);
}

SCR_WposAddObj(iObj, iFld)      /* JMP 25-01-97 */
int     iObj, iFld;
{
    int     i;

    if(SCR_WPOS_MAX <= SCR_WPOS_NB) {
	SCR_WPOS = (WPOS *)SCR_realloc(SCR_WPOS, sizeof(WPOS), SCR_WPOS_MAX, SCR_WPOS_MAX + 100);
	SCR_WPOS_MAX += 100;
	}

    if(SCR_WPOS == 0) return(-1);
    SCR_WPOS[SCR_WPOS_NB].wp_obj = iObj;
    SCR_WPOS[SCR_WPOS_NB].wp_fld = iFld;
    SCR_WPOS_NB++;
    return(SCR_WPOS_NB - 1);
}

SCR_WposFindObj(iObj, iFld)
int     iObj, iFld;
{
    int     i;

    if(SCR_WPOS == 0) return(-1);
    for(i = 0 ; i < SCR_WPOS_NB ; i++)
	if(SCR_WPOS[i].wp_obj == iObj && SCR_WPOS[i].wp_fld == iFld)
	    return(i);

    return(-1);
}

SCR_WposAddV(iObj, iFld, v, x, r)
int     iObj, iFld, v, x, r;
{
    int     n = SCR_WposFindObj(iObj, iFld);

    if(n < 0) n = SCR_WposAddObj(iObj, iFld);
    SCR_WPOS[n].wp_pos[v] = x;
    SCR_WPOS[n].wp_rpos[v] = r;
    return(0);
}

SCR_WposSetAll(iObj, iFld, x, y, w, h)
int     iObj, iFld, *x, *y, *w, *h;
{
    int     n = SCR_WposFindObj(iObj, iFld);
    int     r;

    if(n < 0) return(0);

    SCR_WposSet1(n, x, 0);
    SCR_WposSet1(n, y, 1);
    SCR_WposSet1(n, w, 2);
    SCR_WposSet1(n, h, 3);
    return(0);
}

SCR_WposSet1(n, x, v)
int     n, *x, v;
{
    int    r = SCR_WPOS[n].wp_rpos[v];

    switch(r) {
	case 1 : *x = SCR_WPOS[n].wp_pos[v]; break;
	case 2 : *x += SCR_WPOS[n].wp_pos[v]; break;
	case 3 : *x -= SCR_WPOS[n].wp_pos[v]; break;
	}
    return(0);
}










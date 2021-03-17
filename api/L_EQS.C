#include "iode.h"

#define EXPR        sl->sl_expr
#define LG          sl->sl_lg
#define MBR         sl->sl_mbr
#define NMBR        sl->sl_nmbr
#define ENDO        sl->sl_endo
#define OP          sl->sl_op
#define NARGS       sl->sl_nargs
#define APP(x)      L_append(sl, x)
#define FRT(x)      L_front(sl, x)
#define APP_OP(x)   L_append_op(x)
#define APP_FN(x,y) L_append_fn(x, y)
#define APP_VAL(y)  L_append_const(y)

/*NH*/
/*
    Compiles an equation and alloc ALEC struct
    return NULL if error (L_errno).
    nb_names = 0 for first call, L_NB_NAMES for second member
*/

ALEC *L_cc1_alloc(lec, nb_names)
char    *lec;
int     nb_names;
{
    ALEC    *alec = 0;

    if(L_open_string(lec)) return(alec);
    if(L_cc1(nb_names) != 0) return(alec);
    L_close();
    alec = (ALEC *)L_malloc(L_NB_EXPR * sizeof(ALEC));
    memcpy(alec, L_EXPR, L_NB_EXPR * sizeof(ALEC));
    return(alec);
}

/*NH*/
L_cc1_eq(sl, eq)
SLEC    *sl;
char    *eq;
{
    int     pos;

    /* SLIT EQ AND COMPILE EACH MEMBER */
    pos = L_split_eq(eq);
    if(pos < 0) return(L_errno = L_ASSIGN_ERR);
    eq[pos] = 0;
    EXPR[0] = L_cc1_alloc(eq, 0);
    LG[0]   = L_NB_EXPR - 1;
    L_alloc_expr(-1);
    eq[pos] = ':';
    if(EXPR[0] == 0) return(L_errno);
    EXPR[1] = L_cc1_alloc(eq + pos + 2, L_NB_NAMES);
    LG[1]   = L_NB_EXPR - 1;
    L_alloc_expr(-1);
    if(EXPR[1] == 0) {
	L_free(EXPR[0]);
	return(L_errno);
	}
    return(0);
}

/*
    Solves  an  equation  with  respect  to  endo  if  it is
    possible.  If  endo  is present more than once or in non
    invertible operator, returns equation of the form F(x) =
    0, and L_errno is set to L_DUP_ERR or L_INVERT_ERR.
*/

CLEC *L_solve(eq, endo)
char    *eq, *endo;
{
    CLEC    *clec = 0;
    int     dupendo = 0;

    L_invert(eq, endo, &dupendo);
    switch(L_errno) {
	case 0              : clec = L_cc2(L_EXPR);
			      if(clec != 0) clec->dupendo = dupendo;
			      break;

	case L_DUP_ERR      :
	case L_INVERT_ERR   :
	default             : break;
	}
    L_alloc_expr(-1);
    return(clec);
}

/*NH*/
/* Solve equation with respect to endo. Output is in
    L_EXPR (type ALEC[]) and L_NB_EXPR.
    If L_errno == L_DUP_ERR : duplicate endo in equation (cannot be solved)
		  L_INVERT_ERR : cannot invert operator (ex "(X<t)*2 := Y")
		  L_ASSIGN_ERR : sign := not found
    Otherwise : lec error.
*/

L_invert(eq, endo, dupendo)
char    *eq, *endo;
int     *dupendo;
{
    SLEC    slec, *sl = &slec;
    ALEC    *expr;
    int     pos, count0, count1;

    if(L_cc1_eq(sl, eq)) return(L_errno);

    /* FIND MEMBER CONTAINING ENDO AND SET INFO IN sl */
    *dupendo = 0;
    ENDO = endo;
    count0 = L_count_endo(EXPR[0], LG[0], ENDO);
    count1 = L_count_endo(EXPR[1], LG[1], ENDO);
    if(count0 + count1 == 0) {
	     L_free(EXPR[0]); L_free(EXPR[1]);
	     return(L_errno = L_DUP_ERR);
	     }
    if(count0 + count1 >= 2) {
	     *dupendo = 1;
	     L_NB_EXPR = 0;
	     L_append(sl, 0);
	     L_append(sl, 1);
	     APP_OP(L_MINUS);
	     APP_OP(L_EOE);
	     L_free(EXPR[0]); L_free(EXPR[1]);
	     return(0);
	     }
    if(count0 == 0) {MBR = 1; NMBR = 0;}
    else            {MBR = 0; NMBR = 1;}
    L_NB_EXPR = 0;
    L_append(sl, NMBR);
    L_free(EXPR[NMBR]);
    expr = EXPR[MBR];

    /* LOOP ON OPERATORS */
    while(L_split_expr(sl) == 0)
	switch(OP) {
	    case L_PLUS   : APP(NMBR); APP_OP(L_MINUS); break;
	    case L_TIMES  : APP(NMBR); APP_OP(L_DIVIDE); break;
	    case L_MINUS  : if(MBR == 1) {FRT(NMBR); APP_OP(L_MINUS);}
			    else {APP(NMBR); APP_OP(L_PLUS);}
			    break;
	    case L_DIVIDE : if(MBR == 1) {FRT(NMBR); APP_OP(L_DIVIDE);}
			    else {APP(NMBR); APP_OP(L_TIMES);}
			    break;
	    case L_LN     : APP_FN(L_EXPN, 1); break;
	    case L_EXPN   : APP_FN(L_LN, 1); break;
	    case L_UPLUS  : break;
	    case L_UMINUS : APP_FN(L_UMINUS, 1); break;
	    case L_COS    : APP_FN(L_ACOS, 1); break;
	    case L_ACOS   : APP_FN(L_COS, 1); break;
	    case L_SIN    : APP_FN(L_ASIN, 1); break;
	    case L_ASIN   : APP_FN(L_SIN, 1); break;
	    case L_TAN    : APP_FN(L_ATAN, 1); break;
	    case L_ATAN   : APP_FN(L_TAN, 1); break;
	    case L_SQRT   : APP_VAL(2); APP_FN(L_EXP, 2); break;
	    case L_EXP    : if(MBR == 0) {
				APP_FN(L_LN, 1); APP(NMBR); APP_OP(L_DIVIDE);
				APP_FN(L_EXPN, 1);
				}
			    else {
				FRT(NMBR); APP_FN(L_LOG, 2);
				}
			    break;
	    /*
	    case L_LOG    : if(MBR == 0) {
				}
			    else {
				FRT(NMBR); APP_OP(L_EXP);
				}
			    break;
	    /* JMP 22-06-00 */
	    case L_LOG    : APP_FN(L_EXP, 1); break; /* JMP 22-06-00 */
	    case L_DIFF   : if(NARGS == 2) APP(NMBR);
			    APP(MBR); APP_FN(L_LAG, NARGS); APP_OP(L_PLUS);
			    break;
	    case L_DLN    : if(NARGS == 2) APP(NMBR);
			    APP(MBR); APP_FN(L_LAG, NARGS);
			    APP_FN(L_LN, 1); APP_OP(L_PLUS);
			    APP_FN(L_EXPN, 1);
			    break;
	    case L_GRT    : APP_VAL(100); APP_OP(L_DIVIDE); APP_VAL(1);
			    APP_OP(L_PLUS);
			    if(NARGS == 2) APP(NMBR);
			    APP(MBR); APP_FN(L_LAG, NARGS); APP_OP(L_TIMES);
			    break;
	    case L_RAPP   : if(NARGS == 2) APP(NMBR);
			    APP(MBR); APP_FN(L_LAG, NARGS); APP_OP(L_TIMES);
			    break;
	    default :       L_free(expr); return(L_errno = L_INVERT_ERR);
	    }

    APP_OP(L_EOE);
    L_free(expr);
    return(L_errno);
}

/*
    Returns position of := in equation. If not present, returns -1.
*/
L_split_eq(eq)
char    *eq;
{
    int i;

    for(i = 0 ; eq[i] != 0 ; i++)
	if(eq[i] == ':' && eq[i + 1] == '=') return(i);

    return(-1);
}

/*NH*/
L_count_endo(al, lg, endo)
ALEC    *al;
int     lg;
char    *endo;
{
    int     count, i;

    for(count = 0, i = 0; i < lg; i++, al++)
	if( al->al_type == L_VAR &&
	    al->al_val.v_var.per.p_s == 0 &&
	    al->al_val.v_var.lag == 0 &&
	    strcmp(L_NAMES[al->al_val.v_var.pos], endo) == 0)
	  count++;

    return(count);
}

/*NH*/
L_split_expr(sl)
SLEC    *sl;
{
    ALEC    *al;
    int     lg, pos;

    al = EXPR[MBR];
    lg = LG[MBR] - 1;
ag:
    OP = al[lg].al_type;
    NARGS = al[lg].al_val.v_nb_args;
    switch(OP) {
	case L_VAR : return(1);
	case L_CLOSEP : al++ ; lg -= 2; goto ag;
	}
    pos = L_sub_expr(al, lg - 1);
    if(pos < 0) return(L_errno = L_DUP_ERR);
    EXPR[0] = al;
    LG[0] = pos;
    EXPR[1] = al + pos;
    LG[1] = lg - pos;
    if(L_count_endo(al, pos, ENDO) > 0) {
	MBR = 0 ; NMBR = 1;
	}
    else {
	MBR = 1 ; NMBR = 0;
	}
    return(0);
}

/*NH*/
void L_append(sl, mbr)
SLEC    *sl;
int     mbr;
{
    L_alloc_expr(L_NB_EXPR + LG[mbr]);
    memcpy(L_EXPR + L_NB_EXPR, EXPR[mbr], sizeof(ALEC) * LG[mbr]);
    L_NB_EXPR += LG[mbr];
}

/*NH*/
void L_front(sl, mbr)
SLEC    *sl;
int     mbr;
{

    L_alloc_expr(L_NB_EXPR + LG[mbr]);
    L_move_arg(L_EXPR + LG[mbr], L_EXPR, sizeof(ALEC) * L_NB_EXPR);
    memcpy(L_EXPR, EXPR[mbr], sizeof(ALEC) * LG[mbr]);
    L_NB_EXPR += LG[mbr];
}

/*NH*/
void L_append_op(op)
int     op;
{
    L_alloc_expr(L_NB_EXPR + 1);
    L_EXPR[L_NB_EXPR].al_type = op;
    L_NB_EXPR++;
}

/*NH*/
void L_append_fn(op, nargs)
int     op, nargs;
{
    L_alloc_expr(L_NB_EXPR + 1);
    L_EXPR[L_NB_EXPR].al_type = op;
    L_EXPR[L_NB_EXPR].al_val.v_nb_args = nargs;
    L_NB_EXPR++;
}

/*NH*/
void L_append_const(a)
int     a;
{
    L_alloc_expr(L_NB_EXPR + 1);
    L_EXPR[L_NB_EXPR].al_type = L_LCONST;
    L_EXPR[L_NB_EXPR].al_val.v_long = a;
    L_NB_EXPR++;
}


#include "iode.h"

LSTACK  L_OPS[L_MAX_STACK];
int     L_NB_OPS = 0,
	L_PAR = 0;
ALEC    *L_EXPR = 0;
char    **L_NAMES = 0;
int     L_NB_EXPR = 0, L_NB_AEXPR, L_NB_NAMES = 0, L_NB_ANAMES = 0;

L_alloc_expr(nb)
int     nb;
{
    if(nb <= 0) {
	SW_nfree(L_EXPR);
	L_NB_AEXPR = L_NB_EXPR = 0;
	L_EXPR = 0;
    }
    if(L_NB_AEXPR >= nb) return(0);
    nb = 100 * (1 + nb / 100);
    L_EXPR = (ALEC *) SW_nrealloc(L_EXPR,
				  sizeof(ALEC) * L_NB_AEXPR,
				  sizeof(ALEC) * nb);
    L_NB_AEXPR = nb;
    return(0);
}

/*NH*/
/*
    Compiles an open stream.
*/

CLEC *L_cc_stream()
{
    CLEC    *cl;

    if(L_cc1(0) != 0)
	return((CLEC *)0);
    cl = L_cc2(L_EXPR);
    L_alloc_expr(-1);
    return(cl);
}

/*
    Compiles a string. Return an allocated CLEC struct pointer.
*/

CLEC *L_cc(lec)
char    *lec;
{
    CLEC    *clec = 0;

    if(L_open_string(lec)) return(clec);
    clec = L_cc_stream();
    L_close();
    return(clec);
}

/*
    Check the syntax of a string.
*/
/*
L_check(lec)
char    *lec;
{
    if(L_open_string(lec)) return(-1);
    L_cc1(0);
    L_alloc_expr(-1);
    L_close();
    return(L_errno);
}
*/
/*NH*/
/*
    Analyse an expression. Return error code
    nb_names is the number of names already
    known in L_NAMES (generally 0)
*/

L_cc1(nb_names)
{
    int     keyw,
	    start = 1,
	    beg = 1;    /* indicate if next token is an oper or an expr */

    L_NB_OPS = L_PAR = L_errno = L_NB_EXPR = L_NB_AEXPR = 0;
    L_NB_NAMES = nb_names;
    L_alloc_expr(1);

    /* LOOP ON TOKEN */
    while(1) {
	keyw = L_get_token();
	if(L_errno) goto ended;
again:
	switch(keyw) {
	case L_PERIOD:
	case L_LCONST :
	case L_DCONST :
	case L_VAR :
	case L_VAL :
	case L_COEF :
	    if(beg == 0) goto err;
	    if(L_save_var()) goto ended;
	    beg = 0;
	    break;
	case L_OP :
	    if(beg != 0) {
		switch(L_TOKEN.tk_def) {
		case L_MINUS :
		    L_TOKEN.tk_def = L_UMINUS;
		    break;
		case L_PLUS  :
		    L_TOKEN.tk_def = L_UPLUS;
		    break;
		default      :
		    goto err;
		}
		keyw = L_FN;
		goto again;
	    }
	    beg = 1;
	    if(L_add_stack(keyw)) goto ended;
	    break;
	case L_FN :
	case L_TFN:
	case L_MTFN:
	case L_OPENP :
	    if(beg == 0) goto err;
	    if(L_add_stack(keyw)) goto ended;
	    break;
	case L_OCPAR :
	    if(beg == 0) goto err;
	    if(L_add_stack(keyw)) goto ended;
	    beg = 0;
	    break;
	case L_CLOSEP :
	    if(beg == 1) goto err;
	    if(L_add_stack(keyw)) goto ended;
	    break;
	case L_COMMA :
	    if(beg == 1) goto err;
	    if(L_add_stack(keyw)) goto ended;
	    beg = 1;
	    break;
	case YY_EOF:
	case L_EOE :
	    if(start) {
		L_alloc_expr(1);
		L_EXPR[0].al_type = L_EOE;
		L_NB_EXPR++;
		goto ended;
	    }
	    if(beg == 1) goto err;
	    if(L_PAR != 0) L_errno = L_PAR_ERR;
	    else L_clear_stack();
	    goto ended;
	case L_OPENB:
	    if(beg == 1) goto err;
	    if(L_anal_lag()) goto ended;
	    beg = 0;
	    break;
	case YY_ERROR :
	    goto ended;
	default :
	    goto err;
	}
	start = 0;
    }

err :
    L_errno = L_SYNTAX_ERR;
ended:
    return(L_errno);
}
/*NH*/
L_add_new_series(name)
char    *name;
{
    int     i, j;

    for(i = 0 ; i < L_NB_NAMES ; i++)
	if(strcmp(name, L_NAMES[i]) == 0) return(i);

    if(L_NB_NAMES >= L_NB_ANAMES) {
	L_NAMES = (char **) SCR_realloc(L_NAMES, sizeof(char *),
					L_NB_ANAMES, L_NB_ANAMES + 10);
	for(j = 0 ; j < 10 ; j++)
	    L_NAMES[L_NB_ANAMES + j] = SCR_malloc(L_MAX_NAME + 1);

	L_NB_ANAMES += 10;
    }

    strcpy(L_NAMES[i], name);
    L_NB_NAMES++;
    return(i);
}

/*NH*/
L_free_anames()
{
    int     j;


    for(j = 0 ; j < L_NB_ANAMES ; j++)
	SCR_free(L_NAMES[j]);

    SCR_free(L_NAMES);
    L_NB_ANAMES = 0;
    L_NAMES = 0;
}

/*NH*/
L_save_var()
{
    ALEC    *al;

    L_alloc_expr(L_NB_EXPR + 1);
    al = L_EXPR + L_NB_EXPR;
    al->al_type = L_TOKEN.tk_def;
    switch(L_TOKEN.tk_def) {
    case L_PERIOD :
	al->al_val.v_per.p_y = L_TOKEN.tk_period.p_y;
	al->al_val.v_per.p_p = L_TOKEN.tk_period.p_p;
	al->al_val.v_per.p_s = L_TOKEN.tk_period.p_s;
	break;
    case L_DCONST:
	al->al_val.v_real = L_TOKEN.tk_real;
	break;
    case L_LCONST:
	al->al_val.v_long = L_TOKEN.tk_long;
	break;
    default :
	if(is_val(L_TOKEN.tk_def)) break;
	al->al_val.v_var.pos = L_add_new_series(L_TOKEN.tk_name);
	al->al_val.v_var.lag  = 0;
	al->al_val.v_var.per.p_y = 0;
	al->al_val.v_var.per.p_p = 0;
	al->al_val.v_var.per.p_s = 0;
	break;
    }

    L_NB_EXPR++;
    return(0);
}
/*NH*/
L_save_op()
{
    int     op, pos;
    ALEC    *al;

    L_alloc_expr(L_NB_EXPR + 1);
    al = L_EXPR + L_NB_EXPR;

    op = last_op;
    if(is_fn(op)) {
	pos = op - L_FN;
	if(L_MAX_FARGS[pos] < last_ls.ls_nb_args ||
		L_MIN_FARGS[pos] > last_ls.ls_nb_args)
	    return(L_errno = L_ARGS_ERR);
    }
    if(is_tfn(op)) {
	pos = op - L_TFN;
	if(L_MAX_TARGS[pos] < last_ls.ls_nb_args ||
		L_MIN_TARGS[pos] > last_ls.ls_nb_args)
	    return(L_errno = L_ARGS_ERR);
    }
    if(is_mtfn(op)) {
	pos = op - L_MTFN;
	if(L_MAX_MTARGS[pos] < last_ls.ls_nb_args ||
		L_MIN_MTARGS[pos] > last_ls.ls_nb_args)
	    return(L_errno = L_ARGS_ERR);
    }
    al->al_type = op;
    al->al_val.v_nb_args = last_ls.ls_nb_args;
    L_NB_EXPR ++;
    L_NB_OPS--;
    return(0);
}

/*NH*/
L_priority_sup(op)
int op;
{
    if(L_NB_OPS <= 0) return(0);
    if(last_op == L_OPENP) return(0);
    if(!is_op(last_op)) return(1);
    if(L_PRIOR[op - L_OP] <= L_PRIOR[last_op - L_OP]) return(1);
    return(0);
}
/*NH*/
L_add_stack(op)
int     op;
{
    if(L_NB_OPS >= L_MAX_STACK) return(L_errno = L_STACK_ERR);

    switch(op) {
    case L_OP :
	while(L_priority_sup(L_TOKEN.tk_def))
	    if(L_save_op() != 0) return(L_errno);
    case L_FN :
    case L_TFN :
    case L_MTFN :
	L_OPS[L_NB_OPS].ls_nb_args = 1;
	L_OPS[L_NB_OPS].ls_op = L_TOKEN.tk_def;
	break;
    case L_OPENP :
	L_PAR++;
	L_OPS[L_NB_OPS++].ls_op = L_OPENP;
	if(L_save_op() != 0) return(L_errno);
	L_OPS[L_NB_OPS].ls_op = L_OPENP;
	break;
    case L_CLOSEP :
	L_PAR--;
	while(L_NB_OPS > 0) {
	    if(last_op == L_OPENP) {
		last_op = L_CLOSEP;
		return(L_save_op());
	    }
	    if(L_save_op() != 0) return(L_errno);
	}
	return(L_errno = L_PAR_ERR);
    case L_COMMA :
	if(L_add_stack(L_CLOSEP)) return(L_errno);
	if(L_NB_OPS <= 0 ||
		!(is_fn(last_op) || is_tfn(last_op) || is_mtfn(last_op)) )
	    return(L_errno = L_SYNTAX_ERR);
	last_ls.ls_nb_args++;
	return(L_add_stack(L_OPENP));
    case L_OCPAR :
	if(L_NB_OPS <= 0 || !is_fn(last_op)) return(L_errno = L_SYNTAX_ERR);
	last_ls.ls_nb_args = 0;
	return(0);
    default :
	break;
    }

    L_NB_OPS++;
    return(0);
}
/*NH*/
L_clear_stack()
{
    while(L_NB_OPS > 0)
	if(L_save_op() != 0) return(L_errno);
    L_alloc_expr(L_NB_EXPR + 1);
    L_EXPR[L_NB_EXPR++].al_type = L_EOE;
    return(0);
}
/*NH*/
L_anal_lag()
{
    int     op, lag;

    switch(L_get_token()) {
    case L_OP :
	op = L_TOKEN.tk_def;
	if(op != L_MINUS && op != L_PLUS) goto err;
	if(L_get_token() != L_LCONST) goto err;
	lag = LYYLONG;
	if(op == L_MINUS) lag = -lag;
	L_lag_expr(lag);
	break;

    case L_PERIOD :
	L_time_expr();
	break;

    default :
	goto err;
    }

    if(L_errno != 0) return(L_errno);
    if(L_get_token() == L_CLOSEB) return(0);

err:
    L_errno = L_LAG_ERR;
    return(L_errno);
}

/*NH*/
L_sub_expr(al, i)
ALEC    *al;
int     i;
{
    int     nb_par = 0,
	    keyw;

    for( ; i >= 0 ; i--) {
	keyw = al[i].al_type;
	switch(keyw) {
	case L_OPENP  :
	    nb_par--;
	    break;
	case L_CLOSEP :
	    nb_par++;
	    break;
	default :
	    break;
	}
	if(nb_par > 0) continue;
	if(is_op(keyw)) {
	    i = L_sub_expr(al, i - 1);
	    i = L_sub_expr(al, i - 1);
	    return(i);
	}
	if(!is_fn(keyw) && !is_tfn(keyw) && !is_mtfn(keyw)) return(i);
    }
err:
    L_errno = L_LAG_ERR;
    return(-1);
}
/*NH*/
L_time_expr()
{
    int     pos;
    ALEC    *al;

    pos = L_sub_expr(L_EXPR, L_NB_EXPR - 1);
    if(pos < 0) return(L_errno);
    al = L_EXPR + pos;
    for( ; pos < L_NB_EXPR ; pos++, al++) {
	if(al->al_type != L_VAR) continue;
	if(al->al_val.v_var.per.p_s != 0) continue;
	memcpy(&(al->al_val.v_var.per), &(L_TOKEN.tk_period), sizeof(PERIOD));
    }
    return(0);
}
/*NH*/
L_lag_expr(lag)
int     lag;
{
    int     pos;
    ALEC    *al;

    pos = L_sub_expr(L_EXPR, L_NB_EXPR - 1);
    if(pos < 0) return(L_errno);
    al = L_EXPR + pos;
    for( ; pos < L_NB_EXPR ; pos++, al++) {
	if(al->al_type != L_VAR) continue;
	if(al->al_val.v_var.per.p_s != 0) continue;
	al->al_val.v_var.lag += lag;
    }
    return(0);
}

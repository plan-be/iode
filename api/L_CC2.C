#include "iode.h"

/*NH*/
/*
    Second step of compilation process.
    Creates the compiled form :
	    - (short) exec form lenght
	    - (short) nb variables
	    - (L_CLIST) variable names
	    - executable form
*/

CLEC *L_cc2(expr)
ALEC *expr;
{

    unsigned char    *ll = 0, *tmp, *cexpr;
    CLEC    *ptr = 0;
    int     lg = 0, i, pos, pos1, alg = 0, j, nvargs;
    PERIOD  *per;
    long    l;
	short   lag; // GB 11/12/12
	long    len, len1; // GB 11/12/12
    ALEC    *al;
    CVAR    cvar;

    if(expr == 0) return(ptr);
//    cexpr = L_expr2c(expr);
//    SCR_free(cexpr);
    for(al = expr, i = 0 ; al->al_type != L_EOE ; al++, i++) {
        if(lg + 30 >= alg) {
            ll = SW_nrealloc(ll, alg, alg + 512);
            alg += 512;
        }
        ll[lg++] = al->al_type;
        switch(al->al_type) {
        case L_VAR:
        case L_COEF:
			cvar.pad = 0; // JMP 11/9/2015
            cvar.pos = al->al_val.v_var.pos;
            cvar.lag = al->al_val.v_var.lag;
            cvar.per = al->al_val.v_var.per;
            cvar.ref = 0;
            memcpy(ll + lg, &cvar, sizeof(CVAR));
            lg += sizeof(CVAR);
            break;
        case L_PERIOD:
            memcpy(ll + lg, &(al->al_val.v_per), sizeof(PERIOD));
            lg += sizeof(PERIOD) + s_short;
            break;
        case L_DCONST:
            memcpy(ll + lg, &(al->al_val.v_real), sizeof(LECREAL)); /* FLOAT 11-04-98 */
            lg += sizeof(LECREAL); /* FLOAT 11-04-98 */
            break;
        case L_LCONST:
            memcpy(ll + lg, &(al->al_val.v_long), sizeof(long));
            lg += sizeof(long);
            break;
        case L_OPENP:
        case L_CLOSEP:
            lg--;
            break;
        default:
            if(is_fn(al->al_type))
                ll[lg++] = al->al_val.v_nb_args;

            if(is_tfn(al->al_type)) {
                pos = L_sub_expr(expr, i - 1);
                len = L_calc_len(expr, pos, i);
                /* Move last arg */
                tmp = ll + lg - len - 1;
                L_move_arg(tmp + 2 + sizeof(short), tmp, len);
                memcpy(tmp + 2, &len, sizeof(short));
                tmp[0] = al->al_type;
                tmp[1] = al->al_val.v_nb_args;
                lg += sizeof(short) + 1;
            }

            if(is_mtfn(al->al_type)) {
                nvargs = L_MIN_MTARGS[al->al_type - L_MTFN];
                len = 0;
                pos = i - 1;
                for(j = 0 ; j < nvargs ; j++) {
                    pos1 = L_sub_expr(expr, pos);
                    len1 = L_calc_len(expr, pos1, pos + 1);
                    len += len1;
                    pos = pos1 - 1;
                    /* Move arg */
                    tmp = ll + lg - 1 - len;
                    L_move_arg(tmp + 3 + (nvargs - j + 1) * sizeof(short), tmp, len1);
                    memcpy(tmp + 3 + (nvargs - j) * sizeof(short), &len1, sizeof(short));
                }

                lg += 2 + (1 + nvargs) * sizeof(short);
                tmp[0] = al->al_type;
                tmp[1] = al->al_val.v_nb_args;
                tmp[2] = nvargs;
                len += nvargs * sizeof(short);
                memcpy(tmp + 3, &len, sizeof(short));
            }

            break;
        }
    }

    /* Create the compiled form :
        - (short) exec form lenght
        - (short) nb variables
        - (L_CLIST) variable names
        - executable form
    */

    if(lg == 0) goto fin;
    len = sizeof(CLEC) + (L_NB_NAMES - 1) * sizeof(LNAME) + lg;
    ptr = (CLEC *) L_malloc(len);
    ptr->tot_lg  = len;
    ptr->exec_lg  = lg;
    ptr->nb_names = L_NB_NAMES;
    for(i = 0 ; i < L_NB_NAMES ; i++)
        strcpy(ptr->lnames[i].name, L_NAMES[i]);
    tmp = (char *) ptr;
    memcpy(tmp + len - lg, ll, lg);
fin:
    SW_nfree(ll);
    return(ptr);
}
/*NH*/
int L_move_arg(s1, s2, lg)
char    *s1, *s2;
short   lg;
{
    int i;

    for(i = lg - 1 ; i >= 0 ; i--)
        s1[i] = s2[i];

    return(0);
}

/*NH*/
L_calc_len(expr, from, to)
ALEC    *expr;
int     from,
        to;
{
    int     lg = 0, i;
    ALEC    *al;

    if(expr == 0) return(0);
    for(al = expr + from, i = from ; i < to ; al++, i++) {
        lg ++;
        switch(al->al_type) {
        case L_COEF:
        case L_VAR:
            lg += sizeof(CVAR);
            break;
        case L_PERIOD:
            lg += sizeof(PERIOD) + s_short;
            break;
        case L_DCONST:
            lg += sizeof(LECREAL);
            break; /* FLOAT 11-04-98 */
        case L_LCONST:
            lg += sizeof(long);
            break;
        case L_OPENP:
        case L_CLOSEP:
            lg--;
            break;
        default:
            if(is_fn(al->al_type)) lg++;
            if(is_tfn(al->al_type)) lg += 1 + sizeof(short);
            if(is_mtfn(al->al_type))
                lg += 2 + (sizeof(short) *
                           (1 + L_MIN_MTARGS[al->al_type - L_MTFN])); /* JMP 17-04-98 */
            break;
        }
    }
    return(lg);
}













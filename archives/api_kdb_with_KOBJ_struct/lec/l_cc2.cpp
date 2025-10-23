/**
 *  @header4iode
 *  
 *  Second step of the LEC compilation process. During this step, a CLEC structure is created based on the content of L_EXPR and L_NAMES. 
 *
 *  A CLEC struct is the "executable" form a the LEC expression. This struct contains (see also iode.h):
 *      - (short)   : length of the "executable" expression 
 *      - (short)   : number of variables 
 *      - (LNAME)   : list of LNAME structs where LNAME = [variable name, position in KDB defined JIT at "link"] (see iode.h)
 *      - (char*)   : executable expression (following the last LNAME above). Each element of the expression contains:
 *                      a type (1 byte) = type of the ALEC expression (can be L_VAR, L_COEF, op...)
 *                      the optional parameter(s) whose content depends on type. For example:
 *                           - CVAR struct for type variables or scalars (see iode.h):
 *                                  - position of the variable or scalar in the list of names LNAME's
 *                                  - lag (for variables)
 *                                  - ref (used only at execution time)
 *                                  - Period (for variables)
 *                           - Period for type "period"
 *                           - long for type "integer constant"
 *                           - char for the nb of args in case of type "function"
 *                           - more complex (recursive) struct for types "TFN's" or "MTFN's"
 * 
 * Main functions:
 * 
 *      CLEC *L_cc2(ALEC* expr)                          Second stage of LEC compilation. Generates an "executable" LEC expression.
 *      void L_move_arg(char *s1, char *s2, int lg)      Copies lg bytes from a buffer to another in reverse order. The 2 buffers may overlap.
 *      CLEC *L_cc_stream()                              Compiles L_YY, the open YY stream containing a LEC expression.
 *      CLEC *L_cc(char* lec)                            Compiles a LEC string. 
 */

#include "api/lec/lec.h"

/**
 * Calculates the number of bytes required to save a sequence of ALEC atomic expressions into a CLEC struct by
 * adding the size reclaimed by each ALEC atomic expression.
 * 
 * @param [in] expr     ALEC*   pointer to a table of ALEC atomic expressions
 * @param [in] from     int     starting position in ALEC of the expression whose size is to be evaluated
 * @param [in] to       int     ending position in ALEC of that expression
 * @return              int     size in bytes of the expression in CLEC form
*/
static int L_calc_len(ALEC* expr, int from, int to)
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
            case L_Period:
                lg += sizeof(Period) + s_short;
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


/**
 * Second stage of LEC compilation. Generates an "executable" LEC expression (heterogeous container).
 * 
 * @param [in]  expr    ALEC*   pointer to the first atomic element of the expression (result of L_cc1(), normally L_EXPR)
 * @return              CLEC*   pointer to a compiled LEC (see above for details on the contents of a CLEC)
*/
CLEC *L_cc2(ALEC* expr) 
{
    unsigned char    *ll = 0, *tmp;
    CLEC    *ptr = 0;
    int     lg = 0, i, pos, pos1, alg = 0, j, nvargs;
    //Period  *per;
    //long    l;
    //short   lag; // GB 11/12/12
    long    len, len1; // GB 11/12/12
    ALEC    *al;
    CVAR    cvar;

    if(expr == 0) return(ptr);
    for(al = expr, i = 0 ; al->al_type != L_EOE ; al++, i++) {
        if(lg + 30 >= alg) {
            ll = (unsigned char*) SW_nrealloc(ll, alg, alg + 512);
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
            case L_Period:
                memcpy(ll + lg, &(al->al_val.v_per), sizeof(Period));
                lg += sizeof(Period) + s_short;
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
                    L_move_arg((char*) tmp + 2 + sizeof(short), (char*) tmp, len);
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
                        L_move_arg((char*) tmp + 3 + (nvargs - j + 1) * sizeof(short), (char*) tmp, len1);
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

    /* Create the compiled form CLEC* :
        - (short) exec form lenght
        - (short) nb variables
        - (LNAME) variable names
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
    tmp = (unsigned char *) ptr;
    memcpy(tmp + len - lg, ll, lg);
fin:
    SW_nfree(ll);
    return(ptr);
}

/**
 * Copies lg bytes from a buffer to another in reverse order. The 2 buffers may overlap.
 * 
 * @param [out] s1  char*   output buffer
 * @param [in]  s2  char*   input buffer    
 * @param [in]  lg  int     nb of bytes to copy
*/
void L_move_arg(char *s1, char *s2, int lg)
{
    int i;

    for(i = lg - 1 ; i >= 0 ; i--)
        s1[i] = s2[i];
}


/**
 * Compiles L_YY, the open YY stream containing a LEC expression (see l_cc1.c).
 *  
 * @return  CLEC*  compiled and serialized LEC expression.
 */
CLEC *L_cc_stream()
{
    CLEC    *cl;

    if(L_cc1(0) != 0)
        return((CLEC *)0);
    cl = L_cc2(L_EXPR);
    L_alloc_expr(-1);       // Frees L_EXPR
    return(cl);
}


/**
 *  Compiles a LEC string. Returns an allocated CLEC struct pointer.
 *  
 *  @param [in]     lec     char*   LEC expression
 *  @return                 CLEC*   compiled and serialized LEC expression
 *  
 */
CLEC *L_cc(char* lec)
{
    CLEC    *clec = 0;

    if(L_open_string(lec)) return(clec);
    clec = L_cc_stream();
    L_close();
    return(clec);
}


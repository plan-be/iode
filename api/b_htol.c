/**
 *  @header4iode
 * 
 *  Report functions to transform high periodicity to low periodicity series (variables). 
 *  
 *  See b_data.c for more details on the report function syntax and logic.
 *  
 *  List of functions 
 *  -----------------
 *  int B_WsHtoLLast(char* arg)     $WsHtoLLast Filename VarList
 *  int B_WsHtoLMean(char* arg)     $WsHtoLMean Filename VarList
 *  int B_WsHtoLSum(char* arg)      $WsHtoLSum Filename VarList
 */

#include "iode.h"

#define HTOL_LAST   0
#define HTOL_MEAN   1
#define HTOL_SUM    2


// Core functions

/**
 *  Computes the target SAMPLE of a conversion of variables from a high to a low periodicity.
 *  
 *  @param [in]   SAMPLE*  f_smpl     sample of the input var file
 *  @param [in]   SAMPLE*  ws_smpl    sample of the current WS 
 *  @param [out]  SAMPLE** t_smpl     allocated target sample
 *  @param [out]  int*     skip       nb of periods to skip (i.e. set to L_NAN) in the **target** variable 
 *  @param [out]  int*     shift      nb of periods to skip in the **source** variable 
 *  @return       rc                  0 on success, -1 on error (incompatible perriodicity...) 
 */
static int HTOL_smpl(SAMPLE *f_smpl, SAMPLE *ws_smpl, SAMPLE **t_smpl, int* skip, int* shift)
{
    int     f_nbper, ws_nbper;
    long    s, y;
    PERIOD  p1, p2;

    f_nbper = PER_nbper(&(f_smpl->s_p1));
    ws_nbper = PER_nbper(&(ws_smpl->s_p1));

    if(ws_nbper <= 0 || ws_nbper == 12) {
        B_seterror("Set Ws periodicity first");
        return(-1);
    }

    if(ws_nbper >= f_nbper) {
        B_seterror("File has less observations than the current ws");
        return(-1);
    }

    memcpy(&p1, &(f_smpl->s_p1), sizeof(PERIOD));
    memcpy(&p2, &(f_smpl->s_p2), sizeof(PERIOD));

    s = p1.p_s;
    y = p1.p_y;
    *shift = f_nbper/ws_nbper;

    p1.p_p = p2.p_p = (ws_smpl->s_p1).p_p;
    p1.p_s = 1;

    memcpy(&p1, PER_addper(&p1, (s - 1)/(*shift)), sizeof(PERIOD));
    *skip = (p1.p_y - y) * f_nbper + (p1.p_s - 1) * (*shift) - (s - 1);
    p2.p_s = ws_nbper;

    if(*skip < 0) {
        *skip += *shift;
        p1.p_y += 1;
    }

    *t_smpl = PER_pertosmpl(&p1, &p2);
    return(0);
}


/**
 *  Transforms high periodicity variables from an external KDB into low periodicity variables in the current WS. 
 *  For example, monthly data in a file into yearly data in the current WS.
 *  
 *  Three methods are implemented : 
 *   - addition of sub-period data: see WsHtoLSum
 *   - average of sub-period data: see WsHtoLMean
 *   - last sub-period value: see WsHtoLLast
 *  
 *  The source periodicity is that of the source file. The target periodicity is the current WS periodicity.
 *  
 *  @param [in] int     method  aggregation method : HTOL_LAST, HTOL_MEAN or HTOL_SUM
 *  @param [in] char*   arg     parameter of the report function (see B_WsHtoLLast() for an example of the syntax)
 *  @return     int             0 on success, -1 on error (file not found, SAMPLEs incompatible...)    
 */

static int B_htol(int method, char* arg)
{
    int     lg, nb, rc = 0,
                    i, j, f, t, shift, skip;
    char    file[K_MAX_FILE + 1], **data = NULL;
    IODE_REAL    *t_vec = NULL, *f_vec = NULL;
    KDB     *from = NULL, *to = NULL;
    SAMPLE  *t_smpl = NULL;


    lg = B_get_arg0(file, arg, 80);

    data = B_ainit_chk(arg + lg, NULL, 0);
    nb = SCR_tbl_size(data);
    if(nb == 0) goto done;

    from = K_load(K_VAR, file, nb, data);
    if(from == NULL) {
        rc = -1;
        goto done;
    }

    if(HTOL_smpl(KSMPL(from), KSMPL(KV_WS), &t_smpl, &skip, &shift) < 0) {
        rc = -1;
        goto done;
    }

    to = K_create(K_VAR, K_UPPER);
    memcpy((SAMPLE *) KDATA(to), t_smpl, sizeof(SAMPLE));
    t_vec = (IODE_REAL *) SW_nalloc((1 + t_smpl->s_nb) * sizeof(IODE_REAL));
    f_vec = (IODE_REAL *) SW_nalloc((1 + KSMPL(from)->s_nb) * sizeof(IODE_REAL));

    for(i = 0; i < KNB(from); i++) {
        memcpy(f_vec, KVVAL(from, i, 0), KSMPL(from)->s_nb * sizeof(IODE_REAL));
        memset(t_vec, 0, t_smpl->s_nb * sizeof(IODE_REAL));

        for(f = 0, t = 0; f < skip; f++) {
            if(f != 0 && f % shift == 0) {
                t_vec[t] = L_NAN;
                t++;
            }
        }

        for(; f + shift <= KSMPL(from)->s_nb; t++, f += shift) { /* JMP_M 4.20 21-07-95 */
            if(method == HTOL_LAST)
                t_vec[t] = f_vec[f + shift - 1] ;
            else /* SUM and MEAN */
                for(j = 0; j < shift; j++) {
                    if(L_ISAN(f_vec[f + j])) t_vec[t] += f_vec[f + j];
                    else {
                        t_vec[t] = L_NAN;
                        break;
                    }
                }

            if(method == HTOL_MEAN && L_ISAN(t_vec[t])) t_vec[t] /= shift;
        }

        nb = t_smpl->s_nb;
        for(; t < nb; t++) t_vec[t] = L_NAN;

        K_add(to, KONAME(from, i), t_vec, &(nb));
    }

    KV_merge(KV_WS, to, 1);

done:
    K_free(to);
    K_free(from);
    SCR_free_tbl(data);

    SW_nfree(t_smpl);
    SW_nfree(t_vec);
    SW_nfree(f_vec);

    if(rc < 0) return(-1);
    else return(0);
}


// Report functions

/**
 *  Syntax: $WsHtoLLast Filename VarList
 *  
 *  @see https://iode.plan.be/doku.php?id=WsHtoLLast
 */
int B_WsHtoLLast(char* arg)
{
    return(B_htol(HTOL_LAST, arg));
}


/**
 *  Syntax: $WsHtoLMean Filename VarList
 *  
 *  @see https://iode.plan.be/doku.php?id=WsHtoLMean
 */
int B_WsHtoLMean(char* arg)
{
    return(B_htol(HTOL_MEAN, arg));
}


/**
 *  Syntax: $WsHtoLSum Filename VarList
 *  
 *  @see https://iode.plan.be/doku.php?id=WsHtoLSum
 */
int B_WsHtoLSum(char* arg)
{
    return(B_htol(HTOL_SUM, arg));
}

/**
 *  @header4iode
 *  
 *  Calculation of TBL cells on a GSample
 *  --------------------------------------
 *  
 *  This module calculates the values of table cells based on:
 *  - a list of files loaded in memory and stored in K_RWS[VARIABLES].
 *  - a group of column definitions (COLS = GSample compiled by COL_cc(gsample))
 *      where each column defines:
 *          - the period(s) to be used for the calculations (1 or 2 periods)
 *          - an optional operation between the periods (ex growth rates)
 *          - the position in K_RWS[VARIABLES][...] of the (max 2) files to be used for the calculations: 
 *              1 for the WS, 2 for the file K_RWS[VARIABLES][0]...
 *          - an optional operation between the files (ex: diff in %)
 *  - the LEC formulas defined in the table cells 
 *  
 *  How to use these functions to print a table ?
 *      1. call COL_cc(smpl) to compile the GSample in a COLS struct, say cls.
 *      2. call COL_resize() to extend COLS according to the number of columns in the TBL 
 *      3. for each TBL line, call: 
 *          COL_clear(cls) to reset the COLS values 
 *          COL_exec(tbl, i, cls) to store in cls the computed values of the cells in line i
 *          COL_text() to generate the value of the TABLE_CELL_STRING cells
 *  
 *  List of functions 
 *  -----------------
 *      int COL_exec(TBL* tbl, int i, COLS* cls)    Calculates the values of all LEC formulas in one TBL line for all columns of a GSample.
 *      int COL_resize(TBL* tbl, COLS* cls)         Extends the number of COL's (compiled GSample) by multiplying by the number of columns in a TBL definition
 *      void COL_clear(COLS* cls)                   Resets the values in a COLS structure. 
 */
#include <math.h>

#include "api/constants.h"
#include "api/k_super.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/gsample/gsample.h"
#include "api/lec/lec.h"
#include "api/objs/pack.h"
#include "api/objs/variables.h"


/**
 *  Duplicates a CLEC structure.
 *   
 *  @param [in] CLEC*   clec    any CLEC pointer
 *  @return     CLEC*           allocated copy of clec or NULL if clec is empty
 *  
 */
static CLEC *COL_cp_clec(CLEC* clec)
{
    CLEC    *aclec;

    /* GB 19/09/96 */
    if(clec == 0 || clec->tot_lg == 0) return((CLEC *)0);
    aclec = (CLEC *) SW_nalloc(clec->tot_lg);
    memcpy(aclec, clec, clec->tot_lg);
    return(aclec);
}


/**
 *  Links a CLEC. The VAR KDB file is K_RWS[VARIABLES][i - 1]. The Scalar KDB is the current workspace. 
 *  
 *  @param [in] int     i       position of the KBD pointer in K_RWS[VARIABLES], starting at 1 for pos 0
 *  @param [in] CLEC*   clec    Compiled LEC to link with K_RWS[VARIABLES]]i - 1]
 *  @return     int             0 on success, 
 *                              -1 on error. A message is sent to kmsg() if the file is not present.
 *  
 */
static int COL_link(int i, CLEC* clec)
{
    KDB     *kdb = NULL;

    kdb = K_RWS[VARIABLES][i - 1];
    if(kdb == NULL) {
        kmsg("File [%d] not present", i);
        return(-1);
    }

    return(L_link(kdb, KS_WS, clec));
}


/**
 *  Calculates the value of a table CELL on a specific GSample column (COL).
 *  
 *  First links clec and dclec (divisor) according to the COL definition (which 
 *  includes the files numbers and periods, that both refer to K_WS[VARIABLES]). 
 *  The linking of scalars is always done with KS_WS, the current Scalar workspace.
 *  
 *  The result is stored in cl->cl_res, which is IODE_NAN on error.
 *       
 *  @param [in, out] COL*  cl    COL on which the calculation must be applied
 *  @param [in, out] CLEC* clec  compiled LEC expression of the table cell
 *  @param [in, out] CLEC* dclec compiled LEC expression of the table divisor of the current column
 *  @return          int         0 on success, -1 if dclec cannot be linked (TODO: check this)
 *  
 */
 
static int COL_calc(COL* cl, CLEC* clec, CLEC* dclec)
{
    int     i, j, t[2], tmp, per;
    KDB     *kdb = NULL;
    double  vy[2], vf[2], div, mant, sign;

    for(i = 0; i < 2; i++) { /* deux fichiers */
        if(cl->cl_fnb[i] == 0) continue;
        if(COL_link(cl->cl_fnb[i], clec)) goto err;                 // TODO: consistency: impossible clec link returns 0, but -1 for dclec
        if(dclec && COL_link(cl->cl_fnb[i], dclec)) return(-1);
        kdb = K_RWS[VARIABLES][cl->cl_fnb[i] - 1];

        for(j = 0 ; j < 2 ; j++) {
            if(j == 1 && cl->cl_opy == COL_NOP) {
                t[1]  = t[0];
                vy[1] = vy[0];
                break;
            }
            t[j]  = cl->cl_per[j].difference(KSMPL(kdb)->start_period);
            vy[j] = L_exec(kdb, KS_WS, clec, t[j]);
            if(!IODE_IS_A_NUMBER(vy[j])) goto err; /* JMP 16-12-93 */
            div = 1.0;
            if(dclec) div = L_exec(kdb, KS_WS, dclec, t[j]);
            if(!IODE_IS_A_NUMBER(div) || div == 0) goto err; /* JMP 16-12-93 */
            vy[j] /= div;
        }

        if(t[0] > t[1]) {
            tmp = t[0];
            t[0] = t[1];
            t[1] = tmp;
        }
        per = t[1] - t[0];
        switch(cl->cl_opy) {
            case COL_NOP  :
                vf[i] = vy[0];
                break;
            case COL_MDIFF:
                vf[i] = 0.0;
                if(per == 0) break;
                vf[i] = (vy[0] - vy[1]) / per;
                break;
            case COL_MGRT :
                vf[i] = 0.0;
                if(per == 0) break;
                if(vy[1] == 0.0) goto err;

                // Correction JMP 13/4/2018 pour taux de croissance négatifs
                mant = vy[0] / vy[1]; // JMP 16/5/2019
                sign = 1;
                if(mant < 0) {
                    mant = -mant;
                    sign = -1;
                }
                vf[i] = sign * 100 * (pow(mant, (1.0 / per)) -1);

                //if(vy[0] * vy[1] <= 0) goto err;  // JMP 08-08-97
                //vf[i] = 100 * (pow((vy[0] / vy[1]), (1.0 / per)) -1) ;
                break;
            case COL_BASE :
                if(vy[1] == 0.0) goto err;
                vf[i] = 100 * (vy[0] / vy[1]);
                break;
            case COL_DIFF :
                vf[i] = vy[0] - vy[1];
                break;
            case COL_GRT  :
                if(vy[1] == 0.0) goto err;
                vf[i] = 100 * (vy[0] / vy[1] - 1.0);
                break;
            case COL_MEAN :
            case COL_ADD  :
                vf[i] = 0.0;
                for(j = t[0]; j <= t[1] ; j++) {
                    vy[0] = L_exec(kdb, KS_WS, clec, j);
                    if(!IODE_IS_A_NUMBER(vy[0])) goto err; /* JMP 16-12-93 */
                    div = 1.0;
                    if(dclec) div = L_exec(kdb, KS_WS, dclec, j);
                    if(!IODE_IS_A_NUMBER(div) || div == 0) goto err; /* JMP 16-12-93 */
                    vf[i] += vy[0] / div;
                }
                if(cl->cl_opy == COL_MEAN) vf[i] /= per + 1;
                break;
            default :
                goto err;
        }
    }

    switch(cl->cl_opf) {
        case COL_NOP  :
            cl->cl_res = vf[0];
            break;
        case COL_DIFF :
            cl->cl_res = vf[0] - vf[1];
            break;
        case COL_MEAN :
            cl->cl_res = 0.5 * (vf[0] + vf[1]);
            break;
        case COL_GRT  :
            if(vf[1] == 0) goto err;
            cl->cl_res = 100 * (vf[0] / vf[1] - 1);
            break;
        case COL_BASE  :
            if(vf[1] == 0) goto err;
            cl->cl_res = 100 * (vf[0] / vf[1]);
            break;
        case COL_ADD  :
            cl->cl_res = vf[0] + vf[1];
            break;
        default       :
            goto err;
    }

    return(0);
err:
    cl->cl_res = IODE_NAN;
    return(0);
}


/**
 *  After the compilation of a GSample into a COLS structure, multiply the resulting number of COL's 
 *  by the number of columns in the TBL definition (usually 2).
 *  
 *  For example, if the GSample is "2020/2019:5" and the table consists of 2 columns, the 
 *  resulting COLS* will contain 5 x 2 COL's.
 *    
 *  @param [in]         TBL*    tbl     TBL to be calculated
 *  @param [in, out]    COLS*   cls     compiled GSample (via COL_cc())
 *  @return             int             new number of columns in cls
 *  
 */
 
int COL_resize(TBL* tbl, COLS* cls)
{
    COL     *old, *ptr;
    int     i,j, old_nb, new_nb, dim;

    old = cls->cl_cols;
    old_nb = cls->cl_nb;

    dim = tbl->t_nc;
    new_nb = cls->cl_nb * dim;

    cls->cl_cols = (COL *) SW_nalloc(sizeof(COL)* new_nb);
    cls->cl_nb = new_nb;

    ptr = cls->cl_cols;
    for(i = 0; i < old_nb; i++)
        for(j = 0; j < dim; j++) memcpy(ptr++, old + i, sizeof(COL));

    if(old_nb != 0) SW_nfree(old);

    if(tbl->t_free == 0) return(1 + old_nb * (dim - 1));
    else return(new_nb);
}


/**
 *  Resets the values in the COLS. 
 *  
 *  @param [in, out] COLS   cls     table of COL's to reset
 *  
 */
void COL_clear(COLS* cls)
{
    int i;
    COL *cl =  cls->cl_cols;

    for(i = 0; i < cls->cl_nb ; i++, cl++) cl->cl_res = 0;
}


/**
 *  Calculates the values of all LEC formulas in one TBL line for all columns 
 *  of a GSample (precompiled into a COLS structure).
 *  
 *  Stores each column calculated values in cls[i]->cl_res.
 *      
 *  @param [in]      TBL*    tbl     TBL to be calculated
 *  @param [in]      int     i       line position to be calculated
 *  @param [in, out] COLS*   cls     compiled GSample (group of COL structures)
 *  @return          int             0 on success, -1 on failure   
 *  
 */
 
int COL_exec(TBL* tbl, int i, COLS* cls)
{
    int     j, lg, d;    /* last used DB to compile */
    COL     *cl;
    TLINE   *line = T_L(tbl) + i;
    TCELL   *cell = (TCELL *) line->tl_val;
    TCELL   *dcell = (TCELL *)(tbl->t_div).tl_val;
    CLEC    *clec = 0, *dclec = 0, *aclec = 0, *adclec = 0;

    lg = cls->cl_nb / T_NC(tbl);

    for(d = 0; d < T_NC(tbl); d++) {
        if(cell[d].tc_type != TABLE_CELL_LEC) continue;
        if(cell[d].tc_val == 0) continue;
        clec = (CLEC *) P_get_ptr(cell[d].tc_val, 1);
        aclec = COL_cp_clec(clec);
        /*GB    if(dcell[d].tc_val) dclec = (CLEC *) P_get_ptr(dcell[d].tc_val, 1); */
        if(dcell[d].tc_type == TABLE_CELL_LEC && dcell[d].tc_val) /* JMP 27-09-96 */
            dclec = (CLEC *) P_get_ptr(dcell[d].tc_val, 1);
        else dclec = NULL;
        adclec = COL_cp_clec(dclec);

        for(j = 0; j < lg; j++) {
            cl = cls->cl_cols + d + (j * T_NC(tbl));
            if(COL_calc(cl, aclec, adclec) < 0) return(-1);
        }
        SW_nfree(aclec);
        SW_nfree(adclec);
    }
    return(0);
}


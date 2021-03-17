#include "iode.h"
/*
FREF    fref[K_MAX_FREF + 1];
*/
COL_resize(tbl, cls)
TBL     *tbl;
COLS    *cls;
{
    COL     *old, *ptr;
    int     i ,j, old_nb, new_nb, dim;

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

void COL_clear(cls)
COLS    *cls;
{
    int i;
    COL *cl =  cls->cl_cols;

    for (i = 0; i < cls->cl_nb ; i++, cl++) cl->cl_res = 0;
}

CLEC *COL_cp_clec(clec)
CLEC    *clec;
{
    CLEC    *aclec;

    /* GB 19/09/96 */
    if(clec == 0 || clec->tot_lg == 0) return((CLEC *)0);
    aclec = (CLEC *) SW_nalloc(clec->tot_lg);
    memcpy(aclec, clec, clec->tot_lg);
    return(aclec);
}

/*
    Calculates all column elements
    alloue temporairement les CLEC utiles
*/
COL_exec(tbl, i, cls)
TBL     *tbl;
int     i;
COLS    *cls;
{
    int     j, lg, d;    /* last used DB to compile */
    COL     *cl;
    TLINE   *line = T_L(tbl) + i;
    TCELL   *cell = (TCELL *) line->tl_val;
    TCELL   *dcell = (TCELL *) (tbl->t_div).tl_val;
    CLEC    *clec = 0, *dclec = 0, *aclec = 0, *adclec = 0;

    lg = cls->cl_nb / T_NC(tbl);

    for(d = 0; d < T_NC(tbl); d++) {
        if(cell[d].tc_type != KT_LEC) continue;
        if(cell[d].tc_val == 0) continue;
        clec = (CLEC *) P_get_ptr(cell[d].tc_val, 1);
        aclec = COL_cp_clec(clec);
        /*GB    if(dcell[d].tc_val) dclec = (CLEC *) P_get_ptr(dcell[d].tc_val, 1); */
        if(dcell[d].tc_type == KT_LEC && dcell[d].tc_val) /* JMP 27-09-96 */
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

COL_calc(cl, clec, dclec)
COL     *cl;
CLEC    *clec, *dclec;
{
    int     i, j, k, t[2], tmp, per;
    KDB     *kdb = NULL;
    double  vy[2], vf[2], div, mant, sign;

    for(i = 0; i < 2; i++) { /* deux fichiers */
        if(cl->cl_fnb[i] == 0) continue;
        if(COL_link(cl->cl_fnb[i], clec)) goto err;
        if(dclec && COL_link(cl->cl_fnb[i], dclec)) return(-1);
        kdb = K_RWS[K_VAR][cl->cl_fnb[i] - 1];

        for(j = 0 ; j < 2 ; j++) {
            if(j == 1 && cl->cl_opy == COL_NOP) {
                t[1]  = t[0];
                vy[1] = vy[0];
                break;
            }
            t[j]  = PER_diff_per(&(cl->cl_per[j]), &(KSMPL(kdb)->s_p1));
            vy[j] = L_exec(kdb, KS_WS, clec, t[j]);
            if(!L_ISAN(vy[j])) goto err; /* JMP 16-12-93 */
            div = 1.0;
            if(dclec) div = L_exec(kdb, KS_WS, dclec, t[j]);
            if(!L_ISAN(div) || div == 0) goto err; /* JMP 16-12-93 */
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
                if(!L_ISAN(vy[0])) goto err; /* JMP 16-12-93 */
                div = 1.0;
                if(dclec) div = L_exec(kdb, KS_WS, dclec, j);
                if(!L_ISAN(div) || div == 0) goto err; /* JMP 16-12-93 */
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
    cl->cl_res = L_NAN;
    return(0);
}

COL_link(i, clec)
int     i;
CLEC    *clec;
{
    KDB     *kdb = NULL;

    kdb = K_RWS[K_VAR][i - 1];
    if(kdb == NULL) {
        kmsg("File [%d] not present", i);
        return(-1);
    }

    return(L_link(kdb, KS_WS, clec));
}
















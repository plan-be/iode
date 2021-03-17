#include "iode.h"

KV_sample(kdb, nsmpl)
KDB      *kdb;
SAMPLE   *nsmpl;
{
    int     i, start1 = 0, start2 = 0, new_val;
    char    *pack, *ptr;
    SAMPLE  *ksmpl, smpl;

    ksmpl = (SAMPLE *) KDATA(kdb);
    if(ksmpl->s_nb != 0) {
        if(PER_common_smpl(ksmpl, nsmpl, &smpl) < 0) return(-1);
        if(smpl.s_nb > 0) {
            start1 = PER_diff_per(&(smpl.s_p1), &(ksmpl->s_p1));
            start2 = PER_diff_per(&(smpl.s_p1), &(nsmpl->s_p1));
        }
    }

    memcpy(ksmpl, nsmpl, sizeof(SAMPLE));
    for(i = 0 ; i < KNB(kdb); i++) {
        new_val = KV_alloc_var(ksmpl->s_nb);
        ptr = SW_getptr(new_val);
        if(KSOVAL(kdb, i) != 0) {
            if(smpl.s_nb > 0)
                memcpy((IODE_REAL *)(P_get_ptr(ptr, 0)) + start2,
                       KVVAL(kdb, i, start1),
                       sizeof(IODE_REAL) * smpl.s_nb);
            SW_free(KSOVAL(kdb, i));
        }
        KSOVAL(kdb, i) = new_val;
    }
    return(0);
}
/*
    Merges two databases : kdb1 <- kdb1 + kdb2
*/
KV_merge(kdb1, kdb2, replace)
KDB     *kdb1, *kdb2;
int     replace;
{
    int     i, start1, start2, pos, nb1;
    int     *p_pos;
    SAMPLE  *k1smpl, *k2smpl, smpl;

    k1smpl = (SAMPLE *) KDATA(kdb1);
    k2smpl = (SAMPLE *) KDATA(kdb2);

    if(k1smpl->s_nb == 0) memcpy(k1smpl, k2smpl, sizeof(SAMPLE));

    if(PER_common_smpl(k1smpl, k2smpl, &smpl) < 0) return(-1);
    if(smpl.s_nb > 0) {
        start1 = PER_diff_per(&(smpl.s_p1), &(k1smpl->s_p1)); /* always >= 0 */
        start2 = PER_diff_per(&(smpl.s_p1), &(k2smpl->s_p1)); /* always >= 0 */
    }

    nb1 = k1smpl->s_nb;
    for(i = 0; i < KNB(kdb2); i++) {
        pos = K_find(kdb1, KONAME(kdb2, i));
        if (pos < 0) pos = K_add(kdb1, KONAME(kdb2, i), NULL, &nb1);
        else if (!replace) continue;

        if(pos >= 0 && KSOVAL(kdb2, i) != 0)
            memcpy((IODE_REAL *) KVVAL(kdb1, pos, start1),
                   (IODE_REAL *) KVVAL(kdb2, i, start2),
                   sizeof(IODE_REAL) * smpl.s_nb);
    }

    return(0);
}



void KV_merge_del(kdb1, kdb2, replace)
KDB   *kdb1, *kdb2;
int   replace;
{
    SAMPLE  *k1smpl, *k2smpl;

    k1smpl = (SAMPLE *) KDATA(kdb1);
    k2smpl = (SAMPLE *) KDATA(kdb2);

    if(KNB(kdb2) == 0) return;
    if(KNB(kdb1) == 0) {
        if(k1smpl->s_nb != 0)
            KV_sample(kdb2, k1smpl);
        else
            memcpy(k1smpl, k2smpl, sizeof(SAMPLE));
        KNB(kdb1)   = KNB(kdb2);
        KOBJS(kdb1) = KOBJS(kdb2);
        KNB(kdb2)   = 0;
        KOBJS(kdb2) = 0;
        K_free(kdb2);
        return;
    }

    KV_merge(kdb1, kdb2, replace);
    K_free(kdb2);
}

double  KV_get(kdb, i, j, mode)
KDB     *kdb;
int     i, j, mode;
{
    int     pernb;
    double  var;
    IODE_REAL    *vt;
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(kdb);
    pernb = PER_nb((smpl->s_p1).p_p);

    switch(mode) {
    case K_LEVEL :
        var = *KVVAL(kdb, i, j);
        break;
    case K_DIFF :
        pernb = 1;
        /* NO BREAK */
    case K_DIFFY :
        if(j < pernb) {
            var = L_NAN;
            break;
        }
        vt = KVVAL(kdb, i, 0);
        if(L_ISAN(vt[j]) && L_ISAN(vt[j - pernb]))
            var = vt[j] - vt[j - pernb];
        else var = L_NAN;
        break;

    case K_GRT :
        pernb = 1;
        /* NO BREAK */
    case K_GRTY :
        if(j < pernb) {
            var = L_NAN;
            break;
        }
        vt = KVVAL(kdb, i, 0);
        if(L_ISAN(vt[j]) && L_ISAN(vt[j - pernb]) && !L_IS0(vt[j - pernb]))
            var = (vt[j]/vt[j - pernb] - 1) * 100.0;
        else var = L_NAN;
        break;
    }

    return(var);
}


void KV_set(kdb, i, j, mode, new)
KDB     *kdb;
int     i, j, mode;
IODE_REAL    new;
{
    int     pernb;
    IODE_REAL    *var;
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(kdb);
    pernb = PER_nb((smpl->s_p1).p_p);

    var = KVVAL(kdb, i, 0);
    switch(mode) {
    case K_LEVEL :
        var[j] = new;
        break;

    case K_DIFF :
        pernb = 1;
        /* NO BREAK */
    case K_DIFFY :
        if(j < pernb)  break;

        if(L_ISAN(new) && L_ISAN(var[j - pernb])) var[j] = var[j - pernb] + new;
        else var[j] = L_NAN;
        break;

    case K_GRT :
        pernb = 1;
        /* NO BREAK */
    case K_GRTY :
        if(j < pernb) break;

        if(L_ISAN(new) && L_ISAN(var[j - pernb]))
            var[j] = (new/100.0 + 1.0) * var[j - pernb];
        else var[j] = L_NAN;
        break;
    }
}

KV_extrapolate(dbv, method, smpl, vars)
KDB     *dbv;
int     method;
SAMPLE  *smpl;
char    **vars;
{
    int     rc = -1, i, v, bt, t;
    IODE_REAL    *val;
    KDB     *edbv = NULL;

    bt = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));
    if(bt < 0 || PER_diff_per(&(KSMPL(dbv)->s_p2), &(smpl->s_p2)) < 0) {
        B_seterror(B_msg(131));
        goto done;
    }

    if (vars == NULL || SCR_tbl_size(vars) == 0) edbv = dbv;
    else edbv = K_refer(dbv, SCR_tbl_size(vars), vars);
    // if(edbv == NULL) goto done; /* JMP 12-05-11 */
    if(edbv == NULL || KNB(edbv) == 0) goto done; /* JMP 12-05-11 */

    for(v = 0; v < KNB(edbv); v++) {
        val = KVVAL(edbv, v, 0);
        for(i = 0, t = bt; i < smpl->s_nb; i++, t++)
            K_init_values_1(val, t, method);
    }
    rc = 0;

done:
    if(edbv != dbv) K_free_kdb(edbv);
    return(rc);

}

KDB *KV_aggregate(dbv, method, pattern, filename)
KDB     *dbv;
int     method;
char    *pattern, *filename;
{
    int     i, t, nb_per, added, *times, nbtimes = 500,
                                         epos, npos;
    ONAME   ename, nname;
    IODE_REAL    *eval = NULL, *nval;
    KDB     *edbv = NULL, *ndbv = NULL;
    SAMPLE  *smpl;

    if (filename == NULL || filename[0] == 0) edbv = dbv;
    else edbv = K_interpret(K_VAR, filename);
    if(edbv == NULL) goto done;

    smpl = KSMPL(edbv);
    nb_per = KSMPL(edbv)->s_nb;
    eval = SCR_malloc(nb_per * sizeof(IODE_REAL));
    times = SCR_malloc(nbtimes * sizeof(int));

    ndbv = K_create(K_VAR, K_UPPER);
    if(ndbv == NULL) goto done;
    else memcpy(KDATA(ndbv), KSMPL(edbv), sizeof(SAMPLE));

    for(epos = 0; epos < KNB(edbv); epos++) {
        strcpy(ename, KONAME(edbv, epos));
        if(K_aggr(pattern, ename, nname) < 0) continue;

        added = 1;
        npos = K_find(ndbv, nname);
        if(npos < 0) {
            npos = K_add(ndbv, nname, NULL, &nb_per);
            if(npos > nbtimes - 1) {
                times = (int *) SCR_realloc((char *) times, sizeof(int), nbtimes, nbtimes + 500);
                nbtimes += 500;
            }
            times[npos] = 1;

        }
        else {
            added = 0;
            times[npos] += 1;
        }
        if(npos < 0) goto done;

        memcpy(eval, KVVAL(edbv, epos, 0), nb_per * sizeof(IODE_REAL));
        nval = KVVAL(ndbv, npos, 0);
        for(t = 0; t < smpl->s_nb; t++) {
            if(added) nval[t] = eval[t];
            else {
                if(method == 1) { /* m = 1 PROD */
                    if(L_ISAN(eval[t])) nval[t] *= eval[t];
                    else nval[t] = L_NAN;
                }
                else { /* m != 1 SUM or MEAN */
                    if(L_ISAN(eval[t])) nval[t] += eval[t];
                    else nval[t] = L_NAN;
                }
            }
        }

    }

    if(method == 2) {
        for(npos = 0; npos < KNB(ndbv); npos++) {
            nval = KVVAL(ndbv, npos, 0);
            for(t = 0; t < smpl->s_nb; t++)
                if(L_ISAN(nval[t])) nval[t] /= times[npos];
        }
    }

done:
    if(edbv != dbv) K_free(edbv); /* JMP 12-11-98 */
    SCR_free(eval);
    SCR_free(times);
    return(ndbv);
}

KV_GetSmpl(smpl, name)
SAMPLE  *smpl;
char    *name;
{
    int     pos, t;
    IODE_REAL    *val;
    SAMPLE  *wsmpl = KSMPL(K_WS[K_VAR]);

    pos = K_find(K_WS[K_VAR], name);
    if(pos < 0) return(-1);
    val = KVVAL(K_WS[K_VAR], pos, 0);

    for(t = 0 ; t < wsmpl->s_nb ; t++)
        if(L_ISAN(val[t])) break;

    memcpy(&(smpl->s_p1), PER_addper(&(wsmpl->s_p1), t), sizeof(PERIOD));

    for( ; t < wsmpl->s_nb ; t++)
        if(!L_ISAN(val[t])) break;

    memcpy(&(smpl->s_p2), PER_addper(&(wsmpl->s_p1), t), sizeof(PERIOD));

    smpl->s_nb = PER_diff_per(&(smpl->s_p2), &(smpl->s_p1));
}










/**
 * @header4iode
 *
 * Functions acting on workspaces of variables.
 *
 *    int KV_sample(KDB *kdb, SAMPLE *nsmpl)                                  Changes the SAMPLE of a KDB of variables.
 *    int KV_merge(KDB *kdb1, KDB* kdb2, int replace)                         Merges two KDB of variables: kdb1 <- kdb1 + kdb2.            
 *    void KV_merge_del(KDB *kdb1, KDB *kdb2, int replace)                    Merges 2 KDB of variables, then deletes the second one.
 *    int KV_add(KDB* kdb, char* varname)                                               Adds a new variable in KV_WS. Fills it with IODE_NAN.
 *    double KV_get(KDB *kdb, int pos, int t, int mode)                       Gets VAR[t]  where VAR is the series in position pos in kdb. 
 *    void KV_set(KDB *kdb, int pos, int t, int mode, double new)          Sets VAR[t], where VAR is the series in position pos in kdb. 
 *    int KV_extrapolate(KDB *dbv, int method, SAMPLE *smpl, char **vars)     Extrapolates variables on a selected SAMPLE according to one of the available methods.
 *    KDB *KV_aggregate(KDB *dbv, int method, char *pattern, char *filename)  Creates a new KDB with variables created by aggregation based on variable names.
 *    void KV_init_values_1(double* val, int t, int method)                Extrapolates 1 value val[t] based on val[t], val[t-1] and a selected method.
 *   
 *    int KV_per_pos(PERIOD* per2)                                            Retrieves the position of a PERIOD in the current KV_WS sample.
 *    int KV_aper_pos(char* aper2)                                            Retrieves the position of a period in text format in the current KV_WS sample.  
 *    double KV_get_at_t(char*varname, int t)                                 Retrieves the value of varname[t] 
 *    double KV_get_at_per(char*varname, PERIOD* per)                         Retrieves the value of varname[per] 
 *    double KV_get_at_aper(char*varname, char* aper)                         Retrieves the value of varname[aper]
 *    int KV_set_at_t(char*varname, int t, double val)                        Replaces the value of varname[t] by val.
 *    int KV_set_at_per(char*varname, PERIOD* per, double val)                Replaces the value of varname[per] by val.
 *    int KV_set_at_aper(char*varname, char* aper, double val)                Replaces the value of varname[aper] by val.
 */
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/grep.h"
#include "api/objs/pack.h"
#include "api/objs/variables.h"


/**
 * Changes the SAMPLE of a KDB of variables. Truncates the vars and/or add NaN values to fill the variables on the new sample.
 * 
 * @param [in, out] kdb     KDB*            pointer to a KDB of variables
 * @param [in]      nsmpl   SAMPLE* | NULL  new sample or NULL. If NULL or empty, no changes are made to the current WS
 * @return                  int             -1 if the kdb's sample and nsmpl don't overlap or the resulting set is empty
 *                                          0 otherwise
 */
 
int KV_sample(KDB *kdb, SAMPLE *nsmpl)
{
    int     i, start1 = 0, start2 = 0, new_val;
    char    *ptr;
    SAMPLE  *ksmpl, smpl;

    if(nsmpl == NULL || nsmpl->s_nb == 0) return(0);
    
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
                memcpy((double *)(P_get_ptr(ptr, 0)) + start2,
                       KVVAL(kdb, i, start1),
                       sizeof(double) * smpl.s_nb);
            SW_free(KSOVAL(kdb, i));
        }
        KSOVAL(kdb, i) = new_val;
    }
    return(0);
}

/**
 *  Merges two KDB of variables: kdb1 <- kdb1 + kdb2.
 *  
 *  The resulting sample if the sample of kdb1.
 *  If homonyms exist, vars from kbd2 replace their homonyms in kdb1 
 *  if and only if replace is not 0.
 *  
 *  @param [in, out]     kdb1         KDB *   KDB that will contain both kdb1 and kdb2.
 *  @param [in]          kdb2         KDB *   source KDB.
 *  @param [in]          replace      int     if not null, kdb2 vars replace kdb1 homonyms.
 *  @return                           int     -1 if there is no intersection between the 2 samples
 *  TODO: what if kdb1 == NULL ?                                             0 if ok.
 */
int KV_merge(KDB *kdb1, KDB* kdb2, int replace)
{
    int     i, start1, start2, pos, nb1;
    //int     *p_pos;
    SAMPLE  *k1smpl, *k2smpl, smpl;

    if(kdb2 == NULL) return(0);  // JMP 30/09/2022
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
        if(pos < 0) pos = K_add(kdb1, KONAME(kdb2, i), NULL, &nb1);
        else if(!replace) continue;

        if(pos >= 0 && KSOVAL(kdb2, i) != 0)
            memcpy((double *) KVVAL(kdb1, pos, start1),
                   (double *) KVVAL(kdb2, i, start2),
                   sizeof(double) * smpl.s_nb);
    }

    return(0);
}


/**
 *  Merges 2 KDB of variables, then deletes the second one.
 * 
 *  The resulting sample is kdb1 sample if it is defined. 
 *  If not, the sample of kdb2 is used.
 * 
 *  @param [in, out] kdb1       KDB*   ws that will contain the result of the merge.
 *  @param [in, out] kdb2       KDB*   ws to copy into kdb1
 *  @param [in]      replace    int    indicates if kdb2 homonyms must replace kdb1
 * 
 *  TODO: what if kdb1 == NULL ?
 */

void KV_merge_del(KDB *kdb1, KDB *kdb2, int replace)
{
    SAMPLE  *k1smpl, *k2smpl;

    if(kdb2 == NULL) return;  // JMP 30/09/2022
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


/**
 *  Adds a new variable in KV_WS. Fills it with IODE_NAN.
 *  If the variable already exists, replaces all values by IODE_NAN.
 *  
 *  @param [in] kdb     KDB*        KDB of variables
 *  @param [in] char*   varname     name of the new variable
 *  @return     int                 position of varname in the kdb
 */
int KV_add(KDB* kdb, char* varname)
{
    int      pos, t, nobs;
    double   *vptr;

    if(KTYPE(kdb) != VARIABLES) 
        return(-1);
    
    // Create varname with NaN 
    pos = K_find(kdb, varname);
    if(pos < 0) {
        nobs = KSMPL(kdb)->s_nb;
        pos = K_add(kdb, varname, NULL, &nobs); // Set IODE_NAN if the new var
    }
    else { 
        // Replaces all values by IODE_NAN 
        vptr = K_vptr(kdb, varname, 0);
        if(vptr == NULL) 
            return(-1);
        for(t = 0; t < KSMPL(kdb)->s_nb; t++) 
            vptr[t] = IODE_NAN;
    }
        
    return(pos);
}

/**
 * Gets VAR[t] where VAR is the series at position pos in kdb. The value can be modified depending on the value of mode.
 *
 * @param [in] kdb       KDB*    KDB of variables
 * @param [in] pos       int     position of the variable in the kdb
 * @param [in] t         int     index in the variable (0 = first position in the series...)
 * @param [in] mode      int     transformation of the value, see below. 
 *
 * @return               double  depends on the param mode (can be IODE_NAN if the operation is impossible):
 *                                    VAR_MODE_LEVEL : no modification    x[t]
 *                                    VAR_MODE_DIFF  : diff on one period (x[t]-x[t-1])
 *                                    VAR_MODE_Y0Y_DIFF : diff on one year   (x[t]-x[t-{nb sub periods}])
 *                                    VAR_MODE_GROWTH_RATE   : grt on one period  (x[t]/x[t-1] - 1)*100
 *                                    VAR_MODE_Y0Y_GROWTH_RATE  : grt on one year    (x[t]/x[t-{nb sub periods}] - 1) * 100
 */
double KV_get(KDB *kdb, int pos, int t, int mode)
{
    int     pernb;
    double  var;
    double    *vt;
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(kdb);
    pernb = PER_nb((smpl->s_p1).p_p);

    switch(mode) {
        case VAR_MODE_LEVEL :
            var = *KVVAL(kdb, pos, t);
            break;
        case VAR_MODE_DIFF :
            pernb = 1;
        /* NO BREAK */
        case VAR_MODE_Y0Y_DIFF : 
            if(t < pernb) {
                var = IODE_NAN;
                break;
            }
            vt = KVVAL(kdb, pos, 0);
            if(IODE_IS_A_NUMBER(vt[t]) && IODE_IS_A_NUMBER(vt[t - pernb]))
                var = vt[t] - vt[t - pernb];
            else var = IODE_NAN;
            break;

        case VAR_MODE_GROWTH_RATE :
            pernb = 1;
        /* NO BREAK */
        case VAR_MODE_Y0Y_GROWTH_RATE :
            if(t < pernb) {
                var = IODE_NAN;
                break;
            }
            vt = KVVAL(kdb, pos, 0);
            if(IODE_IS_A_NUMBER(vt[t]) && IODE_IS_A_NUMBER(vt[t - pernb]) && !IODE_IS_0(vt[t - pernb]))
                var = (vt[t]/vt[t - pernb] - 1) * 100.0;
            else var = IODE_NAN;
            break;
    }

    return(var);
}


/**
 * Set VAR[t], where VAR is the series in position pos in kdb. The new value of VAR[t] depends on 
 * the parameter mode and the value new.
 * 
 * @param [in, out]  kdb       KDB*        KDB of variables
 * @param [in]       pos       int         position of the variable in the kdb
 * @param [in]       t         int         index in the variable (0 = first position in the series...)
 * @param [in]       mode      int         transformation of the value, see below. 
 * @param [in]       value     double      new value of VAR[t] before transformation according to mode, see below. 
 *                                              VAR_MODE_LEVEL : no modification    x[t] = new
 *                                              VAR_MODE_DIFF  : diff on one period x[t] = x[t-1] + new
 *                                              VAR_MODE_Y0Y_DIFF : diff on one year   x[t] = x[t-{nb sub periods}] + new
 *                                              VAR_MODE_GROWTH_RATE   : grt on one period  x[t] = (1 + 0.01*new) * x[t-1]
 *                                              VAR_MODE_Y0Y_GROWTH_RATE  : grt on one year    x[t] = (1 + 0.01*new) * x[t-{nb sub periods}] 
 *                              
 */
 
void KV_set(KDB *kdb, int pos, int t, int mode, double value)
{
    int     pernb;
    double    *var;
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(kdb);
    pernb = PER_nb((smpl->s_p1).p_p);

    var = KVVAL(kdb, pos, 0);
    switch(mode) {
        case VAR_MODE_LEVEL :
            var[t] = value;
            break;

        case VAR_MODE_DIFF :
            pernb = 1;
        /* NO BREAK */
        case VAR_MODE_Y0Y_DIFF :
            if(t < pernb)  break;

            if(IODE_IS_A_NUMBER(value) && IODE_IS_A_NUMBER(var[t - pernb])) var[t] = var[t - pernb] + value;
            else var[t] = IODE_NAN;
            break;

        case VAR_MODE_GROWTH_RATE :
            pernb = 1;
        /* NO BREAK */
        case VAR_MODE_Y0Y_GROWTH_RATE :
            if(t < pernb) break;

            if(IODE_IS_A_NUMBER(value) && IODE_IS_A_NUMBER(var[t - pernb]))
                var[t] = (value/100.0 + 1.0) * var[t - pernb];
            else var[t] = IODE_NAN;
            break;
    }
}



/**
 *  Extrapolates 1 value val[t] based on val[t], val[t-1] and a selected method.
 *      
 *  @param [in, out]    val     double*  pointer to the VAR data
 *  @param [in]         t       int         position in val to be calculated  
 *  @param [in]         method  int         extrapolation method 
 *  @return                     void
 *  
 *  Methods:
 *       VAR_INIT_ASIS     : val[t] remains unchanged
 *       VAR_INIT_TM1      : if(val[t] == 0 or val[t] == NaN) val[t] = val[t-1] (*)
 *       VAR_INIT_TM1_NA   : if(val[t] == NaN)                val[t] = val[t-1] (*)
 *       VAR_INIT_TM1_A    :                                  val[t] = val[t-1] (*)
 *       VAR_INIT_EXTRA    : if(val[t] == 0 or val[t] == NaN) val[t] = fn(val[t-1], val[t-2]) (*)
 *       VAR_INIT_EXTRA_NA : if(val[t] <> NaN)                val[t] = fn(val[t-1], val[t-3]) (*)
 *       VAR_INIT_EXTRA_A  :                                  val[t] = fn(val[t-1], val[t-3]) (*)
 *       
 *       where fn(v1, v2) = 2 * v1 - v2
 *      (*) If val[t-1] and / or val[t-2] are NaN, val[t] = 1.0
 *
 */
void KV_init_values_1(double* val, int t, int method)
{

    switch(method) {
        case VAR_INIT_TM1 :
            if(IODE_IS_A_NUMBER(val[t]) && !IODE_IS_0(val[t])) return;
            goto calc1;
        case VAR_INIT_TM1_NA :
            if(IODE_IS_A_NUMBER(val[t])) return;
            goto calc1;
        case VAR_INIT_TM1_A :
calc1:
            val[t] = 1.0;
            if(t > 0 && IODE_IS_A_NUMBER(val[t - 1])) val[t] = val[t - 1];
            return;
        case VAR_INIT_EXTRA :
            if(IODE_IS_A_NUMBER(val[t]) && !IODE_IS_0(val[t])) return;
            goto calc2;
        case VAR_INIT_EXTRA_NA :
            if(IODE_IS_A_NUMBER(val[t])) return;
            goto calc2;
        case VAR_INIT_EXTRA_A :
calc2:
            val[t] = 1.0;
            if(t > 0 && IODE_IS_A_NUMBER(val[t - 1])) {
                if(t > 1 && IODE_IS_A_NUMBER(val[t - 2]))
                    val[t] = 2 * val[t - 1] - val[t - 2];
                else val[t] = val[t - 1];
            }
        case VAR_INIT_ASIS :
            return;
    }
}


/**
 *  Extrapolates variables on a selected SAMPLE according to one of the available methods. These extrapolation methods are
 *  described in the function KV_init_values(). 
 *  
 *  @param [in, out]    dbv    KDB*     KDB of variables on which the operation will be applied
 *  @param [in]         method int      identification of the extrapolation method (see K_init_values())
 *  @param [in]         smpl   SAMPLE*  SAMPLE on which the operation is to be carried out
 *  @param [in]         vars   char**   if not NULL, restricted list of variables to extrapolate
 *                                      if NULL, all variables in KDB will be modified
 *  @return                             0 on success,
 *                                      -1 if the SAMPLE is invalid or the KDB is empty or dbv does not contain any variable 
 *                                          from the list vars
 *  @details 
 */
 
int KV_extrapolate(KDB *dbv, int method, SAMPLE *smpl, char **vars)
{
    int         rc = -1, i, v, bt, t;
    double   *val;
    KDB         *edbv = NULL;

    bt = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));
    if(bt < 0 || PER_diff_per(&(KSMPL(dbv)->s_p2), &(smpl->s_p2)) < 0) {
        B_seterror(B_msg(131));
        goto done;
    }

    if(vars == NULL || SCR_tbl_size((unsigned char**) vars) == 0) 
        edbv = dbv;
    else 
        edbv = K_refer(dbv, SCR_tbl_size((unsigned char**) vars), vars);
    // if(edbv == NULL) goto done; /* JMP 12-05-11 */
    if(edbv == NULL || KNB(edbv) == 0) goto done; /* JMP 12-05-11 */

    for(v = 0; v < KNB(edbv); v++) {
        val = KVVAL(edbv, v, 0);
        for(i = 0, t = bt; i < smpl->s_nb; i++, t++)
            KV_init_values_1(val, t, method);
    }
    rc = 0;

done:
    if(edbv != dbv) K_free_kdb(edbv);
    return(rc);

}


/**
 *  Creates a new KDB with variables created by aggregation based on variable names.
 *    
 *  @param [in, out]    dbv         KDB*     KDB of variables on which the operation will be applied
 *  @param [in]         method      int      identification of the extrapolation method 
 *  @param [in]         pattern     char*    pattern defining the variables to be aggregated
 *  @param [in]         filename    char*    if not null nor empty, the onput variables will be loaded from that file
 *  @return                         KDB*     new KDB created by aggragation
 *                                           NULL on error (filename given but inexistent, not enough memory...)
 */

KDB *KV_aggregate(KDB *dbv, int method, char *pattern, char *filename)
{
    int     t, nb_per, added, *times, nbtimes = 500,
                                         epos, npos;
    ONAME   ename, nname;
    double    *eval = NULL, *nval;
    KDB     *edbv = NULL, *ndbv = NULL;
    SAMPLE  *smpl;

    if(filename == NULL || filename[0] == 0) edbv = dbv;
    else edbv = K_interpret(VARIABLES, filename);
    if(edbv == NULL) goto done;

    smpl = KSMPL(edbv);
    nb_per = KSMPL(edbv)->s_nb;
    eval = (double*) SCR_malloc(nb_per * sizeof(double));
    times = (int *) SCR_malloc(nbtimes * sizeof(int));

    ndbv = K_create(VARIABLES, UPPER_CASE);
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

        memcpy(eval, KVVAL(edbv, epos, 0), nb_per * sizeof(double));
        nval = KVVAL(ndbv, npos, 0);
        for(t = 0; t < smpl->s_nb; t++) {
            if(added) nval[t] = eval[t];
            else {
                if(method == 1) { /* m = 1 PROD */
                    if(IODE_IS_A_NUMBER(eval[t])) nval[t] *= eval[t];
                    else nval[t] = IODE_NAN;
                }
                else { /* m != 1 SUM or MEAN */
                    if(IODE_IS_A_NUMBER(eval[t])) nval[t] += eval[t];
                    else nval[t] = IODE_NAN;
                }
            }
        }

    }

    if(method == 2) {
        for(npos = 0; npos < KNB(ndbv); npos++) {
            nval = KVVAL(ndbv, npos, 0);
            for(t = 0; t < smpl->s_nb; t++)
                if(IODE_IS_A_NUMBER(nval[t])) nval[t] /= times[npos];
        }
    }

done:
    if(edbv != dbv) K_free(edbv); /* JMP 12-11-98 */
    SCR_free(eval);
    SCR_free(times);
    return(ndbv);
}


/**
 *  Retrieves the position of a PERIOD in the current KV_WS sample.
 *  
 *  @param [in] PERIOD* per2    PERIOD whose position is searched
 *  @return     int             position in the current WS sample (starting at 0)
 *                              -1 if no WS is loaded or no sample is defined
 */
 
int KV_per_pos(PERIOD* per2)
{
    SAMPLE* smpl;
    int     diff;
    
    if(KV_WS == NULL) return(-1);
    smpl = KSMPL(KV_WS);
    diff = PER_diff_per(per2, &(smpl->s_p1));
    return(diff);
}


/**
 *  Retrieves the position of a period in text format in the current KV_WS sample.
 *  
 *  Example : 
 *      Let the current sample be 2000Y1 2020Y1
 *      Then KV_aper_pos("2002Y1") = 2
 *  
 *  @param [in] char*   aper2 PERIOD in text format (e.g.: "2001Y1")
 *  @return     int           position in the current WS sample (starting at 0)
 *                            or -1 if no WS is loaded or no sample is defined
 *  
 */
 
int KV_aper_pos(char* aper2)
{
    PERIOD  *per2;
    int     pos;
    
    per2 = PER_atoper(aper2);
    pos = KV_per_pos(per2);
    SCR_free(per2);
    return(pos);
}


/**
 *  Retrieves the value of varname[t] in the current KV_WS.
 *  
 *  Example : 
 *      KV_get_at_t("A", 2) => retrieves A[2]
 *  
 *  @param [in] char*   varname Variable name 
 *  @param [in] int     t       position to retrieve starting at 0
 *  
 *  @return     double          value of varname[t]
 *                              IODE_NAN on error
 *  
 */
 
double KV_get_at_t(char*varname, int t)
{
    double  *var_ptr;
    
    var_ptr = KVPTR(varname);
    if(var_ptr == NULL) return(IODE_NAN);
    
    if(t < 0 || KSMPL(KV_WS)->s_nb < t) return(IODE_NAN);
    return(var_ptr[t]);
}

/**
 *  Retrieves the value of varname[per] in the current KV_WS.
 *  
 *  Example : 
 *      KV_get_at_aper("A", "2002Y1")
 *  
 *  @param [in] char*   varname Variable name 
 *  @param [in] PERIOD* per     PERIOD to retrieve
 *  
 *  @return     double          value of varname[per]
 *                              IODE_NAN on error
 *  
 */
 
double KV_get_at_per(char*varname, PERIOD* per)
{
    int     t;
    
    t = KV_per_pos(per);
    return(KV_get_at_t(varname, t));
}


/**
 *  Retrieves the value of varname[aper] in the current KV_WS.
 *  
 *  Example : 
 *      KV_get_at_aper("A", "2002Y1")
 *  
 *  @param [in] char*   varname Variable name 
 *  @param [in] char*   aper    PERIOD in text format (e.g.: "2001Y1")
 *  
 *  @return     double          value of varname[aper]
 *                              IODE_NAN on error
 *  
 */
 
double KV_get_at_aper(char*varname, char* aper)
{
    int     t;
    
    t = KV_aper_pos(aper);
    return(KV_get_at_t(varname, t));
}


/**
 *  Replaces the value of varname[t] by val.
 *  
 *  Example : 
 *      KV_set_at_t("A", 2, 1.0) => A[2] = 1.0 
 *  
 *  @param [in] char*   varname Variable name 
 *  @param [in] int     t       position in the current WS sample
 *  @param [in] double  val     new value of varname[t]
 *  @return     int             0 on success, -1 on error
 *  
 */

int KV_set_at_t(char*varname, int t, double val)
{
    double  *var_ptr;
    
    var_ptr = KVPTR(varname);
    if(var_ptr == NULL) return(-1);

    if(t < 0 || KSMPL(KV_WS)->s_nb < t) return(-1);
    
    var_ptr[t] = val;
    return(0);
}

/**
 *  Replaces the value of varname[per] by val.
 *  
 *  @param [in] char*   varname Variable name 
 *  @param [in] PERIOD* per     PERIOD to modify
 *  @param [in] double  val     new value of varname[per]
 *  @return     int             0 on success, -1 on error
 *  
 */
 
int KV_set_at_per(char*varname, PERIOD* per, double val)
{
    int     t;
    
    t = KV_per_pos(per);
    return(KV_set_at_t(varname, t, val));
}


/**
 *  Replaces the value of varname[aper] by val.
 *  
 *  Example : 
 *      KV_set_at_aper("A", "2002Y1", 2.0) => A[2] = 2.0
 *  
 *  @param [in] char*   varname Variable name 
 *  @param [in] char*   aper    PERIOD in text format (e.g.: "2001Y1")
 *  @param [in] double  val     new value of varname[aper]
 *  @return     int             0 on success, -1 on error  
 */
 
int KV_set_at_aper(char*varname, char* aper, double val)
{
    int     t;
    
    t = KV_aper_pos(aper);
    return(KV_set_at_t(varname, t, val));
}


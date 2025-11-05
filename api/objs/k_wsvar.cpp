/**
 * @header4iode
 *
 * Functions acting on workspaces of variables.
 *
 *    int KV_sample(KDB *kdb, Sample *new_sample)                                  Changes the Sample of a KDB of variables.
 *    int KV_merge(KDB *kdb1, KDB* kdb2, int replace)                         Merges two KDB of variables: kdb1 <- kdb1 + kdb2.            
 *    void KV_merge_del(KDB *kdb1, KDB *kdb2, int replace)                    Merges 2 KDB of variables, then deletes the second one.
 *    int KV_add(KDB* kdb, char* varname)                                               Adds a new variable in KV_WS. Fills it with IODE_NAN.
 *    double KV_get(KDB *kdb, int pos, int t, int mode)                       Gets VAR[t]  where VAR is the series in position pos in kdb. 
 *    void KV_set(KDB *kdb, int pos, int t, int mode, double new)          Sets VAR[t], where VAR is the series in position pos in kdb. 
 *    int KV_extrapolate(KDB *dbv, int method, Sample *smpl, char **vars)     Extrapolates variables on a selected Sample according to one of the available methods.
 *    KDB *KV_aggregate(KDB *dbv, int method, char *pattern, char *filename)  Creates a new KDB with variables created by aggregation based on variable names.
 *    void KV_init_values_1(double* val, int t, int method)                Extrapolates 1 value val[t] based on val[t], val[t-1] and a selected method.
 *   
 *    int KV_per_pos(Period* per2)                                            Retrieves the position of a Period in the current KV_WS sample.
 *    int KV_aper_pos(char* aper2)                                            Retrieves the position of a period in text format in the current KV_WS sample.  
 *    double KV_get_at_t(char*varname, int t)                                 Retrieves the value of varname[t] 
 *    double KV_get_at_per(char*varname, Period* per)                         Retrieves the value of varname[per] 
 *    double KV_get_at_aper(char*varname, char* aper)                         Retrieves the value of varname[aper]
 *    int KV_set_at_t(char*varname, int t, double val)                        Replaces the value of varname[t] by val.
 *    int KV_set_at_per(char*varname, Period* per, double val)                Replaces the value of varname[per] by val.
 *    int KV_set_at_aper(char*varname, char* aper, double val)                Replaces the value of varname[aper] by val.
 */
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/grep.h"
#include "api/objs/pack.h"
#include "api/objs/variables.h"


/**
 * Changes the Sample of a KDB of variables. Truncates the vars and/or add NaN values to fill the variables on the new sample.
 * 
 * @param [in, out] kdb          KDB*            pointer to a KDB of variables
 * @param [in]      new_sample   Sample* | NULL  new sample or NULL. If NULL or empty, no changes are made to the current WS
 * @return                       int             -1 if the kdb's sample and new_sample don't overlap or the resulting set is empty
 *                                               0 otherwise
 */
 
int KV_sample(KDB *kdb, Sample *new_sample)
{
    if(new_sample == nullptr || new_sample->nb_periods == 0) 
        return 0;

    Sample smpl;
    int start1 = 0;
    int start2 = 0;
    if(kdb->sample) 
    {
        smpl = kdb->sample->intersection(*new_sample);
        if(smpl.nb_periods > 0) 
        {
            start1 = smpl.start_period.difference(kdb->sample->start_period);
            start2 = smpl.start_period.difference(new_sample->start_period);
        }
    }

    if(kdb->sample)
        delete kdb->sample;
    kdb->sample = new Sample(*new_sample);

    char* ptr;
    int new_val;
    for(int i = 0 ; i < kdb->size(); i++) 
    {
        new_val = KV_alloc_var(kdb->sample->nb_periods);
        ptr = SW_getptr(new_val);
        if(kdb->get_handle(i) != 0) 
        {
            if(smpl.nb_periods > 0)
                memcpy((double *)(P_get_ptr(ptr, 0)) + start2,
                       KVVAL(kdb, i, start1),
                       sizeof(double) * smpl.nb_periods);
            SW_free(kdb->get_handle(i));
        }
        kdb->k_objs[i].o_val = new_val;
    }

    return 0;
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
    int i, start1, start2, pos, nb1;

    if(kdb2 == NULL) 
        return(0);

    if(!kdb1->sample)
    {
        if(kdb2->sample)
            kdb1->sample = new Sample(*kdb2->sample);
        else
        {
            kwarning("Cannot merge the 2 Variables databases: sample of both databases is empty");
            return -1;
        }
    }

    Sample smpl = kdb1->sample->intersection(*kdb2->sample);
    if(smpl.nb_periods > 0) 
    {
        start1 = smpl.start_period.difference(kdb1->sample->start_period); /* always >= 0 */
        start2 = smpl.start_period.difference(kdb2->sample->start_period); /* always >= 0 */
    }

    nb1 = kdb1->sample->nb_periods;
    for(i = 0; i < kdb2->size(); i++) 
    {
        pos = kdb1->index_of(kdb2->get_name(i));
        if(pos < 0) 
            pos = K_add(kdb1, (char*) kdb2->get_name(i).c_str(), NULL, &nb1);
        else if(!replace) 
            continue;

        if(pos >= 0 && kdb2->get_handle(i) != 0)
            memcpy((double *) KVVAL(kdb1, pos, start1),
                   (double *) KVVAL(kdb2, i, start2),
                   sizeof(double) * smpl.nb_periods);
    }

    return 0;
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

    if(kdb2 == NULL) 
        return;

    if(kdb2->size() == 0) 
        return;
    
    if(kdb1->size() == 0) 
    {
        if(kdb1->sample)
            KV_sample(kdb2, kdb1->sample);
        else if(kdb2->sample)
            kdb1->sample = new Sample(*kdb2->sample);
        kdb1->k_nb = kdb2->k_nb;
        kdb1->k_objs = kdb2->k_objs;
        kdb2->k_nb = 0;
        kdb2->k_objs = NULL;
        delete kdb2;
        kdb2 = nullptr;
        return;
    }

    KV_merge(kdb1, kdb2, replace);
    delete kdb2;
    kdb2 = nullptr;
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

    if(kdb->k_type != VARIABLES) 
        return(-1);
    
    // Create varname with NaN 
    pos = kdb->index_of(varname);
    if(pos < 0) 
    {
        nobs = kdb->sample->nb_periods;
        pos = K_add(kdb, varname, NULL, &nobs); // Set IODE_NAN if the new var
    }
    else 
    { 
        // Replaces all values by IODE_NAN 
        vptr = K_vptr(kdb, varname, 0);
        if(vptr == NULL) 
            return(-1);
        for(t = 0; t < kdb->sample->nb_periods; t++)
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
    Sample  *smpl;

    smpl = kdb->sample;
    if(!smpl || pos < 0 || pos >= kdb->size() || t < 0 || t >= smpl->nb_periods)
    {
        kwarning("Cannot get the value of the variable in position %d at index %d", pos, t);
        return IODE_NAN;
    }
    
    pernb = get_nb_periods_per_year((smpl->start_period).periodicity);

    switch(mode) 
    {
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
    Sample  *smpl;

    smpl = kdb->sample;
    if(!smpl || pos < 0 || pos >= kdb->size() || t < 0 || t >= smpl->nb_periods)
    {
        kwarning("Cannot set the value of the variable in position %d at index %d", pos, t);
        return;
    }
    
    pernb = get_nb_periods_per_year((smpl->start_period).periodicity);

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
 *  Extrapolates variables on a selected Sample according to one of the available methods. These extrapolation methods are
 *  described in the function KV_init_values(). 
 *  
 *  @param [in, out]    dbv    KDB*     KDB of variables on which the operation will be applied
 *  @param [in]         method int      identification of the extrapolation method (see K_init_values())
 *  @param [in]         smpl   Sample*  Sample on which the operation is to be carried out
 *  @param [in]         vars   char**   if not NULL, restricted list of variables to extrapolate
 *                                      if NULL, all variables in KDB will be modified
 *  @return                             0 on success,
 *                                      -1 if the Sample is invalid or the KDB is empty or dbv does not contain any variable 
 *                                          from the list vars
 *  @details 
 */
 
int KV_extrapolate(KDB *dbv, int method, Sample *smpl, char **vars)
{
    int         rc = -1, i, v, bt, t, at;
    double      *val;
    KDB         *edbv = NULL;

    bt = smpl->start_period.difference(dbv->sample->start_period);
    at = dbv->sample->end_period.difference(smpl->end_period);
    if(bt < 0 || at < 0) 
    {
        error_manager.append_error("WsExtrapolate : sample definition error");
        goto done;
    }

    if(vars == NULL || SCR_tbl_size((unsigned char**) vars) == 0) 
        edbv = dbv;
    else 
        edbv = K_refer(dbv, SCR_tbl_size((unsigned char**) vars), vars);
    // if(edbv == NULL) goto done; /* JMP 12-05-11 */
    if(edbv == NULL || edbv->size() == 0) 
        goto done; /* JMP 12-05-11 */

    for(v = 0; v < edbv->size(); v++) 
    {
        val = KVVAL(edbv, v, 0);
        for(i = 0, t = bt; i < smpl->nb_periods; i++, t++)
            KV_init_values_1(val, t, method);
    }
    rc = 0;

done:
    if(edbv != dbv)
    {
        delete edbv;
        edbv = nullptr;
    }
    
    return rc;
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
    double  *eval = NULL, *nval;
    KDB     *edbv = nullptr, *ndbv = nullptr;
    Sample  *smpl;

    if(filename == NULL || filename[0] == 0) 
        edbv = dbv;
    else
        edbv = K_interpret(VARIABLES, filename, 0);
    
    if(!edbv) 
        goto done;

    if(!edbv->sample)
        goto done;
    
    if(edbv->sample->nb_periods == 0)
        goto done;

    smpl = edbv->sample;
    nb_per = edbv->sample->nb_periods;
    eval = (double*) SCR_malloc(nb_per * sizeof(double));
    times = (int *) SCR_malloc(nbtimes * sizeof(int));

    ndbv = new KDB(VARIABLES, DB_STANDALONE);
    if(ndbv == NULL) 
        goto done;
    
    ndbv->sample = new Sample(*edbv->sample);
    for(epos = 0; epos < edbv->size(); epos++) 
    {
        strcpy(ename, edbv->get_name(epos).c_str());
        if(K_aggr(pattern, ename, nname) < 0) 
            continue;

        added = 1;
        npos = ndbv->index_of(nname);
        if(npos < 0) 
        {
            npos = K_add(ndbv, nname, NULL, &nb_per);
            if(npos > nbtimes - 1) 
            {
                times = (int *) SCR_realloc((char *) times, sizeof(int), nbtimes, nbtimes + 500);
                nbtimes += 500;
            }
            times[npos] = 1;

        }
        else 
        {
            added = 0;
            times[npos] += 1;
        }

        if(npos < 0) 
            goto done;

        memcpy(eval, KVVAL(edbv, epos, 0), nb_per * sizeof(double));
        nval = KVVAL(ndbv, npos, 0);
        for(t = 0; t < smpl->nb_periods; t++) 
        {
            if(added) 
                nval[t] = eval[t];
            else 
            {
                if(method == 1) 
                { /* m = 1 PROD */
                    if(IODE_IS_A_NUMBER(eval[t])) 
                        nval[t] *= eval[t];
                    else 
                        nval[t] = IODE_NAN;
                }
                else 
                { /* m != 1 SUM or MEAN */
                    if(IODE_IS_A_NUMBER(eval[t])) 
                        nval[t] += eval[t];
                    else 
                        nval[t] = IODE_NAN;
                }
            }
        }

    }

    if(method == 2) 
    {
        for(npos = 0; npos < ndbv->size(); npos++) 
        {
            nval = KVVAL(ndbv, npos, 0);
            for(t = 0; t < smpl->nb_periods; t++)
                if(IODE_IS_A_NUMBER(nval[t])) 
                    nval[t] /= times[npos];
        }
    }

done:
    if(edbv != dbv)
    {
        delete edbv;
        edbv = nullptr;
    }
    SCR_free(eval);
    SCR_free(times);
    return ndbv;
}


/**
 *  Retrieves the position of a Period in the current KV_WS sample.
 *  
 *  @param [in] Period* per2    Period whose position is searched
 *  @return     int             position in the current WS sample (starting at 0)
 *                              -1 if no WS is loaded or no sample is defined
 */
 
int KV_per_pos(Period* per2)
{
    Sample* smpl;
    int     diff;
    
    if(KV_WS == NULL) return(-1);
    smpl = KV_WS->sample;
    diff = per2->difference(smpl->start_period);
    return(diff);
}


/**
 *  Retrieves the position of a period in text format in the current KV_WS sample.
 *  
 *  Example : 
 *      Let the current sample be 2000Y1 2020Y1
 *      Then KV_aper_pos("2002Y1") = 2
 *  
 *  @param [in] char*   aper2 Period in text format (e.g.: "2001Y1")
 *  @return     int           position in the current WS sample (starting at 0)
 *                            or -1 if no WS is loaded or no sample is defined
 *  
 */
 
int KV_aper_pos(char* aper2)
{
    Period  *per2;
    int     pos;
    
    per2 = new Period(std::string(aper2));
    pos = KV_per_pos(per2);
    delete per2;
    per2 = nullptr;
    return pos;
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
    
    if(t < 0 || KV_WS->sample->nb_periods < t) 
        return IODE_NAN;
    return(var_ptr[t]);
}

/**
 *  Retrieves the value of varname[per] in the current KV_WS.
 *  
 *  Example : 
 *      KV_get_at_aper("A", "2002Y1")
 *  
 *  @param [in] char*   varname Variable name 
 *  @param [in] Period* per     Period to retrieve
 *  
 *  @return     double          value of varname[per]
 *                              IODE_NAN on error
 *  
 */
 
double KV_get_at_per(char*varname, Period* per)
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
 *  @param [in] char*   aper    Period in text format (e.g.: "2001Y1")
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

    if(t < 0 || KV_WS->sample->nb_periods < t) 
        return(-1);
    
    var_ptr[t] = val;
    return(0);
}

/**
 *  Replaces the value of varname[per] by val.
 *  
 *  @param [in] char*   varname Variable name 
 *  @param [in] Period* per     Period to modify
 *  @param [in] double  val     new value of varname[per]
 *  @return     int             0 on success, -1 on error
 *  
 */
 
int KV_set_at_per(char*varname, Period* per, double val)
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
 *  @param [in] char*   aper    Period in text format (e.g.: "2001Y1")
 *  @param [in] double  val     new value of varname[aper]
 *  @return     int             0 on success, -1 on error  
 */
 
int KV_set_at_aper(char*varname, char* aper, double val)
{
    int     t;
    
    t = KV_aper_pos(aper);
    return(KV_set_at_t(varname, t, val));
}


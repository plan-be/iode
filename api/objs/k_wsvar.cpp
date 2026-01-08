/**
 * @header4iode
 *
 * Functions acting on workspaces of variables.
 *
 *    int KV_sample(CKDBVariables* kdb, Sample *new_sample)                               Changes the Sample of a KDB of variables.
 *    int KV_merge(CKDBVariables* kdb1, KDB* kdb2, int replace)                           Merges two KDB of variables: kdb1 <- kdb1 + kdb2.            
 *    void KV_merge_del(CKDBVariables* kdb1, CKDBVariables* kdb2, int replace)                      Merges 2 KDB of variables, then deletes the second one.
 *    int KV_add(CKDBVariables* kdb, char* varname)                                       Adds a new variable in global_ws_var. Fills it with IODE_NAN.
 *    double KV_get(CKDBVariables* kdb, int pos, int t, int mode)                         Gets VAR[t]  where VAR is the series in position pos in kdb. 
 *    void KV_set(CKDBVariables* kdb, int pos, int t, int mode, double new)               Sets VAR[t], where VAR is the series in position pos in kdb. 
 *    int KV_extrapolate(CKDBVariables* dbv, int method, Sample *smpl, char* pattern)     Extrapolates variables on a selected Sample according to one of the available methods.
 *    CKDBVariables* KV_aggregate(CKDBVariables*dbv, int method, char *pattern, char *filename)    Creates a new KDB with variables created by aggregation based on variable names.
 *    void KV_init_values_1(double* val, int t, int method)                     Extrapolates 1 value val[t] based on val[t], val[t-1] and a selected method.
 *   
 *    int KV_per_pos(Period* per2)                                              Retrieves the position of a Period in the current global_ws_var sample.
 *    int KV_aper_pos(char* aper2)                                              Retrieves the position of a period in text format in the current global_ws_var sample.  
 *    double KV_get_at_t(char*varname, int t)                                   Retrieves the value of varname[t] 
 *    double KV_get_at_per(char*varname, Period* per)                           Retrieves the value of varname[per] 
 *    double KV_get_at_aper(char*varname, char* aper)                           Retrieves the value of varname[aper]
 *    int KV_set_at_t(char*varname, int t, double val)                          Replaces the value of varname[t] by val.
 *    int KV_set_at_per(char*varname, Period* per, double val)                  Replaces the value of varname[per] by val.
 *    int KV_set_at_aper(char*varname, char* aper, double val)                  Replaces the value of varname[aper] by val.
 */
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/grep.h"
#include "api/objs/pack.h"
#include "api/objs/variables.h"
#include "api/report/undoc/undoc.h"


/**
 * Changes the Sample of a KDB of variables. Truncates the vars and/or add NaN values to fill the variables on the new sample.
 * 
 * @param [in, out] kdb          KDB*            pointer to a KDB of variables
 * @param [in]      new_sample   Sample* | NULL  new sample or NULL. If NULL or empty, no changes are made to the current WS
 * @return                       int             -1 if the kdb's sample and new_sample don't overlap or the resulting set is empty
 *                                               0 otherwise
 */
 
int KV_sample(CKDBVariables* kdb, Sample *new_sample)
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

    int i = 0;
    char* ptr;
    SWHDL new_handle;
    for(auto it = kdb->k_objs.begin(); it != kdb->k_objs.end(); it++, i++)   
    {
        new_handle = (SWHDL) KV_alloc_var(kdb->sample->nb_periods);
        ptr = SW_getptr(new_handle);
        if(it->second != 0) 
        {
            if(smpl.nb_periods > 0)
                memcpy((double *)(P_get_ptr(ptr, 0)) + start2,
                       kdb->get_var_ptr(it->first, start1),
                       sizeof(double) * smpl.nb_periods);
            SW_free(it->second);
        }
        it->second = new_handle;
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
 *                                            0 if ok.
 */
int KV_merge(CKDBVariables* kdb1, CKDBVariables* kdb2, int replace)
{
    int start1, start2, nb1;

    if(!kdb1)
    {
        kwarning("Cannot merge into a NULL database");
        return -1;
    }

    if(!kdb2)
    {
        kwarning("Cannot merge from a NULL database");
        return -1;
    }

    if(kdb1->k_type != kdb2->k_type)
    {
        kwarning("Cannot merge 2 KDB of different types");
        return -1;
    }

    if(kdb1->k_type != VARIABLES) 
    {
        kwarning("KV_merge is only implemented for databases of type VARIABLES");
        return -1;
    }

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

    bool found;
    nb1 = kdb1->sample->nb_periods;
    size_t size = sizeof(double) * smpl.nb_periods;
    for(auto& [name, handle_2] : kdb2->k_objs) 
    {
        found = kdb1->contains(name);

        // replace == 0 means do not replace existing variables
        if(found && replace == 0)
            continue;
        
        if(!found)
            kdb1->set_obj(name, (double*) NULL);

        if(!kdb1->contains(name))
        {
            kerror(0, "Cannot merge variable %s from the second database into the first one", name.c_str());
            return -1;
        }

        if(handle_2 > 0)
            memcpy((double *) kdb1->get_var_ptr(name, start1),
                   (double *) kdb2->get_var_ptr(name, start2), size);
    }

    return 0;
}


/**
 *  Merges 2 KDB of variables, then deletes the second one (but not the IODE objects in memory).
 * 
 *  The resulting sample is kdb1 sample if it is defined. 
 *  If not, the sample of kdb2 is used.
 * 
 *  @param [in, out] kdb1       KDB*   ws that will contain the result of the merge.
 *  @param [in, out] kdb2       KDB*   ws to copy into kdb1
 *  @param [in]      replace    int    indicates if kdb2 homonyms must replace kdb1
 * 
 */

void KV_merge_del(CKDBVariables* kdb1, CKDBVariables* kdb2, int replace)
{
    if(!kdb1)
    {
        kwarning("Cannot merge into a NULL database");
        return;
    }

    if(!kdb2)
    {
        kwarning("Cannot merge from a NULL database");
        return;
    }

    if(kdb1->k_type != kdb2->k_type) 
    {
        kwarning("Cannot merge 2 KDB of different types");
        return;
    }

    if(kdb2->size() == 0) 
        return;
    
    if(kdb1->size() == 0) 
    {
        if(kdb1->k_type == VARIABLES)
        {
            if(kdb1->sample)
                KV_sample(kdb2, kdb1->sample);
            else if(kdb2->sample)
                kdb1->sample = new Sample(*kdb2->sample);
        }
        kdb1->k_objs = kdb2->k_objs;
        kdb2->clear(false);
        kdb2 = nullptr;
        return;
    }

    KV_merge(kdb1, kdb2, replace);
    kdb2->clear(false);
    kdb2 = nullptr;
}


/**
 *  Adds a new variable in global_ws_var. Fills it with IODE_NAN.
 *  If the variable already exists, replaces all values by IODE_NAN.
 *  
 *  @param [in] kdb     KDB*        KDB of variables
 *  @param [in] char*   varname     name of the new variable
 *  @return     int                 position of varname in the kdb
 */
int KV_add(CKDBVariables* kdb, char* varname)
{
    int      t, nobs;
    double   *vptr;

    if(!kdb)
    {
        kwarning("Cannot add a variable to a NULL database");
        return -1;
    }

    if(kdb->k_type != VARIABLES)
    {
        kwarning("KV_add is only implemented for databases of type VARIABLES");
        return -1;
    } 
    
    // Create varname with NaN 
    bool found = kdb->contains(varname);
    if(!found) 
    {
        nobs = kdb->sample->nb_periods;
        // Set IODE_NAN if the new var
        kdb->set_obj(varname, (double*) NULL);
    }
    else 
    { 
        // Replaces all values by IODE_NAN 
        vptr = K_vptr(kdb, varname, 0);
        if(vptr == NULL) 
            return -1;
        
        for(t = 0; t < kdb->sample->nb_periods; t++)
            vptr[t] = IODE_NAN;
    }
    
    int pos = kdb->index_of(varname);
    return pos;
}

/**
 * Gets VAR[t] where VAR is the series named 'name' in kdb. The value can be modified depending on the value of mode.
 *
 * @param [in] kdb       KDB*    KDB of variables
 * @param [in] name      string  name of the variable in the kdb
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
double KV_get(CKDBVariables* kdb, const std::string& name, int t, int mode)
{
    Sample* smpl = kdb->sample;
    if(!smpl || !kdb->contains(name) || t < 0 || t >= smpl->nb_periods)
    {
        kwarning("Cannot get the value of the variable name %s at period position %d", name.c_str(), t);
        return IODE_NAN;
    }
    
    int pernb = get_nb_periods_per_year((smpl->start_period).periodicity);

    double var;
    double* vt;
    switch(mode) 
    {
        case VAR_MODE_LEVEL :
            var = *kdb->get_var_ptr(name, t);
            break;
        case VAR_MODE_DIFF :
            pernb = 1;
        /* NO BREAK */
        case VAR_MODE_Y0Y_DIFF : 
            if(t < pernb) 
            {
                var = IODE_NAN;
                break;
            }
            vt = kdb->get_var_ptr(name);
            if(IODE_IS_A_NUMBER(vt[t]) && IODE_IS_A_NUMBER(vt[t - pernb]))
                var = vt[t] - vt[t - pernb];
            else 
                var = IODE_NAN;
            break;
        case VAR_MODE_GROWTH_RATE :
            pernb = 1;
        /* NO BREAK */
        case VAR_MODE_Y0Y_GROWTH_RATE :
            if(t < pernb) 
            {
                var = IODE_NAN;
                break;
            }
            vt = kdb->get_var_ptr(name);
            if(IODE_IS_A_NUMBER(vt[t]) && IODE_IS_A_NUMBER(vt[t - pernb]) && !IODE_IS_0(vt[t - pernb]))
                var = (vt[t]/vt[t - pernb] - 1) * 100.0;
            else 
                var = IODE_NAN;
            break;
    }

    return(var);
}


/**
 * Set VAR[t], where VAR is the series in position pos in kdb. The new value of VAR[t] depends on 
 * the parameter mode and the value new.
 * 
 * @param [in, out]  kdb       KDB*        KDB of variables
 * @param [in]       name      string      name of the variable in the kdb
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
 
void KV_set(CKDBVariables* kdb, const std::string& name, int t, int mode, double value)
{
    Sample* smpl = kdb->sample;
    if(!smpl || !kdb->contains(name) || t < 0 || t >= smpl->nb_periods)
    {
        kwarning("Cannot set the value of the variable named %s at period position %d", name.c_str(), t);
        return;
    }
    
    int pernb = get_nb_periods_per_year((smpl->start_period).periodicity);

    double* var = kdb->get_var_ptr(name);
    switch(mode) 
    {
        case VAR_MODE_LEVEL :
            var[t] = value;
            break;
        case VAR_MODE_DIFF :
            pernb = 1;
        /* NO BREAK */
        case VAR_MODE_Y0Y_DIFF :
            if(t < pernb)  
                break;
            if(IODE_IS_A_NUMBER(value) && IODE_IS_A_NUMBER(var[t - pernb])) 
                var[t] = var[t - pernb] + value;
            else 
                var[t] = IODE_NAN;
            break;
        case VAR_MODE_GROWTH_RATE :
            pernb = 1;
        /* NO BREAK */
        case VAR_MODE_Y0Y_GROWTH_RATE :
            if(t < pernb) 
                break;
            if(IODE_IS_A_NUMBER(value) && IODE_IS_A_NUMBER(var[t - pernb]))
                var[t] = (value/100.0 + 1.0) * var[t - pernb];
            else 
                var[t] = IODE_NAN;
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
 *  @param [in, out]    dbv      KDB*     KDB of variables on which the operation will be applied
 *  @param [in]         method   int      identification of the extrapolation method (see K_init_values())
 *  @param [in]         smpl     Sample*  Sample on which the operation is to be carried out
 *  @param [in]         pattern  char*    if not NULL, restricted list of variables to extrapolate
 *                                        if NULL, all variables in KDB will be modified
 *  @return                               0 on success,
 *                                        -1 if the Sample is invalid or the KDB is empty or dbv does not contain any variable 
 *                                        from the list vars
 */
 
int KV_extrapolate(CKDBVariables* dbv, int method, Sample* smpl, char* pattern)
{
    int bt = smpl->start_period.difference(dbv->sample->start_period);
    int at = dbv->sample->end_period.difference(smpl->end_period);
    if(bt < 0 || at < 0) 
    {
        error_manager.append_error("WsExtrapolate : sample definition error");
        return -1;
    }

    if(!dbv)
        return -1;

    if(dbv->size() == 0) 
    {
        error_manager.append_error("WsExtrapolate : database is empty");
        return -1;
    }
    
    std::string pattern_str = (pattern != NULL) ? std::string(pattern) : std::string("*");
    if(pattern_str.empty())
        pattern_str = "*";
    std::vector<std::string> var_list = dbv->filter_names(pattern);

    int i, t;
    double* val;
    for(const std::string& name : var_list) 
    {
        val = dbv->get_var_ptr(name);
        for(i = 0, t = bt; i < smpl->nb_periods; i++, t++)
            KV_init_values_1(val, t, method);
    }
    
    return 0;
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

CKDBVariables* KV_aggregate(CKDBVariables* dbv, int method, char *pattern, char *filename)
{
    int     nb_per, res, npos, added, *times, nbtimes = 500;
    double* eval = NULL, *nval;
    Sample* smpl;
    char    c_nname[K_MAX_NAME + 1];
    std::string nname;
    CKDBVariables* ndbv = nullptr;

    CKDBVariables* edbv = nullptr;
    if(filename == NULL || filename[0] == 0) 
        edbv = dbv;
    else
    {
        edbv = new CKDBVariables(false);
        bool success = edbv->load(std::string(filename));
        if(!success)
            goto done;
    }
    
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

    ndbv = new CKDBVariables(false);
    if(!ndbv) 
        goto done;
    
    ndbv->sample = new Sample(*edbv->sample);

    for(auto& [ename, handle] : edbv->k_objs) 
    {
        res = K_aggr(pattern, (char*) ename.c_str(), c_nname);
        if(res < 0) 
            continue;
    
        added = 1;
        nname = std::string(c_nname);
        npos = ndbv->index_of(nname);
        if(npos < 0) 
        {
            ndbv->set_obj(c_nname, (double*) NULL);
            npos = ndbv->index_of(nname);
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

        memcpy(eval, edbv->get_var_ptr(ename), nb_per * sizeof(double));
        nval = ndbv->get_var_ptr(nname);
        for(int t = 0; t < smpl->nb_periods; t++) 
        {
            if(added) 
                nval[t] = eval[t];
            else 
            {
                /* m = 1 PROD */
                if(method == 1) 
                {
                    if(IODE_IS_A_NUMBER(eval[t])) 
                        nval[t] *= eval[t];
                    else 
                        nval[t] = IODE_NAN;
                }
                /* m != 1 SUM or MEAN */
                else
                {
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
        for(auto& [name, handle] : ndbv->k_objs) 
        {
            nval = ndbv->get_var_ptr(name);
            for(int t = 0; t < smpl->nb_periods; t++)
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
 *  Retrieves the position of a Period in the current global_ws_var sample.
 *  
 *  @param [in] Period* per2    Period whose position is searched
 *  @return     int             position in the current WS sample (starting at 0)
 *                              -1 if no WS is loaded or no sample is defined
 */
 
int KV_per_pos(Period* per2)
{   
    if(!global_ws_var.get()) 
        return(-1);
    
    Sample* smpl = global_ws_var->sample;
    int diff = per2->difference(smpl->start_period);
    return(diff);
}


/**
 *  Retrieves the position of a period in text format in the current global_ws_var sample.
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
 *  Retrieves the value of varname[t] in the current global_ws_var.
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
    double* var_ptr = global_ws_var->get_var_ptr(varname);
    if(var_ptr == NULL) 
        return(IODE_NAN);
    
    if(t < 0 || global_ws_var->sample->nb_periods < t) 
        return IODE_NAN;
    return(var_ptr[t]);
}

/**
 *  Retrieves the value of varname[per] in the current global_ws_var.
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
 *  Retrieves the value of varname[aper] in the current global_ws_var.
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
    double* var_ptr = global_ws_var->get_var_ptr(varname);
    if(var_ptr == NULL) 
        return(-1);

    if(t < 0 || global_ws_var->sample->nb_periods < t) 
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


double* CKDBVariables::get_obj(const SWHDL handle) const
{    
    return (double*) P_get_ptr(SW_getptr(handle), 0);
}

double* CKDBVariables::get_obj(const std::string& name) const
{
    SWHDL handle = this->get_handle(name);
    if(handle == 0)  
        throw std::invalid_argument("IODE Variable with name '" + name + "' not found.");
    
    return get_obj(handle);
}

bool CKDBVariables::set_obj(const std::string& name, const double* value)
{
    char* pack = NULL;
    if(!this->sample || this->sample->nb_periods == 0)
    {
        std::string error_msg = "Cannot set variable object '" + name + "' because ";
        error_msg += "the variable database has no sample defined";
        throw std::runtime_error(error_msg);
    }
    
    int nb_obs = this->sample->nb_periods;
    K_vpack(&pack, (double*) value, (int*) &nb_obs);
    bool success = set_packed_object(name, pack);
    if(!success)
    {
        std::string error_msg = "Failed to set variable object '" + name + "'";
        kwarning(error_msg.c_str());
    }
    return success;
}

bool CKDBVariables::set_obj(const std::string& name, const std::vector<double>& value)
{
    return set_obj(name, value.data());
}

bool CKDBVariables::grep_obj(const std::string& name, const SWHDL handle, 
    const std::string& pattern, const bool ecase, const bool forms, const bool texts, 
    const char all) const
{
    return false;
}

char* CKDBVariables::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    char* obj = (char*) SCR_stracpy((unsigned char*) "Not yet implemented") ;
    return obj;
}

bool CKDBVariables::print_obj_def(const std::string& name)
{
    Sample* smpl = this->sample;
    if(!smpl || smpl->nb_periods == 0) 
    {
        std::string msg = "Cannot print the variable '" + name + "' because ";
        msg += "the variable database has no sample defined";
        kwarning((char*) msg.c_str());
        return false;
    }

    double* val = this->get_var_ptr(name); 
    if(val == NULL) 
        return false;
    
    W_printfRepl((char*) "&1L%s ", name.c_str());
    for(int j = 0 ; j < smpl->nb_periods; j++, val++) 
    {
        W_printfRepl((char*) "&1D");
        B_PrintVal(*val);
    }

    W_printf((char*) "\n");
    return true;
}

void CKDBVariables::update_reference_db()
{
    K_RWS[this->k_type][0] = new CKDBVariables(this, "*");      
}

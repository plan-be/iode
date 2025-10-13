#pragma once

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"

/*----------------------- TYPEDEF ----------------------------*/

// VAR = Variable (pointer to a vector of double)
using VAR = double*;

/*----------------------- DEFINE ----------------------------*/

#define LTOH_LIN    'L'
#define LTOH_CS     'C'
#define LTOH_STEP   'S'

/*----------------------- ENUMS ----------------------------*/

enum IodeVarMode
{
    VAR_MODE_LEVEL,              //< no modification             x[t]
    VAR_MODE_DIFF,               //< difference on one period    (x[t]-x[t-1])
    VAR_MODE_GROWTH_RATE,        //< growth rate on one period   (x[t]/x[t-1] - 1)*100
    VAR_MODE_Y0Y_DIFF,           //< difference on one year      (x[t]-x[t-{nb sub periods}])
    VAR_MODE_Y0Y_GROWTH_RATE     //< growth rate on one year     (x[t]/x[t-{nb sub periods}] - 1) * 100
};

const static int IODE_NB_VAR_MODES = 5;

enum VariablesInitialization
{
    VAR_INIT_TM1,
    VAR_INIT_TM1_A,
    VAR_INIT_EXTRA,
    VAR_INIT_EXTRA_A,
    VAR_INIT_ASIS,
    VAR_INIT_TM1_NA,
    VAR_INIT_EXTRA_NA
};

enum IodeHighToLow
{
    HTOL_LAST,
    HTOL_MEAN,
    HTOL_SUM 
};

enum IodeLowToHigh
{
    LTOH_STOCK,
    LTOH_FLOW
};

/*----------------------- FUNCS ----------------------------*/

double *K_vval(KDB *, int, int);
double *K_vptr(KDB *, char*, int);

inline double* KVVAL(KDB* kdb, int pos, int t)
{
    return K_vval(kdb, pos, t);
}  

inline double* KVPTR(KDB* kdb, char* name)
{
    return K_vptr(kdb, name, 0);
}

/* k_wsvar.c */
int KV_sample(KDB *,Sample *);
int KV_merge(KDB *,KDB *,int );
void KV_merge_del(KDB *,KDB *,int );
int KV_add(KDB* kdb, char* varname);
double KV_get(KDB *,int ,int ,int );
void KV_set(KDB *,int ,int ,int ,double );
int KV_extrapolate(KDB *,int ,Sample *,char **);
KDB *KV_aggregate(KDB *,int ,char *,char *);
void KV_init_values_1(double* val, int t, int method);
//int KV_GetSmpl(Sample *,char *);
double KV_get_at_t(char*varname, int t);
double KV_get_at_per(char*varname, Period* per);
double KV_get_at_aper(char*varname, char* aper);
int KV_set_at_t(char*varname, int t, double val);
int KV_set_at_per(char*varname, Period* per, double val);
int KV_set_at_aper(char*varname, char* aper, double val);
        
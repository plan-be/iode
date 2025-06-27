#pragma once

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/kdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------- TYPEDEF ----------------------------*/

// VAR = Variable (pointer to a vector of double)
typedef double     *VAR;

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

extern double *K_vval(KDB *, int, int);
extern double *K_vptr(KDB *, char*, int);

/* k_wsvar.c */
extern int KV_sample(KDB *,SAMPLE *);
extern int KV_merge(KDB *,KDB *,int );
extern void KV_merge_del(KDB *,KDB *,int );
extern int KV_add(KDB* kdb, char* varname);
extern double KV_get(KDB *,int ,int ,int );
extern void KV_set(KDB *,int ,int ,int ,double );
extern int KV_extrapolate(KDB *,int ,SAMPLE *,char **);
extern KDB *KV_aggregate(KDB *,int ,char *,char *);
void KV_init_values_1(double* val, int t, int method);
//extern int KV_GetSmpl(SAMPLE *,char *);
extern double KV_get_at_t(char*varname, int t);
extern double KV_get_at_per(char*varname, PERIOD* per);
extern double KV_get_at_aper(char*varname, char* aper);
extern int KV_set_at_t(char*varname, int t, double val);
extern int KV_set_at_per(char*varname, PERIOD* per, double val);
extern int KV_set_at_aper(char*varname, char* aper, double val);

/*----------------------- MACROS ----------------------------*/

#define KSMPL(kdb)          ((SAMPLE *) (kdb)->k_data)
#define KVVAL(kdb, pos, t)  K_vval(kdb, pos, t)
#define KVPTR(name)         K_vptr(KV_WS, name, 0)

/*----------------------- GLOBALS ----------------------------*/

extern int KVAR_MODE;
extern int KVAR_NDEC;
extern int KVAR_START;

#ifdef __cplusplus
}
#endif

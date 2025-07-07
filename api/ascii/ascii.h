#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/utils/yy.h"
#include "api/objs/kdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------- GLOBALS ------------------ */

extern KDB*  (*K_load_asc[])(char*);
extern int   (*K_save_asc[])(KDB*, char*);
extern int   (*K_save_csv[])(KDB*, char*, SAMPLE*, char**);

extern char* KV_CSV_SEP;
extern char* KV_CSV_DEC;
extern char* KV_CSV_NAN;
extern char* KV_CSV_AXES;
extern int   KV_CSV_NBDEC;

/*---------------- FUNCS ------------------ */

/* k_cccmt.cpp */
KDB *KC_load_asc(char *);
int KC_save_asc(KDB *,char *);
int KC_save_csv(KDB *, char *);

inline int _KC_save_csv_(KDB *kdb, char *filename, SAMPLE *sample, char **varlist)
{
    return KC_save_csv(kdb, filename);
}

/* k_ccscl.cpp */
KDB *KS_load_asc(char *);
int KS_save_asc(KDB *,char *);
int KS_save_csv(KDB *, char *);

inline int _KS_save_csv_(KDB *kdb, char *filename, SAMPLE *sample, char **varlist)
{
    return KS_save_csv(kdb, filename);
}

/* k_ccidt.cpp */
KDB *KI_load_asc(char *);
int KI_save_asc(KDB *,char *);
int KI_save_csv(KDB *, char *);

inline int _KI_save_csv_(KDB *kdb, char *filename, SAMPLE *sample, char **varlist)
{
    return KI_save_csv(kdb, filename);
}

/* k_cclst.cpp */
KDB *KL_load_asc(char *);
int KL_save_asc(KDB *,char *);
int KL_save_csv(KDB *, char *);

inline int _KL_save_csv_(KDB *kdb, char *filename, SAMPLE *sample, char **varlist)
{
    return KL_save_csv(kdb, filename);
}

/* k_cceqs.cpp */
KDB *KE_load_asc(char *);
int KE_save_asc(KDB *,char *);
int KE_save_csv(KDB *, char *);

inline int _KE_save_csv_(KDB *kdb, char *filename, SAMPLE *sample, char **varlist)
{
    return KE_save_csv(kdb, filename);
}

/* k_cctbl.cpp */
KDB *KT_load_asc(char *);
int KT_save_asc(KDB *,char *);
int KT_save_csv(KDB *, char *);

inline int _KT_save_csv_(KDB *kdb, char *filename, SAMPLE *sample, char **varlist)
{
    return KT_save_csv(kdb, filename);
}

/* k_ccvar.cpp */
KDB *KV_load_str(char *);
KDB *KV_load_asc(char *);
int KV_save_asc(KDB *,char *);
int KV_save_csv(KDB *, char *, SAMPLE *, char **);

inline int compare(const void* a, const void* b)
{
    return YY_strcmp((const char*) a, (const char*) b);
}

inline int compare_eqs(const void* a, const void* b)
{
    return K_compare((YYKEYS*) a, (YYKEYS*) b);
}

#ifdef __cplusplus
}
#endif

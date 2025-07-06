#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/kdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------- GLOBALS ------------------ */

extern  KDB   *(*K_load_asc[])();
extern  int   (*K_save_asc[])();
extern  int   (*K_save_csv[])();

extern char *KV_CSV_SEP;
extern char *KV_CSV_DEC;
extern char *KV_CSV_NAN;
extern char *KV_CSV_AXES;
extern int  KV_CSV_NBDEC;

/*---------------- FUNCS ------------------ */

/* k_cccmt.c */
extern KDB *KC_load_asc(char *);
extern int KC_save_asc(KDB *,char *);
extern int KC_save_csv(KDB *, char *);

/* k_ccscl.c */
extern KDB *KS_load_asc(char *);
extern int KS_save_asc(KDB *,char *);
extern int KS_save_csv(KDB *, char *);

/* k_ccidt.c */
extern KDB *KI_load_asc(char *);
extern int KI_save_asc(KDB *,char *);
extern int KI_save_csv(KDB *, char *);

/* k_cclst.c */
extern KDB *KL_load_asc(char *);
extern int KL_save_asc(KDB *,char *);
extern int KL_save_csv(KDB *, char *);

/* k_cceqs.c */
extern KDB *KE_load_asc(char *);
extern int KE_save_asc(KDB *,char *);
extern int KE_save_csv(KDB *, char *);

/* k_cctbl.c */
extern KDB *KT_load_asc(char *);
extern int KT_save_asc(KDB *,char *);
extern int KT_save_csv(KDB *, char *);

/* k_ccvar.c */
extern KDB *KV_load_str(char *);
extern KDB *KV_load_asc(char *);
extern int KV_save_asc(KDB *,char *);
extern int KV_save_csv(KDB *, char *, SAMPLE *, char **);

#ifdef __cplusplus
}
#endif

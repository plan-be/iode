#pragma once

#include "scr4/s_swap.h"        // SWHDL

#include "api/constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------- STRUCTS ----------------------------*/

typedef struct  _kobj_ {
    SWHDL       o_val;          // Handle of the object in the scr4/swap memory -> to be passed to SW_getptr()
    ONAME       o_name;         // name of the object
    char        o_pad[3];
} KOBJ;

typedef struct _kdb_ {
    KOBJ        *k_objs;                // map <position in the memory, object name>
	long        k_nb;                   // number of objects in the database
    short       k_type;                 // type of the object: COMMENTS, EQUATIONS, ..., VARIABLES
    short       k_mode;                 // case of the object name: UPPER_CASE, LOWER_CASE or ASIS_CASE 
    char        k_arch[LMAGIC];         // not used
    char        k_magic[LMAGIC];        // not used
    OFNAME       k_oname;               // not used : old version of filename replaced since 6.44 by k_nameptr (allocated)
    char        k_desc[K_MAX_DESC];     // short file content description
    char        k_data[K_MAX_DESC];     // Sample if Variables database
    char        k_compressed;           // are the objects compressed in the file ? (LZH method, slow)
    char        k_reserved[59];         // not used (NOTE: decreased by 4 bytes in version 6.44 for k_nameptr)
    char        *k_nameptr;             // filepath to the database file
} KDB;

/*----------------------- GLOBALS ----------------------------*/

extern KDB      *K_WS[7];
extern KDB      *K_RWS[7][5];
extern int      K_PWS[7];
extern int      K_WARN_DUP;
extern int      K_SECRETSEP;

/*----------------------- DEFINE ----------------------------*/

#define KC_WS   K_WS[COMMENTS]
#define KE_WS   K_WS[EQUATIONS]
#define KI_WS   K_WS[IDENTITIES]
#define KL_WS   K_WS[LISTS]
#define KS_WS   K_WS[SCALARS]
#define KT_WS   K_WS[TABLES]
#define KV_WS   K_WS[VARIABLES]

#define KC_RWS   K_RWS[COMMENTS][K_PWS[COMMENTS]]
#define KE_RWS   K_RWS[EQUATIONS][K_PWS[EQUATIONS]]
#define KI_RWS   K_RWS[IDENTITIES][K_PWS[IDENTITIES]]
#define KL_RWS   K_RWS[LISTS][K_PWS[LISTS]]
#define KS_RWS   K_RWS[SCALARS][K_PWS[SCALARS]]
#define KT_RWS   K_RWS[TABLES][K_PWS[TABLES]]
#define KV_RWS   K_RWS[VARIABLES][K_PWS[VARIABLES]]

/*----------------------- MACROS ----------------------------*/

#define KARCH(kdb)   ((kdb)->k_arch)
#define KMAGIC(kdb)  ((kdb)->k_magic)
#define KTYPE(kdb)   ((kdb)->k_type)
#define KMODE(kdb)   ((kdb)->k_mode)
#define KNAMEPTR(kdb)((kdb)->k_nameptr) // 6.44
#define KDESC(kdb)   ((kdb)->k_desc)
#define KDATA(kdb)   ((kdb)->k_data)
#define KFREE(kdb)   ((kdb)->k_free)
#define KNB(kdb)     ((kdb)->k_nb)
#define KOBJS(kdb)   ((kdb)->k_objs)

#define KONAME(kdb, pos)    ((kdb)->k_objs[pos].o_name)                 // name of the object
#define KSOVAL(kdb, pos)    ((kdb)->k_objs[pos].o_val)                  // handle of the object in the scr4/swap memory
#define KGOVAL(kdb, pos)    (SW_getptr((kdb)->k_objs[pos].o_val))       // pointer to the object in the scr4/swap (as a char*)

#define KOVAL(kdb, pos)     K_oval0(kdb, pos)

/*----------------------- GLOBALS ----------------------------*/

extern char    k_magic[][LMAGIC];
extern char    k_ext[][4];

/*----------------------- FUNCS ----------------------------*/

/* k_kdb.c */
extern void K_sort(KDB* kdb);
extern KDB *K_init_kdb(int ,char *);
extern char *K_get_kdb_nameptr(KDB *kdb);
extern void K_set_kdb_name(KDB *kdb, U_ch *filename);
extern void K_set_kdb_fullpath(KDB *kdb, U_ch *filename);
extern KDB *K_create(int ,int );
extern int K_free(KDB *);
extern int K_clear(KDB *);
extern int K_free_kdb(KDB *);
extern int K_merge(KDB* kdb1, KDB* kdb2, int replace);
extern KDB* K_refer(KDB* kdb, int nb, char** names);
extern KDB *K_quick_refer(KDB *, char **names);

/* k_val.c */
extern char *K_oval(KDB *,int ,int );
extern char *K_oval0(KDB *,int );
extern char *K_oval1(KDB *,int );

extern char *K_optr(KDB *,char* ,int );
extern char *K_optr0(KDB *,char* );
extern char *K_optr1(KDB *,char* );

// Estimation tests by equation name
extern double K_etest(KDB* kdb, char*name, int test_nb);
extern double K_e_stdev (KDB* kdb, char*name);
extern double K_e_meany (KDB* kdb, char*name);
extern double K_e_ssres (KDB* kdb, char*name);
extern double K_e_stderr(KDB* kdb, char*name);
extern double K_e_fstat (KDB* kdb, char*name);
extern double K_e_r2    (KDB* kdb, char*name);
extern double K_e_r2adj (KDB* kdb, char*name);
extern double K_e_dw    (KDB* kdb, char*name);
extern double K_e_loglik(KDB* kdb, char*name);

// Values of scalars by name
extern double K_s_get_info(KDB* kdb, char*name, int info_nb);
extern double K_s_get_value (KDB* kdb, char*name);
extern double K_s_get_relax (KDB* kdb, char*name);
extern double K_s_get_stderr(KDB* kdb, char*name);
extern double K_s_get_ttest (KDB* kdb, char*name);
extern int K_s_set_info(KDB* kdb, char*name, int info_nb, double val);
extern int K_s_set_value (KDB* kdb, char*name, double val);
extern int K_s_set_relax (KDB* kdb, char*name, double val);
extern int K_s_set_stderr(KDB* kdb, char*name, double val);

#ifdef __cplusplus
}
#endif

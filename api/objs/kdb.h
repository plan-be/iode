#pragma once

#include "scr4/s_swap.h"        // SWHDL
#include "api/constants.h"

#include <string>


enum IodeDatabaseType
{
    DB_GLOBAL,          //< K_WS[iode_type]
    DB_DEEP_COPY,       //< deep copy of a global KDB (useful when working with estimation of block of eqs in the GUI)
    DB_SHALLOW_COPY     //< shallow copy (useful when working on subset in the GUI)
};


/*----------------------- STRUCTS ----------------------------*/

struct  KOBJ 
{
    SWHDL       o_val;          // Handle of the object in the scr4/swap memory -> to be passed to SW_getptr()
    ONAME       o_name;         // name of the object
};

struct KDB 
{
    KOBJ*       k_objs;                 // map <position in the memory, object name>
	long        k_nb;                   // number of objects in the database
    short       k_type;                 // type of the object: COMMENTS, EQUATIONS, ..., VARIABLES
    short       k_mode;                 // case of the object name: UPPER_CASE, LOWER_CASE or ASIS_CASE 
    std::string k_arch;                 // processor architecture on which the file has been created/saved/loaded
    std::string description;            // short description of the content of the database
    char        k_data[K_MAX_DESC];     // Sample if Variables database
    char        k_compressed;           // are the objects compressed in the file ? (LZH method, slow)
    char        k_db_type;              // type of database: DB_NORMAL (default), DB_DEEP_COPY, DB_SHALLOW_COPY
    std::string filepath;               // filepath to the database file
};

/*----------------------- GLOBALS ----------------------------*/

inline KDB*     K_WS[7] = { NULL };             // Current workspaces
inline KDB*     K_RWS[7][5] = {{ NULL }};       // Currently loaded workspaces (for printing and identity execution)
inline int      K_PWS[7] = { 0 };               // ??? TODO: check if still in use
inline int      K_WARN_DUP = 0;                 // If null, adding an existing object name in a KDB does not trigger 
                                                // an error (used in K_add_entry())
inline int      K_SECRETSEP = '#';              // pour les macros pushed A#n in reports

/**
 * k_ext[][4] : extensions of IODE filenames.
 * 
 *   - cmt..var = IODE objects internal format
 *   - ac..av = IODE objects ascii format
 *   - next extensions : other IODE files
 */
inline char k_ext[][4] = 
{
    "cmt", // 0 = FILE_COMMENTS
    "eqs", // 1 = FILE_EQUATIONS
    "idt", // ... 
    "lst",
    "scl",
    "tbl",
    "var",
    "ooo", // 7 = IODE_NB_TYPES

    "ac",  // 8
    "ae",
    "ai",
    "al",
    "as",
    "at",
    "av",
    "",    // 15

    "rep", // 16
    "a2m",
    "agl",
    "prf",
    "dif",
    "mif",
    "rtf",
    "ps",
    "asc",
    "txt",
    "csv",  // 26 = FILE_CSV // JMP 2-3-2016  -> TODO: pas très propre, à modifier

    "xxx"
};

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

#define KTYPE(kdb)   ((kdb)->k_type)
#define KMODE(kdb)   ((kdb)->k_mode)
#define KNB(kdb)     ((kdb)->k_nb)
#define KOBJS(kdb)   ((kdb)->k_objs)

#define KONAME(kdb, pos)    ((kdb)->k_objs[pos].o_name)                 // name of the object
#define KSOVAL(kdb, pos)    ((kdb)->k_objs[pos].o_val)                  // handle of the object in the scr4/swap memory
#define KGOVAL(kdb, pos)    (SW_getptr((kdb)->k_objs[pos].o_val))       // pointer to the object in the scr4/swap (as a char*)

#define KOVAL(kdb, pos)     K_oval0(kdb, pos)

/*----------------------- FUNCS ----------------------------*/

/* k_kdb.c */
void K_sort(KDB* kdb);
KDB *K_init_kdb(int ,char *);
void K_set_kdb_fullpath(KDB *kdb, U_ch *filename);
KDB *K_create(int ,int );
int K_free(KDB *);
int K_clear(KDB *);
int K_free_kdb(KDB *);
int K_merge(KDB* kdb1, KDB* kdb2, int replace);
KDB* K_refer(KDB* kdb, int nb, char** names);
KDB *K_quick_refer(KDB *, char **names);

/* k_val.c */
char *K_oval(KDB *,int ,int );
char *K_oval0(KDB *,int );
char *K_oval1(KDB *,int );

char *K_optr(KDB *,char* ,int );
char *K_optr0(KDB *,char* );
char *K_optr1(KDB *,char* );

// Estimation tests by equation name
double K_etest(KDB* kdb, char*name, int test_nb);
double K_e_stdev (KDB* kdb, char*name);
double K_e_meany (KDB* kdb, char*name);
double K_e_ssres (KDB* kdb, char*name);
double K_e_stderr(KDB* kdb, char*name);
double K_e_fstat (KDB* kdb, char*name);
double K_e_r2    (KDB* kdb, char*name);
double K_e_r2adj (KDB* kdb, char*name);
double K_e_dw    (KDB* kdb, char*name);
double K_e_loglik(KDB* kdb, char*name);

// Values of scalars by name
double K_s_get_info(KDB* kdb, char*name, int info_nb);
double K_s_get_value (KDB* kdb, char*name);
double K_s_get_relax (KDB* kdb, char*name);
double K_s_get_stderr(KDB* kdb, char*name);
double K_s_get_ttest (KDB* kdb, char*name);
int K_s_set_info(KDB* kdb, char*name, int info_nb, double val);
int K_s_set_value (KDB* kdb, char*name, double val);
int K_s_set_relax (KDB* kdb, char*name, double val);
int K_s_set_stderr(KDB* kdb, char*name, double val);

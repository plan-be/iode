#pragma once

#include "api/constants.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"

#include <string>
#include <variant>
#include <array>
#include <map>
#include <iostream>


/*----------------------- STRUCTS ----------------------------*/

template<class T> class IodeDatabase: public std::map<std::string, T> 
{
    short       k_type;                 // type of the object: COMMENTS, EQUATIONS, ..., VARIABLES
    short       k_mode;                 // case of the object name: UPPER_CASE, LOWER_CASE or ASIS_CASE 
    char        k_arch[LMAGIC];         // not used
    OFNAME      k_oname;                // not used : old version of filename replaced since 6.44 by k_nameptr (allocated)
    char        k_desc[K_MAX_DESC];     // short file content description
    char        k_data[K_MAX_DESC];     // Sample if Variables database
    char        k_compressed;           // are the objects compressed in the file ? (LZH method, slow)
    char        k_reserved[59];         // not used (NOTE: decreased by 4 bytes in version 6.44 for k_nameptr)
    char        *k_nameptr;             // filepath to the database file
};

class CommentsDatabase : public IodeDatabase<CMT> {};
class EquationsDatabase : public IodeDatabase<EQ> {};
class IdentitiesDatabase : public IodeDatabase<IDT> {};
class ListsDatabase : public IodeDatabase<LIS> {};
class ScalarsDatabase : public IodeDatabase<SCL> {};
class TablesDatabase : public IodeDatabase<TBL> {};
class VariablesDatabase : public IodeDatabase<VAR> {};

/*----------------------- GLOBALS ----------------------------*/

using IodeDatabaseVariant = std::variant<CommentsDatabase, EquationsDatabase, IdentitiesDatabase,
                                         ListsDatabase, ScalarsDatabase, TablesDatabase, VariablesDatabase>;

// Current workspaces
inline std::array<IodeDatabaseVariant, IODE_NB_TYPES> K_WS;
// Currently loaded workspaces (for printing and identity execution)
inline std::array<std::array<IodeDatabaseVariant, IODE_NB_TYPES>, 5> K_RWS;
// table of "current" K_RWS number. Set to 0 and never used (yet)
inline std::array<int, IODE_NB_TYPES> K_PWS = { 0 };
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
inline std::vector<std::string> k_ext = 
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
};

/*----------------------- DEFINE ----------------------------*/

#define KC_WS   static_cast<CommentsDatabase>(K_WS[COMMENTS])
#define KE_WS   static_cast<EquationsDatabase>(K_WS[EQUATIONS])
#define KI_WS   static_cast<IdentitiesDatabase>(K_WS[IDENTITIES])
#define KL_WS   static_cast<ListsDatabase>(K_WS[LISTS])
#define KS_WS   static_cast<ScalarsDatabase>(K_WS[SCALARS])
#define KT_WS   static_cast<TablesDatabase>(K_WS[TABLES])
#define KV_WS   static_cast<VariablesDatabase>(K_WS[VARIABLES])

#define KC_RWS   static_cast<CommentsDatabase>(K_RWS[K_PWS[COMMENTS]][COMMENTS])
#define KE_RWS   static_cast<EquationsDatabase>(K_RWS[K_PWS[EQUATIONS]][EQUATIONS])
#define KI_RWS   static_cast<IdentitiesDatabase>(K_RWS[K_PWS[IDENTITIES]][IDENTITIES])
#define KL_RWS   static_cast<ListsDatabase>(K_RWS[K_PWS[LISTS]][LISTS])
#define KS_RWS   static_cast<ScalarsDatabase>(K_RWS[K_PWS[SCALARS]][SCALARS])
#define KT_RWS   static_cast<TablesDatabase>(K_RWS[K_PWS[TABLES]][TABLES])
#define KV_RWS   static_cast<VariablesDatabase>(K_RWS[K_PWS[VARIABLES]][VARIABLES])

/*----------------------- MACROS ----------------------------*/

/*
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
*/

/*----------------------- FUNCS ----------------------------*/

/* k_kdb.c */
void K_sort(KDB* kdb);
KDB *K_init_kdb(int ,char *);
char *K_get_kdb_nameptr(KDB *kdb);
void K_set_kdb_name(KDB *kdb, U_ch *filename);
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

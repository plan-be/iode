#pragma once

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/lec/lec.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------- DEFINE ----------------------------*/

#define EQS_NBTESTS     20

/*----------------------- ENUMS ----------------------------*/

enum IodeEquationMethod
{
    EQ_LSQ,
    EQ_ZELLNER,
    EQ_INSTRUMENTAL,
    EQ_GLS,
    EQ_MAX_LIKELIHOOD
};

const static int IODE_NB_EQ_METHODS = 5;

enum IodeEquationTest
{
    EQ_CORR,
    EQ_STDEV,
    EQ_MEANY,
    EQ_SSRES,
    EQ_STDERR,
    EQ_STDERRP,
    EQ_FSTAT,
    EQ_R2,
    EQ_R2ADJ,
    EQ_DW,
    EQ_LOGLIK,
};

const static int IODE_NB_EQ_TESTS = 11;

enum IodeEquationAscii
{
    EQ_ASCII_BLK,
    EQ_ASCII_CMT,
    EQ_ASCII_INSTR,
    EQ_ASCII_METH,
    EQ_ASCII_SMPL,
    EQ_ASCII_LSQ,
    EQ_ASCII_ZEL,
    EQ_ASCII_INF,
    EQ_ASCII_GLS,
    EQ_ASCII_DATE,
    EQ_ASCII_OPEN,
    EQ_ASCII_CLOSE,
    EQ_ASCII_STDEV,
    EQ_ASCII_MEANY,
    EQ_ASCII_SSRES,
    EQ_ASCII_STDERR,
    EQ_ASCII_FSTAT,
    EQ_ASCII_R2,
    EQ_ASCII_R2ADJ,
    EQ_ASCII_DW,
    EQ_ASCII_LOGLIK,
    EQ_ASCII_MAXLIK
};

/*----------------------- STRUCTS ----------------------------*/

// EQ = Equation (struct continaing a LEC equation, its compled form (CLEC), the estimation parameter and tests...)
//
// WARNING about the method property. 
// Before using it, check that the method property value is in the appropriate range: in the very first versions of iode,
// the allowed values for method were 'l', 'z', instead of 0, 1...
typedef struct _eq_ {
    char    *endo;      // endogeneous variable (= equation name)   
    char    *lec;       // LEC form of the equation (LHS := RHS)
    CLEC    *clec;      // Compiled equation for the simulation
    char    solved;     // Indicates if in clec, the equation is solved with respect to its endogenous (e.g.: "ln X := RHS" => "X := exp(RHS)")
    char    method;     // Estimation method
    SAMPLE  smpl;       // Estimation sample
    char    *cmt;       // Free comment
    char    *blk;       // List of equations estimated simultaneously
    char    *instr;     // List of instruments used to modify metric in the estimation process (INSTR method)
    long    date;       // Estimation date
    float   tests[EQS_NBTESTS]; // Estimation tests
} EQ;

/*----------------------- FUNCS ----------------------------*/

extern EQ*  K_eptr(KDB* kdb, char* name);

/* k_eqs.c */
extern void E_free(EQ *);
extern int E_split_eq(char *,char **,char **);
extern int E_dynadj(int ,char *,char *,char *,char **);
extern int E_DynamicAdjustment(int ,char **,char *,char *);

/*----------------------- MACROS ----------------------------*/

#define KELEC(kdb, pos)                   K_oval0(kdb, pos)
#define KECLEC(kdb, pos)    ((CLEC *)     K_oval1(kdb, pos))
#define KESOLV(kdb, pos)    (* (char *)   K_oval(kdb, pos, 2))
#define KEMETH(kdb, pos)    (* (char *)   K_oval(kdb, pos, 3))
#define KESMPL(kdb, pos)    (* (SAMPLE *) K_oval(kdb, pos, 4))
#define KECMT(kdb, pos)                   K_oval(kdb, pos, 5)
#define KEBLK(kdb, pos)                   K_oval(kdb, pos, 6)
#define KEINSTR(kdb, pos)                 K_oval(kdb, pos, 7)
#define KEDATE(kdb, pos)    (* (long *)   K_oval(kdb, pos, 8))
#define KETESTS(kdb, pos)   ((float *)    K_oval(kdb, pos, 9))

//#define EQ_STDEV (kdb, pos)  KETESTS(kdb, pos)[1]
//#define EQ_MEANY (kdb, pos)  KETESTS(kdb, pos)[2]
//#define EQ_SSRES (kdb, pos)  KETESTS(kdb, pos)[3]
//#define EQ_STDERR(kdb, pos)  KETESTS(kdb, pos)[4]
//#define EQ_FSTAT (kdb, pos)  KETESTS(kdb, pos)[6]
//#define EQ_R2    (kdb, pos)  KETESTS(kdb, pos)[7]
//#define EQ_R2ADJ (kdb, pos)  KETESTS(kdb, pos)[8]
//#define EQ_DW    (kdb, pos)  KETESTS(kdb, pos)[9]
//#define EQ_LOGLIK(kdb, pos)  KETESTS(kdb, pos)[10]

#define KEVAL(kdb, pos)     (K_eunpack(SW_getptr(kdb->k_objs[pos].o_val), kdb->k_objs[pos].o_name) )
#define KEPTR(name)         K_eptr(KE_WS, name)      // returns an allocated EQ

#ifdef __cplusplus
}
#endif

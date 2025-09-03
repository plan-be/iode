#pragma once

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/lec/lec.h"
#include "api/objs/kdb.h"

#include <string>
#include <array>
#include <vector>

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

const static std::vector<std::string> v_eq_tests_names = 
{ 
    "corr", "stdev", "meany", "ssres", "stderr", "stderrp",
    "fstat", "r2", "r2adj", "dw", "loglik"
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
struct EQ 
{
    std::string   endo;                     // endogenous variable (= equation name)   
    std::string   lec;                      // LEC form of the equation (LHS := RHS)
    CLEC*         clec;                     // Compiled equation for the simulation
    char          solved;                   // Indicates if in clec, the equation is solved with respect to its endogenous (e.g.: "ln X := RHS" => "X := exp(RHS)")
    char          method;                   // Estimation method
    Sample        sample;                   // Estimation sample
    std::string   comment;                  // Free comment
    std::string   block;                    // List of equations estimated simultaneously
    std::string   instruments;              // List of instruments used to modify metric in the estimation process (INSTR method)
    long          date;                     // Estimation date
    std::array<float, EQS_NBTESTS> tests;   // Estimation tests
};

/*----------------------- FUNCS ----------------------------*/

EQ*  K_eptr(KDB* kdb, char* name);

/* k_eqs.c */
void E_free(EQ *);
int E_split_eq(char *,char **,char **);
int E_dynadj(int ,char *,char *,char *,char **);
int E_DynamicAdjustment(int ,char **,char *,char *);

/*----------------------- FUNCS ----------------------------*/

int K_epack(char **,char *,char *);
EQ* K_eunpack(char *, char *);

inline std::string KELEC(KDB* kdb, int pos) 
{                  
    return std::string(K_oval0(kdb, pos));
}

inline CLEC* KECLEC(KDB* kdb, int pos) 
{    
    return ((CLEC *) K_oval1(kdb, pos));
}

inline char KESOLV(KDB* kdb, int pos) 
{    
    return *((char *) K_oval(kdb, pos, 2));
}

inline char KEMETH(KDB* kdb, int pos) 
{    
    return *((char *) K_oval(kdb, pos, 3));
}

inline Sample KESMPL(KDB* kdb, int pos) 
{    
    return *((Sample *) K_oval(kdb, pos, 4));
}

inline std::string KECMT(KDB* kdb, int pos) 
{                  
    return std::string(K_oval(kdb, pos, 5));
}

inline std::string KEBLK(KDB* kdb, int pos) 
{                  
    return std::string(K_oval(kdb, pos, 6));
}

inline std::string KEINSTR(KDB* kdb, int pos) 
{                
    return std::string(K_oval(kdb, pos, 7));
}

inline long KEDATE(KDB* kdb, int pos) 
{    
    return *((long *) K_oval(kdb, pos, 8));
}

inline std::array<float, EQS_NBTESTS> KETESTS(KDB* kdb, int pos) 
{   
    std::array<float, EQS_NBTESTS> tests;
    memcpy(tests.data(), K_oval(kdb, pos, 9), EQS_NBTESTS * sizeof(float));
    return tests;
}

inline EQ* KEVAL(KDB* kdb, int pos) 
{     
    return K_eunpack(SW_getptr(kdb->k_objs[pos].o_val), kdb->k_objs[pos].o_name);
}

inline EQ* KEPTR(char* name) 
{         
    return K_eptr(KE_WS, name);      // returns an allocated EQ
}

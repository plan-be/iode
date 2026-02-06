#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cstring>      // for std strncpy
#include <iostream>

// set SCR4 flags to help IntelliSense (VS Code) 
// to find the right functions and variables definitions
#ifndef REALD
    #define REALD
#endif
#ifndef SCRPROTO
    #define SCRPROTO
#endif 
#ifndef NOEMS 
    #define NOEMS
#endif
#ifndef NOEMF
    #define NOEMF
#endif

#define _max_(a, b)        (((a) > (b)) ? (a) : (b))
#define _min_(a, b)        (((a) < (b)) ? (a) : (b))

#include "scr4/s_strs.h"

/*---------------- DEFINE ------------------------*/

// ALD 23/11/2023
#include "version.h"

#define __DBG__ DebugForce("%s[%d]\n", __FILE__, __LINE__);     // tests

#ifdef __GNUC__
    #include <sys/stat.h>   // for mkdir
    #include <unistd.h>

    #define _unlink unlink
    #define _access access
    #define _creat creat
    #define _open open
    #define _read read
    #define _lseek lseek
    #define _close close
    #define _rmdir rmdir
    #define vsnprintf_s vsnprintf
#endif

// values.h n'existe pas en  VCC, remplacer les defines
#ifndef _MSC_VER
#include <values.h>
#else
#include <float.h>
#define DMAXEXP   DBL_MAX_EXP
#define DMINEXP   DBL_MIN_EXP
#define MAXDOUBLE DBL_MAX
#define MINDOUBLE DBL_MIN
#define MAXFLOAT  FLT_MAX
#define MINFLOAT  FLT_MIN
#endif

#include <set>
#include <map>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>       // for std::hash
#include <algorithm>        // for std::min, std::max

#include "api/utils/logging.h"


/* ALLOCS DOC 28/8/2012 */
// A PLACER DEVANT LES INCLUDES DE SCR (ou dans cc -c -DALLOCDOCON ...)
#define ALLOCDOCON

// REALD defines if not yet defined (to avoid compilation warning)
#ifndef REALD
    #define REALD
#endif

#if defined(DOS) || defined(SCRW32)
#define P_ALIGN     4
#endif

#ifdef UNIX
#define P_ALIGN     4
#endif

#ifdef SUN3
#define P_ALIGN     4
#endif

#ifdef SUN4
#define P_ALIGN     4
#endif

#ifdef HP
#define P_ALIGN     4
#endif

#ifndef U_sh
#define U_sh    unsigned short
#endif

#define K_VERSION  "1.0"
#define OK_MAX_NAME  10
#define K_MAX_NAME   20  /* IODE64K */

// ALD 18/11/2022
#define I_DEFAULT_FILENAME  "ws"

// JMP 23/5/2019 (from b_rep.c for b_proc.c)
#define LINELENGTH 102400

#define OK_MAX_FILE  64
#define K_MAX_FILE   512
#define K_MAX_DESC   64
//#define K_CHUNCK    1024 /* JMP 11-03-07 */
#define K_CHUNCK     (64 * 1024)
#define LMAGIC       32

#define IODE_NAN     (double)(-2.0e37)
#define X64          (sizeof(char*) == 8)

const static char* DEFAULT_INSTALLATION_DIR = "c:/iode";               // see function HLP_filename() in dos/o_help.c
const static char NAN_REP[3] = "--";

/*----------------------- TYPEDEF ----------------------------*/

typedef char    OFNAME[OK_MAX_FILE];
typedef char    FNAME[K_MAX_FILE];
typedef char    ONAME[K_MAX_NAME + 1];
typedef long    OSIZE;   /* JMP 23-05-00 */

/*---------------- ENUMS ------------------------*/

enum IodeCase
{
    UPPER_CASE,       /* default for mode */
    LOWER_CASE,
    ASIS_CASE
};

enum IodeType
{
    COMMENTS,
    EQUATIONS,
    IDENTITIES,
    LISTS,
    SCALARS,
    TABLES,
    VARIABLES,
    OBJECTS
};

#define IODE_NB_TYPES  7

const static std::vector<std::string> v_iode_types = 
{ "Comment", "Equation", "Identity", "List", "Scalar", "Table", "Variable", "Object" };


enum IodeFileType
{
    FILE_ASCII = -1,

    FILE_COMMENTS,
    FILE_EQUATIONS,
    FILE_IDENTITIES,
    FILE_LISTS,
    FILE_SCALARS,
    FILE_TABLES,
    FILE_VARIABLES,

    // 7

    // Warning: only to be used in api/objs/k_objfile.c
    ASCII_COMMENTS = 8,
    ASCII_EQUATIONS,
    ASCII_IDENTITIES,
    ASCII_LISTS,
    ASCII_SCALARS,
    ASCII_TABLES,
    ASCII_VARIABLES,

    // 15

    // iode version < 7 (Jean-Marc Paul + Geert Bryon)
    FILE_REP = 16,
    FILE_A2M,
    FILE_AGL,
    FILE_PRF,
    FILE_DIF,
    FILE_MIF,
    FILE_RTF,
    FILE_PS ,
    FILE_AAS,
    FILE_TXT,
    FILE_CSV,

    // 27

    // iode version >= 7 (Alix Damman)
    FILE_HTML = 28,
    FILE_REF,
    FILE_LOG,
    FILE_SETTINGS,
    FILE_ANY,
    DIRECTORY
};

// (binary) 7 + (ascii) 7 + iode version < 7 files (11) + iode version >= 7 (6) = 31 
const static int IODE_NB_FILE_EXT = 31;

/*-------------------- GLOBALS ------------------------*/

constexpr int EQS_NBTESTS = 20;

inline char    *BUF_DATA = NULL;
inline char    B_SEPS[] = " ,\n\t";     // Accepted separators for fn arguments (in report, DOS GUI..)
                                        // !! Semi-colon not accepted !!
inline int     B_MULTIBAR = 0;          // Graph parameter (Geert Bryon)

/*-------------------- MACROS ------------------------*/

#define P_len(ptr)            P_get_len(ptr, -1)
#define K_ISFILE(filename)    (filename != 0 && filename[0] != '-' && filename[0] != 0)
#define IODE_IS_A_NUMBER(x)   ((x) >= (double)(-1.0e37))
#define IODE_IS_0(x)          (((x) > 0 && (x) < 1.0e-36) || ((x) <= 0 && (-x) < 1.0e-36))

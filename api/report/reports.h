#pragma once

#include "api/constants.h"
#include "api/time/period.h"         // Period
#include "api/time/sample.h"         // Sample

#include <string>
#include <map>


/*------------------------ DEFINE ----------------------- */

#define RP_LINELENGTH 102400

/*------------------------ STRUCTS ----------------------- */

// REPFILE contains a report to be interpreted and the current line during interpretation
struct REPFILE 
{
    char                *filename;      // Source file for report file (*.rep) or
                                        // Proc name and position for procedures (PROCDEF)
    unsigned char       **tbl;          // Lines of the report
    int                 curline,        // Current line (during execution)
			            nblines;        // Total number of lines in the report
};

struct BFNS 
{
    char*   func_name_camel_case;           // function name in CamelCase
    int     (*fn)(char* arg, int type_);    // function pointer in *non GUI* mode
    int     (*sfn)(char* arg, int type_);   // function pointer in GUI mode
    int     type;                           // suffix required after function name (var, idt...) or not:
			                                //   0=not required
						                    //   1=ws type required: cmt, idt, ... see k_ext => not used ?
						                    //   2=suffix required: file extensions defined in k_ext
						                    //   3=ws type required in non GUI, no extension in GUI
						                    //   4=suffix required: file extensions defined in k_ext if non GUI, not required in GUI
    int     group;                          // for color highlighting
};

const static int BFNS_NB_GROUPS = 2;

struct RPFN 
{
    U_ch*   (*fn)(U_ch** args);     // Pointer to the corresponding function
};

/*------------------------ GLOBALS ----------------------- */

extern std::map<std::string, BFNS> B_fns;  // Table of report command names and function pointers
extern std::map<std::string, RPFN> RP_FNS; // Table of report @functions names and function pointers

inline REPFILE* CUR_REPFILE = 0;            // Pointer to the current REPFILE during report execution
inline char*    RP_RPTSTR = 0;              // Repeat string used by the command $repeat. Default = '_'.
inline char*    RP_VSEPS = 0;               // Separators allowed in report @functions
inline int      RP_RT = 0;                  // Indicates what to do on error (0-ignore, -2=return, -3=ABORT, -4=QUITODE)
inline int      RP_ARG0 = 0;                // Position of the first argument in RP_ARGV (can be changed by a call to $shift)
inline int      RP_STDOUT = 0;              // If set, the report is written to stdout (default to 0 -> write to file specified by the user)
inline int      RP_DEBUG = 0;               // Debug level: 0=no debug, 1=std debug, 2=full debug
inline int      RP_ALLOW_INDENT = 0;        // If set, indentation is allowed in reports
inline int      RP_ALLOW_MULTILINE = 1;     // If set, accepts instruction on multiple lines (non breaking char = '\')
inline int      RP_ALLOW_NOPARSING = 1;     // If set, text between '`' is left asis
inline int      RP_NOPARSING_CHAR = '`';    // If set, strings between RP_NOPARSING_CHAR are not parsed
inline int      RP_PRINT = 2;               // 2 = print and display messages, 1 = print only, 0 = ignore
inline int      RP_T = 0;                   // current value of t for LEC evaluations in reports
inline Period   RP_PER;                     // current Period for LEC evaluations in reports

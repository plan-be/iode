#pragma once

#include "scr4/s_strs.h"        // U_ch

#include "api/constants.h"
#include "api/utils/time.h"         // PERIOD

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------ DEFINE ----------------------- */

#define RP_LINELENGTH 102400

/*------------------------ STRUCTS ----------------------- */

// REPFILE contains a report to be interpreted and the current line during interpretation
typedef struct _repfile_ {
    char                *filename;      // Source file for report file (*.rep) or
                                        // Proc name and position for procedures (PROCDEF)
    unsigned char       **tbl;          // Lines of the report
    int                 curline,        // Current line (during execution)
			nblines;        // Total number of lines in the report
} REPFILE;

typedef struct _repfns_ {
    char    *keyw;          // function name to use in reports
    char    *key_camel;     // function name in CamelCase
    int     (*fn)();        // function pointer in *non GUI* mode
    int     (*sfn)();       // function pointer in GUI mode
    int     type;           // suffix required after keyw (var, idt...) or not:
			    //   0=not required
						//   1=ws type required: cmt, idt, ... see k_ext => not used ?
						//   2=suffix required: file extensions defined in k_ext
						//   3=ws type required in non GUI, no extension in GUI
						//   4=suffix required: file extensions defined in k_ext if non GUI, not required in GUI
    int     group;          // for color highlighting
} BFNS;

const static int BFNS_NB_GROUPS = 2;

typedef struct _rpfn_ {
    U_ch    *name;          // name of the @function (ex "upper")
    U_ch    *(*fn)();       // Pointer to the corresponding function
} RPFN;

/*------------------------ GLOBALS ----------------------- */

extern REPFILE *CUR_REPFILE;        // Pointer to the current REPFILE during report execution
extern BFNS    B_fns[];             // Table of report command names and function pointers
extern RPFN    RP_FNS[];            // Table of report @functions names and function pointers
extern char*   RP_RPTSTR;           // Repeat string used by the command $repeat. Default = '_'.
extern char*   RP_VSEPS;            // Separators allowed in report @functions
extern int     RP_RT;               // Indicates what to do on error (0-ignore, -2=return, -3=ABORT, -4=QUITODE)
extern int     RP_ARG0;             // Position of the first argument in RP_ARGV (can be changed by a call to $shift)
extern int     RP_STDOUT;           // If set, the report is written to stdout (default to 0 -> write to file specified by the user)
extern int     RP_DEBUG;            // Debug level: 0=no debug, 1=std debug, 2=full debug
extern int     RP_ALLOW_INDENT;     // If set, indentation is allowed in reports
extern int     RP_ALLOW_MULTILINE;  // If set, accepts instruction on multiple lines (non breaking char = '\')
extern int     RP_ALLOW_NOPARSING;  // If set, text between '`' is left asis
extern int     RP_NOPARSING_CHAR;   // If set, strings between RP_NOPARSING_CHAR are not parsed
extern int     RP_PRINT;            // 2 = print and display messages, 1 = print only, 0 = ignore
extern int     RP_T;                // current value of t for LEC evaluations in reports
extern PERIOD  RP_PER;              // current PERIOD for LEC evaluations in reports

#ifdef __cplusplus
}
#endif

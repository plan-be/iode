#pragma once

#include "api/pch.h"
#include "api/time/period.h"         // Period
#include "api/time/sample.h"         // Sample



/*------------------------ DEFINE ----------------------- */

#define RP_LINELENGTH 102400

/*------------------------ STRUCTS ----------------------- */

// REPFILE contains a report to be interpreted and the current line during interpretation.
// REPFILE objects must be created with std::make_shared (see execute()).
struct REPFILE : public std::enable_shared_from_this<REPFILE>
{
    std::string                 filename;           // Source file for report file (*.rep) or
                                                    // Proc name and position for procedures (PROCDEF)
    std::vector<std::string>    report_lines;       // Lines of the report
    int                         current_line = 0;   // Current line (during execution)

    /**
     *  Creates and initialises a REPFILE struct.
     *  REPFILEs contain the source filename (or the PROC name) and the contents (the lines) of the report.
     *  Other informations like the current line during execution are also stored in the REPFILE struct.
     *
     *  @param [in]     const std::string&                  filename        source filename (or PROC description)
     *  @param [in]     const std::vector<std::string>&     report_lines    report lines
     */
    REPFILE(const std::string& filename, const std::vector<std::string>& report_lines)
        : filename(filename), report_lines(report_lines), current_line(0) {}

    std::string read_multiline();
    int read_line(std::string& line, int mode);
    int execute();

private:
    static int chk_ignore(const std::string& line);
    int err_dump(char* name, char* arg);
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
    std::string (*fn)(const std::vector<std::string>& args);     // Pointer to the corresponding function
};

/**
 *  Exception thrown by a report @function (RPF_* functions) when it cannot be evaluated
 *  (wrong number of arguments, empty workspace...).
 *  Also thrown by RP_gfn() when the @function does not exist.
 *  The text of the @function call is then left unchanged in the report line (see RP_expand()).
 */
struct RPFunctionError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

/**
 *  Exception thrown by RP_gmacro() when a macro (%name%) is not defined or when a report
 *  argument (%1%, %2%...) is missing.
 *  The text of the macro (%name%) is then left unchanged in the report line (see RP_expand()).
 */
struct RPMacroError : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

/*------------------------ GLOBALS ----------------------- */

extern std::map<std::string, BFNS> B_fns;  // Table of report command names and function pointers
extern std::map<std::string, RPFN> RP_FNS; // Table of report @functions names and function pointers

inline std::shared_ptr<REPFILE> CUR_REPFILE = nullptr;  // Current REPFILE during report execution
inline std::vector<std::string> RP_ARGV;    // Arguments of the current report (without the report name)
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

/*------------------------ FUNCTIONS ----------------------- */

/**
 * @brief
 *
 * @param filepath
 * @param parameters
 *
 * @note same as function IodeExecArgs() from b_api.c (from iode_dos repository).
 *       See https://iode.plan.be/doku.php?id=reportexec
 */
void execute_report(const std::string& filepath, const std::string& parameters = "");

void execute_report(const std::string& filepath,
    const std::vector<std::string>& parameters = std::vector<std::string>());

/**
 * @brief
 *
 * @param commands Sets of report command(s) separated by '\n'
 */
void execute_report_line(const std::string& commands);

void execute_report_line(const std::vector<std::string>& commands);

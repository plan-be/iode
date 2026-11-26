/**
 *  @header4iode
 * 
 *  IODE report Control Flow
 *  ------------------------
 *  Main functions for parsing and executing of IODE reports.
 *  
 *  See also the variable B_fns in b_rep_syntax.c: there are 2 fn pointers, 
 *  one for the non GUI mode ($function), the other for the GUI mode (#function).
 *  
 *  Report description
 *  ------------------
 *  Reports consist of a set of lines that are interpreted in sequence. There are 3 types of lines:
 *      - lines that start with $<command> or #<command> are IODE commands that are "executed". Their
 *        result, if any, is printed (or ignored) according to the last $PrintDest command. 
 *        The list of currently available "$commands" can be found in b_rep_syntax.c
 *      - lines that start with a $ or # followed by a space are comments. 
 *      - all other lines are plain text that will be send to the current Printing destination 
 *        after parsing
 *  
 *  Parsing
 *  -------
 *  Before the execution of the command it may contain, each line is parsed and the following 
 *  character sequences are replaced by calculated values:
 *  - %macro%: macros (between %%) are replaced by their value
 *  - {lex}: LEC expressions (between {}) are calculated and replaced by their (formatted) value
 *  - {$command}: IODE command between {} are executed and their result replaces the 
 *      original {} sequence. See the variable B_fns in b_rep_syntax.c for the current list of commands.
 *      B_fns contains 2 fn pointers, one for the non GUI mode ($function), 
 *      the other for the GUI mode (#function).

 *  - @function(): report functions (@function()) are executed and replaced by their value. 
 *      See RP_FNS in b_rep_syntax.c for the current list of functions.

 *  - `any expression`: text between backquotes remains asis (not parsed) unless otherwise specified 
 *      by a "$noparsing 0" command
 *     
 *  Hierarchy of the report engine functions
 *  ----------------------------------------
 *  B_ReportExec and B_ReportLine are the 2 top level functions.
 *  
 *  B_ReportExec calls 
 *      RP_ReportExec_1                     Reads a report file and creates a REPFILE structure, then calls REPFILE::execute()
 *  B_ReportLine                            Creates a REPFILE structure from a string (possibly containing multiple lines separated by '\n') then calls REPFILE::execute() 
 *      REPFILE::execute()                  Executes a REPFILE structure, ie a table of report lines
 *          REPFILE::read_line()            Expands a report line 
 *              REPFILE::read_multiline()   Groups lines ended by " \" into one single line
 *              RP_expand (recursive)       Replaces %macro%, {lec}, @fn()
 *                  RP_extract              Extracts a string bw %% or {}
 *                  RP_gmacro               Replaces %macro% par its value
 *                      RP_expand...
 *                  RP_gcmd                 Calculates {macro} and put the result in the report line
 *                      RP_expand...
 *                  RP_extractpar           Extracts an @function
 *                  RP_gfn                  Executes @functions and put the result in the report line
 *                      RP_expand...
 *          RP_exec_fn()                    Executes the report command (if any)
 *              RP_find_fn                  Finds the right function
 *              (*B_fns[name].fn)           Executes the no-GUI function
 *              (*B_fns[name].sfn)          Executes the GUI function
 *              
 *  
 *  Examples of report line parsing
 *  -------------------------------
 *       Creation of the variable B and the 3 macros c, C and C1
 *       -------------------------------------------------------
 *       $WsSample 2000Y1 2010Y1
 *       $DataCalcVar B t
 *       $define C   1
 *       $define C1  2
 *       $define c   b
 *      
 *       Set the current execution period
 *       --------------------------------
 *       $SetTime 2005Y1
 *       
 *       Expressions between {...}
 *       -------------------------
 *       Note that {} expressions cannot be nested 
 *  
 *       Example 1: {lec + %define%}      : {1 + %C%}         -> 2
 *       Example 2: {lec + @fn()}         : {1 + @upper(b)}   -> 6
 *       Example 3: {lec + %@fn()%}       : {1 + %@upper(c)%} -> 2
 *       Example 4: {lec + @fn(%define%)} : {1 + @upper(%c%)} -> 6
 *       
 *       Expressions between %...%
 *       -------------------------
 *       Note that %defines% cannot be nested 
 *       Example 1: %C%               : %C%         -> 1
 *       Example 2: %@fn()%           : %@upper(c)% -> 1
 *       Example 3: %C{lec}%          : %C{2-1}%    -> 2
 *       
 *       Expressions @-functions()
 *       -------------------------
 *       Example 1: @fn()             : @date() @time()        -> 14-10-2022 01:19:44
 *       Example 2: @fn(%def%)        : @upper(%c%)            -> B
 *       Example 3: @fn(@fn())        : @upper(@getdir())      -> C:\USR\IODE_SRC\CMD
 *       Example 4: @fn({lec})        : @drop({1+1},ABCDE)     -> CDE
 *       Example 5: @fn({lec})        : @drop({1+1},A BCDE)    -> BCDE
 *       Example 6: @fn({lec})        : @drop({1+1},"A,BC,DE") -> BC,DE
 */
#include "api/b_args.h"
#include "api/k_super.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"
#include "api/write/write.h"

#include "api/report/engine/engine.h"
#include "api/report/undoc/undoc.h"


// Global working variables
// ------------------------
int RP_DEPTH = 0;           // Level of the current report in the report "stack"


// Function implementation
// -----------------------


/**
 *  Reads a report file (with no line expansion).
 *  The max line length is RP_LINELENGTH.
 *  
 *  @param [in]         const std::string&          filename    report file
 *  @return             std::vector<std::string>                content of the file (at least one line,
 *                                                              possibly empty, if the file can be read).
 *                                                              Empty vector if the file cannot be opened or
 *                                                              if one of the lines exceeds RP_LINELENGTH
 */
std::vector<std::string> RP_read_file(const std::string& filename)
{
    std::vector<std::string> lines;

    FILE* fd = fopen(filename.c_str(), "r");
    if(fd == NULL)
        return lines;

    std::string line;
    int ch;
    while((ch = getc(fd)) != EOF)
    {
        if(ch == '\n')
        {
            lines.push_back(line);
            line.clear();
        }
        else if(line.size() < RP_LINELENGTH)
            line.push_back((char) ch);
        else
        {
            fclose(fd);
            std::string error_msg = "Report line exceeds " + std::to_string(RP_LINELENGTH) + " characters";
            error_manager.append_error(error_msg);
            return std::vector<std::string>();
        }
    }
    fclose(fd);

    // last line (not terminated by '\n')
    lines.push_back(line);
    return lines;
}


/**
 *  Reads the next line in a REPFILE struct. If the line ends with a " \\", joins the next line.
 *  
 *  If the last characters are " \\" and the global RP_ALLOW_MULTILINE is not null, 
 *  the next line(s) are joined to the current one as long as they are terminated by " \\". 
 *  
 *  The 2 chars " \\" at the end of the line(s) are replaced by a single '\n'. 
 *  
 *  @return     std::string     line(s) (empty string if the end of the report is reached)
 */

std::string REPFILE::read_multiline()
{
    std::string multi_line;

    while(current_line < (int) report_lines.size())
    {
        multi_line += report_lines[current_line];
        current_line++;

        // replaces the final " \\" by '\n' and joins the next line
        size_t lg = multi_line.size();
        if(RP_ALLOW_MULTILINE && lg > 1 &&
           multi_line[lg - 2] == ' ' && multi_line[lg - 1] == '\\')
        {
            multi_line.resize(lg - 2);
            multi_line.push_back('\n');
        }
        else
            break;
    }

    return multi_line;
}

/**
 *  Reads the current line of the report.
 *  Expands the line (%define%, {lex expression}...) and stores the expanded line in line.
 *
 *  @param [out]        std::string&    line    line (having been expanded or not)
 *  @param [in]         int             mode    1 to expand the line,
 *                                              0 to keep the line unchanged, for example when searching for $goto or $procend lines
 *  @return             int                     EOF if the last line is reached
 *                                              rc of RP_expand() on error
 *                                              0 on success
 */
int REPFILE::read_line(std::string& line, int mode)
{
    if(current_line >= (int) report_lines.size())
        return EOF;

    std::string multi_line = read_multiline();

    int rc = 0;
    if(mode)  // expand macros, {}, ...
        rc = RP_expand(line, multi_line);
    else
        line = multi_line;

    if(rc)
        return rc;

    // Shift text to the margin if $ or # and indent allowed
    if(RP_ALLOW_INDENT && !line.empty())
    {
        // Goto 1st non space
        int i;
        for(i = 0 ; line[i] && SCR_is_space(line[i]); i++);
        if(U_is_in(line[i], "$#"))
            U_ljust_text((unsigned char*) line.data());
    }

    return rc;
}


/**
 *  Indicates if errors on a report line must be ignored, ie if the second char of the line is '-'.
 *  Example : $-command
 *  
 *  @param [in] const std::string&  line    report line
 *  @return     int                         1 if the error must be ignored
 *                                          0 else
 */
int REPFILE::chk_ignore(const std::string& line)
{
    if(line.size() > 1 && line[1] == '-')
        return 1;
    return 0;
}


/**
 *  Splits a report line into the command and its arguments.
 *  
 *  @param [in]      const std::string& text   line to split
 *  @param [out]     std::string&       cmd    command (first word in text: letters, digits and '_'),
 *                                             truncated to lg characters
 *  @param [out]     std::string&       arg    arguments (text following the command, without leading
 *                                             white spaces and trailing characters <= ' ')
 *  @param [in]      int                lg     max length of cmd
 *  @return          int                       -1 if text is empty or starts with '\n'
 *                                             0 otherwise
 */
int RP_splitline(const std::string& text, std::string& cmd, std::string& arg, int lg)
{
    int     rt = 0;
    size_t  j = 0;

    if(text.empty() || text[0] == '\n')
        rt = -1;
    else
    {
        // NOTE: text[j] is passed as a (signed) char, as in the previous version:
        //       the CP850 accented letters are not part of the command name
        while(j < text.size() && (SCR_is_anum(text[j]) || text[j] == '_'))
            j++;
    }
    cmd = text.substr(0, std::min(j, (size_t) std::max(lg, 0)));

    // arguments: removes the leading white spaces and the trailing characters <= ' '
    // (same as U_ljust_text() + SCR_strip())
    size_t start = text.find_first_not_of(" \t\n\r\v\f", j);
    arg = (start == std::string::npos) ? std::string() : text.substr(start);
    while(!arg.empty() && (unsigned char) arg.back() <= ' ')
        arg.pop_back();
    return rt;
}


/**
 *  Returns the BFNS struct corresponding to the command name.
 *  
 *  See in b_rep_syntax.c the definition of the table B_fns for more details.
 *  
 *  @global [in]     BFNS   B_fns   table of command names and fn pointers
 *  @param [in]      const std::string&  name    command name ($name), possibly followed by an
 *                                               extension (e.g. "WsLoadVar")
 *  @param [out]     int&                type    type of object or file to which the command must be assigned
 *  @param [in]      int                 fs      full screen : character preceding the command in the report ('#' or '$').
 *  @return          BFNS*                       BFNS struct corresponding to the command name (nullptr if not found)
 *
 */
BFNS* RP_find_fn(const std::string& name, int& type, int fs)
{
    std::string cmd_name = to_lower(trim(name));

    size_t n;
    std::string prefix;
    std::string suffix;
    std::string extension;
    for(auto& [func_name, fns]: B_fns) 
    {
        // command without extension 
        if(fns.type == 0) 
        {
            if(cmd_name == func_name)
                return &fns;
            continue;
        }
        else 
        {
            n = func_name.length();
            prefix = cmd_name.substr(0, n);
            if(prefix != func_name) 
                continue;
            
            for(int j = 0; k_ext[j]; j++) 
            {
                if(j > VARIABLES && fns.type % 2) 
                    break;
                
                suffix = cmd_name.substr(n);
                extension = std::string(k_ext[j]);
                if(suffix == extension)
                {
                    type = j;
                    return &fns;
                }
            }
            
            // if(fns.type > 2 && fs == "#") { => erreur "" => ''
            if(fns.type > 2 && fs == '#') 
            {
                type = 0;
                return &fns;
            }
        }
    }

    return nullptr;
}


/**
 *  Retrieves a report command function pointer and executes the function with its parameters.
 *  
 *  @param [in] char*   name    report function name ("WsLoadVar")
 *  @param [in] char*   arg     parameters of the report function ("fun.var")
 *  @param [in] int     fs      full screen: character preceding the command in the report ('#' or '$').
 *  @return     int             -1 if the fns does not exist
 *                              return code of the called function otherwise
 */
int RP_exec_fn(char* name, char* arg, int fs)
{
    int parm;

    BFNS* fns = RP_find_fn(name, parm, fs);
    if(!fns) 
        return -1;
    
    if(fs == '#' && fns->sfn != 0)
        return((*fns->sfn)(arg, parm));
    
    if(fs == '$' && fns->fn != 0)
        return((*fns->fn)(arg, parm));
    
    return -1;
}


/**
 *  Prints or displays the current report line if an error has occured.
 *  
 *  @param [in] char*   name    report function name
 *  @param [in] char*   arg     function arguments
 *  @return             int     always 0
 */
int REPFILE::err_dump(char* name, char* arg)
{
    std::string error_msg = "Error: " + std::string(name) + " " + std::string(arg);

    // print
    if(RP_PRINT == 1)
        W_printf((char*) error_msg.c_str());
    // display
    else
        kerror(0, (char*) error_msg.c_str());

    return 0;
}


/**
 *  Extracts from buf, starting at position i, the string terminated by the specific character
 *  ch (eg '%' or '}').
 *  On success, i is set to the position of ch in buf.
 *
 *  If ch is not found in buf, returns an empty string and sets i to -1
 *  (the result may also be empty when ch is found, eg "{}").
 *
 *  Example:
 *      str = "Value: {A[1990Y1] + 2} EUR";
 *             01234567890123456789012345
 *      pos = 8; // starting search position = 1 + position of '{'
 *      RP_extract(str, pos, '}') => "A[1990Y1] + 2", pos = 21
 *
 *  @param [in]      const std::string& buf  input string
 *  @param [in, out] int&               i    position in buf of the first character of the
 *                                           resulting string. On return: position of ch,
 *                                           or -1 if ch is not found
 *  @param [in]      int                ch   ending character
 *  @return          std::string             extracted string (without ch)
 */
std::string RP_extract(const std::string& buf, int& i, int ch)
{
    size_t start = (size_t) std::max(i, 0);
    size_t pos = (ch == '\0' || start > buf.size()) ? std::string::npos : buf.find((char) ch, start);
    if(pos == std::string::npos)
    {
        i = -1;
        return std::string();
    }

    i = (int) pos;
    return buf.substr(start, pos - start);
}


/**
 *  Calculates a macro value. The macro may contain expressions between {} or @-functions.
 *  
 *  Eg:
 *      Report                                         | Result
 *      -----------------------------------------------|-----------------------
 *      $define A AAA                                  |
 *      $define A1A XXX                                |
 *      $show A1A                   = %A1A%            | A1A             = XXX
 *      $show %%A{{1}A%%            = %A{1}A%          | %A{1}A%         = XXX
 *      $show %%A@@upper({{1})A%%   = %A@upper({1})A%  | %A@upper({1})A% = XXX
 *
 *      
 *  @param [in] const std::string&  str  text between the 2 % on the report line
 *  @return     std::string              value of the macro after transformation
 *  @throw      RPMacroError             if the macro is not defined or the report argument is missing:
 *                                       the caller (RP_expand()) then leaves the text %str% unchanged
 */
std::string RP_gmacro(const std::string& str)
{
    // expands the macros, LEC expressions and @functions in the name of the macro
    std::string tmp;
    if(str.find_first_of("%{@") != std::string::npos)
        RP_expand(tmp, str);
    else
        tmp = str;

    int nb_args = (int) RP_ARGV.size();
    if(tmp[0] >= '0' && tmp[0] <= '9')
    {
        // %0% = number of arguments, %1%, %2%... = arguments of the report
        int pos = atoi(tmp.c_str());
        if(pos == 0)
            return std::to_string(nb_args - RP_ARG0);

        pos += RP_ARG0;
        if(pos < 1 || pos > nb_args)
        {
            error_manager.append_error("Report : Argument (" + std::to_string(pos) + ") is missing");
            throw RPMacroError("Report : Argument (" + std::to_string(pos) + ") is missing");
        }
        return RP_ARGV[pos - 1];
    }

    // %*% = all the arguments of the report (after the $shift's) separated by ';'
    if(tmp[0] == '*')
    {
        std::vector<std::string> args;
        for(int i = std::max(RP_ARG0, 0); i < nb_args; i++)
            args.push_back(RP_ARGV[i]);
        return join(args, ";");
    }

    std::string name = RP_MACRO->to_key(tmp);
    if(!RP_MACRO->contains(name))
    {
        std::string error_msg = "Report: Macro '" + name + "' is not defined";
        kwarning(error_msg.c_str());
        throw RPMacroError(error_msg);
    }
    return RP_MACRO->get_macro(name);
}


/**
 *  Calculates the value of an expression between {curly brackets}.
 *  
 *  Before evaluating the value, expands the line contents by replacing expressions bw %% and the @-functions 
 *  by their calculated values (@see RP_expand()).
 *  
 *  @param [in] const std::string&  str  text between {} containing optionally macros, lec expressions and @-functions
 *  @return     std::string              calculated value (empty string on error)
 */
std::string RP_gcmd(const std::string& str)
{
    std::string tmp;
    if(str.find_first_of("%{@") != std::string::npos)
        RP_expand(tmp, str);
    else
        tmp = str;

    std::string res;
    RP_eval(res, tmp);
    return res;
}


/** 
 *  Calculates RP_T, the position in the current sample of the current calculation period RP_PER.
 *
 *  Note: when a LEC expression in evaluated in a report line ({LEC_expression}), the calculation period must be
 *  defined. That "current calculation period" is stored in the global RP_PER which is set by a call to $SetTime.
 *  
 *  @see RP_settime()
 *  
 *  @return int     0 on success
 *                  -3 RP_T is < 0 (RP_PER < current sample)
 */
int RP_evaltime()
{
    RP_T = 0;
    if(RP_PER.year == 0) 
        return 0;
    
    KDBVariablesPtr kdb_var = global_ws_var;
    if(!kdb_var)
        return 0;
    
    std::shared_ptr<Sample> sample = kdb_var->get_sample();
    if(!sample)
        return 0;
    
    Period start_period = sample->start_period;
    if(start_period.year == 0)
        return 0;

    RP_T = RP_PER.difference(start_period);
    if(RP_T < 0)
    {
        std::string msg = "The current report period " + RP_PER.to_string();
        msg += " is before the sample period " + start_period.to_string() + ".\n";
        msg += "Cannot evaluate LEC expressions in reports.";
        kwarning(msg.c_str());
        return -3;
    }
    
    return 0;
}


/**
 *  Evaluates a LEC expression in (the period) RP_PER. 
 *  
 *  @param [in]         const std::string&  lec   LEC expression or empty string (trimmed before evaluation)
 *  @return             double              IODE_NAN on error or if RP_PER is before the current WS sample
 *                                          calculated LEC value on success
 */
double RP_evallec(const std::string& lec)
{
    if(RP_evaltime() < 0)
        return IODE_NAN;

    std::string _lec_ = trim(lec);
    double x = IODE_NAN;
    std::shared_ptr<CLEC> clec = nullptr;
    if(_lec_[0]) 
    {
        try
        {
            clec = std::make_shared<CLEC>(_lec_);
        }
        catch(const std::exception&) 
        {
            error_manager.append_error("Syntax error " + std::string(L_error()));
            return(x);
        }

        int rc = clec->link(global_ws_var, global_ws_scl);
        if(rc == 0)
            x = clec->execute(RP_T);
    }

    return x;
}


/**
 *  Formats a double. 
 *  See https://iode.plan.be/doku.php?id=les_expressions_lec_dans_les_rapports for examples.
 *  
 *  The number value is formatted according to the value of format.
 *      - format empty => default format (20 positions, free # of decimal places)
 *      - format = "T" => format as a Period (e.g.: {3@T} => 1990Y1)
 *      - format = "99999.99" => format as 8.2lf
 *      - format = ".99" => format as 30.2lf
 *      - format = "999" => format as 3.0lf
 *  
 *  Examples can be found in RP_expand().
 *  
 *  @param [out]     std::string&       buf     formatted double
 *  @param [in]      const std::string& format  format (see above). Empty string for the default format
 *  @param [in]      double             value   any double
 *  @return          int                        0 always
 */
int RP_fmt(std::string& buf, const std::string& format, double value)
{
    int lg = 20, nbdec = -1;

    // removes the leading white spaces and the trailing characters <= ' '
    // (same as U_ljust_text() + SCR_strip())
    std::string fmt_str = format;
    size_t start = fmt_str.find_first_not_of(" \t\n\r\v\f");
    if(start == std::string::npos)
        fmt_str.clear();
    else
        fmt_str.erase(0, start);
    while(!fmt_str.empty() && (unsigned char) fmt_str.back() <= ' ')
        fmt_str.pop_back();

    if(!fmt_str.empty())
    {
        // format "T" (or "t") => Period
        if(fmt_str[0] == 'T' || fmt_str[0] == 't')
        {
            int t = (int) value;
            Period per = global_ws_var->get_sample()->start_period.shift(t);
            buf = per.to_string();
            return 0;
        }

        std::vector<std::string> fmt = split_skip_empty(fmt_str, ".");
        if(!fmt.empty())
        {
            lg = (int) fmt[0].size();
            if(fmt.size() > 1)
            {
                nbdec = (int) fmt[1].size();
                lg += nbdec + 1;
            }
            else if(fmt_str[0] == '.')
            {
                lg = 30;
                nbdec = (int) fmt[0].size();
            }
            else
                nbdec = 0;
        }
    }

    // SCR_fmt_dbl() writes size characters (or "--" for NaN).
    // Its internal buffers are 80 bytes long => size is limited to 79.
    int size = (value > 0) ? lg : lg + 1;
    size = std::min(size, 79);
    std::vector<unsigned char> tmp(size + 3, 0);
    SCR_fmt_dbl(value, tmp.data(), size, nbdec);
    buf = std::string((char*) tmp.data());
    // removes the spaces (same as SCR_sqz())
    buf.erase(std::remove(buf.begin(), buf.end(), ' '), buf.end());
    return 0;
}



/**
 *  Interprets strings between accolades found in a report line. 
 *  
 *  Executes an expression between {}:
 *      Syntax                      Result 
 *      ----------------------------------------------------------------------------
 *      {$command_name args}        result of a report command called in non GUI mode
 *      {#command_name args}        result of a report command called in GUI mode
 *      {$!command_name args}       inverse of the result of a report command called in non GUI mode
 *      {#!command_name args}       inverse of the result of a report command called in GUI mode
 *      {=ExcelRange}               an Excel Range contents
 *      {lec_expressions}           a LEC expression value 
 *      {lec_expressions@format}    a LEC expression formatted value
 *                                 
 *  @param [out]     std::string&   res    result (empty string on error)
 *  @param [in]      const std::string& farg   contents to be interpreted
 *  @return          int                   0 on success, -1 on error
 */
int RP_eval(std::string& res, const std::string& farg)
{
    char        ch, name[31];
    int         rc, rp_rt;
    double      x;
    bool        inv = false;

    res.clear();
    ch = farg[0];

    // farg = "$name args, #name args, $!name args or #!name args" => executes a report command
    if(ch == '$' || ch == '#')
    {
        rp_rt = RP_RT;      // Memorise the current return value
        RP_RT = 0;          // Reset RT to "$Onerror Ignore"

        // if $!name of #!name: inverts the return code
        if(farg[1] == '!')
            inv = true;

        // get_next_arg() and the report commands modify their argument: works on a copy of farg
        std::string cmd = farg;

        // Retrieve the command name (max 30 chars)
        get_next_arg(name, cmd.data() + 1 + (int) inv, 30);

        // Execute the function name
        // (the arguments start 2 chars after the length of the name, but not after the end of cmd)
        size_t args_pos = std::min(strlen(name) + 2, cmd.size());
        rc = RP_exec_fn(name, cmd.data() + args_pos, ch);
        
        // Reset the previous OnError value (before calling RP_exec_fn)
        RP_RT = rp_rt;          
        
        // rc must be 0 or 1
        // NOTE: on error set rc to 0 (why ?)
        rc = (rc < 0) ? 0 : 1;

        // invert rc if $!name
        if(inv) 
            rc = !rc;       

        // Return code to save in res
        res = std::to_string(rc);
        return 0;
    }

    // farg = "=ExcelAddress" => Excel Get via DDE
    if(ch == '=')
    {
        std::string address = farg.substr(1);
        char* item = B_ExcelGetItem(address.data());
        if(item == NULL)
            return -1;
        res = std::string(item);
        SCR_free(item);
        replace_all(res, "\t", " ");
        replace_all(res, "\r\n", " ");
        return 0;
    }

    // farg = "lec_expression" or "lec_expression@format"
    std::string lec = farg;
    std::string fmt;                    // empty => default format
    size_t pos = farg.find('@');
    if(pos != std::string::npos)
    {
        lec = farg.substr(0, pos);      // LEC formula
        fmt = farg.substr(pos + 1);     // format
    }

    x = RP_evallec(lec);
    // Empty string on error
    if(!IODE_IS_A_NUMBER(x))
        return -1;

    RP_fmt(res, fmt, x);       // The formatted value is returned
    return 0;
}



// ========= Report "@functions" interpretation and execution =========================

/**
 *  Extracts from buf, starting at position i, the string terminated by the closing parenthesis
 *  of the first opening parenthesis (the closing parenthesis is not included in the result).
 *  Each new opening parenthesis raises the level by 1, each closing lowers the level by 1.
 *  The parenthesis between double quotes are ignored.
 *  Before the first opening parenthesis (level 0), only letters, digits, '_' and ' ' are
 *  accepted (function name).
 *
 *  @note the function name is RP_extractpar because originally the idea was to use the
 *  same function for different types of parentheses: '[]', '{}', '()'..., not only for functions
 *  but also for {lec} formulas for example.
 *
 *  If no closing parenthesis is found or if an invalid character is found at level 0,
 *  returns an empty string and sets i to -1.
 *
 *  Example:
 *      str          = "Zip: @upper(@take(10,"b-(1000)")) ";
 *      position        0123456789012345678901234567890123
 *      level           0000000000011111122222222222222210
 *
 *      pos = 6;
 *      RP_extractpar(str, pos, "()") => "upper(@take(10,\"b-(1000)\")", pos = 32
 *
 *  @param [in]      const std::string& buf       input string
 *  @param [in, out] int&               i         in: position in buf of the first character of
 *                                                the resulting string
 *                                                out: position of the closing parenthesis in buf,
 *                                                or -1 if not found
 *  @param [in]      const std::string& brackets  opening and closing characters (eg "()" or "[]")
 *  @return          std::string                  string between position i and the closing
 *                                                parenthesis (excluded)
 */
std::string RP_extractpar(const std::string& buf, int& i, const std::string& brackets)
{
    int     nb = 0;
    bool    instr = false;
    size_t  start = (size_t) std::max(i, 0);
    size_t  k;

    if(brackets.size() < 2)
    {
        i = -1;
        return std::string();
    }

    // buf[k] != '\0': stops at the first '\0' (as with the previous const char* buf)
    for(k = start; k < buf.size() && buf[k]; k++)
    {
        if(buf[k] == '"')
        {
            instr = !instr;
            continue;
        }
        if(instr)
            continue;
        if(buf[k] == brackets[1] && nb <= 1)
            break;
        if(buf[k] == brackets[1])
            nb--;
        else if(buf[k] == brackets[0])
            nb++;
        // buf[k] passed as a (signed) char as before: CP850 accented letters are invalid
        else if(nb == 0 && !(SCR_is_anum(buf[k]) || buf[k] == '_') && buf[k] != ' ')
        {
            i = -1;
            return std::string();
        }
    }

    // no closing parenthesis
    if(k >= buf.size() || buf[k] == '\0')
    {
        i = -1;
        return std::string();
    }

    i = (int) k;
    return buf.substr(start, k - start);
}


/**
 *  Evaluates an @function. 
 *  
 *  @param [out] std::string&  res  calculated value of @fn(...). Empty string on error
 *  @param [in]  const std::string&  str  function name and its arguments (ex: "take(2,abcd)")
 *  @return      int                0 on success, -1 on error (illegal function or @fn() throws RPFunctionError)
 */
int RP_fneval(std::string& res, const std::string& str)
{
    res.clear();
    std::vector<std::string> v_args = split_multi_quoted(str, "(,", '"');
    if(v_args.empty())
        return -1;

    // find the function name (in lowercase)
    std::string name = v_args[0];
    name = trim(name);
    name = to_lower(name);

    auto it = RP_FNS.find(name);
    if(it == RP_FNS.end())
        return -1;

    // execute the @function
    std::vector<std::string> args(v_args.begin() + 1, v_args.end());
    try
    {
        res = it->second.fn(args);
    }
    catch(const RPFunctionError&)
    {
        // invalid call: the text of the @function is left unchanged
        return -1;
    }

    return 0;
}


/**
 *  Evaluates a string containing @-function(s). Can be recursive via a call to RP_expand().
 *  
 *  For example: "upper(@take(5,abcdef))".
 *  
 *  @param [in] const std::string&  str  input string beginning after the @-char. (upper(...), not @upper(...))
 *  @return     std::string              resulting string
 *  @throw      RPFunctionError          if the @function does not exist or cannot be evaluated:
 *                                       the caller (RP_expand()) then leaves the text of the @function unchanged
 */
std::string RP_gfn(const std::string& str)
{
    // expands the macros, LEC expressions and @functions in the arguments
    std::string fn_call;
    if(str.find_first_of("%{@") != std::string::npos)
    {
        RP_expand(fn_call, str);
    }
    else
        fn_call = str;

    std::string value;
    if(RP_fneval(value, fn_call) != 0)
        throw RPFunctionError("invalid @function call: @" + str + ")");
    return value;
}




/**
 *  Expands a report line by replacing macros, lec expressions and @-functions by their calculated values.
 *  
 *  How it works
 *  ------------
 *  A report line is a text including some special sequences that can be interpreted 
 *  and possibly replaced by a calculated value. 
 *  
 *  The resulting (modified) text is then "executed" if it contains an IODE command (in REPFILE::execute()). 
 *  If not, it is sent to the current printing destination.
 *  
 *  Each line is interpreted from left to right. 
 *  As soon as one of the special characters '%', '{' or '@'is encountered, a special treatment is applied.
 *  
 *  Case '%':
 *      - if the character following '%' is also '%', the 2 '%' are replaced by only one '%'. 
 *          
 *          Example: 10%% increase in GNP -> only one % remains in the text
 *          
 *      - otherwise, the next '%' is searched. If the text between the 2 '%', say "macro" is found
 *        in the RP_MACRO kdb, %macro% is replaced by its value.
 *  
 *          Example: 
 *              $define VAR XYZ
 *              the variable %VAR% -> the variable XYZ
 *      
 *      - If "macro" is not found, or if there is no 2d '%', the text is unchanged
 *  
 *          Examples: 
 *              $define MYVAR
 *              the variable "%MYVAR%" is empty  -> the variable "" is empty
 *  
 *              $undef MYVAR
 *              the variable %MYVAR% is empty  -> the variable %MYVAR% is empty
 *              the variable %MYVAR is empty   -> the variable %MYVAR is empty
 *              Add 10% to the price           -> Add 10% to the price
 *                
 *
 *  Case '{': 
 *      - if the following character is '{', only one '{' is kept in the resulting text
 *  
 *          Example: 
 *              List of values {{A,B,C}  -> List of values {A,B,C}  
 *  
 *      - if the following character is '=', the content between braces is considered an Excel 
 *          reference and replaced by the value of the referenced cell(s)
 *  
 *          Example: 
 *              single cell {=Sheet1!R2C2}          -> 12.1
 *              range       {=Sheet1!R2C2:R3C3}     -> 12.1 2.0 3.4 12.5
 *  
 *      - if the following character is '$' or '#', it is a report command that is executed. 
 *          The result (0 on error, or 1 on success) is placed in the resulting text.
 *  
 *              normal command            {$WsLoadVar x.av}  -> {$WsLoadVar x.av}  -> 1
 *              error + inverting result  {$!WsLoadVar toto} -> {$!WsLoadVar toto} -> 1
 *              error                     {$WsLoadVar toto}  -> {$WsLoadVar toto}  -> 0
 *
 *  
 *      - otherwise, the text between {} is calculated as a LEC formula at the current time period 
 *          defined by $SetTime. If the formula ends with @T or @999.999, the result is formatted 
 *          accordingly.
 *  
 *          Examples:     
 *              {1+1.1234}          -> 2.1234
 *              {1+1.1234@9}        -> 2
 *              {1+1.1234@999}      -> 2
 *              {1+1.1234@.9}       -> 2.1
 *              {1+1.1234@.99}      -> 2.12
 *              {1+1@T}             -> 1962Y1
 *  
 *  Case '@':
 *      - if the following character is '@', a single '@' is kept in the resulting text
 *  
 *      - otherwise, the text is read until the closing parenthesis, the corresponding function 
 *          is executed. Note that if there are no parentheses, the text remains unchanged 
 *          (e.g. jmp@plan.be remains as is).
 *  
 *              $define VAL 123.123
 *              {%VAL%@999.9} gb@plan.be        -> 123.1 gb@plan.be
 *              
 *  Case '`': 
 *      - the text is left unchanged until the following ` or the end of line
 *  
 *  @param [out]     std::string&   line    expanded report line
 *  @param [in]      const std::string& buf     partial input report line
 *  @return          int                    0 (always)
 */
int RP_expand(std::string& line, const std::string& buf)
{
    int     i, previ, upper, sqz;
    std::string macro, lec, fn, text;

    line.clear();
    line.reserve(buf.size());

    // i < buf.size(): a '{' at the end of buf makes i jump over the end of the string
    for(i = 0; i < (int) buf.size() && buf[i]; i++)
    {
        upper = 0;
        sqz = 0;
        switch(buf[i]) 
        {
            case '%' :      // macro 
                i++;
                if(buf[i] == '%') 
                    goto dft;
                if(buf[i] == '!')      
                    upper = -1;
                else if(buf[i] == '#') 
                    upper = 1;
                else if(buf[i] == '-') 
                    sqz = 1;
                if(upper || sqz)
                    i++;
                previ = i;
                macro = RP_extract(buf, i, '%');
                if(i >= 0)
                {
                    std::string value;
                    try
                    {
                        value = RP_gmacro(macro);
                    }
                    catch(const RPMacroError&)
                    {
                        // undefined macro or missing argument: the text is left unchanged
                        value = "%" + macro + "%";
                    }
                    // keeps only letters (CP850 accented letters included), digits and '_'
                    if(sqz)
                        std::erase_if(value, [](unsigned char c) { return !(SCR_is_anum(c) || c == '_'); });
                    // SCR_lower() / SCR_upper() are CP850 aware (and do not change the length)
                    if(upper < 0)
                        SCR_lower((unsigned char*) value.data());
                    if(upper > 0)
                        SCR_upper((unsigned char*) value.data());

                    line += value;
                }
                else
                {
                    // no closing '%': i goes back to the character preceding the macro name
                    i = previ - 1;
                    line.push_back(buf[i]);
                }
                break;

            case '{' :      // LEC expression
                i++;
                if(buf[i] == '{')
                    goto dft;
                previ = i;
                lec = RP_extract(buf, i, '}');
                // no closing '}': nothing is added to line and the rest of the line is
                // left as is (i = previ)
                if(i >= 0)
                    line += RP_gcmd(lec);
                else
                    i = previ;
                break;

            case '@' :      // IODE function
                i++;
                if(buf[i] == '@') 
                    goto dft;
                previ = i;  // Keeps the previous value of i in case of error in RP_gfn()
                fn = RP_extractpar(buf, i, "()");
                if(i < 0)
                {
                    // no closing parenthesis: the text is left unchanged
                    i = previ - 1;
                    goto dft;
                }
                try
                {
                    line += RP_gfn(fn);
                }
                catch(const RPFunctionError&)
                {
                    // invalid @function: the text is left unchanged
                    i = previ - 1;
                    goto dft;
                }
                break;

            default  :
                // No interpretation until next backquote
                if(RP_ALLOW_NOPARSING && buf[i] == RP_NOPARSING_CHAR) 
                { 
                    i++;
                    if(buf[i] == RP_NOPARSING_CHAR) 
                        goto dft;  
                    previ = i;
                    text = RP_extract(buf, i, RP_NOPARSING_CHAR);
                    if(i >= 0)
                        line += text;
                    else
                        i = previ;
                    break;
                }

            dft:
                line.push_back(buf[i]);
                break;
        }
    }

    return 0;
}


/**
 *  Executes the report.
 *
 *  The current Report Context (calling report and parameters) is restored at the end of the function.
 *
 *    - saves the calling context CUR_REPFILE (calling report) in the local variable oldrf (for recursivity)
 *    - replaces the current context CUR_REPFILE by this report
 *    - executes the report line by line
 *    - flushes the output
 *    - resets the context to the calling context old_rf
 *
 *  @note the REPFILE object must be owned by a std::shared_ptr (created with std::make_shared).
 *        A local std::shared_ptr (rf) keeps the report alive during its execution.
 *
 *  @return          int            0:  success or error must be ignored (the 2d char of the line is '-')
 *                                  -1: error in one of the commands failed
 *                                  -3: expand failed or abort report(s) execution
 *                                  -4: quit iode
 */
int REPFILE::execute()
{
    std::shared_ptr<REPFILE> rf = shared_from_this();
    int     rc, rt = 0;
    int     ignore = 0; // ignore: 0 ignore code retour, 1 non
    std::string name;
    std::shared_ptr<REPFILE> oldrf;
    char    debug_line[1024];
    std::string line;
    std::string arg;

    oldrf = CUR_REPFILE;    // Save the current report context
    CUR_REPFILE = rf;       // change locally the current report context

    while(1)
    {
        // Execute one line
        line.clear();

        // Reads the next line and expands the %macros%, {lec}, @fns()
        rc = rf->read_line(line, 1);

        // Display the expanded line via kmsg() if RP_DEBUG or RP_STDOUT is not null
        if(RP_DEBUG || RP_STDOUT)
        {
            SCR_strlcpy((unsigned char*) debug_line, (unsigned char*) line.c_str(), 1023);
            SCR_strip((unsigned char*) debug_line);
            if(debug_line[0]) 
            {
                if(RP_DEBUG)
                    kmsg("[%d] - %s", rf->current_line, debug_line);
                else if(RP_STDOUT)
                    kmsg("iode> %s", debug_line);
            }
        }

        // EOF or readline has failed => goto done
        switch(rc) 
        {
            case -2  : // expand of macro failed, or LEC returns a NAN
                rt = -3;
                goto done;
            case EOF :
                goto done;
        }

        // Readline Ok: executes the $command or print the result if no $command found
        // $$ or ## are considered as simple txt
        if((line[0] == '#' || line[0] == '$') && line[1] != line[0]) 
        { 
            if(line[1] == 0 || U_is_in(line[1], " \n\r\t")) 
                continue;
            
            // $command, #command, $-command or #-command
            ignore = chk_ignore(line);     // Ignore the errors if $-command or #-command
            rt = RP_splitline(line.substr(1 + ignore), name, arg, 30);
            
            // Executes the command
            if(rt == 0) 
            {
                if(!name.empty())
                    rt = RP_exec_fn(name.data(), arg.data(), line[0]);
                if(RP_DEBUG > 1) 
                    RP_debug(line.data()); // GB MEM
            }

            CUR_REPFILE = rf; // restore the current REPFILE after RP_exec_fn() (recursivity)
            
            // Manage return codes from RP_exec_fn()
            switch(rt)   
            {
                case -1:                // error
                    if(ignore)          // $-command => rt = 0
                    {        
                        rt = 0;
                        break;
                    }
                    if(RP_PRINT) 
                        err_dump(name.data(), arg.data());
                    rt = RP_RT;         // replaces the error code by the value of RP_RT (defined by OnError)
                    switch(rt)  
                    {
                        case  0:        // OnError ignore => rt = 0
                            break;     
                        case -2:        // OnError return => rt = 0 (and continue the report execution)
                            rt = 0;
                            goto done; 
                        case -3:        // OnError abort  => rt = -3 (abort the current report and the calling ones)
                        case -4:
                            goto done;  // OnError quitode => rt = -4 (quit IODE at the end of the report) 
                        default:
                            goto done;
                    }
                    break;
                case -2:
                    rt = 0;
                    goto done; // return
                case -3:       // abort
                case -4:
                    goto done; // quitode
                default:
                    rt = 0;
                    break;     // no error
            }
        }
        else 
        {
            W_printf((char*) "%s\n", line.c_str());
            continue;
        }
    }

done:
    W_putc('\n');
    W_flush();
    CUR_REPFILE = oldrf; // Reset the calling report context
    return rt;
}


/**
 *  Executes a report file.
 *    - changes the extension to .rep
 *    - reads the file and stores its contents in a REPFILE struct
 *    - executes the REPFILE struct line / line (see REPFILE::execute())
 *    - resets the current report (CUR_REPFILE)
 *  
 *  @param [in] const std::string&   file   report file name
 *  @return     int                         -1 if file cannot be read or
 *                                          REPFILE::execute() return code
 */
int RP_ReportExec_1(const std::string& file)
{
    std::string filename = set_file_extension(file, FILE_REP);
    std::vector<std::string> report_lines = RP_read_file(filename);
    if(report_lines.empty())
    {
        error_manager.append_error("Cannot open file '" + filename + "'");
        return -1;
    }

    std::shared_ptr<REPFILE> rf = std::make_shared<REPFILE>(filename, report_lines);
    int rc = rf->execute();
    CUR_REPFILE = nullptr;
    return rc;
}


/**
 *  Executes a report file (recursive function).
 *  
 *  Can be called from inside a report (via $ReportExec) or directly by a C/C++ call. 
 *  The current Report Context at the time of the call (calling Report and parameters) 
 *  is restored at the end of the function.
 *  
 *  @param [in] char*   line    Sets of report command(s) separated by '\n'
 *  @return     int             0 on success, report error code on error
 */

int B_ReportExec(char* arg, int unused)
{
    int             rc = 0, 
                    o_arg0;

    // Save current report execution context: command = o_arg0 and parameters = o_argv
    std::vector<std::string> o_argv = std::move(RP_ARGV);
    o_arg0 = RP_ARG0;

    std::vector<std::string> v_args = expand_args(arg, 0);
    if(v_args.empty())
    {
        rc = -1;
        goto done;
    }

    if(RP_DEPTH == 0) 
    {
        RP_T = 0;
        memset(&RP_PER, 0, sizeof(Period));
    }
    
    // Changes the report context: v_args[0] is the report filename, the next ones its arguments
    RP_ARGV.assign(v_args.begin() + 1, v_args.end());
    RP_ARG0 = 0;

    // Executes the report v_args[0]
    RP_DEPTH ++; // Recursive level of the current report
    rc = RP_ReportExec_1(v_args[0]);
    RP_DEPTH --;

done:
    // Restores the report context at the time of entering the function
    RP_ARGV = std::move(o_argv);
    RP_ARG0 = o_arg0;
    
    // When exiting the top level report, all $defines are deleted
    if(RP_DEPTH == 0) 
    {
        RP_MACRO->clear();
        W_close(); // TODO: check this !!
    }

    return rc;
}


/**
 *  Executes a set of report lines separated by '\n'. 
 *  
 *  Can be called from inside a report (via $repeat or $foreach for example) or on its own in a C/C++ code. 
 *  The current Report Context (calling report and parameters) is restored at the end of the function.
 *  
 *  @param [in] char*   line    Sets of report command(s) separated by '\n'
 *  @return     int             0 on success, report error code on error
 */

int B_ReportLine(char* line, int cleanup)
{
    int                 rc = 0, o_arg0;
    std::shared_ptr<REPFILE> rf;
    std::vector<std::string> report_lines;

    // Save current report execution context: command o_arg0 and parameters o_argv
    std::vector<std::string> o_argv = std::move(RP_ARGV);
    o_arg0 = RP_ARG0;

    // To mimic B_Report(), args is artificially created as if a report called "temp.rep" were executed
    std::vector<std::string> v_args = expand_args("temp.rep", 0);
    if(v_args.empty())
    {
        rc = -1;
        goto done;
    }

    // Premier rapport ? (début de session de rapport)
    if(RP_DEPTH == 0 && cleanup) 
    {
        RP_T = 0;
        memset(&RP_PER, 0, sizeof(Period));
    }

    // Sauve les arguments pour usage dans les ss fonctions (v_args[0] = "temp.rep")
    RP_ARGV.assign(v_args.begin() + 1, v_args.end());
    RP_ARG0 = 0;
    RP_DEPTH ++;

    // Splits the line on '\n' and skips the empty lines
    if(line)
        report_lines = split_skip_empty(std::string(line), "\n");
    rf = std::make_shared<REPFILE>("temp.rep", report_lines);

    // Exécute la ligne (non expandée à ce stade)
    rc = rf->execute();

    // Remonte un niveau de profondeur d'exécution des rapports
    RP_DEPTH --;

done:
    // Resets previous report context 
    RP_ARGV = std::move(o_argv);
    RP_ARG0 = o_arg0;

    // Si fin des rapports vide les macros
    // Macros ($define) are saved in the KDB RP_MACROS
    if(RP_DEPTH == 0 && cleanup) 
        RP_MACRO->clear();   

    return rc;
}

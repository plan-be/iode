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
 *      RP_ReportExec_1                     Reads a report file and creates a REPFILE structure, then calls RP_ReportExec_tbl()
 *  B_ReportLine                            Creates a REPFILE structure from a string (possibly containing multiple lines separated by '\n') then calls RP_ReportExec_tbl() 
 *      RP_ReportExec_tbl                   Executes a REPFILE structure, ie a table of report lines
 *          RP_readline                     Expands a report line 
 *              RP_read_multiline           Groups lines ended by " \" into one single line
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
char** RP_ARGV = NULL;      // Current report arguments
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
 *  @param [in] const std::shared_ptr<REPFILE>&     rf  a report struct
 *  @return     char*                                   allocated string containing the line(s)
 */

char* RP_read_multiline(const std::shared_ptr<REPFILE>& rf)
{
    char*   multi_line = 0;
    int     lg;
    
    while(1) 
    {
        if(rf->current_line >= rf->nblines) 
            return(multi_line);
        multi_line = (char*) SCR_strafcat((unsigned char*) multi_line,
                                          (unsigned char*) rf->report_lines[rf->current_line].c_str());
        rf->current_line++;
        
        lg = (int) strlen(multi_line);
        if(RP_ALLOW_MULTILINE && lg > 1  && 
           multi_line[lg - 2] == ' ' && multi_line[lg - 1] == '\\') 
        {
                multi_line[lg - 2] = '\n';
                multi_line[lg - 1] = 0;
        }
        else 
            break;
    }        
    
    return multi_line;    
}

/**
 *  Reads the current line from the REPFILE rf.
 *  Expands the line (%define%, {lex expression}...) and stores the expanded line in line.
 *
 *  @param [in, out]    const std::shared_ptr<REPFILE>&     rf      REPFILE struct
 *  @param [out]        std::string&                        line    line (having been expanded or not)
 *  @param [in]         int                                 mode    1 to expand the line,
 *                                                                  0 to keep the line unchanged, for example when searching for $goto or $procend lines
 *  @return             int                                         EOF if the last line is reached
 *                                                                  rc of RP_expand() on error
 *                                                                  0 on success
 */
int RP_readline(const std::shared_ptr<REPFILE>& rf, std::string& line, int mode)
{
    if(rf->current_line >= rf->nblines)
        return EOF;

    char* multi_line = RP_read_multiline(rf);

    int rc = 0;
    if(mode)  // expand macros, {}, ...
        rc = RP_expand(line, multi_line);
    else
        line = std::string(multi_line);

    SCR_free(multi_line);
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
 *  @param [in] char*   line    report line 
 *  @return     int             0 if the error must be ignored
 *                              1 else
 */
int RP_chk_ignore(char* line)
{
    if(line[1] == '-') 
        return(1);
    return 0;
}


/**
 *  Splits a report line into the command and its arguments.
 *  
 *  @param [in, out] char*          text   line to split
 *  @param [in, out] char*          cmd    command (first word in text)
 *  @param [out]     std::string&   arg    arguments (text following the command, without leading
 *                                         and trailing spaces)
 *  @param [in] lg   int                   max length of cmd
 *  @return          int                   -1 if the name of the cmd (first word on the line text) is too long (> lg)
 *                                         0 otherwise
 */
int RP_splitline(char* text, char* cmd, std::string& arg, int lg)
{
    int     i = 0, j = 0, rt = 0;

    char*   args;

    if(text == NULL)
    {
        cmd[0] = 0;
        arg.clear();
        return -1;
    }

    if(text[0] == '\0' || text[0] == '\n')
    {
        rt = -1;
        goto done;
    }

    while(1) 
    {
        if(SCR_is_anum(text[j]) || text[j] == '_') 
        {
            if(i < lg)  
                cmd[i] = text[j];
        }
        else 
            break;

        i++;
        j++;
    }

done :
    cmd[i] = 0;
    args = text + j;
    U_ljust_text((unsigned char*) args);
    SCR_strip((unsigned char*) args);
    arg = std::string(args);
    return rt;
}


/**
 *  Returns the BFNS struct corresponding to the command name.
 *  
 *  See in b_rep_syntax.c the definition of the table B_fns for more details.
 *  
 *  @global [in]     BFNS   B_fns   table of command names and fn pointers
 *  @param [in, out] char*  name    command name ($name)
 *  @param [out]     int*   type    type of object or file to which the command must be assigned
 *  @param [in]      int    fs      full screen : character preceding the command in the report ('#' or '$').
 *  @return          BFNS*          BFNS struct corresponding to the command name (nullptr if not found)  
 *  
 */
BFNS* RP_find_fn(char* c_name, int* type, int fs)
{
    std::string name = std::string(c_name);
    name = trim(name);
    name = to_lower(name);

    size_t n;
    std::string prefix;
    std::string suffix;
    std::string extension;
    for(auto& [func_name, fns]: B_fns) 
    {
        // command without extension 
        if(fns.type == 0) 
        {
            if(name == func_name) 
                return &fns;
            continue;
        }
        else 
        {
            n = func_name.length();
            prefix = name.substr(0, n);
            if(prefix != func_name) 
                continue;
            
            for(int j = 0; k_ext[j]; j++) 
            {
                if(j > VARIABLES && fns.type % 2) 
                    break;
                
                suffix = name.substr(n);
                extension = std::string(k_ext[j]);
                if(suffix == extension) 
                {
                    *type = j;
                    return &fns;
                }
            }
            
            // if(fns.type > 2 && fs == "#") { => erreur "" => ''
            if(fns.type > 2 && fs == '#') 
            {
                *type = 0;
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

    BFNS* fns = RP_find_fn(name, &parm, fs);
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
int RP_err_dump(char* name, char* arg)
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
 *  Extracts from (buf + *i) the string terminated by the specific character ch (eg '%' or '}').
 *  Increments *i, the current position in buf.
 *  
 *  If ch is not found in buf, the result goes to the end of buf.
 *  
 *  Example:
 *      str = "Value: {A[1990Y1] + 2} EUR";
 *             01234567890123456789012345 
 *      pos = 8; // starting search position = 1 + position of '{'
 *      RP_extract(str, &pos, '}') => "A[1990Y1] + 2", pos = 22
 *      
 *  @param [in]      char*    buf     input string
 *  @param [in, out] int*     i       position of the current pointer in buf (=first character in the resulting string)
 *  @param [in]      int      ch      ending character
 *  @return          char*            allocated string 
 */
char *RP_extract(char* buf, int* i, int ch)
{
    int     pos, j;
    char    *res;

    pos = U_pos(ch, ((unsigned char*) buf) + (*i));
    if(pos < 0) 
        return NULL;

    res = RP_alloc(pos + 1);
    for(j = 0; j < pos; j++, (*i)++) 
        res[j] = buf[*i];

    return res;
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
 *  @param [in] char*   str     pointer to the first character following % on the report line
 *  @return     char*           allocated value of the macro after transformation
 */
char *RP_gmacro(char* str) 
{
    int     pos;
    char    *res = NULL, buf[20], *tmp = 0;

    if(str == NULL) 
        return NULL;

    if(U_pos('%', (unsigned char*) str) >= 0 || U_pos('{', (unsigned char*) str) >= 0 ||
       U_pos('@', (unsigned char*) str) >= 0)
    {
        std::string expanded;
        RP_expand(expanded, str);
        tmp = (char*) SCR_stracpy((unsigned char*) expanded.c_str());
    }
    else
        tmp = (char*) SCR_stracpy((unsigned char*) str);

    if(tmp[0] >= '0' && tmp[0] <= '9') 
    {
        pos = atoi(tmp);

        if(pos == 0) 
        {
            sprintf(buf, "%d", SCR_tbl_size((unsigned char**) RP_ARGV) - RP_ARG0);
            res = (char*) SCR_stracpy((unsigned char*) buf);
        }
        else 
        {
            pos += RP_ARG0;
            if(pos < 0 || pos > SCR_tbl_size((unsigned char**) RP_ARGV)) {
                error_manager.append_error("Report : Argument (" + std::to_string(pos) + ") is missing");
                SCR_free(tmp); // JMP 24/8/2012
                return NULL;
            }
            res = (char*) SCR_stracpy((unsigned char*) RP_ARGV[pos - 1]);
        }
    }
    else 
    {
        if(tmp[0] == '*') 
            res = (char*) SCR_mtov(((unsigned char**) RP_ARGV) + RP_ARG0, ';');
        else 
        {
            std::string name(copy_char_array(tmp));
            name = RP_MACRO->to_key(name);
            if(!RP_MACRO->contains(name)) 
            {
                std::string error_msg = "Report: Macro '" + name + "' is not defined";
                kwarning(error_msg.c_str());
                SCR_free(tmp);
                return NULL;
            }
            std::string macro = RP_MACRO->get_macro(name);
            res = (char*) SCR_stracpy((unsigned char*) macro.c_str());
        }
    }

    SCR_free(tmp);
    return res;
}


/**
 *  Calculates the value of an expression between {curly brackets}.
 *  
 *  Before evaluating the value, expands the line contents by replacing expressions bw %% and the @-functions 
 *  by their calculated values (@see RP_expand()).
 *  
 *  @param [in] char*   str     report line containing optionaly macros, lec expressions and @-functions
 *  @return     char*           allocated report line after replacement of macros, lec expressions and @-functions
 */
char *RP_gcmd(char* str) 
{
    char    *tmp = 0;

    if(str == NULL)
        return NULL;

    if(U_pos('%', (unsigned char*) str) >= 0 ||
       U_pos('{', (unsigned char*) str) >= 0 ||
       U_pos('@', (unsigned char*) str) >= 0)
    {
        std::string expanded;
        RP_expand(expanded, str);
        tmp = (char*) SCR_stracpy((unsigned char*) expanded.c_str());
    }
    else
        tmp = (char*) SCR_stracpy((unsigned char*) str);

    std::string res;
    RP_eval(res, tmp);
    SCR_free(tmp);
    return (char*) SCR_stracpy((unsigned char*) res.c_str());
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
 *  @param [in, out]    char*       lec     LEC expression or empty string (the expression is first stripped, hence the [out]).
 *  @return             double              IODE_NAN on error or if RP_PER is before the current WS sample
 *                                          calculated LEC value on success
 */
double RP_evallec(char* c_lec)
{
    if(RP_evaltime() < 0) 
        return IODE_NAN;

    std::string lec(c_lec);
    lec = trim(lec);
    double x = IODE_NAN;
    std::shared_ptr<CLEC> clec = nullptr;
    if(lec[0]) 
    {
        try
        {
            clec = std::make_shared<CLEC>(lec);
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
 *  @param [in, out] char*       buf        formatted double
 *  @param [in]      char*       format     format (see above)
 *  @param [in]      double   value      any double 
 *  @return          int                    0 always              
 */
int RP_fmt(char* buf, char* format, double value)
{
    int     t, lg = 20, nbdec = -1;
    char    **fmt;

    if(format == NULL) 
        goto normal;

    U_ljust_text((unsigned char*) format);
    SCR_strip((unsigned char*) format);
    SCR_upper((unsigned char*) format);
    if(format[0] == '\0') 
        goto normal;

    if(format[0] == 'T') 
    {
        t = (int) value;
        Period per = global_ws_var->get_sample()->start_period.shift(t);
        strcpy(buf, (char*) per.to_string().c_str());
        return 0;
    }

    fmt = (char**) SCR_vtom((unsigned char*) format, (int) '.');
    if(fmt == NULL || fmt[0] == NULL) 
        goto normal;

    lg = (int)strlen(fmt[0]);
    if(fmt[1] != NULL) 
    {
        nbdec = (int)strlen(fmt[1]);
        lg += nbdec + 1;
    }
    else if(format[0] == '.') 
    {
        lg = 30;
        nbdec = (int)strlen(fmt[0]);
    }
    else 
        nbdec = 0;

    SCR_free_tbl((unsigned char**) fmt);

normal:
    if(value > 0) 
        SCR_fmt_dbl((double) value, (unsigned char*) buf, lg, nbdec);
    else          
        SCR_fmt_dbl((double) value, (unsigned char*) buf, lg + 1, nbdec);
    SCR_sqz((unsigned char*) buf);
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
 *  @param [in]      char*          farg   contents to be interpreted
 *  @return          int                   0 on success, -1 on error
 */
int RP_eval(std::string& res, char* farg)
{
    char        ch, name[31],
                *lec, *fmt;
    int         rc, rp_rt, pos;
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
        
        // Retrieve the command name (max 30 chars)
        get_next_arg(name, farg + 1 + (int) inv, 30);

        // Execute the function name
        rc = RP_exec_fn(name, farg + strlen(name) + 2, ch);
        
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
        char* item = B_ExcelGetItem(farg + 1);
        if(item == NULL)
            return -1;
        SCR_replace((unsigned char*) item, (unsigned char*) "\t", (unsigned char*) " ");
        SCR_replace((unsigned char*) item, (unsigned char*) "\r\n", (unsigned char*) " ");
        res = std::string(item);
        SCR_free(item);
        return 0;
    }

    // farg = "lec_expression" or "lec_expression@format"
    lec = farg;    
    fmt = NULL;
    pos = U_pos('@', (unsigned char*) farg);
    if(pos >= 0) 
    {
        farg[pos] = '\0';       // LEC formula ends with a 0 
        fmt = farg + pos + 1;   // format 
    }

    x = RP_evallec(lec);
    // TODO: reset farg[pos] = '@' ?
    // Empty string on error
    if(!IODE_IS_A_NUMBER(x)) 
        return -1;
    
    char buf[41] = { 0 };
    RP_fmt(buf, fmt, x);       // The formatted value is returned
    res = std::string(buf);
    return 0;
}



// ========= Report "@functions" interpretation and execution =========================

/**
 *  Extracts from (buf + *i) the string terminated by a closing parenthesis or bracket of level 0. 
 *  Each new opening parenthesis raises the level by 1, each closing lowers the level by 1.
 *  The parenthesis between double quotes are ignored.
 *  
 *  @note the function name is RP_extractpar because originally the idea was to use the 
 *  same function for different types of parentheses: '[]', '{}', '()'..., not only for functions 
 *  but also for {lec} formulas for example.
 *  
 *  If no closing parenthesis is found, returns NULL.
 *  
 *  Increments *i, the current position in buf.
 *  
 *  Example:
 *      str          = "Zip: @upper(@take(10,"b-(1000)")) ";
 *      position        0123456789012345678901234567890123
 *      level           0000000000010000022222222222222100
 *  
 *      pos = 6;
 *      RP_extractpar(str, &pos, '()') => "upper(@take(10,b-1000))", pos = 29
 *      
 *  @param [in]      char*    buf     input string
 *  @param [in, out] int*     i       in: position of the current pointer in buf (=first character in the resulting string)
 *                                    out: position of the next character in buf after the closing parenthesis. 
 *                                    If no closing par is found, unchanged
 *  @param [in]      int      ch      opening and ending character (eg "()" or "[]")
 *  @return          char*            allocated string containing the string bw parentheses
 *                                    NULL is no closing parenthesis of level 0 is found
 */
char *RP_extractpar(char* buf, int* i, char* brackets)
{
    int     nb = 0, k, instr = 0;
    char    *res;

    for(k = *i ; buf[k] ; k++) 
    {
        if(buf[k] == '"') 
        {
            if(instr) 
                instr = 0;
            else      
                instr = 1;
            continue;
        }
        if(instr) 
            continue;
        if(buf[k] == brackets[1] && nb <= 1) 
            break;
        if(buf[k] == brackets[1]) 
            nb--;
        else
        {
            if(buf[k] == brackets[0]) 
                nb++;   
            else
            {
                if(nb == 0 && !(SCR_is_anum(buf[k]) || buf[k] == '_') && buf[k] != ' ') 
                    return NULL;    
            }
        } 
    }

    if(buf[k] == 0) 
        return NULL;

    k -= *i;
    res = SW_nalloc(k + 1);
    memcpy(res, buf + *i, k);
    *i += k;

    return res;
}


/**
 *  Evaluates an @function. 
 *  
 *  @param [in, out] char**  res    pointer to the allocated string containing @fn(...) 
 *                                  calculated value. *res = NULL on error
 *  @param [in]      char*   str    function name and its arguments (ex: "take(2,abcd)")
 *  @return          int            0 on success, -1 on error (illegal function or @fn() returns NULL
 */
int RP_fneval(char** res, char* str)
{
    *res = 0;
    std::vector<std::string> v_args = split_multi_quoted(str, "(,", '"');
    
    // find the function name (in lowercase)
    std::string name = v_args[0];
    name = trim(name);
    name = to_lower(name);

    int rc = -1;
    char** tbl_args = vector_to_double_char(v_args);
    for(auto& [func_name, fns]: RP_FNS)
    {
        if(name == func_name)
        {
            // execute the @function 
            *res = (char*) fns.fn((unsigned char**) tbl_args + 1);
            if(*res == NULL) 
                rc = -1;
            rc = 0;
        }
    }

    SCR_free_tbl((unsigned char**) tbl_args);
    return rc;
}


/**
 *  Evaluates a string containing @-function(s). Can be recursive via a call to RP_expand().
 *  
 *  For example: "upper(@take(5,abcdef))".
 *  
 *  @param [in] U_ch*   str     input string beginning after the @-char. (upper(...), not @upper(...))
 *  @return     U_ch*           allocated resulting string
 */
U_ch *RP_gfn(U_ch* str)
{
    if(str == NULL) 
        return NULL;

    U_ch* res = NULL;
    int  allc = 0;
    if(U_pos('%', str) >= 0 || U_pos('{', str) >= 0 || U_pos('@', str) >= 0) 
    {
        std::string expanded;
        RP_expand(expanded, (char*) str);
        str = SCR_stracpy((unsigned char*) expanded.c_str());
        allc = 1;
    }

    RP_fneval((char**) &res, (char*) str);
    if(allc) 
        SCR_free(str);
    return res;
}




/**
 *  Expands a report line by replacing macros, lec expressions and @-functions by their calculated values.
 *  
 *  How it works
 *  ------------
 *  A report line is a text including some special sequences that can be interpreted 
 *  and possibly replaced by a calculated value. 
 *  
 *  The resulting (modified) text is then "executed" if it contains an IODE command (in RP_ReportExec_tbl). 
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
 *  @param [in]      char*          buf     partial input report line
 *  @return          int                    0 (always)
 */
int RP_expand(std::string& line, char* buf)
{
    int     i, previ, upper, sqz;
    char    *w = NULL, *res = NULL;

    line.clear();
    line.reserve(strlen(buf));

    for(i = 0; buf[i]; i++)
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
                w = RP_extract(buf, &i, '%');
                if(w != NULL) 
                {
                    res = RP_gmacro(w);
                    if(res == 0) 
                    {
                        res = SW_nalloc(10 + (int)strlen(w));
                        sprintf(res, "%%%s%%", w);
                    }
                    if(sqz) 
                        SCR_asqz((unsigned char*) res, (unsigned char*) "_");
                    if(upper < 0) 
                        SCR_lower((unsigned char*) res);
                    if(upper > 0)
                        SCR_upper((unsigned char*) res);

                    line += res;
                    SW_nfree(res);
                    RP_free(w);
                }
                else
                {
                    i--;
                    line.push_back(buf[i]);
                }
                break;

            case '{' :      // LEC expression
                i++;
                if(buf[i] == '{') 
                    goto dft;
                w = RP_extract(buf, &i, '}');
                res = RP_gcmd(w);
                if(res)
                    line += res;

                SW_nfree(res);
                RP_free(w);
                break;

            case '@' :      // IODE function
                i++;
                if(buf[i] == '@') 
                    goto dft;
                previ = i;  // Keeps the previous value of i in case of error in RP_gfn()
                w = RP_extractpar(buf, &i, "()");
                res = (char*) RP_gfn((unsigned char*) w);
                if(res == 0) 
                {
                    i = previ - 1;
                    SCR_free(w);
                    goto dft;
                }
                line += res;

                SW_nfree(res);
                SW_nfree(w);
                break;

            default  :
                // No interpretation until next backquote
                if(RP_ALLOW_NOPARSING && buf[i] == RP_NOPARSING_CHAR) 
                { 
                    i++;
                    if(buf[i] == RP_NOPARSING_CHAR) 
                        goto dft;  
                    w = RP_extract(buf, &i, RP_NOPARSING_CHAR);
                    if(w)
                        line += w;
                    RP_free(w);
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
 *  Executes the report stored in the REPFILE struct rf.
 *  
 *  The current Report Context (calling report and parameters) is restored at the end of the function.
 *  
 *    - saves the calling context CUR_REPFILE (calling report) in the local variable oldrf (for recursivity)
 *    - replaces the current context CUR_FILE by rf 
 *    - executes the REPFILE rf line by line
 *    - flushes the output
 *    - resets the context to the calling context old_rf
 *  
 *  @param [in, out] std::shared_ptr<REPFILE>   rf  report to execute (passed by value to keep the report
 *                                                  alive during its execution)
 *  @return          int            0:  success or error must be ignored (the 2d char of the line is '-')
 *                                  -1: error in one of the commands failed
 *                                  -3: expand failed or abort report(s) execution
 *                                  -4: quit iode 
 */
int RP_ReportExec_tbl(std::shared_ptr<REPFILE> rf)
{
    int     rc, rt = 0;
    int     ignore = 0; // ignore: 0 ignore code retour, 1 non
    char    name[31];
    long    cline = 0L;
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
        rc = RP_readline(rf, line, 1);

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
            ignore = RP_chk_ignore(line.data());     // Ignore the errors if $-command or #-command
            rt = RP_splitline(line.data() + 1 + ignore, name, arg, 30);
            
            // Executes the command
            if(rt == 0) 
            {
                if(name[0]) 
                    rt = RP_exec_fn(name, arg.data(), line[0]);
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
                        RP_err_dump(name, arg.data());
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
 *    - executes the REPFILE struct line / line (see RP_ReportExec_tbl)
 *    - resets the current report (CUR_REPFILE)
 *  
 *  @param [in] const std::string&   file   report file name
 *  @return     int                         -1 if file cannot be read or
 *                                          RP_ReportExec_tbl() return code
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
    int rc = RP_ReportExec_tbl(rf);
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
    unsigned char   **o_argv;
    int             rc = 0, 
                    o_arg0;

    // Save current report execution context: command = o_arg0 and parameters = o_argv
    o_argv = (unsigned char**) RP_ARGV;
    o_arg0 = RP_ARG0;

    std::vector<std::string> v_args = expand_args(arg, 0);
    std::vector<char*> argv;
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
    
    argv.reserve(v_args.size() + 1);
    for(std::string& item : v_args)
        argv.push_back(item.data());
    argv.push_back(NULL);

    // Changes the report context
    RP_ARGV = argv.data() + 1;
    RP_ARG0 = 0;

    // Executes the report argv[0]
    RP_DEPTH ++; // Recursive level of the current report
    rc = RP_ReportExec_1(argv[0]);
    RP_DEPTH --;

done:
    // Restores the report context at the time of entering the function
    RP_ARGV = (char**) o_argv;
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
    unsigned char       **o_argv;
    int                 rc = 0, o_arg0;
    std::shared_ptr<REPFILE> rf;
    std::vector<std::string> report_lines;

    // Save current report execution context: command o_arg0 and parameters o_argv
    o_argv = (unsigned char**) RP_ARGV;
    o_arg0 = RP_ARG0;

    // To mimic B_Report(), args is artificially created as if a report called "temp.rep" were executed
    std::vector<std::string> v_args = expand_args("temp.rep", 0);
    std::vector<char*> argv;
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

    argv.reserve(v_args.size() + 1);
    for(std::string& item : v_args)
        argv.push_back(item.data());
    argv.push_back(NULL);

    // Sauve les arguments pour usage dans les ss fonctions
    RP_ARGV = argv.data() + 1;
    RP_ARG0 = 0;
    RP_DEPTH ++;

    // Splits the line on '\n' and skips the empty lines
    if(line)
    {
        for(const std::string& report_line : split(std::string(line), '\n'))
            if(!report_line.empty())
                report_lines.push_back(report_line);
    }
    rf = std::make_shared<REPFILE>("temp.rep", report_lines);

    // Exécute la ligne (non expandée à ce stade)
    rc = RP_ReportExec_tbl(rf);

    // Remonte un niveau de profondeur d'exécution des rapports
    RP_DEPTH --;

done:
    // Resets previous report context 
    RP_ARGV = (char**) o_argv;
    RP_ARG0 = o_arg0;

    // Si fin des rapports vide les macros
    // Macros ($define) are saved in the KDB RP_MACROS
    if(RP_DEPTH == 0 && cleanup) 
        RP_MACRO->clear();   

    return rc;
}

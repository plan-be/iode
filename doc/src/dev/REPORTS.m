<IODE: Reports>
IODE: Reports
¯¯¯¯¯¯¯¯¯¯¯¯
..sep |
..esc ~
..ignore ¯

Three groups of functions that manage reports can be identified:
&EN ~b<IODE Report Engine>~B: compilation and execution of report commands, like ~c$goto~C, ~c$define~C, ~c$foreach~C...
&EN ~b<IODE Report Commands>~B: create, update, etc IODE objects, like ~c$DataCreateVar~C, ~c$FileDeleteTxt~C...
&EN ~b<IODE Report Functions>~B: calculation of @-functions and replacement by their computed value, like ~c@upper()~C, ~c@time()~C...

The available commands and functions are defined in two tables (defined in b_rep_syntax.c)
that contain the names of the functions and commands, and a pointer to their C-functions.

Adding a command or function can easily be done by modifying one of these tables.

&IT Examples

&EN <Example of report with @-functions>
&EN <Example of report with $procdef and $foreach>

>

<IODE Report Engine>
IODE Report engine
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
That group of functions is responsible for parsing and executing IODE reports.
They manage the ~bcontrol flow~B and call the IODE commands (~c$command~C or ~c##command~C) and IODE functions (~c@function~C).

Generally (but not always), a report is stored in a text file with the suffix ".rep".
Before being "run" or "executed", the report file is read and stored in a REPFILE structure.
That structure is then "executed" line by line.

Internally, a report consists in a table of lines.
Each line is first interpreted by the Report Engine (RE) and
the result is sent to the current printing destination, for example a htlm file.

&TI More details on report structure
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Reports consist in a set of lines that are interpreted in sequence.
There are 3 types of lines:

1. lines that start with $ or ## are IODE commands that are "executed". Their
      result, if any, is printed (or ignored) according to the last ~c$PrintDest~C command.
      The list of currently available "commands" can be found in b_rep_syntax.c

2. lines that start with a $ or ## followed by a space are comments.

3. all other lines are plain text that will be sent to the current Printing Destination
      after parsing

&TI Line Parsing
¯¯¯¯¯¯¯¯¯¯¯¯
~iBefore~I the execution of the command it may contain, each line is parsed and the following
character sequences are replaced by calculated values:

&EN ~c%macro%~C: macros (between %%), created by a $define command, are replaced by their current value

&EN ~c{lex}~C: LEC expressions (between {}) are calculated and replaced by their (formatted) value

&EN ~c{$command}~C: IODE commands between {} are executed and their result replaces the
    original {} sequence. See the variable B_fns in b_rep_syntax.c for the current list of available commands.

&EN ~c@function()~C: report functions (@function()) are executed and replaced by their value.
    See RP_FNS in b_rep_syntax.c for the current list of functions.

&EN ~c`any expression`~C: strings between backquotes remain as is (are not parsed)
unless otherwise specified by a ~c$noparsing 0~C command.


&TI List of source files
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
The files below interpret and execute reports and report lines.

&EN <b_rep_engine.c>:   main report engine functions
&EN <b_rep_cmds.c>:     functions controlling the report flow ($debug...) and some os commands ($mkdir...)
&EN <b_rep_debug.c>:    debugging function
&EN <b_rep_defs.c>:     functions to manage macros ($defines)
&EN <b_rep_foreach.c>:  functions to execute $foreach commands
&EN <b_rep_proc.c>:     compilation and execution of report procedures ($prodef, $procexec)
&EN <b_rep_syntax.c>:   tables of command and function names and pointers
&EN <b_rep_utils.c>:    utilities for report execution
>

<b_rep_engine.c>
b_rep_engine.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Main report engine functions.

&IT Hierarchy of the report engine functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
B_ReportExec and B_ReportLine are the 2 top level functions. As shown below, some functions
are recursive (RP_expand(), RP_gmacro(), RP_gcmd(), RP_gfn()).

&CO
B_ReportExec calls
    RP_ReportExec_1                     Reads a report file and creates a REPFILE structure, then calls RP_ReportExec_tbl()
B_ReportLine                            Creates a REPFILE structure from a string (possibly containing multiple lines separated by '\n') then calls RP_ReportExec_tbl()
    RP_ReportExec_tbl                   Executes a REPFILE structure, ie a table of report lines
        RP_readline                     Expands a report line
            RP_read_multiline           Groups lines ended by " \" into one single line
            RP_expand (recursive)       Replaces %macro%, {lec}, @fn()
                RP_extract              Extracts a string bw %% or {}
                RP_gmacro               Replaces %macro% par its value
                    RP_expand...
                RP_gcmd                 Calculates {macro} and put the result in the report line
                    RP_expand...
                RP_extractpar           Extracts an @function
                RP_gfn                  Executes @functions and put the result in the report line
                    RP_expand...
                RP_add                  Util : concatenates a string to the current line and reallocates
        RP_exec_fn()                    Executes the report command (if any)
            RP_find_fn                  Finds the right function
            (*B_fns[i].fn)              Executes the no-GUI function
            (*B_fns[i].sfn)             Executes the GUI function

&TX

&IT Examples of report line parsing
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
The examples below show how report lines are transformed before being executed.

Note that this is not the exact result of a report execution: to keep some expression
in the result, like ~c{1 + %C%}~C, the source report had to contain more backquotes than shown below.

&CO
     Creation of the variable B and the 3 macros c, C and C1
     -------------------------------------------------------
     $WsSample 2000Y1 2010Y1
     $DataCalcVar B t
     $define C   1
     $define C1  2
     $define c   b

     Set the current execution period
     --------------------------------
     $SetTime 2005Y1

     Expressions between {...}
     -------------------------
     Note that {} expressions cannot be nested

     Example 1: {lec + %define%}      : {1 + %C%}         ->> {1 + 1}                      ->> 2
     Example 2: {lec + @fn()}         : {1 + @upper(b)}   ->> {1 + B}                      ->> 6
     Example 3: {lec + %@fn()%}       : {1 + %@upper(c)%} ->> {1 + %C%}       ->> {1 + 1}  ->> 2
     Example 4: {lec + @fn(%define%)} : {1 + @upper(%c%)} ->> {1 + @upper(b)} ->> {1 + B}  ->> 6

     Expressions between %...%
     -------------------------
     Note that %defines% cannot be nested
     Example 1: %C%               : %C%                  ->> 1
     Example 2: %@fn()%           : %@upper(c)% ->> %C%  ->> 1
     Example 3: %C{lec}%          : %C{2-1}%    ->> %C1% ->> 2

     Expressions @-functions()
     -------------------------
     Example 1: @fn()             : @date() @time()        ->> 14-10-2022 01:19:44
     Example 2: @fn(%def%)        : @upper(%c%)            ->> B
     Example 3: @fn(@fn())        : @upper(@getdir())      ->> C:\USR\IODE_SRC\CMD
     Example 4: @fn({lec})        : @drop({1+1},ABCDE)     ->> CDE
     Example 5: @fn({lec})        : @drop({1+1},A BCDE)    ->> BCDE
     Example 6: @fn({lec})        : @drop({1+1},"A,BC,DE") ->> BC,DE
&TX

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                              | Description
| ~cREPFILE *RP_create_repfile(char *filename, unsigned char **tbl)   | Creates and initialises a REPFILE struct.
| ~cint RP_free_repfile(REPFILE *rf)                                  | Frees a REPFILE struct.
| ~cunsigned char **RP_read_file(char* filename)                      | Reads a report file (with no line expansion).
| ~cchar* RP_read_multiline(REPFILE* rf)                              | Reads the next line in a REPFILE struct. If the line ends with a " \\", joins the next line.
| ~cint RP_readline(REPFILE* rf, char** line, int mode)               | Reads the current line from the REPFILE rf.
| ~cint RP_chk_ignore(char* line)                                     | Indicates if errors on a report line must be ignored
| ~cint RP_splitline(char* text, char* cmd, char** arg, int lg)       | Splits a report line into the command and its arguments.
| ~cint RP_find_fn(char* name, int* type, int fs)                     | Finds the position in B_fns of the command "name" and, if there is a suffix, the corresponding object or file type.
| ~cint RP_exec_fn(char* name, char* arg, int fs)                     | Retrieves a report command function pointer and executes the function with its parameters
| ~cint RP_err_dump(char* name, char* arg)                            | Prints or displays the current report line if an error has occured.
| ~cchar *RP_extract(char* buf, int* i, int ch)                       | Extracts a string terminated by the specific character.
| ~cchar *RP_gmacro(char* str)                                        | Calculates a macro value. The macro may contain expressions between {} or @-functions
| ~cchar *RP_gcmd(char* str)                                          | Calculates the value of an expression between curly brackets.
| ~cint RP_evaltime()                                                 | Calculates RP_T, the position in the current sample of the current calculation period RP_PER.
| ~cIODE_REAL RP_evallec(char* lec)                                   | Evaluates a LEC expression in (the period) RP_PER.
| ~cint RP_fmt(char* buf, char* format, IODE_REAL value)              | Formats a double.
| ~cint RP_eval(char** res, char* farg)                               | Interprets strings between accolades found in a report line.
| ~cint RP_add(char** line, int* lg, int* j, char* res)               | Reallocates the string line (of length *lg) in order to concatanate res from the position *j.
| ~cint RP_expand(char** line, char* buf)                             | Expands a report line by replacing macros, lec expressions and @-functions par their calculated values
| ~cint RP_ReportExec_tbl(REPFILE *rf)                                | Executes the report stored in the REPFILE struct rf.
| ~cint RP_ReportExec_1(char* file)                                   | Executes a report file.
..te

>

<b_rep_cmds.c>
b_rep_cmds.c
¯¯¯¯¯¯¯¯¯¯¯¯
Commands related to the control flow of IODE reports ($msg, $goto, $return...) and
to some operating system functions ($mkdir...).
Some other commands of the same group can be found in other source files: b_rep_defs.c, b_rep_foreach.c,
b_rep_proc.c, etc.

The names and C-function pointers of these functions are defined in the table ~cB_fns[]~C.
The functions themselves are called by ~cRP_exec_fn()~C.

The syntax of each function can be found in the online manual:
     https://iode.plan.be/doku.php?id=les_commandes_d_execution_des_rapports.

All functions have the same syntax:

&CO
    int <<function>>(char* arg)

    @param [in] char*   arg     parameters of the function
    @return     int             return code: 0 on success
&TX

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
..sep @
@ Syntax                                           @ Description
@ ~cint RP_vseps(char* seps)                       @ $vseps <<seps>>
@ ~cint RP_repeatstring(char* buf)                 @ $repeatstring <<string>>
@ ~cint RP_repeat(char* buf)                       @ $repeat <<command>>
@ ~cint RP_onerror_1(char* arg)                    @ Sub function of RP_onerror() which assigns the 2 global variables RP_RT and RP_PRINT.
@ ~cint RP_onerror(char* arg)                      @ $onerror [{return|abort|quitode}] [{display|print|noprint}]
@ ~cint RP_abort(char* arg)                        @ $abort
@ ~cint RP_quitode(char* arg)                      @ $quitode or $quit
@ ~cint RP_return(char* arg)                       @ $return
@ ~cint RP_label(char* arg)                        @ $label <<label>>
@ ~cint RP_goto_label(char *command, char *parm)   @ Search in the current REPFILE (CUR_REPFILE) the line beginning with "$command parm" or "##command parm", where command can be "label" or "next".
@ ~cint RP_goto(char* arg)                         @ $goto label [value]
@ ~cint RP_message(char* arg)                      @ $show message
@ ~cint RP_warning(char* arg)                      @ $msg text
@ ~cint RP_beep()                                  @ $beep
@ ~cint RP_ask(char* arg)                          @ $ask <<label>> <<question>>
@ ~cint B_ReportPrompt(char* arg)                  @ ##prompt <<macro_name>> <<question>>
@ ~cint RP_setdebug(char* arg)                     @ $debug {0|n|N|1|2|f|F}
@ ~cint RP_setindent(char* arg)                    @ $indent {0|n|N|1}
@ ~cint RP_setmultiline(char* arg)                 @ $multiline {0|n|N|1}
@ ~cint RP_noparsingchar(char* arg)                @ $noparsing [0|n|N|1]
@ ~cint RP_shift(char* arg)                        @ $shift [n] or $shift_args [n]
@ ~cint RP_chdir(char* arg)                        @ $chdir <<dirname>>
@ ~cint RP_rmdir(char* arg)                        @ $rmdir <<dirname>>
@ ~cint RP_mkdir(char* arg)                        @ $mkdir <<dirname>>
@ ~cint RP_settime(char* arg)                      @ $settime <<period>>
@ ~cint RP_incrtime(char* arg)                     @ $incrtime [n]
@ ~cint RP_system(char* arg)                       @ $system <<command>>
@ ~cint B_shellexec(char *arg)                     @ $shellexec <<command>>
@ ~cint B_Sleep(char* arg)                         @ $sleep <<msecs>>
@ ~cint B_GraphDefault(char* type)                 @ $graphdefault {l|L|s|S|b|B}
..te
..sep |
>

<b_rep_debug.c>
b_rep_debug.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Function to send a message and the memory status to the file ~c./memdbg.txt~C.

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                     | Description
| ~cvoid RP_debug(char* txt) | Appends a message to the report debug file memdbg.txt.
..te

>

<b_rep_defs.c>
b_rep_defs.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Implementation of the report $defines and of the mechanism to push/pop macros in PROCDEF
and FOREACH blocks.

The $defines are stored in the special ~cKDB RP_MACRO~C of type ASIS_CASE.

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                      | Description
| ~cint RP_macro_createdb()                   | Creates the KDB RP_MACRO if it does not exist.
| ~cint RP_macro_deletedb()                   | Deletes the KDB RP_MACRO and its content.
| ~cint RP_define_1(char *name, char *macro)  | Adds or replaces a macro to RP_MACRO.
| ~cint RP_define(char* arg)                  | Report function to define a new macro.
| ~cchar* RP_get_macro_ptr(char* macro_name)  | Returns the pointer to a macro (aka define) value.
| ~cint RP_undef_1(char *name)                | Deletes one macro.
| ~cint RP_undef(char *arg)                   | Report function to delete macros.
| ~cint RP_define_calcdepth(char *name)       | Returns the max depth of a saved (pushed) macro.
| ~cint RP_define_save(char *name)            | Saves (pushes) a macro under the name "name##<<depth+1>>".
| ~cint RP_define_restore(char *name)         | Deletes the macro "name" and restores (pops) the macro "name##<<depth>>" under the name "name".
| ~cint RP_define_save_list(char **list)      | Saves (pushes) a list of macros using RP_define_save() for each macro.
| ~cint RP_define_restore_list(char **list)   | Restores a list of macros using RP_define_restore() for each macro.
..te
>

<b_rep_foreach.c>
b_rep_foreach.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to compile and execute ~c$foreach~C and ~c$next~C commands.

The mechanism implemented is detailed in the source file.

&IT Example
&CO
$define A XYZ
A is %A%
$foreach A 1 2 3
    A is %A%
$next A
A is %A%
&TX

prints:

&CO
A is XYZ
    A is 1
    A is 2
    A is 3
A is XYZ
&TX

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                          | Description
| ~cint RP_foreach(char* arg)                     | Implemention of $foreach <<index_name>> <<list_of_values>>
| ~cint RP_foreach_break(char *name)              | Exits the foreach block. Not implemented
| ~cint RP_foreach_next(char* arg)                | Implementation of $next <<index_name>>
..te
>
<b_rep_proc.c>
b_rep_proc.c
¯¯¯¯¯¯¯¯¯¯¯¯
Compilation and execution of report procedures (~c$prodef, $procexec~C).

&IT Syntax of report procedures ($proc*)
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
    $proc[def] procname [parm1] ... [parms]
     ...
    $procend
     ...
    $procexec procname parms ..
&TX

&IT Example
¯¯¯¯¯¯¯¯¯¯¯¯
~iNote that when there is a whitespace between $ or ## and the command, it is considered as a comment.~I

&CO
    $ Definition of the procedure TotalBySex
    $ --------------------------------------
        $procdef TotalBySex resprefix agelist arrlist
            $foreach sex F;H
                @chronoreset()
                $ show %sex% - %agelist% - %arrlist%
                $define POP 0
                $foreach pos P01;P02;P03;P04;P05;P06;P07;P08;P09;P10;P11;P12;P13;P14;P15
                    $foreach age @lvalue(%agelist%)
                        $foreach arr @lvalue(%arrlist%)
                            $define POP %POP% + %pos%_%age%_%sex%_%arr%
                        $next arr
                    $next age
                $next pos
                $DataUpdateidt %resprefix%_%sex% %POP%
                $IdtExecute 1991Y1 2010Y1 %resprefix%_%sex%
                $ show CPU : @chronoget() for %sex% - %agelist% - %arrlist%
            $next sex
        $procend

    $ Call to the procedure TotalBySex
    $ --------------------------------
        $foreach agegroup 7074 7579 8084 8589 90plus
            $procexec TotalBySex POP%agegroup%_WAL L%agegroup% WAL
            $procexec TotalBySex POP%agegroup%_VLA L%agegroup% VLA
            $ procexec TotalBySex POP%agegroup%_ARR L%agegroup% ARR
            $procexec TotalBySex POP%agegroup%_BRU L%agegroup% BRU
        $next agegroup
&TX

&IT Parameter usage
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&EN all parameters but the last one are words or strings enclosed in ""
&EN the parameters in excess (if any) are assigned to the last formal parameter
&EN parameters are used in the body of the procedure like $defines : %parm1%, ...
&EN if a $define with the same name as a procedure parameter exists before executing the procedure,
it is locally replaced by the actual value of the parameter, but retrieves its value at the end
of the procedure execution
&EN the $procdef statement is closed by the first $procend (in the same file)
&EN a procedure can be redefined
&EN procedures are kept in memory after the end of the report execution
&EN procedures are called by the command $procexec
&EN a procedure must be defined before its first use

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                       | Description
| ~cstatic int RP_proc_find(char *name)        | Retrieves the position of a REP_PROC in REP_PROCS by its name (case sensitive!)
| ~cstatic int RP_proc_is_procend(char *line)  | Checks if a line closes a PROC (i.e.: contains "$procend").
| ~cstatic void RP_proc_delete(int proc_nb)    | Deletes a REP_PROC object and frees its reference in the table REP_PROCS.
| ~cstatic int RP_proc_create(char *name)      | Adds a new empty PROC in REP_PROCS.
| ~cvoid RP_proc_free_all()                    | Frees all the defined procedures and the table REP_PROCS.
| ~cint RP_procdef(char* arg)                  | Reads and creates a new PROC.
| ~cint RP_procexec(char* arg)                 | Executes a procedure (called by $procexec parms).
..te

&IT List of global variables
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                      | Description
| ~cREP_PROC    **REP_PROCS   | Table of pointers to existing PROCs
| ~cint         REP_NB_PROCS  | Size of REP_PROCS
..te
>

<b_rep_syntax.c>
b_rep_syntax.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
This file contains two tables defining the commands and functions available in IODE reports.

The two global tables are:
&EN ~cB_fns[] ~C that lists all report $commands and ##commands names and pointers.
&EN ~cRP_FNS[]~C that lists all report @functions names and pointers.

To add a new $command to IODE reports, add a new line in B_fns[] with the name in lowercase
and the function pointer(s).

To add a new @function to IODE reports, add a new line in RP_FNS[] with the name in lowercase
and the function pointer.

&IT List of global variables
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax            | Description
| ~cBFNS B_fns[]    | Names of the report commands + GUI and non-GUI function pointers + type defining the allowed suffixes.
| ~cRPFN RP_FNS[]   | Names of the report @functions and their function pointers
..te
>

<b_rep_utils.c>
b_rep_utils.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
This file contains utilities used inside IODE report functions.

The main part of this file contains ~iallocation functions~I that are aimed to
optimize allocations during report executions. These functions superseed SCR_malloc(), SCR_free()...

&NO WARNING: in the most recent versions of IODE, the define ~cRP_STDMALLOC~C is set to 1.
Therefore, the standard functions (SCR_malloc()...) are used inside RP_alloc(), RP_free()...

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                               | Description
| ~cchar *RP_alloc(int size)                                           | Mimics SCR_malloc() using RP_ALLOCATIONS if RP_STDALLOC is null.
| ~cint RP_free(char *ptr)                                             | Mimics SCR_free() using RP_ALLOCATIONS if RP_STDALLOC is null.
| ~cvoid RP_free_bufs()                                                | Frees all memory buffers allocated by the RP_ALLOCATIONS library.
| ~cchar *RP_stracpy(char *ptr)                                        | Mimics SCR_stracpy() using RP_ALLOCATIONS if RP_STDALLOC is null.
| ~cunsigned char **RP_vtoms(unsigned char* str, unsigned char *seps)  | Mimics SCR_vtoms() using RP_ALLOCATIONS if RP_STDALLOC is null.
| ~cunsigned char **RP_vtom(unsigned char* str, int sep)               | Mimics SCR_vtom() using RP_ALLOCATIONS if RP_STDALLOC is null.
| ~cint RP_free_tbl(unsigned char **tbl)                               | Mimics SCR_free_tbl() using RP_ALLOCATIONS if RP_STDALLOC is null.
| ~cint RP_tbl_size(unsigned char **tbl)                               | Mimics SCR_tbl_size().
| ~cint RP_is_cmd(char *line)                                          | Checks that a report line is a command ($command or ##command).
| ~cU_ch **SCR_vtomsq(char* str, char* seps, int quote)                | Splits a string on one of separators. Text enclosed in quote char are not split.
..te
>


<IODE Report Commands>
IODE Report commands
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
This group contains functions acting on IODE objects and/or files that are called by the report
engine (see b_rep_syntax.c for a complete list of commands).

These functions have a similar syntax and always
return an integer code, 0 indicating success, other values indicating an error.

They can be divided in 3 groups:
&EN functions requiring as 2d argument an ~bobject type~B like B_DataCreate() for the report commands $DataCreateIdt, $DataCreateVar...
&EN functions requiring as 2d argument a ~bfile type~B like B_FileDelete() for the report commands $FileDeleteCsv, $FileDeleteTxt...
&EN functions with ~bonly one argument~B, like B_DataListCount() or B_DataCalcVar()

&IT Functions with an IODE object suffix (cmt, eqs...)
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Some functions need a suffix in the report syntax. For example $DataDelete and $DataUpdate
require a suffix indicating which type of objects is their target (one of the 7 IODE objects).
In that way, only one function is needed for $DataDelete~iVar~I, $DataDelete~iIdt~I...

When called by the report engine, these functions have 2 parameters:
&EN the argument of the function (the report line without the function name)
&EN the type of object treated (K_CMT <<= type <<= K_VAR)

For these functions, the parameters and return values are as follows:
&CO
    @param [in] char*   arg     report line without the first word
    @param [in] int     type    type of object to be manipulated (K_CMT <<= type <<= K_VAR)
    @return     int             0 on success, -1 on error
&TX
For instance, the report command
&CO
    $DataDeleteVar A B C
&TX
generates the C call:
&CO
    B_DataDelete("A B C", K_VAR);

    where arg == "A B C" and type == K_VAR
&TX

&IT Functions with a file extension suffix (csv, txt...)
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
The principle is the same as above but for filename extension suffix (.csv, .txt...)
instead of object type suffix (.cmt...).

When called by the report engine, these functions have also 2 parameters:
&EN the argument of the function (report line without function name)
&EN the type of ~bfile~B treated (K_CMT <<= type <<= K_CSV)

The parameters and return values are the same as for the functions with an IODE object type parameter.

For instance, the report command
&CO
    $FileDeleteCsv myfile
&TX
calls the C function
&CO
    B_FileDelete(arg, K_CSV);

    where arg == "myfile" and type == K_VAR
&TX

&IT Other functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
All other functions receive only the argument on the report line.
B_DataListSort(), for example, has only one argument.

For these functions, the parameters and return values are as follows:

&CO
 @param [in] char*   arg     report line without the first word (i.e. the function name)
 @return     int             0 on success, -1 on error (not enough args for example)
&TX

&TI List of source files
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&EN <b_fsys.c> : file manipulation + conversion from/to ansi-oem-utf8
&EN <b_file.c> : file manipulation
&EN <b_xode.c> : import comments and series (aka variables) from various non-IODE formats

&EN <b_htol.c> : transformation of high periodicity to low periodicity series.
&EN <b_ltoh.c> : transformation of low periodicity series to high periodicity series.
&EN <b_trend.c>  : Implementation of the **Hodrick-Prescott** method for trend series construction. 
&EN <b_ras.c>  : implementation of a RAS algorithm.

&EN <b_data.c> : functions acting on data (i.e.: IODE objects)
&EN <b_est.c>  : estimation functions
&EN <b_step.c>  : find the best possible tests for all possible combinations of coefficients in a block of equations

&EN <b_model.c> : model simulation, decomposition and recompilation
&EN <b_ws.c>    : functions related to WS management (clear, load, save, sample...)
&EN <b_rep_super.c>: IODE report super function default implementations for non-GUI programs

&EN <b_pdest.c> : functions to set up printing parameters and to generate outputs in various formats.
&EN <b_print.c> : functions to print object definitions.
&EN <b_view.c> : functions to display or print tables and variables as text or as graphs.


>




<b_data.c>
b_data.c
¯¯¯¯¯¯¯¯
Functions acting on IODE objects called by the report engine (see b_rep_syntax.c)
and their sub-functions.

These functions all have a similar syntax and always
return an integer as return code, 0 indicating success, other values an error.

There are 2 groups of functions, one where a suffix is required, one with no suffix.

&IT Functions with a suffix
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Some functions need a suffix in the report syntax. For example $DataDelete and $DataUpdate
required a suffix indicating which type of objects is the target (one of the 7 IODE objects).
In that way, only one function is needed for $DataDeleteVar or $DataDeleteIdt...

When called by the report engine, these functions have 2 parameters:
&EN the argument of the function (the remaining of the report line)
&EN the type of object treated

For these functions, the parameters and return values are as follows:
&CO
@param [in] char*   arg     report line  without the command
@param [in] int     type    type of object whose names are to be saved in the list
@return     int             0 on success, -1 on error (not enough args)
&TX

For instance, the report command
&CO
    $DataDeleteVar A B C
&TX
generates the C call:
&CO
    B_DataDelete("A B C", K_VAR);
    where arg == "A B C" and type == K_VAR
&TX

&IT Other functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
All other functions receive simply the argument on the report line.
B_DataListSort(), for example, has only one argument.

For these functions, the parameters and return values are as follows:
&CO
@param [in] char*   arg     report line without the command
@return     int             0 on success, -1 on error (not enough args)
&TX

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                      | Description
| ~cint B_DataPattern(char* arg,int type)     |  Creates an IODE list with all existing objects of a given type having the name constructed by the combinations of 2 lists.
| ~cint B_DataRasVar(char* arg)               |  RAS method implementation.
| ~cint B_DataCalcVar(char* arg)              |  Computes a new variable based on a LEC expression.
| ~cint B_DataCreate(char* arg, int type)     |  Creates one or more new objects.
| ~cint B_DataDelete(char* arg, int type)     |  Deletes one or more objects.
| ~cint B_DataRename(char* arg, int type)     |  Renames an object. Equations cannot be renamed.
| ~cint B_DataDuplicate(char* arg, int type)  |  Duplicates an object. Equations cannot be duplicated.
| ~cint B_DataUpdate(char* arg, int type)     |  Updates an object. The syntax can differ according to the object type.
| ~cint B_DataSearch(char* arg, int type)     |  Searches all objects containing a given string in their names and/or definitions.
| ~cint B_DataListSort(char* arg)             |  Sorts a list alphanumerically.
| ~cint B_DataScan(char* arg, int type)       |  Analyses a KDB content and creates 2 lists _EXO and _SCAL with all VAR and all SCL found in the kdb objects.
| ~cint B_DataExist(char* arg, int type)      |  Checks that an object exists. Returns -1 if not, the object position in WS otherwise.
| ~cint B_DataAppend(char* arg, int type)     |  Appends data (a string) to a CMT or a LST.
| ~cint B_DataList(char* arg, int type)       |  Constructs a list of objects corresponding to a given name pattern. Objects can be in WS or in a file.
| ~cint B_DataCalcLst(char* arg)              |  List calculus: 4 operations between 2 lists.
| ~cint B_DataListCount(char* arg)            |  Returns the number of elements in a list.
| ~cint B_DataCompareEps(char* arg)           |  Defines the threshold under which the difference between 2 variables are considered equal.
| ~cint B_DataCompare(char* arg, int type)    |  Compares the objects in the current WS to the content of an IODE file and stores the results in 4 lists.
| ~cint B_DataDisplayGraph(char* arg)         |  Shows VARs or combinations of VARS in graphical form.
| ~cint B_DataPrintGraph(char* arg)           |  Prints VARs or combinations of VARS in graphical form.
..te
>

<b_ras.c>
b_ras.c
¯¯¯¯¯¯¯¯
Implementation of a RAS algorithm.

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                                                                     | Description
| ~cint RasExecute(char *pattern, char *xdim, char *ydim, PERIOD *rper, PERIOD *cper, int maxit, double eps) | Implementation of a RAS algorithm
..te
>

<b_est.c>
b_est.c
¯¯¯¯¯¯¯¯
Estimation functions called in IODE reports.

Except for B_EqsEstimateEqs(), all functions in this group share the same syntax:
&CO
    int fn_name(char* arg)

    where:
        arg is the report line (without the command name)
        the return code is 0 on success, any other value indicating and error
&TX

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                           | Description
| ~cint B_EqsEstimateEqs(SAMPLE* smpl, char** eqs) |  Estimates a bloc of equations on a defined SAMPLE.
| ~cint B_EqsEstimate(char* arg)                   |  Implementation of the report function $EqsEstimate.
| ~cint B_EqsSetSample(char* arg)                  |  Implementation of the report function $EqsSetSample.
| ~cint B_EqsSetMethod(char* arg)                  |  Implementation of the report function $EqsSetMethod.
| ~cint B_EqsSetBloc(char* arg)                    |  Implementation of the report function $EqsSetBlock
| ~cint B_EqsSetCmt(char* arg)                     |  Implementation of the report function $EqsSetCmt.
| ~cint B_EqsSetInstrs(char* arg)                  |  Implementation of the report function $EqsSetInstrs.
..te
>

<b_step.c>
b_step.c
¯¯¯¯¯¯¯¯¯¯

Report function that estimates a block of equations and finds the best possible tests 
for all possible combinations of coefficients.

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                         | Description
| ~cint B_EqsStepWise(char* arg) | $EqsStepWise from to eqname leccond {r2|fstat}
..te
>

<b_trend.c>
b_trend.c
¯¯¯¯¯¯¯¯¯¯

Implementation of the **Hodrick-Prescott** method for trend series construction. 

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                         | Description
| ~cint B_WsTrend(char* arg)     | $WsTrend VarFilename Lambda series1 series2 ...
| ~cint B_WsTrendStd(char* arg)  | $WsTrendStd VarFilename Lambda series1 series2 ...
..te
>



<b_fsys.c>
b_fsys.c
¯¯¯¯¯¯¯¯¯¯
File manipulation and conversion from/to ansi-oem-utf8.

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                            | Equivalent in Reports
| ~cint B_SysRename(char* arg)      | $SysMoveFile filein fileout
| ~cint B_SysCopy(char* arg)        | $SysCopyFile filein fileout
| ~cint B_SysAppend(char* arg)      | $SysAppendFile filein fileout
| ~cint B_SysDelete(char* arg)      | $SysDeleteFile file1 file2 ...
| ~cint B_SysOemToUTF8(char *arg)   | $SysOemToUTF8 inputfile outputfile
| ~cint B_SysAnsiToUTF8(char *arg)  | $SysAnsiToUTF8 inputfile outputfile
| ~cint B_SysAnsiToOem(char *arg)   | $SysAnsiToOem inputfile outputfile
| ~cint B_SysOemToAnsi(char *arg)   | $SysOemToAnsi inputfile outputfile
..te
>

<b_file.c>
b_file.c
¯¯¯¯¯¯¯¯¯
Functions acting on files called by the report engine (see b_rep_syntax.c).

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                  | Equivalent in Reports
| ~cint B_FileCopy(char* arg, int type)   | $FileCopy<<type>> source_file dest_file
| ~cint B_FileRename(char* arg, int type) | $FileRename<<type>> source_file dest_file
| ~cint B_FileDelete(char* arg, int type) | $FileDelete<<type>> file1 [file2...]
..te
>

<b_xode.c>
b_xode.c
¯¯¯¯¯¯¯¯¯
Report functions to import comments and variables from various non-IODE formats.

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                            | Equivalent in Reports
| ~cint B_FileImportCmt(char* arg)  | $FileImportCmt format rule infile outfile language [trace]
| ~cint B_FileImportVar(char* arg)  | $FileImportVar format rule infile outfile from to  [trace]$FileImportVar format rule infile outfile from to  [trace]
..te
>

<b_model.c>
b_model.c
¯¯¯¯¯¯¯¯¯
Report functions related to model simulations.

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                    | Equivalent in Reports
| ~cint B_ModelSimulate(char *arg)          | $ModelSimulate per_from per_to equation_list
| ~cint B_ModelSimulateParms(char* arg)     | $ModelSimulateParms eps relax maxit {Connex, Triang, None } 0 - 4 (starting values) {Yes, no } nbtri {yes, No } 
| ~cint B_ModelExchange(char* arg)          | $ModelExchange eqname1-varname1,eqname2-varname2,...
| ~cint B_ModelCompile(char* arg)           | $ModelCompile  [eqname1, eqname2, ... ]
| ~cint B_ModelCalcSCC(char *arg)           | $ModelCalcSCC nbtris prename intername postname [eqs]
| ~cint B_ModelSimulateSCC(char *arg)       | $ModelSimulateSCC from to pre inter post
| ~cint B_ModelSimulateSaveNIters(char *arg)| $ModelSimulateSaveNiters varname
| ~cint B_ModelSimulateSaveNorms(char *arg) | $ModelSimulateSaveNorms varname
..te
>


<b_htol.c>
b_htol.c
¯¯¯¯¯¯¯¯¯
Report functions to transform high periodicity to low periodicity series.

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                           | Equivalent in Reports
| ~cint B_WsHtoLLast(char* arg)    | $WsHtoLLast Filename VarList
| ~cint B_WsHtoLMean(char* arg)    | $WsHtoLMean Filename VarList
| ~cint B_WsHtoLSum(char* arg)     | $WsHtoLSum Filename VarList
..te
>

<b_ltoh.c>
b_ltoh.c
¯¯¯¯¯¯¯¯¯
Report functions to transform low periodicity series to high periodicity series (i.e. variables).

Two types of series are considered: stock and flow:
&EN in the first case, the interpolated values are of the same order of magnitude as the original values
&EN in the latter case, the values of the sub-periods are additive over a period.

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                           | Equivalent in Reports
| ~cint B_WsLtoHStock(char* arg)   | $WsLtoHStock {L|C|S} Filename VarList
| ~cint B_WsLtoHFlow(char* arg)    | $WsLtoHFlow  {L|C|S} Filename VarList
..te
>


<b_ws.c>
b_ws.c
¯¯¯¯¯¯¯¯
Functions related to WS management (clear, load, save, sample, import...).

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                             | Equivalent in Reports
| ~cint B_WsLoad(char* arg, int type)                | $WsLoad<<type>> filename
| ~cint X_findtype(char* filename)                   | Returns the type of content of filename according to its extension
| ~cint B_WsDump(KDB* kdb, char* filename)           | Dumps the content of KDB in a file
| ~cint B_WsSave(char* arg, int type)                | $WsSave<<type>> filename
| ~cint B_WsSaveCmp(char* arg, int type)             | $WsSaveCmp<<type>> filename
| ~cint B_WsExport(char* arg, int type)              | $WsExport<<type>> filename
| ~cint B_WsImport(char* arg, int type)              | $WsImport<<type>> filename
| ~cint B_WsSample(char* arg)                        | $WsSample period_from period_to
| ~cint B_WsClear(char* arg, int type)               | $WsClear<<type>>
| ~cint B_WsClearAll(char* arg)                      | $WsClearAll
| ~cint B_WsDescr(char* arg, int type)               | $WsDescr<<type>> free text
| ~cint B_WsName(char* arg, int type)                | Sets the WS name. Obsolete as report function.
| ~cint B_WsCopy(char* arg, int type)                | $WsCopy<<type>> fichier;fichier;.. obj1 obj2... or $WsCopyVar file;file;.. [from to] obj1 obj2...
| ~cint B_WsMerge(char* arg, int type)               | $WsMerge<<type>> filename
| ~cint B_WsExtrapolate(char* arg)                   | $WsExtrapolate [method] from to [variable list]
| ~cint B_WsAggrChar(char* arg)                      | $WsAggrChar char
| ~cint B_WsAggrSum(char* arg)                       | $WsAggrSum pattern filename
| ~cint B_WsAggrProd(char* arg)                      | $WsAggrProd pattern filename
| ~cint B_WsAggrMean(char* arg)                      | $WsAggrMean pattern filename
| ~cIODE_REAL *B_StatUnitRoot_1(char* arg, int print)| Sub function of B_StatUnitRoot() with an optional parameter to print the result (or not).
| ~cint B_StatUnitRoot(char* arg)                    | $StatUnitRoot drift trend order expression
| ~cint B_CsvSave(char* arg, int type)               | $CsvSave<<type>> file name1 name2 ...
| ~cint B_CsvNbDec(char *nbdec)                      | $CsvNbDec nn
| ~cint B_CsvSep(char *sep)                          | $CsvSep char
| ~cint B_CsvNaN(char *nan)                          | $CsvNaN text
| ~cint B_CsvAxes(char *var)                         | $CsvAxes AxisName
| ~cint B_CsvDec(char *dec)                          | $CsvDec char
..te
>

<b_rep_super.c>
b_rep_super.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Default implementation of GUI report functions (called in non-GUI programs).

In a report, the commands $WsLoadVar and ##WsLoadVar call different functions. $-command
calls a non-GUI function, whereas the ##-command calls a GUI function.

In the table B_fns (defined in b_rep_syntax.c), two different pointers are defined,
one for the non GUI version ($) and one for the GUI version (##).

&CO
    (*B_fns[i].fn)              Executes the no-GUI function
    (*B_fns[i].sfn)             Executes the GUI function
&TX

For example, the command "##WsLoad" calls the GUI function SB_WsLoad() which opens
the Dialog "Workspace / Load" and waits for a user action. In contrast, the command "$WsLoadVar"
calls B_WsLoad(filename, K_VAR) which loads a WS, but without user interaction.

In non-GUI programs, a call to a GUI-function must be replaced by a default implementation, generally
a empty one.

The logic is the same as the one described in k_super.c: for each GUI-function
a default (empty) function is implemented and its pointer is set to the variable ~c<<fn>>_super~C.
That variable can be replaced, if needed, by assigning a new value with a GUI function.

&CO
int (*SB_WsLoad_super        )();

int SB_WsLoad ()
{
    if(SB_WsLoad_super)
        return(*SB_WsLoad_super         )();
    else
        return(0);
}
&TX

In the GUI version, the function pointer SB_WsLoad_super should be replaced by a GUI implementation
of the function.

>

<Iode Report Functions>
Iode Report @-functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
When the report engine finds a @function() in a report line, the @-function is calculated and its
result replaces "in-place" the @function and its parameters.

The available @-functions are defined in the table ~cRP_FNS[]~C (in b_rep_syntax.c), which contains
the names and associated C function pointers. The functions themselves are called by ~RP_fneval()~C.

&IT @-function parameters specifications
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&EN parameters are separated by commas.
&EN spaces are regular characters (and, if present, are included in the parameters)
&EN parameters enclosed by double quotes "" may contain spaces or commas

&IT Examples
¯¯¯¯¯¯¯¯¯
&CO
      "@take(2,ABCDE)"        =>> "AB"
      "@take(2, ABCDE)"       =>> " A"
      "@take(3,"A,BCDE")"     =>> "A,B"
&TX

In case of error in the @function syntax, for example an illegal number of parameters,
or if the @function does not exist, the text of the function remained unchanged
in the report line:
&CO
    Correct:
      "@take(2,ABCDE)"        =>> "AB"
      "@take(2,"A,BCDE")"     =>> "A,"
    Incorrect:
      "@take(2,A,BCDE)"       =>> "@take(2,A,BCDE)"
      "@takes(2,A)"           =>> "@takes(2,A)"
&TX
Each function in this group must have the same syntax:
&CO
    U_ch* function_name(U_ch** arg)

    @param [in] U_ch**   arg    List of arguments of the function (sep = ',')
    @return     U_ch*           allocated string containing the computed value of the function
                                NULL on error
&TX

To add a new @function:
&EN create the function based on an existing one. The syntax must be identical
&EN add the function name / pointer to the table RP_FNS[] (see at the end of the current file).

More details can be found here : https://iode.plan.be/doku.php?id=les_fonctions_de_rapports

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax
| ~cU_ch *RPF_take(U_ch** args)
| ~cU_ch *RPF_drop(U_ch** args)
| ~cU_ch *RPF_replace(U_ch** args)
| ~cU_ch *RPF_equal(U_ch** args)
| ~cU_ch *RPF_upper(U_ch** args)
| ~cU_ch *RPF_lower(U_ch** args)
| ~cU_ch *RPF_sqz(U_ch** args)
| ~cU_ch *RPF_strip(U_ch** args)
| ~cU_ch *RPF_fmtint(U_ch** args)
| ~cU_ch *RPF_ansi(U_ch** args)
| ~cU_ch *RPF_count(U_ch** args)
| ~cU_ch *RPF_index(U_ch** args)
| ~cU_ch *RPF_void(U_ch **args)
| ~cU_ch *RPF_date(U_ch** args)
| ~cU_ch *RPF_time(U_ch** args)
| ~cU_ch *RPF_month(U_ch** args)
| ~cU_ch *RPF_sstderr(U_ch** args)
| ~cU_ch *RPF_srelax(U_ch** args)
| ~cU_ch *RPF_ttitle(U_ch** args)
| ~cU_ch *RPF_cvalue(U_ch** args)
| ~cU_ch *RPF_vvalue(U_ch** args)
| ~cU_ch *RPF_lvalue(U_ch** args)
| ~cU_ch *RPF_ivalue(U_ch** args)
| ~cU_ch *RPF_evalue(U_ch** args)
| ~cU_ch *RPF_eqsample(U_ch** args)
| ~cU_ch *RPF_eqsamplefromto(U_ch** args, int fromto)
| ~cU_ch *RPF_eqsamplefrom(U_ch **args)
| ~cU_ch *RPF_eqsampleto(U_ch **args)
| ~cU_ch *RPF_eqlhsrhs(U_ch** args, int lhsrhs)
| ~cU_ch *RPF_eqlhs(U_ch **args)
| ~cU_ch *RPF_eqrhs(U_ch **args)
| ~cU_ch *RPF_sample(U_ch** args)
| ~cint RPF_vsliste1(CLEC* cl, U_ch*** tbl, int* nb, int type)
| ~cU_ch *RPF_vsliste(U_ch** args, int type)
| ~cU_ch **RPF_unique(U_ch** tbl)
| ~cU_ch *RPF_vliste(U_ch** args)
| ~cU_ch *RPF_sliste(U_ch** args)
| ~cU_ch *RPF_expand(U_ch** args, int type)
| ~cU_ch *RPF_cexpand(U_ch **args)
| ~cU_ch *RPF_eexpand(U_ch **args)
| ~cU_ch *RPF_iexpand(U_ch **args)
| ~cU_ch *RPF_lexpand(U_ch **args)
| ~cU_ch *RPF_sexpand(U_ch **args)
| ~cU_ch *RPF_texpand(U_ch **args)
| ~cU_ch *RPF_vexpand(U_ch **args)
| ~cint RPF_CalcPeriod(U_ch** args)
| ~cU_ch *RPF_SimNorm(U_ch** args)
| ~cU_ch *RPF_SimNIter(U_ch** args)
| ~cU_ch *RPF_SimMaxit()
| ~cU_ch *RPF_SimEps()
| ~cU_ch *RPF_SimRelax()
| ~cU_ch *RPF_vtake(U_ch** args)
| ~cU_ch *RPF_vdrop(U_ch** args)
| ~cU_ch *RPF_vcount(U_ch** args)
| ~cU_ch *RPF_memory(U_ch** args)
| ~cU_ch *RPF_ChronoReset()
| ~cU_ch *RPF_ChronoGet()
| ~cU_ch *RPF_fappend(U_ch** args)
| ~cU_ch *RPF_fdelete(U_ch** args)
| ~cU_ch *RPF_getdir()
| ~cU_ch *RPF_chdir(U_ch **args)
| ~cU_ch *RPF_mkdir(U_ch **args)
| ~cU_ch *RPF_rmdir(U_ch **args)
..te
>

<b_pdest.c>
b_pdest.c
¯¯¯¯¯¯¯¯¯
Functions (and their subfunctions) called by the report engine to set up printing parameters and
to generate outputs in various formats.


&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                               | Description or equivalent in Reports
| ~cint B_PrintDestExt(char* file, int newf, int type) | Define the printing destination.
| ~cint B_PrintDestFile(char *arg, int newf)           | Define the output file for the following printouts.
| ~cint B_PrintDest(char *file)                        | $PrintDest [nom_fichier] [format]
| ~cint B_PrintDestNew(char* file)                     | $PrintDestNew [nom_fichier] [format]
| ~cint B_PrintNbDec(char* nbdec)                      | $PrintNbDec nb
| ~cint B_PrintLang(char* lang)                        | $PrintLang {English | French | Dutch}
| ~cint B_PrintMulti(char* multi)                      | $PrintMulti STACKMODE
| ~cint B_PrintA2mAppend(char* arg)                    | $PrintA2mAppend [NO|Yes]
| ~cint B_PrintTBreak(char* arg)                       | $PrintTableBreak [NO|Yes]
| ~cint B_PrintTPage(char* arg)                        | $PrintTablePage  [NO|Yes]
| ~cint B_PrintGPage(char* arg)                        | $PrintGraphPage [NO|Yes]
| ~cint B_PrintParaNum(char* arg)                      | $PrintParanum [NO|Yes]
| ~cint B_PrintPageHeader(char* arg)                   | $PrintPageHeader following_pages_title
| ~cint B_PrintPageFooter(char* arg)                   | $PrintPageFooter following_pages_footer
| ~cint B_PrintFont(char* arg)                         | $PrintFont Times|Helvetica|Courier|Bookman|Palatino [size [incr]]
| ~cint B_PrintTFont(char* arg)                        | $PrintTableFont Times|Helvetica|Courier|Bookman|Palatino [size]
| ~cint B_PrintTBox(char* arg)                         | $PrintTableBox n
| ~cint B_PrintTColor(char* arg)                       | $PrintTableColor [NO|Yes]
| ~cint B_PrintTWidth(char* arg)                       | $PrintTableWidth width [col1 [coln]]
| ~cint B_PrintGSize(char* arg)                        | $PrintGraphSize  width [height] [fontsize]
| ~cint B_PrintGTheme(char* arg)                       | $PrintGraphTheme theme
| ~cint B_PrintGBand(char* arg)                        | $PrintGraphBand [per_from per_to]
| ~cint B_PrintGBox(char* arg)                         | $PrintGraphBox n
| ~cint B_PrintGBrush(char* arg)                       | $PrintGraphBrush pct|Yes
| ~cint B_GetColor(char* arg)                          | Sub function of B_PrintColor() to interpret color names.
| ~cint B_PrintGColor(char* arg)                       | $PrintBackground Black|Blue|Magenta|Cyan|Red|Green|Yellow|White
| ~cint B_PrintRtfHelp(char* arg)                      | $PrintRtfHelp [YES|No]
| ~cint B_PrintHtmlHelp(char* arg)                     | $PrintHtmlHelp [YES|No]
| ~cint B_PrintRtfTitle(char* arg)                     | $PrintRtfTitle Help title
| ~cint B_PrintRtfCopy(char* arg)                      | $PrintRtfCopyright copyright text
| ~cint B_PrintRtfLevel(char* arg)                     | $PrintRtfLevel [+|-|n]
| ~cint B_PrintRtfTopic(char* arg)                     | $PrintRtfTopic topic title
| ~cint B_PrintGdiOrient(char* arg)                    | $PrintOrientation {Portrait|Landscape}
| ~cint B_PrintGdiDuplex(char* arg)                    | $PrintDuplex {Simplex|Duplex|VerticalDuplex}
| ~cint B_PrintGdiPrinter(char* arg)                   | $SetPrinter printer_name
| ~cint B_PrintGIFBackColor(char* arg)                 | $PrintGIFBackColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}
| ~cint B_PrintGIFTransColor(char* arg)                | $PrintGIFTransColor {Black|Blue|Magenta|Cyan|Red|Green|Yellow|White}
| ~cint B_PrintGIFInterlaced(char* arg)                | $PrintGIFInterlaced {Yes|No} 
| ~cint B_PrintGIFTransparent(char* arg)               | $PrintGIFTransparent {Yes|No}
| ~cint B_PrintGIFFilled(char* arg)                    | $PrintGIFilled {Yes|No}
| ~cint B_PrintGIFFont(char* arg)                      | $PrintGIFFont FontNb (between 0 and 5)
| ~cint B_PrintHtmlStrip(char* arg)                    | $PrintHtmlStrip [YES|No]
| ~cint B_PrintHtmlStyle(char* arg)                    | $PrintHtmlStyle filename
| ~cint B_A2mToAll(char* arg, int type)                | Convert an A2M file to another format.
| ~cint B_A2mToPrinter(char* arg)                      | $A2mToPrinter file.a2m
| ~cint B_A2mToHtml(char* arg)                         | $A2mToHtml filein fileout
| ~cint B_A2mToRtf(char* arg)                          | $A2mToRtf filein fileout
| ~cint B_A2mToMif(char* arg)                          | $A2mToMif filein fileout
| ~cint B_A2mToCsv(char* arg)                          | $A2mToCsv filein fileout
| ~cint B_A2mSetCol(int *dest, int col)                | Extracts a color definition from B_GIFCOLS and saves it in dest[3].
| ~cint B_PrintHtmlTableClass(char *table_class)       | $PrintHtmlTableClass class_name
| ~cint B_PrintHtmlTRClass(char *tr_class)             | $PrintHtmlTRClass class_name
| ~cint B_PrintHtmlTHClass(char *th_class)             | $PrintHtmlTHClass class_name
| ~cint B_PrintHtmlTDClass(char *td_class)             | $PrintHtmlTDClass class_name
..te
>

<b_print.c> 
b_print.c
¯¯¯¯¯¯¯¯¯¯¯¯
Functions (and their subfunctions) to print IODE object definitions.

&IT List of functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                               | Description or equivalent in Reports
| ~cint B_PrintVal(IODE_REAL val)                                    | Print a double with the function T_print_val() and with the number of decimals set to -1
| ~cIODE_REAL B_calc_ttest(SCL* scl)                                 | Return the t-test of a scalar or L_NAN if it cannot be determined.
| ~cint B_replesc(unsigned char* out, unsigned char* in)             | Replace \ by / in a string
| ~cint B_PrintDefGnl(char* name, char* text)                        | Print an object name and its title in an enum_1 paragraph.
| ~cint B_isdef(char* txt)                                           | Checks if a string contains non space charaters.
| ~cint B_dump_str(unsigned char*head, unsigned char*txt)            | Print a header and a modified text: spaces are added before and after specific characters in the text
| ~cint B_get1int(char* arg)                                         | Return the integer value of the beginning of a string.
| ~cint B_ScrollSet(char* arg, long *plong, int inf, int sup)        | Interprets the first part of a string as a integer and check that the value is between 2 boundaries.
| ~cint B_PrintObjTblTitle(char* arg)                                | $PrintObjTitle 0 or 1
| ~cint B_PrintObjLec(char* arg)                                     | $PrintObjLec {0|1|2}
| ~cint B_PrintObjEqsInfos(char* arg)                                | $PrintObjInfos {0|1|2}
| ~cint B_PrintObjDef_1(char* arg, int* type)                        | Print the definition of the object named arg of the given type
| ~cint B_PrintObjDef(char* arg, int type)                           | $PrintObjDefXxx object_list
| ~cint B_PrintObjDefArgs(char* arg, int type)                       | Print a list of objects of a given type.
| ~cint B_PrintDefTbl(KDB* kdb, int pos)                             | Print the table in position pos in kdb.  
| ~cint B_DumpTblDef(TBL* tbl)                                       | Print a table definition.
| ~cint B_CellDef(TCELL* cell)                                       | Checks that a TCELL is not empty (for TEXT cells) and not "1" (for LEC cells).
| ~cint B_PrintTblCell(TCELL* cell, int straddle)                    | Print a TABLE cell optionally on several columns.
| ~cint B_PrintDefCmt(KDB* kdb, int pos)                             | Print a comment.
| ~cint B_PrintDefLst(KDB* kdb, int pos)                             | Print a list.
| ~cint B_PrintDefIdt(KDB* kdb, int pos)                             | Print a identity.
| ~cint B_PrintDefEqs(KDB* kdb, int pos)                             | Print a equation.
| ~cint B_PrintLec(char* name, char* eqlec, CLEC* eqclec, int coefs) | Print a LEC expression. Set the engogenous (name) in bold.
| ~cint B_PrintEqs(char* name, EQ* eq)                               | Print an equation and optionally its statistical tests.
| ~cint B_PrintDefSclPtr(SCL* scl, char*name, int enum_)             | Print a scalar in an enumeration list.
| ~cint B_PrintDefScl(KDB* kdb, int pos)                             | Print the scalar kdb[pos].
| ~cint B_PrintDefVar(KDB* kdb, int pos)                             | Print the variable kdb[pos] in a table. Sub-function of B_PrintObjDef_1().
..te

>


<b_view.c>
b_view.c
¯¯¯¯¯¯¯¯
Functions to display or print calculated tables and tables of variables. The same functions are used
to print and to display tables as graphs or as text.

The functions generate IODE tables in A2M format based on TBL structures and GSAMPLE definition.

&IT List of functions 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Syntax                                                | Description or equivalent in Reports
| ~cint B_ViewVar(char* arg)                            | Display a list of variables in the form of tables of max 50 variables.
| ~cint B_PrintVar(char* arg)                           | Print a list of variables in the form of tables of max 50 variables.
| ~cint B_ViewPrintVar(char* arg, int mode)             | Print or display (according to the mode parameter) variables in the form of tables. 
| ~cint B_ViewByTbl(char* arg)                          | $ViewTbl sample table [list of tables]
| ~cint B_ViewTbl(char* arg)                            | Alias of B_ViewByTbl()
| ~cint B_PrintTbl(char* arg)                           | $PrintTbl gsample table1 [table2...]
| ~cint B_ViewGr(char* arg)                             | $ViewGr gsample tbl1[+tbl2] tbl3 ... 
| ~cint B_PrintGr(char* arg)                            | $PrintGr gsample table1 [table2...]
| ~cint B_ViewPrintTbl_1(char* name, char* smpl)        | Calculate and display (or print according to the value of B_viewmode) a table on a specified GSAMPLE.
| ~cint B_ViewPrintGr_1(char* names, char* gsmpl)       | Calculate and display (or print according to the value of B_viewmode) a graph on a specified GSAMPLE, based on TBL definition(s).
| ~cint B_ViewPrintTbl(char* arg, int type, int mode)   | Calculate, then print or display (according to the mode parameter) IODE TBLs either in the form of graphs or in the form of text (SCROLLs).
| ~cint B_ViewTblFile(char* arg)                        | $PrintTblFile n varfilename    (n := 2, 3, 4, 5)
| ~cint B_ViewTblEnd()                                  | Close a Print tables or Print variables session.
..te
>
 
<Example of report with @-functions>
Example of report with @-functions
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯

&TI Report
¯¯¯¯¯¯¯¯¯
&CO
    $ `@-functions()` tests and examples
    $ --------------------------------
    $ Syntax: rep_fns input_dir output_dir
    $ Example: rep_fns data output

    $debug 1
    $indent
    $goto continue {%0% >>= 2}
        $show Syntax: rep_fns input_dir output_dir
        $show Syntax error. Report aborted.
        $return
    $label continue
    $define input_dir %1%
    $define output_dir %2%
    $label continue

    $PrintDest %output_dir%\rep_fns.a2m A2M

    STRINGS
    =======

    `@take()`
    `-------`
        Success
            `@take(2,ABCDE)`        =>> "@take(2,ABCDE)"
            `@take(-2,ABCDE)`       =>> "@take(-2,ABCDE)"
            `@take(2, ABCDE)`       =>> "@take(2, ABCDE)"
            `@take(0,ABCDE)`        =>> "@take(0,ABCDE)"
            `@take(2,"A,B,C,D,E")`  =>> "@take(2,"A,B,C,D,E")"

        Errors (##parms <<>> 2)
            `@take(2,A,B,C,D,E)`    =>> "@take(2,A,B,C,D,E)"
            `@take(2,A,BCDE)`       =>> "@take(2,A,BCDE)"

    `@drop()`
    `-------`
        Success
            `@drop(2,ABCDE)      `  =>> "@drop(2,ABCDE)"
            `@drop(-1,ABCDE)     `  =>> "@drop(-1,ABCDE)"
            `@drop(2, ABCDE)     `  =>> "@drop(2, ABCDE)"
            `@drop(0,ABCDE)      `  =>> "@drop(0,ABCDE)"
            `@drop(2,"A,B,C,D,E")`  =>> "@drop(2,"A,B,C,D,E")"
            `@DROP(2,A)          `  =>> "@DROP(2,A)"
            `@drop(3, "a bc")    `  =>> "@drop(3, "a bc")"

        Error (##parms <<>> 2)
            `@drop(2,A,B,C,D,E)`    =>> "@drop(2,A,B,C,D,E)"
            `@drop(2,A,BCDE)`       =>> "@drop(2,A,BCDE)"
            `@drop(n,ABCDE)`        =>> "@drop(n,ABCDE)"

    `@replace()`
    `----------`
        Success
            `@replace(Buro du Plan,Buro,Bureau)`            =>> @replace(Buro du Plan,Buro,Bureau)
            `@replace(Buro du Buro du Plan,Buro,Bureau)`    =>> @replace(Buro du Buro du Plan,Buro,Bureau)
            `@replace(Bureauplan,plan, du Plan)`            =>> @replace(Bureauplan,plan, du Plan)
            `@replace("Bureau, du Plan",",","") `           =>> @replace("Bureau, du Plan",",","")
            `@replace("Bureau, du Plan", ",", "") `         =>> @replace("Bureau, du Plan", ",", "")

        Error
            `@replace(Buro plan,Bureau du Plan)`            =>> @replace(Buro plan,Bureau du Plan)
            `@replace(AB,B,C,D)`                            =>> @replace(AB,B,C,D)

    `@equal()`
    `--------`
            `@equal(aa,aa)      `   =>>  "@equal(aa,aa)"
            `@equal(aa, aa)     `   =>>  "@equal(aa, aa)"
            `@equal(aa,bb)      `   =>>  "@equal(aa,bb)"
            `@equal(aa,aa,aa)   `   =>>  "@equal(aa,aa,aa)"

    `@upper()`
    `-------
            `@upper(ab cd,ef)    `  =>>  "@upper(ab cd,ef)"
            `@upper(AB)          `  =>>  "@upper(AB)"
            `@upper("abc", "def")`  =>>  "@upper("abc", "def")"
            `@upper("abc","def") `  =>>  "@upper("abc","def")"

    `@lower()`
    `-------
            `@lower(ab cd,ef)    `  =>>  "@lower(ab cd,ef)"
            `@lower(AB)          `  =>>  "@lower(AB)"
            `@lower("abc", "def")`  =>>  "@lower("abc", "def")"
            `@lower("abc","def") `  =>>  "@lower("abc","def")"

    `@sqz()`
    `------`
            `@sqz( ab    cd )`      =>>  "@sqz( ab    cd )"
            `@sqz(" ab    cd ")`    =>>  "@sqz(" ab    cd ")"
            `@sqz( ab,"  cd", ef)`  =>>  "@sqz( ab,"  cd", ef)"
            `@sqz( ab, " cd", ef)`  =>>  "@sqz( ab, " cd", ef)"

    `@strip()`
    `--------`
            `@strip( ab    cd )`      =>>  "@strip( ab    cd )"
            `@strip(" ab    cd ")`    =>>  "@strip(" ab    cd ")"
            `@strip( ab,"  cd", ef)`  =>>  "@strip( ab,"  cd", ef)"
            `@strip( ab, " cd", ef)`  =>>  "@strip( ab, " cd", ef)"


    `@fmt()  `
    `--------`
            `@fmt(123456)              ` ->> "@fmt(123456)"
            `@fmt(123456,99999999)     ` ->> "@fmt(123456,99999999)"
            `@fmt(123456,999)          ` ->> "@fmt(123456,999)"
            `@fmt(123.456,999999)      ` ->> "@fmt(123.456,999999)"
            `@fmt(123456,00999999)     ` ->> "@fmt(123456,00999999)"
            `@fmt(123456,"ABC999,999Z")` ->> "@fmt(123,"ABC999,999Z")"
            `@fmt(123456,ABC999999Z)   ` ->> "@fmt(123,ABC999999Z)"


    TIME & DATE
    -----------

    Tests of @date and @time were cancelled due to impossible comparison... Time flies...

    `@date()`
    `-------`
            `@date()            `   =>>  `"@date()"              `
            `@date(dd-mm-yyyy)  `   =>>  `"@date(dd-mm-yyyy)"    `
            `@date(dd-mm-yy)    `   =>>  `"@date(dd-mm-yy)"      `
            `@date(d-m-y)       `   =>>  `"@date(d-m-y)"         `
            `@date(yy/mm/dd)    `   =>>  `"@date(yy/mm/dd)"      `
            `@date(mm/dd/yyyy)  `   =>>  `"@date(mm/dd/yyyy)"    `
            `@date(mm/xxxx/zzzz)`   =>>  `"@date(mm/xxxx/zzzz)"  `

    `@time()`
    `-------
            `@time()            `   =>>  `"@time()"              `
            `@time(hh:mm:ss)    `   =>>  `"@time(hh:mm:ss)"      `
            `@time(hhhh:mm)     `   =>>  `"@time(hhhh:mm)"       `
            `@time(xxx:mm/dd)   `   =>>  `"@time(xxx:mm/dd)"     `

    `@month()       `
    `--------`
            `@month(1)  `           =>>  "@month(1)"
            `@month(1,F)`           =>>  "@month(1,F)"
            `@month(1,e)`           =>>  "@month(1,e)"
            `@month(1,N)`           =>>  "@month(1,N)"

    FILES
    -----

    `@fdelete()     `
    `--------`
            `@fdelete(essais.txt)`  =>>  "@fdelete(essais.txt)"

    `@fappend()     `
    `--------`
            `@fappend(essais.txt,"line 1",NL,line 2,NL,NL,line 4)`  =>>  "@fappend(essais.txt,"line 1",NL,line 2,NL,NL,line 4)"


    ## OBJECTS
    ## -------

    $ Load objects
    $WsLoadCmt %input_dir%\fun
    $WsLoadEqs %input_dir%\fun
    $WsLoadIdt %input_dir%\fun
    $WsLoadLst %input_dir%\fun
    $WsLoadScl %input_dir%\fun
    $WsLoadTbl %input_dir%\fun
    $WsLoadVar %input_dir%\fun


    `@sstderr()   `
    `----------`
            `@sstderr(acaf1)`               =>>  "@sstderr(acaf1)"
            `@sstderr(acaf1,xyz,acaf3)`     =>>  "@sstderr(acaf1,xyz,acaf3)"
            `@sstderr(acaf1,acaf2, acaf3)`  =>>  "@sstderr(acaf1,acaf2, acaf3)"

    `@srelax()   `
    `---------`
            `@srelax(acaf1)`                =>>  "@srelax(acaf1)"
            `@srelax(acaf1,xyz,acaf3)`      =>>  "@srelax(acaf1,xyz,acaf3)"
            `@srelax(acaf1,acaf2, acaf3)`   =>>  "@srelax(acaf1,acaf2, acaf3)"

    `@ttitle()      `
    `--------`
            `@ttitle(C8_1)       `          =>>  "@ttitle(C8_1)"
            `@ttitle( c8_10)     `          =>>  "@ttitle( c8_10)"
            `@ttitle(C8_1 c8_10) `          =>>  "@ttitle(C8_1 c8_10)"
            `@ttitle(C8_1, c8_10)`          =>>  "@ttitle(C8_1, c8_10)"
            `@ttitle(XXX)        `          =>>  "@ttitle(XXX)"

    `@cvalue()      `
    `--------`
            `@cvalue(acaf)      `           =>>  "@cvalue(acaf)"
            `@cvalue(ACAF)      `           =>>  "@cvalue(ACAF)"
            `@cvalue(ACAG)      `           =>>  "@cvalue(ACAG)"
            `@cvalue(ACAF,ACAG) `           =>>  "@cvalue(ACAF,ACAG)"

    `@vvalue()      `
    `--------`
            `@vvalue(acaf)      `           =>>  "@vvalue(acaf)"
            `@vvalue(ACAF)      `           =>>  "@vvalue(ACAF)"
            `@vvalue(ACAG)      `           =>>  "@vvalue(ACAG)"
            `@vvalue(ACAF,ACAG) `           =>>  "@vvalue(ACAF,ACAG)"
            `@vvalue(XXX)       `           =>>  "@vvalue(XXX)"
            `@vvalue()          `           =>>  "@vvalue()"


    `@lvalue()      `
    `--------`
            `@lvalue(endo)         `        =>>  "@lvalue(endo)"
            `@lvalue(ENDO)         `        =>>  "@lvalue(ENDO)"
            `@lvalue(_SEARCH,XENVI)`        =>>  "@lvalue(_SEARCH,XENVI)"
            `@lvalue(XXX)          `        =>>  "@lvalue(XXX)"
            `@lvalue()             `        =>>  "@lvalue()"

    `@ivalue()      `
    `--------`
            `@ivalue(FLGR)          `       =>>  "@ivalue(FLGR)"
            `@ivalue(FLGR,IUGR)     `       =>>  "@ivalue(FLGR,IUGR)"
            `@ivalue(flgr)          `       =>>  "@ivalue(flgr)"
            `@ivalue(XXX)           `       =>>  "@ivalue(XXX)"
            `@ivalue()              `       =>>  "@ivalue()"

    `@evalue()      `
    `--------`
            `@evalue(ACAF)          `       =>>  "@evalue(ACAF)"
            `@evalue(brugp)         `       =>>  "@evalue(brugp)"
            `@evalue(acaf,brugp)    `       =>>  "@evalue(acaf,brugp)"
            `@evalue(XXX)           `       =>>  "@evalue(XXX)"
            `@evalue()              `       =>>  "@evalue()"

    `@eqsample()    `
    `--------`
            `@eqsample(ACAF)        `       =>>  "@eqsample(ACAF)"
            `@eqsample(ACAF,ACAG)   `       =>>  "@eqsample(ACAF,ACAG)"
            `@eqsample(XXX)         `       =>>  "@eqsample(XXX)"
            `@eqsample()            `       =>>  "@eqsample()"

    `@eqsamplefrom()`
    `--------`
            `@eqsamplefrom(ACAF)    `       =>>  "@eqsamplefrom(ACAF)"
            `@eqsamplefrom(XXX)     `       =>>  "@eqsamplefrom(XXX)"
            `@eqsamplefrom()        `       =>>  "@eqsamplefrom()"

    `@eqsampleto()  `
    `--------`
            `@eqsampleto(ACAF)      `       =>>  "@eqsampleto(ACAF)"
            `@eqsampleto(XXX)       `       =>>  "@eqsampleto(XXX)"
            `@eqsampleto()          `       =>>  "@eqsampleto()"

    `@eqlhs()       `
    `--------`
            `@eqlhs(ACAF)      `       =>>  "@eqlhs(ACAF)"
            `@eqlhs(ACAF,ACAG) `       =>>  "@eqlhs(ACAF,ACAG)"
            `@eqlhs(XXX)       `       =>>  "@eqlhs(XXX)"
            `@eqlhs()          `       =>>  "@eqlhs()"

    `@eqrhs()       `
    `--------`
            `@eqrlhs(ACAF)      `       =>>  "@eqrhs(ACAF)"
            `@eqrlhs(ACAF,ACAG) `       =>>  "@eqrhs(ACAF,ACAG)"
            `@eqrlhs(XXX)       `       =>>  "@eqrhs(XXX)"
            `@eqrlhs()          `       =>>  "@eqrhs()"

    `@count()       `
    `--------`
            `@count(ACAF,ACAG) `        =>>  "@count(ACAF,ACAG)"
            `@count(ACAF;ACAG) `        =>>  "@count(ACAF;ACAG)"
            `@count()          `        =>>  "@count()"

    `@index()       `
    `--------`
            `@index(0,A,B,C) `          =>>  "@index(0,A,B,C)"
            `@index(2,A,B,C) `          =>>  "@index(2,A,B,C)"
            `@index(4,A,B,C) `          =>>  "@index(4,A,B,C)"
            `@index(1,A;B;C) `          =>>  "@index(1,A;B;C)"
            `@index(A;B;C)   `          =>>  "@index(A;B;C)"
            `@index()        `          =>>  "@index()"


    `@sample()      `
    `--------`
            `@sample()  `               =>>  "@sample()"
            `@sample(B) `               =>>  "@sample(B)"
            `@sample(E) `               =>>  "@sample(E)"
            `@sample(X) `               =>>  "@sample(X)"




    `@vliste()      `
    `--------`
            `@vliste(ACAF) `            =>>  "@vliste(ACAF)"
            `@vliste(ACAF,ACAG) `       =>>  "@vliste(ACAF,ACAG)"


    `@sliste()      `
    `--------`
            `@sliste(ACAF) `            =>>  "@sliste(ACAF)"
            `@sliste(ACAF,ACAG) `       =>>  "@sliste(ACAF,ACAG)"

    `@cexpand()     `
    `--------`
            `@cexpand(AC*)    `       =>>  "@cexpand(AC*)"
            `@cexpand(*U)     `       =>>  "@cexpand(*U)"
            `@cexpand(AC*,*U) `       =>>  "@cexpand(AC*,*U)"
            `@cexpand()       `       =>>  "@cexpand()"


    `@eexpand()     `
    `--------`
            `@eexpand(AC*)    `       =>>  "@eexpand(AC*)"
            `@eexpand(*U)     `       =>>  "@eexpand(*U)"
            `@eexpand(AC*,*U) `       =>>  "@eexpand(AC*,*U)"
            `@eexpand()       `       =>>  "@eexpand()"

    `@iexpand()     `
    `--------`
            `@iexpand(AC*)    `       =>>  "@iexpand(AC*)"
            `@iexpand(*U)     `       =>>  "@iexpand(*U)"
            `@iexpand(AC*,*U) `       =>>  "@iexpand(AC*,*U)"
            `@iexpand()       `       =>>  "@iexpand()"

    `@lexpand()     `
    `--------`
            `@lexpand(AC*)    `       =>>  "@lexpand(AC*)"
            `@lexpand(*0)     `       =>>  "@lexpand(*0)"
            `@lexpand(AC*,*0) `       =>>  "@lexpand(AC*,*0)"
            `@lexpand()       `       =>>  "@lexpand()"

    `@sexpand()     `
    `--------`
            `@sexpand(AC*)    `       =>>  "@sexpand(AC*)"
            `@sexpand(ac*)    `       =>>  "@sexpand(ac*)"
            `@sexpand(*4)     `       =>>  "@sexpand(*4)"
            `@sexpand(AC*,*4) `       =>>  "@sexpand(AC*,*4)"
            `@sexpand()       `       =>>  "@sexpand()"

    `@texpand()     `
    `----------`
            `@texpand(AC*)    `       =>>  "@texpand(AC*)"
            `@texpand(*U)     `       =>>  "@texpand(*U)"
            `@texpand(AC*,*U) `       =>>  "@texpand(AC*,*U)"
            `@texpand()       `       =>>  "@texpand()"

    `@vexpand()     `
    `----------`
            `@vexpand(AC*)    `       =>>  "@vexpand(AC*)"
            `@vexpand(*U)     `       =>>  "@vexpand(*U)"
            `@vexpand(AC*,*U) `       =>>  "@vexpand(AC*,*U)"
            `@vexpand()       `       =>>  "@vexpand()"

    ## SIMULATIONS
    ## -----------

    $ModelSimulateParms 0.0001 0.7 100 Both 0 no 5 no
    $ModelSimulate 2000Y1 2001Y1

    `@SimMaxit()    `
    `-----------`
            `@SimMaxit()   `          =>>  "@SimMaxit()"

    `@SimEps()      `
    `---------`
            `@SimEps()   `            =>>  "@SimEps()"

    `@SimRelax()    `
    `-----------`
            `@SimRelax()   `          =>>  "@SimRelax()"

    `@SimNorm()     `
    `----------`
            `@SimNorm(2000Y1)   `     =>>  "@SimNorm(2000Y1)"
            `@SimNorm(2001Y1)   `     =>>  "@SimNorm(2001Y1)"
            `@SimNorm(2002Y1)   `     =>>  "@SimNorm(2002Y1)"
            `@SimNorm(1990Y1)   `     =>>  "@SimNorm(1990Y1)"

    `@SimNIter()    `
    `-----------`
            `@SimNiter(2000Y1)   `    =>>  "@SimNiter(2000Y1)"
            `@SimNiter(2001Y1)   `    =>>  "@SimNiter(2001Y1)"
            `@SimNiter(2002Y1)   `    =>>  "@SimNiter(2002Y1)"
            `@SimNiter(1990Y1)   `    =>>  "@SimNiter(1990Y1)"


    `@vtake()       `
    `--------`
            `@vtake(2,A,B,C,D,E)   `  =>>  "@vtake(2,A,B,C,D,E)"
            `@vtake(-1;A;B;C;D;E)  `  =>>  "@vtake(-1;A;B;C;D;E)"
            `@vtake(3 A B)  `         =>>  "@vtake(3 A B)"
            `@vtake(3,A,"B C")  `     =>>  "@vtake(3,A,"B C")"
            `@vtake(A,B)  `           =>>  "@vtake(A,B)"
            `@vtake()  `              =>>  "@vtake()"


    `@vdrop()       `
    `--------`
            `@vdrop(2,A,B,C,D,E)   `  =>>  "@vdrop(2,A,B,C,D,E)"
            `@vdrop(-1;A;B;C;D;E)  `  =>>  "@vdrop(-1;A;B;C;D;E)"
            `@vdrop(3 A B)  `         =>>  "@vdrop(3 A B)"
            `@vdrop(A,B)  `           =>>  "@vdrop(A,B)"
            `@vdrop()  `              =>>  "@vdrop()"

    `@vcount()      `
    `--------`
            `@vcount(A,B,C,D,E)   `   =>>  "@vcount(A,B,C,D,E)"
            `@vcount(C;D;E)       `   =>>  "@vcount(C;D;E)"
            `@vcount(3 A B)       `   =>>  "@vcount(3 A B)"
            `@vcount(3 "A B")     `   =>>  "@vcount(3,"A B")"
            `@vcount()            `   =>>  "@vcount()"

    ## SYSTEM
    ## ------

    `@memory()      `
    `---------`
            `@memory()  `               =>>  "...unstable..."

    `@ChronoReset() `
    `--------------`
            `@chronoreset()`            =>>  "@chronoreset()"

    `@ChronoGet()   `
    `------------`
            `@chronoget()`              =>>  "@chronoget()"



    `@getdir()      `
    `---------`
            `@getdir()`                 =>>  "...unstable..."

    `@mkdir()       `
    `--------`
            `@mkdir(testdir)`           =>>  "@mkdir(testdir)"
            `@mkdir(testdir)`           =>>  "@mkdir(testdir)"
            `@getdir()      `           =>>  "...unstable..."
            `@chdir(testdir)`           =>>  "...unstable..."
            `@getdir()      `           =>>  "...unstable..."

    `@chdir()       `
    `--------`
            `@chdir(..)`                =>>  "...unstable..."
            `@getdir()`                 =>>  "...unstable..."

    `@rmdir()       `
    `--------`
            `@getdir()      `           =>>  "...unstable..."
            `@mkdir(testdir)`           =>>  "@mkdir(testdir)"
            `@chdir(testdir)`           =>>  "...unstable..."
            `@chdir(..)`                =>>  "...unstable..."
            `@rmdir(testdir)`           =>>  "@rmdir(testdir)"

    `@void()        `
    `-------`
            `@void(ABC,DEF,G)      `    =>>  "@void(ABC,DEF,G)"
            `@void(@getdir())      `    =>>  "@void(@getdir())"
            `@void(@vexpand(*))    `    =>>  "@void(@vexpand(*))"

    $return
&TX

&TI Resulting a2m file
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
    STRINGS
    =======

    @take()
    -------
        Success
            @take(2,ABCDE)        =>> "AB"
            @take(-2,ABCDE)       =>> "DE"
            @take(2, ABCDE)       =>> " A"
            @take(0,ABCDE)        =>> ""
            @take(2,"A,B,C,D,E")  =>> "A,"

        Errors (##parms <<>> 2)
            @take(2,A,B,C,D,E)    =>> "@take(2,A,B,C,D,E)"
            @take(2,A,BCDE)       =>> "@take(2,A,BCDE)"

    @drop()
    -------
        Success
            @drop(2,ABCDE)        =>> "CDE"
            @drop(-1,ABCDE)       =>> "ABCD"
            @drop(2, ABCDE)       =>> "BCDE"
            @drop(0,ABCDE)        =>> "ABCDE"
            @drop(2,"A,B,C,D,E")  =>> "B,C,D,E"
            @DROP(2,A)            =>> ""
            @drop(3, "a bc")      =>> " bc""

        Error (##parms <<>> 2)
            @drop(2,A,B,C,D,E)    =>> "@drop(2,A,B,C,D,E)"
            @drop(2,A,BCDE)       =>> "@drop(2,A,BCDE)"
            @drop(n,ABCDE)        =>> "ABCDE"

    @replace()
    ----------
        Success
            @replace(Buro du Plan,Buro,Bureau)            =>> Bureau du Plan
            @replace(Buro du Buro du Plan,Buro,Bureau)    =>> Bureau du Bureau du Plan
            @replace(Bureauplan,plan, du Plan)            =>> Bureau du Plan
            @replace("Bureau, du Plan",",","")            =>> Bureau du Plan
            @replace("Bureau, du Plan", ",", "")          =>> Bureau, du Plan

        Error
            @replace(Buro plan,Bureau du Plan)            =>> @replace(Buro plan,Bureau du Plan)
            @replace(AB,B,C,D)                            =>> @replace(AB,B,C,D)

    @equal()
    --------
            @equal(aa,aa)         =>>  "1"
            @equal(aa, aa)        =>>  "0"
            @equal(aa,bb)         =>>  "0"
            @equal(aa,aa,aa)      =>>  "0"

    @upper()
    ------
            @upper(ab cd,ef)      =>>  "AB CD EF"
            @upper(AB)            =>>  "AB"
            @upper("abc", "def")  =>>  "ABC  "DEF""
            @upper("abc","def")   =>>  "ABC DEF"

    @lower()
    ------
            @lower(ab cd,ef)      =>>  "ab cd ef"
            @lower(AB)            =>>  "ab"
            @lower("abc", "def")  =>>  "abc  "def""
            @lower("abc","def")   =>>  "abc def"

    @sqz()
    ------
            @sqz( ab    cd )      =>>  "abcd"
            @sqz(" ab    cd ")    =>>  "abcd"
            @sqz( ab,"  cd", ef)  =>>  "abcdef"
            @sqz( ab, " cd", ef)  =>>  "ab"cd"ef"

    @strip()
    --------
            @strip( ab    cd )      =>>  " ab    cd"
            @strip(" ab    cd ")    =>>  " ab    cd"
            @strip( ab,"  cd", ef)  =>>  " ab   cd  ef"
            @strip( ab, " cd", ef)  =>>  " ab  " cd"  ef"


    @fmt()
    --------
            @fmt(123456)               ->> "123456"
            @fmt(123456,99999999)      ->> "  123456"
            @fmt(123456,999)           ->> "456"
            @fmt(123.456,999999)       ->> "   123"
            @fmt(123456,00999999)      ->> "00123456"
            @fmt(123456,"ABC999,999Z") ->> "ABC   ,123Z"
            @fmt(123456,ABC999999Z)    ->> "ABC   123Z"


    TIME and DATE
    -------------

    Tests of @date and @time were cancelled due to impossible comparison... Time flies...

    @date()
    -------
            @date()               =>>  "@date()"
            @date(dd-mm-yyyy)     =>>  "@date(dd-mm-yyyy)"
            @date(dd-mm-yy)       =>>  "@date(dd-mm-yy)"
            @date(d-m-y)          =>>  "@date(d-m-y)"
            @date(yy/mm/dd)       =>>  "@date(yy/mm/dd)"
            @date(mm/dd/yyyy)     =>>  "@date(mm/dd/yyyy)"
            @date(mm/xxxx/zzzz)   =>>  "@date(mm/xxxx/zzzz)"

    @time()
    ------
            @time()               =>>  "@time()"
            @time(hh:mm:ss)       =>>  "@time(hh:mm:ss)"
            @time(hhhh:mm)        =>>  "@time(hhhh:mm)"
            @time(xxx:mm/dd)      =>>  "@time(xxx:mm/dd)"

    @month()
    --------
            @month(1)             =>>  "Janvier"
            @month(1,F)           =>>  "Janvier"
            @month(1,e)           =>>  "January"
            @month(1,N)           =>>  "Januari"

    FILES
    -----

    @fdelete()
    --------
            @fdelete(essais.txt)  =>>  ""

    @fappend()
    --------
            @fappend(essais.txt,"line 1",NL,line 2,NL,NL,line 4)  =>>  ""





    @sstderr()
    ----------
            @sstderr(acaf1)               =>>  "0.001369"
            @sstderr(acaf1,xyz,acaf3)     =>>  "0.001369 -- 0.873010"
            @sstderr(acaf1,acaf2, acaf3)  =>>  "0.001369 0.000001 0.873010"

    @srelax()
    ---------
            @srelax(acaf1)                =>>  "1.000000"
            @srelax(acaf1,xyz,acaf3)      =>>  "1.000000 -- 1.000000"
            @srelax(acaf1,acaf2, acaf3)   =>>  "1.000000 1.000000 1.000000"

    @ttitle()
    --------
            @ttitle(C8_1)                 =>>  "Dterminants de l'output potentiel"
            @ttitle( c8_10)               =>>  "Coin salarial parafiscal"
            @ttitle(C8_1 c8_10)           =>>  "Table C8_1 c8_10 not found"
            @ttitle(C8_1, c8_10)          =>>  "Dterminants de l'output potentiel
    Coin salarial parafiscal"
            @ttitle(XXX)                  =>>  "Table XXX not found"

    @cvalue()
    --------
            @cvalue(acaf)                 =>>  "Cmt acaf not found"
            @cvalue(ACAF)                 =>>  "Ondernemingen: ontvangen kapitaaloverdrachten."
            @cvalue(ACAG)                 =>>  "Totale overheid: netto ontvangen kapitaaloverdrachten."
            @cvalue(ACAF,ACAG)            =>>  "Ondernemingen: ontvangen kapitaaloverdrachten.;Totale overheid: netto ontvangen kapitaaloverdrachten."

    @vvalue()
    --------
            @vvalue(acaf)                 =>>  "na  na  na  na  na  na  na  na  na  na  1.2130001  5.2020001  9.184  8.0790005  11.332  13.518001  15.784  16.544001  21.489  20.281  21.277  32.417999  24.446999  27.025002  24.504  27.560999  25.542  27.499001  25.353001  17.165001  23.771  26.240999  30.159  34.661999  8.1610022  -13.130997  32.171001  39.935799  29.645657  13.530404919696  10.0466107922005  2.86792273645546  -0.929212509051645  -6.09156498756888  -14.5820944628981  -26.5387895697886  -28.9872879825975  -33.3784257842954  -38.4095177823974  -37.4635096412738  -37.8274288322944  -44.5447926335432  -55.5592898172187  -68.8946543226201  -83.3406251108009  -96.4104198284833  "
            @vvalue(ACAF)                 =>>  "na  na  na  na  na  na  na  na  na  na  1.2130001  5.2020001  9.184  8.0790005  11.332  13.518001  15.784  16.544001  21.489  20.281  21.277  32.417999  24.446999  27.025002  24.504  27.560999  25.542  27.499001  25.353001  17.165001  23.771  26.240999  30.159  34.661999  8.1610022  -13.130997  32.171001  39.935799  29.645657  13.530404919696  10.0466107922005  2.86792273645546  -0.929212509051645  -6.09156498756888  -14.5820944628981  -26.5387895697886  -28.9872879825975  -33.3784257842954  -38.4095177823974  -37.4635096412738  -37.8274288322944  -44.5447926335432  -55.5592898172187  -68.8946543226201  -83.3406251108009  -96.4104198284833  "
            @vvalue(ACAG)                 =>>  "na  na  na  na  na  na  na  na  na  na  -11.028999  -15.847  -19.288002  -21.814999  -25.447002  -24.618999  -27.770998  -28.839001  -29.434998  -30.411001  -30.353001  -41.060997  -31.178001  -32.604  -30.237003  -38.061001  -31.939999  -35.59  -37.238003  -25.991001  -28.1721855713507  -30.934  -40.285999  -43.157997  -16.029003  -41.845993  -40.237  -32.93  -38.345695  -39.8581741316036  -41.534786567348  18.9398011359783  19.9808148751188  21.0205021787734  22.0664755229642  23.1079621640615  24.1296371451098  25.1609090496654  26.1921114843413  27.2299551185986  28.2539289782105  29.2846003640349  30.3239611503116  31.3701388106954  32.4202988291984  33.469601344881  "
            @vvalue(ACAF,ACAG)            =>>  "na  na  na  na  na  na  na  na  na  na  1.2130001  5.2020001  9.184  8.0790005  11.332  13.518001  15.784  16.544001  21.489  20.281  21.277  32.417999  24.446999  27.025002  24.504  27.560999  25.542  27.499001  25.353001  17.165001  23.771  26.240999  30.159  34.661999  8.1610022  -13.130997  32.171001  39.935799  29.645657  13.530404919696  10.0466107922005  2.86792273645546  -0.929212509051645  -6.09156498756888  -14.5820944628981  -26.5387895697886  -28.9872879825975  -33.3784257842954  -38.4095177823974  -37.4635096412738  -37.8274288322944  -44.5447926335432  -55.5592898172187  -68.8946543226201  -83.3406251108009  -96.4104198284833  ;na  na  na  na  na  na  na  na  na  na  -11.028999  -15.847  -19.288002  -21.814999  -25.447002  -24.618999  -27.770998  -28.839001  -29.434998  -30.411001  -30.353001  -41.060997  -31.178001  -32.604  -30.237003  -38.061001  -31.939999  -35.59  -37.238003  -25.991001  -28.1721855713507  -30.934  -40.285999  -43.157997  -16.029003  -41.845993  -40.237  -32.93  -38.345695  -39.8581741316036  -41.534786567348  18.9398011359783  19.9808148751188  21.0205021787734  22.0664755229642  23.1079621640615  24.1296371451098  25.1609090496654  26.1921114843413  27.2299551185986  28.2539289782105  29.2846003640349  30.3239611503116  31.3701388106954  32.4202988291984  33.469601344881  "
            @vvalue(XXX)                  =>>  "VAR XXX not found"
            @vvalue()                     =>>  "@vvalue()"


    @lvalue()
    --------
            @lvalue(endo)                 =>>  "$endo0,$endo1,"
            @lvalue(ENDO)                 =>>  "$endo0,$endo1,"
            @lvalue(_SEARCH,XENVI)        =>>  "ZKF,XEX,XPWMAB,XPWMS,XPWXAB,XPWXS,XQWXAB,XQWXS,XPOIL,XNATY,XTFP"
            @lvalue(XXX)                  =>>  "List XXX not found"
            @lvalue()                     =>>  "@lvalue()"

    @ivalue()
    --------
            @ivalue(FLGR)                 =>>  "FLG/VBBP"
            @ivalue(FLGR,IUGR)            =>>  "FLG/VBBP;IUG/VBBP"
            @ivalue(flgr)                 =>>  "FLG/VBBP"
            @ivalue(XXX)                  =>>  "Idt XXX not found"
            @ivalue()                     =>>  "@ivalue()"

    @evalue()
    --------
            @evalue(ACAF)                 =>>  "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+ acaf4*(TIME=1995)"
            @evalue(brugp)                =>>  "BRUGP := 0"
            @evalue(acaf,brugp)           =>>  "(ACAF/VAF[-1]) :=acaf1+acaf2*GOSF[-1]+ acaf4*(TIME=1995);BRUGP := 0"
            @evalue(XXX)                  =>>  "Eqs XXX not found"
            @evalue()                     =>>  "@evalue()"

    @eqsample()
    --------
            @eqsample(ACAF)               =>>  "1980Y1:1996Y1"
            @eqsample(ACAF,ACAG)          =>>  "@eqsample(ACAF,ACAG)"
            @eqsample(XXX)                =>>  "[Eqs XXX not found]"
            @eqsample()                   =>>  "@eqsample()"

    @eqsamplefrom()
    --------
            @eqsamplefrom(ACAF)           =>>  "1980Y1"
            @eqsamplefrom(XXX)            =>>  "[Eqs XXX not found]"
            @eqsamplefrom()               =>>  "@eqsamplefrom()"

    @eqsampleto()
    --------
            @eqsampleto(ACAF)             =>>  "1996Y1"
            @eqsampleto(XXX)              =>>  "[Eqs XXX not found]"
            @eqsampleto()                 =>>  "@eqsampleto()"

    @eqlhs()
    --------
            @eqlhs(ACAF)             =>>  "(ACAF/VAF[-1]) "
            @eqlhs(ACAF,ACAG)        =>>  "@eqlhs(ACAF,ACAG)"
            @eqlhs(XXX)              =>>  "[Eqs XXX not found]"
            @eqlhs()                 =>>  "@eqlhs()"

    @eqrhs()
    --------
            @eqrlhs(ACAF)             =>>  "acaf1+acaf2*GOSF[-1]+
    acaf4*(TIME=1995)"
            @eqrlhs(ACAF,ACAG)        =>>  "@eqrhs(ACAF,ACAG)"
            @eqrlhs(XXX)              =>>  "[Eqs XXX not found]"
            @eqrlhs()                 =>>  "@eqrhs()"

    @count()
    --------
            @count(ACAF,ACAG)         =>>  "2"
            @count(ACAF;ACAG)         =>>  "1"
            @count()                  =>>  "0"

    @index()
    --------
            @index(0,A,B,C)           =>>  ""
            @index(2,A,B,C)           =>>  "B"
            @index(4,A,B,C)           =>>  ""
            @index(1,A;B;C)           =>>  "A;B;C"
            @index(A;B;C)             =>>  ""
            @index()                  =>>  ""


    @sample()
    --------
            @sample()                 =>>  "1960Y1 2015Y1"
            @sample(B)                =>>  "1960Y1"
            @sample(E)                =>>  "2015Y1"
            @sample(X)                =>>  "1960Y1 2015Y1"




    @vliste()
    --------
            @vliste(ACAF)             =>>  "ACAF;VAF;GOSF;TIME"
            @vliste(ACAF,ACAG)        =>>  "ACAF;VAF;GOSF;TIME;ACAG;VBBP"


    @sliste()
    --------
            @sliste(ACAF)             =>>  "acaf1;acaf2;acaf4"
            @sliste(ACAF,ACAG)        =>>  "acaf1;acaf2;acaf4"

    @cexpand()
    --------
            @cexpand(AC*)           =>>  "ACAF;ACAG"
            @cexpand(*U)            =>>  "DPU;DPUU;IFU;IHU;WBU"
            @cexpand(AC*,*U)        =>>  "ACAF;ACAG;DPU;DPUU;IFU;IHU;WBU"
            @cexpand()              =>>  ""


    @eexpand()
    --------
            @eexpand(AC*)           =>>  "ACAF;ACAG"
            @eexpand(*U)            =>>  "CGU;DPU;DPUU;IFU;IHU;WBU"
            @eexpand(AC*,*U)        =>>  "ACAF;ACAG;CGU;DPU;DPUU;IFU;IHU;WBU"
            @eexpand()              =>>  ""

    @iexpand()
    --------
            @iexpand(AC*)           =>>  ""
            @iexpand(*U)            =>>  "MU;NAWRU"
            @iexpand(AC*,*U)        =>>  "MU;NAWRU"
            @iexpand()              =>>  ""

    @lexpand()
    --------
            @lexpand(AC*)           =>>  ""
            @lexpand(*0)            =>>  "COPY0;ENDO0;TOTAL0"
            @lexpand(AC*,*0)        =>>  "COPY0;ENDO0;TOTAL0"
            @lexpand()              =>>  ""

    @sexpand()
    --------
            @sexpand(AC*)           =>>  "acaf1;acaf2;acaf3;acaf4"
            @sexpand(ac*)           =>>  "acaf1;acaf2;acaf3;acaf4"
            @sexpand(*4)            =>>  "acaf4;dtf4;gamma4;k4;kl14;nfyh4;pc4;pmab4;pxs4;qc4;qi5_4;qmab_4;qms4;qxab4;qxs4;y4"
            @sexpand(AC*,*4)        =>>  "acaf1;acaf2;acaf3;acaf4;dtf4;gamma4;k4;kl14;nfyh4;pc4;pmab4;pxs4;qc4;qi5_4;qmab_4;qms4;qxab4;qxs4;y4"
            @sexpand()              =>>  ""

    @texpand()
    ----------
            @texpand(AC*)           =>>  ""
            @texpand(*U)            =>>  "MULT1RESU;MULT2RESU;NAWRU;T1NIVEAU"
            @texpand(AC*,*U)        =>>  "MULT1RESU;MULT2RESU;NAWRU;T1NIVEAU"
            @texpand()              =>>  ""

    @vexpand()
    ----------
            @vexpand(AC*)           =>>  "ACAF;ACAG"
            @vexpand(*U)            =>>  "CGU;DPU;DPUU;IFU;IHU;MU;NAWRU;WBU"
            @vexpand(AC*,*U)        =>>  "ACAF;ACAG;CGU;DPU;DPUU;IFU;IHU;MU;NAWRU;WBU"
            @vexpand()              =>>  ""



    @SimMaxit()
    -----------
            @SimMaxit()             =>>  "100"

    @SimEps()
    ---------
            @SimEps()               =>>  "0.0001"

    @SimRelax()
    -----------
            @SimRelax()             =>>  "0.700000"

    @SimNorm()
    ----------
            @SimNorm(2000Y1)        =>>  "7.15565e-05"
            @SimNorm(2001Y1)        =>>  "6.85269e-05"
            @SimNorm(2002Y1)        =>>  "0"
            @SimNorm(1990Y1)        =>>  "0"

    @SimNIter()
    -----------
            @SimNiter(2000Y1)       =>>  "31"
            @SimNiter(2001Y1)       =>>  "37"
            @SimNiter(2002Y1)       =>>  "0"
            @SimNiter(1990Y1)       =>>  "0"


    @vtake()
    --------
            @vtake(2,A,B,C,D,E)     =>>  "A;B"
            @vtake(-1;A;B;C;D;E)    =>>  "E"
            @vtake(3 A B)           =>>  "A;B"
            @vtake(3,A,"B C")       =>>  "A;B;C"
            @vtake(A,B)             =>>  ""
            @vtake()                =>>  ""


    @vdrop()
    --------
            @vdrop(2,A,B,C,D,E)     =>>  "C;D;E"
            @vdrop(-1;A;B;C;D;E)    =>>  "A;B;C;D"
            @vdrop(3 A B)           =>>  ""
            @vdrop(A,B)             =>>  "B"
            @vdrop()                =>>  ""

    @vcount()
    --------
            @vcount(A,B,C,D,E)      =>>  "5"
            @vcount(C;D;E)          =>>  "3"
            @vcount(3 A B)          =>>  "3"
            @vcount(3 "A B")        =>>  "3"
            @vcount()               =>>  "0"


    @memory()
    ---------
            @memory()                 =>>  "...unstable..."

    @ChronoReset()
    --------------
            @chronoreset()            =>>  ""

    @ChronoGet()
    ------------
            @chronoget()              =>>  "0"



    @getdir()
    ---------
            @getdir()                 =>>  "...unstable..."

    @mkdir()
    --------
            @mkdir(testdir)           =>>  ""
            @mkdir(testdir)           =>>  ""
            @getdir()                 =>>  "...unstable..."
            @chdir(testdir)           =>>  "...unstable..."
            @getdir()                 =>>  "...unstable..."

    @chdir()
    --------
            @chdir(..)                =>>  "...unstable..."
            @getdir()                 =>>  "...unstable..."

    @rmdir()
    --------
            @getdir()                 =>>  "...unstable..."
            @mkdir(testdir)           =>>  ""
            @chdir(testdir)           =>>  "...unstable..."
            @chdir(..)                =>>  "...unstable..."
            @rmdir(testdir)           =>>  ""

    @void()
    -------
        @void(ABC,DEF,G)          =>>  ""
        @void(@getdir())          =>>  ""
        @void(@vexpand(*))        =>>  ""
&TX
>

<Example of report with $procdef and $foreach>
Example of report with $procdef and $foreach
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&TI Report
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
    $ Testing various procedures
    $ --------------------------
    $
    $  Syntax: rep_proc input_dir output_dir
    $
    $  Example: rep_proc data output
    $
    $debug 1
    $indent
    $goto continue {%0% >>= 2}
        $show Syntax: rep_proc input_dir output_dir
        $show Syntax error. Report aborted.
        $return
    $label continue

    $define input_dir %1%
    $define output_dir %2%
    $ Define output destination
    $ -------------------------
    $PrintDest %output_dir%\rep_proc.a2m A2M

    $ ----------------------------------------------------------
    $ printtbl: prints a list of variables on a specific period
    $procdef printtbl year vars
        $foreach var %vars%
            %var%[%year%] = {%var%[%year%]}
        $next var
    $procend

    $ ----------------------------------------------------------
    $ sumvars: creates a new series names %result% = sum of %vars%
    $procdef sumvars result vars
        $define eq 0
        $foreach var %vars%
            $define eq %eq% + %var%
        $next var
        $DataCalcVar %result% %eq%
    $procend

    $ ----------------------------------------------------------
    $ sumvarsmsg: displays msg, then calls sumvars result vars
    $procdef sumvarsmsg msg result vars
        %msg%
        $ During the call : result = '%result%' vars = '%vars%'
        $procexec sumvars %result% %vars%
        %result%[1992Y1] = {%result%[1992Y1]}
    $procend

    $WsSample 1990Y1 2000Y1
    $DataCalcVar PIB_A t
    $DataCalcVar PIB_B ln t
    $DataCalcVar PIB_C sin(t)
    $procexec sumvarsmsg "Hello IODE!" PIB PIB_A PIB_B
    $procexec sumvarsmsg "Hello World" PIB3 PIB_A PIB_C
    $procexec printtbl 1992Y1 PIB_A PIB_B PIB_C PIB PIB3

    $ Load objects
    $ ------------
    $WsLoadCmt %input_dir%\fun
    $WsLoadEqs %input_dir%\fun
    $WsLoadIdt %input_dir%\fun
    $WsLoadLst %input_dir%\fun
    $WsLoadScl %input_dir%\fun
    $WsLoadTbl %input_dir%\fun
    $WsLoadVar %input_dir%\fun

    $procdef print_eq eqs
    .par1 tit_1 
    Equations with coefficients
    
        $PrintObjLec 1
        $foreach eq %eqs%
            $PrintObjDefEqs %eq%
        $next eq
    $procend
    $procexec print_eq ACAG ACAF
&TX

&TI Resulting a2m file
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
&CO
        Hello IODE!






        PIB[1992Y1] = 2.6931472

        Hello World






        PIB3[1992Y1] = 2.9092974




            PIB_A[1992Y1] = 2


            PIB_B[1992Y1] = 0.69314718


            PIB_C[1992Y1] = 0.90929743


            PIB[1992Y1] = 2.6931472


            PIB3[1992Y1] = 2.9092974



    .par1 tit_1 
    Equations with coefficients



    .par1 enum_1

    \bACAG\B  :=  \bACAG\B[-1 ] + r \iVBBP\I[-1 ] + (0.006*\iVBBP\I[-1 ]*(\iTIME\I= 2001) -0.008*(\iTIME\I= 2008) )


    .par1 enum_1

    (\bACAF\B/\iVAF\I[-1 ])   := \i0.015768407\I + \i-7.961502e-06\I*\iGOSF\I[-1 ] +
    \i-0.0085051842\I*(\iTIME\I= 1995)

&TX

>
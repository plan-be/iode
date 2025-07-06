/**
 *  @header4iode
 * 
 *  IODE report internal commands 
 *  -----------------------------
 *  Functions  related to the control flow of the reports ($msg, $return, $goto...).
 *  They are not directly acting on IODE objects or files. 
 *  
 *  They are grouped in the table B_fns[] and called by RP_exec_fn().
 *  Each element of B_fns[] contains for the command $goto:
 *      - the function name (eg. "goto")
 *      - the function pointer in *non GUI* mode (RP_goto)
 *      - the function pointer in *GUI* mode (NULL because if there is no "GUI" version of $goto)
 *      - the suffix allowed or required after the function name
 *  
 *  The syntax of each function can be found in the online manual:
 *       https://iode.plan.be/doku.php?id=les_commandes_d_execution_des_rapports.
 *      
 *  All functions have the same syntax:
 *  
 *      int <function>(char* arg)
 *  
 *      @param [in] char*   arg     parameters of the function
 *      @return     int             return code: 0 on success
 *      
 *  Note that RP_exec_fn() calls the function with a second parameter "type" which, being
 *  not needed by these functions, is ignored.
 *    
 *  List of functions
 *  -----------------
 *      int RP_vseps(char* seps)                         $vseps <seps>
 *      int RP_repeatstring(char* buf)                   $repeatstring <string>
 *      int RP_repeat(char* buf)                         $repeat <command>
 *      int RP_onerror_1(char* arg)                      Sub function of RP_onerror() which assigns the 2 global variables RP_RT and RP_PRINT.
 *      int RP_onerror(char* arg)                        $onerror [{return|abort|quitode}] [{display|print|noprint}]
 *      int RP_abort(char* arg)                          $abort 
 *      int RP_quitode(char* arg)                        $quitode or $quit
 *      int RP_return(char* arg)                         $return
 *      int RP_label(char* arg)                          $label <label>
 *      int RP_goto_label(char *command, char *parm)     Search in the current REPFILE (CUR_REPFILE) the line beginning with "$command parm" or "#command parm", where command can be "label" or "next".
 *      int RP_goto(char* arg)                           $goto label [value]
 *      int RP_message(char* arg)                        $show message
 *      int RP_warning(char* arg)                        $msg text
 *      int RP_silent(char* arg)                         $silent {0|1|N|n|Y|y]
 *      int RP_beep()                                    $beep
 *      int RP_ask(char* arg)                            $ask <label> <question>
 *      int B_ReportPrompt(char* arg)                    #prompt <macro_name> <question>
 *      int RP_setdebug(char* arg)                       $debug {0|n|N|1|2|f|F}
 *      int RP_setindent(char* arg)                      $indent {0|n|N|1}
 *      int RP_setmultiline(char* arg)                   $multiline {0|n|N|1} 
 *      int RP_noparsingchar(char* arg)                  $noparsing [0|n|N|1]
 *      int RP_shift(char* arg)                          $shift [n] or $shift_args [n]
 *      int RP_chdir(char* arg)                          $chdir <dirname>
 *      int RP_rmdir(char* arg)                          $rmdir <dirname>
 *      int RP_mkdir(char* arg)                          $mkdir <dirname>
 *      int RP_settime(char* arg)                        $settime <period>
 *      int RP_incrtime(char* arg)                       $incrtime [n]
 *      int RP_system(char* arg)                         $system <command>
 *      int B_shellexec(char *arg)                       $shellexec <command>
 *      int B_Sleep(char* arg)                           $sleep <msecs>
 *      int B_GraphDefault(char* type)                   $graphdefault {l|L|s|S|b|B} 
 *  
 */
#include "scr4/scr4.h"

#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/k_super.h"

#include "api/report/engine/engine.h"

#ifdef _MSC_VER
    #include <direct.h>
#endif 

char *RP_RPTSTR = 0;    // Repeat string used by the command $repeat. Default = '_'.
char *RP_VSEPS = 0;     //Vector of separators used by RPF_vtake(), RPF_vdrop() and RPF_vcount()

// $vseps <seps> 
// @see https://iode.plan.be/doku.php?id=vseps

int RP_vseps(char* seps)  
{
    if(seps == 0 || strlen(seps) == 0) seps = ";, ";
    SCR_free(RP_VSEPS);
    RP_VSEPS = SCR_stracpy(seps);
    return(0);
}


// $repeatstring <string>
int RP_repeatstring(char* buf)   /* JMP 06-06-99 */
{
    SCR_sqz(buf);
    if(strlen(buf) == 0) buf = "_";
    SCR_free(RP_RPTSTR);
    RP_RPTSTR = SCR_stracpy(buf);
    return(0);
}

// $repeat <command>
// Nouvelle version améliorée pour les allocs JAN 2009
int RP_repeat(char* buf)
{
    U_ch    *line = 0, *cmd;
    U_ch    **args;
    int     rc, i, pos1, pos2, maxlg = 0, lg;

    rc = RP_expand(&line, buf);
    if(rc) return(rc);

    pos1 = U_pos('"', line); // Position du premier " dans line
    if(pos1 < 0) {
        SCR_free(line);
        return(-1);
    }
    pos2 = pos1 + 1 + U_pos('"', line + pos1 + 1); // Position du second " dans line
    line[pos2] = 0; // line + pos1  devient la commande ($repeat "$DatadeleteVar _ _" donne dans line + pos 1 = [$DataDeleteVar _ _] par exemple)
    args = B_ainit_chk(line + pos2 + 1, NULL, 0); // Arguments sur lesquels il faut boucler

    if(args == NULL) {
        SCR_free(line);
        return(-1);
    }

    if(RP_RPTSTR == 0) RP_repeatstring("_");

    // Calcule la longueur max d'un arg et alloue une commande assez longue
    for(maxlg = i = 0 ; args[i] ; i++) {
        lg = (int)strlen(args[i]);
        if(lg > maxlg) maxlg = lg;
    }
    //cmd = SCR_malloc(strlen(line + pos1 + 1) + 10 * maxlg + 64);
    cmd = RP_alloc((int)strlen(line + pos1 + 1) + 10 * maxlg + 64);

    // Démarre la boucle
    for(i = 0 ; args[i] ; i++) {
        strcpy(cmd, line + pos1 + 1); // Commande
        SCR_replace(cmd, RP_RPTSTR, args[i]);
        rc = B_ReportLine(cmd, 1);
        if(rc) break;
    }
    //SCR_free(cmd);
    RP_free(cmd);
    SCR_free(line); //GB 20/08/2012 solved memory leak
    SCR_free_tbl(args);
    return(0);
}

/** Sub function of RP_onerror() which assigns the 2 global variables RP_RT and RP_PRINT.
 *  See b_rep_engine.c for their definitions.
 *  - RP_RT indicates what to do on error (ignore, return...)
 *  - RP_PRINT indicates how to use error messages (print, display...)
 */
int RP_onerror_1(char* arg)
{
    SCR_upper(arg);

    if(strcmp(arg, "IGNORE")  == 0) RP_RT = 0;
    else    if(strcmp(arg, "RETURN")  == 0) RP_RT = -2;
    else    if(strcmp(arg, "ABORT")   == 0) RP_RT = -3;
    else    if(strcmp(arg, "QUITODE") == 0) RP_RT = -4;
    else    if(strcmp(arg, "DISPLAY") == 0) RP_PRINT = 2;
    else    if(strcmp(arg, "PRINT")   == 0) RP_PRINT = 1;
    else    if(strcmp(arg, "NOPRINT") == 0) RP_PRINT = 0;
    return(0);
}

// $onerror [{return|abort|quitode}] [{display|print|noprint}]
int RP_onerror(char* arg)
{
    return(B_ainit_loop(arg, RP_onerror_1, NULL));
}

// $abort 
// Quits the current report execution stack.
int RP_abort(char* arg)
{
    return(-3);
}

// $quitode or $quit
// Exits the program. 
//
// Note that "ODE" is the original name of IODE. 
// ODE stands for "Outils de Développements Econométriques"
// IODE for "Intégrateur d'Outils de Développements Econométriques".

int RP_quitode(char* arg)
{
    SCR_APP_ABORT = ACT_APPL_QUIT;
    //K_end_ws(0);
    ODE_end(1);
    exit(0);
    /*    return(-4); */
}

// $return
// Quits the current report and goes up one level in the report stack (depth decreased by 1).
int RP_return(char* arg)
{
    return(-2);
}


// $label <label>
// Note that there is no action associated with $label: it is only a marker used by $goto.
int RP_label(char* arg)
{
    return(0);
}


/**
 *  Search in the current REPFILE (CUR_REPFILE) the line beginning with 
 *  "$command parm" or "#command parm", where command can be "label" or "next".
 *  
 *  If command == "label", the search starts at line 0 of CUR_REPFILE to allow
 *  the label to be placed before $goto.
 *  If command == "next", the search starts at the current line in CUR_REPFILE.
 *    
 *  @param [in] char*   command     command to search (label or next)
 *  @param [in] char*   parm        label to search
 *  @return 
 */
int RP_goto_label(char *command, char *parm)
{
    int     rc, beg = 1;
    char    *line = NULL, name[128], *arg;

    
    if(strcmp(command, "label") == 0)  // JMP 14/2/2013
        CUR_REPFILE->curline = 0;
    SCR_sqz(parm);
    SCR_lower(parm);

    while(1) {
        SW_nfree(line);
        line = 0;
        rc = RP_readline(CUR_REPFILE, &line, 0);
        if(rc == EOF) {
            B_seterror("%s %.80s not found", command, parm);
            rc = -3;
            goto done;
        }

        //if((line[0] == '#' || line[0] == '$') && line[1] != line[0]) {
        //    if(line[1] == 0 || U_is_in(line[1], " \n\r\t")) continue;
        if(!RP_is_cmd(line)) continue;
        if(RP_splitline(line + 1, name, &arg, 30) < 0) {
            rc = -1;
            goto done;
        }

        SCR_lower(name);
        SCR_lower(arg);
        if(strcmp(name, command) == 0 && strcmp(parm, arg) == 0) {
            rc = 0;
            goto done;
        }
    }

done:
    SW_nfree(line);
    line = 0;
    return(rc);
}


// $goto label [value]
// See RP_goto_label().
int RP_goto(char* arg)
{
    unsigned    char    **args, **SCR_vtomsq();
    int                 rc = 0, nb;

    args = SCR_vtomsq(arg, B_SEPS, '"');
    nb = SCR_tbl_size(args);
    switch(nb) {
        case 0 : // JMP 13-12-12: erreur si pas d'arg uniquement
            rc = -3;
            break;

        case 1 :
            rc = RP_goto_label("label", args[0]);
            break;

        default: // JMP 13-12-12: Ok si plus de 2 args
            rc = atoi(args[1]);
            switch(rc) {
                case  0 :
                case -1 :
                    break;
                //case  1 : // JMP 12-12-12 !!
                default :   // JMP 12-12-12 !!
                    rc = RP_goto_label("label", args[0]);
                    break;
            }
            break;

    }

    SCR_free_tbl(args);
    return(rc);
}


// Messages & questions
// ====================

// $show message
int RP_message(char* arg)
{
    char    fmt[2048];

    if(arg == NULL || arg[0] == 0) return(0);

    //kmsg("%.80s", arg);

    sprintf(fmt, "%%.%ds", SCR_PAGE_SIZE[1]-20);   /* JMP 17-03-11 */
    kmsg(fmt, arg); // JMP 4/02/09                 /* JMP 17-03-11 */
    /*    SCR_beep();    /* JMP 17-12-93 */
    return(0);
}


// $msg text
int RP_warning(char* arg)            
{
    if(arg == NULL || arg[0] == 0) return(0); /* JMP 11-07-96 */

    if(strlen(arg) > 512) arg[512] = 0;
    kwarning("%s", arg); 
    return(0);
}

// $silent {0|1|n|N|Y|y}
int RP_silent(char* arg)   
{  
     int     ch;
    
    ch = SCR_upper_char(arg[0]);
    switch(ch) {
        case '0' :
        case 'n':
        case 'N':
            kmsg_toggle(1);
            break;

        default :
            kmsg_toggle(0);
            break;
    }
    return(0);
}

// $beep
int RP_beep(char* arg)
{
    kbeep();
    return(0);
}

// $ask <label> <question>
int RP_ask(char* arg)
{
    int     lg;
    U_ch    name[31];

    lg = B_get_arg0(name, arg, 30);
    //if(SCR_confirme(arg + lg + 1) != 0) return(0); /* rep. NON -> ligne suivante */
    if(kconfirm(arg + lg + 1) != 0) return(0);       /* rep. NON -> ligne suivante */  // JMP 10/12/2021
    /* rep OUI -> va en label */
    if(RP_goto_label("label", name) != 0) return(-3);
    return(0);
}

// #prompt <macro_name> <question>
// See https://iode.plan.be/doku.php?id=prompt
// Warning: $prompt does not exist. Only #prompt is valid (in GUI mode)
// TODO: implement $prompt 
int B_ReportPrompt(char* arg)
{
    return(RP_define(arg));
}

// Report language parameters
// ==========================

// $debug {0|n|N|1|2|f|F}
int RP_setdebug(char* arg)   
{  
    int     ch;
    
    ch = SCR_upper_char(arg[0]);
    switch(ch) {
        case '0' :
        case 'n':
        case 'N':
            RP_DEBUG = 0;
            break;
        case '2' :
        case 'F' :
        case 'f' :
            RP_DEBUG = 2;
            break;

        default :
            RP_DEBUG = 1;
            break;
    }
    return(0);
}

// $indent {0|n|N|1}
int RP_setindent(char* arg) 
{
    int     ch;
    
    ch = SCR_upper_char(arg[0]);
    switch(ch) {
        case '0' :
        case 'n':
        case 'N':
            RP_ALLOW_INDENT = 0;
            break;

        default :
            RP_ALLOW_INDENT = 1;
            break;
    }
    return(0);
}

// $multiline {0|n|N|1} 
int RP_setmultiline(char* arg) 
{
    int     ch;
    
    ch = SCR_upper_char(arg[0]);
    switch(ch) {
        case '0' :
        case 'n':
        case 'N':
            RP_ALLOW_MULTILINE = 0;
            break;

        default :
            RP_ALLOW_MULTILINE = 1;
            break;
    }
    return(0);
}

// $noparsing [0|n|N|1]
int RP_noparsing(char* arg) 
{
    int     ch;
    
    ch = SCR_upper_char(arg[0]);
    switch(ch) {
        case '0' :
        case 'n':
        case 'N':
            RP_ALLOW_NOPARSING = 0;
            break;

        default :
            RP_ALLOW_NOPARSING = 1;
            break;
    }

    return(0);
}

// Report argument management
// ==========================

// $shift [n]
// $shift_args [n]
// Pops or Pushes current report arguments
int RP_shift_args(char* arg)
{
    int     incr = 1;

    SCR_strip(arg);
    if(arg[0]) incr = atoi(arg);

    RP_ARG0 += incr;
    return(0);
}


// Directory management
// ====================

// $chdir <dirname>
int RP_chdir(char* arg)
{
    int rc;

#ifdef _MSC_VER
    rc = _chdir(arg); 
#else
    rc = chdir(arg); 
#endif

    //ODE_settitle();
    ksettitle();     // JMP 10/12/2021
    return(rc);
}

// $rmdir <dirname>
int RP_rmdir(char* arg)
{
    return(_rmdir(arg));
}

// $mkdir <dirname>
int RP_mkdir(char* arg)
{
    return(_mkdir(arg));
}


// Current value of t in {report expressions}
// ==========================================
// $settime <period>
int RP_settime(char* arg)
{
    PERIOD *rp_per;

    rp_per = PER_atoper(arg);
    if(rp_per == NULL) {
        B_seterror("SetTime %.80s: invalid syntax", arg);
        return(-1);
    }

    memcpy(&RP_PER, rp_per, sizeof(PERIOD));
    SCR_free(rp_per); // JMP 15/2/2013
    return(0);
}

// $incrtime [n]
int RP_incrtime(char* arg)
{
    int     incr = 1;
    PERIOD  *per;

    SCR_strip(arg);
    if(arg[0]) incr = atoi(arg);

    per = PER_addper(&RP_PER, incr);
    memcpy(&RP_PER, per, sizeof(PERIOD));
    return(0);
}


// Other functions
// ===============

// $system <command>
int RP_system(char* arg)
{
    return(kexecsystem(arg)); // JMP 13/12/2021
}

// $shellexec <command>
int B_shellexec(char *arg)
{
    return(kshellexec(arg));
}

// $sleep <msecs>
int B_Sleep(char* arg)
{
    int         ms = atoi(arg);
    extern int  SCR_sleep(int);

    SCR_sleep(ms);
    return(0);
}

// $graphdefault {l|L|s|S|b|B} 
// (Moved from k_tbl.c)
//
// Defines the default table graph default (Line, Bar...). 
// Used when creating a new line in a table.
int B_GraphDefault(char* type) 
{
    extern int T_GRAPHDEFAULT;
    
    switch(type[0]) {
        case 'l' :
        case 'L' :
            T_GRAPHDEFAULT = 0;
            break;
        case 's' :
        case 'S' :
            T_GRAPHDEFAULT = 1;
            break;
        case 'b' :
        case 'B' :
            T_GRAPHDEFAULT = 2;
            break;
        default  :
            B_seterrn(304);
            return(-1);
    }
    return(0);
}
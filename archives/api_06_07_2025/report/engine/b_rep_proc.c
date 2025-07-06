/**
 *  @header4iode
 * 
 *  IODE report functions $procdef, $procend and $procexec implementation
 *  ---------------------------------------------------------------------
 *  
 *  Syntax of report procedures ($proc*)
 *  
 *      $proc[def] procname [parm1] ... [parms]
 *       ...
 *      $procend
 *       ...
 *      $procexec procname parms ..
 *  
 *  Parameter usage and management
 *  
 *   - all parameters but the last are words or strings enclosed in ""
 *   - the parameters in excess (if any) are assigned to the last formal parameter
 *   - parameters are used in the body of the procedure like $defines : %parm1%, ...
 *   - if a $define with the same name as a procedure parameter exists before executing the procedure, 
 *      it is locally replaced by the actual value of the parameter, but retrieves its value at the end 
 *      of the procedure execution
 *   - the $proc statement is closed by the first $procend (in the same file)
 *   - a procedure can be redefined
 *   - procedures are kept in memory after the end of the report execution
 *   - procedures are called by the command $procexec
 *   - a procedure must be defined before its first use
 *  
 *  Example
 *  -------
 *  Note that when there is a whitespace between $ or ## and the command, it is considered as a comment.
 *  
 *  # Definition of the procedure TotalBySex
 *  
 *     $procdef TotalBySex resprefix agelist arrlist
 *          $foreach sex F;H
 *              @chronoreset()
 *              $ show %sex% - %agelist% - %arrlist%
 *              $define POP 0
 *              $foreach pos P01;P02;P03;P04;P05;P06;P07;P08;P09;P10;P11;P12;P13;P14;P15
 *                  $foreach age @lvalue(%agelist%)
 *                      $foreach arr @lvalue(%arrlist%)
 *                          $define POP %POP% + %pos%_%age%_%sex%_%arr%
 *                      $next arr
 *                  $next age
 *              $next pos
 *              $DataUpdateidt %resprefix%_%sex% %POP%
 *              $IdtExecute 1991Y1 2010Y1 %resprefix%_%sex%
 *              $ show CPU : @chronoget() for %sex% - %agelist% - %arrlist%
 *          $next sex
 *      $procend
 *  
 *  # Call to the procedure TotalBySex
 *      $foreach agegroup 7074 7579 8084 8589 90plus
 *          $procexec TotalBySex POP%agegroup%_WAL L%agegroup% WAL
 *          $procexec TotalBySex POP%agegroup%_VLA L%agegroup% VLA
 *          $ procexec TotalBySex POP%agegroup%_ARR L%agegroup% ARR
 *          $procexec TotalBySex POP%agegroup%_BRU L%agegroup% BRU
 *      $next agegroup
 *      
 *  List of functions
 *  -----------------
 *      static int RP_proc_find(char *name)         Retrieves the position of a REP_PROC in REP_PROCS by its name (case sensitive!)
 *      static int RP_proc_is_procend(char *line)   Checks if a line closes a PROC (i.e.: contains "$procend").
 *      static void RP_proc_delete(int proc_nb)     Deletes a REP_PROC object and frees its reference in the table REP_PROCS.
 *      static int RP_proc_create(char *name)       Adds a new empty PROC in REP_PROCS.
 *      void RP_proc_free_all()                     Frees all the defined procedures and the table REP_PROCS.
 *      int RP_procdef(char* arg)                   Reads and creates a new PROC.
 *      int RP_procexec(char* arg)                  Executes a procedure (called by $procexec parms).
 *      
 *  List of global variables
 *  ------------------------
 *      REP_PROC    **REP_PROCS       Table of pointers to existing PROCs 
 *      int         REP_NB_PROCS      Size of REP_PROCS
 */
#include "api/b_errors.h"

#include "api/report/engine/engine.h"


// REP_PROC: procedure parameters and code
typedef struct _rp_proc_ {
    char    proc_name[80];      // PROC name (case sensitive)
    char    **proc_parms;       // List of parameter names
    int     proc_nb_parms;      // Number of parameters
    REPFILE *proc_rf;           // Procedure lines (= sub-report) including $procdef and $procend
} REP_PROC;

// Global variables
static REP_PROC    **REP_PROCS;            // Table of pointers to existing PROCs (including deleted ones as NULL pointers)
static int         REP_NB_PROCS = 0;       // Size of REP_PROCS

/**
 *  Retrieves the position of a REP_PROC in REP_PROCS by its name (case sensitive!).
 *  
 *  @param [in] char* name  PROC name (case sensitive) to search
 *  @return     int         position of name in REP_PROCS or -1 if not found
 */
static int RP_proc_find(char *name)
{
    int     i;

    SCR_sqz(name);
    for(i = 0 ; i < REP_NB_PROCS ; i++)
        if(REP_PROCS[i] && strcmp(REP_PROCS[i]->proc_name, name) == 0) return(i);

    return(-1);
}


/**
 *  Checks if a line closes a PROC (i.e.: contains "$procend").
 *  
 *  @param [in] char* line  line to check
 *  @return     int         1 if it is a $procend statement
 *                          0 if not
 */
static int RP_proc_is_procend(char *line)
{
    char    buf[RP_LINELENGTH + 1];

    strcpy(buf, line);  // JMP 23/5/2019
    //U_rjust_text(buf);  // JMP 23/5/2019  05/11/2022
    //U_ljust_text(buf);  // JMP 23/5/2019  05/11/2022
    SCR_sqz(buf);       // JMP 05/11/2022
    SCR_lower(buf);     // JMP 23/5/2019
    if(strcmp(buf, "$procend") == 0) return(1);
    return(0);
}


/**
 *  Deletes a REP_PROC object and frees its reference in the table REP_PROCS.
 *  
 *  @param [in] int     proc_nb     procedure position in REP_PROCS.
 */
static void RP_proc_delete(int proc_nb)
{
    REP_PROC *proc;

    if(proc_nb < 0) return;
    proc = REP_PROCS[proc_nb];
    if(proc == 0) return;
    SCR_free_tbl(proc->proc_parms);
    RP_free_repfile(proc->proc_rf);
    //SCR_free(proc->proc_filename);
    SCR_free(proc);
    REP_PROCS[proc_nb] = 0;
}


/**
 *  Adds a new empty PROC in REP_PROCS.
 *  If a proc with the same name exists, deletes first the current one.
 *  Increase the size of REP_PROCS if necessary.
 *  
 *  @param [in] char*   name    name of the new procedure
 *  @return     int             position of the new PROC in REP_PROCS
 */
static int RP_proc_create(char *name)
{
    int         proc_nb;
    REP_PROC    *proc;

    proc_nb = RP_proc_find(name);
    if(proc_nb >= 0) RP_proc_delete(proc_nb);

    for(proc_nb = 0 ; proc_nb < REP_NB_PROCS ; proc_nb++)
        if(REP_PROCS[proc_nb] == 0) break;

    if(proc_nb == REP_NB_PROCS) {
        REP_PROCS = (REP_PROC **) SCR_realloc((REP_PROC **)REP_PROCS, sizeof(REP_PROC *), REP_NB_PROCS, REP_NB_PROCS + 10);
        REP_NB_PROCS += 10;
    }

    proc = REP_PROCS[proc_nb] = (REP_PROC *) SCR_malloc(sizeof(REP_PROC));
    SCR_strlcpy(proc->proc_name, name, sizeof(proc->proc_name) - 1);
    return(proc_nb);
}


/**
 *  Free all the defined procedures and the table REP_PROCS.
 */
void RP_proc_free_all()
{
    int         i;

    for(i = 0; i < REP_NB_PROCS; i++)
        RP_proc_delete(i);
    SCR_free(REP_PROCS);
    REP_NB_PROCS = 0;  // JMP 5/11/2022
    REP_PROCS = 0;     // JMP 5/11/2022
}


/**
 *  Reads and creates a new PROC.
 *  
 *  @param [in] char*   arg     line starting with "$procdef" 
 *  @return     int             0 if a PROC is created, -1 on error
 */
int RP_procdef(char* arg)
{
    //REPFILE			*RP_create_repfile(char *, unsigned char **);
    REP_PROC        *proc;
    REPFILE         *rf = CUR_REPFILE;
    unsigned char   name[128], buf[1024], *list, **lines; // JMP 23/5/2019
    int             proc_nb, nblines = 0, line1, i;

    if(RP_splitline(arg, name, &list, 30) < 0) return(-1);

    // Creates the REP_PROC object
    // if a proc with the same name already exists, first deletes the existing one
    proc_nb = RP_proc_create(name);
    proc = REP_PROCS[proc_nb];

    // Compute the formal parameters (sep = space)
    proc->proc_parms = SCR_vtoms(list, " ");
    proc->proc_nb_parms = SCR_tbl_size(proc->proc_parms);

    // Save the current position in CUR_REPFILE
    line1 =  rf->curline;

    // Reads CUR_REPFILE to the next $procend line (or EOF)
    for(nblines = 0; rf->curline < rf->nblines ; rf->curline++, nblines++) {
        if(RP_proc_is_procend(rf->tbl[rf->curline])) {                      // JMP 23/5/2019
            rf->curline++; // To go to the next line in the report execution (if any)
            
            // Creates a REPFILE in memory containing the PROC code 
            // REFILE filename is constructed as >>> proc <name> (in <file>[<line>])
            lines = (unsigned char **) SCR_malloc((nblines + 1) * sizeof(char *));
            for(i = 0 ; i < nblines ; i++)
                lines[i] = SCR_stracpy(rf->tbl[line1 + i]);
            sprintf(buf, ">>> proc %s (in %s[%d]) + ", name, rf->filename, line1);
            proc->proc_rf = RP_create_repfile(buf, lines); 
            break;
        }
    }

    // Checks that a $procend statement has  been found (if not -> error)
    if(rf->curline > rf->nblines)  {
        B_seterror("Procedure %s: no $procend statement", name);
        //TODO: delete the REP_PROCS[proc_nb]
        return(-1);
    }

    return(0);
}


/**
 *  Execution of a procedure (called by $procexec parms).
 *  
 *  The procedure must exist BEFORE the call (but can have been defined in
 *  a previous report execution).
 *   
 *  The actual parameters are placed in macros (= $define) with the names of the correpsonding 
 *  formal parameters and the PROC as called like a normal report (REPFILE struct).
 *      
 *  @param [in] char*   arg     line beginning with "$procexec"
 *  @return     int             -1 if the PROC does not exist
 */
int RP_procexec(char* arg)
{
    unsigned char   name[128], *list, **aparms, *lastparms;
    int             rc = 0, i, nformal, nactual, proc_nb;
    REP_PROC        *proc;

    if(RP_splitline(arg, name, &list, 30) < 0) return(-1);

    // Searches the proc definition
    proc_nb = RP_proc_find(name);
    if(proc_nb < 0) {
        B_seterror("Procedure %s: not defined", name);
        return(-1);
    }
    proc = REP_PROCS[proc_nb];

    // Saves (temporarily pushes) the defines with the same names as the formal parameters
    rc = RP_define_save_list(proc->proc_parms);
    if(rc < 0) return(rc);

    // Creates macros with the actual parameters
    aparms = SCR_vtomsq(list, B_SEPS, '"');
    //aparms = SCR_vtoms(list, " ");
    nactual = SCR_tbl_size(aparms);
    nformal = proc->proc_nb_parms;

    // Creates macros (=defines) for each actual parameter.
    // If there are less actual parms than formal parms, creates only $defines for the actual ones
    if(nformal >= nactual) {
        for(i = 0 ; i < nactual; i++) {
            rc = RP_define_1(proc->proc_parms[i], aparms[i]);
            if(rc < 0) return(rc);
        }
    }
    // If there are more actual parms than formal parms, the last $define (formal parm)
    //  is created with the list of all remaining actual parms separated by a space
    else {
        for(i = 0 ; i < nformal - 1; i++) {
            rc = RP_define_1(proc->proc_parms[i], aparms[i]);
            if(rc < 0) return(rc);
        }
        lastparms = SCR_mtov(aparms + nformal - 1, ' ');
        rc = RP_define_1(proc->proc_parms[nformal - 1], lastparms);
        SCR_free(lastparms);
        if(rc < 0) return(rc);
    }

    SCR_free_tbl(aparms);

    // Executes the proc lines like a normal report
    proc->proc_rf->curline = 0;
    rc = RP_ReportExec_tbl(proc->proc_rf);

    // Restores (pops) the defines with the same name as the formal parameters
    RP_define_restore_list(proc->proc_parms);

    return(rc);
}

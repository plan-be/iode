#include "iode.h"

/*---------------------------------------------------------------------------
 Report procedures ($proc*)
 -----------------

 Syntax : 
 $proc[def] procname [parm1] ... [parms]
  ...
 $procend
  ...
 $procexec procname parms ..

 Parameters : 
  - all parameters but the last are "words" or string enclosed in "" 
  - the last parameter is a list of all remaining parameters (if any)
  - parameters are used in the body of the procedure like $define : %parm1%, ...
  - if a $define with the same name as a procedure parameter exists before executing the procedure, it 
    is locally inaccessible, but retrieved its value at the end of the procedure execution
  - the $proc statement is closed by the first $endproc (in the same file)
  - a procedure can be redefined by a new definition
  - procedures are kept in memory after the end of the report execution
  - procedures are called by the command $procexec
  - a procedure must be defined before its first use
------------------------------------------------------------------------------ */

// Objet REP_PROC : contient le texte d'une procédure
typedef struct _rp_proc_ {
    char    proc_name[80];      // Nom de la procédure
    char    **proc_parms;       // Nom des paramètres
    int     proc_nb_parms;      // Nombre de paramètres

    //char    *proc_filename;     // Fichier source
    //int     proc_line1;         // Ligne de départ de la définition dans le fichier
                    
    REPFILE *proc_rf;           // Nombre de lignes de la procédure (avec $procdef et $procend)
} REP_PROC;

REP_PROC    **REP_PROCS;
int         REP_NB_PROCS = 0;

RP_proc_find(char *name)
{
    int     i;

    SCR_sqz(name);
    for(i = 0 ; i < REP_NB_PROCS ; i++)
        if(REP_PROCS[i] && strcmp(REP_PROCS[i]->proc_name, name) == 0) return(i);
        
    return(-1);   
}

RP_proc_is_procend(char *line)
{
    char    buf[LINELENGTH + 1];
    
    strcpy(buf, line);  // JMP 23/5/2019
    U_rjust_text(buf);  // JMP 23/5/2019
    U_ljust_text(buf);  // JMP 23/5/2019
    SCR_lower(buf);     // JMP 23/5/2019
    if(strcmp(buf, "$procend") == 0) return(1);
    return(0);    
}
    
RP_proc_delete(int proc_nb)
{
    REP_PROC *proc;
    
    if(proc_nb < 0) return;
    proc = REP_PROCS[proc_nb];
    if(proc == 0) return;
    SCR_free_tbl(proc->proc_parms);
    B_free_repfile(proc->proc_rf);
    //SCR_free(proc->proc_filename);
    SCR_free(proc);
    REP_PROCS[proc_nb] = 0;
}

/*************************************************************************
Creates the REP_PROC object named name.
If a proc with the same name exists, deletes first the current one.
**************************************************************************/
RP_proc_create(char *name)
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

// Free all the defined procedures 
RP_proc_free_all()
{
    int         i;
    
    for(i = 0; i < REP_NB_PROCS; i++)
        RP_proc_delete(i);
    SCR_free(REP_PROCS);
}


/*********************************************************************************
Lecture d'une procédure :  
return : -1 if error ($procend not found)
         0 if ok   
**********************************************************************************/

RP_procdef(arg)
char    *arg;
{
	REPFILE			*B_create_repfile(char *, unsigned char **);
    REP_PROC        *proc;
    REPFILE         *rf = CUR_REPFILE;
    unsigned char   name[128], buf[1024], *list, **parms, **lines; // JMP 23/5/2019
    int             proc_nb, nblines = 0, line1, i;

    if(RP_splitline(arg, name, &list, 30) < 0) return(-1);
    
    // Create the REP_PROC object 
    // if a proc with the same name already exist, delete first the current one
    proc_nb = RP_proc_create(name);
    proc = REP_PROCS[proc_nb];
    
    // Compute the formal parameters (sep = space)
    proc->proc_parms = SCR_vtoms(list, " "); 
    proc->proc_nb_parms = SCR_tbl_size(proc->proc_parms);
    
    // Save the current filename and position in file
    line1 =  rf->curline;
    
    // Lecture dans le fichier en mémoire jusqu'à la ligne $procend ou la fin du fichier
    for(nblines = 0; rf->curline < rf->nblines ; rf->curline++, nblines++) {
        if(RP_proc_is_procend(rf->tbl[rf->curline])) {          // JMP 23/5/2019
            rf->curline++; // To go to the next line in the report execution (if any)
            lines = SCR_malloc((nblines + 1) * sizeof(char *));
            for(i = 0 ; i < nblines ; i++) 
                lines[i] = SCR_stracpy(rf->tbl[line1 + i]);
            sprintf(buf, ">>> proc %s (in %s[%d]) + ", name, rf->filename, line1);
            proc->proc_rf = B_create_repfile(buf, lines);
            break;
        }    
    }
        
    // Checks that a $procend statement has  been found (if not -> error)
    if(rf->curline > rf->nblines)  {
        B_seterror("Procedure %s: no $procend statement", name);
        return(-1);
    }
    
    return(0);
}

/*********************************************************************************
Execution of a procedure (called by $procexec parms)
The procedure must exist BEFORE the call (but can have been defined in 
a previous report execution.
**********************************************************************************/

RP_procexec(arg)
char    *arg;
{
    unsigned char   name[128], buf[512], *list, **aparms, *lastparms;
    int             rc = 0, i, nformal, nactual, proc_nb;
    REP_PROC        *proc;

    if(RP_splitline(arg, name, &list, 30) < 0) return(-1);
        
    // Search the proc definition
    proc_nb = RP_proc_find(name);
    if(proc_nb < 0) {
        B_seterror("Procedure %s: not defined", name);
        return(-1);
    }
    proc = REP_PROCS[proc_nb];
    
    // Save the defines with the same name as the formal parameters
    rc = RP_define_save_list(proc->proc_parms);
    if(rc < 0) return(rc);

    // Create macros with the actual parameters
    aparms = SCR_vtomsq(list, B_SEPS, '"');
    //aparms = SCR_vtoms(list, " ");
    nactual = SCR_tbl_size(aparms);
    nformal = proc->proc_nb_parms;
    
    // if less actual parms than formal parms, create only the first macro
    if(nformal >= nactual) {
        for(i = 0 ; i < nactual; i++) {
            rc = RP_define_1(proc->proc_parms[i], aparms[i]);
            if(rc < 0) return(rc);
        }
    }
    // else (more actual parms than formal parms), the last formal parms 
    //      contains a list of all actual remaining parms sep by space
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
    
    // Execute the proc lines like a normal report
    proc->proc_rf->curline = 0;
    rc = B_ReportExec_tbl(proc->proc_rf);
        
    // Restore the defines with the same name as the formal parameters
    RP_define_restore_list(proc->proc_parms);
    
    return(rc);
}

/**
 *  @header4iode
 * 
 *  Report functions to execute identities and specify the execution parameters (see also b_rep_syntax.c).
 *  
 *  List of Functions
 *  ------------------
 *      int B_IdtExecute(char* arg)             $IdtExecute period_from period_to [idt1 idt2...]
 *      int B_IdtExecuteVarFiles(char* arg)     $IdtExecuteVarFiles file1 [file2 ...]
 *      int B_IdtExecuteSclFiles(char* arg)     $IdtExecuteSclFiles file1 [file2 ...]
 *      int B_IdtExecuteTrace(char* arg)        $IdtExecuteTrace {Yes | No}
 */
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/k_exec.h"
#include "api/objs/identities.h"
#include "api/objs/variables.h"

#include "api/report/commands/commands.h"


// List of functions
int B_IdtExecute(char* arg);
int B_IdtExecuteIdts(SAMPLE* smpl, char** idts);
int B_IdtExecuteVarFiles(char* arg);
int B_IdtExecuteSclFiles(char* arg);
int B_IdtExecuteTrace(char* arg);


/**
 *  Report function to execute identities. Used in conjunction with report functions
 *      B_IdtExecuteVarFiles(char* arg)
 *      B_IdtExecuteSclFiles(char* arg)
 *      B_IdtExecuteTrace(char* arg)
 *  
 *  Syntax: $IdtExecute period_from period_to [idt1 idt2...]
 *  
 *  @see https://iode.plan.be/doku.php?id=idtexecute
 *  
 *  At the end of the function, the VarFiles and SclFiles defined by calls to B_IdtExecuteVarFiles() and
 *  B_IdtExecuteSclFiles() are reset to NULL.
 *  
 *  @params See b_data.c for details on arg syntax
 *  
 *  @return int     0 on success, -1 on error (file not found, syntax error...)
 */

int B_IdtExecute(char* arg)
{
    int     lg1, lg2;
    int     rc = 0;
    char    from[16], to[16], **idts;
    SAMPLE  *smpl;

    lg1 = B_get_arg0(from, arg, 15);
    lg2 = B_get_arg0(to, arg + lg1, 15);
    smpl = PER_atosmpl(from, to);

    if(smpl == NULL) {
        B_seterror("IdtExecute: %s %s wrong sample", from, to);
        return(-1);
    }

    idts = B_ainit_chk(arg + lg1 + lg2, NULL, 0);

    rc = B_IdtExecuteIdts(smpl, idts);

    SCR_free_tbl(idts);
    SCR_free(smpl);
    return(rc);
}

/**
 *  Executes a group of identities on a defined sample. 
 *  
 *  The input VAR and SCL files are found in the globals KEXEC_VFILES and KEXEC_SFILES that
 *  are specified via the functions B_IdtExecuteVarFiles() and B_IdtExecuteSclFiles().
 *  
 *  At the end of the functions, KEXEC_VFILES and KEXEC_SFILES are reset to NULL.
 *  The resulting variables are copied into KV_WS.
 *  
 *  @see https://iode.plan.be/doku.php?id=idtexecute
 *  
 *  @param   [in]   SAMPLE* smpl    SAMPLE on which the identities must be calculated.
 *  @param   [in]   char**  idts    list of identity names or NULL to execute all the idts present in KI_WS.
 *  @return int     0 on success, -1 on error (file not found,)
 */

int B_IdtExecuteIdts(SAMPLE* smpl, char** idts)
{
    KDB     *tdbi, *tkdb;

    if(idts == NULL || SCR_tbl_size(idts) == 0)
        tkdb = KI_exec(KI_WS,
                       KV_WS, SCR_tbl_size(KEXEC_VFILES), KEXEC_VFILES,
                       KS_WS, SCR_tbl_size(KEXEC_SFILES), KEXEC_SFILES,
                       smpl);
    else {
        tdbi = K_refer(KI_WS, SCR_tbl_size(idts), idts);
        tkdb = KI_exec(tdbi,
                       KV_WS, SCR_tbl_size(KEXEC_VFILES), KEXEC_VFILES,
                       KS_WS, SCR_tbl_size(KEXEC_SFILES), KEXEC_SFILES,
                       smpl);
        K_free_kdb(tdbi);
    }

    SCR_free_tbl(KEXEC_VFILES);
    KEXEC_VFILES = NULL;
    SCR_free_tbl(KEXEC_SFILES);
    KEXEC_SFILES = NULL;

    if(tkdb == NULL) return(-1);
    KV_sample(tkdb, smpl);

    if(KV_WS != NULL) KV_merge_del(KV_WS, tkdb, 1);
    else KV_WS = tkdb;

    return(0);
}


/**
 *  Report function that defines the list of external VAR files where the next call to B_IdtExecute() 
 *  will search the VARs needed to compute the identities.
 *  
 *  Syntax: $IdtExecuteVarFiles file1 [file2 ...]
 *  
 *  @see https://iode.plan.be/doku.php?id=idtexecuteVarFiles
 *  
 *  @params See b_data.c for details on arg syntax
 *  
 *  @return int     0 on success, -1 on error 
 */

int B_IdtExecuteVarFiles(char* arg)
{
    SCR_free_tbl(KEXEC_VFILES);
    KEXEC_VFILES = B_ainit_chk(arg, NULL, 0);
    return(0);
}


/**
 *  Report function that defines the list of external SCL files where the next call to B_IdtExecute() 
 *  will search the scalars needed to compute the identities.
 *  
 *  Syntax: $IdtExecuteSclFiles file1 [file2 ...]
 *  
 *  @see https://iode.plan.be/doku.php?id=idtexecuteSclFiles
 *  
 *  @params See b_data.c for details on arg syntax
 *  
 *  @return int     0 on success, -1 on error 
 */

int B_IdtExecuteSclFiles(char* arg)
{
    SCR_free_tbl(KEXEC_SFILES);
    KEXEC_SFILES = B_ainit_chk(arg, NULL, 0);
    return(0);
}


/**
 *  The trace of the identity construction can be saved in the current A2M file defined by $PrintDest. 
 *  Useful information (for example, the file where a variable is read to construct an identity) will be listed 
 *  in the "Trace file".
 *  
 *  Syntax:  $IdtExecuteTrace {Yes | No}
 *  
 *  @see https://iode.plan.be/doku.php?id=idtexecuteTrace
 *  
 *  @params See b_data.c for details on arg syntax
 *  
 *  @return int     0 on success, -1 on error 
 */

int B_IdtExecuteTrace(char* arg)
{
    KEXEC_TRACE = (SCR_upper_char(arg[0]) == 'Y') ? 1 : 0;  /* JMP 19-12-97 */
    return(0);
}

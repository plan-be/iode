/**
 *  @header4iode
 * 
 *  Report functions to execute identities and specify the execution parameters (see also b_rep_syntax.c).
 *  
 *  List of Functions
 *  ------------------
 *      int B_IdtExecute(char* arg, int unused)             $IdtExecute period_from period_to [idt1 idt2...]
 *      int B_IdtExecuteVarFiles(char* arg, int unused)     $IdtExecuteVarFiles file1 [file2 ...]
 *      int B_IdtExecuteSclFiles(char* arg, int unused)     $IdtExecuteSclFiles file1 [file2 ...]
 *      int B_IdtExecuteTrace(char* arg, int unused)        $IdtExecuteTrace {Yes | No}
 */
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/k_exec.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"

#include "api/report/commands/commands.h"


/**
 *  Report function to execute identities. Used in conjunction with report functions
 *      B_IdtExecuteVarFiles(char* arg, int unused)
 *      B_IdtExecuteSclFiles(char* arg, int unused)
 *      B_IdtExecuteTrace(char* arg, int unused)
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

int B_IdtExecute(char* arg, int unused)
{
    int     lg1, lg2;
    int     rc = 0;
    char    from[16], to[16], **idts;
    Sample  *smpl;

    error_manager.clear();

    lg1 = B_get_arg0(from, arg, 15);
    lg2 = B_get_arg0(to, arg + lg1, 15);
    try
    {
        smpl = new Sample(std::string((char*) from), std::string((char*) to));
    }
    catch(const std::exception& e)
    {
        std::string error_msg = "IdtExecute: wrong sample\n" + std::string(e.what());
        error_manager.append_error(error_msg);
        return(-1);
    }

    idts = B_ainit_chk(arg + lg1 + lg2, NULL, 0);

    rc = B_IdtExecuteIdts(smpl, idts);

    SCR_free_tbl((unsigned char**) idts);
    delete smpl;
    smpl = nullptr;

    if(rc != 0)
        error_manager.display_last_error();
    return rc;
}

/**
 *  Executes a group of identities on a defined sample. 
 *  
 *  The input VAR and Scalar files are found in the globals KEXEC_VFILES and KEXEC_SFILES that
 *  are specified via the functions B_IdtExecuteVarFiles() and B_IdtExecuteSclFiles().
 *  
 *  At the end of the functions, KEXEC_VFILES and KEXEC_SFILES are reset to NULL.
 *  The resulting variables are copied into KV_WS.
 *  
 *  @see https://iode.plan.be/doku.php?id=idtexecute
 *  
 *  @param   [in]   Sample* smpl    Sample on which the identities must be calculated.
 *  @param   [in]   char**  idts    list of identity names or NULL to execute all the idts present in KI_WS.
 *  @return int     0 on success, -1 on error (file not found,)
 */

int B_IdtExecuteIdts(Sample* smpl, char** idts)
{
    KDB*  kdb_idt;
    KDB*  kdb_var;

    error_manager.clear();

    if(idts == NULL || SCR_tbl_size((unsigned char**) idts) == 0)
    {
        kdb_var = KI_exec(KI_WS.get(),
                          KV_WS.get(), SCR_tbl_size((unsigned char**) KEXEC_VFILES), KEXEC_VFILES,
                          KS_WS.get(), SCR_tbl_size((unsigned char**) KEXEC_SFILES), KEXEC_SFILES,
                          smpl);
    }
    else 
    {
        kdb_idt = K_refer(KI_WS.get(), SCR_tbl_size((unsigned char**) idts), idts);
        kdb_var = KI_exec(kdb_idt,
                          KV_WS.get(), SCR_tbl_size((unsigned char**) KEXEC_VFILES), KEXEC_VFILES,
                          KS_WS.get(), SCR_tbl_size((unsigned char**) KEXEC_SFILES), KEXEC_SFILES,
                          smpl);
        delete kdb_idt;
        kdb_idt = nullptr;
    }

    SCR_free_tbl((unsigned char**) KEXEC_VFILES);
    KEXEC_VFILES = NULL;
    SCR_free_tbl((unsigned char**) KEXEC_SFILES);
    KEXEC_SFILES = NULL;

    std::string last_error = error_manager.get_last_error();
    if(last_error != "")
    {
        error_manager.append_error("Error(s) during identity execution:\n" + last_error);
        return -1;
    }

    if(!kdb_var) 
        return -1;
    
    KV_sample(kdb_var, smpl);

    if(KV_WS.get()) 
        KV_merge_del(KV_WS.get(), kdb_var, 1);
    else
        KV_WS.reset(kdb_var);

    return 0;
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

int B_IdtExecuteVarFiles(char* arg, int unused)
{
    SCR_free_tbl((unsigned char**) KEXEC_VFILES);
    KEXEC_VFILES = B_ainit_chk(arg, NULL, 0);
    return(0);
}


/**
 *  Report function that defines the list of external Scalar files where the next call to B_IdtExecute() 
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

int B_IdtExecuteSclFiles(char* arg, int unused)
{
    SCR_free_tbl((unsigned char**) KEXEC_SFILES);
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

int B_IdtExecuteTrace(char* arg, int unused)
{
    KEXEC_TRACE = (SCR_upper_char(arg[0]) == 'Y') ? 1 : 0;  /* JMP 19-12-97 */
    return(0);
}

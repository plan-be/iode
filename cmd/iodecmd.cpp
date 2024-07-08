/**
 *  @header4iode
 *  
 *  IODECMD
 *  -------
 *  Console program to run IODE reports. 
 *  The aim of that version of IODE is to automate processes like (re-)estimations, 
 *  simulations, tables and graphs production, etc.
 *  
 *  List of functions 
 *  -----------------
 *  
 *      void A2mMessage_super_CMD(char* msg)   | Function that superseeds the standard scr4 function A2mMessage() in the context of a console program.
 *      int kconfirm_super_CMD(char *fmt)      | Function that superseeds the standard function kconfirm() in the context of a console program.
 *      void ODE_assign_super_CMD()            | Assigns to "super-function" pointers their specific implementations for iodecmd.exe.
 *      void IodeCmdSyntax()                   | Prints the syntax of iodecmd.
 *      int main(int argc, char **argv)        | Main function
 *
 *  
 *  Examples
 *  --------
 *      C:> iodecmd myreport.rep parm1 parm1
 *  
 *      C:> iodecmd --help
 *          IODECMD v. 6.66 (Mar 22 2023)
 *          Syntax: iodecmd [-nbrun n] [-alloclog filename] [-v] [-y] [-h] reportfile [arg1 arg2 ...]
 *              where:
 *                 -nbrun n: how many times the report must be executed.
 *                 -alloclog filename: log allocations not freed at the end of the report execution
 *                 -v or --verbose: give more detailed messages
 *                 -y or --forceyes: when a question is asked, automatically answer yes
 *                 -h or --help: display this message
 *                 reportfile: report filename
 *                 arg1 arg2 ...: optional report arguments
 *  
 *      test.rep:
 *      --------
 *          # Try to load an unexisting varfile
 *          # When called without -y parameter, stops and wait for an answer
 *          # When called with -y parameter, continues until the end of the report
 *      
 *          $wsloadvar xxx.var  
 *  
 *      Run without -y
 *      --------------
 *      C:\Usr\iode_src\cmd>iodecmd test
 *      Loading xxx
 *      xxx not found or incompatible type
 *      Error : wsloadvar xxx
 *      Press ENTER to continue
 *      
  *      Run with -y
 *      -----------
 *      C:\Usr\iode_src\cmd>iodecmd -y test
 *      Loading xxx
 *      xxx not found or incompatible type
 *      Error : wsloadvar xxx
 *  
 *  
 */


#include "iode.h"

/* Allocation debugging  (see scr/s_allc.c) */
extern "C" long  SCR_TOTAL_ALLOC;        // Total remaining memory allocation in bytes 
// NOTE: SCR_ALLOC_DOC is declared as extern in scr4/s_strs.h 
extern "C" int   SCR_ALLOC_DOC;          // Log the remaining allocations    
extern "C" char  *SCR_ALLOC_DOC_LOGFILE; // Log file for remaining allocations    


// "super" functions
// -----------------

/**
 *  Function that superseeds the standard scr4 function A2mMessage() in the context 
 *  of a console program.
 *    
 *  @param [in] msg char*   message to display/print/ignore
 */
 void A2mMessage_super_CMD(char* msg)
{
    fprintf(stderr, "%-79.79s\n", msg);
}


/**
 *  Function that superseeds the standard function kconfirm() in the context of a console program.
 *  The question is constructed as printf(fmt, ...).
 *  The answer is read via a call to get_s(). 
 *  The return value is 0 (meaning continue) if the user answers yes (OoYyJj1) to the question.
 *  
 *  @param [in] fmt char*   format string in printf like syntax
 *  @return         int     0 if the answer starts with one of the following vchars : "0oYyJj1"
 *                          1 otherwise
 */
int kconfirm_super_CMD(const char* fmt)
{
    char    buf[256];

    strcpy_s(buf, fmt);
    gets_s(buf, sizeof(buf) - 1);
    SCR_sqz((unsigned char*) buf);
//    printf("buf = '%s'\n", buf);
    return(!U_is_in(buf[0], "OoYyJj1"));
}


/**
 *  Assigns to "super-function" pointers their specific implementations for iodecmd.exe.
 *  First assigne the default function pointers defined in iode api library.
 */
void ODE_assign_super_CMD()
{
    IODE_assign_super_API();
    A2mMessage_super    = A2mMessage_super_CMD;
    kconfirm_super      = kconfirm_super_CMD;
}


/**
 *  Prints the syntax of iodecmd.
 *  
 */
void IodeCmdSyntax()
{
    printf("IODECMD v. %d.%d (%s)\n", IODE_VERSION_MAJOR, IODE_VERSION_MINOR, __DATE__);
    printf("Syntax: iodecmd [-nbrun n] [-alloclog filename] [-v] [-y] [-h] reportfile [arg1 arg2 ...]\n");
    printf("    where:\n"); 
    printf("       -nbrun n: how many times the report must be executed.\n");
    printf("       -alloclog filename: log allocations not freed at the end of the report execution \n");
    printf("       -v or --verbose: give more detailed messages\n");
    printf("       -y or --forceyes: when a question is asked, automatically answer yes\n");
    printf("       -h or --help: display this message \n");
    printf("       reportfile: report filename  \n");
    printf("       arg1 arg2 ...: optional report arguments\n");
}


/**
 *  Main program. 
 *  
 *  @param [in] argc    int     nb of arguments
 *  @param [in] argv    char**  table of args
 *  @return             int     0 on success, report exec return code on error
 */
int main(int argc, char **argv)
{

    int     i, nbrun = 1, rc;
    char    *reportname = NULL, **reportargs = NULL;
    int     IODE_VERBOSE = 0;

    ODE_assign_super_CMD();
    SCR_ALLOC_DOC = 0;

    // Error file location: 
    // --------------------
    //  Old versions: iode.msg was appended to iodecmd.exe file
    //    => rejected by McAfee ATP
    //
    // strcpy(SCR_NAME_ERR, argv[0]);
    // SCR_ERR_FILE_NB = 1;
    //
    //  To work around the McAfee ATP problem, an appended file was used:  
    // SCR_change_ext(SCR_NAME_ERR, argv[0], "msg");
    // SCR_ERR_FILE_NB = 1;
    //
    // The latest versions (> 6.65) simply used the iode.msg file located in the same directory
    // as iodecmd.exe : 
    B_IodeMsgPath();    // IODE > 6.65

    for(i = 1 ; i < argc ; i++) {
        if(strcmp(argv[i], "-nbrun") == 0) {
            nbrun = atol(argv[i + 1]);
            i++;
        }
        else if(strcmp(argv[i], "-alloclog") == 0) {
            SCR_ALLOC_DOC_LOGFILE = argv[i + 1];
            SCR_ALLOC_DOC = 1;
            i++;
        }
        else if(strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) {
            IODE_VERBOSE = 1;
        }
        else if(strcmp(argv[i], "--forceyes") == 0 || strcmp(argv[i], "-y") == 0) {
            //IODE_FORCEYES = 1;
            kmsgbox_continue = 1; // JMP 11/12/2021
            kpause_continue = 1;  // JMP 16/06/2022
        }

        else if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            IodeCmdSyntax();
            return(0);
        }
        else if(argv[i][0] == '-') {
            IodeCmdSyntax();
            return(1);
        }
        else {
            reportname = argv[i];
            reportargs = argv + i + 1;
            break;
        }
    }


    if(nbrun < 1 || reportname == NULL) {
        printf("%s\n", IODE_VERSION); // JMP 17-11-2015
        printf("Iodecmd : nothing to do ...\n");
        exit(1);
    }

    if(IODE_VERBOSE) printf("%s\n", IODE_VERSION);
    IodeInit();

//    printf("Before 1st run : %d bytes allocated\n", SCR_TOTAL_ALLOC);
    for(i = 0 ; i < nbrun ; i++) {
        rc = IodeExecArgs(reportname, reportargs);
        if(IODE_VERBOSE) printf("\n*****Run %d/%d *****\nrc = %d -- total residual allocation : %d\n", i + 1, nbrun, rc, SCR_TOTAL_ALLOC);
    }

    IodeEnd();
    if(IODE_VERBOSE) printf("After last run : %d bytes allocated\n", SCR_TOTAL_ALLOC);
    AllocDocLoop();
    exit(rc);
}


// Function used by A2M HTML generator (does not work in 64bits)
#include "iode.h" 

int A2mGIF_HTML(A2MGRF *go, U_ch* filename) 
{
    return(0);
} 
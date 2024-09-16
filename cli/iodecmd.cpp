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

#include <cstdio>
#include <cstring>

#include <iostream>
#include <string>

#include "iode.h"
#include <istream>

/* Allocation debugging  (see scr/s_allc.c) */
extern "C" long SCR_TOTAL_ALLOC;        // Total remaining memory allocation in bytes 
extern "C" int SCR_ALLOC_DOC;           // Log the remaining allocations    
extern "C" char *SCR_ALLOC_DOC_LOGFILE; // Log file for remaining allocations    

const static std::string s_yes = "OoYyJj1";


// "super" functions
// -----------------

/**
 *  Function that supersedes the standard scr4 function A2mMessage() in the context 
 *  of a console program.
 *    
 *  @param [in] msg char*   message to display/print/ignore
 */
void A2mMessage_super_CMD(char* msg)
{
    std::cerr << std::string(msg, 79) << std::endl;
}

/**
 *  Function that supersedes the standard function kconfirm() in the context of a console program.
 *  The question is constructed as printf(fmt, ...).
 *  The answer is read via a call to std::getline(). 
 *  The return value is 0 (meaning continue) if the user answers yes (OoYyJj1) to the question.
 *  
 *  @param [in] fmt char*   format string in printf like syntax
 *  @return         int     0 if the answer starts with one of the following chars : "0oYyJj1"
 *                          1 otherwise
 */
int kconfirm_super_CMD(const char* fmt)
{
    std::cout << std::string(fmt) << std::endl;

    std::string answer;
    std::getline(std::cin, answer);
    // Remove all spaces
    answer.erase(std::remove(answer.begin(), answer.end(), ' '), answer.end());
    
    return (s_yes.find(answer[0]) != std::string::npos) ? 0 : 1;
}

/**
 *  Assigns to "super-function" pointers their specific implementations for iodecmd.exe.
 *  First assign the default function pointers defined in iode API library.
 */
void ODE_assign_super_CMD()
{
    IODE_assign_super_API();
    A2mMessage_super = A2mMessage_super_CMD;
    kconfirm_super = kconfirm_super_CMD;
}

// Function used by A2M HTML generator (does not work in 64bits)
int A2mGIF_HTML(A2MGRF *go, U_ch* filename) 
{
    return 0;
}

/**
 *  Prints the syntax of iodecmd.
 *  
 */
void IodeCmdSyntax()
{
    std::cout << "IODECMD v. " << IODE_VERSION_MAJOR << "." << IODE_VERSION_MINOR << " (" << __DATE__ << ")\n";
    std::cout << "Syntax: iodecmd [-nbrun n] [-alloclog filename] [-v] [-y] [-h] reportfile [arg1 arg2 ...]\n";
    std::cout << "    where:\n"; 
    std::cout << "       -nbrun n: how many times the report must be executed.\n";
    std::cout << "       -alloclog filename: log allocations not freed at the end of the report execution \n";
    std::cout << "       -v or --verbose: give more detailed messages\n";
    std::cout << "       -y or --forceyes: when a question is asked, automatically answer yes\n";
    std::cout << "       -h or --help: display this message \n";
    std::cout << "       reportfile: report filename  \n";
    std::cout << "       arg1 arg2 ...: optional report arguments\n";
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
    int nbrun = 1; 
    int rc;
    char *reportname = nullptr; 
    char **reportargs = nullptr;
    int IODE_VERBOSE = 0;

    ODE_assign_super_CMD();
    SCR_ALLOC_DOC = 0;

    B_IodeMsgPath(NULL);    // IODE > 6.65

    std::string argument;
    for(int i = 1; i < argc; i++) 
    {
        argument = std::string(argv[i]);

        if (argument == "-nbrun")
        {
            nbrun = std::atol(argv[i + 1]);
            i++;
        } 
        else if (argument == "-alloclog")
        {
            SCR_ALLOC_DOC_LOGFILE = argv[i + 1];
            SCR_ALLOC_DOC = 1;
            i++;
        } 
        else if (argument == "--verbose" || argument == "-v")
        {
            IODE_VERBOSE = 1;
        } 
        else if (argument == "--forceyes" || argument == "-y")
        {
            kmsgbox_continue = 1; // JMP 11/12/2021
            kpause_continue = 1;  // JMP 16/06/2022
        } 
        else if (argument == "--help" || argument == "-h")
        {
            IodeCmdSyntax();
            return 0;
        } 
        else if (argv[i][0] == '-') 
        {
            IodeCmdSyntax();
            return 1;
        } 
        else 
        {
            reportname = argv[i];
            reportargs = argv + i + 1;
            break;
        }
    }

    if (nbrun < 1 || reportname == nullptr) 
    {
        std::cout << IODE_VERSION << std::endl;
        std::cout << "Iodecmd : nothing to do ..." << std::endl;
        exit(1);
    }

    if (IODE_VERBOSE) 
        std::cout << IODE_VERSION << std::endl;
    
    IodeInit(NULL);

    for (int i = 0; i < nbrun; i++) 
    {
        rc = IodeExecArgs(reportname, reportargs);
        if (IODE_VERBOSE) 
            std::cout << "\n*****Run " << i + 1 << "/" << nbrun << " *****\nrc = " << rc 
                      << " -- total residual allocation : " << SCR_TOTAL_ALLOC << std::endl;
    }

    IodeEnd();

    if (IODE_VERBOSE) 
        std::cout << "After last run : " << SCR_TOTAL_ALLOC << " bytes allocated" << std::endl;

    AllocDocLoop();

    exit(rc);
}

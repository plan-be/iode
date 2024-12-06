#pragma once
#include <istream>
#include "api/iode.h"
#include "cpp_api/iode_cpp_api.h"


/* Allocation debugging  (see scr/s_allc.c) */
extern "C" long SCR_TOTAL_ALLOC;        // Total remaining memory allocation in bytes 
extern "C" int SCR_ALLOC_DOC;           // Log the remaining allocations    
extern "C" char *SCR_ALLOC_DOC_LOGFILE; // Log file for remaining allocations    


// Function used by A2M HTML generator (does not work in 64bits)
int A2mGIF_HTML(A2MGRF *go, U_ch* filename) 
{
    return 0;
}

int free_tbl(char** tbl)
{
    return SCR_free_tbl((unsigned char**) tbl);
}

/**
 *  Function that supersedes the standard scr4 function A2mMessage() in the context 
 *  of a console program.
 *    
 *  @param [in] msg char*   message to display/print/ignore
 */
void A2mMessage_super_CLI(char* msg)
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
int kconfirm_super_CLI(const char* fmt)
{
    std::string s_yes = "OoYyJj1";
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
void cli_assign_super()
{
    A2mMessage_super = A2mMessage_super_CLI;
    kconfirm_super = kconfirm_super_CLI;
}

// Initialize the command line interface
void initialize_cli()
{
    cli_assign_super();
    SCR_ALLOC_DOC = 0;
    B_IodeMsgPath(NULL);
}

// Clean up and finish the command line interface
void finish_cli()
{
    IodeEnd();
    if(SCR_ALLOC_DOC_LOGFILE)
        free(SCR_ALLOC_DOC_LOGFILE);
}

// Set the log file for the command line interface
void set_log_file_cli(char* log_file)
{
    SCR_ALLOC_DOC_LOGFILE = strdup(log_file);
    SCR_ALLOC_DOC = 1;
}

// Determine if the program should always continue
void always_continue(bool value)
{
    if(value)
    {
        kmsgbox_continue = 1;
        kpause_continue = 1;
    }
    else
    {
        kmsgbox_continue = 0;
        kpause_continue = 0;
    }
}

// Get the total allocated memory for the program in bytes
int get_total_allocated_memory()
{
    return (int) SCR_TOTAL_ALLOC;
}

void alloc_doc_loop()
{
    AllocDocLoop();
}

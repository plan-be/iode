/**
 * @header4iode
 *
 * Basic functions for managing error messages.
 * 
 * Main functions
 * --------------
 *      void B_IodeMsgPath(char*)           Retrieves the path to the iode.msg file and stores the result in the global SCR_NAME_ERR.
 *      char *B_msg(int n)                  Returns a static buffer containing the message n from file iode.msg. 
 *      void B_seterror(char* fmt, ...)     Formats an error message and adds the text of the message to the global table of last errors.
 *      void B_seterrn(int n, ...)          Formats a message found in iode.msg and adds the result to the list of last errors.
 *      void B_get_last_error()             Returns the last recorded errors (in B_ERROR_MSG).
 *      void B_display_last_error()         Displays the last recorded errors (in B_ERROR_MSG) using kmsgbox().
 *      void B_print_last_error()           Displays or prints the last recorded errors (in B_ERROR_MSG) using W_printf().
 *      void B_clear_last_error()           Resets the list of last messages (B_ERROR_MSG and B_ERROR_NB).
 *   
 */

#include "iode.h"
#include <stdarg.h>

char**   B_ERROR_MSG; // Table of last recorded error messages 
int      B_ERROR_NB;  // Nb of last recorded error messages 
char     *B_ERROR_DFT_MSG = "Error message not found (check the file 'iode.msg')"; // Default message if not found in iode.msg 

//extern char SCR_ERR_NAME[];  // Name of the iode.msg file

/**
 *  Retrieves the path to the iode.msg file and stores the result in the global SCR_NAME_ERR. 
 *  The path is constructed by appending "iode.msg" to the path of the current executable.
 *  
 *  To use is the context of programs calling iode API fns like iodecmd or 
 *  tests linked with iode_c_api.lib.
 *  
 *  Don't use this function in the context of the "DOS" GUI interface where this file is 
 *  already appended to iode.scr file.
 *  
 *  @param char* dir_path The directory path where the iode.msg file is located.
 *                        If NULL, the function will retrieve the directory path 
 *                        of the current executable.
 */
void B_IodeMsgPath(char* dir_path)
{
	char 	    module[1024], file[1024];
    static  int done = 0;

    // Get module directory (c:/iode p.ex)
    if(done) 
        return;

    if(dir_path == NULL)
    {
#ifdef __GNUC__
        readlink("/proc/self/exe", module, sizeof(module) - 1);
#else
        GetModuleFileName(0, module, 1000);
#endif
        SCR_split_dir(module, file);
    }
    else
    {
        strcpy(module, dir_path);
    }

#ifdef __GNUC__
	sprintf(SCR_NAME_ERR, "%s/iode.msg", module);
 #else
	sprintf(SCR_NAME_ERR, "%s\\iode.msg", module);
 #endif
 
    done = 1; 
}

/**
 *  Returns a static buffer containing the message n from file iode.msg. 
 *  
 *  @note The identification in iode.msg is 1000 + n.
 *  
 *  @param [in] n   int             message number (see dos/iode.msg)
 *  @return         static char*    message text
 *  
 */
char *B_msg(int n)
{
    char    *msg;
    
    B_IodeMsgPath(NULL);                    // JMP 10/04/2023
    msg = SCR_err_txt(n + 1000);   
    if(msg == 0) return(B_ERROR_DFT_MSG);   // JMP 13/04/2023
    return(msg); 
}


/**
 *  Adds an error message to the table B_ERROR_MSG only if the same message is not yet present.
 *  
 *  @param [in] msg     char*   formatted message
 *
 *  @global [in, out]   B_ERROR_NB  int       number of records errors
 *  @global [in, out]   B_ERROR_MSG char**    recorded error messages
 *  
 *  @return             void    
 */
void B_add_error(char* msg)
{
    int     i;
    
    if(msg == 0 || msg[0] == 0) return;
    
    if(B_ERROR_NB > 0) {
        for(i = 0; i < B_ERROR_NB; i++) {
            // If the same msg is already in the list, do not add
            if(B_ERROR_MSG[i] && strcmp(B_ERROR_MSG[i], msg) == 0)
                return;
        }
    }
    
    SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, msg);
}


/**
 *  Formats an error message and adds the text of the message to the global table of last errors.
 *  
 *  @param [in] fmt     char*   format in printf style
 *  @param [in] ...      -      any argument needed by fmt
 *
 *  @global [in, out] B_ERROR_NB  int       number of records errors
 *  @global [in, out] B_ERROR_MSG char**    recorded error messages
 *  
 *  @return             void    
 *  
 */
void B_seterror(char* fmt, ...)
{
    char    buf[256];
    va_list myargs;

    
    va_start(myargs, fmt);
    if(fmt == 0) strcpy(buf, B_ERROR_DFT_MSG);
    
#ifdef _MSC_VER   
    vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
    vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif    
    va_end(myargs);

    //SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, buf);
    B_add_error(buf);
}



/**     
 *  Formats a message found in iode.msg and adds the result to the list of last errors.
 *  
 *  Read the text or format of a message in the file iode.msg.
 *  Next, formats the message using the optional parameters and calls B_seterror() to 
 *  add the resulting message to the table of errors B_ERROR_MSG.
 *  
 *  Warning: if the parameters of the formating string are incorrect or missing, the program may
 *  crash!
 *  
 *  @param [in] n       int     message id in iode.msg (add 1000 to find the message)
 *  @param [in] ...      -      any argument needed to format the message
 *
 *  @global [in, out] B_ERROR_NB  int       number of records errors
 *  @global [in, out] B_ERROR_MSG char**    recorded error messages
 *  
 *  @return             void    
 */
void B_seterrn(int n, ...)
{
    char    buf[256];
    char    *B_msg();
    va_list myargs;
    
    va_start(myargs, n);    
#ifdef _MSC_VER   
    vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, B_msg(n), myargs);
#else
    vsnprintf_s(buf, sizeof(buf) - 1, B_msg(n), myargs);
#endif    

    B_seterror("%s", buf);
    va_end(myargs);
}


/**
 *  Returns the last recorded errors (in B_ERROR_MSG).
 *  Reset B_ERROR_MSG after having displayed its content.
 *  This function is intended to be called from a C++ code which will wrap 
 *  the returned char array into a C++ exception. 
 *  The returned vector char* must be freed using SCR_free() by the caller.
 *  
 *  @global [in, out] B_ERROR_NB  int       number of records errors
 *  @global [in, out] B_ERROR_MSG char**    recorded error messages
 *  @return char**                          recorded error messages
 */
char* B_get_last_error()
{
    char* errors = NULL;

    if(B_ERROR_NB == 0) return errors;
    
    // Adds a null pointer to close B_ERROR_MSG
    SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, NULL);

    errors = SCR_mtov(B_ERROR_MSG, '\n');

    B_clear_last_error();

    return errors;
}

/**
 *  Displays the last recorded errors (in B_ERROR_MSG) using kmsgbox().
 *  Reset B_ERROR_MSG after having displayed its content.
 *  
 *  @global [in, out] B_ERROR_NB  int       number of records errors
 *  @global [in, out] B_ERROR_MSG char**    recorded error messages
 */
void B_display_last_error()
{
    char    *v = NULL;

    if(B_ERROR_NB == 0) return;
    
    // Adds a null pointer to close B_ERROR_MSG
    SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, NULL);

    v = SCR_mtov(B_ERROR_MSG, '\n');
    //buts[0] = "Ok";
    //buts[1] = 0;
    //kmsgbox("E R R O R", v, buts);             // JMP 10/12/2021
    kerror(0, v);
    kpause();
    SCR_free(v);
    B_clear_last_error();
}


/**
 *  Displays or prints the last recorded errors (in B_ERROR_MSG) using W_printf().
 *  Reset B_ERROR_MSG after having displayed its content.
 *  
 *  @global [in, out] B_ERROR_NB  int       number of records errors
 *  @global [in, out] B_ERROR_MSG char**    recorded error messages
 */
void B_print_last_error()
{
    int     i;

    if(B_ERROR_NB == 0) return;
    SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, NULL);

    for(i = 0; i < B_ERROR_NB - 1; i++) W_printf("%s\n", B_ERROR_MSG[i]);

    B_clear_last_error();
}


/**
 *  Resets the list of last messages (B_ERROR_MSG and B_ERROR_NB).
 *  
 *  @global [in, out] B_ERROR_NB  int       number of records errors
 *  @global [in, out] B_ERROR_MSG char**    recorded error messages
 */
void B_clear_last_error()
{
    if(B_ERROR_NB == 0) return;
    SCR_add_ptr(&B_ERROR_MSG, &B_ERROR_NB, NULL);
    SCR_free_tbl(B_ERROR_MSG);
    B_ERROR_MSG = 0;
    B_ERROR_NB = 0;
}

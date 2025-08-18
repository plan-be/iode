/**
 * @header4iode
 *
 * Basic functions for managing error messages.
 * 
 * Main functions
 * --------------
 *      char *B_msg(int n)                  Returns a static buffer containing the message n from the table iode_msg_map. 
 *      void B_seterror(char* fmt, ...)     Formats an error message and adds the text of the message to the global table of last errors.
 *      void B_seterrn(int n, ...)          Formats a message found in iode_msg_map and adds the result to the list of last errors.
 *      void B_get_last_error()             Returns the last recorded errors (in B_ERROR_MSG).
 *      void B_display_last_error()         Displays the last recorded errors (in B_ERROR_MSG) using kmsgbox().
 *      void B_print_last_error()           Displays or prints the last recorded errors (in B_ERROR_MSG) using W_printf().
 *      void B_clear_last_error()           Resets the list of last messages (B_ERROR_MSG and B_ERROR_NB).
 *   
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "scr4/s_prost.h"

#include "api/write/write.h"
#include "api/k_super.h"
#include "b_errors.h"


/**
 *  Returns a static buffer containing the message n from the table iode_msg_map. 
 *  
 *  @note The identification in iode_msg_map is 1000 + n.
 *  
 *  @param [in] n   int             message number (see iode_msg_map)
 *  @return         static char*    message text
 *  
 */
char *B_msg(int n)
{
    char    *msg;
    
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
    
    SCR_add_ptr((unsigned char***) &B_ERROR_MSG, &B_ERROR_NB, (unsigned char*) msg);
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
 *  Formats a message found in iode_msg_map and adds the result to the list of last errors.
 *  
 *  Read the text or format of a message in the table iode_msg_map.
 *  Next, formats the message using the optional parameters and calls B_seterror() to 
 *  add the resulting message to the table of errors B_ERROR_MSG.
 *  
 *  Warning: if the parameters of the formating string are incorrect or missing, the program may
 *  crash!
 *  
 *  @param [in] n       int     message id in iode_msg_map (add 1000 to find the message)
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

    if(B_ERROR_NB == 0) 
        return errors;
    
    // Adds a null pointer to close B_ERROR_MSG
    SCR_add_ptr((unsigned char***) &B_ERROR_MSG, &B_ERROR_NB, NULL);

    errors = (char*) SCR_mtov((unsigned char**) B_ERROR_MSG, (int) '\n');

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

    if(B_ERROR_NB == 0) 
        return;
    
    // Adds a null pointer to close B_ERROR_MSG
    SCR_add_ptr((unsigned char***) &B_ERROR_MSG, &B_ERROR_NB, NULL);

    v = (char*) SCR_mtov((unsigned char**) B_ERROR_MSG, (int) '\n');
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

    if(B_ERROR_NB == 0) 
        return;
    
    SCR_add_ptr((unsigned char***) &B_ERROR_MSG, &B_ERROR_NB, NULL);

    for(i = 0; i < B_ERROR_NB - 1; i++) 
        W_printf("%s\n", B_ERROR_MSG[i]);

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
    SCR_add_ptr((unsigned char***) &B_ERROR_MSG, &B_ERROR_NB, NULL);
    SCR_free_tbl((unsigned char**) B_ERROR_MSG);
    B_ERROR_MSG = 0;
    B_ERROR_NB = 0;
}

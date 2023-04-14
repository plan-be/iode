/**
 *  @header4iode
 * 
 *  Functions to send information and error messages from estimation functions.
 *  
 *  List of functions 
 *  -----------------
 *      void E_msg(char* fmt,...)       Displays a message using kmsg().
 *      void E_error(char* fmt,...)     Displays an estimation error message using B_seterror().
 */

#include "iode.h"
#include <stdarg.h>


/**
 *  Displays a message using kmsg().
 *  
 *  @param [in] char*   fmt     Message format (1st arg of vsnprintf_s())
 *  @param [in]         ...     fmt parameters 
 *  
 */
 
void E_msg(char* fmt,...)
{
    va_list     myargs;
    char        buf[512];
    extern char* B_ERROR_DFT_MSG;
    
    va_start(myargs, fmt);
    if(fmt == 0) strcpy(buf, B_ERROR_DFT_MSG);  
    else {
#ifdef _MSC_VER   
        vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
        vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif  
    }
    
    va_end(myargs);

    kmsg("%s\n", buf);
}


//void E_msg_n(int n)
//{
//    char    *B_msg();
//
//    E_msg(B_msg(200 + n));
//}
//

/**
 *  Displays an estimation error message using B_seterror().
 *  
 *  @param [in] char*   fmt     Message format (1st arg of vsnprintf_s())
 *  @param [in]         ...     fmt parameters 
 *  
 */

void E_error(char* fmt,...)
{
    va_list     myargs;
    char        buf[512];
    extern char* B_ERROR_DFT_MSG;
    
    va_start(myargs, fmt);
    if(fmt == 0) strcpy(buf, B_ERROR_DFT_MSG);
    else {
#ifdef _MSC_VER   
        vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
        vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif  
    }
    va_end(myargs);

    B_seterror(buf);
}


/**
 *  Displays the estimation error message number n using B_seterror().
 *  
 *  @param [in] int n   Estimation error number (identified par 1200+n in iode.msg) 
 *  
 */
 
void E_error_n(int n)
{
    char    *B_msg();

    E_error("%s", B_msg(200 + n));
}

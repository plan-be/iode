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
#include <stdarg.h>

#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/estimation/estimation.h"

extern "C" char* B_ERROR_DFT_MSG;


/**
 *  Displays a message using kmsg().
 *  
 *  @param [in] char*   fmt     Message format (1st arg of vsnprintf_s())
 *  @param [in]         ...     fmt parameters  
 */
void Estimation::E_msg(char* fmt, ...)
{
    va_list     myargs;
    char        buf[512];
    
    va_start(myargs, fmt);
    if(fmt == 0) strcpy(buf, B_ERROR_DFT_MSG);  
    else 
    {
#ifdef _MSC_VER   
        vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, fmt, myargs);
#else
        vsnprintf_s(buf, sizeof(buf) - 1, fmt, myargs);
#endif  
    }
    va_end(myargs);

    kmsg("%s\n", buf);
}


// see B_seterrn() for code example
void Estimation::E_msg_n(int n, ...)
{
    char    buf[256];
    va_list myargs;
    
    va_start(myargs, n);    
#ifdef _MSC_VER   
    vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, B_msg(200 + n), myargs);
#else
    vsnprintf_s(buf, sizeof(buf) - 1, B_msg(200 + n), myargs);
#endif    
    va_end(myargs);

    kmsg("%s\n", buf);
}


/**
 *  Displays an estimation error message using B_seterror().
 *  
 *  @param [in] char*   fmt     Message format (1st arg of vsnprintf_s())
 *  @param [in]         ...     fmt parameters 
 */
void Estimation::E_error(char* fmt,...)
{
    va_list     myargs;
    char        buf[512];
    
    va_start(myargs, fmt);
    if(fmt == 0) strcpy(buf, B_ERROR_DFT_MSG);
    else 
    {
#if defined(_MSC_VER)  
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
 *  @param [in] int n   Estimation error number (identified par 1200+n in iode_msg_map) 
 */
void Estimation::E_error_n(int n, ...)
{
    // see B_seterrn() for code example

    char    buf[256];
    va_list myargs;
    
    va_start(myargs, n);
#if defined(_MSC_VER)  
        vsnprintf_s(buf, sizeof(buf) - 1, _TRUNCATE, B_msg(200 + n), myargs);
#else
        vsnprintf_s(buf, sizeof(buf) - 1, B_msg(200 + n), myargs);
#endif  
    va_end(myargs);

    B_seterror(buf);
}

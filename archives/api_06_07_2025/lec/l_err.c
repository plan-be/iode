/**
 *  @header4iode
 *  
 *  LEC: numerical and textual error messages.
 *      
 *      int L_errnon:       Last error number during LEC compilation
 *      char* L_error():    Returns a static buffer containing the last LEC compilation error message.
 */


#include "api/lec/lec.h"

//#define L_MAX_WARNS   100
//#define L_MAX_ERRS    20

// LEC error number (during compilation)
int     L_errno = 0;  

// LEC error messages corresponding to L_errno
char  *L_ERR_TEXT[] = {
    "syntax error",
    "unpaired parentheses",
    "incorrect nb or args",
    "stack full",
    "memory full",
    "illegal period",
    "illegal lag",
    "different periodicities",
    "period out of bounds",
    "link limit exceeded",
    "division by zero",
    "log domain error",
    "illegal coefficient",
    "assign not found (:=)",
    "endo not found",
    "cannot invert equation",
    "variable not found",
    "macro undefined"
};


/**
 *  Returns a static buffer containing the last LEC compilation error message.
 *  
 *  @return     char*   static buffer with the last error messag.e 
 */
 
char *L_error()
{
    static char     msg[128];
    extern YYFILE*  L_YY; // LEC stream the compiler is reading from

    msg[0] = 0;
    if(L_errno <= 0) return(msg);
    if(L_YY == 0)
        sprintf(msg, "%s", L_ERR_TEXT[L_errno - 1]);
    else
        sprintf(msg, "%s:%s", YY_error(L_YY), L_ERR_TEXT[L_errno - 1]);
    return(msg);
}






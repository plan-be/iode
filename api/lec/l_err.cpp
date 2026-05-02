/**
 *  @header4iode
 *  
 *  LEC: numerical and textual error messages.
 *      
 *      int L_errnon:       Last error number during LEC compilation
 *      char* L_error():    Returns a static buffer containing the last LEC compilation error message.
 */
#include "api/lec/l_err.h"


/**
 *  Returns a static buffer containing the last LEC compilation error message.
 *  
 *  @return     char*   static buffer with the last error messag.e 
 */
 
char* L_error()
{
    static char msg[128];

    msg[0] = 0;
    if(L_errno <= 0) 
        return(msg);
    
    if(L_YY == 0)
        sprintf(msg, "%s", L_ERR_TEXT[L_errno - 1]);
    else
        sprintf(msg, "%s:%s", YY_error(L_YY), L_ERR_TEXT[L_errno - 1]);
    
    return msg;
}

#include "iode.h"

#define L_MAX_WARNS   100
#define L_MAX_ERRS    20

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

/*
    Returns a static buffer pointer containing the last Lec error message
*/

char *L_error()
{
    static char msg[128];

    msg[0] = 0;
    if(L_errno <= 0) return(msg);
    if(L_YY == 0)
	sprintf(msg, "%s", L_ERR_TEXT[L_errno - 1]);
    else
	sprintf(msg, "%s:%s", YY_error(L_YY), L_ERR_TEXT[L_errno - 1]);
    return(msg);
}






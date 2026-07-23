#pragma once
#include <string>
#include "scr4/s_yy.h"              // YYKEYS

inline int L_errno = 0;             // LEC error number (during compilation)
inline YYFILE* L_YY = nullptr;      // LEC stream the compiler is reading from

enum LecError
{
    L_SYNTAX_ERR = 1,
    L_PAR_ERR,
    L_ARGS_ERR,
    L_STACK_ERR,
    L_MEMORY_ERR,
    L_PERIOD_ERR,
    L_LAG_ERR,
    L_PERIODY_ERR,
    L_BOUNDS_ERR,
    L_LINK_ERR,
    L_DIVIDE_ERR,
    L_LOG_ERR,
    L_COEF_ERR,
    L_ASSIGN_ERR,
    L_DUP_ERR,
    L_INVERT_ERR,
    L_NOT_FOUND_ERR,
    L_MACRO_ERR
};

// LEC error messages corresponding to L_errno
inline const std::string L_ERR_TEXT[] = 
{
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
 *  Returns the last LEC compilation error message.
 */
inline std::string L_error()
{ 
    if(L_errno <= 0) 
        return "";
    
    if(!L_YY)
        return L_ERR_TEXT[L_errno - 1];
    else
        return std::string(YY_error(L_YY)) + ":" + L_ERR_TEXT[L_errno - 1];
}

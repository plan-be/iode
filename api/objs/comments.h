#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"

/*----------------------- TYPEDEF ----------------------------*/

// CMT = Comment (string)
using CMT = char*;

/*----------------------- MACROS ----------------------------*/

// Returns a pointer to the CMT in swap (not allocated -> do not free)
inline CMT KCPTR(KDB* kdb, char* name)
{
    return K_optr0(kdb, name);            
}

inline CMT KCVAL(KDB* kdb, int pos)
{
    return (CMT ) K_oval0(kdb, pos);
}  

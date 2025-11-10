#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"

/*----------------------- TYPEDEF ----------------------------*/

// CMT = Comment (string)
using CMT = char*;

/*----------------------- MACROS ----------------------------*/

// Returns a pointer to the CMT in swap (not allocated -> do not free)
inline CMT KCVAL(KDB* kdb, const std::string& name)
{
    return K_optr0(kdb, (char*) name.c_str());            
}

inline CMT KCVAL(KDB* kdb, SWHDL handle)
{
    return (CMT) P_get_ptr(SW_getptr(handle), 0);            
}

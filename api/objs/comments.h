#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"

/*----------------------- TYPEDEF ----------------------------*/

// CMT = Comment (string)
using CMT = char*;

/*----------------------- FUNCS ----------------------------*/

inline CMT KCPTR(const char* name) 
{
    return K_optr0(KC_WS, (char*) name);
}

inline CMT KCVAL(KDB* kdb, int pos) 
{
    return (CMT)K_oval0(kdb, pos);
}

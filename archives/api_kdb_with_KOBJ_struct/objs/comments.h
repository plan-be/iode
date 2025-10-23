#pragma once

#include "api/constants.h"

/*----------------------- TYPEDEF ----------------------------*/

// CMT = Comment (string)
using CMT = char*;

/*----------------------- MACROS ----------------------------*/

#define KCPTR(name)         K_optr0(KC_WS, name)            // Returns a pointer to the CMT in swap (not allocated -> do not free)
#define KCVAL(kdb, pos)     ((CMT )   K_oval0(kdb, pos))

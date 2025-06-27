#pragma once

/*----------------------- TYPEDEF ----------------------------*/

// CMT = Comment (string)
typedef char    *CMT;

/*----------------------- MACROS ----------------------------*/

#define KCPTR(name)         K_optr0(KC_WS, name)     // Returns a pointer to the CMT in swap (not allocated -> do not free)

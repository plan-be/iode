#pragma once

#include "api/constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------- TYPEDEF ----------------------------*/

// CMT = Comment (string)
typedef char    *CMT;

/*----------------------- MACROS ----------------------------*/

#define KCPTR(name)         K_optr0(KC_WS, name)     // Returns a pointer to the CMT in swap (not allocated -> do not free)
#define KCVAL(kdb, pos)     ((CMT )   K_oval0(kdb, pos))
//#define KCVAL(kdb, pos)     ((CMT *)  K_oval0(kdb, pos))    // TODO: Check (CMT) or (CMT *) => ok:CMT

#ifdef __cplusplus
}
#endif

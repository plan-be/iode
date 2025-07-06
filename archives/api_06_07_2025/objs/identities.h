#pragma once

#include "api/constants.h"
#include "api/lec/lec.h"

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------- STRUCTS ----------------------------*/

// IDT = Identity (struct with the identity LEC formula and the compiled CLEC structure)
typedef struct _idt_ {
    char     *lec;      // LEC expression (may ot be an equation)
    CLEC     *clec;     // Compiled version of LEC
} IDT;

/*----------------------- MACROS ----------------------------*/

#define KIPTR(name)         K_optr0(KI_WS, name)
#define KILEC(kdb, pos)     K_oval0(kdb, pos)
#define KICLEC(kdb, pos)    ((CLEC *) K_oval1(kdb, pos))

#ifdef __cplusplus
}
#endif

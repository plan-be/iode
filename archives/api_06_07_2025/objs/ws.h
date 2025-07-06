#pragma once 

#include "api/constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/* k_ws.c */
extern void K_init_ws(int );
extern void K_end_ws(int );
extern int K_load_RWS(int ref, char *filename);

#ifdef __cplusplus
}
#endif

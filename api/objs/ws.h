#pragma once 

#include "api/constants.h"

/* k_ws.c */
void K_init_ws(int ws);
void K_end_ws(int ws);
int K_load_RWS(int ref, char *filename);

#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"


/* k_grep.c */
char* K_expand(int type, char* file, char* c_pattern, int all);
int K_aggr(char* pattern, char* ename, char* nname);

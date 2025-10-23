#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"


/* k_grep.c */
char* *K_grep(KDB*, char*, int, int, int, int, int);
char* K_expand(int, char*, char*, int);
char* K_expand_kdb(KDB*, int, char*, int);
int K_aggr(char*, char*, char*);

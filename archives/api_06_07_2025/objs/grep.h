#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/* k_grep.c */
extern char **K_grep(KDB *,char *,int ,int ,int ,int ,int );
extern char *K_expand(int ,char *,char *,int );
extern char *K_expand_kdb(KDB *,int ,char *,int );
extern int K_aggr(char *,char *,char *);

#ifdef __cplusplus
}
#endif

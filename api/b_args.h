#pragma once

#include "scr4/s_args.h"

#include "api/constants.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char **B_ainit_chk(char *,ADEF *,int );
extern char **B_vtom_chk(char *,int );
extern int B_loop(char **,int (*)(char*, void*), char *);
extern int B_ainit_loop(char *,int (*)(char*, void*), char *);
extern int B_get_arg0(char *,char *,int );
extern int B_argpos(char *,int );

#ifdef __cplusplus
}
#endif

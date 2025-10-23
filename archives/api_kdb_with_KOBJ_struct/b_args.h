#pragma once

#include "scr4/s_args.h"
#include "api/constants.h"

char **B_ainit_chk(char *,ADEF *,int );
char **B_vtom_chk(char *,int );
int B_loop(char **,int (*)(char*, void*), char *);
int B_ainit_loop(char *,int (*)(char*, void*), char *);
int B_get_arg0(char *,char *,int );
int B_argpos(char *,int );

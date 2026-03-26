#pragma once

#include "scr4/s_args.h"
#include "api/constants.h"

char **B_ainit_chk(char* arg, ADEF* adef, int nb);
char **B_vtom_chk(char* arg, int nb);
int B_loop(char** argv, int (*fn)(char*, void*), char* client);
int B_ainit_loop(char* arg, int (*fn)(char*, void*), char* client);
int B_get_arg0(char* arg0, char* arg, int lg);
int B_argpos(char* str, int ch);

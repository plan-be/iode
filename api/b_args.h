#pragma once
#include "api/iode_scr4.h"
#include "scr4/args/s_args.h"
#include "scr4/strs/s_prost.h"
#include "scr4/strs/s_strs.h"

#include "api/pch.h"

std::vector<std::string> expand_arg(const std::string& arg, const int nb);
char **B_vtom_chk(char* arg, int nb);
int B_loop(char** argv, int (*fn)(char*, void*), char* client);
int B_ainit_loop(char* arg, int (*fn)(char*, void*), char* client);
int B_get_arg0(char* arg0, char* arg, int lg);
int B_argpos(char* str, int ch);

#pragma once
#include "api/iode_scr4.h"
#include "scr4/strs/s_prost.h"
#include "scr4/strs/s_strs.h"

#include "api/pch.h"

std::vector<std::string> expand_args(const std::string& arg, const int nb,
    const std::string& separators = " ,;\n\t\r");
int B_loop(char** argv, int (*fn)(char*, void*), char* client);
int B_ainit_loop(char* arg, int (*fn)(char*, void*), char* client);
std::string get_next_arg(std::string& args, int lg);
int B_argpos(const std::string& str, int ch);

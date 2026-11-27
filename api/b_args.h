#pragma once
#include "api/iode_scr4.h"
#include "scr4/strs/s_prost.h"
#include "scr4/strs/s_strs.h"

#include "api/pch.h"

std::vector<std::string> expand_args(const std::string& arg, const int nb,
    const std::string& separators = " ,;\n\t\r");
int B_loop(const std::vector<std::string>& argv, const std::function<int(const std::string&)>& fn);
int B_ainit_loop(const std::string& arg, const std::function<int(const std::string&)>& fn);
std::string get_next_arg(std::string& args, int lg);
int B_argpos(const std::string& str, int ch);

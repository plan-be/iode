#pragma once 
#include "cpp_api/common.h"

#include <string>
#include <cctype>
#include <vector>
#include <regex>

std::vector<std::string> build_command_functions_list(const int group, const bool gui=false);
std::vector<std::string> build_report_functions_list();
std::vector<std::string> build_lec_functions_list();

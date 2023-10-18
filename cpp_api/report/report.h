#pragma once

#include "common.h"
#include "utils/utils.h"

#include <string>
#include <vector>
#include <stdexcept>
#include <boost/algorithm/string.hpp>


/**
 * @brief 
 * 
 * @param filepath 
 * @param parameters 
 * 
 * @note same as function IodeExecArgs() from b_api.c
 *       See https://iode.plan.be/doku.php?id=reportexec 
 */
void execute_report(const std::string& filepath, const std::string& parameters = "");

void execute_report(const std::string& filepath, 
    const std::vector<std::string>& parameters = std::vector<std::string>());

/**
 * @brief 
 * 
 * @param commands Sets of report command(s) separated by '\n'
 */
void execute_report_line(const std::string& commands);

void execute_report_line(const std::vector<std::string>& commands);

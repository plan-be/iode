#pragma once

#include <string>
#include <vector>

#include "cpp_api/common.h"
#include "api/utils/utils.h"
#include "api/time/period.h"
#include "cpp_api/KDB/kdb_variables.h"

/**
 * @brief 
 * 
 * @param other 
 * @return CLEC* 
 * 
 * @note 
 */
CLEC* clec_deep_copy(const CLEC* other);

/**
 * @brief 
 * 
 * @param clec_1 
 * @param clec_2 
 * @return true 
 * @return false 
 */
bool clec_equal(const CLEC* clec_1, const CLEC* clec_2);

/**
 *  Evaluate the LEC expression for a specific period t.
 *  
 *  @param   lec  std::string   LEC expression to evaluate
 *  @param   t    int           position of the period in the sample
 *  @return       double        value of the calculated LEC expression at the period t 
 *                              or IODE_NAN on error
 * 
 *  @note same as function IodeExecLecT() from b_api.c (from iode_dos repository)
 */
double execute_lec(const std::string& lec, const int t);

/**
 *  Evaluate the LEC expression for a specific period.
 *  
 *  @param   lec      std::string   LEC expression to evaluate
 *  @param   period   std::string   period for which the LEC expression is evaluated
 *  @return           double        value of the calculated LEC expression for the passed period 
 *                                  or IODE_NAN on error
 * 
 *  @note same as function IodeExecLecT() from b_api.c (from iode_dos repository)
 */
double execute_lec(const std::string& lec, const std::string& period);

/**
 *  Evaluate the LEC expression on the full Variables sample.
 * 
 *  @param   lec  std::string   LEC expression to evaluate
 * 
 *  @return  std::vector<double>    vector containing the calculated values
 * 
 *  @note same as function IodeExecLec() from b_api.c (from iode_dos repository)
 */
std::vector<double> execute_lec(const std::string& lec);

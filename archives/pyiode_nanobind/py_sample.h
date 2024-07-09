#pragma once
#include <string>
#include <vector>
#include <exception>

#include "cpp_api/iode_cpp_api.h"

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/pair.h>

namespace nb = nanobind;


void init_sample(nb::module_ &m);


std::pair<std::string, std::string> ws_sample_get();

/*
 * ws_sample_set()
 * 
 * Example: if the current sample is 200Y1 2005Y1:
 * ws_sample_set("", "") )              => ["2002Y1", "2005Y1"]
 * ws_sample_set("2002Y1", "") )        => ["2002Y1", "2005Y1"]
 * ws_sample_set("", "2010Y1") )        => ["2002Y1", "2010Y1"]
 * ws_sample_set("2001Y1", "2020Y1") )  => ["2001Y1", "2020Y1"]
 * ws_sample_set("2001Q1", "2020Q3") )  => error 
 */
std::pair<std::string, std::string> ws_sample_set(const std::string& from = "", const std::string& to = "");

std::string ws_sample_to_string();

//  TODO: if as_floats is True and per_from and per_to are not empty, 
//        return the sample per_from to per_to in floats instead of the KV_WS sample
/*
def ws_sample_to_larray_axis(axis_name: str = 'time', 
                             per_from:str = '', per_to: str = '', 
                             as_floats: bool = False) -> la.Axis;
*/

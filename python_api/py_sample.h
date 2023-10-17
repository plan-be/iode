#pragma once
#include <string>
#include <vector>
#include <exception>

#include "cpp_api/iode_cpp_api.h"


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

inline std::pair<std::string, std::string> ws_sample_get()
{
    Sample sample = Variables.get_sample();
    return std::pair<std::string, std::string>(sample.start_period().to_string(), 
        sample.end_period().to_string());
}

inline std::pair<std::string, std::string> ws_sample_set(const std::string& from = "", const std::string& to = "")
{
    Variables.set_sample(from, to);
    return ws_sample_get();
}

inline std::string ws_sample_to_string()
{
    Sample sample = Variables.get_sample();
    if(sample.nb_periods() == 0)
        return "";
    else
        return sample.start_period().to_string() + " " + sample.end_period().to_string();
}

//  TODO: if as_floats is True and per_from and per_to are not empty, 
//        return the sample per_from to per_to in floats instead of the KV_WS sample
/*
def ws_sample_to_larray_axis(axis_name: str = 'time', 
                             per_from:str = '', per_to: str = '', 
                             as_floats: bool = False) -> la.Axis:
    '''
    Return the current sample or the sample [per_from, per_to] definition as an Axis.
        e.g.: Axis(["2000Y1", "2001Y1", ..., "2010Y1"], "time")
        
    If as_floats == True, the list items are floats.
       e.g.: [2000.0, 2000.25, 2000.50,2000.75,...]
    '''
    
    lst = ws_sample_to_list(per_from, per_to, as_floats)
    ax = la.Axis(lst, axis_name)
    return ax 
*/

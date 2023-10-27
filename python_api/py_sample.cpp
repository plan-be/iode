#include "py_sample.h"


void init_sample(nb::module_ &m)
{
    // Sample

    m.def("ws_sample_set", &ws_sample_set, nb::arg("from") = "", nb::arg("to") = "", "Set a new IODE sample and return it");
    
    m.def("ws_sample_get", &ws_sample_get, 
        "Return the current sample lower and upper bounds, e.g.: ('2000Y1', '2010Y1') or ('', '') if the sample is undefined.");
    
    m.def("ws_sample_nb_periods", []() { return Variables.get_nb_periods(); }, 
        "Return the number of periods in the current sample");
    
    m.def("ws_sample_to_string", &ws_sample_to_string, 
        "Return the current sample definition in a string: 'from to', e.g.: '2000Y1 2020Y1' or '' if the sample is undefined.");
    
    m.def("ws_sample_to_list", [](const std::string& from = "", const std::string& to = "") { return Variables.get_list_periods(from, to); }, 
        nb::arg("from") = "", nb::arg("to") = "",
        "Return the current sample definition as a list of periods.\ne.g.: ['2000Y1', '2001Y1', ..., '2010Y1'] or [] if the sample is undefined");
    
    std::string doc = "Return the current sample definition as a list of periods converted to float.\n";
    doc += "e.g.: [2000.0, 2001.0, ..., 2010.0] or [] if the sample is undefined";
    m.def("ws_sample_to_float_list", [](const std::string& from = "", const std::string& to = "") { return Variables.get_list_periods_as_float(from, to); }, 
        nb::arg("from") = "", nb::arg("to") = "", doc.c_str());
}


std::pair<std::string, std::string> ws_sample_get()
{
    Sample sample = Variables.get_sample();
    return std::pair<std::string, std::string>(sample.start_period().to_string(), 
        sample.end_period().to_string());
}

std::pair<std::string, std::string> ws_sample_set(const std::string& from, const std::string& to)
{
    Variables.set_sample(from, to);
    return ws_sample_get();
}

std::string ws_sample_to_string()
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

#include "cpp_api/lec/lec.h"


/**
 *  Compile and link the LEC expression with the current Scalars and Variables workspaces.
 *  
 *  @param   lec     std::string   LEC expression
 * 
 *  @note same as function IodeLinkLec() from b_api.c (from iode_dos repository).
 */
static std::shared_ptr<CLEC> link_lec(const std::string& lec)
{
    char* error_msg = to_char_array("Could not compile LEC expression " + lec);

    std::shared_ptr<CLEC> clec = nullptr; 
    
    try
    {
        clec = std::make_shared<CLEC>(lec);
    }
    catch(const std::exception&)
    {
        kerror(0, error_msg);
        return nullptr;
    } 
    
    int res = clec->link(global_ws_var, global_ws_scl);
    if(res != 0)
    {
        kerror(0, error_msg);
        return nullptr;
    }

    return clec;
}

double execute_lec(const std::string& lec, const int t)
{
    std::shared_ptr<CLEC> clec = link_lec(lec);
    if(!clec) 
        return IODE_NAN;

    double value = clec->execute(global_ws_var, global_ws_scl, t);
    return value;
}

double execute_lec(const std::string& lec, const std::string& period)
{
    if(!global_ws_var->check_sample())
        return IODE_NAN;
    
    std::shared_ptr<Sample> sample = global_ws_var->get_sample();
    int t = sample->get_period_position(period);
    return execute_lec(lec, t);
}

std::vector<double> execute_lec(const std::string& lec)
{
    std::vector<double> res;

    int nb_per = global_ws_var->get_nb_periods();
    if(nb_per == 0)
    {
        kerror(0, "The Variables sample is undefined");
        return res;
    }

    std::shared_ptr<CLEC> clec = link_lec(lec);
    if(!clec) 
        return res;

    res.reserve(nb_per);
    for(int t = 0; t < nb_per; t++) 
        res.push_back(clec->execute(global_ws_var, global_ws_scl, t));
    
    return res;
}

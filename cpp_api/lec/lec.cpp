#include "cpp_api/lec/lec.h"


/**
 *  Compile and link the LEC expression with the current Scalars and Variables workspaces.
 *  
 *  @param   lec     std::string   LEC expression
 * 
 *  @note same as function IodeLinkLec() from b_api.c (from iode_dos repository).
 */
static CLEC* link_lec(const std::string& lec)
{
    char* error_msg = to_char_array("Could not compile LEC expression " + lec);

    CLEC* clec = L_cc(to_char_array(lec));
    if(clec == NULL)
    {
        kerror(0, error_msg);
        return NULL;
    } 
    
    int res = L_link(KV_WS, KS_WS, clec);
    if(res != 0)
    {
        SCR_free(clec);
        kerror(0, error_msg);
        return NULL;
    }

    return clec;
}

double execute_lec(const std::string& lec, const int t)
{
    CLEC* clec = link_lec(lec);
    if(clec == NULL) 
        return IODE_NAN;

    double value = L_exec(KV_WS, KS_WS, clec, t);
    SCR_free(clec);

    return value;
}

double execute_lec(const std::string& lec, const std::string& period)
{
    int t = Variables.get_sample()->get_period_position(period);
    return execute_lec(lec, t);
}

std::vector<double> execute_lec(const std::string& lec)
{
    std::vector<double> res;

    int nb_per = Variables.get_nb_periods();
    if(nb_per == 0)
    {
        kerror(0, "The Variables sample is undefined");
        return res;
    }

    CLEC* clec = link_lec(lec);
    if(clec == NULL) 
        return res;

    res.reserve(nb_per);
    for(int t = 0; t < nb_per; t++) 
        res.push_back(L_exec(KV_WS, KS_WS, clec, t));
    SCR_free(clec);

    return res;
}

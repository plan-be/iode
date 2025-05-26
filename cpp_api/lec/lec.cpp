#include "lec.h"


CLEC* clec_deep_copy(const CLEC* other)
{
    // NOTE : see end of function L_cc2() from l_cc2.c to calculate tot_lg (= len)
    CLEC* copy = (CLEC*) SW_nalloc(other->tot_lg);

    copy->tot_lg = other->tot_lg,      
	copy->exec_lg = other->exec_lg;       
    copy->nb_names = other->nb_names;
    copy->dupendo = other->dupendo;
    copy->pad = '\0';
    for(int i = 0; i < other->nb_names; i++)
    {
        strncpy(copy->lnames[i].name, other->lnames[i].name, sizeof(ONAME) / sizeof(char));
        memset(copy->lnames[i].pad, '\0', sizeof(LNAME::pad) / sizeof(char));
        copy->lnames[i].pos = other->lnames[i].pos;
    }

    return copy;
}

bool clec_equal(const CLEC* clec_1, const CLEC* clec_2)
{
    if(clec_1->tot_lg != clec_2->tot_lg) return false;
    if(clec_1->exec_lg != clec_2->exec_lg) return false;
    if(clec_1->nb_names != clec_2->nb_names) return false;
    if(clec_1->dupendo != clec_2->dupendo) return false;
    if(clec_1->pad != clec_2->pad) return false;
    for(int i = 0; i < clec_1->nb_names; i++)
    {
        if(strcmp(clec_1->lnames[i].name, clec_2->lnames[i].name)) return false;
        if(strcmp(clec_1->lnames[i].pad, clec_2->lnames[i].pad)) return false;
        if(clec_1->lnames[i].pos, clec_2->lnames[i].pos) return false;
    }
    return true;
}

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

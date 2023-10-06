#pragma once
#include <string>
#include <vector>
#include <exception>

#include "cpp_api/iode_cpp_api.h"


/*
 *  TODO
 *  ----
 *   ws_copy(filename, filetype, list)
 *   ws_copy_cmt(filename, list)
 *   ws_copy_eqs(filename, list)
 *   ws_copy_idt(filename, list)
 *   ws_copy_lst(filename, list)
 *   ws_copy_scl(filename, list)
 *   ws_copy_tbl(filename, list)
 *   ws_copy_var(filename, list, sample) *
 *   ws_merge(filename, filetype, list)
 *   ws_merge_cmt(filename, list)
 *   ws_merge_eqs(filename, list)
 *   ws_merge_idt(filename, list)
 *   ws_merge_lst(filename, list)
 *   ws_merge_scl(filename, list)
 *   ws_merge_tbl(filename, list)
 *   ws_merge_var(filename, list)
 *
 *   ws_extrapolate(...)
 *
 *   ws_extrapolate
 *   ws_seasonadj
 *   ws_seasadj
 *   ws_trend
 *   ws_trendstd
 *   ws_import
 *   ws_export
 */

// TODO: rewrite IodeContents with regex or a better algorithm because K_grep() is VERY slow for large databases

/*
    r"""Return the names of objects of a given type, satisfying a pattern specification.

    Parameters
    ----------
    pattern: string
        string containing wildcards characters like '*' or '?'.
        Default '*', meaning "all objects".

    objtype: int
        IODE object type (0-6, 0 for comments...)
        Default 6 for

    Returns
    -------
    List[str]
        List of object names

    Examples
    -------
    >>> iode.ws_load_cmt("../fun.cmt")
    >>> result = iode.ws_content("ACA*", 0)
    >>> print(result)
    ['ACAF', 'ACAG']
    """
*/
inline std::vector<std::string> ws_content(const std::string& pattern = "*", const int iode_type = I_VARIABLES)
{
    if(iode_type < 0 || iode_type > I_VARIABLES)
        throw std::invalid_argument("Invalid value " + std::to_string(iode_type) + " for the argument " +
                                    "'iode_type'.\nThe value must be in range [0, " + 
                                    std::to_string(I_VARIABLES) + "]");

    char** cnt = IodeContents(const_cast<char*>(pattern.c_str()), iode_type);

    std::vector<std::string> list;
    if (cnt == NULL)
        return list;
    
    int nb = 0;
    std::string elem;
    while(cnt[nb] != NULL)
    {
        elem = std::string(cnt[nb]);
        list.push_back(elem);
        nb++;
    }
    return list;
}

// Clear WS

inline void ws_clear_all()
{
    Comments.clear();
    Equations.clear();
    Identities.clear();
    Lists.clear();
    Scalars.clear();
    Tables.clear();
    Variables.clear();
}

inline void ws_clear(const int iode_type)
{
    switch (iode_type)
    {
    case I_COMMENTS:
        Comments.clear();
        break;
    case I_EQUATIONS:
        Equations.clear();
        break;
    case I_IDENTITIES:
        Identities.clear();
        break;
    case I_LISTS:
        Lists.clear();
        break;
    case I_SCALARS:
        Scalars.clear();
        break;
    case I_TABLES:
        Tables.clear();
        break;
    case I_VARIABLES:
        Variables.clear();
        break;    
    default:
        throw std::invalid_argument("Invalid value " + std::to_string(iode_type) + " for the argument " +
                                    "'iode_type'.\nThe value must be in range [0, " + 
                                    std::to_string(I_VARIABLES) + "]");
        break;
    }
}

// Load WS

inline int ws_load(const std::string& filename, const int iode_type = -1)
{
    int iode_type_ = iode_type;

    if(iode_type_ < 0)
        iode_type_ = (int) get_iode_file_type(filename);

    switch (iode_type_)
    {
    case I_COMMENTS:
        Comments.load(filename);
        return Comments.count();
        break;
    case I_EQUATIONS:
        Equations.load(filename);
        return Equations.count();
        break;
    case I_IDENTITIES:
        Identities.load(filename);
        return Identities.count();
        break;
    case I_LISTS:
        Lists.load(filename);
        return Lists.count();
        break;
    case I_SCALARS:
        Scalars.load(filename);
        return Scalars.count();
        break;
    case I_TABLES:
        Tables.load(filename);
        return Tables.count();
        break;
    case I_VARIABLES:
        Variables.load(filename);
        return Variables.count();
        break;
    default:
        throw std::invalid_argument("Invalid value " + std::to_string(iode_type) + " for the argument " +
                                    "'iode_type'.\nThe value must be in range [0, " + 
                                    std::to_string(I_VARIABLES) + "]");
        break;
    }
    return -1;
}

// Save WS

inline void ws_save(const std::string& filename, const int iode_type)
{
    switch (iode_type)
    {
    case I_COMMENTS:
        Comments.save(filename);
        break;
    case I_EQUATIONS:
        Equations.save(filename);
        break;
    case I_IDENTITIES:
        Identities.save(filename);
        break;
    case I_LISTS:
        Lists.save(filename);
        break;
    case I_SCALARS:
        Scalars.save(filename);
        break;
    case I_TABLES:
        Tables.save(filename);
        break;
    case I_VARIABLES:
        Variables.save(filename);
        break;    
    default:
        throw std::invalid_argument("Invalid value " + std::to_string(iode_type) + " for the argument " +
                                    "'iode_type'.\nThe value must be in range [0, " + 
                                    std::to_string(I_VARIABLES) + "]");
        break;
    }
}


// High to Low

inline void ws_htol(const std::string& filename, const std::string& varlist, const int series_type)
{
    if(series_type < 0 || series_type > EnumIodeHtoL::HTOL_SUM)
        throw std::invalid_argument("The value " + std::to_string(series_type) + " is invalid.\n" + 
                                    "The value must be in range [0, " + std::to_string(EnumIodeHtoL::HTOL_SUM) + "]");

    high_to_low((EnumIodeHtoL) series_type, filename, varlist);
}

inline void ws_htol(const std::string& filename, const std::vector<std::string>& varlist, const int series_type)
{        
    std::string s_varlist = boost::algorithm::join(varlist, " ");
    ws_htol(filename, s_varlist, series_type);
}


// Low to High

inline void ws_ltoh(const std::string& filename, const std::string& varlist, const int series_type, const char method)
{
    if(series_type < 0 || series_type > EnumIodeLtoH::LTOH_FLOW)
    throw std::invalid_argument("The value " + std::to_string(series_type) + " is invalid.\n" + 
                                "The value must be in range [0, " + std::to_string(EnumIodeLtoH::LTOH_FLOW) + "]");

    low_to_high((EnumIodeLtoH) series_type, method, filename, varlist);
}

inline void ws_ltoh(const std::string& filename, const std::vector<std::string>& varlist, const int series_type, const char method)
{
    std::string s_varlist = boost::algorithm::join(varlist, " ");
    ws_ltoh(filename, s_varlist, series_type, method);
}


#pragma once

#include "../api/iode.h"
#include "utils.h"

#include <QString>

#include <string>
#include <stdexcept>
#include <stdlib.h>

/* I hope to one day fully rewrite the IODE API into full pure C++ code instead of C but,
   in the meantime, we need bridge functions here
*/

inline int get_nb_elements_WS(EnumIodeType type)
{
    return K_WS[K_CMT + type]->k_nb;
}

inline char* get_iode_object_name(EnumIodeType type, int pos)
{
    return const_cast<char*>(K_WS[K_CMT + type]->k_objs[pos].o_name);
}

inline char* get_iode_object_value(EnumIodeType type, int pos)
{
    return K_oval0(K_WS[K_CMT + type], pos);
}

inline char* get_comment_name(int pos)
{
    return get_iode_object_name(Comments, pos);
}

inline char* get_comment_value(int pos)
{
    return get_iode_object_value(Comments, pos);
}


inline void CPP_WsLoad(std::string arg, EnumIodeType type, std::string str_type)
{
    char* c_arg = const_cast<char*>(arg.c_str());

    int res = B_WsLoad(c_arg, type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to import " + str_type + " from file " + arg);
}


inline void CPP_WsSave(std::string arg, EnumIodeType type, std::string str_type)
{
    char* c_arg = const_cast<char*>(arg.c_str());

    int res = B_WsSave(c_arg, type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to save " + str_type + " to file " + arg);
}


inline void CPP_XodeRuleImport(EnumIodeType type, std::string trace, std::string rule, std::string ode, std::string asc, std::string from, std::string to, int fmt, int lang)
{
    char* c_trace = const_cast<char*>(trace.c_str());
    char* c_rule = const_cast<char*>(rule.c_str());
    char* c_ode = const_cast<char*>(ode.c_str());
    char* c_asc = const_cast<char*>(asc.c_str());
    char* c_from = const_cast<char*>(from.c_str());
    char* c_to = const_cast<char*>(to.c_str());

    int res = X_RuleImport(type, c_trace, c_rule, c_ode, c_asc, c_from, c_to, fmt, lang);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to import file " + asc);
}


inline void CPP_X_RuleExport(std::string trace, std::string rule, std::string out, std::string vfile, std::string cfile, std::string from, std::string to,
    std::string na, std::string sep, int fmt)
{
    char* c_trace = const_cast<char*>(trace.c_str());
    char* c_rule = const_cast<char*>(rule.c_str());
    char* c_out = const_cast<char*>(out.c_str());
    char* c_vfile = const_cast<char*>(vfile.c_str());
    char* c_cfile = const_cast<char*>(cfile.c_str());
    char* c_from = const_cast<char*>(from.c_str());
    char* c_to = const_cast<char*>(to.c_str());
    char* c_na = const_cast<char*>(na.c_str());
    char* c_sep = const_cast<char*>(sep.c_str());

    int res = X_RuleExport(c_trace, c_rule, c_out, c_vfile, c_cfile, c_from, c_to, c_na, c_sep, fmt);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to export file " + vfile);
}




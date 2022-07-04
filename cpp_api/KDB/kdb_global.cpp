#pragma once
#include "kdb_global.h"


void load_global_kdb(EnumIodeType iode_type, std::string& filepath)
{
    filepath = check_filepath(filepath, iode_type, "load", true);

    char* c_filepath = const_cast<char*>(filepath.c_str());

    int res = B_WsLoad(c_filepath, iode_type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to import " + vIodeTypes[iode_type] + " from file " + filepath);
}

void save_global_kdb(EnumIodeType iode_type, std::string& filepath)
{
    filepath = check_filepath(filepath, iode_type, "save", false);

    char* c_filepath = const_cast<char*>(filepath.c_str());

    int res = B_WsSave(c_filepath, iode_type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to save " + vIodeTypes[iode_type] + " to file " + filepath);
}

void clear_global_kdb(EnumIodeType iode_type)
{
    int res = B_WsClear("", iode_type);
    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Something went wrong when trying to clear objets of type " + vIodeTypes[iode_type]);
}

/**
* Warning: renturned char** array must be freed by the caller
*/
char** filter_kdb_names(const EnumIodeType iode_type, const std::string& pattern)
{
    char** c_names;
    if (pattern.empty()) return NULL;
    else
    {
        char* c_pattern = const_cast<char*>(pattern.c_str());
        // Retrieves all object names matching one or more patterns in K_WS (similar to grep)
        char* c_lst = K_expand(iode_type, NULL, c_pattern, '*');
        // Parses a string and replaces @filename and $listname by their contents
        return B_ainit_chk(c_lst, NULL, 0);
    }
}

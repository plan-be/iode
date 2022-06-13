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

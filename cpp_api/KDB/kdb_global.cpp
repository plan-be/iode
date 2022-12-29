#pragma once
#include "kdb_global.h"


void load_global_kdb(EnumIodeType iode_type, std::string& filepath)
{
    filepath = check_filepath(filepath, (EnumIodeFile) iode_type, "load", true);

    char* c_filepath = to_char_array(filepath);

    int res = B_WsLoad(c_filepath, iode_type);
    if (res != EXIT_SUCCESS)
        throw IodeExceptionFunction("Cannot load " + vIodeTypes[iode_type] + "s from file " + filepath, "Unknown");
}

void save_global_kdb(EnumIodeType iode_type, std::string& filepath)
{
    filepath = check_filepath(filepath, (EnumIodeFile) iode_type, "save", false);

    char* c_filepath = to_char_array(filepath);

    int res = B_WsSave(c_filepath, iode_type);
    if (res != EXIT_SUCCESS)
        throw IodeExceptionFunction("Cannot save " + vIodeTypes[iode_type] + "s to file " + filepath, "Unknown");
}

void clear_global_kdb(EnumIodeType iode_type)
{
    int res = B_WsClear("", iode_type);
    if (res != EXIT_SUCCESS)
        throw IodeExceptionFunction("Cannot clear " + vIodeTypes[iode_type] + "s database", "Unknown");
}

/**
* Warning: renturned char** array must be freed by the caller
*/
char** filter_kdb_names(const EnumIodeType iode_type, const std::string& pattern)
{
    if (pattern.empty()) return NULL;
    else
    {
        char* c_pattern = to_char_array(pattern);
        // Retrieves all object names matching one or more patterns in K_WS (similar to grep)
        char* c_lst = K_expand(iode_type, NULL, c_pattern, '*');
        // Parses a string and replaces @filename and $listname by their contents
        char** c_names = B_ainit_chk(c_lst, NULL, 0);
        // remove duplicates
        c_names = remove_duplicates(c_names);
        // return names
        return c_names; 
    }
}

// WARNING: copying content of k_nameptr requires -Zp1 compiler option 
KDB* hard_copy_kdb(KDB* source_kdb, char** names)
{
    short iode_type = source_kdb->k_type;
    int nb_objs = SCR_tbl_size((unsigned char**) names);

    // K_create() sets:
    // - k_arch = #define ARCH
    // - k_type = first argument of K_create()
    // - k_mode = second argument of K_create()
    // - k_nameptr = "ws"
    KDB* dest_kdb = K_create(iode_type, source_kdb->k_mode);
    if (dest_kdb == NULL) return NULL;
    // copy char k_desc[K_MAX_DESC]
    strncpy(dest_kdb->k_desc, source_kdb->k_desc, K_MAX_DESC);
    // copy char k_data[K_MAX_DESC]
    memcpy(dest_kdb->k_data, source_kdb->k_data, sizeof(char) * K_MAX_DESC);
    // copy char k_compressed
    dest_kdb->k_compressed = source_kdb->k_compressed;
    // copy char* k_nameptr
    dest_kdb->k_nameptr = copy_char_array(source_kdb->k_nameptr);
    // copy k_magic[LMAGIC]
    memcpy(dest_kdb->k_magic, source_kdb->k_magic, sizeof(char) * LMAGIC);
    // copy k_oname[OK_MAX_FILE]  
    strncpy(dest_kdb->k_oname, source_kdb->k_oname, OK_MAX_FILE);

    int pos_dest_kdb;
    char* c_obj_name;
    for (int i=0; i<nb_objs; i++)
    {
        c_obj_name = names[i];
        pos_dest_kdb = K_dup(source_kdb, c_obj_name, dest_kdb, c_obj_name);
        if (pos_dest_kdb < 0)
        {
            std::string obj_name = std::string(c_obj_name);
            std::string action = "Trying to copy " + vIodeTypes[iode_type] + " named " + obj_name + " in new database";
            if (pos_dest_kdb == -1) throw IodeExceptionFunction(action, "Object with name " + 
                std::string(obj_name) + " does not exist in the source database");
            else throw IodeExceptionFunction(action, "Unknown");
        }
    }

    return dest_kdb;
}

// Replaced by K_get_kdb_nameptr()
// 
// std::string get_kdb_filename(KDB* kdb)
// {
//     return std::string(kdb->k_nameptr);
// }

void set_kdb_filename(KDB* kdb, const std::string& filename)
{
    K_set_kdb_name(kdb, (unsigned char*) filename.c_str());
}

void low_to_high(const EnumIodeLtoH type, const char method, std::string& filepath, const std::string& var_list)
{
    int res;

    check_filepath(filepath, I_VARIABLES_FILE, "low_to_high", true);
    std::string method_name = mLowToHigh.at(method);

    std::string arg = std::string(1, method) + " " + filepath + " " + var_list;
    char* c_arg = to_char_array(arg);

    if (type == LTOH_STOCK) res = B_WsLtoHStock(c_arg);
    else res = B_WsLtoHFlow(c_arg);

    if (res < 0)
    {
        IodeExceptionFunction error("Cannot Cannot transforms low periodicity variables into high periodicity variables");
        error.add_argument("type", (type == LTOH_STOCK) ? "Stock" : "Flow");
        error.add_argument("method", method_name);
        error.add_argument("file", filepath);
        error.add_argument("variables list", var_list);
        throw error;
    }
}

void high_to_low(const EnumIodeHtoL type, std::string& filepath, const std::string& var_list)
{
    int res;
    std::string type_name; 

    check_filepath(filepath, I_VARIABLES_FILE, "high_to_low", true);

    std::string arg = filepath + " " + var_list;
    char* c_arg = to_char_array(arg);

    switch (type)
    {
    case HTOL_LAST:
        res = B_WsHtoLLast(c_arg);
        type_name = "Last";
        break;
    case HTOL_MEAN:
        res = B_WsHtoLMean(c_arg);
        type_name = "Mean";
        break;
    case HTOL_SUM:
        res = B_WsHtoLSum(c_arg);
        type_name = "Sum";
        break;
    default:
        break;
    }

    if (res < 0)
    {
        IodeExceptionFunction error("Cannot transforms high periodicity variables into low periodicity variables");
        error.add_argument("type", type_name);
        error.add_argument("file", filepath);
        error.add_argument("variables list", var_list);
        throw error;
    }
}

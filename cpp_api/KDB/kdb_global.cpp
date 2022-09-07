#pragma once
#include "kdb_global.h"


void load_global_kdb(EnumIodeType iode_type, std::string& filepath)
{
    filepath = check_filepath(filepath, iode_type, "load", true);

    char* c_filepath = const_cast<char*>(filepath.c_str());

    int res = B_WsLoad(c_filepath, iode_type);
    if (res != EXIT_SUCCESS)
        throw IodeExceptionFunction("Cannot load " + vIodeTypes[iode_type] + "s from file " + filepath, "Unknown");
}

void save_global_kdb(EnumIodeType iode_type, std::string& filepath)
{
    filepath = check_filepath(filepath, iode_type, "save", false);

    char* c_filepath = const_cast<char*>(filepath.c_str());

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
        char* c_pattern = const_cast<char*>(pattern.c_str());
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

static int copy_item_kdb(KDB* source_kdb, KDB* dest_kdb, const int pos_source_kdb, char* obj_name)
{
    int pos;
    short iode_type = source_kdb->k_type;

    switch (iode_type)
    {
    case I_COMMENTS:
        char* comment;
        comment = (char*) KCVAL(source_kdb, pos_source_kdb);
        pos = K_add(dest_kdb, obj_name, comment);
        break;
    case I_EQUATIONS:
        EQ* eq;
        eq = KEVAL(source_kdb, pos_source_kdb);
        pos = K_add(dest_kdb, obj_name, eq, obj_name);
        break;
    case I_IDENTITIES:
        char* lec;
        lec = (char*) KILEC(source_kdb, pos_source_kdb);
        pos = K_add(dest_kdb, obj_name, lec);
        break;
    case I_LISTS:
        char* list;
        list = (char*) KLVAL(source_kdb, pos_source_kdb);
        pos = K_add(dest_kdb, obj_name, list);
        break;
    case I_SCALARS:
        SCL* scalar;
        scalar = KSVAL(source_kdb, pos_source_kdb);
        pos = K_add(dest_kdb, obj_name, scalar);
        break;
    case I_TABLES:
        TBL* table;
        table = KTVAL(source_kdb, pos_source_kdb);
        pos = K_add(dest_kdb, obj_name, table);
        break;
    case I_VARIABLES:
    {
        int nb_objs = KSMPL(KV_WS)->s_nb;                  
        IODE_REAL* var = KVVAL(source_kdb, pos_source_kdb, 0);
        pos = K_add(dest_kdb, obj_name, var, &nb_objs);
        break;
    }
    default:
        break;
    }

    return pos;
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
    // copy char k_desc[K_MAX_DESC]
    memcpy(KDESC(dest_kdb), KDESC(source_kdb), K_MAX_DESC * sizeof(char));
    // copy char k_data[K_MAX_DESC]
    memcpy(KDATA(dest_kdb), KDATA(source_kdb), K_MAX_DESC * sizeof(char));
    // copy char k_compressed  
    dest_kdb->k_compressed = source_kdb->k_compressed;
    // copy char* k_nameptr from global KDB
    int nameptr_size = strlen(source_kdb->k_nameptr) + 1;
    dest_kdb->k_nameptr = new char[nameptr_size];
    strncpy(dest_kdb->k_nameptr, source_kdb->k_nameptr, nameptr_size);

    // QUESTION FOR JMP: is it necessary to copy k_magic, k_oname and k_reserved ?    

    int pos_source_kdb;
    int pos_dest_kdb;
    char* obj_name;
    for (int i=0; i<nb_objs; i++)
    {
        obj_name = names[i];
        pos_source_kdb = K_find(source_kdb, obj_name);
        if (pos_source_kdb < 0) return NULL;
        pos_dest_kdb = copy_item_kdb(source_kdb, dest_kdb, pos_source_kdb, obj_name);
        if (pos_dest_kdb < 0) return NULL;
    }

    return dest_kdb;
}

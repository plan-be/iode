#pragma once
#include "kdb_abstract.h"


KDBAbstract::KDBAbstract(const EnumIodeType iode_type, const std::string& pattern, const bool shallow_copy) : 
    iode_type(iode_type), iode_type_name(iode_type_name)
{
    if (K_WS[iode_type] == NULL) IodeInit();
    cpp_assign_super_API();

    IodeExceptionInitialization error(iode_type_name);
    error.add_argument("IODE type name (number): ", iode_type_name + " (" + std::to_string(iode_type) + ")");
    error.add_argument("pattern: ", pattern);
    error.add_argument("shallow copy? : ", std::to_string(shallow_copy));

    if (pattern.empty())
    {
        kdb_type = KDB_GLOBAL;
        local_kdb = NULL;
    }
    else
    {
        char** c_names = filter_kdb_names(iode_type, pattern);
        if (shallow_copy)
        {
            kdb_type = KDB_SHALLOW_COPY;
            local_kdb = K_quick_refer(K_WS[iode_type], c_names);
        } 
        else
        {
            kdb_type = KDB_LOCAL;
            local_kdb = hard_copy_kdb(K_WS[iode_type], c_names);
        }   
        SW_nfree(c_names);
        if (local_kdb == NULL)
        {
            IodeExceptionInvalidArguments error("Cannot extract subset of KDB of " + iode_type_name + "s");
            error.add_argument("filter pattern", pattern);
            throw error;
        }
    }
}

KDBAbstract::~KDBAbstract()
{
    // frees a KDB and its contents.
    if (kdb_type == KDB_LOCAL) K_free(local_kdb);
    // the KDB struct is deleted (freed) but not the objects.
    if(kdb_type == KDB_SHALLOW_COPY) K_free_kdb(local_kdb);
}

// object name

int KDBAbstract::set_name(const int pos, const std::string& new_name)
{
    std::string old_name = get_name(pos);
    int new_pos = rename(old_name, new_name);
    return new_pos;
}

int KDBAbstract::rename(const std::string& old_name, const std::string& new_name)
{
    if (iode_type == I_EQUATIONS) throw IodeExceptionFunction("Cannot rename an equation",  
        "The name of an equation is always its endogeneous variable");

    check_name(new_name, iode_type);

    char* c_old_name = const_cast<char*>(old_name.c_str());
    char* c_new_name = const_cast<char*>(new_name.c_str());
    
    int pos;
    switch (kdb_type)
    {
    case KDB_GLOBAL:
        pos = K_ren(K_WS[iode_type], c_old_name, c_new_name);
        break;
    case KDB_LOCAL:
        pos = K_ren(local_kdb, c_old_name, c_new_name);
        break;
    case KDB_SHALLOW_COPY:
        // first rename in global KDB
        pos = K_ren(K_WS[iode_type], c_old_name, c_new_name);
        if (pos < 0) break;
        // then rename in local KDB
        pos = K_ren(local_kdb, c_old_name, c_new_name);
        break;
    default:
        break;
    }

    // see K_ren documentation
    if (pos < 0)
    {
        IodeExceptionFunction error("Cannot rename " + old_name + " as " + new_name);
        std::string msg = iode_type_name + " cannot be renamed as " + new_name + ".\n";
        if (pos == -1) error.set_reason("Name " + old_name + " does not exist.");
        else if (pos == -2) error.set_reason(iode_type_name + " with name " + new_name + " already exists.");
        else error.set_reason("Unknown");
        throw error;
    }

    return pos;
}

// delete

void KDBAbstract::remove(const int pos)
{
    // throw exception if object with passed position is not valid
    std::string name = get_name(pos);
    remove(name);
}

void KDBAbstract::remove(const std::string& name)
{
    // throw exception if object with passed name does not exist
    int pos = get_position(name);

    switch (kdb_type)
    {
    case KDB_GLOBAL:
        K_del(K_WS[iode_type], pos);
        break;
    case KDB_LOCAL:
        K_del(local_kdb, pos);
        break;
    case KDB_SHALLOW_COPY:
        char* c_name;
        // first delete in shallow copy KDB
        K_del_entry(local_kdb, pos);
        // then delete in global KDB
        c_name = const_cast<char*>(name.c_str());
        pos = K_find(K_WS[iode_type], c_name);
        K_del(K_WS[iode_type], pos);
        break;
    default:
        throw IodeExceptionFunction("Cannot remove " + iode_type_name + " with name " + name, 
            "Wrong value " + std::to_string(kdb_type) + " for class member kdb_type of KDBAbstract");
        break;
    }
}

// Other methods

void KDBAbstract::dump(std::string& filepath)
{
    filepath = check_filepath(filepath, iode_type, "save", false);
    char* c_filepath = const_cast<char*>(filepath.c_str());
    if (strlen(c_filepath) >= sizeof(FNAME)) throw IodeExceptionFunction("Cannot save " + iode_type_name + "s",  
        "Filepath " + filepath + " is too long");

    int res = B_WsDump(get_KDB(), c_filepath);
    if (res != EXIT_SUCCESS)
        throw IodeExceptionFunction("Cannot save " + iode_type_name + "s in file " + filepath, "Unknown");
}

void KDBAbstract::clear()
{
    int res;
    res = B_WsDescr("", iode_type);
    res += B_WsName("", iode_type);

    res += K_clear(get_KDB());
    if (res != EXIT_SUCCESS) throw IodeExceptionFunction("Cannot clear " + iode_type_name + "s", "Unknown");
}

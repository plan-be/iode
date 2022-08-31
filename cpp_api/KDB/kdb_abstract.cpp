#pragma once
#include "kdb_abstract.h"


KDBAbstract::KDBAbstract(const EnumIodeType iode_type, const std::string& pattern) : 
    iode_type(iode_type), iode_type_name(iode_type_name), shallow_copy_kdb(nullptr)
{
    if (K_WS[iode_type] == NULL) IodeInit();
    cpp_assign_super_API();

    if (pattern.empty()) shallow_copy_kdb = NULL;
    else
    {
        char** c_names = filter_kdb_names(iode_type, pattern);
        shallow_copy_kdb = K_quick_refer(K_WS[iode_type], c_names);
        SW_nfree(c_names);
        if (shallow_copy_kdb == NULL)
        {
            IodeExceptionInvalidArguments error("Cannot extract subset of KDB of " + iode_type_name + "s");
            error.add_argument("filter pattern", pattern);
            throw error;
        }
    }
}

KDBAbstract::~KDBAbstract()
{
    if (shallow_copy_kdb) K_free_kdb(shallow_copy_kdb);
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
    
    // first rename in global KDB
    int pos = K_ren(K_WS[iode_type], c_old_name, c_new_name);
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
    
    // rename in shallow copy KDB
    if (shallow_copy_kdb) pos = K_ren(shallow_copy_kdb, c_old_name, c_new_name);

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

    if (shallow_copy_kdb)
    {
        K_del_entry(shallow_copy_kdb, pos);
        char* c_name = const_cast<char*>(name.c_str());
        pos = K_find(K_WS[iode_type], c_name);
    }
    K_del(K_WS[iode_type], pos);
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

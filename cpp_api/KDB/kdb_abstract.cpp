#pragma once
#include "kdb_abstract.h"


KDBAbstract::KDBAbstract(std::string& filepath) : kdb_type(KDB_GLOBAL)
{
    EnumIodeFile file_type = get_iode_file_type(filepath);
    if(file_type > I_VARIABLES_FILE)
    {
        std::filesystem::path p_filepath(filepath);
        std::string ext = p_filepath.extension().string();
        std::string msg = "Expected file with extension ";
        msg += boost::algorithm::join(v_binary_ext, ", ");
        msg += boost::algorithm::join(v_ascii_ext, ", ");
        msg += "\nBut got file with extension " + ext;
        throw IodeExceptionInitialization("IODE database", msg);
    } 

    iode_type = (EnumIodeType) file_type;
    iode_type_name = vIodeTypes[iode_type];

    if (K_WS[iode_type] == NULL) IodeInit();
    cpp_assign_super_API();

    local_kdb = NULL;
    load_global_kdb(iode_type, filepath);
}


KDBAbstract::KDBAbstract(const EnumIodeKDBType kdb_type, const EnumIodeType iode_type, const std::string& pattern) : 
    kdb_type(kdb_type), iode_type(iode_type)
{
    if (K_WS[iode_type] == NULL) IodeInit();
    cpp_assign_super_API();

    iode_type_name = vIodeTypes[iode_type];

    IodeExceptionInitialization error(iode_type_name);
    error.add_argument("IODE type name (number): ", iode_type_name + " (" + std::to_string(iode_type) + ")");
    error.add_argument("pattern: ", pattern);
    error.add_argument("shallow copy? : ", kdb_type == KDB_SHALLOW_COPY ? "Yes" : "No");

    if (kdb_type == KDB_GLOBAL)
    {
        local_kdb = NULL;
    }
    else
    {
        char** c_names = filter_kdb_names(iode_type, pattern);
        if (kdb_type == KDB_SHALLOW_COPY)
            local_kdb = K_quick_refer(K_WS[iode_type], c_names);
        else
            local_kdb = hard_copy_kdb(K_WS[iode_type], c_names); 
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

    if (count() == 0) return -1;

    check_name(new_name, iode_type);

    char* c_old_name = to_char_array(old_name);
    char* c_new_name = to_char_array(new_name);
    
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
    if (pos < 0) return;

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
        c_name = to_char_array(name);
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

void KDBAbstract::merge(const KDBAbstract& other, const bool overwrite)
{
    KDB* kdb = get_KDB();
    KDB* other_kdb = other.get_KDB();
    if(kdb == NULL || other_kdb == NULL) throw IodeExceptionFunction("Cannot merge two " + 
        iode_type_name + " databases", "At least one of the two databases is null");
    int res = K_merge(kdb, other_kdb, overwrite ? 1 : 0);
    if (res < 0) throw IodeExceptionFunction("Cannot merge two databases", "Unknown");
}

// TODO JMP: please provide input values to test B_WsCopy()
void KDBAbstract::copy_into(const std::string& input_file, const std::string objects_names)
{
    std::string buf = input_file + " ";
    if(kdb_type == I_VARIABLES)
    {
        Sample smpl = KSMPL(get_KDB());
        std::string from = smpl.start_period().to_string();
        std::string to = smpl.end_period().to_string();
        buf += from + " " + to + " ";
    }
    buf += objects_names;

    int res = B_WsCopy(const_cast<char*>(buf.c_str()), kdb_type);

    if (res < 0) B_display_last_error();
}

// TODO JMP: please provide input values to test B_WsMerge()
void KDBAbstract::merge_into(std::string& input_file)
{
    KDB* kdb = get_KDB();
    if (kdb == NULL) return;

    input_file = check_filepath(input_file, (EnumIodeFile) iode_type, "merge_into", true);
    
    int res = K_cat(kdb, const_cast<char*>(input_file.c_str()));
    if (res < 0) throw IodeExceptionFunction("Cannot merge content of file " + input_file + 
        " in the " + vIodeTypes[iode_type] + " database");
}

void KDBAbstract::dump(std::string& filepath)
{
    if (count() == 0) return;

    filepath = check_filepath(filepath, (EnumIodeFile) iode_type, "save", false);
    char* c_filepath = to_char_array(filepath);
    if (strlen(c_filepath) >= sizeof(FNAME)) throw IodeExceptionFunction("Cannot save " + iode_type_name + "s",  
        "Filepath " + filepath + " is too long");

    int res = B_WsDump(get_KDB(), c_filepath);
    if (res != EXIT_SUCCESS)
        throw IodeExceptionFunction("Cannot save " + iode_type_name + "s in file " + filepath, "Unknown");
}

void KDBAbstract::clear()
{
    KDB* kdb = get_KDB();
    if (kdb == NULL) return;

    int res;
    res = B_WsDescr("", iode_type);
    res += B_WsName("", iode_type);

    res += K_clear(kdb);
    if (res != EXIT_SUCCESS) throw IodeExceptionFunction("Cannot clear " + iode_type_name + "s", "Unknown");
}

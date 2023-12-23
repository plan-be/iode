#pragma once
#include "kdb_abstract.h"


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
        char** c_names = filter_names_from_global_db(pattern);
        if (kdb_type == KDB_SHALLOW_COPY)
            local_kdb = K_quick_refer(K_WS[iode_type], c_names);
        else
            local_kdb = hard_copy_kdb(K_WS[iode_type], c_names); 
        SCR_free_tbl((unsigned char**) c_names);
        if (local_kdb == NULL)
        {
            IodeExceptionInvalidArguments error("Cannot extract subset of KDB of " + iode_type_name + "s");
            error.add_argument("filter pattern", pattern);
            throw error;
        }
    }
}

KDBAbstract::KDBAbstract(const KDBAbstract& kdb_to_copy)
{
    iode_type = kdb_to_copy.iode_type;
    iode_type_name = kdb_to_copy.iode_type_name;
    kdb_type = kdb_to_copy.kdb_type;
    int nb_names = kdb_to_copy.count();
    char** c_names;
    switch (kdb_type)
    {
    case KDB_GLOBAL:
        local_kdb = nullptr;
        break;
    case KDB_LOCAL:
        local_kdb = hard_copy_kdb(kdb_to_copy.local_kdb);
        break;
    case KDB_SHALLOW_COPY:
        c_names = new char*[nb_names+1];
        for(int i=0; i < nb_names; i++)
            c_names[i] = KONAME(kdb_to_copy.local_kdb, i);
        c_names[nb_names] = NULL;
        local_kdb = K_quick_refer(K_WS[iode_type], c_names);
        break;
    default:
        break;
    }
}

KDBAbstract::~KDBAbstract()
{
    // frees a KDB and its contents.
    if (kdb_type == KDB_LOCAL) K_free(local_kdb);
    // the KDB struct is deleted (freed) but not the objects.
    if(kdb_type == KDB_SHALLOW_COPY) K_free_kdb(local_kdb);
}

// private methods

char** KDBAbstract::filter_names_from_global_db(const std::string& pattern) const
{
    char* c_pattern = to_char_array(pattern);
    // Retrieves all object names matching one or more patterns in K_WS (similar to grep)
    char* c_lst = K_expand(this->iode_type, NULL, c_pattern, '*');
    // Parses a string and replaces @filename and $listname by their contents
    char** c_names = B_ainit_chk(c_lst, NULL, 0);
    // remove duplicates
    c_names = remove_duplicates(c_names);
    // return names
    return c_names; 
}

// object name

std::vector<std::string> KDBAbstract::get_names(const std::string& pattern, const bool must_exist) const
{
    std::vector<std::string> v_names;
    if(pattern.empty())
    {
        for (int i=0; i < count(); i++) 
            v_names.push_back(get_name(i));
        return v_names;
    }
    else
    {
        char** c_names = filter_names_from_global_db(pattern);
        if(c_names == NULL)
            return v_names;

        int nb_names = SCR_tbl_size((unsigned char **) c_names);
        std::string name;
        for(int i=0; i < nb_names; i++)
        {
            name = std::string(c_names[i]);
            if(must_exist && !contains(name))
                continue;
            v_names.push_back(name);
        }

        SCR_free_tbl((unsigned char **) c_names);
    }

    return v_names;
}

std::string KDBAbstract::get_names_as_string(const std::string& pattern, const bool must_exist) const
{
    std::string names;
    if(pattern.empty())
    {
        for(int i=0; i < count(); i++) 
            names += get_name(i) + ";";
    }
    else
    {
        std::vector<std::string> v_names = get_names(pattern, must_exist);
        for(const std::string& name: v_names)
            names += std::string(name) + ";";
    }

    // remove last ;
    if(!names.empty()) 
        names.pop_back();

    return names;
}

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
void KDBAbstract::merge_into(const std::string& input_file)
{
    KDB* kdb = get_KDB();
    if (kdb == NULL) return;

    std::string input_file_ = check_filepath(input_file, (EnumIodeFile) iode_type, "merge_into", true);
    
    int res = K_cat(kdb, to_char_array(input_file));
    if (res < 0) throw IodeExceptionFunction("Cannot merge content of file " + input_file_ + 
        " in the " + vIodeTypes[iode_type] + " database");
}

// TODO ald: rewrite B_DataSearchParms() in C++
std::vector<std::string> KDBAbstract::get_associated_objects_list(const std::string& name, 
    const EnumIodeType other_type)
{
    char** c_list = B_DataSearchParms(to_char_array(name), 1, 1, 1, 1, 1, (int) other_type);

    std::vector<std::string> objs_list;
    if(c_list == NULL || SCR_tbl_size((unsigned char**) c_list) == 0)
        return objs_list;
    
    for(int i=0; i < SCR_tbl_size((unsigned char**) c_list); i++)
        objs_list.push_back(std::string(c_list[i]));
    return objs_list;
}

void KDBAbstract::load(const std::string& filepath)
{
    if(kdb_type != KDB_GLOBAL)
    {
        std::string msg = "The load method can only be called if the ";
        msg += "Database object represents the global " + iode_type_name + " workspace";
        kwarning(to_char_array(msg));
        return;
    }

    load_global_kdb(iode_type, filepath);
}

void KDBAbstract::save(const std::string& filepath)
{
    if (count() == 0) return;

    std::string filepath_ = check_filepath(filepath, (EnumIodeFile) iode_type, "save", false);
    char* c_filepath = to_char_array(filepath_);
    if (strlen(c_filepath) >= sizeof(FNAME)) throw IodeExceptionFunction("Cannot save " + iode_type_name + "s",  
        "Filepath " + filepath_ + " is too long");

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

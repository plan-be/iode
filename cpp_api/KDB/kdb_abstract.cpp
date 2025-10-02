#pragma once
#include "kdb_abstract.h"
#include "cpp_api/utils/super.h"


KDBAbstract::KDBAbstract(const IodeType iode_type, const std::string& filepath)
{
    cpp_assign_super_API();

	if(K_WS[iode_type] == NULL)
    {
        memset(K_RWS[iode_type], 0, sizeof(K_RWS[iode_type]));
        K_WS[iode_type] = K_RWS[iode_type][0] = K_init_kdb(iode_type, I_DEFAULT_FILENAME);
    }

    k_type = (short) iode_type;

    if(!filepath.empty())
    {
        // throw an error if the passed filepath is not valid
        std::string filepath_ = check_filepath(filepath, (IodeFileType) iode_type, "load", true);
        char* c_filepath = to_char_array(filepath_);

        int res = B_WsLoad(c_filepath, (int) k_type);
        if (res != EXIT_SUCCESS)
            throw std::runtime_error("Cannot load objects for the '" + v_iode_types[iode_type] + 
                                    "' database from the file '" + filepath + "'\nReason: unknown");  
    }
    
    k_db_type = DB_GLOBAL;
}

KDBAbstract::KDBAbstract(KDBAbstract* kdb, const bool deep_copy, const std::string& pattern)
{
    KDB* source_kdb = kdb->get_database(); 
    if(source_kdb == NULL || source_kdb == nullptr)
        throw std::runtime_error("Cannot create a deep copy of the database.\nThe input database is empty");

    // ---- prepare the subset database ----
    k_mode = source_kdb->k_mode;                                        // short
    k_type = source_kdb->k_type;                                        // short
    k_objs = NULL;
    k_arch = ARCH;                                                      // std::string   
    strncpy(k_desc, source_kdb->k_desc, K_MAX_DESC);                    // char[K_MAX_DESC]
    memcpy(k_data, source_kdb->k_data, sizeof(char) * K_MAX_DESC);      // char[K_MAX_DESC]
    k_compressed = source_kdb->k_compressed;                            // char
    k_db_type = source_kdb->k_db_type;                                  // char
    filepath = source_kdb->filepath;                                    // std::string

    std::vector<std::string> names = filter_names_from_database(source_kdb, (IodeType) k_type, pattern);

    std::string error_msg = "Cannot extract a subset of the database of " + v_iode_types[k_type] + ".\n";
    int pos;

    // ---- deep copy of objects ---- 
    if(deep_copy)
    {
        k_db_type = DB_DEEP_COPY;
        k_nb = 0;

        char* c_name;
        for(const std::string& name: names)
        {
            c_name = to_char_array(name);
            pos = K_dup(const_cast<KDB*>(source_kdb), c_name, this, c_name);
            if(pos < 0)
            {
                for(int i = 0; i < k_nb; i++)
                    if(k_objs[i].o_val != 0) SW_free(k_objs[i].o_val);

                SW_nfree(k_objs);
                k_objs = NULL; 

                filepath.clear();

                error_msg += "Cannot to copy " + v_iode_types[k_type] + " named '" + name + "' in the subset.\n";
                if (pos == -1) 
                    error_msg += "Object with name '" + name + "' does not exist in the " + v_iode_types[k_type] + " database.";
                else 
                    error_msg += "Reason: unknown";
                throw std::runtime_error(error_msg);
            }
        }
    }
    // ---- shallow copy of objects ---- 
    // -> same as K_quick_refer() function <-
    else
    {
        k_db_type = DB_SHALLOW_COPY;

        k_nb = (long) names.size();
        k_objs = (KOBJ*) SW_nalloc(sizeof(KOBJ) * K_CHUNCK * (1 + k_nb / K_CHUNCK));

        for(int i = 0 ; i < k_nb; i++) 
        {
            pos = K_find(const_cast<KDB*>(source_kdb), to_char_array(names[i]));
            if(pos < 0) 
            {
                SW_nfree(k_objs);
                k_objs = NULL;

                filepath.clear();

                error_msg += "Object with name '" + names[i] + "' does not exist in the " + v_iode_types[k_type] + " database.";
                throw std::runtime_error(error_msg);
            }
            memcpy(k_objs + i, source_kdb->k_objs + pos, sizeof(KOBJ));
        }

        // Sort entries
        K_sort(this);
    }
}

KDBAbstract::~KDBAbstract()
{
    if(is_global_database())
        return;

    // see K_free() from k_kdb.c
    if(is_local_database())
    {
        for(int i = 0; i < k_nb; i++)
            if(k_objs[i].o_val != 0) SW_free(k_objs[i].o_val);
    }

    SW_nfree(k_objs);
    k_objs = NULL;

    filepath.clear();
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
        KDB* kdb = get_database();
        std::vector<std::string> v_names_ = filter_names_from_database(kdb, (IodeType) k_type, pattern);
        if(v_names_.size() == 0)
            return v_names_;

        for(const std::string& name: v_names_)
        {
            if(must_exist && !contains(name))
                continue;
            v_names.push_back(name);
        }
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
    if (k_type == EQUATIONS) 
        throw std::runtime_error(std::string("Cannot rename an equation.\n") + 
                                 "The name of an equation is always its endogenous variable");

    if(count() == 0) 
        return -1;

    check_name(new_name, k_type);

    char* c_old_name = to_char_array(old_name);
    char* c_new_name = to_char_array(new_name);
    
    int pos;
    KDB* kdb = get_database();
    switch (k_db_type)
    {
    case 0:
        pos = K_ren(K_WS[k_type], c_old_name, c_new_name);
        break;
    case 1:
        pos = K_ren(kdb, c_old_name, c_new_name);
        break;
    case 2:
        // first rename in global KDB
        pos = K_ren(K_WS[k_type], c_old_name, c_new_name);
        if (pos < 0) break;
        // then rename in local KDB
        pos = K_ren(kdb, c_old_name, c_new_name);
        break;
    default:
        break;
    }

    // see K_ren documentation
    if(pos < 0)
    {
        std::string error_msg = "Cannot rename '" + old_name + "' as '" + new_name + "'.\n";
        if(pos == -1) 
            throw std::invalid_argument(error_msg + "There is no object named '" + old_name + "' in the database.");
        else if(pos == -2) 
            throw std::invalid_argument(error_msg + "An object named '" + new_name + "' already exists in the database.");
        else 
            throw std::runtime_error(error_msg + "Reason: unknown");
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
    if(pos < 0) 
        return;

    KDB* kdb = get_database();
    switch(k_db_type)
    {
    case 0:
        K_del(K_WS[k_type], pos);
        break;
    case 1:
        K_del(kdb, pos);
        break;
    case 2: 
        char* c_name;
        // first delete in shallow copy KDB
        K_del_entry(kdb, pos);
        // then delete in global KDB
        c_name = to_char_array(name);
        pos = K_find(K_WS[k_type], c_name);
        K_del(K_WS[k_type], pos);
        break;
    default:
        throw std::invalid_argument("Cannot remove object named '" + name + "'\n."
                                    "Something went wrong.");
        break;
    }
}

// remove only the entry (= handle) at the given position, not the object in memory
void KDBAbstract::remove_entry(const int pos)
{
    // throw exception if object with passed position is not valid
    std::string name = get_name(pos);
    remove_entry(name);
}

// remove only the entry (= handle) at the given position, not the object in memory
void KDBAbstract::remove_entry(const std::string& name)
{
    // throw exception if object with passed name does not exist
    int pos = get_position(name);
    if(pos < 0) 
        return;

    KDB* kdb = get_database();
    K_del_entry(kdb, pos);
}

// Other methods

void KDBAbstract::merge(const KDBAbstract& other, const bool overwrite)
{
    KDB* kdb = get_database();
    KDB* other_kdb = other.get_database();

    if(kdb == NULL || other_kdb == NULL) 
        throw std::runtime_error("Cannot merge the two '" + v_iode_types[k_type] + "' databases.\n" + 
                                 "At least one of the two databases is null");

    int res = K_merge(kdb, other_kdb, overwrite ? 1 : 0);
    if(res < 0) 
        throw std::runtime_error("Cannot merge the two '" + v_iode_types[k_type] + "' databases.\n" + 
                                 "Reason: unknown");
}

// TODO JMP: please provide input values to test B_WsCopy()
void KDBAbstract::copy_from(const std::string& input_file, const std::string objects_names)
{
    std::string buf = input_file + " " + objects_names;
    int res = B_WsCopy(const_cast<char*>(buf.c_str()), k_type);
    if(res < 0)
    {
        std::string last_error = error_manager.get_last_error();
        if(!last_error.empty())
        {
            std::string msg = "Cannot copy the content of file '" + input_file;
            msg += "' into the " + v_iode_types[k_type] + " database.\n" + last_error;
            throw std::runtime_error(msg);
        }
    }
}

// TODO JMP: please provide input values to test B_WsMerge()
void KDBAbstract::merge_from(const std::string& input_file)
{
    KDB* kdb = get_database();
    if(kdb == NULL) 
        return;

    // throw an error if the passed filepath is not valid
    std::string input_file_ = check_filepath(input_file, (IodeFileType) k_type, "merge_from", true);
    
    int res = K_cat(kdb, to_char_array(input_file));
    if(res < 0) 
        throw std::runtime_error("Cannot merge the content of the file '" + input_file_ + "' in the current " + 
                                 v_iode_types[k_type] + " database");
}

// TODO ALD: rewrite B_DataSearchParms() in C++
std::vector<std::string> KDBAbstract::search(const std::string& pattern, const bool word, const bool case_sensitive, 
        const bool in_name, const bool in_formula, const bool in_text, const std::string& list_result)
{
    std::vector<std::string> objs_list;

    int c_word  = word ? 1 : 0;
    int c_ecase = case_sensitive ? 1 : 0;
    int c_names = in_name ? 1 : 0;
    int c_forms = in_formula ? 1 : 0;
    int c_texts = in_text ? 1 : 0;
    char** c_list = B_DataSearchParms(to_char_array(pattern), c_word, c_ecase, c_names, c_forms, c_texts, (int) this->k_type);

    // check if returned list of objects is not null
    if(c_list == NULL || SCR_tbl_size((unsigned char**) c_list) == 0)
        return objs_list;

    // save as IODE list (default to list _RES)
    KL_lst(to_char_array(list_result), c_list, 200);
    
    // convert C table as C++ vector
    for(int i=0; i < SCR_tbl_size((unsigned char**) c_list); i++)
        objs_list.push_back(std::string(c_list[i]));

    // free allocated memory for the returned C table
    SCR_free_tbl((unsigned char**) c_list);

    // return C++ vector
    return objs_list;
}

void KDBAbstract::save(const std::string& filepath, const bool compress)
{
    KDB* kdb = get_database();
    if(kdb == NULL) 
        return;

    if(kdb->k_nb == 0) 
        return;

    // throw an error if the filepath is not valid
    std::string filepath_ = check_filepath(filepath, (IodeFileType) k_type, "save", false);

    char* c_filepath = to_char_array(filepath_);
    if (strlen(c_filepath) >= sizeof(FNAME)) 
        throw std::invalid_argument("Cannot save the '" + v_iode_types[k_type] + "' database.\n" +   
                                    "The filepath '" + filepath_ + "' is too long.");

    // LZH compression
    int klzh = K_LZH;
    K_LZH = 2;

    int res = B_WsDump(kdb, c_filepath);
    K_LZH = klzh;

    if (res != EXIT_SUCCESS)
        throw std::runtime_error("Cannot save the '" + v_iode_types[k_type] + "' database in the file '" + 
                                  filepath + "'.\nReason: unknown.");
}

void KDBAbstract::clear()
{
    KDB* kdb = get_database();
    if(kdb == NULL) 
        return;

    int res;
    res = B_WsDescr("", k_type);
    res += B_WsName("", k_type);

    res += K_clear(kdb);
    if (res != EXIT_SUCCESS) 
        throw std::runtime_error("Cannot clear the '" + v_iode_types[k_type] + "' database.\nReason: unknown");
}

#pragma once
#include "kdb_abstract.h"
#include "cpp_api/utils/super.h"

KDBAbstract::KDBAbstract(const IodeType iode_type, const std::string& filepath)
    : KDB(iode_type, DB_GLOBAL) 
{
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
}

KDBAbstract::KDBAbstract(KDBAbstract* kdb, const bool deep_copy, const std::string& pattern)
    : KDB((IodeType) kdb->k_type, deep_copy ? DB_STANDALONE : DB_SHALLOW_COPY)
{
    if(!kdb)
        throw std::runtime_error("Cannot create a deep copy of the database.\nThe input database is empty");

    KDB* c_kdb = kdb->get_database();
    if(!c_kdb)
        throw std::runtime_error("Cannot create a deep copy of the database.\nThe input database is empty");

    // ---- prepare the subset database ----
    k_mode = c_kdb->k_mode;                                         // short
    k_arch = ARCH;                                                  // std::string   
    description = c_kdb->description;                               // std::string
    if(c_kdb->sample)                                               // Sample*
        sample = new Sample(*c_kdb->sample);
    else
        sample = nullptr;
    k_compressed = c_kdb->k_compressed;                             // char
    filepath = c_kdb->filepath;                                     // std::string

    std::vector<std::string> names = filter_names_from_database(c_kdb, (IodeType) k_type, pattern);

    std::string error_msg = "Cannot extract a subset of the database of " + v_iode_types[k_type] + ".\n";
    
    // ---- deep copy of objects ---- 
    if(deep_copy)
    {
        bool success;
        for(const std::string& name: names)
        {
            success = duplicate(*c_kdb, name, name);
            if(!success)
            {
                for(auto& [name, handle] : k_objs)
                    if(handle > 0) 
                        SW_free(handle);
                k_objs.clear(); 

                filepath.clear();

                throw std::runtime_error("Failed to create a deep copy of the " + v_iode_types[k_type] + 
                                         " database.");
            }
        }
    }
    // ---- shallow copy of objects ---- 
    // -> same as K_quick_refer() function <-
    else
    {
        SWHDL handle;
        for(const std::string& name: names) 
        {
            handle = c_kdb->get_handle(name);
            if(handle == 0) 
            {
                for(auto& [_, _handle_] : k_objs)
                    if(_handle_ > 0) 
                        SW_free(handle);
                k_objs.clear();

                filepath.clear();

                throw std::runtime_error("Failed to create a deep copy of the " + v_iode_types[k_type] + 
                                         " database.");
            }
            k_objs[name] = handle;
        }
    }
}

// object name

std::vector<std::string> KDBAbstract::get_names(const std::string& pattern, const bool must_exist) const
{
    KDB* kdb = get_database();
    
    if(pattern.empty())
        return kdb->get_names();
    
    std::vector<std::string> v_names_ = filter_names_from_database(kdb, (IodeType) k_type, pattern);
    if(v_names_.size() == 0)
    return v_names_;
    
    std::vector<std::string> v_names;
    for(const std::string& name: v_names_)
    {
        if(must_exist && !contains(name))
            continue;
        v_names.push_back(name);
    }

    return v_names;
}

std::string KDBAbstract::get_names_as_string(const std::string& pattern, const bool must_exist) const
{
    KDB* kdb = get_database();
    
    if(pattern.empty())
        return kdb->get_names_as_string();
    
    std::string names;
    std::vector<std::string> v_names = get_names(pattern, must_exist);
    for(const std::string& name: v_names)
        names += std::string(name) + ";";

    // remove last ;
    if(!names.empty() && names.back() == ';')
        names.pop_back();

    return names;
}

bool KDBAbstract::set_name(const int pos, const std::string& new_name)
{
    std::string old_name = get_name(pos);
    bool success = rename(old_name, new_name);
    return success;
}

bool KDBAbstract::rename(const std::string& old_name, const std::string& new_name, const bool overwrite)
{
    bool success;
    KDB* kdb = get_database();
    switch (k_db_type)
    {
    case DB_GLOBAL:
        success = K_WS[k_type]->rename(old_name, new_name, overwrite);
        break;
    case DB_STANDALONE:
        success = kdb->rename(old_name, new_name, overwrite);
        break;
    case 2:
        // first rename in global KDB
        success = K_WS[k_type]->rename(old_name, new_name, overwrite);
        if (!success) 
            break;
        // then rename in local KDB
        success = kdb->rename(old_name, new_name, overwrite);
        break;
    default:
        break;
    }

    return success;
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
    KDB* kdb = get_database();
    switch(k_db_type)
    {
    case DB_GLOBAL:
        K_WS[k_type]->remove(name);
        break;
    case DB_STANDALONE:
        kdb->remove(name);
        break;
    case DB_SHALLOW_COPY:
        // first delete in shallow copy KDB
        kdb->remove(name);
        // then delete in global KDB
        K_WS[k_type]->remove(name);
        break;
    default:
        throw std::invalid_argument("Cannot remove object named '" + name + "'\n."
                                    "Something went wrong.");
        break;
    }
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

    if(kdb->size() == 0) 
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
    kdb->clear();
}

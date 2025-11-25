#pragma once
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"
#include "cpp_api/common.h"
#include "cpp_api/utils/super.h"
#include "cpp_api/KDB/kdb_global.h"

#include <string>
#include <vector>
#include <stdexcept>


// using is the C++11 version of typedef
using Comment = std::string;
using List = std::string;
using Variable = std::vector<double>;


// Defining an abstract class above the template class 
// is required to define an abstract Python class in Cython
struct KDBAbstract
{    
    IodeType iode_type;
    
public:
    KDBAbstract(const IodeType iode_type): iode_type(iode_type) {}

    // ======== methods ========

    virtual IodeDatabaseType get_database_type() const = 0;

    virtual int size() const = 0;

    virtual int get_iode_type() const = 0;

    virtual std::string get_iode_type_str() const = 0;

    virtual bool is_global_database() const = 0;

    virtual bool is_local_database() const = 0;

    virtual bool is_shallow_copy_database() const = 0;

    virtual std::string get_filename() const = 0;

    virtual void set_filename(const std::string& filename) = 0;

    virtual std::string get_description() const = 0;

    virtual void set_description(const std::string& description) = 0;

    virtual int index_of(const std::string& name) const = 0;

    virtual bool contains(const std::string& name) const = 0;

    // object names

    virtual std::string get_name(const int pos) const = 0;

    virtual bool set_name(const int pos, const std::string& new_name) = 0;

    virtual bool rename(const std::string& old_name, const std::string& new_name, 
        const bool overwrite = false) = 0;

    virtual std::string expand(const std::string& pattern, const char all) const = 0;

    virtual std::vector<std::string> filter_names(const std::string& pattern, 
        const bool must_exist=true) const = 0;

    virtual std::vector<std::string> get_names() const = 0;

    virtual std::string get_names_as_string() const = 0;

    // delete

    virtual void remove(const int pos) = 0;
    
    virtual void remove(const std::string& name) = 0;

    // Other methods

    virtual void merge(const KDBAbstract& other, const bool overwrite=true) = 0;

    virtual void copy_from(const std::string& input_file, const std::string objects_names = "*") = 0;
    
    virtual void merge_from(const std::string& input_file) = 0;
    
    virtual std::vector<std::string> search(const std::string& pattern, const bool word=true, const bool case_sensitive=true, 
        const bool in_name=true, const bool in_formula=true, const bool in_text=true, const std::string& list_result="_RES") = 0;

    // load - save - clear

    virtual bool load(const std::string& filepath) = 0;
    
    virtual bool save(const std::string& filepath, const bool compress=false) = 0;
    
    virtual void clear() = 0;
};


template<typename B, typename T> struct KDBTemplate : public KDBAbstract
{    
    B* local_kdb = nullptr;

protected:
    virtual T copy_obj(const T original) const = 0;
    
    virtual T get_unchecked(const std::string& name) const = 0;
    
public:
    // ======== constructors ========

    // global or standalone database
    KDBTemplate(const IodeType iode_type, const bool is_global, const std::string& filepath) 
        : KDBAbstract(iode_type)
    {
        if(!is_global)
            local_kdb = new B(iode_type, false);

        if(filepath.empty())
            return;
        
        // throw an error if the passed filepath is not valid
        std::string _filepath_ = check_filepath(filepath, (IodeFileType) iode_type, "load", true);
        
        bool success = false;
        if(is_global)
        {
            int res = B_WsLoad((char*) _filepath_.c_str(), iode_type);
            success = (res == 0);
        }
        else
        {
            local_kdb->filepath = utf8_to_oem(_filepath_);
            success = local_kdb->load(_filepath_);
        }

        if(!success)
        {
            std::string msg = "Cannot create the " + v_iode_types[iode_type] + 
                            " database from the file '" + _filepath_ + "'";
            throw std::runtime_error(msg);
        }
    }

    // shallow copy database
    KDBTemplate(KDBTemplate* kdb, const std::string& pattern)
        : KDBAbstract(kdb->iode_type)
    {
        B* c_kdb = kdb->get_database();
        this->iode_type = (IodeType) c_kdb->k_type;
        this->local_kdb = new B(c_kdb, pattern);
    }

    ~KDBTemplate()
    {
        if(local_kdb)
        {
            delete local_kdb;
            local_kdb = nullptr;
        }
    }

    B* get_database() const
    {
        B* kdb = nullptr; 
        if(!local_kdb)
        {
            if(iode_type == COMMENTS)
                kdb = global_ws_cmt.get();
            else
                kdb = (B*) get_global_db(iode_type);
        }
        else
            kdb = local_kdb;
        
        if(!kdb)
            throw std::runtime_error("The current " + get_iode_type_str() + 
                " has not been initialized yet");
        return kdb;
    }

    B* get_global_database() const
    {
        B* kdb = nullptr; 
        if(iode_type == COMMENTS)
            kdb = global_ws_cmt.get();
        else
            kdb = (B*) get_global_db(iode_type);
        
        if(!kdb)
            throw std::runtime_error("The global " + get_iode_type_str() + 
                " has not been initialized yet");
        return kdb;
    }

    // ======== methods ========

    IodeDatabaseType get_database_type() const override
    {
        B* kdb = get_database();
        return (IodeDatabaseType) kdb->k_db_type;
    }

    int size() const override
    { 
        B* kdb = get_database();
        return kdb->size();
    }

    int get_iode_type() const override
    { 
        B* kdb = get_database();
        return kdb->k_type; 
    }

    std::string get_iode_type_str() const override
    { 
        B* kdb = get_database();
        int _type_ = kdb->k_type;
        return v_iode_types[_type_]; 
    }

    bool is_global_database() const override
    { 
        return get_database_type() == DB_GLOBAL; 
    }

    bool is_local_database() const override
    { 
        return get_database_type() == DB_STANDALONE; 
    }

    bool is_shallow_copy_database() const override
    { 
        return get_database_type() == DB_SHALLOW_COPY; 
    }

    std::string get_filename() const override
    {
        B* kdb = get_database();
        if(!kdb->filepath.empty()) 
        {
            std::string filename = oem_to_utf8(kdb->filepath);
            return filename;
        }
        else
            return "ws";
    }

    void set_filename(const std::string& filename) override
    { 
        B* kdb = get_database();
        std::string filename_oem = utf8_to_oem(filename);
        set_kdb_filename(kdb, filename_oem); 
    }

    std::string get_description() const override
    { 
        B* kdb = get_database();
        std::string desc = kdb->description;
        if(!desc.empty())
            desc = oem_to_utf8(desc);
        return desc;
    }

    void set_description(const std::string& description) override
    {
        B* kdb = get_database();
        kdb->description = utf8_to_oem(description);
    }

    int index_of(const std::string& name) const override
    {
        B* kdb = get_database();
        check_name(name, this->iode_type);
        int pos = kdb->index_of(name);
        return pos;
    }

    bool contains(const std::string& name) const override
    { 
        B* kdb = get_database();
        return kdb->contains(name);
    }

    // object names

    std::string get_name(const int pos) const override
    {
        B* kdb = get_database();
        return kdb->get_name(pos);
    }

    bool set_name(const int pos, const std::string& new_name) override
    {
        std::string old_name = get_name(pos);
        bool success = rename(old_name, new_name);
        return success;
    }

    bool rename(const std::string& old_name, const std::string& new_name, 
        const bool overwrite = false) override
    {
        B* kdb;
        bool success = false;
        switch(get_database_type())
        {
        case DB_GLOBAL:
            kdb = get_global_database();
            success = kdb->rename(old_name, new_name, overwrite);
            break;
        case DB_STANDALONE:
            kdb = get_database();
            success = kdb->rename(old_name, new_name, overwrite);
            break;
        case DB_SHALLOW_COPY:
            // first rename in global KDB
            kdb = get_global_database();
            success = kdb->rename(old_name, new_name, overwrite);
            if (!success) 
                break;
            // then rename in local KDB
            kdb = get_database();
            success = kdb->rename(old_name, new_name, overwrite);
            break;
        default:
            break;
        }
    
        return success;
    }

    std::string expand(const std::string& pattern, const char all) const override
    {
        B* kdb = get_database();
        std::string lst = kdb->expand(pattern, all);
        return lst;
    }

    std::vector<std::string> filter_names(const std::string& pattern, 
        const bool must_exist=true) const override
    {
        B* kdb = get_database();
        std::vector<std::string> names = kdb->filter_names(pattern, must_exist);
        return names;
    }

    std::vector<std::string> get_names() const override
    {
        B* kdb = get_database();
        return kdb->get_names();
    }

    std::string get_names_as_string() const override
    {
        B* kdb = get_database();
        return kdb->get_names_as_string();
    }

    //  Create + Update + Get + Copy methods

    // Note: The definition of a template must be visible in the translation unit in which it is instantiated 
    // (which usually means your code in the header file), or it must be explicitly instantiated for the desired types. 
    // That is simply how C++ templates work.
    // https://stackoverflow.com/questions/23412703/visual-studio-static-libraries-and-variadic-template-classes#comment35876150_23412703
    // https://stackoverflow.com/a/47890906


    template<class... Args> bool add(const std::string& name, Args... args)
    {
        bool success = false;
        std::string error_msg = "Cannot add " + get_iode_type_str() + " with name '" + name + "'.\n";
        error_msg += "The " + get_iode_type_str() + " with name '" + name + "' already exists in the database.\n";
        error_msg += "Use the update() method instead.";

        B* kdb = get_database();        
        check_name(name, kdb->k_type);

        if(get_database_type() != DB_SHALLOW_COPY)
        {
            if(kdb->contains(name))
                throw std::invalid_argument(error_msg);
            success = kdb->set(name, args...);
        }
        else
        {
            B* global_kdb = get_global_database();
            if(global_kdb->contains(name))
                throw std::invalid_argument(error_msg);
            // add new obj to the global KDB
            success = global_kdb->set(name, args...);
            if(!success)
                return false;
            // add a new entry and copy the pointer
            kdb->add_entry(name);
            kdb->k_objs[name] = global_kdb->get_handle(name);
            success = true;
        }

        return success;
    }

    template<class... Args> void update(const std::string& name, Args... args)
    {
        std::string error_msg = "Cannot update " + get_iode_type_str() + " with name '" + name + "'.\n";
        error_msg += "Name '" + name + "' not found in the database.\nUse the add() method instead.";

        B* kdb = get_database();
        if(!kdb->contains(name))
            throw std::invalid_argument(error_msg);
        
        // NOTE: In the case of a shallow copy, only pointers to objects 
        //       are duplicated, not the objects.
        //       Modifying an object passing either the shallow copy
        //       or the global database modifies the same object.
        kdb->set(name, args...);
    }

    T get(const std::string& name) const
    {
        // throw exception if object with passed name does not exist
        if(!this->contains(name))
            throw std::invalid_argument("Cannot get " + get_iode_type_str() + " with name '" + name + "'.\n" +  
                                        "The object with name '" + name + "' does not exist in the database.");
        return get_unchecked(name);
    }

    T copy(const std::string& name) const
    {
        // throw exception if object with passed name does not exist
        T obj = get(name);
        return copy_obj(obj);
    }

    // delete

    void remove(const int pos) override
    {
        // throw exception if object with passed position is not valid
        std::string name = get_name(pos);
        remove(name);
    }
    
    void remove(const std::string& name) override
    {
        B* kdb;
        switch(get_database_type())
        {
        case DB_GLOBAL:
            kdb = get_global_database();
            kdb->remove(name);
            break;
        case DB_STANDALONE:
            kdb = get_database();
            kdb->remove(name);
            break;
        case DB_SHALLOW_COPY:
            // first delete in shallow copy KDB
            kdb = get_database();
            kdb->remove(name);
            // then delete in global KDB
            kdb = get_global_database();
            kdb->remove(name);
            break;
        default:
            throw std::invalid_argument("Cannot remove object named '" + name + "'\n."
                                        "Something went wrong.");
            break;
        }
    }

    // Other methods

    void merge(const KDBAbstract& other, const bool overwrite=true) override
    {
        B* kdb = get_database();

        KDBTemplate<B, T>* other_templ = (KDBTemplate<B, T>*) (&other);
        B* other_kdb = other_templ->get_database();
    
        int res = K_merge(kdb, other_kdb, overwrite ? 1 : 0);
        if(res < 0) 
            throw std::runtime_error("Cannot merge the two '" + get_iode_type_str() + "' databases.\n" + 
                                     "Reason: unknown");
    }
    
    void copy_from(const std::string& input_file, const std::string objects_names = "*") override
    {
        std::string buf = input_file + " " + objects_names;
        int res = B_WsCopy(const_cast<char*>(buf.c_str()), this->iode_type);
        if(res < 0)
        {
            std::string last_error = error_manager.get_last_error();
            if(!last_error.empty())
            {
                std::string msg = "Cannot copy the content of file '" + input_file;
                msg += "' into the " + get_iode_type_str() + " database.\n" + last_error;
                throw std::runtime_error(msg);
            }
        }
    }
    
    // TODO JMP: please provide input values to test B_WsMerge()
    void merge_from(const std::string& input_file) override
    {
        B* kdb = get_database();
    
        // throw an error if the passed filepath is not valid
        IodeFileType file_type = (IodeFileType) this->iode_type;
        std::string input_file_ = check_filepath(input_file, file_type, "merge_from", true);
        
        int res = K_cat(kdb, to_char_array(input_file));
        if(res < 0) 
            throw std::runtime_error("Cannot merge the content of the file '" + input_file_ + "' in the current " + 
                                     get_iode_type_str() + " database");
    }
    
    // TODO ALD: rewrite B_DataSearchParms() in C++
    std::vector<std::string> search(const std::string& pattern, const bool word=true, 
        const bool case_sensitive=true, const bool in_name=true, const bool in_formula=true, 
        const bool in_text=true, const std::string& list_result="_RES") override
    {
        std::vector<std::string> objs_list;
    
        int c_word  = word ? 1 : 0;
        int c_ecase = case_sensitive ? 1 : 0;
        int c_names = in_name ? 1 : 0;
        int c_forms = in_formula ? 1 : 0;
        int c_texts = in_text ? 1 : 0;
        char** c_list = B_DataSearchParms(to_char_array(pattern), c_word, c_ecase, 
                                          c_names, c_forms, c_texts, (int) this->iode_type);
    
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

    // load - save - clear

    bool load(const std::string& filepath) override
    {
        B* kdb = get_database();
        kdb->clear();
        bool success = kdb->load(filepath);
        return success;
    }
    
    bool save(const std::string& filepath, const bool compress=false) override
    {
        B* kdb = get_database();
    
        if(kdb->size() == 0) 
            return true;
    
        // throw an error if the filepath is not valid
        IodeFileType file_type = (IodeFileType) this->iode_type;
        std::string _filepath_ = check_filepath(filepath, file_type, "save", false);
    
        if(_filepath_.size() >= sizeof(FNAME)) 
            throw std::invalid_argument("Cannot save the '" + get_iode_type_str() + "' database.\n" +   
                                        "The filepath '" + _filepath_ + "' is too long.");
    
        // LZH compression
        int klzh = K_LZH;
        K_LZH = 2;
    
        bool success = kdb->save(_filepath_);
        K_LZH = klzh;
    
        return success;
    }
    
    void clear() override
    {
        B* kdb = get_database();
        kdb->clear();
    }
};


template<typename B, typename T>
T* template_subset(T* parent, const std::string& pattern, const bool deep_copy=false)
{
    // standalone copy database
    if(deep_copy)
    {
        T* db_subset = new T(false);
        B* kdb = db_subset->get_database();

        bool success = false;
        B* kdb_parent = parent->get_database();
        if(!kdb_parent)
            delete db_subset;
        else
        {
            kdb->k_db_type = DB_STANDALONE;
            kdb->k_type = kdb_parent->k_type;
            kdb->k_mode = kdb_parent->k_mode;
            kdb->k_arch = kdb_parent->k_arch;
            kdb->description = kdb_parent->description;
            kdb->k_compressed = kdb_parent->k_compressed;
            kdb->filepath = kdb_parent->filepath;
            if(kdb_parent->sample)
                kdb->sample = new Sample(*kdb_parent->sample);
            else
                kdb->sample = nullptr;
            success = kdb->copy_objs(*kdb_parent, pattern);
        }

        if(!success)
        {
            delete db_subset;
            std::string error_msg = "Cannot create a subset the database '"; 
            error_msg += v_iode_types[parent->get_iode_type()] + "' using the pattern '";
            error_msg += pattern + "'";
            throw std::runtime_error(error_msg);
        }
        return db_subset;
    }
    // shallow copy database
    else
    {
        T* db_subset = new T(parent, pattern);
        return db_subset;
    }  
}

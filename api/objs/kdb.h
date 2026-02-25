#pragma once

#include "api/constants.h"
#include "api/b_args.h"         // B_ainit_chk
#include "api/b_errors.h"       // error_manager
#include "api/time/sample.h"    // Period, Sample
#include "api/objs/xdr.h"

#include <string>
#include <set>
#include <map>
#include <memory>               // std::shared_ptr
#include <vector>
#include <array>
#include <memory>               // std::unique_ptr, std::shared_ptr
#include <stdexcept>            // std::invalid_argument, std::out_of_range, std::runtime_error

#ifndef SKBUILD
    #include "gtest/gtest.h"
#endif

inline int K_SECRETSEP = '#';   // pour les macros pushed A#n in reports
inline int B_TABLE_TITLE = 0;   // Specify how to print a TABLE 
                                //      0 : print table full definitions
                                //      1 : print only table titles

enum IodeDatabaseType
{
    DB_GLOBAL,          //< global database (must be unique)
    DB_STANDALONE,      //< isolated database (useful when working with estimation of block of equations)
    DB_SUBSET           //< subset of a global or standalone database (shallow copy of IODE objects)
};


/*----------------------- STRUCTS ----------------------------*/

struct KDBInfo
{
    char        k_db_type;                  // type of database: DB_GLOBAL (default), DB_STANDALONE, DB_SUBSET
    short       k_type = (short) OBJECTS;   // type of the object: COMMENTS, EQUATIONS, ..., VARIABLES
    short       k_mode = (short) ASIS_CASE; // case of the object name: UPPER_CASE, LOWER_CASE or ASIS_CASE 
    std::string k_arch = std::string(ARCH); // processor architecture on which the file has been created/saved/loaded
    std::string description = "";           // short description of the content of the database
    Sample*     sample = nullptr;           // Sample if Variables database
    char        k_compressed = 0;           // are the objects compressed in the file ? (LZH method, slow)
    std::string filepath = std::string(I_DEFAULT_FILENAME);     // filepath to the database file

protected:
    void set_mode(const short type)
    {
        switch(type) 
        {
            case COMMENTS :
                this->k_mode = (short) ASIS_CASE;
                break;
            case EQUATIONS :
                this->k_mode = (short) UPPER_CASE;
                break;
            case IDENTITIES :
                this->k_mode = (short) UPPER_CASE;
                break;
            case LISTS :
                this->k_mode = (short) UPPER_CASE;
                break;
            case SCALARS :
                this->k_mode = (short) LOWER_CASE;
                break;
            case TABLES :
                this->k_mode = (short) UPPER_CASE;
                break;
            case VARIABLES :
                this->k_mode = (short) UPPER_CASE;
                break;
            case OBJECTS :
                this->k_mode = (short) ASIS_CASE;
                break;
        }
    }

public:
    KDBInfo(const IodeType type, const bool is_global)
    {
        this->k_db_type = is_global ? DB_GLOBAL: DB_STANDALONE;
        this->k_type = (short) type;
        this->set_mode(this->k_type);
    }

    KDBInfo(const KDBInfo& other)
    {
        this->k_db_type = other.k_db_type;
        this->k_type = other.k_type;
        this->k_mode = other.k_mode;
        this->k_arch = other.k_arch;
        this->description = other.description;
        this->k_compressed = other.k_compressed;
        this->filepath = other.filepath;

        if(other.sample)
            this->sample = new Sample(*(other.sample));
        else
            this->sample = nullptr;
    }

    ~KDBInfo()
    {
        if(this->sample)
            delete this->sample;
        this->sample = nullptr;
    }

    int get_iode_type() const 
    { 
        return k_type; 
    }

    bool is_global_database() const 
    { 
        return k_db_type == DB_GLOBAL; 
    }

    bool is_detached_database() const 
    { 
        return k_db_type == DB_STANDALONE; 
    }

    bool is_subset_database() const 
    { 
        return k_db_type == DB_SUBSET; 
    }

    int preload(FILE *fd, const std::string& filepath, const int vers);

    virtual void clear()
    {        
        this->description.clear();
        this->k_compressed = 0;
        this->filepath = I_DEFAULT_FILENAME;

        if(this->sample)
            delete this->sample;
        this->sample = nullptr;
    }

    /**
     *  Translates a little-endian KDB struct into a little/big-endian KDB.
     * 
     *  INTEL architecture (x86)
     *  ------------------------
     *  In the little-endian (x86) architecture, returns an allocated copy 
     *  of the original KDB (but no translation is performed) for compatibility 
     *  between architectures.
     * 
     *  Other architectures (big-endian)
     *  --------------------------------
     *  Switches the architecture of a KDB (little-endian to big-endian or the 
     *  other way round). 
     *
     *  If okdb is null, the ikdb architecture is little-endian (when reading from a file) 
     *  and the transformation is done "in-place". No allocation is done.
     *  
     *  If okdb is not null, the ikdb is big-endian (in mem) and the result (little-endian) 
     *  is allocated. ikdb is untouched in this case.
     *  
     *  @param [in]     ikdb    KDB*    input KDB struct
     *  @param [out]    okdb    KDB**   allocated copy of this
     */
    void xdr(KDBInfo** okdb)
    {
        KDBInfo* xdr_kdb = nullptr;
        short type = this->k_type;
        
        if(okdb != NULL) 
        {
            // create a copy
            xdr_kdb = new KDBInfo(*this);
            *okdb = xdr_kdb;
        }
    
    #ifdef INTEL
        return;
    #else
        if(okdb == NULL) 
        {
            /* intel read */
            xdr_kdb = this;
            K_xdrSHORT(&type);
        }
    
        XDR_rev(&(xdr_kdb->k_type), 1, sizeof(short));
        XDR_rev(&(xdr_kdb->k_mode), 1, sizeof(short));
    
        if(type == VARIABLES) 
        {
            if(xdr_kdb->sample)
                K_xdrSMPL((unsigned char*) xdr_kdb->sample);
        }
    #endif
    }
};


// NOTE: intermediate base class to expose to Cython
struct KDB: public KDBInfo 
{
protected:
    virtual bool binary_to_obj(const std::string& name, char* pack) = 0;
    virtual bool obj_to_binary(char** pack, const std::string& name) = 0;

    virtual bool grep_obj(const std::string& name, const std::string& pattern, 
        const bool ecase, const bool forms, const bool texts, const char all) const = 0;

    virtual char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) = 0;

    virtual void update_reference_db() = 0;

public:
    // global or standalone database
    KDB(const IodeType type, const bool is_global) : KDBInfo(type, is_global) {}

    // subset (shallow or deep copy) 
    KDB(KDB* db_parent, const bool copy) : KDBInfo((IodeType) db_parent->k_type, false)
    {
        k_db_type = copy ? DB_STANDALONE : DB_SUBSET;
    }
    
    // copy constructor
    // NOTE: new database is a 'deep copy' of the passed database
    KDB(const KDB& other) : KDBInfo(other)
    {
        this->k_db_type = DB_STANDALONE;
    }

    ~KDB() {}

    virtual void clear(bool delete_objs = true) = 0;

    virtual int size() const = 0;

    IodeDatabaseType get_database_type() const
    {
        return (IodeDatabaseType) this->k_db_type;
    }

    std::string get_iode_type_str() const
    { 
        int _type_ = this->k_type;
        return v_iode_types[_type_]; 
    }

    std::string get_filename_utf8() const
    {
        if(!this->filepath.empty()) 
        {
            std::string filename = oem_to_utf8(this->filepath);
            return filename;
        }
        else
            return "ws";
    }

    void set_filename_utf8(const std::string& filename)
    { 
        std::string filename_oem = utf8_to_oem(filename);
        this->filepath = filename_oem; 
    }

    void set_fullpath(const std::string& filename)
    {
        if(filepath.empty())
        {
            kwarning("Empty filepath");
            return;
        }
        std::filesystem::path p_filepath(filepath);
        p_filepath = std::filesystem::absolute(p_filepath);
        this->filepath = p_filepath.string();
    }

    std::string get_description_utf8() const
    { 
        std::string desc = this->description;
        if(!desc.empty())
            desc = oem_to_utf8(desc);
        return desc;
    }

    void set_description_utf8(const std::string& description)
    {
        this->description = utf8_to_oem(description);
    }

    // strip the name and set it to upper/lower/asis according to k_mode
    std::string to_key(const std::string& name) const
    {
        std::string key = trim(name);
        switch(this->k_mode)
        {
            case UPPER_CASE :
                return to_upper(key);
            case LOWER_CASE :
                return to_lower(key);
            default :
                return key;
        }
    }

    virtual bool contains(const std::string& name) const = 0;
    virtual int index_of(const std::string& name) const = 0;
    virtual const std::string& get_name(const int index) const = 0;
    virtual std::set<std::string> get_names() const = 0;
    virtual std::string get_names_as_string() const = 0;

    std::set<std::string> filter_names(const std::string& pattern, 
        const bool must_exist=true) const
    {
        std::set<std::string> names;

        if(pattern.empty())
            return names;

        if(pattern == "*")
            return get_names();

        std::string error_msg = "'" + v_iode_types[this->k_type] + "': ";
        error_msg += "no names found matching the pattern '" + pattern + "'";
        
        char* OLD_SEPS = A_SEPS;
        A_SEPS = (char*) " ,;\n\t\r";

        // Retrieves all object names matching one or more patterns in the 
        // global or standalone database (similar to grep)
        std::string lst = this->expand(pattern, '*');
        if(lst.empty())
            throw std::runtime_error(error_msg);
        
        // Parses a string and replaces @filename and $listname by their contents
        char** c_names = B_ainit_chk((char*) lst.c_str(), NULL, 0);

        A_SEPS = OLD_SEPS;

        // 1. convert char** -> std::vector<std::string>
        // 2. check that each name exists in the database (if must_exist == true)
        std::string name;
        int nb_names = SCR_tbl_size((unsigned char **) c_names);
        for(int i = 0; i < nb_names; i++)
        {
            name = std::string(c_names[i]);
            if(must_exist && !contains(name))
                continue;
            names.insert(name);
        }
        SCR_free_tbl((unsigned char **) c_names);

        if(names.size() == 0)
            throw std::runtime_error(error_msg);
        
        // return names
        return names; 
    }

    virtual bool rename(const std::string& old_name, const std::string& new_name, 
        const bool overwrite = false) = 0;
    virtual bool remove(const std::string& name) = 0;

    virtual bool duplicate(const std::string& old_name, const std::string& new_name) = 0;

    // other methods

    std::vector<std::string> grep(const std::string& pattern, const bool ecase, 
        const bool names, const bool forms, const bool texts, const char all) const;
    std::string expand(const std::string& pattern, const char all='*') const;
    virtual bool print_obj_def(const std::string& name) = 0;
    char* dde_create_obj(int objnb, int* nc, int* nl);

    virtual bool copy_from_file(const std::string& file, const std::string& objs_names, 
        std::set<std::string>& v_found) 
    { 
        return false; 
    }

    bool copy_from(const std::vector<std::string>& input_files, const std::string& objects_names);
    bool copy_from(const std::string& input_files, const std::string& objects_names = "*");

    virtual void merge_from(const std::string& input_file) = 0;

    // TODO ALD: rewrite B_DataSearchParms() in C++
    std::vector<std::string> search(const std::string& pattern, const bool word=true, 
        const bool case_sensitive=true, const bool in_name=true, const bool in_formula=true, 
        const bool in_text=true, const std::string& list_result="_RES");

    // load

    virtual bool load_asc(const std::string& filename) = 0;
    bool load_binary(const int file_type, const std::string& filename, 
                        const std::vector<std::string>& objs=std::vector<std::string>());
    bool load(const std::string& filename);

    // save

    virtual bool save_asc(const std::string& filename) = 0;
    bool save_binary(const std::string& filename, const bool override_filepath = true);
    bool save(const std::string& filename, const bool compress=false);

    virtual bool save_csv(const std::string& filename, const std::vector<std::string>& 
        varlist=std::vector<std::string>(), Sample* sample=nullptr) 
    { 
        std::string msg = "Cannot save CSV: ";
        msg += "CSV saving is not implemented for database of type ";
        msg += "'" + v_iode_types[this->k_type] + "s'";
        throw std::runtime_error(msg); 
    }
};


template<class T> struct KDBTemplate: public KDB
{
#ifndef SKBUILD
    FRIEND_TEST(SubsetsTest, Subset);
    FRIEND_TEST(SubsetsTest, MultiSubsets);
#endif

public:
    // NOTE: if an IODE object is added/removed/updated from the current database, 
    //       it must also done in all subset instances ('shallow copies')
    std::map<std::string, T*> k_objs;

private:
    // only used by subsets ('shallow copies')
    KDBTemplate* db_parent = nullptr;
    // only used by global or standalone databases
    std::set<KDBTemplate*> children_db;

    // TODO: delete after solving the issue #1140
    // set of pointers to objects that are tracked for deletion 
    // (to avoid deleting an object that is still referenced by a subset) 
    std::set<T**> tracked_ptrs; 

private:
    // TODO: delete after solving the issue #1140
    void delete_tracked_ptr(T* obj_ptr)
    {
        if(this->k_db_type == DB_SUBSET)
            return;
        
        for(T** tracked_ptr : tracked_ptrs)
        {
            if(*tracked_ptr == obj_ptr)
            {
                *tracked_ptr = nullptr;
                tracked_ptrs.erase(tracked_ptr);
                break;
            }
        }
    }

    void delete_objs()
    {
        for(auto& [name, obj_ptr] : k_objs)
        {
            // NOTE: it should not be necessary to check for nullptr before deleting, 
            //       but we do it just in case
            if(obj_ptr)
            {
                this->delete_tracked_ptr(obj_ptr);
                delete obj_ptr;
            }
        }
        k_objs.clear();
    }

    bool shallow_copy(const KDBTemplate& other, const std::set<std::string>& subset_keys)
    {
        T* obj_ptr;
        bool success = true;
        for(const std::string& name : subset_keys) 
        {
            if(!other.contains(name))  
            {
                std::string error_msg = v_iode_types[this->k_type];
                error_msg += " with '" + name + "' not found in the source database";
                kwarning(error_msg.c_str());
                success = false;
                continue;
            }
            
            obj_ptr = other.get_obj_ptr(name);
            this->k_objs[name] = obj_ptr;
        }

        if(!success)
            this->delete_objs();

        return success;
    }

    bool deep_copy(const KDBTemplate& other, const std::set<std::string>& subset_keys)
    {
        bool success = true;
        for(const std::string& name : subset_keys) 
        {
            if(!other.contains(name))  
            {
                std::string error_msg = v_iode_types[this->k_type];
                error_msg += " with '" + name + "' not found in the source database";
                kwarning(error_msg.c_str());
                success = false;
                continue;
            }

            this->copy_obj_from(other, name, name);
        }

        if(!success)
            this->delete_objs();

        return success;
    }

public:
    // global or standalone database
    KDBTemplate(const IodeType type, const bool is_global) : KDB(type, is_global) {}

    // subset (shallow or deep copy) 
    KDBTemplate(KDBTemplate* db_parent, const std::string& pattern, const bool copy) 
        : KDB(db_parent, copy)
    {
        std::string error_msg;
        if(!db_parent)
        {
            error_msg = "Cannot create a subset database: parent database is null";
            throw std::invalid_argument(error_msg);
        }

        error_msg = "Cannot create a subset the database of type '";
        error_msg += v_iode_types[this->k_type] + "' using the pattern '";
        error_msg += pattern + "'";

        std::set<std::string> subset_keys;
        try
        {
            subset_keys = db_parent->filter_names(pattern);
        }
        catch(const std::exception& e)
        {
            subset_keys.clear();
            error_msg += std::string(e.what()) + " in the parent database";
            throw std::runtime_error(error_msg);
        }

        KDBTemplate* true_db_parent = db_parent;
        // case where we create a subset of a subset
        if(db_parent->k_db_type == DB_SUBSET)
            true_db_parent = db_parent->db_parent;
        // normal case (global/standalone -> subset)
        else
            true_db_parent = db_parent;

        if(true_db_parent->sample)
            this->sample = new Sample(*true_db_parent->sample);
        this->description = true_db_parent->description;
        this->k_compressed = true_db_parent->k_compressed;
        this->filepath = true_db_parent->filepath;

        if(this->k_db_type == DB_SUBSET)
        {
            bool success = shallow_copy(*true_db_parent, subset_keys);
            if(!success)
                throw std::runtime_error(error_msg);
            
            true_db_parent->children_db.insert(this);
            this->db_parent = true_db_parent;
        }
        else
        {
            bool success = deep_copy(*true_db_parent, subset_keys);
            if(!success)
                throw std::runtime_error(error_msg);
            
            this->db_parent = nullptr;
        }
    }

    // copy constructor
    KDBTemplate(const KDBTemplate& other): KDB(other) 
    {
        if(other.size() == 0)
            return;

        std::string error_msg = "Cannot create a copy of the database of type '";
        error_msg += v_iode_types[this->k_type] + "'";

        std::set<std::string> subset_keys;
        try
        {
            subset_keys = other.filter_names("*");
        }
        catch(const std::exception& e)
        {
            subset_keys.clear();
            error_msg += ":\n" + std::string(e.what()) + " in the parent database";
            throw std::runtime_error(error_msg);
        }

        bool success = deep_copy(other, subset_keys);
        if(!success)
            throw std::runtime_error(error_msg);
    }

    ~KDBTemplate()
    {
        if(k_db_type != DB_SUBSET)
        {
            for(KDBTemplate* child_db : children_db)
            {
                child_db->k_objs.clear();
                child_db->db_parent = nullptr;
            }
            children_db.clear();
            this->delete_objs();

            // TODO: delete after solving the issue #1140
            for(T** tracked_ptr : tracked_ptrs)
                *tracked_ptr = nullptr;
        }

        if(db_parent)
        {
            db_parent->children_db.erase(this);
            db_parent = nullptr;
        }
    }

    void clear(bool delete_objs = true) override
    {
        if(k_db_type != DB_SUBSET)
        {
            for(KDBTemplate* child_db : children_db)
            {
                child_db->k_objs.clear();
                child_db->db_parent = nullptr;
            }

            children_db.clear();
        }
        
        if(delete_objs && k_db_type != DB_SUBSET)
            this->delete_objs();
        else
            this->k_objs.clear();

        KDBInfo::clear();
    }

    KDBTemplate* get_top_level_db()
    {
        if(k_db_type != DB_SUBSET)
            return this;
        else
        {
            if(!db_parent)
            {
                std::string error_msg = "Internal error: subset database of type '";
                error_msg += v_iode_types[this->k_type] + "' has a null parent database";;
                throw std::runtime_error(error_msg);
            }
            return db_parent;
        }
    }

    std::set<KDBTemplate*> get_children_db() const
    {
        if(k_db_type != DB_SUBSET)
            return children_db; 
        else
            return db_parent->children_db;
    }

    int size() const override
    { 
        return (int) k_objs.size();
    }

    bool contains(const std::string& name) const override
    {
        std::string key = to_key(name);
        return k_objs.contains(key);
    }

    bool parent_contains(const std::string& name) const
    {
        if(k_db_type != DB_SUBSET || !db_parent)
            return false;

        return db_parent->contains(name);
    }

    int index_of(const std::string& name) const override
    {
        std::string key = to_key(name);
        // check_name(key, this->iode_type);
        auto it = k_objs.find(key);
        if (it != k_objs.end()) 
            return (int) std::distance(k_objs.begin(), it);
        else
            return -1;
    }

    // NOTE: - repeated calls to this function can be inefficient (O(n) each)
    //         To iterate over all names, prefer the C++17 loop syntax:
    //         for(const auto& [name, _] : kdb.k_objs) { ... } 
    const std::string& get_name(const int index) const override
    {
        static const std::string empty_string = "";
        if(index < 0 || index >= this->size())
        {
            std::string msg = "Cannot find the name corresponding to the index ";
            msg += std::to_string(index) + ": index out of the range [0, ";
            msg += std::to_string(this->size() - 1) + "]";
            throw std::out_of_range(msg);
        }

        auto it = k_objs.begin();
        std::advance(it, index);
        return const_cast<std::string&>(it->first);
    }

    std::set<std::string> get_names() const override
    {
        std::set<std::string> names;
        for(const auto& [name, _] : k_objs) 
            names.insert(name);
        return names;
    }

    std::string get_names_as_string() const override
    {
        std::string names;
        for(const auto& [name, _] : k_objs) 
            names += name + ";";

        // remove last ;
        if(!names.empty() && names.back() == ';')
            names.pop_back();

        return names;
    }

    bool rename(const std::string& old_name, const std::string& new_name, 
        const bool overwrite = false) override
    {
        std::string old_key = to_key(old_name);
        std::string new_key = to_key(new_name);

        // renaming equations is not allowed (made on purpose)
        if(this->k_type == EQUATIONS)
            throw std::invalid_argument("Renaming equations in the database is not allowed");

        if(k_objs.size() == 0)
            return false;

        check_name(new_key, k_type);

        if(!contains(old_key))
        {
            std::string msg = "Cannot rename object: there is no object previously named '" + 
                              old_key + "' found in the database.";
            throw std::invalid_argument(msg);
        }

        // rename in top-level database
        KDBTemplate* top_db = get_top_level_db();        
        if(!overwrite)
        {
            if(top_db->contains(new_key))
            {
                std::string msg = "Cannot rename object: an object named '" + new_key + 
                                  "' already exists in the database.";
                throw std::invalid_argument(msg);
            }
        }

        T* obj_ptr = top_db->get_obj_ptr(old_key);
        top_db->k_objs.erase(old_key);
        top_db->k_objs[new_key] = obj_ptr;

        // rename in all subset instances (including 'this' if 'this' is a subset)
        for(KDBTemplate* subset : get_children_db())
        {
            if(!subset->contains(old_key))
                continue;
            subset->k_objs.erase(old_key);
            subset->k_objs[new_key] = obj_ptr;
        }

        return true;
    }

    bool remove(const std::string& name) override
    {
        std::string key = to_key(name);
        KDBTemplate* top_db = get_top_level_db();
        if(!(this->contains(key) && top_db->contains(key)))
        {
            std::string str_type = v_iode_types[this->k_type];
            std::string msg = "Cannot remove " + str_type + ": no " + str_type + 
                              " named '" + key + "' found in the database";
            kwarning(msg.c_str());
            return false;
        }

        // remove from top-level database
        T* obj_ptr = top_db->k_objs[key];
        top_db->k_objs.erase(key);

        // remove from all subset instances (including 'this' if 'this' is a subset)
        for(KDBTemplate* subset : get_children_db())
            subset->k_objs.erase(key);
        
        this->delete_tracked_ptr(obj_ptr);

        delete obj_ptr;
        return true;
    }

    T copy_obj(const T& obj) const
    {
        // uses the copy constructor of T to create a copy of the object
        return T(obj);
    }

    void copy_obj_to(KDBTemplate& dest, const std::string& source_key, 
        const std::string& dest_key) const
    {
        T* obj_ptr = this->get_obj_ptr(source_key);
        T* obj_copy_ptr = new T(*obj_ptr);
        dest.set_obj_ptr(dest_key, obj_copy_ptr);
    }

    void copy_obj_from(const KDBTemplate& other, const std::string& source_key, 
        const std::string& dest_key)
    {
        T* obj_ptr = other.get_obj_ptr(source_key);
        T* obj_copy_ptr = new T(*obj_ptr);
        this->set_obj_ptr(dest_key, obj_copy_ptr);
    }

    bool duplicate(const KDBTemplate& other, const std::string& old_name, const std::string& new_name)
    {
        std::string error_msg = "Cannot duplicate object '" + old_name + "' as '" + new_name + "': ";
        if(this == &other && old_name == new_name)
        {
            error_msg += "source and destination are identical.";
            kwarning(error_msg.c_str());
            return false;
        } 
    
        if(!other.contains(old_name))
        {
            error_msg += "object '" + old_name + "' does not exist in the source database";
            kwarning(error_msg.c_str());
            return false;
        }
    
        std::string old_key = to_key(old_name);
        std::string new_key = to_key(new_name);
        if(this->contains(new_name))
            this->remove(new_key);
        
        if(!other.contains(old_name))
        {
            error_msg += "Could not retrieve the " + v_iode_types[other.k_type];
            error_msg += " named '" + old_key + "' in the source database.";
            kwarning(error_msg.c_str());
            return false;
        } 
    
        this->copy_obj_from(other, old_key, new_key);
    
        return true;
    }

    bool duplicate(const std::string& old_name, const std::string& new_name)
    {
        return duplicate(*this, old_name, new_name);
    }

    bool copy_from_file(KDBTemplate& from, const std::string& file, const std::string& objs_names, 
        std::set<std::string>& v_found)
    {   
        bool success = from.load_binary(this->k_type, file);
        if(!success)
            return false;
    
        std::set<std::string> s_objs;
        try
        {
            s_objs = from.filter_names(objs_names);
        }
        catch(const std::exception& e)
        {
            kwarning(e.what());
            return false;
        }
    
        // delete objects already found in previous files
        std::vector<std::string> objs_to_copy;
        for(const std::string& name : s_objs)
        {
            if(!from.contains(name)) 
                continue;
            
            // if already found in previous files -> remove from "from"
            if(v_found.contains(name)) 
                from.remove(name);
            // object to be copied
            else
                objs_to_copy.push_back(name);
        }
    
        std::string msg = std::to_string(objs_to_copy.size()) + " "; 
        msg += to_lower(v_iode_types[this->k_type]) + " read from file '" + file + "'";
        kmsg(msg.c_str());
    
        for(const std::string& name : objs_to_copy)
        {
            // check if the object already exists in this database
            // if yes -> delete the corresponding object
            if(this->contains(name))
                this->remove(name);

            // copy the object from "from" to "this"
            this->copy_obj_from(from, name, name);
    
            v_found.insert(name);
        }
        
        return true;
    }

    void merge(KDBTemplate& other, const bool overwrite, const bool clear_source)
    {
        if(other.size() == 0) 
            return;
    
        bool found = false;
        for(const std::string& name : other.get_names()) 
        {
            if(!other.contains(name)) 
                continue;
    
            found = this->contains(name);
            if(found)
            {
                // existing elements in this are not overwritten.
                if(!overwrite) 
                    continue;
                
                // delete elements from this
                this->remove(name);
            }
            
            this->copy_obj_from(other, name, name);
        }
    
        if(clear_source)
            other.clear(false);
    }

    // NOTE: see KDBVariables::merge_from(const std::string& filename) for the 
    // specific implementation for IODE Variables (to handle the sample)
    // TODO JMP: please provide input values to test B_WsMerge()
    void merge_from(KDBTemplate& from, const std::string& filename)
    {
        if(this->k_type == VARIABLES)
            return;

        // throw an error if the passed filepath is not valid
        IodeFileType file_type = (IodeFileType) this->k_type;
        std::string input_file = check_filepath(filename, file_type, "merge_from", true);
        
        bool success = from.load(input_file);
        if(!success)
            return;
    
        if(this->size() == 0) 
        {
            this->description = from.description;
            this->filepath = from.filepath;
        }
     
        this->merge(from, true, true);
    }

    // NOTE: get_obj_ptr() and set_obj_ptr() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)
    T* get_obj_ptr(const std::string& name) const
    {   
        std::string key = to_key(name);
        if(key.empty())
        {
            std::string error_msg = "Cannot get an object with an empty name from the ";
            error_msg += "database of type '" + v_iode_types[this->k_type] + "'";
            throw std::invalid_argument(error_msg);
        }
        
        try
        {
            return k_objs.at(key);
        }
        catch(const std::out_of_range&)
        {
            std::string error_msg = v_iode_types[this->k_type] + " with name ";
            error_msg += "'" + name + "' not found in the database";
            throw std::invalid_argument(error_msg);
        }
    }

    // TODO: delete after solving the issue #1140
    void add_tracked_ptr(T** obj_ptr)
    {
        if(this->k_db_type != DB_SUBSET && obj_ptr)
            tracked_ptrs.insert(obj_ptr);
    }

    // virtual to be overridden in class KDBVariables
    virtual T* set_obj_ptr(const std::string& name, T* obj_ptr)
    {
        std::string key = to_key(name);
        if(key.empty())
        {
            std::string error_msg = "Cannot set an object with an empty name in the ";
            error_msg += "database of type '" + v_iode_types[this->k_type] + "'";
            throw std::invalid_argument(error_msg);
        }

        if(!obj_ptr)
        {
            std::string error_msg = "Cannot set a null object with name '" + name + "' in the ";
            error_msg += "database of type '" + v_iode_types[this->k_type] + "'";
            throw std::invalid_argument(error_msg);
        }

        KDBTemplate* top_db = get_top_level_db();

        if(this->k_db_type == DB_SUBSET)
        {
            // if the object already exists in the top-level database 
            // -> delete it
            if(top_db->contains(key))
            {
                // NOTE: if the object already exists in the top-level database 
                //       and is the same as the one we want to set, 
                //       exit without doing anything
                if(top_db->k_objs[key] == obj_ptr)
                    return obj_ptr;
                delete top_db->k_objs[key];
            }
            // if the object does not exist in the top-level database 
            // -> check the name validity
            else
                check_name(key, this->k_type);
        }
        else
        {
            // if the object already exists in this database -> delete it
            if(this->contains(key))
            {
                // NOTE: if the object already exists in the database 
                //       and is the same as the one we want to set, 
                //       exit without doing anything
                if(this->k_objs[key] == obj_ptr)
                    return obj_ptr;
                delete this->k_objs[key];
            }
            // if the object does not exist in this database -> check the name validity
            else
                check_name(key, this->k_type);
        }

        // create a copy of the object to be set. 
        // -> to make sure that 2 or more databases do not share the same object pointer 
        //    (which can lead to bugs when the pointer is deleted in one database but 
        //     still used in another database or when the destructor of the second database 
        //     is called and tries to delete the same pointer again) 
        T* new_obj_ptr = new T(*obj_ptr);

        this->k_objs[key] = new_obj_ptr;

        if(this->k_db_type == DB_SUBSET)
            top_db->k_objs[key] = new_obj_ptr;

        for(KDBTemplate* subset : get_children_db())
        {
            if(!subset->contains(name))
                continue;
            subset->k_objs[key] = new_obj_ptr;
        }

        return new_obj_ptr;
    }

    virtual T get(const std::string& name) const
    {
        T* obj_ptr = this->get_obj_ptr(name);
        if(!obj_ptr)
        {
            std::string error_msg = "Cannot get a null object with name '" + name + "' from the ";
            error_msg += "database of type '" + v_iode_types[this->k_type] + "'";
            throw std::runtime_error(error_msg);
        }

        return *obj_ptr;
    }

    T* set(const std::string& name, const T& obj)
    {
        T* obj_ptr = const_cast<T*>(&obj);
        T* new_obj_ptr = this->set_obj_ptr(name, obj_ptr);
        return new_obj_ptr;
    }

    virtual bool add(const std::string& name, const T& obj)
    {
        if(this->contains(name))
        {
            std::string msg = "Cannot add list: a list named '" + name + 
                              "' already exists in the database.";
            throw std::invalid_argument(msg);
        }
    
        if(this->parent_contains(name))
        {
            std::string msg = "Cannot add list: a list named '" + name + 
                              "' exists in the parent database of the present subset";
            throw std::invalid_argument(msg);
        }

        this->set(name, obj);
        return true;
    }

    virtual void update(const std::string& name, const T& obj)
    {
        if(!this->contains(name))
        {
            std::string msg = "Cannot update list: no list named '" + name + 
                              "' exists in the database.";
            throw std::invalid_argument(msg);
        }

        this->set(name, obj);
    }
};

/*----------------------- GLOBALS ----------------------------*/

KDB& get_global_db(const int iode_type);

/**
 * k_ext[][4] : extensions of IODE filenames.
 * 
 *   - cmt..var = IODE objects internal format
 *   - ac..av = IODE objects ascii format
 *   - next extensions : other IODE files
 */
inline char k_ext[][4] = 
{
    "cmt", // 0 = FILE_COMMENTS
    "eqs", // 1 = FILE_EQUATIONS
    "idt", // ... 
    "lst",
    "scl",
    "tbl",
    "var",
    "ooo", // 7 = IODE_NB_TYPES

    "ac",  // 8
    "ae",
    "ai",
    "al",
    "as",
    "at",
    "av",
    "",    // 15

    "rep", // 16
    "a2m",
    "agl",
    "prf",
    "dif",
    "mif",
    "rtf",
    "ps",
    "asc",
    "txt",
    "csv",  // 26 = FILE_CSV // JMP 2-3-2016  -> TODO: pas très propre, à modifier

    "xxx"
};

/*----------------------- FUNCS ----------------------------*/

inline bool wrap_grep_gnl(const std::string& pattern, const std::string& text, 
    const bool ecase, const char all)
{
    return SCR_grep_gnl((char*) pattern.c_str(), (char*) text.c_str(), 
                        (int) ecase, (int) all) == 0;
}

/* k_kdb.c */
bool dump_string(char* head, char* txt);
bool print_definition_generic(const std::string& name, char* text);

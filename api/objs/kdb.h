#pragma once

#include "scr4/s_swap.h"        // SWHDL
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


inline int K_WARN_DUP = 0;      // If null, adding an existing object name in a KDB  
                                // does not trigger an error (used in KDB::add_entry())
inline int K_SECRETSEP = '#';   // pour les macros pushed A#n in reports
inline int B_TABLE_TITLE;       // Specify how to print a TABLE 
                                //      0 : print table full definitions
                                //      1 : print only table titles

enum IodeDatabaseType
{
    DB_GLOBAL,          //< global database (must be unique)
    DB_STANDALONE,      //< isolated database (useful when working with estimation of block of eqs in the GUI)
    DB_SHALLOW_COPY     //< shallow copy (useful when working on subset in the GUI)
};


/*----------------------- STRUCTS ----------------------------*/

struct KDBInfo
{
    char        k_db_type;                  // type of database: DB_GLOBAL (default), DB_STANDALONE, DB_SHALLOW_COPY
    short       k_type = (short) OBJECTS;   // type of the object: COMMENTS, EQUATIONS, ..., VARIABLES
    short       k_mode = (short) ASIS_CASE; // case of the object name: UPPER_CASE, LOWER_CASE or ASIS_CASE 
    std::string k_arch = std::string(ARCH); // processor architecture on which the file has been created/saved/loaded
    std::string description = "";           // short description of the content of the database
    Sample*     sample = nullptr;           // Sample if Variables database
    char        k_compressed = 0;           // are the objects compressed in the file ? (LZH method, slow)
    std::string filepath = std::string(I_DEFAULT_FILENAME);     // filepath to the database file

public:
    KDBInfo(const bool is_global)
    {
        this->k_db_type = is_global ? DB_GLOBAL: DB_STANDALONE;
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

    bool is_local_database() const 
    { 
        return k_db_type == DB_STANDALONE; 
    }

    bool is_shallow_copy_database() const 
    { 
        return k_db_type == DB_SHALLOW_COPY; 
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


struct KDB: public KDBInfo 
{
#ifndef SKBUILD
    FRIEND_TEST(SubsetsTest, Subset);
    FRIEND_TEST(SubsetsTest, MultiSubsets);
#endif

    // NOTE: if an IODE object is added/removed/updated from the current database, 
    //       it must also done in all subset instances ('shallow copies')
    // map <object name, position in the SCR memory>
    std::map<std::string, SWHDL> k_objs;

private:
    // only used by subsets ('shallow copies')
    KDB* db_parent = nullptr;
    // only used by global or standalone databases
    std::set<KDB*> subset_instances;

private: 
    bool shallow_copy(const KDB& other, const std::set<std::string>& subset_keys)
    {
        SWHDL handle;
        bool success = true;
        for(const std::string& name : subset_keys) 
        {
            handle = other.get_handle(name);
            if(handle == 0)  
            {
                std::string error_msg = v_iode_types[this->k_type];
                error_msg += " with '" + name + "' not found in the source database";
                kwarning(error_msg.c_str());
                success = false;
                continue;
            }

            this->k_objs[name] = handle;
        }

        if(!success)
            this->k_objs.clear();

        return success;
    }

    bool deep_copy(const KDB& other, const std::set<std::string>& subset_keys)
    {
        bool ok;
        bool success = true;
        for(const std::string& name : subset_keys) 
        {
            ok = duplicate(other, name, name);
            if(!ok)
            {
                std::string error_msg = v_iode_types[this->k_type];
                error_msg += " with '" + name + "' could not be copied ";
                error_msg += "from the source database";
                kwarning(error_msg.c_str());
                success = false;
                continue;
            }
        }

        if(!success)
        {
            if(this->size() > 0)
            {
                for(auto& [this_name, this_handle] : k_objs) 
                    SW_free(this_handle);
                k_objs.clear();
            }
        }

        return success;
    }

protected:
    bool set_packed_object(const std::string& name, char* pack);

    virtual bool grep_obj(const std::string& name, const SWHDL handle, 
        const std::string& pattern, const bool ecase, const bool forms, 
        const bool texts, const char all) const = 0;

    virtual char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) = 0;

    virtual void update_reference_db() = 0;

public:
    // global or standalone database
    KDB(const IodeType type, const bool is_global) : KDBInfo(is_global)
    {
        this->k_type = (short) type;
        switch(this->k_type) 
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

    // subset (shallow or deep copy) 
    KDB(KDB* db_parent, const std::string& pattern, const bool copy) : KDBInfo(false)
    {
        k_db_type = copy ? DB_STANDALONE : DB_SHALLOW_COPY;
        this->k_type = db_parent->k_type;

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

        KDB* true_db_parent = db_parent;
        // case where we create a subset of a subset
        if(db_parent->k_db_type == DB_SHALLOW_COPY)
            true_db_parent = db_parent->db_parent;
        // normal case (global/standalone -> subset)
        else
            true_db_parent = db_parent;

        switch(this->k_type) 
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

        if(true_db_parent->sample)
            this->sample = new Sample(*true_db_parent->sample);
        this->description = true_db_parent->description;
        this->k_compressed = true_db_parent->k_compressed;
        this->filepath = true_db_parent->filepath;

        if(this->k_db_type == DB_SHALLOW_COPY)
        {
            bool success = shallow_copy(*true_db_parent, subset_keys);
            if(!success)
                throw std::runtime_error(error_msg);
            
            true_db_parent->subset_instances.insert(this);
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
    // NOTE: new database is a 'deep copy' of the passed database
    KDB(const KDB& other) : KDBInfo(other)
    {
        this->k_db_type = DB_STANDALONE;

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

    ~KDB()
    {
        if(k_db_type != DB_SHALLOW_COPY)
        {
            for(KDB* subset_db : subset_instances)
            {
                subset_db->k_objs.clear();
                subset_db->db_parent = nullptr;
            }
            subset_instances.clear();
            
            for(auto& [name, handle] : k_objs) 
                SW_free(handle);
            k_objs.clear();
        }

        if(k_db_type == DB_SHALLOW_COPY)
            k_objs.clear();

        if(db_parent)
        {
            db_parent->subset_instances.erase(this);
            db_parent = nullptr;
        }
    }

    void clear(bool delete_objs = true)
    {
        KDBInfo::clear();

        if(k_db_type != DB_SHALLOW_COPY)
        {
            for(KDB* subset_db : subset_instances)
            {
                subset_db->k_objs.clear();
                subset_db->db_parent = nullptr;
            }
            subset_instances.clear();
        }
        
        if(delete_objs && k_db_type != DB_SHALLOW_COPY)
        {
            for(auto& [name, handle] : k_objs) 
                SW_free(handle);
        }
        
        k_objs.clear();
    }

    KDB* get_top_level_db()
    {
        if(k_db_type != DB_SHALLOW_COPY)
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

    std::set<KDB*> get_subsets() const
    {
        if(k_db_type != DB_SHALLOW_COPY)
            return subset_instances; 
        else
            return db_parent->subset_instances;
    }

    bool copy_objs(const KDB& other, const std::string& pattern = "*")
    {
        if(this == &other)
            return true;

        if(other.size() == 0)
            return true;
        
        std::string error_msg = "Cannot copy " + v_iode_types[this->k_type] + ":\n";

        if(this->k_type != other.k_type)
        {
            error_msg += "\tincompatible database types";
            kwarning(error_msg.c_str());
            return false;
        }
        
        std::set<std::string> names;
        try
        {
            names = other.filter_names(pattern);
        }
        catch(const std::exception& e)
        {
            error_msg += std::string(e.what()) + " in the source database";
            kwarning(error_msg.c_str());
            return false;
        }

        bool OK;
        bool success = true;
        for(const std::string& name : names) 
        {
            OK = duplicate(other, name, name);
            if(!OK)
            {
                error_msg += "\t- failed to duplicate '" + name + "'";
                success = false;
            }
        }

        if(!success)
            kwarning(error_msg.c_str());

        return success;
    }

    int size() const 
    { 
        return (int) k_objs.size();
    }

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

    bool contains(const std::string& name) const
    {
        std::string key = to_key(name);
        return k_objs.contains(key);
    }

    bool parent_contains(const std::string& name) const
    {
        if(k_db_type != DB_SHALLOW_COPY || !db_parent)
            return false;

        return db_parent->contains(name);
    }

    int index_of(const std::string& name) const
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
    const std::string& get_name(const int index) const
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

    std::set<std::string> get_names() const
    {
        std::set<std::string> names;
        for(const auto& [name, _] : k_objs) 
            names.insert(name);
        return names;
    }

    std::string get_names_as_string() const
    {
        std::string names;
        for(const auto& [name, _] : k_objs) 
            names += name + ";";

        // remove last ;
        if(!names.empty() && names.back() == ';')
            names.pop_back();

        return names;
    }

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

    SWHDL get_handle(const std::string& name) const
    {
        std::string key = to_key(name);
        auto it = k_objs.find(key);
        if (it != k_objs.end()) 
            return it->second;
        else
            return 0;
    }

    char* get_ptr_obj(const std::string& name) const
    {
        std::string key = to_key(name);
        auto it = k_objs.find(key);
        if (it != k_objs.end()) 
            return SW_getptr(it->second);
        else
            return NULL;
    }

    bool rename(const std::string& old_name, const std::string& new_name, const bool overwrite = false)
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
        KDB* top_db = get_top_level_db();        
        auto it = top_db->k_objs.find(old_key);
        if(!overwrite)
        {
            auto it_new = top_db->k_objs.find(new_key);
            if (it_new != top_db->k_objs.end())
            {
                std::string msg = "Cannot rename object: an object named '" + new_key + 
                                  "' already exists in the database.";
                throw std::invalid_argument(msg);
            }
        }

        SWHDL handle = it->second;
        top_db->k_objs.erase(it);
        top_db->k_objs[new_key] = handle;

        // rename in all subset instances (including 'this' if 'this' is a subset)
        for(KDB* subset : get_subsets())
        {
            if(!subset->contains(old_key))
                continue;
            subset->k_objs.erase(old_key);
            subset->k_objs[new_key] = handle;
        }

        return true;
    }

    bool add_entry(const std::string& name)
    {
        std::string key = to_key(name);
        check_name(key, this->k_type);
        
        // add in top-level database
        KDB* top_db = get_top_level_db();
        bool found = top_db->contains(key);
        if(found) 
        {
            if(K_WARN_DUP)
            {
                kerror(0, "%s already defined", (char*) key.c_str());
                return false;
            }
        }
        else
            top_db->k_objs[key] = 0;

        k_objs[key] = 0;
        return true;
    }

    bool duplicate(const KDB& other, const std::string& old_name, const std::string& new_name);

    bool remove(const std::string& name)
    {
        std::string key = to_key(name);
        if(!this->contains(key))
        {
            std::string str_type = v_iode_types[this->k_type];
            std::string msg = "Cannot remove " + str_type + ": no " + str_type + 
                              " named '" + key + "' found in the database";
            kwarning(msg.c_str());
            return false;
        }

        // remove from top-level database
        KDB* top_db = get_top_level_db();
        auto it = top_db->k_objs.find(key);
            SW_free(it->second);
        top_db->k_objs.erase(it);

        // remove from all subset instances (including 'this' if 'this' is a subset)
        for(KDB* subset : get_subsets())
            subset->k_objs.erase(key);
        
        return true;
    }

    // other methods

    std::vector<std::string> grep(const std::string& pattern, const bool ecase, 
        const bool names, const bool forms, const bool texts, const char all) const;

    std::string expand(const std::string& pattern, const char all='*') const;

    virtual bool print_obj_def(const std::string& name) = 0;

    char* dde_create_obj(int objnb, int* nc, int* nl);

    void merge(const KDB& other, const bool overwrite=true);
    
    void copy_from(const std::string& input_file, const std::string objects_names = "*");
    
    void merge_from(const std::string& input_file);

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


// type O = IODE object type used in the old API (e.g., char*, double*, etc.)
template<class T> struct KDBTemplate: public KDB
{
    // global or standalone database
    KDBTemplate(const IodeType type, const bool is_global) : KDB(type, is_global) {}

    // subset (shallow or deep copy) 
    KDBTemplate(KDBTemplate* db_parent, const std::string& pattern, const bool copy) 
        : KDB(db_parent, pattern, copy) {}

    // copy constructor
    KDBTemplate(const KDBTemplate& other): KDB(other) {}

    // NOTE: get_obj() and set_obj() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)

    virtual T* get_obj(const SWHDL handle) const = 0;
    virtual T* get_obj(const std::string& name) const = 0;

    virtual bool set_obj(const std::string& name, const T* value) = 0;
};

/*----------------------- GLOBALS ----------------------------*/

KDB* get_global_db(const int iode_type);

inline std::array<std::array<KDB*, 5>, IODE_NB_TYPES> K_RWS = {{ nullptr }};

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

template<typename T>
T* template_subset(T* db_parent, const std::string& pattern, const bool deep_copy=false)
{
    // standalone copy database
    if(deep_copy)
    {
        bool success = false;
        T* db_subset = new T(false);

        if(!db_parent)
            delete db_subset;
        else
        {
            db_subset->k_db_type = DB_STANDALONE;
            db_subset->k_type = db_parent->k_type;
            db_subset->k_mode = db_parent->k_mode;
            db_subset->k_arch = db_parent->k_arch;
            db_subset->description = db_parent->description;
            db_subset->k_compressed = db_parent->k_compressed;
            db_subset->filepath = db_parent->filepath;
            if(db_parent->sample)
                db_subset->sample = new Sample(*db_parent->sample);
            else
                db_subset->sample = nullptr;
            success = db_subset->copy_objs(*db_parent, pattern);
        }

        if(!success)
        {
            delete db_subset;
            std::string error_msg = "Cannot create a subset the database '"; 
            error_msg += v_iode_types[db_parent->k_type] + "' using the pattern '";
            error_msg += pattern + "'";
            throw std::runtime_error(error_msg);
        }
        return db_subset;
    }
    // subset (shallow or deep copy) 
    else
    {
        T* db_subset = new T(db_parent, pattern);
        return db_subset;
    }  
}

/* k_kdb.c */
void K_set_kdb_fullpath(KDB *kdb, U_ch *filename);
int K_merge(KDB* kdb1, KDB* kdb2, int replace);
bool dump_string(char* head, char* txt);
bool print_definition_generic(const std::string& name, char* text);

/* k_val.c */
char *K_oval(KDB *,int ,int );
char *K_oval0(KDB *,int );
char *K_oval1(KDB *,int );

char *K_optr(KDB *,char* ,int );
char *K_optr0(KDB *,char* );
char *K_optr1(KDB *,char* );

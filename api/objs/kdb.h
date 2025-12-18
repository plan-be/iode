#pragma once

#include "scr4/s_swap.h"        // SWHDL
#include "api/constants.h"
#include "api/b_args.h"         // B_ainit_chk
#include "api/b_errors.h"       // error_manager
#include "api/time/sample.h"    // Period, Sample
#include "api/objs/xdr.h"

#include <string>
#include <map>
#include <memory>               // std::shared_ptr
#include <vector>
#include <array>
#include <memory>               // std::unique_ptr, std::shared_ptr


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
    std::map<std::string, SWHDL> k_objs;            // map <object name, position in the SCR memory>
    std::shared_ptr<KDB*> parent = nullptr;         // parent KDB: if an IODE object is added/removed/updated 
                                                    //             from the current KDB, it must done also in the parent KDB
    std::vector<std::shared_ptr<KDB*>> children;    // children KDBs: if the current KDB is modified, all children KDBs must 
                                                    //                be updated too

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

    // shallow copy database
    KDB(KDB* db_parent, const std::string& pattern = "*") : KDBInfo(false)
    {
        this->k_type = db_parent->k_type;

        k_db_type = DB_SHALLOW_COPY;
        this->parent = std::make_shared<KDB*>(db_parent);
        db_parent->children.push_back(std::make_shared<KDB*>(this));

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

        if(db_parent->sample)
            this->sample = new Sample(*db_parent->sample);
        this->description = db_parent->description;
        this->k_compressed = db_parent->k_compressed;
        this->filepath = db_parent->filepath;

        std::string error_msg = "Cannot create a subset the database '";
        error_msg += v_iode_types[this->k_type] + "' using the pattern '";
        error_msg += pattern + "':\n";

        bool failed = false;
        std::vector<std::string> names;
        try
        {
            names = db_parent->filter_names(pattern);
        }
        catch(const std::exception& e)
        {
            failed = true;
            names.clear();
            error_msg += std::string(e.what()) + " in the parent database";
        }
        
        SWHDL handle;
        for(const std::string& name : names) 
        {
            handle = db_parent->get_handle(name);
            if(handle == 0)  
            {
                error_msg += v_iode_types[this->k_type] + " '" + name + "' ";
                error_msg += "not found in the parent database.\n";
                failed = true;
                continue;
            }
            this->k_objs[name] = handle;
        }

        if(failed)
            throw std::runtime_error(error_msg);
    }
    
    // copy constructor
    // NOTE: new database is a 'deep copy' of the passed database
    KDB(const KDB& other) : KDBInfo(other)
    {
        if(other.size() == 0)
            return;

        // copy all objects from other to this
        bool success;
        for(auto& [name, handle] : other.k_objs) 
        {
            success = duplicate(other, name, name);
            if(!success)
            {
                if(this->size() > 0)
                {
                    for(auto& [this_name, this_handle] : this->k_objs) 
                        SW_free(this_handle);
                    this->k_objs.clear();
                }

                filepath = std::string(I_DEFAULT_FILENAME);

                std::string error_msg = "Failed to create a copy of the ";
                error_msg += v_iode_types[k_type]; 
                error_msg += " database.";
                throw std::runtime_error(error_msg);
            }
        }
    }

    ~KDB()
    {
        if(k_db_type != DB_SHALLOW_COPY && this->size() > 0)
        {
            for(auto& [name, handle] : this->k_objs) 
                SW_free(handle);
        }

        if(this->k_objs.size() > 0)
            this->k_objs.clear();
    }

    void clear_objs()
    {
        if(k_db_type != DB_SHALLOW_COPY && this->size() > 0)
        {
            for(auto& [name, handle] : this->k_objs) 
                SW_free(handle);
        }

        if(this->k_objs.size() > 0)
            this->k_objs.clear();
    }

    void clear(bool delete_objs = true)
    {
        KDBInfo::clear();
        if(delete_objs)
            clear_objs();
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
        
        std::vector<std::string> names;
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
        return (int) this->k_objs.size();
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
        return this->k_objs.contains(key);
    }

    int index_of(const std::string& name) const
    {
        std::string key = to_key(name);
        auto it = this->k_objs.find(key);
        if (it != this->k_objs.end()) 
            return (int) std::distance(this->k_objs.begin(), it);
        else
            return -1;
    }

    // NOTE: repeated calls to this function can be inefficient (O(n) each)
    //       To iterate over all names, prefer the C++17 loop syntax:
    //       for(const auto& [name, _] : kdb.k_objs) { ... } 
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

        auto it = this->k_objs.begin();
        std::advance(it, index);
        return const_cast<std::string&>(it->first);
    }

    std::vector<std::string> get_names() const
    {
        std::vector<std::string> names;
        names.reserve(this->size());
        for(const auto& [name, _] : this->k_objs) 
            names.push_back(name);
        return names;
    }

    std::string get_names_as_string() const
    {
        std::string names;
        for(const auto& [name, _] : this->k_objs) 
            names += name + ";";

        // remove last ;
        if(!names.empty() && names.back() == ';')
            names.pop_back();

        return names;
    }

    std::vector<std::string> filter_names(const std::string& pattern, 
        const bool must_exist=true) const
    {
        if(pattern == "*")
            return this->get_names();

        std::vector<std::string> names;
        if(pattern.empty())
            return names;

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
            names.push_back(name);
        }
        SCR_free_tbl((unsigned char **) c_names);

        if(names.size() == 0)
            throw std::runtime_error(error_msg);
        
        // remove duplicates
        sort_and_remove_duplicates(names);
        
        // return names
        return names; 
    }

    SWHDL get_handle(const std::string& name) const
    {
        std::string key = to_key(name);
        auto it = this->k_objs.find(key);
        if (it != this->k_objs.end()) 
            return it->second;
        else
            return 0;
    }

    char* get_ptr_obj(const std::string& name) const
    {
        std::string key = to_key(name);
        auto it = this->k_objs.find(key);
        if (it != this->k_objs.end()) 
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

        if(this->k_objs.size() == 0)
            return false;

        check_name(new_key, k_type);

        auto it = this->k_objs.find(old_key);
        if (it == this->k_objs.end())
        {
            std::string msg = "Cannot rename object: there is no object previously named '" + old_key + 
                              "' found in the database.";
            throw std::invalid_argument(msg);
        }

        if(!overwrite)
        {
            auto it_new = this->k_objs.find(new_key);
            if (it_new != this->k_objs.end())
            {
                std::string msg = "Cannot rename object: an object named '" + new_key + 
                                  "' already exists in the database.";
                throw std::invalid_argument(msg);
            }
        }

        SWHDL handle = it->second;
        this->k_objs.erase(it);
        this->k_objs[new_key] = handle;
        return true;
    }

    bool add_entry(const std::string& name)
    {
        check_name(std::string(name), this->k_type);
        
        bool found = this->contains(name);
        if(found) 
        {
            if(K_WARN_DUP)
            {
                kerror(0, "%s already defined", (char*) name.c_str());
                return false;
            }
        }
        else
            this->k_objs[name] = 0;

        return true;
    }

    bool duplicate(const KDB& other, const std::string& old_name, const std::string& new_name);

    bool remove(const std::string& name)
    {
        std::string key = to_key(name);
        auto it = this->k_objs.find(key);
        if(it == this->k_objs.end())
        {
            std::string str_type = v_iode_types[this->k_type];
            std::string msg = "Cannot remove " + str_type + ": no " + str_type + 
                              " named '" + key + "' found in the database";
            kwarning(msg.c_str());
            return false;
        }

        if(k_db_type != DB_SHALLOW_COPY)
            SW_free(it->second);

        this->k_objs.erase(it);
        return true;
    }

    // other methods

    std::vector<std::string> grep(const std::string& pattern, const bool ecase, 
        const bool names, const bool forms, const bool texts, const char all) const;

    std::string expand(const std::string& pattern, const char all='*') const;

    virtual bool print_obj_def(const std::string& name) = 0;

    char* dde_create_obj(int objnb, int* nc, int* nl);

    // load

    virtual bool load_asc(const std::string& filename) = 0;
    bool load_binary(const int file_type, const std::string& filename, 
                     const std::vector<std::string>& objs=std::vector<std::string>());
    bool load(const std::string& filename);

    // save

    virtual bool save_asc(const std::string& filename) = 0;
    bool save_binary(const std::string& filename, const bool override_filepath = true);
    bool save(const std::string& filename);

    virtual bool save_csv(const std::string& filename, const std::vector<std::string>& 
        varlist=std::vector<std::string>(), Sample* sample=nullptr) 
    { 
        std::string msg = "Cannot save CSV: ";
        msg += "CSV saving is not implemented for database of type ";
        msg += "'" + v_iode_types[this->k_type] + "s'";
        throw std::runtime_error(msg); 
    }
};


template<class T> struct CKDBTemplate: public KDB
{
    // global or standalone database
    CKDBTemplate(const IodeType type, const bool is_global) : KDB(type, is_global) {}

    // shallow copy database
    CKDBTemplate(CKDBTemplate* db_parent, const std::string& pattern = "*") 
        : KDB(db_parent, pattern) {}

    // copy constructor
    CKDBTemplate(const CKDBTemplate& other): KDB(other) {}

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

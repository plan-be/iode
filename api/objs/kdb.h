#pragma once

#include "scr4/s_swap.h"        // SWHDL
#include "api/constants.h"
#include "api/k_super.h"        // kerror
#include "api/time/sample.h"    // Period, Sample

#include <string>
#include <map>
#include <memory>               // std::shared_ptr
#include <vector>


enum IodeDatabaseType
{
    DB_GLOBAL,          //< K_WS[iode_type]
    DB_STANDALONE,      //< isolated database (useful when working with estimation of block of eqs in the GUI)
    DB_SHALLOW_COPY     //< shallow copy (useful when working on subset in the GUI)
};


/*----------------------- STRUCTS ----------------------------*/

struct KDB 
{
    std::map<std::string, SWHDL> k_objs;            // map <object name, position in the SCR memory>
    short       k_type;                             // type of the object: COMMENTS, EQUATIONS, ..., VARIABLES
    short       k_mode;                             // case of the object name: UPPER_CASE, LOWER_CASE or ASIS_CASE 
    std::string k_arch;                             // processor architecture on which the file has been created/saved/loaded
    std::string description = "";                   // short description of the content of the database
    Sample*     sample = nullptr;                   // Sample if Variables database
    char        k_compressed = 0;                   // are the objects compressed in the file ? (LZH method, slow)
    char        k_db_type;                          // type of database: DB_NORMAL (default), DB_STANDALONE, DB_SHALLOW_COPY
    std::string filepath;                           // filepath to the database file
    std::shared_ptr<KDB*> parent = nullptr;         // parent KDB: if an IODE object is added/removed/updated 
                                                    //             from the current KDB, it must done also in the parent KDB
    std::vector<std::shared_ptr<KDB*>> children;    // children KDBs: if the current KDB is modified, all children KDBs must 
                                                    //                be updated too

public:
    KDB(IodeType type, IodeDatabaseType db_type, std::string filename="")
        : k_type((short) type), k_db_type((char) db_type), k_arch(std::string(ARCH)) 
    {
        switch(type) 
        {
            case COMMENTS :
                this->k_mode = (short) ASIS_CASE;
                break;
            case EQUATIONS :
            case IDENTITIES :
            case LISTS :
            case TABLES :
            case VARIABLES :
                this->k_mode = (short) UPPER_CASE;
                break;
            case SCALARS :
                this->k_mode = (short) LOWER_CASE;
                break;
            case OBJECTS :
                this->k_mode = (short) ASIS_CASE;
                break;
        }

        this->filepath = (filename.empty()) ? std::string(I_DEFAULT_FILENAME) : filename;
    }
    
    KDB(KDB& other)
    {
        this->k_db_type = DB_STANDALONE;     // new copy is a deep copy
        this->k_type = other.k_type;
        this->k_mode = other.k_mode;
        this->k_arch = other.k_arch;
        this->description = other.description;
        this->k_compressed = other.k_compressed;
        this->filepath = other.filepath;

        if(other.sample)
            this->sample = new Sample(*other.sample);
        else
            this->sample = nullptr;

        // copy all objects from other to this
        int pos;
        if(other.size() > 0)
        {
            for(auto& [name, handle] : other.k_objs) 
            {
                pos = duplicate(other, name);
                if(pos < 0)
                {
                    if(this->size() > 0)
                    {
                        for(auto& [this_name, this_handle] : this->k_objs) 
                            SW_free(this_handle);
                        this->k_objs.clear();
                    }
    
                    filepath = std::string(I_DEFAULT_FILENAME);
    
                    std::string error_msg = "Cannot create a copy of the database of type '" + v_iode_types[k_type] + "':\n";
                    error_msg += "Cannot copy " + v_iode_types[k_type] + " named '" + name + "'";
                    kerror(0, error_msg.c_str());
                }
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

        if(this->sample)
            delete this->sample;
        this->sample = nullptr;
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
        if(delete_objs)
            clear_objs();

        if(this->sample)
            delete this->sample;
        this->sample = nullptr;

        this->description.clear();
        this->k_compressed = 0;
        this->filepath = I_DEFAULT_FILENAME;
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
            return empty_string;

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

    int duplicate(const KDB& other, const std::string& name);

    bool remove(const std::string& name)
    {
        std::string key = to_key(name);
        auto it = this->k_objs.find(key);
        if (it == this->k_objs.end())
            return false;

        if(k_db_type != DB_SHALLOW_COPY)
            SW_free(it->second);

        this->k_objs.erase(it);
        return true;
    }
};

/*----------------------- GLOBALS ----------------------------*/

inline KDB*     K_WS[7] = { NULL };             // Current workspaces
inline KDB*     K_RWS[7][5] = {{ NULL }};       // Currently loaded workspaces (for printing and identity execution)
inline int      K_PWS[7] = { 0 };               // ??? TODO: check if still in use
inline int      K_WARN_DUP = 0;                 // If null, adding an existing object name in a KDB does not trigger 
                                                // an error (used in K_add_entry())
inline int      K_SECRETSEP = '#';              // pour les macros pushed A#n in reports

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

/*----------------------- DEFINE ----------------------------*/

#define KC_WS   K_WS[COMMENTS]
#define KE_WS   K_WS[EQUATIONS]
#define KI_WS   K_WS[IDENTITIES]
#define KL_WS   K_WS[LISTS]
#define KS_WS   K_WS[SCALARS]
#define KT_WS   K_WS[TABLES]
#define KV_WS   K_WS[VARIABLES]

#define KC_RWS   K_RWS[COMMENTS][K_PWS[COMMENTS]]
#define KE_RWS   K_RWS[EQUATIONS][K_PWS[EQUATIONS]]
#define KI_RWS   K_RWS[IDENTITIES][K_PWS[IDENTITIES]]
#define KL_RWS   K_RWS[LISTS][K_PWS[LISTS]]
#define KS_RWS   K_RWS[SCALARS][K_PWS[SCALARS]]
#define KT_RWS   K_RWS[TABLES][K_PWS[TABLES]]
#define KV_RWS   K_RWS[VARIABLES][K_PWS[VARIABLES]]

/*----------------------- MACROS ----------------------------*/

#define KOVAL(kdb, pos)     K_oval0(kdb, pos)

/*----------------------- FUNCS ----------------------------*/

/* k_kdb.c */
void K_set_kdb_fullpath(KDB *kdb, U_ch *filename);
int K_merge(KDB* kdb1, KDB* kdb2, int replace);
KDB* K_refer(KDB* kdb, int nb, char** names);
KDB *K_quick_refer(KDB* kdb, int nb, char **names);

/* k_val.c */
char *K_oval(KDB *,int ,int );
char *K_oval0(KDB *,int );
char *K_oval1(KDB *,int );

char *K_optr(KDB *,char* ,int );
char *K_optr0(KDB *,char* );
char *K_optr1(KDB *,char* );

// Estimation tests by equation name
double K_etest(KDB* kdb, char*name, int test_nb);
double K_e_stdev (KDB* kdb, char*name);
double K_e_meany (KDB* kdb, char*name);
double K_e_ssres (KDB* kdb, char*name);
double K_e_stderr(KDB* kdb, char*name);
double K_e_fstat (KDB* kdb, char*name);
double K_e_r2    (KDB* kdb, char*name);
double K_e_r2adj (KDB* kdb, char*name);
double K_e_dw    (KDB* kdb, char*name);
double K_e_loglik(KDB* kdb, char*name);

// Values of scalars by name
double K_s_get_info(KDB* kdb, char*name, int info_nb);
double K_s_get_value (KDB* kdb, char*name);
double K_s_get_relax (KDB* kdb, char*name);
double K_s_get_stderr(KDB* kdb, char*name);
double K_s_get_ttest (KDB* kdb, char*name);
int K_s_set_info(KDB* kdb, char*name, int info_nb, double val);
int K_s_set_value (KDB* kdb, char*name, double val);
int K_s_set_relax (KDB* kdb, char*name, double val);
int K_s_set_stderr(KDB* kdb, char*name, double val);

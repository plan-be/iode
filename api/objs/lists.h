#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"       // KDB
#include "api/objs/pack.h"      // P_get_ptr

#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"


/*----------------------- TYPEDEF ----------------------------*/

// using is the C++11 version of typedef
using LIS = char*; 
using List = std::string;

/*----------------------- STRUCTS ----------------------------*/

struct KDBLists : public KDBTemplate<char>
{
    // global or standalone database
    KDBLists(const bool is_global) : KDBTemplate(LISTS, is_global) {}

    // subset (shallow or deep copy) 
    KDBLists(KDBLists* db_parent, const std::string& pattern, const bool copy) 
        : KDBTemplate(db_parent, pattern, copy) {}

    // copy constructor
    KDBLists(const KDBLists& other): KDBTemplate(other) {}

    // NOTE: get_obj() and set_obj() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)

    char* get_obj(const SWHDL handle) const override;
    char* get_obj(const std::string& name) const override;

    bool set_obj(const std::string& name, const char* value) override;
    bool set_obj(const std::string& name, const std::string& value);

    List get(const std::string& name) const;
    bool add(const std::string& name, const List& obj);
    void update(const std::string& name, const List& obj);

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;

    bool print_obj_def(const std::string& name) override;

    void merge_from(const std::string& input_file) override
    {
        KDBLists from(false);
        KDB::merge_from(from, input_file);
    }

    bool copy_from_file(const std::string& file, const std::string& objs_names, 
        std::set<std::string>& v_found)
    {
        KDBLists from(false);
        return KDB::copy_from_file(from, file, objs_names, v_found);
    }

private:
    bool grep_obj(const std::string& name, const SWHDL handle, 
        const std::string& pattern, const bool ecase, const bool forms, 
        const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<KDBLists> global_ws_lst = std::make_unique<KDBLists>(true);
inline std::array<KDBLists*, 5> global_ref_lst = { nullptr };

/*----------------------- FUNCS ----------------------------*/

inline std::size_t hash_value(KDBLists const& cpp_kdb)
{
    if(cpp_kdb.size() == 0)
        return 0;

    std::string list;
    std::size_t seed = 0;
    for(const auto& [name, handle] : cpp_kdb.k_objs)
    {
        hash_combine<std::string>(seed, name); 
        list = cpp_kdb.get(name);
        hash_combine<std::string>(seed, list);
    }

    return seed;
}

/* k_lst.c */
int K_scan(KDB*, char*, char*);
void KE_scan(KDBEquations* dbe, int i, KDBVariables* exo, KDBScalars* scal);
void KI_scan(KDBIdentities* dbi, int i, KDBVariables* exo, KDBScalars* scal);
void KT_scan(KDBTables* dbt, int i, KDBVariables* exo, KDBScalars* scal);
int KL_lst(char*, char** , int);
unsigned char **KL_expand(char*);

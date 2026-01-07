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

// LIS = List (string)
using LIS = char*; 

/*----------------------- STRUCTS ----------------------------*/

struct CKDBLists : public CKDBTemplate<char>
{
    // global or standalone database
    CKDBLists(const bool is_global) : CKDBTemplate(LISTS, is_global) {}

    // shallow copy database
    CKDBLists(CKDBLists* db_parent, const std::string& pattern = "*") 
        : CKDBTemplate(db_parent, pattern) {}

    // copy constructor
    CKDBLists(const CKDBLists& other): CKDBTemplate(other) {}

    // NOTE: get_obj() and set_obj() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)

    char* get_obj(const SWHDL handle) const override;
    char* get_obj(const std::string& name) const override;

    bool set_obj(const std::string& name, const char* value) override;
    bool set_obj(const std::string& name, const std::string& value);

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl) override;

    bool print_obj_def(const std::string& name) override;


private:
    bool grep_obj(const std::string& name, const SWHDL handle, 
        const std::string& pattern, const bool ecase, const bool forms, 
        const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<CKDBLists> global_ws_lst = std::make_unique<CKDBLists>(true);

/*----------------------- FUNCS ----------------------------*/

/* k_lst.c */
int K_scan(KDB*, char*, char*);
void KE_scan(CKDBEquations* dbe, int i, CKDBVariables* exo, CKDBScalars* scal);
void KI_scan(CKDBIdentities* dbi, int i, CKDBVariables* exo, CKDBScalars* scal);
void KT_scan(CKDBTables* dbt, int i, CKDBVariables* exo, CKDBScalars* scal);
int KL_lst(char*, char** , int);
unsigned char **KL_expand(char*);

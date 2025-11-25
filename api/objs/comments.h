#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"

/*----------------------- TYPEDEF ----------------------------*/

// CMT = Comment (string)
using CMT = char*;

/*----------------------- STRUCTS ----------------------------*/

struct CKDBComments : public CKDBTemplate<CMT>
{
    // TEMPORARY: to be removed when KDBTemplate will be merged into CKDBTemplate
    CKDBComments(const IodeType type, const bool is_global) : CKDBTemplate(COMMENTS, is_global) {} 

    // global or standalone database
    CKDBComments(const bool is_global) : CKDBTemplate(COMMENTS, is_global) {}

    // shallow copy database
    CKDBComments(CKDBComments* db_parent, const std::string& pattern = "*") 
        : CKDBTemplate(db_parent, pattern) {}

    // copy constructor
    CKDBComments(const CKDBComments& other): CKDBTemplate(other) {}

    // NOTE: get_obj() and set_obj() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)

    CMT get_obj(const SWHDL handle) const;
    CMT get_obj(const std::string& name) const;

    void set_obj(const std::string& name, const CMT& value);

    bool load_asc(const std::string& filename) override;
    bool save_asc(const std::string& filename) override;

    char* dde_create_obj_by_name(const std::string& name, int* nc, int* nl);

    bool print_obj_def(const std::string& name);

private:
    bool grep_obj(const std::string& name, const SWHDL handle, 
        const std::string& pattern, const bool ecase, const bool forms, 
        const bool texts, const char all) const override;
    
    void update_reference_db() override;
};

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<CKDBComments> global_ws_cmt = std::make_unique<CKDBComments>(true);

/*----------------------- MACROS ----------------------------*/

// Returns a pointer to the CMT in swap (not allocated -> do not free)
inline CMT KCVAL(const CKDBComments* kdb, const std::string& name)
{
    return kdb->get_obj(name);            
}

inline CMT KCVAL(const CKDBComments* kdb, SWHDL handle)
{
    return kdb->get_obj(handle);            
}

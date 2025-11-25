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
    CKDBComments(const IodeType type, const IodeDatabaseType db_type, const std::string filename="")
        : CKDBTemplate(type, db_type, filename) {}

    CKDBComments(const CKDBComments& other): CKDBTemplate(other) {}

    // NOTE: get_obj() and set_obj() methods to be replaced by operator[] when 
    //       k_objs will be changed to std::map<std::string, T>
    //       T& operator[](const std::string& name)

    CMT get_obj(const SWHDL handle) const;
    CMT get_obj(const std::string& name) const;

    void set_obj(const std::string& name, const CMT& value);
};

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<CKDBComments> global_ws_cmt = std::make_unique<CKDBComments>(COMMENTS, DB_GLOBAL);

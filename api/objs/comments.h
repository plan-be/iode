#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"

/*----------------------- TYPEDEF ----------------------------*/

// CMT = Comment (string)
using CMT = char*;

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<KDB> global_ws_cmt = std::make_unique<KDB>(COMMENTS, DB_GLOBAL);

/*----------------------- MACROS ----------------------------*/

// Returns a pointer to the CMT in swap (not allocated -> do not free)
inline CMT KCVAL(KDB* kdb, const std::string& name)
{
    return K_optr0(kdb, (char*) name.c_str());            
}

inline CMT KCVAL(KDB* kdb, SWHDL handle)
{
    return (CMT) P_get_ptr(SW_getptr(handle), 0);            
}

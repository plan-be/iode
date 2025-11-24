#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"       // KDB
#include "api/objs/pack.h"      // P_get_ptr


/*----------------------- TYPEDEF ----------------------------*/

// LIS = List (string)
using LIS = char*; 

/*----------------------- GLOBALS ----------------------------*/
// unique_ptr -> automatic memory management
//            -> no need to delete KDB workspaces manually
inline std::unique_ptr<KDB> global_ws_lst = std::make_unique<KDB>(LISTS, DB_GLOBAL);

/*----------------------- FUNCS ----------------------------*/

inline LIS KLVAL(KDB* kdb, const std::string& name)
{
    return (LIS) K_optr0(kdb, (char*) name.c_str());
} 

inline LIS KLVAL(KDB* kdb, SWHDL handle)
{
    return (LIS) P_get_ptr(SW_getptr(handle), 0);     
} 


/* k_lst.c */
int K_scan(KDB *,char *,char *);
//void K_clecscan(KDB *,CLEC *,KDB *,KDB *);
void KE_scan(KDB *,int ,KDB *,KDB *);
void KI_scan(KDB *,int ,KDB *,KDB *);
void KT_scan(KDB *,int ,KDB *,KDB *);
int KL_lst(char *,char **,int );
unsigned char **KL_expand(char *);

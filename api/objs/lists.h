#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"       // KDB

/*----------------------- TYPEDEF ----------------------------*/

// LIS = List (string)
using LIS = char*; 

/*----------------------- INLINE FUNCS ----------------------------*/

inline LIS KLPTR(const char* name) 
{
    return (LIS) K_optr0(KL_WS, (char*) name);
}

inline LIS KLVAL(KDB* kdb, int pos) 
{
    return (LIS) K_oval0(kdb, pos);
}

/*----------------------- FUNCS ----------------------------*/

/* k_lst.c */
int K_scan(KDB *,char *,char *);
//void K_clecscan(KDB *,CLEC *,KDB *,KDB *);
void KE_scan(KDB *,int ,KDB *,KDB *);
void KI_scan(KDB *,int ,KDB *,KDB *);
void KT_scan(KDB *,int ,KDB *,KDB *);
int KL_lst(char *,char **,int );
unsigned char **KL_expand(char *);

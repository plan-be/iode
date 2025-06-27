#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------- TYPEDEF ----------------------------*/

// LIS = List (string)
typedef char    *LIS;

/*----------------------- MACROS ----------------------------*/

#define KLPTR(name)         K_optr0(KL_WS, name)

/*----------------------- FUNCS ----------------------------*/

/* k_lst.c */
extern int K_scan(KDB *,char *,char *);
//extern void K_clecscan(KDB *,CLEC *,KDB *,KDB *);
extern void KE_scan(KDB *,int ,KDB *,KDB *);
extern void KI_scan(KDB *,int ,KDB *,KDB *);
extern void KT_scan(KDB *,int ,KDB *,KDB *);
extern int KL_lst(char *,char **,int );
extern unsigned char **KL_expand(char *);

#ifdef __cplusplus
}
#endif

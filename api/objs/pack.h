#pragma once

#include "api/constants.h"
#include "api/objs/identities.h"
#include "api/objs/equations.h"
#include "api/objs/tables.h"

#ifdef __cplusplus
extern "C" {
#endif

// Table of function ptrs
extern  int   (*K_pack[])();

/* pack.c */
extern int P_free(char *);
extern void *P_create();
extern void *P_add(void *,void *,int );
extern void *P_get_ptr(void *,int );
extern OSIZE P_get_len(void *,int );
extern void *P_alloc_get_ptr(void *, int );

/* k_pack.c */
extern int K_vpack(char **,double *,int *);
extern int KV_alloc_var(int );
extern int K_ipack(char **,char *);
//extern char *K_tcell_pack(char *,TCELL *);
extern int K_tpack(char **,char *);
extern int K_epack(char **,char *,char *);
extern int K_spack(char **,char *);
extern int KS_alloc_scl(void);
extern int K_cpack(char **,char *);
extern int K_lpack(char **,char *);
extern int K_opack(char **,char *,int *);
//extern void K_vunpack(void);
extern TBL *K_tunpack(char *);
extern EQ *K_eunpack(char *, char *);
extern IDT* K_iunpack(char *);
//extern void K_sunpack(void);
//extern void K_cunpack(void);
//extern void K_lunpack(void);
//extern void K_onpack(void);

#ifdef __cplusplus
}
#endif

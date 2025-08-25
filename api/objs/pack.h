#pragma once

#include "api/constants.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"


/* pack.c */
int P_free(char *);
void *P_create();
void *P_add(void *,void *,int );
void *P_get_ptr(void *,int );
OSIZE P_get_len(void *,int );
void *P_alloc_get_ptr(void *, int );

/* k_pack.c */
int K_vpack(char **,double *,int *);
int KV_alloc_var(int );
int K_ipack(char **,char *);
//char *K_tcell_pack(char *,TCELL *);
int K_tpack(char **,char *);
int K_spack(char **,char *);
int KS_alloc_scl(void);
int K_cpack(char **,char *);
int K_lpack(char **,char *);
int K_opack(char **,char *,int *);
//void K_vunpack(void);
TBL *K_tunpack(char *);
Identity* K_iunpack(char *);
//void K_sunpack(void);
//void K_cunpack(void);
//void K_lunpack(void);
//void K_onpack(void);

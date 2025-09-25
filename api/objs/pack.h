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
//char *K_tcell_pack(char *,TableCell *);
int K_tpack(char**, char*, char* name = NULL);
int K_spack(char**, char*);
int KS_alloc_scl(void);
int K_cpack(char **,char *);
int K_lpack(char **,char *);
int K_opack(char **,char *,int *);
//void K_vunpack(void);
TBL *K_tunpack(char*, char* name = NULL);
Identity* K_iunpack(char *);
//void K_sunpack(void);
//void K_cunpack(void);
//void K_lunpack(void);
//void K_onpack(void);


extern bool debug_pack;
extern bool debug_unpack;

inline void debug_packing(const std::string& element, const std::string& code, int p, int line=-1, 
    int col=-1, const std::string& value = "", int len=-1)
{
    if(!debug_pack)
        return;
    
    std::cout << "Packing " << element << " " << code;
    if(p >= 0)
        std::cout << " (p = " << p << ") ";
    if(line >= 0)
        std::cout << "(line " << line;
    if(col >= 0)
        std::cout << ", column " << col;
    if(line >= 0)
        std::cout << ") ";

    if(len >= 0)
        std::cout << "(len=" << len << ") ";

    if(!value.empty())
        std::cout << value;
    
    std::cout << std::endl;
}

inline void debug_unpacking(const std::string& element, const std::string& code, int p, int line=-1, 
    int col=-1, const std::string& value="")
{
    if(!debug_unpack)
        return;
    
    std::cout << "Unpacking " << element << " " << code;
    if(p >= 0)
        std::cout << " (p = " << p << ") ";
    if(line >= 0)
        std::cout << "(line " << line;
    if(col >= 0)
        std::cout << ", column " << col;
    if(line >= 0)
        std::cout << ") ";

    if(!value.empty())
        std::cout << value;
    
    std::cout << std::endl;
}
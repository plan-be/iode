#pragma once
#include "api/constants.h"


/* pack.c */
int P_free(char* ptr);
void *P_create();
void *P_add(void* vptr1, void* vptr2, int lg);
void *P_get_ptr(void* vptr, int i);
OSIZE P_get_len(void* vptr, int i);
void *P_alloc_get_ptr(void* ptr, int p);

/* k_pack.c */
bool K_opack(char** pack, char* a1, int* a2);

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
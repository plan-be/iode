#pragma once
#include "api/constants.h"

extern int (*K_xdrobj[])(unsigned char* ptr, unsigned char** xdr_ptr);

/* k_xdr.c */
void K_xdrPINT(unsigned char *);
void K_xdrPLONG(unsigned char *);
void K_xdrSMPL(unsigned char* a);

#define K_xdrSHORT(a)   XDR_rev(a, 1, sizeof(short))
#define K_xdrLONG(a)    XDR_rev(a, 1, sizeof(long))
#define K_xdrREAL(a)    XDR_rev(a, 1, sizeof(double))
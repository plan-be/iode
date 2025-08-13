#pragma once

#include "api/constants.h"
#include "api/objs/kdb.h"       // KDB

extern  int  (*K_xdrobj[])(unsigned char* ptr, unsigned char** xdr_ptr);

/* k_xdr.c */
void K_xdrPINT(unsigned char *);
void K_xdrPLONG(unsigned char *);
void K_xdrKDB(KDB *,KDB **);

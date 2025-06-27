#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern  int  (*K_xdrobj[])();

/* k_xdr.c */
extern void K_xdrPINT(unsigned char *);
extern void K_xdrPLONG(unsigned char *);
extern void K_xdrKDB(KDB *,KDB **);

#ifdef __cplusplus
}
#endif

#pragma once 

#include "api/constants.h"
#include "api/utils/time.h"

#ifdef __cplusplus
extern "C" {
#endif

/* k_super.c */
extern int     (*kerror_super)(const int level, const char* fmt);
extern void    (*kwarning_super)(const char* msg);
extern void    (*kpause_super)();
extern void    (*kmsg_super)(const char* fmt);
extern int     (*kwprintf_super)(const char* msg);
extern void    (*kpanic_super)();
extern int     (*kconfirm_super)(const char* msg);
extern int     (*kmsgbox_super)(const unsigned char* str, const unsigned char* v, const unsigned char** buts);
extern int     kmsgbox_continue;
extern int     kpause_continue;
extern void    (*krecordkey_super)(const int ch);
extern void    (*krecordtext_super)(const unsigned char* text);
extern void    (*ksettitle_super)(void);
extern int     (*ktermvkey_super)(const int vkey);
extern int     (*khitkey_super)();
extern int     (*kgetkey_super)();
extern void    (*kbeep_super)(void);
extern SAMPLE* (*kasksmpl_super)(void);
extern int     (*kexecsystem_super)(const char*);
extern int     (*kshellexec_super )(const char*);
extern int     (*ODE_end_super)(const int);

extern int     kerror(const int level, const char* fmt, ...);
extern void    kwarning(const char* fmt, ...);
extern void    kpause();
extern void    kmsg(const char* fmt, ...);
extern void    kmsg_toggle(const int IsOn);
extern int     kwprintf(const char* msg, ...);
extern void    kpanic(void);
extern int     kconfirm(const char* fmt, ...);
extern int     kmsgbox(const unsigned char *str, const unsigned char *v, const unsigned char **buts);
extern void    krecordkey(const int ch);
extern void    krecordtext(const unsigned char* text);
extern void    ksettitle(void);
extern int     ktermvkey(const int vkey);
extern int     khitkey();
extern int     kgetkey();
extern void    kbeep();
extern SAMPLE* kasksmpl();
extern int     kexecsystem(const char*);
extern int     kshellexec(const char*);
extern char    *A_expand_super_API(const char* name);
extern int     ODE_end(const int st);
extern void    IODE_assign_super_API(void);

#ifdef __cplusplus
}
#endif

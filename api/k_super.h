#pragma once 

#include "api/constants.h"
#include "api/utils/time.h"

/* k_super.c */
int     (*kerror_super)(const int level, const char* fmt);
void    (*kwarning_super)(const char* msg);
void    (*kpause_super)();
void    (*kmsg_super)(const char* fmt);
int     (*kwprintf_super)(const char* msg);
void    (*kpanic_super)();
int     (*kconfirm_super)(const char* msg);
int     (*kmsgbox_super)(const unsigned char* str, const unsigned char* v, const unsigned char** buts);
int     kmsgbox_continue;
int     kpause_continue;
void    (*krecordkey_super)(const int ch);
void    (*krecordtext_super)(const unsigned char* text);
void    (*ksettitle_super)(void);
int     (*ktermvkey_super)(const int vkey);
int     (*khitkey_super)();
int     (*kgetkey_super)();
void    (*kbeep_super)(void);
SAMPLE* (*kasksmpl_super)(void);
int     (*kexecsystem_super)(const char*);
int     (*kshellexec_super )(const char*);
int     (*ODE_end_super)(const int);

int     kerror(const int level, const char* fmt, ...);
void    kwarning(const char* fmt, ...);
void    kpause();
void    kmsg(const char* fmt, ...);
void    kmsg_toggle(const int IsOn);
int     kwprintf(const char* msg, ...);
void    kpanic(void);
int     kconfirm(const char* fmt, ...);
int     kmsgbox(const unsigned char *str, const unsigned char *v, const unsigned char **buts);
void    krecordkey(const int ch);
void    krecordtext(const unsigned char* text);
void    ksettitle(void);
int     ktermvkey(const int vkey);
int     khitkey();
int     kgetkey();
void    kbeep();
SAMPLE* kasksmpl();
int     kexecsystem(const char*);
int     kshellexec(const char*);
char    *A_expand_super_API(char* name);
int     ODE_end(const int st);
void    IODE_assign_super_API(void);

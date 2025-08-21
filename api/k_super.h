#pragma once 

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"


inline int     MSG_DISABLED = 0;    // if 1, kmsg() is disabled

inline int     kmsgbox_continue = 0;
inline int     kpause_continue = 0;

inline int     (*kerror_super)(const int level, const char* fmt) = nullptr;
inline void    (*kwarning_super)(const char* msg) = nullptr;
inline void    (*kpause_super)() = nullptr;
inline void    (*kmsg_super)(const char* fmt) = nullptr;
inline int     (*kwprintf_super)(const char* msg) = nullptr;
inline void    (*kpanic_super)() = nullptr;
inline int     (*kconfirm_super)(const char* msg) = nullptr;
inline int     (*kmsgbox_super)(const unsigned char* str, const unsigned char* v, const unsigned char** buts) = nullptr;
inline void    (*krecordkey_super)(const int ch) = nullptr;
inline void    (*krecordtext_super)(const unsigned char* text) = nullptr;
inline void    (*ksettitle_super)(void) = nullptr;
inline int     (*ktermvkey_super)(const int vkey) = nullptr;
inline int     (*khitkey_super)() = nullptr;
inline int     (*kgetkey_super)() = nullptr;
inline void    (*kbeep_super)(void) = nullptr;
inline Sample* (*kasksmpl_super)(void) = nullptr;
inline int     (*kexecsystem_super)(const char*) = nullptr;
inline int     (*kshellexec_super )(const char*) = nullptr;
inline int     (*ODE_end_super)(const int) = nullptr;

/* k_super.c */
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
Sample* kasksmpl();
int     kexecsystem(const char*);
int     kshellexec(const char*);
char    *A_expand_super_API(char* name);
int     ODE_end(const int st);
void    IODE_assign_super_API(void);

#pragma once 

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"


inline bool     MSG_DISABLED = false;

inline bool     KMSGBOX_CONTINUE = false;
inline bool     KPAUSE_CONTINUE = false;

inline int     (*kerror_super)(const int level, const char* fmt) = nullptr;
inline void    (*kwarning_super)(const char* msg) = nullptr;
inline void    (*kpause_super)() = nullptr;
inline void    (*kmsg_super)(const char* fmt) = nullptr;
inline int     (*kwprintf_super)(const char* msg) = nullptr;
inline void    (*kpanic_super)() = nullptr;
inline int     (*kconfirm_super)(const char* msg) = nullptr;
inline void    (*kinformation_super)(const char* fmt) = nullptr;
inline void    (*krecordkey_super)(const int ch) = nullptr;
inline void    (*krecordtext_super)(const unsigned char* text) = nullptr;
inline void    (*ksettitle_super)(void) = nullptr;
inline int     (*ktermvkey_super)(const int vkey) = nullptr;
inline int     (*khitkey_super)() = nullptr;
inline int     (*kgetkey_super)() = nullptr;
inline void    (*kbeep_super)(void) = nullptr;
inline std::shared_ptr<Sample> (*kasksmpl_super)(void) = nullptr;
inline int     (*kexecsystem_super)(const char*) = nullptr;
inline int     (*kshellexec_super )(const char*) = nullptr;
inline int     (*ODE_end_super)(const int) = nullptr;

/* k_super.c */
void    skip_message(const bool value);
void    skip_pause(const bool value);
void    skip_msg_box(const bool value);

int     kerror(const int level, const char* fmt, ...);
void    kwarning(const char* fmt, ...);
void    kpause();
void    kinformation(const char* fmt, ...);
void    kmsg(const char* fmt, ...);
int     kwprintf(const char* msg, ...);
void    kpanic(void);
int     kconfirm(const char* fmt, ...);
void    krecordkey(const int key);
void    krecordtext(const unsigned char* text);
void    ksettitle(void);
int     ktermvkey(const int vkey);
int     khitkey();
int     kgetkey();
void    kbeep();
std::shared_ptr<Sample> kasksmpl();
int     kexecsystem(const char* arg);
int     kshellexec(const char* arg);
char    *A_expand_super_API(char* name);
int     ODE_end(const int st);
void    IODE_assign_super_API(void);

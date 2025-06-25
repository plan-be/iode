#pragma once
#include "s_ini.h"

#ifdef SCRCPP
extern "C" {
#endif

/* s_rdini.c */
extern INIFILE *IniOpen(char *filename,int usection,int uparam);
extern int IniClose(INIFILE *inif);
extern int IniUnRead(INIFILE *inif);
extern int IniRead(INIFILE *inif);
extern int IniSetData(unsigned char **adr,unsigned char *txt,int ucase);
extern int IniSetParam(INIFILE *inif,unsigned char *txt);
extern int IniSetSection(INIFILE *inif,unsigned char *txt);
extern INIFILE *IniOpenSection(unsigned char *filename,unsigned char *section,int usection,int uparam);
extern int IniReadYNParm(char* filename, char* section, char* parm);
extern unsigned char **IniReadSection(char *filename,char *section);
extern unsigned char **IniReadCurSection(INIFILE *inif);
extern unsigned char *IniError(INIFILE *inif);
extern int IniReadTxtParm(unsigned char *filename,unsigned char *section,unsigned char *parm,unsigned char *val,int lg);
extern int IniReadNumParm(unsigned char *filename,unsigned char *section,unsigned char *parm,int *num);
extern int IniWriteParm(char* filename, char* section, char* parm, char* val);
extern unsigned char **IniReadAllSections(unsigned char *filename);

#ifdef SCRCPP
}
#endif

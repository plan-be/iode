#pragma once

extern char* ODE_INIFILE;
extern void B_IodeIniFile();
extern int B_IniReadText(char* section, char* parm, char* res, int maxlen, char* dft);
extern int B_IniWriteText(char* section, char* parm, char* val);
extern int B_IniReadChar(char* section, char* parm, char dft);
extern int B_IniWriteChar(char* section, char* parm, char val);
extern int B_IniReadNum(char* section, char* parm, int dft);
extern int B_IniWriteNum(char* section, char* parm, int val);
extern int B_IniReadYN(char* section, char* parm, int dft);
extern int B_IniWriteYN(char* section, char* parm, int val);

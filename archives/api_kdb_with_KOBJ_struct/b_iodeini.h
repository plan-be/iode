#pragma once

#include "api/constants.h"

inline char* ODE_INIFILE = 0;          // Name of the iode.ini file

void B_IodeIniFile();
int B_IniReadText(char* section, char* parm, char* res, int maxlen, char* dft);
int B_IniWriteText(char* section, char* parm, char* val);
int B_IniReadChar(char* section, char* parm, char dft);
int B_IniWriteChar(char* section, char* parm, char val);
int B_IniReadNum(char* section, char* parm, int dft);
int B_IniWriteNum(char* section, char* parm, int val);
int B_IniReadYN(char* section, char* parm, int dft);
int B_IniWriteYN(char* section, char* parm, int val);

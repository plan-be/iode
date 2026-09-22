#pragma once
#include "api/iode_scr4.h"
#include "scr4/a2m/s_a2m.h"
#include "scr4/strs/s_prost.h"       // SCR_free, SCR_stracpy, SCR_strip
#include "scr4/base/s_prodir.h"      // SCR_split_dir
#include "scr4/ini/s_proini.h"      // IniReadTxtParm, IniWriteParm
#include "scr4/a2m/s_proa2m.h"      // A2mGIF_HTML

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

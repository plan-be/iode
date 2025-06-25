/**
 *  @header4iode
 * 
 *  Functions to read and write parameters in iode.ini. 
 *  
 *  List of functions
 *  -----------------
 *      void B_IodeIniFile()                                                                Retrieves the path to the iode.ini file 
 *      int B_IniReadText(char* section, char* parm, char* res, int maxlen, char* dft)      Reads a text parameter in the current iode.ini file.
 *      int B_IniReadChar(char* section, char* parm, char dft)                              Reads a character parameter in the current iode.ini file.
 *      int B_IniReadNum(char* section, char* parm, int dft)                                Reads a integer parameter in the current iode.ini file. 
 *      int B_IniReadYN(char* section, char* parm, int dft)                                 Reads a Y/N parameter in the current iode.ini file. 
 *      int B_IniWriteText(char* section, char* parm, char* val)                            Saves a text parameter in the current iode.ini file
 *      int B_IniWriteChar(char* section, char* parm, char val)                             Saves a char parameter in the current iode.ini file
 *      int B_IniWriteNum(char* section, char* parm, int val)                               Saves a integer parameter in the current iode.ini file
 *      int B_IniWriteYN(char* section, char* parm, int val)                                Saves a boolean parameter in the current iode.ini file    
 *
 *  List of global variables
 *  ------------------------
 *      char* ODE_INIFILE       Name of the iode.ini file
 */

#include <Windows.h>
#include "scr4/s_prost.h"       // SCR_free, SCR_stracpy, SCR_strip
#include "scr4/s_prodir.h"      // SCR_split_dir
#include "scr4/s_proini.h"      // IniReadTxtParm, IniWriteParm
#include "b_iodeini.h"


char* ODE_INIFILE = 0;          // Name of the iode.ini file

/**
 *  Retrieves the path to the iode.ini file and stores the result in the global ODE_INIFILE. 
 *  The path is constructed by appending "iode.ini" to the path of the current executable.
 *  
 *  If ODE_INIFILE is not null (the path is already calculated), the function returns immediately.
 */
void B_IodeIniFile()
{
	char 	module[1024], file[1024];

    // Get module directory (c:/iode p.ex)
    if(ODE_INIFILE) return;
#ifdef __GNUC__
    readlink("/proc/self/exe", module, sizeof(module) - 1);
#else
    GetModuleFileName(0, module, 1000);
#endif
    SCR_split_dir(module, file);
	sprintf(file, "%s\\iode.ini", module);
	ODE_INIFILE = SCR_stracpy(file);
    //printf("iode.ini file: %s\n", ODE_INIFILE);
}


/**
 *  Reads a text parameter in the current iode.ini file. This file path is calculed by B_IodeIniFile().
 *  The read parameter must be located in [section] parm=xxx.
 *  
 *  Ex.
 *      [A2MGNL]        -> section=A2MGNL
 *      ESC=\           -> parm=ESC, value=\
 *      SEP=@
    
 *      [A2MGDI]
 *      PGHEADER=Header
 *      PGHEADER=Footer %d
 *      FONTSIZE=10
 *  
 *  
 *  @param [in] section     char*   section of the parameter: a section starts with a line containing [section]
 *  @param [in] parm        char*   parameter in [section]: keyword followed by =
 *  @param [in] res         char*   pointer to the result
 *  @param [in] maxlen      int     maximum length of res
 *  @param [in] dft         char*   if [section] parm= is not found, dft is copied to res
 *  @return                 int     0 if the entry is found, -1 otherwise
 *  
 */
int B_IniReadText(char* section, char* parm, char* res, int maxlen, char* dft)
{
    int     rc;
    
    B_IodeIniFile();
	rc =  IniReadTxtParm(ODE_INIFILE, section, parm, res, maxlen);
    if(rc < 0) 
        strcpy(res, dft);
    return(rc);
}


/**
 *  Saves a text parameter in the current iode.ini file. This file path is calculed by B_IodeIniFile().
 *  The written parameter will be located in [section] parm=xxx.
 *  
 *  @param [in] section     char*   section of the parameter: a section starts with a line containing [section]
 *  @param [in] parm        char*   parameter in [section]: keyword followed by =
 *  @param [in] val         char*   pointer to the parameter value
 *  @return                 int     0 if the new entry is saved, -1 otherwise (e.e.: file not writable)
 */
int B_IniWriteText(char* section, char* parm, char* val)
{
    B_IodeIniFile();
	return(IniWriteParm(ODE_INIFILE, section, parm, val));
}


/**
 *  Reads a character parameter in the current iode.ini file. 
 *  
 *  See B_IniReadText() for an example.
 *  
 *  @param [in] section     char*   section of the parameter: a section starts with a line containing [section]
 *  @param [in] parm        char*   parameter in [section]: keyword followed by =
 *  @param [in] dft         char    default value if [section] parm= is not found, 
 *  @return                 int     the entry found or dft is not found
 *  
 */
int B_IniReadChar(char* section, char* parm, char dft)
{
    char    buf[81];
    int     rc;
    
    B_IodeIniFile();
	rc =  IniReadTxtParm(ODE_INIFILE, section, parm, buf, 80);
    return(rc < 0 ? dft : buf[0]);
}


/**
 *  Saves a character parameter in the current iode.ini file. This file path is calculed by B_IodeIniFile().
 *  The written parameter will be located in [section] parm=xxx.
 *  
 *  @param [in] section     char*   section of the parameter: a section starts with a line containing [section]
 *  @param [in] parm        char*   parameter in [section]: keyword followed by =
 *  @param [in] val         int     new value of the parameter 
 *  @return                 int     0 if the new entry is saved, -1 otherwise (e.e.: file not writable)
 *  
 */
int B_IniWriteChar(char* section, char* parm, char val)
{
    char    buf[81];

    buf[0] = val;
    buf[1] = 0;
    return(B_IniWriteText(section, parm, buf));
}


/**
 *  Reads a integer parameter in the current iode.ini file. 
 *  
 *  See B_IniReadText() for an example.
 *  
 *  @param [in] section     char*   section of the parameter: a section starts with a line containing [section]
 *  @param [in] parm        char*   parameter in [section]: keyword followed by =
 *  @param [in] dft         char    default value if [section] parm= is not found, 
 *  @return                 int     the entry found or dft is not found
 *  
 */
int B_IniReadNum(char* section, char* parm, int dft)
{
    int val, res;
    
    B_IodeIniFile();
	res = IniReadNumParm(ODE_INIFILE, section, parm, &val);
    return(res < 0 ? dft : val);
}


/**
 *  Saves a integer parameter in the current iode.ini file. This file path is calculed by B_IodeIniFile().
 *  The written parameter will be located in [section] parm=xxx.
 *  
 *  @param [in] section     char*   section of the parameter: a section starts with a line containing [section]
 *  @param [in] parm        char*   parameter in [section]: keyword followed by =
 *  @param [in] val         int     new value of the parameter 
 *  @return                 int     0 if the new entry is saved, -1 otherwise (e.e.: file not writable)
 */
int B_IniWriteNum(char* section, char* parm, int val)
{
    char    buf[512];

    sprintf(buf, "%d", val);
    return(B_IniWriteText(section, parm, buf));
}


/**
 *  Reads a Y/N parameter in the current iode.ini file. 
 *  
 *  See B_IniReadText() for an example.
 *  
 *  @param [in] section     char*   section of the parameter: a section starts with a line containing [section]
 *  @param [in] parm        char*   parameter in [section]: keyword followed by =
 *  @param [in] dft         char    default value if [section] parm= is not found, 
 *  @return                 int     1 if the parameter begins with y,j,1 or o. 0 otherwise
 *                                  dft if the [section] parm is not found
 */
int B_IniReadYN(char* section, char* parm, int dft)
{
    int res;
    
    B_IodeIniFile();
	res = IniReadYNParm(ODE_INIFILE, section, parm);
    return((res < 0) ? dft : res);
}


/**
 *  Saves a boolean parameter in the current iode.ini file. This file path is calculed by B_IodeIniFile().
 *  The written parameter will be located in [section] parm=xxx.
 *  
 *  @param [in] section     char*   section of the parameter: a section starts with a line containing [section]
 *  @param [in] parm        char*   parameter in [section]: keyword followed by =
 *  @param [in] val         int     new value of the parameter: if null, 0 is written, if not null, 1 is written
 *  @return                 int     0 if the new entry is saved, -1 otherwise (e.e.: file not writable)
 */
int B_IniWriteYN(char* section, char* parm, int val)
{
    char    buf[512];
    
    if(val) strcpy(buf, "1");
    else    strcpy(buf, "0");
    
    return(B_IniWriteText(section, parm, buf));
}

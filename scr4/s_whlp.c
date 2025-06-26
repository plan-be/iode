#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"
#include <htmlhelp.h>


int     WHLP_ACTIVE = 0;
char    HLP_NAME[SCR_MAX_FILE_LG + 1]; // JMP 5/9/2015
extern int SCR_WIN_HELP_ACTIF;          /* BP_M 20-02-2003 12:23 */

/*NH
Retourne le type du fichier d'aide.
&RT
    0 : pas de type défini
    1 : .hlp
    2 : .chm
*/

HLP_filetype(char *filename)
{
    char    ext[10];

    if(filename == 0 || strlen(filename) <= 4) return(0);
    SCR_strlcpy(ext, filename + strlen(filename) - 3, 3);
    SCR_lower(ext);
    if(strcmp(ext, "hlp") ==0) return(1);
    if(strcmp(ext, "chm") ==0) return(2);
    else return(0);
}

/*NH
Calcule le nom du fichier d'aide en ajoutant le path du programme en cours.
*/
char *HLP_FILENAME = 0;

char *HLP_filename(char *filename)
{
    char    file[1024], dir[1024], module[1024];

    if(filename == 0 || strlen(filename) <= 4) return(HLP_FILENAME);

    // Get module directory (c:/iode p.ex)
    GetModuleFileName(hInst, module, 1000);
    SCR_split_dir(module, file);

    // Get help file name
    strcpy(dir, filename);
    SCR_split_dir(dir, file);

    // Join module dir and filename
    if(HLP_FILENAME) SCR_free(HLP_FILENAME);
    HLP_FILENAME = SCR_malloc(strlen(module) + strlen(file) +2);

    sprintf(HLP_FILENAME, "%s\\%s", module, file);
    return(HLP_FILENAME);
}

/*NH*/
HLP_edit(topic)
char   *topic;
{
    //HWND    hWnd = WscrGetMainWnd(); /* JMP 25-01-00 */
	char	buf[2048]; // JMP 5/9/2015

    if(!SCR_WIN_HELP_ACTIF) return(0);      /* BP_M 20-02-2003 12:23 */


    if(HLP_NAME[0] == 0) {
        PG_display_error(SCR_err_txt(102));
        return(-1);
    }

    HLP_filename(HLP_NAME);
    switch(HLP_filetype(HLP_NAME)) {
    case 1 :
        WHLP_ACTIVE = 1;
        if(topic == 0 || topic[0] == 0) {
            //WinHelp(hWnd, HLP_NAME, HELP_FINDER, 0L);
            WinHelp(GetDesktopWindow(), HLP_NAME, HELP_FINDER, 0L);
        }
        else {
            SCR_OemToAnsi(topic, topic);
            //WinHelp(hWnd, HLP_NAME, HELP_PARTIALKEY, (DWORD)topic);
            WinHelp(GetDesktopWindow(), HLP_FILENAME, HELP_PARTIALKEY, (DWORD)topic);
            SCR_AnsiToOem(topic, topic);
        }
        break;
    case 2 :
		sprintf(buf, "hh %s::T0001.htm", HLP_FILENAME);
        WscrWinExec(buf);
        /*HtmlHelp(
           GetDesktopWindow(),
        HLP_FILENAME, //"c:\\Help.chm::/Intro.htm>Mainwin",
        HH_DISPLAY_TOPIC,
        NULL) ;
        */
        break;
    default :
        return(-1);
    }

    return(0);
}

HLP_stack_edit()
{
    int         i, lg = 0;
    extern int  HLP_NB_STTBL;
    extern U_ch **HLP_STTBL;
    U_ch        *buf;
    //HWND        hWnd = WscrGetMainWnd(); /* JMP 25-01-00 */

    if(!SCR_WIN_HELP_ACTIF) return(0);      /* BP_M 20-02-2003 12:23 */

    for(i = HLP_NB_STTBL - 1 ; i >= 0 ; i--)
        if(HLP_STTBL[i]) lg += strlen(HLP_STTBL[i]) + 1;

    if(lg == 0) {
        HLP_edit("");
        return(0);
    }

    buf = SCR_malloc(lg + 1);
    for(i = HLP_NB_STTBL - 1 ; i >= 0 ; i--)
        if(HLP_STTBL[i]) {
            strcat(buf, HLP_STTBL[i]);
            strcat(buf, ";");
        }
    buf[strlen(buf) - 1] = 0;
    HLP_edit(buf);
    //WinHelp(hWnd, HLP_NAME, HELP_PARTIALKEY, (DWORD)buf);
    SCR_free(buf);
    return(0);
}

#endif






#include "scr4w.h"

HWND    hMainWnd = 0;
HANDLE  hInst, hPrevInst, hAccel = 0;
U_ch    *WSCRMainTitle = "Application SCR4/W";       /* JMP 04-02-97 */
U_ch    *WSCRQueryEnd  = "Quitter l'application ?"; /* JMP 04-02-97 */
//int     WSCR_FONTHEIGHT = 12;          // reporté dans s_wobjs.c
//char    *WSCR_FONTNAME = "Terminal";   // Idem

U_ch    **WSCRArgv; /* BP_M 12-12-2009 14:08 */
int     WSCRArgc;   /* BP_M 12-12-2009 14:08 */

FARPROC  W_DLGEDITFN = 0;   /* BP_M 12-12-2009 14:09 */

WNDPROC W_DLGEDITFN_DFT = 0, W_DLGBUTTONFN_DFT = 0, W_DLGCOMBOFN_DFT = 0,
	W_DLGLISTBOXFN_DFT = 0;
int WSCR_AUTOMOVE = 0;
int WSCR_PGF1DEF = 0;
int SCR_tabenter = 0; /* JMP 16-10-99 */
int SCR_RECORD_KEY_WINDOW; /* BP_M 02-11-1999 */
PAGE    *WSCR_PG_NOACTS = 0;
int WSCR_FLD_NOACTS = 0;


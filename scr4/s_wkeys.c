#include "scr4w.h"

extern int     WSCR_DOSKEYS[];
extern int     WSCR_NB_DOSKEYS;

extern int     SCR_last_key ;
/*NH*/
WscrAddDosKey(int key)
{
    SCR_last_key = key;
/*    if(WSCR_NB_DOSKEYS >= WSCR_MAX_DOSKEYS) return(-1);
    WSCR_DOSKEYS[WSCR_NB_DOSKEYS++] = key;
    return(0);
*/
}

/*NH*/
WscrGetDosKey()
{
    int     key = SCR_last_key;

    SCR_last_key = 0;
    return(key);
/*
    int     key;

    if(WSCR_NB_DOSKEYS <= 0) return(0);
    key = WSCR_DOSKEYS[0];
    WSCR_NB_DOSKEYS--;
    memcpy(WSCR_DOSKEYS, WSCR_DOSKEYS + 1,
			 sizeof(WSCR_DOSKEYS[0]) * WSCR_NB_DOSKEYS);
    return(key);
*/
}

extern int SCR_hitkey_inuse;

SCR_hit_key()
{
    MSG     msg;
    int     key = 0;

    if(SCR_last_key) return(SCR_last_key);
    if(SCR_hitkey_inuse) return(0);
    SCR_hitkey_inuse = 1;
//    while(PeekMessage(&msg, 0, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE)) {
    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
	key = 0;
	TranslateMessage(&msg);
	if(msg.message == WM_KEYDOWN || msg.message == WM_SYSKEYDOWN) {
	    key = WscrKeyWinToScr(msg.wParam);
	    if(key == 0) key = msg.wParam;
	    if(key < ' ') key = 0;
	    }
	else if(msg.message == WM_QUIT)
	    key = -1;
	if(key >= 128 && key < 256) key = SCR_AnsiToOemChar(key);
	if(key) WscrAddDosKey(key);
	if(msg.message != WM_KEYDOWN && msg.message != WM_SYSKEYDOWN &&
	   msg.message != WM_KEYUP   && msg.message != WM_SYSKEYUP   &&
	   msg.message != WM_CHAR    && msg.message != WM_DEADCHAR)
		DispatchMessage(&msg);
	}

    if(key == -1) PostQuitMessage(0);
    SCR_hitkey_inuse = 0;
    return(WSCR_NB_DOSKEYS);
}











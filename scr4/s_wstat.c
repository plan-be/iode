#include "scr4w.h"
#include <commctrl.h>

/*NH*/
HWND WscrStatusCreate(HWND hWnd)
{
    HWND hWndStatus;

    hWndStatus = CreateWindowEx(
	    0L,                              // no extended styles
	    STATUScalarASSNAME,                 // status bar
	    "",                              // no text
	    WS_CHILD | WS_BORDER | WS_VISIBLE | WS_GROUP | SBT_POPOUT,  // styles
	    -100, -100, 10, 10,              // x, y, cx, cy
	    hWnd,                            // parent window
	    (HMENU)WSCR_STATUSID,            // window ID
	    hInst,                           // instance
	    NULL);                           // window data

    return(hWndStatus);
}

/* =================================================================
Change le message de la ligne d'état de la fenêtre hWnd. Si hWnd est
nul, affiche dans la dernière fenêtre Popup de l'application en cours.

Si une telle fenêtre n'existe pas, retourne sans afficher.
==================================================================== */

WscrStatusSetText(HWND hWnd, U_ch *text)
{
    HWND    hWndStatus;
    int     rc;

    if(hWnd == 0) hWnd = GetLastActivePopup(hMainWnd);
    if(hWnd == 0) return(-1);
    hWndStatus = GetDlgItem(hWnd, WSCR_STATUSID);
    if(hWndStatus == 0) return(-1);
    if(text == 0) text = "";
    SCR_OemToAnsi(text, text);
    rc = SendMessage(hWndStatus, SB_SETTEXT, 0, (LPARAM)text);
    SCR_AnsiToOem(text, text);
    InvalidateRect(hWndStatus, 0L, TRUE);
    UpdateWindow(hWndStatus);
    return(rc);
}


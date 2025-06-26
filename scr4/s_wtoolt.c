#include "scr4w.h"
#include <commctrl.h>

// DoCreateDialogTooltip - creates a ToolTip control for a dialog box,
//     enumerates the child control windows, and installs a hook
//     procedure to monitor the message stream for mouse messages posted
//     to the control windows.
// Returns TRUE if successful or FALSE otherwise.
//
// Global variables


// g_hhk ¾ handle of the hook procedure

HWND    g_hwndTT, g_hwndDlg;
HHOOK   g_hhk;

// EmumChildProc - registers control windows with a ToolTip control by
//     using the TTM_ADDTOOL message to pass the address of a
//     TOOLINFO structure.
// Returns TRUE if successful or FALSE otherwise.
// hwndCtrl - handle of a control window
// lParam - application-defined value (not used)

BOOL WDDlgSet1Tooltip(HWND hwndCtrl, LPARAM lParam)
{
    TOOLINFO    ti;
//    char        szClass[64];

    // Skip static controls.
//    GetClassName(hwndCtrl, szClass, sizeof(szClass));
//    if (lstrcmp(szClass, "STATIC") {
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND;
	ti.hwnd = g_hwndDlg;
	ti.uId = (UINT) hwndCtrl;
	ti.hinst = 0;
	ti.lpszText = LPSTR_TEXTCALLBACK;
	SendMessage(g_hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);
//    }
    return(TRUE);
}

// GetMsgProc - monitors the message stream for mouse messages intended
//     for a control window in the dialog box.
// Returns a message-dependent value.
// nCode - hook code
// wParam - message flag (not used)
// lParam - address of an MSG structure

LRESULT CALLBACK WDDlgGetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    MSG     *lpmsg;

    lpmsg = (MSG *) lParam;
    if(nCode < 0 || !(IsChild(g_hwndDlg, lpmsg->hwnd)))
	return(CallNextHookEx(g_hhk, nCode, wParam, lParam));

    switch (lpmsg->message) {
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	    if (g_hwndTT != NULL) {
		MSG msg;

		msg.lParam = lpmsg->lParam;
		msg.wParam = lpmsg->wParam;
		msg.message = lpmsg->message;
		msg.hwnd = lpmsg->hwnd;
		SendMessage(g_hwndTT, TTM_RELAYEVENT, 0,
		    (LPARAM) (LPMSG) &msg);
	    }
	    break;
	default:
	    break;
    }
    return(CallNextHookEx(g_hhk, nCode, wParam, lParam));
}

// OnWMNotify - provides the ToolTip control with the appropriate text
//     to display for a control window. This function is called by
//     the dialog box procedure in response to a WM_NOTIFY message.
// lParam - second message parameter of the WM_NOTIFY message

/*VOID OnWMNotify(LPARAM lParam)
{
    LPTOOLTIPTEXT lpttt;
    int idCtrl;

    if ((((LPNMHDR) lParam)->code) == TTN_NEEDTEXT) {
	idCtrl = GetDlgCtrlID((HWND) ((LPNMHDR) lParam)->idFrom);
	lpttt = (LPTOOLTIPTEXT) lParam;

	switch (idCtrl) {
	    case ID_HORZSCROLL:
		lpttt->lpszText = "A horizontal scroll bar.";
		return;

	    case ID_CHECK:
		lpttt->lpszText = "A check box.";
		return;

	    case ID_EDIT:
		lpttt->lpszText = "An edit control.";
		return;
	}
    }
return;
}
*/
WDDlgClearHook()
{
    if(g_hhk == 0) return(0);
    UnhookWindowsHookEx(g_hhk);
    g_hhk = 0;
    return(0);
}

WDDlgCreateTooltip(WDDLG *wddlg)
{
    InitCommonControls();
    g_hwndTT = CreateWindowEx(0, TOOLTIPS_CLASS, (LPSTR) NULL,
	TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	CW_USEDEFAULT, wddlg->hWnd, (HMENU) NULL, hInst, NULL);

    if(g_hwndTT == NULL) return(-1);

    // Enumerate the child windows to register them with the ToolTip
    // control.
    g_hwndDlg = wddlg->hWnd;
    if(!EnumChildWindows(wddlg->hWnd, (WNDENUMPROC) WDDlgSet1Tooltip, 0))
	return(-1);

    // Install a hook procedure to monitor the message stream for mouse
    // messages intended for the controls in the dialog box.
    g_hhk = SetWindowsHookEx(WH_GETMESSAGE, (HOOKPROC)WDDlgGetMsgProc,
	(HINSTANCE) NULL, GetCurrentThreadId());

    if(g_hhk == (HHOOK) NULL) return(-1);
    return(0);
}


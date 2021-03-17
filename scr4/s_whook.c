#include "scr4w.h"

extern DWORD   WSCR_LASTKEY_TIME;

HHOOK WSCR_KEYBOARDHOOKPROC = 0;

LRESULT CALLBACK WscrKeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) /* JMP 01-12-97 */
{
    LPARAM  lp2;

    WSCR_LASTKEY_TIME = GetTickCount(); /* BP_M 25-03-2000 */

    lp2 = lParam & 0x80000000L;  /* JMP 05-03-98 */
    if(nCode >= 0 &&  lp2 == 0 && /* JMP 05-03-98 */
		    WscrProcKeydown((HWND)0, wParam)) return((LRESULT)1);
    return(CallNextHookEx(WSCR_KEYBOARDHOOKPROC, nCode, wParam, lParam));
}


WscrSetKeyboardHook()
{
    WSCR_KEYBOARDHOOKPROC = SetWindowsHookEx(WH_KEYBOARD,
	(HOOKPROC)WscrKeyboardHookProc, (HINSTANCE) NULL, GetCurrentThreadId());
    return(0);
}

WscrClearKeyboardHook()
{
    if(WSCR_KEYBOARDHOOKPROC == 0) return(0);
    UnhookWindowsHookEx(WSCR_KEYBOARDHOOKPROC);
    WSCR_KEYBOARDHOOKPROC = 0;
    return(0);
}

HHOOK WSCR_MOUSEHOOKPROC = 0;

LRESULT CALLBACK WscrMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) /* JMP 01-12-97 */
{
    LPARAM  lp2;


    if(nCode >= 0 && (wParam == WM_LBUTTONUP || wParam == WM_RBUTTONUP ||wParam == WM_MBUTTONUP))
	WSCR_LASTKEY_TIME = GetTickCount(); /* BP_M 25-03-2000 */
    return(CallNextHookEx(WSCR_MOUSEHOOKPROC, nCode, wParam, lParam));

}

WscrSetMouseHook()
{
    WSCR_MOUSEHOOKPROC = SetWindowsHookEx(WH_MOUSE,
	(HOOKPROC)WscrMouseHookProc, (HINSTANCE) NULL, GetCurrentThreadId());
    return(0);
}

WscrClearMouseHook()
{
    if(WSCR_MOUSEHOOKPROC == 0) return(0);
    UnhookWindowsHookEx(WSCR_MOUSEHOOKPROC);
    WSCR_MOUSEHOOKPROC = 0;
    return(0);
}


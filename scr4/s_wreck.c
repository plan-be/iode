#include "scr4w.h"

#if defined(SCRW32) || defined(DOSW32)

WscrIsShift()
{
    if(GetKeyState(VK_SHIFT) >> 15) return(1);
    return(0);
}

WscrIsCtrl()
{
    if(GetKeyState(VK_CONTROL) >> 15) return(1);
    return(0);
}

WscrIsAlt()
{
    if(GetKeyState(VK_MENU) >> 15) return(1);
    return(0);
}

WscrKeyWinToScr(int windef)
{
    int shift = WscrIsShift(), adds = shift * 100;
    int ctrl  = WscrIsCtrl(), addc = ctrl * 200;
    int alt   = WscrIsAlt(),  adda = alt * 300;

//  Debug("S:%d - C:%d - A:%d - Key: %d\n", shift, ctrl, alt, windef);

    if(windef == VK_RETURN) return(shift ? SCR_LINEFEED : SCR_ENTER);
    if(windef == VK_ESCAPE) return(SCR_ESCAPE);
    if(windef == VK_BACK  ) return(ctrl ? SCR_C_BACKSPACE : SCR_BACKSPACE);
    if(windef == VK_INSERT) return(SCR_INSERT);
    if(windef == VK_DELETE) return(SCR_DELETE);
    if(windef == VK_LEFT  ) return(SCR_CSR_LEFT + adds + addc);
    if(windef == VK_RIGHT ) return(SCR_CSR_RIGHT+ adds + addc);
    if(windef == VK_UP    ) return(SCR_CSR_UP   + adds + addc);
    if(windef == VK_DOWN  ) {
	if(alt) return(SCR_CSR_PG_DN);
	else    return(SCR_CSR_DOWN + adds + addc);
	}
    if(windef == VK_PRIOR ) return(SCR_CSR_PG_UP+ adds + addc);
    if(windef == VK_NEXT  ) return(SCR_CSR_PG_DN+ adds + addc);
    if(windef == VK_HOME  ) return(SCR_CSR_HOME + adds + addc);
    if(windef == VK_END   ) return(SCR_CSR_END  + adds + addc);
    if(windef == VK_TAB   ) return(shift ? SCR_S_TAB : SCR_TAB);
    if(windef >= VK_F1 && windef <= VK_F10) {
/*        if(shift) return((SCR_S_F1) + windef - VK_F1);
	if(ctrl)  return((SCR_C_F1) + windef - VK_F1);
*/
	return((SCR_F1) + adda + adds + addc + windef - VK_F1);
	}
    if(windef >= VK_F11 && windef <= VK_F20)
	return((SCR_S_F1) + windef - VK_F11);
    if(alt && windef >= 'a' && windef <= 'z') return(windef - 'a' + 1300);
    if(alt && windef >= 'A' && windef <= 'Z') return(windef - 'A' + 1300);
    if(ctrl && windef >= 'A' && windef <= 'Z') return(windef + 1200 - 'A'); /* BP_M 13-02-2003 18:04 */
    return(0);
}

WscrKeyScrToWin(int scrdef)
{
    int windef;

    if(scrdef == SCR_ENTER    ) return(VK_RETURN);
    if(scrdef == SCR_ESCAPE   ) return(VK_ESCAPE);
    if(scrdef == SCR_INSERT   ) return(VK_INSERT);
    if(scrdef == SCR_DELETE   ) return(VK_DELETE);
    if(scrdef == SCR_CSR_LEFT ) return(VK_LEFT);
    if(scrdef == SCR_CSR_RIGHT) return(VK_RIGHT);
    if(scrdef == SCR_CSR_UP   ) return(VK_UP);
    if(scrdef == SCR_CSR_DOWN ) return(VK_DOWN);
    if(scrdef == SCR_CSR_PG_UP) return(VK_PRIOR);
    if(scrdef == SCR_CSR_PG_DN) return(VK_NEXT);
    if(scrdef == SCR_CSR_HOME ) return(VK_HOME);
    if(scrdef == SCR_CSR_END  ) return(VK_END);
    if(scrdef == SCR_TAB      ) return(VK_TAB);
    if(scrdef >= SCR_F1 && scrdef <= SCR_F10) return(VK_F1 + scrdef - (SCR_F1));
    if(scrdef >= SCR_S_F1 && scrdef <= SCR_S_F10)
				    return(VK_F11 + scrdef - (SCR_S_F1));
    return(0); /* JMP 4.23 11-11-95 */
}

WscrRecord1Key(int key)
{
    BYTE  state, vkey;
    SHORT res;

    key = SCR_OemToAnsiChar(key);
    res = VkKeyScan((TCHAR)key);
    vkey = LOBYTE(res);
    state = HIBYTE(res);

    if(state & 1) keybd_event(VK_SHIFT, 0, 0, 0);
    if(state & 2) keybd_event(VK_CONTROL, 0, 0, 0);
    if(state & 4) keybd_event(VK_MENU, 0, 0, 0);

    keybd_event(vkey, 0, 0, 0);
    keybd_event(vkey, 0, KEYEVENTF_KEYUP, 0);

    if(state & 1) keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    if(state & 2) keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    if(state & 4) keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);

    return(0);
}

WscrRecordKey2(key)
int      key;
{
    int     mksh = 0;
    int     shift  = WscrIsShift();
    int     alt    = WscrIsAlt();
    int     ctrl   = WscrIsCtrl();
    int     flag = 0;

    if(shift) keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
    if(ctrl)  keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    if(alt)   keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);

    if(key >= 0 && key <= 255) return(WscrRecord1Key((unsigned char)key));

    if(key >= SCR_A_A && key <= SCR_A_Z) {
	flag = 2;
	key = key - SCR_A_A + 'A';
    }
    else {
	if(key >= SCR_A_F1 && key <= SCR_A_F10) {
	    flag = 2;
	    key = key - SCR_A_F1 + VK_F1;
	}
	else if(key == SCR_A_TAB) {
	    flag = 2;
	    key = VK_TAB;
	    }
	else {
	    key = WscrKeyScrToWin(key);
	    flag = 1;
	}
    }

    if(flag == 2) /* ALT */
	keybd_event(VK_MENU, 0, 0, 0);

    keybd_event(key, 0, 0, 0);
    keybd_event(key, 0, KEYEVENTF_KEYUP, 0);

    if(flag == 2)  /* ALT */
	keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);

    return(0);
}

#endif

#include "scr4w.h"
#include <commctrl.h>

int WSCR_APF1DEF = 0;

BOOL wmClose(HWND hWnd, int ask)
{
    if(ask == 1 || WSCRQueryEnd == 0) return(FALSE);
    if(MessageBox(hWnd, WSCRQueryEnd, WSCRMainTitle, MB_OKCANCEL | MB_ICONQUESTION) ==
	IDOK) return(FALSE);
    return(TRUE);
}

static VOID wmCommand(HWND hWnd, int iId, HWND hCtl, int nNotify)
{

    WscrActExec(iId - WACT_SHIFT);
    if(SCR_APP_ABORT) PostMessage(hWnd, WM_CLOSE, 1, 0L);
}

static VOID wmCreate(HWND hWnd, LPCREATESTRUCT lPcrst)
{
/*
    hAccel = LoadAccelerators(hInst, MAKEINTRESOURCE(TOPACCEL));
    return;
*/
}

static VOID wmDestroy(HWND hWnd)
{
    char    *ptr = WscrFindObjPtr(hWnd);

    if(ptr) WscrUnRegisterObj(ptr);
    if(hWnd == hMainWnd) PostQuitMessage(0); /* Envoi du message WM_QUIT */
    return;
}

WscrGetMenuComment(HMENU hMenu, UINT uItem, UINT uFlags)
{
    MENU    *mn;
    int     i;

    mn = WscrFindMenu(hMenu);
    if(mn == 0) return(-1);

    if(uFlags & MF_POPUP) mn->mn_choice = uItem;
    else  {
	if(uItem >= 20000) mn->mn_choice = uItem - 20000;
	else {
	    for(i = 0 ; i < mn->mn_nb_opts ; i++)
		if(mn->mn_opts[i].opt_act + WACT_SHIFT == uItem) break;

	    mn->mn_choice = i;
	    }
	}

    if(mn->mn_choice < 0 || mn->mn_choice >= mn->mn_nb_opts)
	mn->mn_choice = 0;

    MN_calc_cmt(mn);

    /* HELP */
    HLP_stack_del(3);
    MN_help_stack(mn);
    MN_help_stack_opt(mn);

    return(0);
}

WscrGetApplComment(HMENU hMenu, UINT uItem, UINT uFlags)
{
    APPL    *ap;
    A_BAR   *ab;
    int     i;

    ap = WscrFindAppl(hMenu);
    if(ap == 0) return(-1);
    ab = (A_BAR *)SCR_PTR(ap->ap_ab);

    if(uFlags & MF_POPUP) ab->ab_choice = uItem;
    else  {
	if(uItem >= 20000) ab->ab_choice = uItem - 20000;
	else {
	    for(i = 0 ; i < ab->ab_nb_opts ; i++)
		if(ab->ab_opts[i].ao_ptr + WACT_SHIFT == uItem) break;

	    ab->ab_choice = i;
	    }
	}

    if(ab->ab_choice < 0 || ab->ab_choice >= ab->ab_nb_opts)
	ab->ab_choice = 0;

    AB_calc_cmt(ab, ab->ab_choice);

    /* HELP */
    HLP_stack_del(3);
    AB_help_stack(ab, ab->ab_choice);
    return(0);
}

WscrGetPgIsamComment(HMENU hMenu, UINT uItem, UINT uFlags)
{
    if(WscrFindApMn(hMenu, ACT_AB_EDIT_ISAM) < 0) return(-1);
    if(uFlags & MF_POPUP) uItem = WIS_SHIFT;
    switch(uItem) {
	case WIS_SHIFT    : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_FIND")); break;
	case WIS_SHIFT+1  : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_NEXT")); break;
	case WIS_SHIFT+2  : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_PREV")); break;
	case WMP_SHIFT2   :
	case WIS_SHIFT+3  : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_ADD")); break;
	case WIS_SHIFT+4  : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_SAVE")); break;
	case WMP_SHIFT2+1 :
	case WIS_SHIFT+5  : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_DEL")); break;
	case WMP_SHIFT1+2 :
	case WMP_SHIFT2+4 :
	case WIS_SHIFT+6  : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_CLOSE")); break;
	case WMP_SHIFT2+2 : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_MODIFY")); break;
	case WMP_SHIFT2+3 : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_STOP")); break;
	case WMP_SHIFT1+1 : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_START")); break;
	case WMP_SHIFT1   : strcpy(SCR_CMT_TXT, SCR_err_txt_t("CM_INDEX")); break;
	default           : break;
	}

    return(0);
}

WscrGetSysMenuComment(HMENU hMenu, UINT uItem, UINT uFlags)
{
    if(uFlags & MF_SYSMENU) {
	switch(uItem) {
	    case SC_CLOSE    : strcpy(SCR_CMT_TXT, SCR_err_txt_t("SC_CLOSE")); break;
	    case SC_MAXIMIZE : strcpy(SCR_CMT_TXT, SCR_err_txt_t("SC_MAXIMIZE")); break;
	    case SC_MINIMIZE : strcpy(SCR_CMT_TXT, SCR_err_txt_t("SC_MINIMIZE")); break;
	    case SC_MOVE     : strcpy(SCR_CMT_TXT, SCR_err_txt_t("SC_MOVE")); break;
	    case SC_RESTORE  : strcpy(SCR_CMT_TXT, SCR_err_txt_t("SC_RESTORE")); break;
	    case SC_SIZE     : strcpy(SCR_CMT_TXT, SCR_err_txt_t("SC_SIZE")); break;
	    case SC_TASKLIST : strcpy(SCR_CMT_TXT, SCR_err_txt_t("SC_TASKLIST")); break;
	    default          : strcpy(SCR_CMT_TXT, SCR_err_txt_t("SC_MENU")); break;
	    }

	return(0);
	}
    return(-1);
}

WscrMenuSelect(HWND hWnd, HMENU hMenu, UINT uItem, UINT uFlags)
{
    char    buf[80];

    if(uItem == 0 && uFlags == 0xFFFF) return(0);
    if(WscrGetMenuComment(hMenu, uItem, uFlags) == 0 ||
       WscrGetApplComment(hMenu, uItem, uFlags) == 0 ||
       WscrGetPgIsamComment(hMenu, uItem, uFlags) == 0 ||
       WscrGetSysMenuComment(hMenu, uItem, uFlags) == 0)
	    WscrDisplayCmt(hWnd);

    return(0);
}

WscrMenuChar(HWND hWnd, HMENU hMenu, UINT ch, UINT uFlags)
{
//    PG_display_error("char %d pressé", ch);
    return(0);
}

WscrProcKeydown(HWND hWnd, int ch)
{
    HWND    hWndChild = GetFocus();           /* JMP 05-03-98 */

    SCR_LKEY = WscrKeyWinToScr(ch);
    //Debug("key : %d\n", SCR_LKEY);      /* BP_M 13-02-2003 15:46 */
    if(APP_key(SCR_LKEY)) {
	if(hWndChild) {
	    WscrSendRecord(hWndChild);   /* JMP 05-03-98 */
	    //if(SCR_APP_ABORT) PostMessage(hWndChild, WM_CLOSE, 1, 0L);
	    if(SCR_LKEY == SCR_F1) WSCR_APF1DEF = 1;
	    }
	return(1);
	}
    return(0);
}

static VOID wmPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC         hDC;

    hDC = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
    return;
}


WscrResizeCtrl(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HWND    hWndStatus, hWndToolbar, hWndHeader;


    hWndStatus = GetDlgItem(hWnd, WSCR_STATUSID);
    if(hWndStatus)
	MoveWindow(hWndStatus, 0, HIWORD(lParam) - 10, LOWORD(lParam),
				      HIWORD(lParam), TRUE);

    hWndToolbar = GetDlgItem(hWnd, WSCR_TOOLBARID);
    if(hWndToolbar) SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0L);

    hWndHeader = GetDlgItem(hWnd, WSCR_HEADERID);
    if(hWndHeader) WscrHeaderSetPos(hWndHeader, 0, 0, 0, 0);

    return(0);
}

static BOOL wmQueryEndSession(HWND hWnd)
{
    return(FALSE);
}

wmHelp(HWND hWnd)
{
    APPL    *ap;
    A_BAR   *ab;
    int     choice;

/*
    ap = WscrFindObjPtr(hWnd);
    if(ap) {
	ab = (A_BAR *) SCR_PTR(ap->ap_ab);
	choice = ab->ab_choice;
	if(choice < 0 || choice >= ab->ab_nb_opts)
	    ab->ab_choice = choice = 0;
	AB_help_stack(ab, choice);
	}
*/
    if(WscrIsShift() || WscrIsCtrl()) return(0); /* JMP 23-08-97 */
    if(WSCR_APF1DEF) {
	WSCR_APF1DEF = 0;
	return(0);
	}
    HLP_stack_edit();
//    if(ap) HLP_stack_del(3);
    return(0);
}

long FAR PASCAL WscrMainProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam) /* JMP 12-01-97 */
{
    BOOL    bTraite = TRUE;
    HWND    hWndStatus;
    long    lRes    = 0L;
/*    WORD    wLo = LOWORD(lParam),
	    wHi = HIWORD(lParam);
WIN16 */
    WORD    wNotify  = HIWORD(wParam),
	    wId      = LOWORD(wParam);
    HWND    hWndCtrl = (HWND)lParam;

    switch(wMsg) {
      case WM_HELP     : wmHelp(hWnd); break;
      case WM_CLOSE    : bTraite = wmClose(hWnd, wParam); break;
      case WM_CREATE   : wmCreate(hWnd, (LPCREATESTRUCT)lParam); break;
      case WM_COMMAND  : wmCommand(hWnd, wId, hWndCtrl, wNotify); break;
      case WM_DESTROY  : wmDestroy(hWnd); break;
      case WM_MENUSELECT: WscrMenuSelect(hWnd, (HMENU) lParam, wId, wNotify); break;
      case WM_KEYDOWN :
      case WM_SYSKEYDOWN : bTraite = WscrProcKeydown(hWnd, wParam); break;
      case WM_MENUCHAR : WscrMenuChar(hWnd, (HMENU) lParam, wId, wNotify); break;
      case WM_PAINT    : wmPaint(hWnd); break;
      case WM_SIZE     :
	    WscrResizeCtrl(hWnd, wParam, lParam);
	    bTraite = FALSE;
	    break;
      case WM_QUERYENDSESSION:
	    bTraite = wmQueryEndSession(hWnd); break;
      case WM_NOTIFY :
	    return(WscrToolbarNotify(hWnd, wParam, lParam));
      default : bTraite = FALSE; break;
      }
    if(!bTraite) lRes = DefWindowProc(hWnd, wMsg, wParam, lParam);
    return(lRes);
}










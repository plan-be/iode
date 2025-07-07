#include "scr4w.h"
#include <commctrl.h>
/*
int CALLBACK WscrPropsCallback(HWND hwndDlg, UINT uMsg, LPARAM lParam)
{
    switch(uMsg) {
	case PSCB_INITIALIZED : return(0);
	default : return(0);
	}
}
*/

BOOL CALLBACK WscrPropsDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    PROPSHEETPAGE   *psp;
    HWND            hWndSheet = GetParent(hWnd);
    NMHDR           *nmh;
    PAGE            *pg;
    TPAGES          *tpgs;
    int             rc, pgnu;
    WORD            wNotify  = HIWORD(wParam),
		    wId      = LOWORD(wParam);
    extern int      WSCR_PGF1DEF;

    switch(wMsg) {
	case WM_HELP:
	    if(WscrIsShift() || WscrIsCtrl()) return(TRUE); /* JMP 23-08-97 */
	    if(WSCR_PGF1DEF) WSCR_PGF1DEF = 0; /* JMP 23-08-97 */
	    else             HLP_stack_edit(); /* JMP 23-08-97 */
	    return(TRUE);

	case WM_NOTIFY  :
	    pg   = WscrFindPage(hWnd);
	    tpgs = WscrFindObjPtr(hWnd);
	    if(pg == 0 || tpgs == 0) break;
	    nmh = (NMHDR *)lParam;

	    switch(nmh->code) {
		case PSN_WIZFINISH :
		case PSN_APPLY :
		    if(tpgs->tp_applied) break;
		    rc = ACT_exec_nu(tpgs->tp_end_act);
		    if(rc) {
		//      PG_display_error("APPLY, rc=%d", rc);
			SetWindowLongPtr(hWnd, DWLP_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
			return(TRUE);
			}
		    SetWindowLongPtr(hWnd, DWLP_MSGRESULT, TRUE);
		    tpgs->tp_applied = 1;
		    break;

		case PSN_KILLACTIVE :
		    rc = ACT_exec_nu(pg->pg_end_act);
		    if(rc) {
		 //     PG_display_error("KILLACTIVE, rc=%d", rc);
			SetWindowLongPtr(hWnd, DWLP_MSGRESULT, PSNRET_INVALID_NOCHANGEPAGE);
			return(TRUE);
			}
		    break;

		case PSN_SETACTIVE :
		    pgnu = SCR_NU(pg);
		    if(pgnu == tpgs->tp_pgsnbs[tpgs->tp_nb - 1])
			PropSheet_SetWizButtons(hWndSheet,
					    PSWIZB_BACK | PSWIZB_FINISH);
		    else if(pgnu == tpgs->tp_pgsnbs[0])
			PropSheet_SetWizButtons(hWndSheet,
					    PSWIZB_NEXT);
		    else
			PropSheet_SetWizButtons(hWndSheet,
					    PSWIZB_BACK | PSWIZB_NEXT);
		    break;

		case PSN_QUERYCANCEL :
		 //   PG_display_error("QUERYCANCEL");
		    tpgs->tp_applied = -1;
		    break;
		case PSN_HELP:
		    HLP_stack_edit();
		    break;
		case PSN_WIZBACK   :
		 //   PG_display_error("WIZBACK");
		    break;
		case PSN_WIZNEXT   :
		 //   PG_display_error("WIZNEXT");
		    break;

		default :
		    break;
		}
	    return(FALSE);

	case WM_COMMAND :
#ifdef WIN32
	    WscrDlgCommand(hWnd, LOWORD(wParam), (HWND)(lParam), HIWORD(wParam));
#else
	    WscrDlgCommand(hWnd, wParam, (HWND)LOWORD(lParam), HIWORD(lParam));
#endif
	    return(TRUE);
	case WM_SYSCOMMAND :
	    return(WscrDlgSysCommand(hWnd, wParam, LOWORD(lParam), HIWORD(lParam)));

	case WM_INITDIALOG :
	    psp = (PROPSHEETPAGE *)lParam;
	    WscrDlgInit(hWnd, (PAGE *)psp->lParam, 1);
	    return(TRUE);

	default :
	    return(FALSE);
	}

    return(FALSE);
}


WscrEditTabbedPages(HWND hwndOwner, TPAGES *tpgs)
{
    PROPSHEETPAGE   *psp;
    PROPSHEETHEADER psh;
    int             i, rc;
    PAGE            *pg;

    InitCommonControls();
    if(hwndOwner == 0) hwndOwner = GetLastActivePopup(hMainWnd);

    if(ACT_exec_nu(tpgs->tp_begin_act)) return(-1);

    psp = (PROPSHEETPAGE *)SCR_malloc(sizeof(PROPSHEETPAGE) * tpgs->tp_nb);

    for(i = 0 ; i < tpgs->tp_nb; i++) {
	pg = (PAGE *)SCR_PTR(tpgs->tp_pgsnbs[i]);
	psp[i].dwSize    = sizeof(PROPSHEETPAGE);
	psp[i].dwFlags   = PSP_USETITLE | PSP_HASHELP;
	psp[i].hInstance = hInst;
#if defined(__cplusplus) || defined(VC) || defined(CodeGear)
	psp[i].pszTemplate = MAKEINTRESOURCE(tpgs->tp_pgsnbs[i]);
	psp[i].pszIcon = NULL;
#else
	psp[i].u.pszTemplate = MAKEINTRESOURCE(tpgs->tp_pgsnbs[i]);
	psp[i].u2.pszIcon = NULL;
#endif
	psp[i].pfnDlgProc = WscrPropsDlgProc;
	SCR_OemToAnsi(tpgs->tp_pgstitles[i], tpgs->tp_pgstitles[i]);
	psp[i].pszTitle = tpgs->tp_pgstitles[i];
	psp[i].lParam = (LPARAM) pg;
	psp[i].pfnCallback = NULL;

	WscrRegisterObj((char *)tpgs, pg, 1, 0, 0);
	}

    psh.dwSize = sizeof(PROPSHEETHEADER);
    psh.dwFlags = PSH_PROPSHEETPAGE | PSH_HASHELP | PSH_NOAPPLYNOW; /* | PSH_MULTILINETABS */ /* JMP 27-08-97 */
    if(tpgs->tp_type == SCR_WIZARD) psh.dwFlags |= PSH_WIZARD ;
    psh.hwndParent = hwndOwner;
    psh.hInstance = hInst;
    SCR_OemToAnsi(tpgs->tp_title, tpgs->tp_title);
    psh.pszCaption = (LPSTR) tpgs->tp_title;
    psh.nPages = tpgs->tp_nb;
#if defined(__cplusplus) || defined(VC) || defined(CodeGear)
    psh.pszIcon = NULL;
    psh.nStartPage = 0;
    psh.ppsp = psp;
#else
    psh.u.pszIcon = NULL;
    psh.u2.nStartPage = 0;
    psh.u3.ppsp = psp;
#endif

    psh.pfnCallback = 0; // WscrPropsCallback;

    tpgs->tp_applied = 0;
    PropertySheet(&psh);
    rc = tpgs->tp_applied;
    tpgs->tp_applied = 0;

    for(i = 0 ; i < tpgs->tp_nb; i++) {
	pg = (PAGE *)SCR_PTR(tpgs->tp_pgsnbs[i]);
	SCR_AnsiToOem(tpgs->tp_pgstitles[i], tpgs->tp_pgstitles[i]);
	WscrUnRegisterObjPg((char *)tpgs, pg);
	}

    SCR_AnsiToOem(tpgs->tp_title, tpgs->tp_title);
    SCR_free(psp);
    if(rc == 1) {
	SCR_LKEY = tpgs->tp_last_key = SCR_F10;
	return(0);
	}
    SCR_LKEY = tpgs->tp_last_key = SCR_ESCAPE;
    return(-2);
}







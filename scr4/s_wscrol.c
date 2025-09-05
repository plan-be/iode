#include "scr4w.h"

#define X0 (rect.left)
#define X1 (rect.right)
#define Y0 (rect.top)
#define Y1 (rect.bottom)
#define H  (wscrl->height)
#define W  (wscrl->width)

LONG FAR PASCAL WscrScrollEditFn(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
LONG FAR PASCAL WscrScrollProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
//WNDPROC WscrScrollEditFn(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
//WNDPROC WscrScrollProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam);
HWND WscrScrollCreate(HWND hParent, int id, int x, int y, int w, int h);

//FARPROC W_SCRL_EDITFN = 0, W_SCRL_DFT_EDITFN = 0;
WNDPROC W_SCRL_EDITFN = 0, W_SCRL_DFT_EDITFN = 0;

WSCROLL FAR W_SCRL_DUMMY = {
    0,                    /*  char     *data;
    0, 0, 0, 0,           /*  int      cline, ccol, line, col;           */
    0, -1, -1, -1, -1,    /*  int      seltype, sellinefrom, selcolfrom, sellineto, selcolto; */
    0,                    /*  int      (*getnl)();                      */
    0,                    /*  int      (*getnc)();                      */
    0,                    /*  int      (*getwidth)();                   */
    0,                    /*  int      (*getwidthp)();                   */
    0,                    /*  int      (*setwidthp)();                   */
    0,                    /*  int      (*getheight)();                  */
    0,                    /*  int      (*getheightp)();                  */
    0,                    /*  int      (*setheightp)();                  */
    0,                    /*  int      (*getalign)();                   */
    0,                    /*  U_ch     *(*gettext)();                   */
    0,                    /*  U_ch     *(*getedtext)();                 */
    0,                    /*  int      (*settext)();                    */
    0,                    /*  int      (*getcolor)();                   */
    0,                    /*  int      (*getfont)();                    */
    0,                    /*  int      (*fnkey)();                      */
    0,                    /*  int      (*fnbeg)();                      */
    0,                    /*  int      (*fnend)();                      */
    0,                    /*  int      (*fncmd)();                      */
    0,                    /*  int      (*fnrmouse)();                      */
    0,                    /*  HWND     hMenu;                           */
    0,                    /*  int      *colws;                          */

		       /* Private */
    0, 0,                 /*  int      mousedown, editon;               */
    0, 0,                 /*  int      nlv, ncv;                        */
    -1, -1, -1, -1,       /*  int      oline, ocol, ocline, occol;      */
    0, 0,                 /*  int      height, width;                   */
    0, 0, 0,              /*  HWND     hWnd, hEditWnd, hHeaderWnd;      */
    0,                    /*  int      enabled */
    0                     /*  int      end */
    };


ATOM WscrScrollRegister()
{
    WNDCLASS    ScrollWndClass;
    static ATOM ScrollClass = 0;

    if(ScrollClass) return(ScrollClass);

    ScrollWndClass.lpszClassName = "WscrScroll";
    ScrollWndClass.hInstance = hInst;
    ScrollWndClass.lpfnWndProc = WscrScrollProc;
    ScrollWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    ScrollWndClass.lpszMenuName = NULL;
    ScrollWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    ScrollWndClass.hIcon = NULL;
    ScrollWndClass.hbrBackground = GetStockObject(WHITE_BRUSH);
    ScrollWndClass.cbClsExtra = 0;
    ScrollWndClass.cbWndExtra = 16;       /* JMP 09-10-00 */

    ScrollClass = RegisterClass(&ScrollWndClass);
    return(ScrollClass);
}


HWND WscrScrollCreate(HWND hParent, int id, int x, int y, int w, int h)
{
    HWND    hWnd = 0, hEditWnd, hButtonWnd;

    if(WscrScrollRegister() == 0) return(hWnd);
    hWnd = CreateWindow("WscrScroll", NULL,
			WS_OVERLAPPEDWINDOW,
	    CW_USEDEFAULT, CW_USEDEFAULT,
	    CW_USEDEFAULT, CW_USEDEFAULT,
/*          x, y, h, w, */
	    hParent, (HMENU)id, hInst, 0L);

    SetWindowLongPtr(hWnd, 12, (LONG)0);
    return(hWnd);
}

WscrScrollCreateHeader(HWND hWndParent)
{
    WSCROLL *wscrl = (WSCROLL *)GetWindowLongPtr(hWndParent, 0);
    HWND    hWnd = (HWND) GetWindowLongPtr(hWndParent, 8);

    if(hWnd) return(-1);
    if(WscrScrollGetHeight(wscrl, -1) == 0) return(0);
    hWnd = WscrCreateHeader(hWndParent, 0, 0, 0, 0);
    SetWindowLongPtr(hWndParent, 8, (LONG)hWnd);
    return(0);
}

WscrScrollCreateEdit(HWND hWndParent)
{
    HWND    hWnd;

    hWnd = CreateWindow("edit", NULL,
	    WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP,
	    0, 0, 200, 20, hWndParent, (HMENU)101, hInst, NULL);
    SendMessage(hWnd, WM_SETFONT, (WPARAM)W_SCRL_FONT, TRUE);
    SetWindowLongPtr(hWndParent, 4, (LONG)hWnd);
    if(W_SCRL_EDITFN == 0) {
	W_SCRL_DFT_EDITFN = (FARPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
	W_SCRL_EDITFN     = MakeProcInstance((FARPROC) WscrScrollEditFn, hInst);
	}
    SetWindowLongPtr(hWnd, GWLP_WNDPROC, (long)W_SCRL_EDITFN);
    return(0);
}

LONG FAR PASCAL WscrScrollProc(HWND hWnd, UINT wMsg,
			       WPARAM wParam, LPARAM lParam)
{
    WSCROLL         *wscrl = 0;
    int             rc = 0;

    switch(wMsg) {
	case WM_CLOSE  :
	    wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
	    if(wscrl) wscrl->end = 1;
	    goto dft;

	case WM_ENABLE :
	    wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
	    if(wscrl) wscrl->enabled = wParam;
	    goto dft;

	case WM_GETDLGCODE :
	    return(DLGC_WANTALLKEYS);

	case WM_CREATE :
	    WscrScrollCreateEdit(hWnd);
	 // WscrScrollCreateHeader(hWnd);
	    wscrl = 0;
	    goto setwscrl;

	case WM_WSCRL_SET :
	    wscrl = (WSCROLL *)lParam;
	    if(wscrl && wscrl->fnbeg)
		wscrl->fnbeg(wscrl, &(wscrl->cline), &(wscrl->ccol));
	  setwscrl:
	    if(wscrl == 0) wscrl = &W_SCRL_DUMMY;
	    wscrl->sellineto = -1;
	    wscrl->enabled = 1;
	    wscrl->end = 0;
	    wscrl->hWnd = hWnd;
	    SetWindowLongPtr(hWnd, 0, (LONG)wscrl);
//            SetScrollRange(hWnd, SB_VERT, 0, WscrScrollGetNl(wscrl) + 1, TRUE);
//            SetScrollRange(hWnd, SB_HORZ, 0, WscrScrollGetNc(wscrl) + 1, TRUE);
	    SetScrollRange(hWnd, SB_VERT, 0, 10, TRUE);
	    SetScrollRange(hWnd, SB_HORZ, 0, 10, TRUE);
	    WscrScrollCreateHeader(hWnd);
	    InvalidateRect(hWnd, NULL, TRUE);
	    return(0L);

	case WM_WSCRL_GET :
	    return(GetWindowLongPtr(hWnd, 0));

	case WM_COMMAND :
	    WscrScrollCommand(hWnd, wParam);
	    return(0L);

	case WM_RBUTTONDOWN :
	    WscrScrollRMouse(hWnd, LOWORD(lParam), HIWORD(lParam));
	    return(0L);

	case WM_LBUTTONUP :
	case WM_LBUTTONDOWN :
	case WM_LBUTTONDBLCLK :
	    if(GetWindowLongPtr(hWnd, 12)) return(0L);
	    SetWindowLongPtr(hWnd, 12, (LONG)1);
	    SetFocus(hWnd);
	    /* NO BREAK */
	case WM_MOUSEMOVE :
	    WscrScrollMouse(hWnd, wMsg, wParam, lParam);
	    SetWindowLongPtr(hWnd, 12, (LONG)0);
	    return(0L);

	case WM_USER + 512 :
	case WM_KEYDOWN :
	case WM_SYSKEYDOWN :    /* JMP 25-05-97 */
	/*case WM_CHAR :          /* JMP 25-05-97 */
	/*case WM_SYSCHAR :       /* JMP 25-05-97 */
	case WM_VSCROLL :
	case WM_HSCROLL :
	    if(GetWindowLongPtr(hWnd, 12)) return(0L);
	    SetWindowLongPtr(hWnd, 12, (LONG)1);
	    rc = WscrScrollScroll(hWnd, wMsg, wParam, lParam);
	    SetWindowLongPtr(hWnd, 12, (LONG)0);
	    if(rc == 0) return(0L);
	    break;

	case WM_SYSKEYUP : /* JMP 09-10-00 */
	    if(wParam == VK_F10) return(0L);
	    break;

	case WM_DESTROY :
	    wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
	    if(wscrl && wscrl->fnend)
		if(wscrl->fnend(wscrl, wscrl->cline, wscrl->ccol)) return(0L);
	    break;

	case WM_PAINT :
	    WscrScrollDisplayEdit(hWnd);
	    WscrScrollPaint(hWnd);
	    WscrScrollPaintHeader(hWnd);
	    WscrScrollInvalidateEdit(hWnd);
	    return(0L);

	case WM_KILLFOCUS :
	case WM_SETFOCUS :
	    wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
	    if(wscrl) {
		wscrl->mousedown = 0; /* JMP 25-04-97 */
		WscrlInvalidate(wscrl);
		WscrScrollRedisplay(hWnd, wscrl);
		}
	    goto dft;

/*
	case WM_CTLCOLOR :
	    SetBkColor((HDC) wParam, WCOL_CYAN);
	    SetTextColor((HDC) wParam, WCOL_BLACK);
	    return((LRESULT)W_BRUSH_CYAN);
	    break;
*/
	default : break;
	}
dft:
    return(DefWindowProc(hWnd, wMsg, wParam, lParam));
}

WscrScrollRMouse(HWND hWnd, WORD x, WORD y)
{
    WSCROLL     *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
    POINT       pt;

    if(wscrl == 0 || wscrl->fnrmouse == 0) return(0);
    pt.x = x;
    pt.y = y;
    ClientToScreen(hWnd, &pt);
    (*wscrl->fnrmouse)(wscrl, pt.x, pt.y);
    return(0);
}

WscrScrollSetEditText(HWND hWnd)
{
    WSCROLL     *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
    HWND        hWndEdit = (HWND) GetWindowLongPtr(hWnd, 4);
    U_ch        *text = 0, buf[80];

    wscrl->editon = 1;
    if(wscrl->getedtext)
	   text = wscrl->getedtext(wscrl, wscrl->cline, wscrl->ccol);
    if(wscrl->gettext && text == 0)
	   text = wscrl->gettext(wscrl, wscrl->cline, wscrl->ccol);
    if(text == 0) {
	   sprintf(buf, "[%d;%d]", wscrl->cline, wscrl->ccol);
	   text = buf;
	   }
    WscrSetWindowTextOem(hWndEdit, text);
    return(0);
}

WscrScrollKeyboard(HWND hWnd, WPARAM wParam)
{
    WSCROLL     *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
    HWND        hWndNext, hWndParent;
    int         rc = 0, key;

    if(WscrProcKeydown(hWnd, wParam)) return(1);
    key = WscrKeyWinToScr(wParam);
    if(key && wscrl->fnkey) {
	rc = wscrl->fnkey(wscrl, wscrl->cline, wscrl->ccol, key);
	WscrSendRecord(hWnd);
	}
    if(rc != 0) return(1);

    switch(wParam) {
	case 0 : /* RETURN or ESCAPE in EDIT */
	    WscrlInvalidate(wscrl);
	    break;

	case VK_ESCAPE :
	    PostMessage(hWnd, WM_CLOSE, 1, 0L);
	    break;

	case VK_UP   :
	    if(wscrl->line == wscrl->cline) wscrl->line--;
	    wscrl->cline--;
	    break;
	case VK_DOWN :
	    if(wscrl->line + wscrl->nlv <= wscrl->cline)
		wscrl->line++;
	    wscrl->cline++;
	    break;
	case VK_NEXT :
	    wscrl->cline += wscrl->nlv;
	    wscrl->line += wscrl->nlv;
	    break;
	case VK_PRIOR:
	    wscrl->cline -= wscrl->nlv;
	    wscrl->line -= wscrl->nlv;
	    break;
	case VK_HOME :
	    if(WscrIsCtrl()) wscrl->cline = wscrl->line = 0;
	    else             wscrl->ccol = wscrl->col= 0;
	    break;
	case VK_END  :
	    if(WscrIsCtrl()) wscrl->cline = wscrl->line = WscrScrollGetNl(wscrl) - 1;
	    else             wscrl->ccol = wscrl->col = WscrScrollGetNc(wscrl) - 1;
	    break;

	case VK_TAB :
	    hWndParent = GetParent(hWnd);
	    if(hWndParent && IsChild(hWndParent, hWnd)) {
		hWndNext = GetNextDlgTabItem(GetParent(hWnd), hWnd, WscrIsShift());
		if(hWndNext) {
		    SetFocus(hWndNext);
		    return(1);
		    }
		}
	    else {
		if(WscrIsShift()) {
		    wscrl->ccol -= wscrl->ncv;
		    wscrl->col  -= wscrl->ncv;
		    }
		else {
		    wscrl->ccol += wscrl->ncv;
		    wscrl->col  += wscrl->ncv;
		    }
		}
	    break;

	case VK_LEFT :
	    if(WscrIsCtrl()) {
		wscrl->ccol -= wscrl->ncv;
		wscrl->col  -= wscrl->ncv;
		}
	    else {
		if(wscrl->col == wscrl->ccol) wscrl->col--;
		wscrl->ccol--;
		}
	    break;

	case VK_RIGHT:
	    if(WscrIsCtrl()) {
		wscrl->ccol += wscrl->ncv;
		wscrl->col  += wscrl->ncv;
		}
	    else {
		if(wscrl->col + wscrl->ncv <= wscrl->ccol) wscrl->col++;
		wscrl->ccol++;
		}
	    break;
	case VK_RETURN:
	    if(wscrl->settext) WscrScrollSetEditText(hWnd);
	    WscrlInvalidate(wscrl);
	    break;
	case VK_SHIFT:
	case VK_CONTROL:
	case VK_MENU:
	    return(2);
	default :
   //       SendMessage(GetParent(hWnd), WM_WSCRL_KEYDOWN, wParam, lParam);
	    WscrlInvalidate(wscrl);
	    break;
	}

    WscrScrollCalcPos(wscrl);
    if(WscrIsShift() && !(wscrl->seltype & WSL_ONESEL)) {
	wscrl->sellineto = wscrl->cline;
	wscrl->selcolto  = wscrl->ccol;
	}
    else {
	wscrl->sellinefrom = wscrl->sellineto = wscrl->cline;
	wscrl->selcolfrom  = wscrl->selcolto  = wscrl->ccol;
	}

    return(0);
}

WscrScrollScroll(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    WSCROLL     *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
    HWND        hWndNext, hWndParent;
    int         rc = 0;

    WscrSendRecord(hWnd);
    switch(wMsg) {
	case WM_KEYDOWN :
	case WM_SYSKEYDOWN :  /* JMP 25-05-97 */
	/*case WM_CHAR : /* JMP 26-05-97 */
	/*case WM_SYSCHAR :     /* JMP 25-05-97 */
	ag:
	    rc = WscrScrollKeyboard(hWnd, wParam);
	    if(rc == 1) return(0);
	    if(rc == 2) return(1);
	    break;

	case WM_VSCROLL :
	    switch(LOWORD(wParam)) {
/*                case SB_LINEUP   : wscrl->line--; break;
		case SB_LINEDOWN : wscrl->line++; break;
		case SB_PAGEDOWN : wscrl->line += wscrl->nlv; break;
		case SB_PAGEUP   : wscrl->line -= wscrl->nlv; break;
		case SB_TOP      : wscrl->line =  0; break;
		case SB_BOTTOM   : wscrl->line =  WscrScrollGetNl(wscrl) - 1; break;
		case SB_THUMBTRACK : wscrl->line = HIWORD(wParam); break;
		case SB_THUMBPOSITION : wscrl->line = HIWORD(wParam); break;
*/
		case SB_LINEUP   : wParam = VK_UP; goto ag;
		case SB_LINEDOWN : wParam = VK_DOWN; goto ag;
		case SB_PAGEDOWN : wParam = VK_NEXT; goto ag;
		case SB_PAGEUP   : wParam = VK_PRIOR; goto ag;
		case SB_TOP      : wParam = VK_HOME; goto ag;
		case SB_BOTTOM   : wParam = VK_END; goto ag;
	    //  case SB_THUMBTRACK : wscrl->line = HIWORD(wParam); break;
	    //  case SB_THUMBPOSITION : wscrl->line = HIWORD(wParam); break;
		}
	    goto vert;

	case WM_HSCROLL :
	    switch(LOWORD(wParam)) {
		case SB_LINEUP   : wscrl->col--; break;
		case SB_LINEDOWN : wscrl->col++; break;
		case SB_PAGEDOWN : wscrl->col += wscrl->ncv; break;
		case SB_PAGEUP   : wscrl->col -= wscrl->ncv; break;
		case SB_TOP      : wscrl->col = 0; break;
		case SB_BOTTOM   : wscrl->col = WscrScrollGetNc(wscrl) - 1; break;
		case SB_THUMBTRACK : wscrl->col = HIWORD(wParam); break;
		case SB_THUMBPOSITION : wscrl->col = HIWORD(wParam); break;
		}
	    goto horiz;
	}

vert :
horiz :
    WscrScrollRedisplay(hWnd, wscrl);
    return(0);
}

WscrlInvalidate(WSCROLL *wscrl)
{
    wscrl->ocline = -1;
    return(0);
}

WscrScrollCalcPos(WSCROLL *wscrl)
{
    int     nl = WscrScrollGetNl(wscrl);
    int     nc = WscrScrollGetNc(wscrl);

    wscrl->cline = _max_(0, wscrl->cline);
    wscrl->cline = _min_(wscrl->cline, nl - 1);
    wscrl->line = _min_(wscrl->line, nl - wscrl->nlv);
    wscrl->line = _max_(0, wscrl->line);
    if(wscrl->line + wscrl->nlv <= wscrl->cline)
	wscrl->line = 1 + wscrl->cline - wscrl->nlv;
    if(wscrl->cline < wscrl->line) wscrl->line = wscrl->cline;


    wscrl->ccol = _max_(0, wscrl->ccol);
    wscrl->ccol = _min_(wscrl->ccol, nc - 1);
    wscrl->col = _min_(wscrl->col, nc - wscrl->ncv);
    wscrl->col = _max_(0, wscrl->col);
    if(wscrl->col + wscrl->ncv <= wscrl->ccol)
	wscrl->col = 1 + wscrl->ccol - wscrl->ncv;
    if(wscrl->ccol < wscrl->col) wscrl->col = wscrl->ccol;

    if(wscrl->seltype & WSL_ONESEL) {
	wscrl->sellinefrom = wscrl->sellineto = wscrl->cline;
	wscrl->selcolfrom = wscrl->selcolto = wscrl->ccol;
	}
    return(0);
}

WscrScrollDisplayEdit(HWND hWnd)
{
    WSCROLL *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
    HWND    hWndEdit = (HWND) GetWindowLongPtr(hWnd, 4);
    int     x, y, w, h;
    RECT    rect;

    WscrScrollCalcPos(wscrl);

    if(wscrl->editon == 0) {
	EnableWindow(hWndEdit, FALSE);
	ShowWindow(hWndEdit, SW_HIDE);
	}
    else {
	WscrScrollCCoord(wscrl, wscrl->cline, wscrl->ccol, &x, &y, &w, &h);
	GetClientRect(hWnd, &rect);
	if(x >= 0 && x < rect.right && x + w > rect.right)
	    w = rect.right - x;
	if(w < 2) w = 2;

	MoveWindow(hWndEdit, x + 1, y + 1, w - 1, h - 1, FALSE);
	EnableWindow(hWndEdit, TRUE);
	ShowWindow(hWndEdit, SW_SHOW);
	SetFocus(hWndEdit);
	}

    return(0);
}

WscrScrollRedisplay(HWND hWnd, WSCROLL *wscrl)
{
    WscrScrollDisplayEdit(hWnd);

    if(wscrl->line == wscrl->oline && wscrl->cline == wscrl->ocline &&
       wscrl->col == wscrl->ocol && wscrl->ccol == wscrl->occol)
	    return(0);

//    SetScrollPos(hWnd, SB_VERT, wscrl->line, TRUE);
//    SetScrollPos(hWnd, SB_HORZ, wscrl->col, TRUE);
    SetScrollPos(hWnd, SB_VERT, 5, TRUE);
    SetScrollPos(hWnd, SB_HORZ, 5, TRUE);
    InvalidateRect(hWnd, NULL, TRUE);

    if(wscrl->cline != wscrl->ocline || wscrl->ccol != wscrl->occol)
	SendMessage(GetParent(hWnd), WM_WSCRL_CHGCURRENT, 0, 0L);

    wscrl->oline = wscrl->line;
    wscrl->ocline = wscrl->cline;
    wscrl->ocol = wscrl->col;
    wscrl->occol = wscrl->ccol;

    return(0);
}

WscrScrollMouse(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    WSCROLL     *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
    int         xc, yc, x, y, j, i, nHeight = H, nWidth = W;
    HWND        hEditWnd;

    if(wscrl->mousedown == 0 && wMsg == WM_MOUSEMOVE) return(0);
    xc = LOWORD(lParam);
    yc = HIWORD(lParam);
    x = nWidth * WscrScrollGetWidth(wscrl, -1);
    y = nHeight * WscrScrollGetHeight(wscrl, -1);
    i = wscrl->cline;
    j = wscrl->ccol;

    /* Mouse down */
    if(wscrl->mousedown && xc < 0 + x) {
	j = wscrl->ccol - 1;
//        PostMessage(hWnd, WM_MOUSEMOVE, wParam, lParam);
	}
    else if(wscrl->mousedown && yc < 0 + y) {
	i = wscrl->cline - 1;
//        PostMessage(hWnd, WM_MOUSEMOVE, wParam, lParam);
	}
    else {
	// Colonne
	if(xc < 0) j = -3;
	else if(xc < 0 + x) j = -1;
	else {
	    for(j = wscrl->col ; j < wscrl->col + wscrl->ncv; j++) {
		x += nWidth * WscrScrollGetWidth(wscrl, j);
		if(xc < 0 + x) break;
		}
	    }

	// Line
	if(yc < 0) i = -2;
	else if(yc < 0 + y) i = -1;
	else {
	    for(i = wscrl->line ; i < wscrl->line + wscrl->nlv; i++) {
		y += nHeight * WscrScrollGetHeight(wscrl, i);
		if(yc < 0 + y) break;
		}
	    }
	}

    j = _max_(0, j);
    j = _min_(j, WscrScrollGetNc(wscrl) - 1);
    i = _max_(0, i);
    i = _min_(i, WscrScrollGetNl(wscrl) - 1);

    switch(wMsg) {
	case WM_MOUSEMOVE :
	    if(i == wscrl->ocline && j == wscrl->occol) return(0);
	    WscrScrollMouseToKeyb(hWnd, wscrl, i, j, 1);
	    goto disp;

	case WM_LBUTTONDBLCLK :
	    WscrScrollMouseToKeyb(hWnd, wscrl, i, j, 0);
	    WscrScrollKeyboard(hWnd, (WPARAM)VK_RETURN);
//            if(wscrl->settext) WscrScrollSetEditText(hWnd);
//            else if(wscrl->fnkey) wscrl->fnkey(wscrl, wscrl->cline, wscrl->ccol, SCR_ENTER);
//            WscrlInvalidate(wscrl);
	    goto disp;

	case WM_LBUTTONUP :
	    if(wscrl->mousedown) ReleaseCapture();
	    wscrl->mousedown = 0;
	    return(0);

	case WM_LBUTTONDOWN   :
	    WscrScrollMouseToKeyb(hWnd, wscrl, i, j, 0);
	    wscrl->mousedown = 1;
	    SetCapture(hWnd);
	    goto disp;

	default : return(0);
	}
disp:
    WscrScrollRedisplay(hWnd, wscrl);
    return(0);
}

WscrScrollMouseToKeyb(HWND hWnd, WSCROLL *wscrl, int i, int j, int shift)
{
    int     key, n, k;

    key = (wscrl->cline > i) ? VK_UP : VK_DOWN;
    n = abs(wscrl->cline - i);
    for(k = 0 ; k < n ; k++) WscrScrollKeyboard(hWnd, (WPARAM)key);

    key = (wscrl->ccol > j) ? VK_LEFT : VK_RIGHT;
    n = abs(wscrl->ccol - j);
    for(k = 0 ; k < n ; k++) WscrScrollKeyboard(hWnd, (WPARAM)key);

    return(0);
}

/*
WscrScrollMouse(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    WSCROLL     *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
    int         xc, yc, x, y, j, i, nHeight = H, nWidth = W;
    HWND        hEditWnd;

    if(wscrl->mousedown == 0 && wMsg == WM_MOUSEMOVE) return;
    xc = LOWORD(lParam);
    yc = HIWORD(lParam);
    x = nWidth * WscrScrollGetWidth(wscrl, -1);
    y = nHeight * WscrScrollGetHeight(wscrl, -1);
    i = wscrl->cline;
    j = wscrl->ccol;

    // Mouse down
    if(wscrl->mousedown && xc < 0 + x) {
	j = wscrl->ccol - 1;
//        PostMessage(hWnd, WM_MOUSEMOVE, wParam, lParam);
	}
    else if(wscrl->mousedown && yc < 0 + y) {
	i = wscrl->cline - 1;
//        PostMessage(hWnd, WM_MOUSEMOVE, wParam, lParam);
	}
    else {
	// Colonne
	if(xc < 0) j = -3;
	else if(xc < 0 + x) j = -1;
	else {
	    for(j = wscrl->col ; j < wscrl->col + wscrl->ncv; j++) {
		x += nWidth * WscrScrollGetWidth(wscrl, j);
		if(xc < 0 + x) break;
		}
	    }

	// Line
	if(yc < 0) i = -2;
	else if(yc < 0 + y) i = -1;
	else {
	    for(i = wscrl->line ; i < wscrl->line + wscrl->nlv; i++) {
		y += nHeight * WscrScrollGetHeight(wscrl, i);
		if(yc < 0 + y) break;
		}
	    }
	}

    wscrl->ccol = _max_(0, j);
    wscrl->ccol = _min_(wscrl->ccol, WscrScrollGetNc(wscrl) - 1);
    wscrl->cline = _max_(0, i);
    wscrl->cline = _min_(wscrl->cline, WscrScrollGetNl(wscrl) - 1);

    switch(wMsg) {
	case WM_MOUSEMOVE :
	    if(wscrl->cline == wscrl->ocline && wscrl->ccol == wscrl->occol)
		return;
	    wscrl->sellineto = wscrl->cline;
	    wscrl->selcolto = wscrl->ccol;
	    if(WSL_ONESEL & wscrl->seltype) {
		wscrl->sellinefrom = wscrl->cline;
		wscrl->selcolfrom = wscrl->ccol;
		}
	    goto disp;

	case WM_LBUTTONDBLCLK :
	    if(wscrl->settext) WscrScrollSetEditText(hWnd);
	    else if(wscrl->fnkey) wscrl->fnkey(wscrl, wscrl->cline, wscrl->ccol, SCR_ENTER);
	    WscrlInvalidate(wscrl);
	    goto disp;

	case WM_LBUTTONUP :
	    if(wscrl->mousedown) ReleaseCapture();
	    wscrl->mousedown = 0;
	    return;

	case WM_LBUTTONDOWN   :
	    wscrl->mousedown = 1;
	    SetCapture(hWnd);
	    if(!(wscrl->seltype & WSL_ONESEL) && WscrIsShift()) {
		wscrl->sellineto = wscrl->cline;
		wscrl->selcolto = wscrl->ccol;
		}
	    else {
		wscrl->sellinefrom = wscrl->sellineto = wscrl->cline;
		wscrl->selcolfrom  = wscrl->selcolto  = wscrl->ccol;
		}
	    if(wscrl->cline == wscrl->ocline && wscrl->ccol == wscrl->occol)
		return;
	    goto disp;

	default : return;
	}
disp:
    WscrScrollRedisplay(hWnd, wscrl);
}
*/

/* ============= PAINT ============================ */

int     *WSCRL_WIDTHS = 0;

WscrScrollPaintHeader(HWND hWnd)
{
    WSCROLL     *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
    HWND        hWndHeader;
    int         ncHeader;
    int         nc = WscrScrollGetNc(wscrl), Item = 0, j, w, x = 0;
    U_ch        *txt;
    RECT        rect;

    if(WscrScrollGetHeight(wscrl, -1) == 0) return(0);

    WscrScrollCreateHeader(hWnd);
    hWndHeader = (HWND) GetWindowLongPtr(hWnd, 8);
    WscrHeaderSetPos(hWndHeader, 0, 0, 0, 0);


    // Add items if necessary
    ncHeader   = WscrGetHeaderNc(hWndHeader);
    for(j = ncHeader; j < nc + 1; j++)
	WscrInsertHeaderItem(hWndHeader, j, 50, " ");

    GetClientRect(hWnd, &rect);
    x = w = WSCRL_WIDTHS[0];
    if(w > 0) {
	if(ncHeader <= nc + 1) WscrInsertHeaderItem(hWndHeader, nc + 1, 50, " ");
	WscrSetHeaderWidth(hWndHeader, Item, w);
	if(wscrl->gettext) txt = wscrl->gettext(wscrl, -1, -1);
	else               sprintf(txt, "[%d; %d]", -1, -1);
	if(txt == 0) txt = " ";
	WscrSetHeaderText(hWndHeader, Item, txt, 1);
	Item++;
	}

    for(j = wscrl->col ; j < nc; j++) {
	if(X0 + x > X1) break;
	w = WSCRL_WIDTHS[1 + j - wscrl->col];
	if(wscrl->gettext) txt = wscrl->gettext(wscrl, -1, j);
	else               sprintf(txt, "[%d; %d]", -1, j);
	if(txt == 0) txt = " ";
	WscrSetHeaderText(hWndHeader, Item, txt, 1);
	WscrSetHeaderWidth(hWndHeader, Item, w);
	Item++;
	x += w;
	}
    WscrSetHeaderText(hWndHeader, Item, " ", 1);
    WscrSetHeaderWidth(hWndHeader, Item, 500);
    return(0);
}

WscrScrollGetColWidth(WSCROLL *wscrl, int width)
{
    int     i, nc, tot = 0;

    nc = WscrScrollGetNc(wscrl);
    for(i = -1 ; i < nc ; i++)
	tot += WscrScrollGetWidth(wscrl, i);

    width -= 10;
    width = (width / tot);
    return(width);
}


WscrScrollCalcColWidth(HWND hWnd, WSCROLL *wscrl)
{
    HDC             hDC = GetDC(hWnd);
    RECT            rect;
    TEXTMETRIC      tm;
    int             j, eq, reste, w, nc, wtot, nWidth, x, ncv;

    GetClientRect(hWnd, &rect);
    SelectObject(hDC, W_SCRL_FONT);
    GetTextMetrics(hDC, &tm);
    W = nWidth  = (120 * tm.tmAveCharWidth) / 100;

    nc = WscrScrollGetNc(wscrl);
    SCR_free((char *)WSCRL_WIDTHS);
    WSCRL_WIDTHS = (int *)SCR_malloc(sizeof(int) * (nc + 1));
    WSCRL_WIDTHS[0] = x = nWidth * WscrScrollGetWidth(wscrl, -1);
    for(j = wscrl->col ; j < nc; j++) {
	WSCRL_WIDTHS[1 + j - wscrl->col] = w =
				  nWidth * WscrScrollGetWidth(wscrl, j);
	x += w;
	/*
	if(X0 + x > X1) {
	    x -= w;
	    break;
	    }
	*/
	if(X0 + x > X1) {
	    x -= w;
	    WSCRL_WIDTHS[1 + j - wscrl->col] = w = X1 - X0 - x;
	    x += w;
	    }
	}

    ncv = j - wscrl->col;
    if(ncv == 0) {
	WSCRL_WIDTHS[1] = X1 - X0 - x;
	ncv = 1;
	reste = 0;
	}
    else {
	reste = X1 - X0 - x;
	eq = reste / ncv;
	for(j = 1 ; j <= ncv ; j++)
	    if(WSCRL_WIDTHS[j]) {
		WSCRL_WIDTHS[j] += eq;
		reste -= eq;
		}

	for(j = 1 ; j <= ncv ; j++) {
	    if(reste == 0) break;
	    if(WSCRL_WIDTHS[j]) {
		WSCRL_WIDTHS[j]++;
		reste--;
		}
	    }
	}

    return(0);
}

WscrScrollPaint(HWND hWnd)
{
    WSCROLL         *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
    HDC             hDC = GetDC(hWnd);
    RECT            rect;
    TEXTMETRIC      tm;
    int             i, j, nHeight, y, x, h, w, headh,
		    nl = WscrScrollGetNl(wscrl),
		    nc = WscrScrollGetNc(wscrl);
    HPEN            hPenBox = CreatePen(PS_SOLID, 1, 0L);
    HPEN            hPenDot = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));

    /* Window Title */
    if(wscrl->gettext)
	WscrSetWindowTextOem(hWnd, wscrl->gettext(wscrl, -2, -2));
    SelectObject(hDC, W_SCRL_FONT);
    GetClientRect(hWnd, &rect);
    GetTextMetrics(hDC, &tm);
    H = nHeight = tm.tmHeight + tm.tmExternalLeading + 3;
    WscrScrollCalcColWidth(hWnd, wscrl);

    SelectObject(hDC, hPenBox);

    /* Titres lines */
    if(wscrl->line < 0) wscrl->line = 0;
    if(wscrl->col  < 0) wscrl->col = 0;

    y = headh = nHeight * WscrScrollGetHeight(wscrl, -1);
    w = WSCRL_WIDTHS[0];
    if(w) {
	for(j = wscrl->line ; j < nl; j++) {
	    h = nHeight * WscrScrollGetHeight(wscrl, j);
	    WscrScrollPaintCell(hWnd, hDC, wscrl, j, -1, X0, Y0 + y, w, h);
	    y += h;
	    if(Y0 + y > Y1) break;
	    }
	}

    y = headh;
    /* Centre */
    for(i = wscrl->line; i < nl; i++) {
	h = nHeight * WscrScrollGetHeight(wscrl, i);
	x = WSCRL_WIDTHS[0];
	for(j = wscrl->col ; j < nc; j++) {
	    w = WSCRL_WIDTHS[1 + j - wscrl->col];
	    WscrScrollPaintCell(hWnd, hDC, wscrl, i, j, X0 + x, Y0 + y, w, h);
	    x += w;
	    if(X0 + x > X1) break;
	    }
	y += h;
	if(Y0 + y > Y1) break;
	}

    /* Clear empty screen */
    WscrScrollPaintCell(hWnd, hDC, wscrl, -3, -3, X0, Y0 + y, X1 - X0, Y1 - Y0 - y);
    WscrScrollPaintCell(hWnd, hDC, wscrl, -3, -3, X0 + x, Y0 + nHeight, X1 - X0 - x, y - nHeight);

    /* Cells Separation lines */
    SelectObject(hDC, hPenDot);

    y = nHeight * WscrScrollGetHeight(wscrl, -1);
    wscrl->nlv = 0;
    for(i = wscrl->line; i < nl; i++) {
	h = nHeight * WscrScrollGetHeight(wscrl, i);
	y += h;
	if(Y0 + y > Y1) break;
	MoveToEx(hDC, X0, Y0 + y, NULL);
	LineTo(hDC, X1, Y0 + y);
	wscrl->nlv++;
	}
    wscrl->nlv = _max_(1, wscrl->nlv);

    x = WSCRL_WIDTHS[0];
    wscrl->ncv = 0;
    for(i = wscrl->col; i < nc; i++) {
	w = WSCRL_WIDTHS[1 + i - wscrl->col];
	x += w;
	if(X0 + x > X1) break;
	MoveToEx(hDC, X0 + x, Y0 + headh, NULL);
	LineTo(hDC, X0 + x, Y1);
	wscrl->ncv++;
	}
    wscrl->ncv = _max_(1, wscrl->ncv);

    /* Titles Separation lines */
    SelectObject(hDC, hPenBox);
    MoveToEx(hDC, X0, Y0 + headh, NULL);
    LineTo(hDC, X1, Y0 + headh);
    w = WSCRL_WIDTHS[0];
    MoveToEx(hDC, X0 + w, Y0 + headh, NULL);
    LineTo(hDC, X0 + w, Y1);

    /* End Paint */
    ReleaseDC(hWnd, hDC);
    DeleteObject(hPenBox);
    DeleteObject(hPenDot);
    ValidateRect(hWnd, NULL);
    return(0);
}

WscrScrollIsSelected(WSCROLL *wscrl, int i, int j)
{
    int     from, to, tmp, colsel = 0, linesel = 0;

    if(i < 0 || j < 0) return(0);

    from = wscrl->sellinefrom;
    to   = wscrl->sellineto;

    if(from > to) {
	tmp = from;
	from = to;
	to = tmp;
	}
    if(from < 0) return(0);
    if(i >= from && i <= to) linesel = 1;

    if(wscrl->seltype & WSL_LINESEL) return(linesel);

    from = wscrl->selcolfrom;
    to   = wscrl->selcolto;

    if(from > to) {
	tmp = from;
	from = to;
	to = tmp;
	}
    if(from < 0) return(0);
    if(j >= from && j <= to) colsel = 1;

    if(wscrl->seltype & WSL_COLSEL)  return(colsel);

    return(linesel && colsel);
}

WscrScrollGetStringWidth(HDC hDC, U_ch *txt)
{
    SIZE    size;
    int     bxy = LOWORD(GetDialogBaseUnits());

    if(txt == 0) return(0);
    GetTextExtentPoint32(hDC, txt, strlen(txt), &size);
    return(size.cx);
}


WscrScrollPaintCell(hWnd, hDC, wscrl, i, j, x, y, w, h)
HWND    hWnd;
HDC     hDC;
WSCROLL *wscrl;
int     i, j, w, h, x, y;
{
    HBRUSH  hBrush = 0;
    HRGN    hRgn;
    U_ch    *txt = 0, buf[256];
    int     color, font = 0, align = 0, len;
    RECT    rect;

    if(w == 0 || h == 0) return(0);

    if(wscrl->editon && i == wscrl->cline && j == wscrl->ccol) return(0);
    if(i == wscrl->cline) {
	if((WSL_LINESEL & wscrl->seltype) || j == wscrl->ccol) color = 4;
	else if(j == -1)     color = 3;
	else                 color = 1;
	}
    else if(j == wscrl->ccol) {
	if((WSL_COLSEL & wscrl->seltype)) color = 4;
	else if(i == -1)          color = 3;
	else                 color = 1;
	}
    else if(i == -1 || j == -1)  color = 2;
    else if(i == -2)        color = 1;
    else if(i == -3)        color = 1;
    else                    color = 1;

    if(color < 4 && WscrScrollIsSelected(wscrl, i, j)) color = 3;
//    if(color == 4 && (wscrl->seltype & (WSL_LINESEL|WSL_COLSEL)))
//        color = 3;

    if(wscrl->getcolor)  color = wscrl->getcolor(wscrl, i, j);
/*  switch(color) {
	case 0 : hBrush = GetStockObject(WHITE_BRUSH); break;
	case 1 : hBrush = GetStockObject(LTGRAY_BRUSH); break;
	case 2 : hBrush = GetStockObject(GRAY_BRUSH); break;
	case 3 : hBrush = GetStockObject(DKGRAY_BRUSH); break;
	default: hBrush = GetStockObject(BLACK_BRUSH); break;
	}
*/
    switch(color) {
	case 0 : hBrush = W_BRUSH_WHITE; break;
	case 1 : hBrush = W_BRUSH_LTGRAY; break;
	case 2 : hBrush = W_BRUSH_GRAY; break;
	case 3 : hBrush = W_BRUSH_DKGRAY; break;
//        case 3 : hBrush = W_BRUSH_YELLOW; break;
	default: hBrush = W_BRUSH_YELLOW; break;
	}


    SetRect(&rect, x + 1, y + 1, x + w, y + h);
    FillRect(hDC, &rect, hBrush);

    if(j < -2 || i < -2) return(0);
    txt = buf;
    if(wscrl->gettext) txt = wscrl->gettext(wscrl, i, j);
    else               sprintf(txt, "[%d; %d]", i, j);
    if(txt == 0) return(0);

//    if(color >= 3) SetTextColor(hDC, WCOL_WHITE);
    if(wscrl->enabled) {
	if(color == 3) SetTextColor(hDC, WCOL_WHITE);
	else           SetTextColor(hDC, WCOL_BLACK);
	}
    else  SetTextColor(hDC, WCOL_GRAY);

    if(wscrl->getfont) {
	font = wscrl->getfont(wscrl, i, j);
	switch(font) {
	    case 1 : SelectObject(hDC, W_SCRL_FONTI); break;
	    case 2 : SelectObject(hDC, W_SCRL_FONTB); break;
	    case 3 : SelectObject(hDC, W_SCRL_FONTBI); break;
	    default: SelectObject(hDC, W_SCRL_FONT); break;
	    }
	}

    SCR_strlcpy(buf, txt, 254);
    U_ljust_text(buf);
    SCR_strip(buf);
    SCR_OemToAnsi(buf, buf);
    if(wscrl->getalign) {
	align = wscrl->getalign(wscrl, i, j);
	len = WscrScrollGetStringWidth(hDC, buf);
	switch(align) {
	    case SCR_CENTER : x += (w - len) / 2; break;
	    case SCR_RJUST  : x += (w - len - 2); break;
	    default: x += 2; break;
	    }
	}



    SetBkMode(hDC, TRANSPARENT);
//  Debug("%5d - %5d - %5d - '%s'\n", x, w, len, txt);
    TextOut(hDC, x, y, buf, strlen(buf));
//  WscrTextOutOem(hDC, x, y, txt);
    return(0);
}


WscrScrollCommand(HWND hWnd, WPARAM wParam)
{
    WSCROLL     *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);

    if(wParam < 600 || wParam > 700) return(0);
    if(wscrl->fncmd) wscrl->fncmd(wscrl, wscrl->cline, wscrl->ccol, wParam);
    WscrlInvalidate(wscrl);
    WscrScrollRedisplay(hWnd, wscrl);
    return(0);
}

WscrScrollGetWidth(wscrl, j)
WSCROLL     *wscrl;
int         j;
{
    int     w = 10;

    if(j == -1) w = 0;

    if(wscrl->getwidth)  w = wscrl->getwidth(wscrl, j);
    return(w);
}

WscrScrollGetHeight(wscrl, i)
WSCROLL     *wscrl;
int         i;
{
    int     h = 1;

    if(i == -1) h = 0;
    if(wscrl->getheight) h = wscrl->getheight(wscrl, i);
    return(h);
}

WscrScrollGetNl(wscrl)
WSCROLL     *wscrl;
{
    if(wscrl->getnl) return(wscrl->getnl(wscrl));
    return(100);
}

WscrScrollGetNc(wscrl)
WSCROLL     *wscrl;
{
    if(wscrl->getnc) return(wscrl->getnc(wscrl));
    return(20);
}

WscrScrollCCoord(WSCROLL *wscrl, int line, int col, int *x, int *y, int *w, int *h)
{
    int     i, j, nHeight = H, nWidth = W;

    *w = WSCRL_WIDTHS[1 + col - wscrl->col];
    *h = nHeight * WscrScrollGetHeight(wscrl, i);
    *x = WSCRL_WIDTHS[0];
    *y = nHeight * WscrScrollGetHeight(wscrl, -1);

    if(line < 0) *y = 0;
    if(col  < 0) *x = 0;
    if(line < wscrl->line || col < wscrl->col) {
       *x = -*w;
       *y = -*h;
       return(0);
       }

    for(j = wscrl->col ; j < col; j++)
	*x += WSCRL_WIDTHS[1 + j - wscrl->col];
    for(i = wscrl->line ; i < line; i++)
	*y += nHeight * WscrScrollGetHeight(wscrl, i);

    return(0);
}

WscrScrollInvalidateEdit(HWND hWnd)
{
    HWND    hWndEdit = (HWND)GetWindowLongPtr(hWnd, 4);

    InvalidateRect(hWndEdit, NULL, TRUE);
    return(0);
}


WscrScrollSaveEditText(HWND hWnd)
{
    WSCROLL     *wscrl = (WSCROLL *)GetWindowLongPtr(hWnd, 0);
    HWND        hWndEdit = (HWND) GetWindowLongPtr(hWnd, 4);
    U_ch        *text = 0;

    if(wscrl->settext == 0) return(0);
    text = WscrGetWindowTextOem(hWndEdit);
    if(text == 0) return(0);
    wscrl->settext(wscrl, wscrl->cline, wscrl->ccol, text);
    SCR_free(text);
    return(0);
}

LONG FAR PASCAL WscrScrollEditFn(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    HWND        hWndParent = GetParent(hWnd);
    WSCROLL     *wscrl = (WSCROLL *)GetWindowLongPtr(hWndParent, 0);

    switch(wMsg) {
	case WM_CHAR :
	    switch(wParam) {
		case VK_ESCAPE :
		case VK_RETURN : return(0L);
		}
	    break;

	case WM_KEYDOWN :
	    switch(wParam) {
		case VK_ESCAPE :
		    WscrScrollSetEditText(hWndParent);
		    /* NO BREAK */
		case VK_RETURN :
		    wParam = 0;
		    goto save;

		case VK_TAB :
		    if(WscrIsShift()) wParam = VK_LEFT;
		    else              wParam = VK_RIGHT;
		    goto save;
		case VK_LEFT :
		case VK_RIGHT :
		case VK_HOME :
		case VK_END :
		    if(!WscrIsCtrl()) break;

		case VK_UP :
		case VK_DOWN :
		case VK_PRIOR :
		case VK_NEXT :
	    save:
		    SetFocus(hWndParent);
		    SendMessage(hWndParent, WM_KEYDOWN, wParam, lParam);
		    return(0L);

		default        : break;
		}
	    break;

	case WM_KILLFOCUS :
	    WscrScrollSaveEditText(hWndParent);
	    wscrl->editon = 0;
	    wscrl->ocline = -1;
	    break;

	default : break;
	}

    return(CallWindowProc(W_SCRL_DFT_EDITFN, hWnd, wMsg, wParam, lParam));
}


/* ============= SCROLL TO WSCROLL ============== */

W2scrlBegin(WSCROLL *wscrl, int *i, int *j)
{
    SCROLL  *scrl = (SCROLL *) wscrl->data;

    if(scrl->sc_fbeg) return(scrl->sc_fbeg(scrl, i, j));
    return(0);
}

W2scrlEnd(WSCROLL *wscrl, int i, int j)
{
    SCROLL  *scrl = (SCROLL *) wscrl->data;

    if(scrl->sc_fend) return(scrl->sc_fend(scrl, i, j));
    return(0);
}

W2scrlGetNl(WSCROLL *wscrl)
{
    SCROLL  *scrl = (SCROLL *) wscrl->data;

    return(scrl->sc_fnl(scrl));
}

W2scrlGetNc(WSCROLL *wscrl)
{
    SCROLL  *scrl = (SCROLL *) wscrl->data;

    return(scrl->sc_fnc(scrl));
}

W2scrlGetWidth(WSCROLL *wscrl, int col)
{
    SCROLL  *scrl = (SCROLL *) wscrl->data;

    if(col == -1) return(scrl->sc_ilc0);
    if(W2scrlGetNc(wscrl) == 1) return(200);
    return(scrl->sc_flen(scrl, col));
}

W2scrlGetHeight(WSCROLL *wscrl, int line)
{
    return(1);
}

U_ch  *W2scrlGetText(WSCROLL *wscrl, int i, int j)
{
    SCROLL  *scrl = (SCROLL *) wscrl->data;

    if(i == -2 && j == -2) return(scrl->sc_ftitle(scrl));
    if(i < 0 && j < 0) return("");
    if(i < 0) return(scrl->sc_fctitle(scrl, j));
    if(j < 0) return(scrl->sc_fltitle(scrl, i));
    return(scrl->sc_ftext(scrl, i, j));
}

W2scrlFnkey(WSCROLL *wscrl, int i, int j, int scrkey)
{
    SCROLL  *scrl = (SCROLL *) wscrl->data;

    switch(scrkey) {
	case SCR_ENTER  : if(wscrl->settext == 0 && scrl->sc_fedit)
			    return(scrl->sc_fedit(scrl, i, j));
			  break;
	case SCR_A_A    :   /* BP_M 21-03-2012 13:15 */
	case SCR_INSERT : if(scrl->sc_fins) return(scrl->sc_fins(scrl, i, j));
			  break;
	case SCR_DELETE : if(scrl->sc_fdel) return(scrl->sc_fdel(scrl, i, j));
			  break;
	default         : if(scrl->sc_ffn)  scrl->sc_ffn(scrl, scrkey, i, j); /* JMP 11-02-97 */
			  break;
	}
    return(0);
}

W2scrlCommand(WSCROLL *wscrl, int i, int j, int wParam)
{
    SCROLL  *scrl = (SCROLL *) wscrl->data;
    int     winkey;

    if(scrl->sc_scmts == 0) return(0);

    winkey = WscrKeyScrToWin(scrl->sc_scmts[wParam - 600].yk_def);
    W2scrlFnkey(wscrl, i, j, winkey);
    return(0);
}

HMENU W2scrlCreateMenu(SCROLL *scrl)
{
    HMENU   hMenu = 0, hSubMenu;
    int     i;
    char    buf[81];
    YYKEYS  *yks = scrl->sc_scmts;

    if(yks == 0) return(hMenu);
    hSubMenu = CreateMenu();
    hMenu = CreateMenu();
    if(hSubMenu == 0 || hMenu == 0) return(0);

    for(i = 0 ; yks[i].yk_word ; i++) {
	if(yks[i].yk_def == 0) continue;
	sprintf(buf, "%s\t%s", yks[i].yk_word, SCR_fnk_txt(yks[i].yk_def)); /* BP_M 20-02-2003 15:36 */
	AppendMenu(hSubMenu, MF_STRING, 600 + i, buf);
	}
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, "Actions");
    return(hMenu);
}

WSCROLL *WscrSCROLLToWSCROLL(SCROLL *scrl)
{
    WSCROLL *wscrl;
    HMENU   hMenu;

    wscrl = (WSCROLL *)SCR_malloc(sizeof(WSCROLL));
    wscrl->data = (char *)scrl;
    wscrl->gettext = W2scrlGetText;
    wscrl->getnl   = W2scrlGetNl;
    wscrl->getnc   = W2scrlGetNc;
    wscrl->getwidth= W2scrlGetWidth;
    wscrl->getheight= W2scrlGetHeight;
    wscrl->fnkey   = W2scrlFnkey;
    wscrl->fncmd   = W2scrlCommand;
    wscrl->fnend   = W2scrlEnd;
    wscrl->fnbeg   = W2scrlBegin;

    hMenu = W2scrlCreateMenu(scrl);
    wscrl->hMenu = hMenu;
    return(wscrl);
}


/* ============== FONCTIONS POUR MPAGES ============ */

WscrScrollSetCurSel(HWND hWndScroll, int line, int col)
{
    WSCROLL     *wscrl;

    if(hWndScroll == 0) return(0);
    wscrl = (WSCROLL *)GetWindowLongPtr(hWndScroll, 0);

//    if(line >= 0) wscrl->cline = wscrl->sellinefrom = wscrl->sellineto = line;
//    if(col >= 0) wscrl->ccol = wscrl->selcolfrom = wscrl->selcolto = col;
    if(line >= 0) wscrl->cline = line;
    if(col >= 0) wscrl->ccol = col;
    WscrScrollCalcPos(wscrl);
    WscrScrollCellChanged(hWndScroll, line, col);
    return(0);
}


WscrScrollCellChanged(HWND hWndScroll, int line, int col)
{
    WSCROLL     *wscrl;

    if(hWndScroll == 0) return(0);
    wscrl = (WSCROLL *)GetWindowLongPtr(hWndScroll, 0);

//    InvalidateRect(hWndScroll, NULL, TRUE);
    WscrlInvalidate(wscrl);
    WscrScrollRedisplay(hWndScroll, wscrl);
    return(0);
}

WscrScrollSetSelect(HWND hWndScroll, int fl, int tl, int fc, int tc)
{
    WSCROLL     *wscrl;

    if(hWndScroll == 0) return(0);
    wscrl = (WSCROLL *)GetWindowLongPtr(hWndScroll, 0);

//    if(line >= 0) wscrl->cline = wscrl->sellinefrom = wscrl->sellineto = line;
//    if(col >= 0) wscrl->ccol = wscrl->selcolfrom = wscrl->selcolto = col;
    wscrl->sellinefrom = fl;
    wscrl->selcolfrom  = fc;
    wscrl->sellineto   = tl;
    wscrl->selcolto    = tc;
    WscrScrollCalcPos(wscrl);
    WscrScrollCellChanged(hWndScroll, 0, 0);
    return(0);
}

/************************** WscrScrollEdit ******************************/

WscrScrollEdit(HWND hWndParent, WSCROLL *wscrl)
{
    HWND    hWnd;
    MSG     msg;
    int     ParentEn;

    if(hWndParent == 0) hWndParent = hMainWnd;

    hWnd = CreateWindow("WscrScroll", 0L,
	    WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    hWndParent, wscrl->hMenu, hInst, 0L);

    if(hWnd == 0) return(-1);

    if(hWndParent) {
	ParentEn = IsWindowEnabled(hWndParent);
	EnableWindow(hWndParent, FALSE);
	}
    SendMessage(hWnd, WM_WSCRL_SET, 0, (LPARAM)wscrl);
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    /* LOOP MSG */
    EnableWindow(hWnd, TRUE);
    SetFocus(hWnd);
    wscrl->end = 0;
    while(GetMessage(&msg, (HWND)0, 0, 0)) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
	if(wscrl->end) break;
	}

    DestroyWindow(hWnd);
    if(hWndParent) {
	EnableWindow(hWndParent, ParentEn);
	SetFocus(hWndParent);
	}
    return(0);
}




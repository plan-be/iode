#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"

LONG FAR PASCAL WscrEMFProc();

/*NH*/
ATOM WscrEMFRegister()
{
    WNDCLASS    EMFWndClass;
    static ATOM EMFClass = 0;

    if(EMFClass) return(EMFClass);

    EMFWndClass.hInstance = hInst;
    EMFWndClass.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    EMFWndClass.lpszMenuName = NULL;
    EMFWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    EMFWndClass.hIcon = NULL;
    EMFWndClass.hbrBackground = GetStockObject(WHITE_BRUSH); /* JMP 02-12-97 */
    EMFWndClass.cbClsExtra = 0;
    EMFWndClass.cbWndExtra = 20;       /* JMP 20-03-98 */
    EMFWndClass.lpszClassName = "WscrEMF";
    EMFWndClass.lpfnWndProc = WscrEMFProc;
    return(EMFClass = RegisterClass(&EMFWndClass));
}

/*NH*/
HWND WscrEMFWMFCreateGnl(int type, HWND hParent, char *filename, HANDLE hemf,
			int del, int x, int y, int w, int h, char *title)
{
    HWND    hWnd = 0;
    HMENU   hMenu;
    char    *tmp = 0;
    extern HWND hWndDOS;
    int     nb = -x, i = -y;

    if(x < -1) WscrEMFCalcWindowPos(nb, i, &x, &y, &w, &h);

    if(x < 0) x = CW_USEDEFAULT;
    if(y < 0) y = CW_USEDEFAULT;
    if(w < 0) w = CW_USEDEFAULT;
    if(h < 0) h = CW_USEDEFAULT;

    if(WscrEMFRegister() == 0) return(hWnd);
    hParent = WscrGetMainWnd(); /* JMP 04-03-17 */
/*#ifdef DOSW32
    if(hParent == 0) hParent = hWndDOS;
#else
    if(hParent == 0) hParent = hMainWnd;
#endif
*/
    hWnd = CreateWindow("WscrEMF", NULL,
			WS_OVERLAPPEDWINDOW,
			x, y, w, h,  /* JMP 20-03-98 */
			hParent, (HMENU)0, hInst, 0L);

    if(hWnd == 0) return(hWnd);
    if(filename) tmp = SCR_stracpy(filename);
    SetWindowLongPtr(hWnd, 0, (LONG)tmp);
    SetWindowLongPtr(hWnd, 4, (LONG)hemf);
    SetWindowLongPtr(hWnd, 8, (LONG)type);
    SetWindowLongPtr(hWnd, 12,(LONG)del);
    if(title) {
	tmp = SCR_stracpy(title);
	SCR_OemToAnsi(tmp, tmp);
	}
    SetWindowLongPtr(hWnd, 16,(LONG)tmp);

    hMenu = CreateMenu();
    AppendMenu(hMenu, MF_STRING, 1000, "&Print");
    AppendMenu(hMenu, MF_STRING, IDCANCEL, "&Quit");
    SetMenu(hWnd, hMenu);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    return(hWnd);
}

/*NH*/
HWND WscrEMFWMFCreate(int type, HWND hParent, char *filename, HANDLE hemf, int del)
{
    return(WscrEMFWMFCreateGnl(type, hParent, filename, hemf,
			del, -1, -1, -1, -1, (char *)0));
}

HWND WscrEMFCreateGnl(HWND hParent, char *filename, HANDLE hemf,
			int del, int x, int y, int w, int h, char *title)
{

    return(WscrEMFWMFCreateGnl(0, hParent, filename, hemf, del,
			    x, y, w, h, title));
}

HWND WscrWMFCreateGnl(HWND hParent, char *filename, HANDLE hemf,
			int del, int x, int y, int w, int h, char *title)
{

    return(WscrEMFWMFCreateGnl(1, hParent, filename, hemf, del,
			    x, y, w, h, title));
}

HWND WscrEMFCreate(HWND hParent, char *filename, HANDLE hemf, int del)
{

    return(WscrEMFWMFCreateGnl(0, hParent, filename, hemf, del,
					-1, -1, -1, -1, (char *)0));
}

HWND WscrWMFCreate(HWND hParent, char *filename, HANDLE hemf, int del)
{

    return(WscrEMFWMFCreateGnl(1, hParent, filename, hemf, del,
				    -1, -1, -1, -1, (char *)0));
}


/*NH*/
LONG FAR PASCAL WscrEMFProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
//    WORD    wId = LOWORD(wParam);
    HENHMETAFILE    hemf;

    switch(wMsg) {
	case WM_DESTROY :
	    WscrEMFDestroy(hWnd);
	    break;

	case WM_PAINT :
	    WscrEMFWMFPaint(hWnd);
	    return(0L);

	case WM_COMMAND :
	    if(wParam == IDCANCEL) PostMessage(hWnd, WM_CLOSE, 1, 0L);
	    if(wParam == 1000) WscrEMFPrintDirect(hWnd); /* JMP 23-03-98 */
	    return(0L);

	case WM_KEYDOWN :
	    if(wParam == VK_ESCAPE) PostMessage(hWnd, WM_CLOSE, 1, 0L);
	    return(0L);

	default : break;
	}

    return(DefWindowProc(hWnd, wMsg, wParam, lParam));
}

/*NH*/
WscrEMFDestroy(HWND hWnd)
{
    HANDLE  hemf;
    int     type = GetWindowLongPtr(hWnd, 8);
    int     del  = GetWindowLongPtr(hWnd, 12);
    char    *filename = (char *)GetWindowLongPtr(hWnd, 0);
    char    *title = (char *)GetWindowLongPtr(hWnd, 16);

    hemf = (HANDLE)GetWindowLongPtr(hWnd, 4);
    if(hemf) {
	if(type == 0) DeleteEnhMetaFile((HENHMETAFILE)hemf);
	else          DeleteMetaFile((HMETAFILE)hemf);
	}
    if(del && filename) unlink(filename);
    SCR_free(filename);
    SCR_free(title);
    hemf = 0;
    SetWindowLongPtr(hWnd, 4, (LONG)hemf);
    SetWindowLongPtr(hWnd, 0, (LONG)0);
    SetWindowLongPtr(hWnd, 16, (LONG)0);
    return(0);
}

/*NH*/
WscrEMFWMFPaint(HWND hWnd)
{
    int     type = GetWindowLongPtr(hWnd, 8);

    if(type == 0) WscrEMFPaint(hWnd);
    else          WscrWMFPaint(hWnd);

    return(0);
}

/*NH*/
WscrEMFPaint(HWND hWnd)
{
    HENHMETAFILE    hemf;
    HDC             hDC;
    RECT            rct;
    char            *filename, *buf, *title;
    int             lg, i;
    ENHMETAHEADER   *emhead;

    filename = (char *)GetWindowLongPtr(hWnd, 0);
    title = (char *)GetWindowLongPtr(hWnd, 16);
    hemf = (HENHMETAFILE)GetWindowLongPtr(hWnd, 4);
    if(filename) hemf = GetEnhMetaFile(filename);

    lg = GetEnhMetaFileHeader(hemf, 0, 0);
    if(lg) {
	buf = SCR_malloc(lg);
	emhead = (ENHMETAHEADER *) buf ;
	lg = GetEnhMetaFileHeader(hemf, lg, emhead);
	SCR_free(buf);
	}

    if(title) SetWindowText(hWnd, title);
    else {
	lg = GetEnhMetaFileDescription(hemf, 0, 0);
	if(lg > 0) {
	    buf = SCR_malloc(lg + 1);
	    GetEnhMetaFileDescription(hemf, lg, buf);
	    for(i = 0 ; buf[i] ; i++);
	    // buf[i] = '-'; /* JMP 23-02-98 */
	    SetWindowText(hWnd, buf);
	    SCR_free(buf);
	    }
	}

    hDC = GetDC(hWnd);
    GetClientRect(hWnd, &rct);
    PlayEnhMetaFile(hDC, hemf, &rct);

    if(filename) DeleteEnhMetaFile(hemf);
    ReleaseDC(hWnd, hDC);
    ValidateRect(hWnd, NULL);
    return(0);
}

/*NH*/
WscrEMFPrint(HWND hWnd, char *filename, HENHMETAFILE hemf, int ask)
{
    extern HDC      WprhDC;
    RECT            rct;
    int             rc = -1;

    if(filename) hemf = GetEnhMetaFile(filename);
    if(hemf == 0) return(-1);

    if(WprPrinterInit(hWnd, ask, "titre")) goto fin;
    rct.left = WprHORZRES / 10;
    rct.top = rct.left;
    rct.bottom = WprHORZRES * 0.5;
    rct.right = (1205 * WprHORZRES * 0.5) / 835;
    PlayEnhMetaFile(WprhDC, hemf, &rct);
    WprPrinterEnd();
    rc = 0;
fin:
    if(filename) DeleteEnhMetaFile(hemf);
    return(rc);
}

/*NH*/
WscrWMFPrint(HWND hWnd, char *filename, HMETAFILE hwmf, int ask)
{
    extern HDC      WprhDC;
    RECT            rect;
    int             rc = -1;

    if(filename) hwmf = GetMetaFile(filename);
    if(hwmf == 0) return(-1);

    if(WprPrinterInit(hWnd, ask, "titre")) goto fin;
    rect.left = WprHORZRES / 10;
    rect.top = rect.left;
    rect.bottom = WprHORZRES * 0.5;
    rect.right = (1205 * WprHORZRES * 0.5) / 835;
    SetMapMode(WprhDC, MM_ANISOTROPIC);
    SetViewportOrgEx(WprhDC, 0, 0, NULL);
    SetViewportExtEx(WprhDC, rect.right, rect.bottom, NULL);
    PlayMetaFile(WprhDC, hwmf);
    WprPrinterEnd();
    rc = 0;
fin:
    if(filename) DeleteMetaFile(hwmf);
    return(rc);
}

/*NH*/
WscrEMFPrintDirect(HWND hWnd)
{
    char            *filename;
    HENHMETAFILE    hemf;
    HMETAFILE       hwmf;
    int             type;

    type = GetWindowLongPtr(hWnd, 8);
    filename = (char *)GetWindowLongPtr(hWnd, 0);
    if(type == 0) {
	hemf = (HENHMETAFILE)GetWindowLongPtr(hWnd, 4);
	WscrEMFPrint(hWnd, filename, hemf, 1);
	}
    else {
	hwmf = (HMETAFILE)GetWindowLongPtr(hWnd, 4);
	WscrWMFPrint(hWnd, filename, hemf, 1);
	}
}

/*NH*/
WscrWMFPaint(HWND hWnd)
{
    HMETAFILE       hwmf;
    HDC             hDC;
    RECT            rect;
    char            *filename, *title;

    filename = (char *)GetWindowLongPtr(hWnd, 0);
    hwmf = (HMETAFILE) GetWindowLongPtr(hWnd, 4);
    if(filename) hwmf = GetMetaFile(filename);

    title = (char *)GetWindowLongPtr(hWnd, 16);
    if(title) SetWindowText(hWnd, title);

    hDC = GetDC(hWnd);
    GetClientRect(hWnd, &rect);
    SetMapMode(hDC, MM_ANISOTROPIC);
    SetViewportOrgEx(hDC, 0, 0, NULL);
    SetViewportExtEx(hDC, rect.right, rect.bottom, NULL);
    PlayMetaFile(hDC, hwmf);

    if(filename) DeleteMetaFile(hwmf);
    ReleaseDC(hWnd, hDC);
    ValidateRect(hWnd, NULL);
    return(0);
}


WscrEMFCalcWindowPos(int nb, int i, int *x, int *y, int *w, int *h)
{
    int     nrows, ncols;
    int     cx = GetSystemMetrics(SM_CXSCREEN);
    int     cy = GetSystemMetrics(SM_CYSCREEN);

    switch(nb) {
	case 1: nrows = 1; ncols = 1; break;
	case 2: nrows = 1; ncols = 2; break;
	case 3: nrows = 2; ncols = 2; break;
	case 4: nrows = 2; ncols = 2; break;
	case 5:
	case 6: nrows = 2; ncols = 3; break;
	case 7:
	case 8: nrows = 2; ncols = 4; break;
	case 9: nrows = 3; ncols = 3; break;
	case 10:
	case 11:
	case 12: nrows = 3 ; ncols = 4; break;
	default : nrows = 4; ncols = 1 + (nb - 1)/ 4 ; break;
	}

    *w = (4 * cx ) / (5 * ncols);
    *h = (4 * cy ) / (5 * nrows);
    if(nb == 2) *h = cy / (2 * nrows);

    *x = cx / 10 + (i % ncols) * *w;
    *y = cy / 10 + (i / ncols) * *h;
}

#endif






#include "scr4w.h"

/* MISC FUNCTIONS OEM-ANSI */

U_ch  *WscrOemVsAnsi(txt, type)
U_ch    *txt;
int     type;
{
    int     len;
    U_ch    *tmp = 0;

    if(txt == 0) return(txt);
    len = lstrlen(txt);
    tmp = SCR_malloc(len + 1);
    if(tmp == 0) return(tmp);
    if(type == 0) OemToAnsi(txt, tmp);
    else          AnsiToOem(txt, tmp);
    return(tmp);
}

U_ch  *WscrOemToAnsi(U_ch  *txt)
{
    return(WscrOemVsAnsi(txt, 0));
}

U_ch  *WscrAnsiToOem(U_ch  *txt)
{
    return(WscrOemVsAnsi(txt, 1));
}

WscrSetWindowTextOem(HWND hWnd, U_ch  *txt)
{
    U_ch    *tmp = WscrOemToAnsi(txt);

    if(tmp == 0) SetWindowText(hWnd, "");
    else  {
	SetWindowText(hWnd, tmp);
	SCR_free(tmp);
	}
    return(0);
}

U_ch  *WscrGetWindowTextOem(HWND hWnd)
{
    int     lg;
    U_ch    *text = 0;

    lg = 1 + GetWindowTextLength(hWnd);
    if(lg <= 1) return(text);
    text = SCR_malloc(lg + 1);
    if(text == 0) return(text);
    GetWindowText(hWnd, text, lg);
    AnsiToOem(text, text);
    return(text);
}

WscrTextOutOem(hDC, x, y, txt)
HDC     hDC;
int     x, y;
U_ch    *txt;
{
    U_ch    *tmp;
    int     len;

/*    TextOut(hDC, x, y, txt, lstrlen(txt));
*/
    tmp = WscrOemToAnsi(txt);
    if(tmp == 0) return(0);
    TextOut(hDC, x, y, tmp, lstrlen(tmp));
    SCR_free(tmp);
    return(0);
}

HMENU WscrCreateMenu(U_ch  *clst, U_ch  sep, int from)
{
    U_ch    **txt;
    HMENU   hMenu = CreateMenu();
    int     i;

    if(hMenu == 0) return(hMenu);

    txt = SCR_vtom(clst, sep);
    for(i = 0 ; txt[i] ; i++) {
	SCR_OemToAnsi(txt[i], txt[i]);
	AppendMenu(hMenu, MF_STRING, from + i, txt[i]);
	}

    SCR_free_tbl(txt);

    return(hMenu);
}

WscrSetSubMenu(HMENU hMenu, HMENU hSubMenu, int pos)
{
    U_ch    buf[128];

    GetMenuString(hMenu, pos, buf, 126, MF_BYPOSITION);
    ModifyMenu(hMenu, pos, MF_BYPOSITION | MF_POPUP, (UINT)hSubMenu, buf);
    return(0);
}


WscrRecordKey(HWND hWnd, int key)
{
    SCR_record_key(key);
    PostMessage(hWnd, WM_USER + 512, 0, 0L);
    return(0);
}

HANDLE WscrGetFont(int attr)
{
    if(attr == SCR_WIN_FIXED) return(W_SCRL_FONT_TERM);

    if(attr < 0 || attr > 19) attr = 0;
    else attr = WSCR_ATTRS[attr];
    switch(attr) {
	case SCR_BOLD      : return(W_SCRL_FONTB);
	case SCR_ITALIC    : return(W_SCRL_FONTI);
	case SCR_BOLDITALIC: return(W_SCRL_FONTBI);
	case SCR_WIN_FIXED : return(W_SCRL_FONT_TERM);
	default            : return(W_SCRL_FONT);
	}
}


/* ========================================================================
Calcule la position et la taille d'un champ d'une page ou de la page entière.

&EN pg : PAGE concernée
&EN fld_nb : numéro du champ ou
&EN2 -1 pour la position de la PAGE entière
&EN x = pointeur vers la position, en pixels, du coin supérieur gauche du champ ou de la page
&EN y = pointeur vers la position, en pixels, du coin supérieur gauche du champ ou de la page
&EN w = pointeur vers la largeur, en pixels
&EN h = pointeur vers la hauteur, en pixels

&RT
&EN -1 si la page ou le champ n'existe pas
&EN 0 si Ok

================================================================== */


WscrGetFldPos(PAGE *pg, int fld_nb, int *x, int *y, int *w, int *h)
{
    HWND    hWnd, hWndParent;
    RECT    rect;
    int     objnb;

    if(fld_nb == -2) {
	if(GetWindowRect(hMainWnd, &rect) == 0) return(-1);
	goto ok;
    }
    objnb = WscrFindPgNb(pg);
    if(objnb == 0) return(-1);
    hWndParent = WSCR_HWNDS[objnb].sh_hwnd;
    if(fld_nb == -1) {
	if(GetWindowRect(hWndParent, &rect) == 0) return(-1);
    }
    else {
	hWnd = GetDlgItem(hWndParent, fld_nb + WFLD_FIRST);
	if(GetWindowRect(hWnd, &rect) == 0) return(-1);
    }
ok:
    *x = rect.left;
    *y = rect.top;
    *w = rect.right - rect.left;
    *h = rect.bottom - rect.top;
    return(0);
}

WscrPixelsToPgDlg(PAGE *pg, int *x, int *y, int *w, int *h)
{
    HWND    hWnd;
    int     bx, by, bxy, objnb;
    RECT    rect;

    objnb = WscrFindPgNb(pg);
    if(objnb == 0) return(-1);
    hWnd = WSCR_HWNDS[objnb].sh_hwnd;
    if(GetWindowRect(hWnd, &rect) == 0) return(-1);
    bxy = GetDialogBaseUnits();
    bx = LOWORD(bxy);
    by = HIWORD(bxy);

    *x = *x - rect.left;
    *x = (*x * 4) / bx;
    *y = *y - rect.top;
    *y = (*y * 8) / by;

    *w = (*w * 4) / bx;
    *h = (*h * 8) / by;
    return(0);
}

WscrPixelsToDlg(int *w, int *h)
{
    int     bx, by, bxy;

    bxy = GetDialogBaseUnits();
    bx = LOWORD(bxy);
    by = HIWORD(bxy);

    *w = (*w * 4) / bx;
    *h = (*h * 8) / by;
    return(0);
}

/*
    Limite la hauteur de la page au champ OBJ
*/

WscrHorSchrinkPage(OBJ obj)
{
    PAGE    *pg = (PAGE *)obj.obj_ptr;
    int     x, y, w, h, objnb;
    HWND    hWnd;
    RECT    rect;

    objnb = WscrFindPgNb(pg);
    if(objnb == 0) return(-1);

    if(WscrGetFldPos(pg, obj.obj_fld, &x, &y, &w, &h) < 0) return(-1);

    hWnd = WSCR_HWNDS[objnb].sh_hwnd;
    if(GetWindowRect(hWnd, &rect) == 0) return(-1);
    MoveWindow(hWnd, rect.left, rect.top, rect.right, rect.top + y + h, FALSE);
    return(0);
}


WscrGetPgTitleHeight(PAGE *pg)
{
    HWND    hWnd;
    int     objnb;

    objnb = WscrFindPgNb(pg);
    if(objnb == 0) return(-1);

    hWnd = WSCR_HWNDS[objnb].sh_hwnd;
    return(WscrCalcTitleHeight(hWnd));
}







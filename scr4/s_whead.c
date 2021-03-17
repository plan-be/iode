#include "scr4w.h"
#include <commctrl.h>

HWND WscrCreateHeader(HWND hWndParent, int x, int y, int w, int h)
{
    HWND        hWnd;
    RECT        rcParent;
    HD_LAYOUT   hdl;
    WINDOWPOS   wp;
    int         i;

    InitCommonControls();

    hWnd = CreateWindowEx(0, WC_HEADER, (LPCTSTR) NULL,
	    WS_CHILD | WS_BORDER | WS_GROUP | HDS_BUTTONS | HDS_HORZ,
	    0, 0, 0, 0, hWndParent, (HMENU) WSCR_HEADERID, hInst,
	    (LPVOID) NULL);
    if(hWnd == 0) return (HWND) NULL;
    SendMessage(hWnd, WM_SETFONT, (WPARAM)W_SCRL_FONT, TRUE);

    WscrHeaderSetPos(hWnd, x, y, w, h);

    return(hWnd);
}

WscrHeaderSetPos(HWND hWnd, int x, int y, int w, int h)
{
    HWND        hWndParent = GetParent(hWnd);
    RECT        rcParent;
    HD_LAYOUT   hdl;
    WINDOWPOS   wp;

    GetClientRect(hWndParent, &rcParent);
    hdl.prc = &rcParent;
    hdl.pwpos = &wp;
    if(!SendMessage(hWnd, HDM_LAYOUT, 0, (LPARAM) &hdl)) return(0);

    if(x) wp.x = x;
    if(y) wp.y = y;
    if(w) wp.cx = w;
    if(h) wp.cy = h;

    SetWindowPos(hWnd, wp.hwndInsertAfter, wp.x, wp.y,
	wp.cx, wp.cy, wp.flags | SWP_SHOWWINDOW);

    return(0);
}

int WscrInsertHeaderItem(HWND hWndHeader, int iInsertAfter,
    int nWidth, LPSTR lpsz)
{
    HD_ITEM hdi;
    int     index;
    U_ch    buf[128];

    buf[0] = 0;
    if(lpsz) SCR_strlcpy(buf, lpsz, 125);
    SCR_OemToAnsi(buf, buf);
    hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH;
    hdi.pszText = buf;

    hdi.cxy = nWidth;
    hdi.cchTextMax = lstrlen(buf);
    hdi.fmt = HDF_LEFT | HDF_STRING;

    index = SendMessage(hWndHeader, HDM_INSERTITEM,
	(WPARAM) iInsertAfter, (LPARAM) &hdi);

    return(index);
}

WscrDeleteHeaderItem(HWND hWndHeader, int iItem)
{
    Header_DeleteItem(hWndHeader, iItem);
    return(0);
}

WscrSetHeaderText(HWND hWndHeader, int iItem, U_ch *txt, int just)
{
    HD_ITEM     hdi;
    U_ch    buf[128];

    buf[0] = 0;
    if(txt) SCR_strlcpy(buf, txt, 125);
    SCR_OemToAnsi(buf, buf);

    hdi.mask = HDI_TEXT | HDI_FORMAT;
    hdi.pszText = buf;

    hdi.cchTextMax = lstrlen(buf);
    hdi.fmt = HDF_STRING;
    switch(just) {
	case 1 : hdi.fmt |= HDF_CENTER; break;
	case 2 : hdi.fmt |= HDF_RIGHT; break;
	default: hdi.fmt |= HDF_LEFT; break;
	}

    Header_SetItem(hWndHeader, iItem, &hdi);
    return(0);
}

int WscrSetHeaderWH(HWND hWndHeader, int iItem, int wh, int type)
{
    HD_ITEM     hdi;

    hdi.mask = type;
    hdi.cxy  = wh;
    return(Header_SetItem(hWndHeader, iItem, &hdi));
}

int WscrSetHeaderHeight(HWND hWndHeader, int iItem, int height)
{
    return(WscrSetHeaderWH(hWndHeader, iItem, height, HDI_HEIGHT));
}

int WscrSetHeaderWidth(HWND hWndHeader, int iItem, int width)
{
    return(WscrSetHeaderWH(hWndHeader, iItem, width, HDI_WIDTH));
}

int WscrGetHeaderWH(HWND hWndHeader, int iItem, int type)
{
    HD_ITEM     hdi;

    hdi.mask = type;
    Header_GetItem(hWndHeader, iItem, &hdi);
    return(hdi.cxy);
}

int WscrGetHeaderHeight(HWND hWndHeader, int iItem)
{
    return(WscrGetHeaderWH(hWndHeader, iItem, HDI_HEIGHT));
}

int WscrGetHeaderWidth(HWND hWndHeader, int iItem)
{
    return(WscrGetHeaderWH(hWndHeader, iItem, HDI_WIDTH));
}

WscrGetHeaderNc(HWND hWndHeader)
{
    return(Header_GetItemCount(hWndHeader));
}


WscrHeaderNotify(NMHDR *nmh, int *colnb, int *size)
{
    HD_NOTIFY   *hdn = (HD_NOTIFY *)nmh;
    HWND        hWndHeader = nmh->hwndFrom;

    *colnb = hdn->iItem;
    if(nmh->code == HDN_ENDTRACK) {
	*size = WscrGetHeaderWidth(hWndHeader, *colnb);
	return(1);
	}
    if(nmh->code == HDN_ITEMCLICK) {
	return(2);
	}

    return(0);
}






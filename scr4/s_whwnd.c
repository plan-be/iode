#include "scr4w.h"

/*NH*/
WscrCenterWindow(HWND hWnd, int w_x, int w_y)
{
    HWND    hParent;
    RECT    rectp, rectd;
    int     wp, hp, wd, hd, hm, cyscr, cxscr, cx, cy;

    hParent = GetParent(hWnd);
    if(hParent == 0 && hWnd != hMainWnd) hParent = hMainWnd;

    cxscr = GetSystemMetrics(SM_CXSCREEN);
    cyscr = GetSystemMetrics(SM_CYSCREEN);
    if(hParent) GetWindowRect(hParent, &rectp);
    else {
        rectp.left = rectp.top = 0;
        rectp.right  = cxscr;
        rectp.bottom = cyscr;
    }

    cy = GetSystemMetrics(SM_CYCAPTION) +
         GetSystemMetrics(SM_CYBORDER);
    if(GetMenu(hWnd)) cy += GetSystemMetrics(SM_CYMENU);
    cx = GetSystemMetrics(SM_CXBORDER);

    WscrCalcDlgRect(hWnd, &rectd);
    wd  = rectd.right  - rectd.left;
    hd  = rectd.bottom - rectd.top;
    if(w_x > 0 && w_y > 0) {
        rectd.top = w_y - cy;
        rectd.left= w_x - cx;
    }
    else {
        wp  = rectp.right  - rectp.left;
        hp  = rectp.bottom - rectp.top;
        rectd.left = rectp.left + (wp - wd) / 2;
        hm = rectp.top +
             GetSystemMetrics(SM_CYCAPTION) +
             /*  GetSystemMetrics(SM_CYMENU) + /* JMP 14-04-00 */
             GetSystemMetrics(SM_CYBORDER);
        rectd.top = hm;
        if(rectd.top + hd > cyscr - 60) rectd.top = cyscr - 60 - hd;
        if(rectd.top < 0) rectd.top = 0;
        if(rectd.left + wd > cxscr - 10) rectd.left = cxscr - 10 - wd;
        if(rectd.left < 0) rectd.left = 0;
    }

    MoveWindow(hWnd, rectd.left, rectd.top, wd + 2, hd + 30, FALSE);
    SCR_wdispatch();
    return(0);
}

/*NH*/
WscrCalcDlgRect(HWND hWnd, RECT *rect)
{
    HWND    hWndNext;
    RECT    rectc;

    GetWindowRect(hWnd, rect);

    hWndNext = GetWindow(hWnd, GW_CHILD | GW_HWNDFIRST);
    while(hWndNext != NULL) {
        GetWindowRect(hWndNext, &rectc);
        if(rect->right  < rectc.right) rect->right = rectc.right;
        if(rect->bottom < rectc.bottom) rect->bottom = rectc.bottom;
        hWndNext = GetWindow(hWndNext, GW_HWNDNEXT);
    }
    return(0);
}


/*NH*/
WscrMoveDlgChildren(HWND hWnd, int x, int y)
{
    HWND    hWndNext;
    HDC     hDC;
    RECT    rectc;
    int     h, w;
    POINT   pt;

    GetDC(hWndNext);
    hWndNext = GetWindow(hWnd, GW_CHILD | GW_HWNDFIRST);
    while(hWndNext != NULL) {
        GetWindowRect(hWndNext, &rectc);
        pt.x = rectc.left;
        pt.y = rectc.top;
        ScreenToClient(hWnd, &pt);
        w = rectc.right - rectc.left;
        h = rectc.bottom - rectc.top;
        MoveWindow(hWndNext, pt.x + x, pt.y + y, w, h, FALSE);
        hWndNext = GetWindow(hWndNext, GW_HWNDNEXT);
    }
    return(0);
}

/*NH*/
WscrResizeDlgChildren(HWND hWnd)
{
    HWND    hWndNext;
    HDC     hDC;
    RECT    rectc, rect;
    int     h, w, len;
    POINT   pt;
    U_ch    buf[512]; // JMP 24/11/2017
    SIZE    sz;
    LONG    style;

    GetDC(hWndNext);
    hWndNext = GetWindow(hWnd, GW_CHILD | GW_HWNDFIRST);
    while(hWndNext != NULL) {
        GetWindowRect(hWndNext, &rectc);
        pt.x = rectc.left;
        pt.y = rectc.top;
        ScreenToClient(hWnd, &pt);
        w = rectc.right - rectc.left;
        h = rectc.bottom - rectc.top;
        GetClassName(hWndNext, buf, 120);
        if(SCR_cstrcmp(buf, "static") == 0) {
            len = GetWindowText(hWndNext, buf, 510); // JMP 24/11/2017
            if(len > 0) {
                hDC = GetDC(hWndNext);
                GetTextExtentPoint32(hDC, buf, len, &sz);
                w = sz.cx;
                h = sz.cy;
                // TextOut(hDC, 4, 4, "BLABLA", 6);        // Test
                // SetRect(&rect, 1, 1, w, h);             // Test
                // FillRect(hDC, &rect, W_BRUSH_YELLOW);   // Test
                ReleaseDC(hWndNext, hDC);           
                MoveWindow(hWndNext, pt.x, pt.y, w, h, FALSE);
            }
        }
        hWndNext = GetWindow(hWndNext, GW_HWNDNEXT);
    }
    return(0);
}

/* ============================================================
Ramène la taille d'une fenêtre au minimum pour contenir
le menu, le toolbar, le status bar et le titre.
============================================================ */

WscrShrinkWindow(HWND hWnd)
{
    RECT    crect, wrect, mrect;
    int     height, width, lastitem;
    SIZE    size;
    HMENU   hMenu;
    HWND    hWndChild;

    GetWindowRect(hWnd, &wrect);
    GetClientRect(hWnd, &crect);
    height = wrect.bottom - wrect.top - (crect.bottom - crect.top);
    hMenu = GetMenu(hWnd);
    if(hMenu) {
        lastitem = GetMenuItemCount(hMenu) - 1;
        GetMenuItemRect(hWnd, hMenu, lastitem, &mrect);
        width = mrect.right - wrect.left + 20;
    }
    else {
        width = 4 * GetSystemMetrics(SM_CXSIZE) +
                2 * GetSystemMetrics(SM_CXBORDER);
    }

    hWndChild = GetDlgItem(hWnd, WSCR_STATUSID);
    if(hWndChild) {
        GetWindowRect(hWndChild, &crect);
        height += crect.bottom - crect.top;
    }

    hWndChild = GetDlgItem(hWnd, WSCR_TOOLBARID);
    if(hWndChild) {
        GetWindowRect(hWndChild, &crect);
        height += crect.bottom - crect.top - 3;
    }

    MoveWindow(hWnd, wrect.left, wrect.top, width, height, TRUE);
    SCR_wdispatch();
}


/* ============================================================
Déplace une fenêtre par rapport à une autre.

&EN hWnd : handle de la fenêtre à déplacer
&EN hParent : handle de la fenêtre de référence ou 0 pour l'écran
&EN x : position en x
&EN2 x = 10000 : coin gauche de la fenêtre parent
&EN2 x = 10001 : centré par rapport à la fenêtre parent
&EN2 x = 10002 : coin droit de la fenêtre parent
&EN2 autre : décalage par rapport à la valeur actuelle
&EN y : position en y
&EN2 y = 10000 : bord supérieur de la fenêtre parent
&EN2 y = 10001 : centré par rapport à la fenêtre parent
&EN2 y = 10002 : bord inférieur de la fenêtre parent
&EN2 autre : décalage par rapport au bord supérieur du parent

&SA WscrMoveAnimateWindow(), WscrCalcMoveWindow()
============================================================ */

WscrMoveWindow(HWND hWnd, HWND hParent, int x, int y)
{
    RECT    rect;

    WscrCalcMoveWindow(hWnd, hParent, x, y, &rect);
    MoveWindow(hWnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
    return(0);
}

/* ============================================================
Déplace une fenêtre par rapport à une autre en plusieurs pas successifs à
partir de la position initiale.

&EN hWnd : handle de la fenêtre à déplacer
&EN hParent : handle de la fenêtre de référence ou 0 pour l'écran
&EN x : position en x
&EN2 x = 10000 : coin gauche de la fenêtre parent
&EN2 x = 10001 : centré par rapport à la fenêtre parent
&EN2 x = 10002 : coin droit de la fenêtre parent
&EN2 autre : décalage par rapport à la valeur actuelle
&EN y : position en y
&EN2 y = 10000 : bord supérieur de la fenêtre parent
&EN2 y = 10001 : centré par rapport à la fenêtre parent
&EN2 y = 10002 : bord inférieur de la fenêtre parent
&EN2 autre : décalage par rapport au bord supérieur du parent

&SA WscrMoveWindow(), WscrCalcMoveWindow()
============================================================ */

WscrMoveAnimateWindow(HWND hWnd, HWND hParent, int x, int y)
{
    RECT    rect, crect;
    int     i, dx, dy, wx, wy;

    GetWindowRect(hWnd, &crect);
    wx = crect.right - crect.left;
    wy = crect.bottom - crect.top;

    WscrCalcMoveWindow(hWnd, hParent, x, y, &rect);
    dx = (crect.left - rect.left) / 20;
    dy = (crect.top - rect.top) / 20;
    for(i = 0 ; i < 20 ; i++) {
        MoveWindow(hWnd, crect.left - i * dx, crect.top - i * dy , wx, wy, TRUE);
        Sleep(20);
    }
    MoveWindow(hWnd, rect.left, rect.top, wx, wy, TRUE);
    return(0);
}

/* ============================================================
Calcule le déplacement d'une fenêtre par rapport à une autre.

&EN hWnd : handle de la fenêtre à déplacer
&EN hParent : handle de la fenêtre de référence ou 0 pour l'écran
&EN x : position en x
&EN2 x = 10000 : coin gauche de la fenêtre parent
&EN2 x = 10001 : centré par rapport à la fenêtre parent
&EN2 x = 10002 : coin droit de la fenêtre parent
&EN2 autre : décalage par rapport à la valeur actuelle
&EN y : position en y
&EN2 y = 10000 : bord supérieur de la fenêtre parent
&EN2 y = 10001 : centré par rapport à la fenêtre parent
&EN2 y = 10002 : bord inférieur de la fenêtre parent
&EN2 autre : décalage par rapport au bord supérieur du parent
&EN rect : pointeur vers un RECT contenant la nouvelle position de la fenêtre.

&SA WscrMoveAnimateWindow(), WscrMoveWindow()
============================================================ */

WscrCalcMoveWindow(HWND hWnd, HWND hParent, int x, int y, RECT *rect)
{
    RECT    rectp, rectd;
    int     nx, ny, wx, wy, px, py, pwx, pwy;

    GetWindowRect(hWnd, &rectd);
    nx = rectd.left;
    ny = rectd.top;
    wx = rectd.right - nx;
    wy = rectd.bottom - ny;

    if(hParent) GetWindowRect(hParent, &rectp);
    else {
        rectp.right  = GetSystemMetrics(SM_CXSCREEN);
        rectp.bottom = GetSystemMetrics(SM_CYSCREEN);
        rectp.left = rectp.top = 0;
    }
    px = rectp.left;
    py = rectp.top;
    pwx = rectp.right - px;
    pwy = rectp.bottom - py;

    switch(x) {
    case 10000 :
        nx = px;
        break;
    case 10001 :
        nx = px + (pwx - wx) / 2;
        break;
    case 10002 :
        nx = px + pwx - wx;
        break;
    default    :
        nx += x;
        break;
    }

    switch(y) {
    case 10000 :
        ny = py;
        break;
    case 10001 :
        ny = py + (pwy - wy) / 2;
        break;
    case 10002 :
        ny = py + pwy - wy;
        break;
    default    :
        ny += y;
        break;
    }

    rect->left  = nx;
    rect->right = nx + wx;
    rect->top  = ny;
    rect->bottom = ny + wy;
    return(0);
}


WscrCalcTitleHeight(HWND hWnd)
{
    int     cy;

    cy = GetSystemMetrics(SM_CYCAPTION) +
         GetSystemMetrics(SM_CYBORDER);
    if(GetMenu(hWnd)) cy += GetSystemMetrics(SM_CYMENU);

    return(cy);
}





















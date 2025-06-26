#include "scr4w.h"

#if defined(SCRW32) || defined(DOSW32)
/* ===============================================================
Retourne le handle vers la fenêtre principale de l'application courante.

&RT hWndDOS ou hMainWnd, selon la version DOSW32 ou SCRW32
================================================================== */
HWND WscrGetMainWnd()
{
#ifndef IODEWIN
    #ifdef DOSW32
        return(hWndDOS);
    #else
        return(hMainWnd);
    #endif // DOSW32
#else
    return(0);
#endif // IODEWIN
}
#endif 


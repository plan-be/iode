#include <scr4w.h>  

/* ===============================================================
Retourne le handle vers la fenêtre principale de l'application courante.

&RT hWndDOS ou hMainWnd, selon la version DOSW32 ou SCRW32
================================================================== */
HWND WscrGetMainWnd()
{
    return(hWndDOS);
}

#include "scr4w.h"

WscrDisplayCmt(HWND hWnd)
{
    if(SCR_CMT.c_line < 0) return(0);

    WscrStatusSetText(hWnd, SCR_CMT_TXT);
    return(0);
}


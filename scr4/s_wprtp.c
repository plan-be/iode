#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"

/* ======================================================================
Edite les propriétés de l'imprimante PrinterName.

&EN PrinterName : pointeur vers le buffer qui contient le nom de
    l'imprimante.

&RT 0 en cas de succès, -1 en cas d'erreur.
&SA WscrSetPrinter*()
========================================================================= */

WscrPrinterProperties(char *PrinterName)
{
    HANDLE          hPrinter;

    if(OpenPrinter(PrinterName, &hPrinter, NULL) == 0) return(-1);

/*
#ifdef SCRW32
    PrinterProperties(GetLastActivePopup(hMainWnd), hPrinter);
#else
    PrinterProperties(hWndDOS, hPrinter);
#endif

*/

/* JMP 04-03-17 */

    PrinterProperties(GetLastActivePopup(WscrGetMainWnd()), hPrinter); /* JMP 04-03-17 */
    ClosePrinter(hPrinter);

    return(0);
}

#endif

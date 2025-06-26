#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"

/* ======================================================================
Fournit la liste de toutes les imprimantes définies sur le système
courant sous forme d'un tableau de noms (char **).

Seul le nom de chaque imprimante est retourné.

&RT tableau alloué de pointeurs terminé par un 0.

&EX
    ListPrinters()
    {
	char **tbl = WscrListPrinters();
	int     i;

	if(tbl == 0) {
	    printf("Erreur\n");
	    return(-1);
	    }

	for(i = 0 ; tbl[i] ; i++)
	    printf("Printer %d : %s\n", i, tbl[i]);

	SCR_free_tbl(tbl);
	return(0);
    }
&TX

========================================================================= */

char **WscrListPrinters()
{
    LPPRINTER_INFO_4    ppinfo4;
    DWORD               nb, ret, err;
    char                **tbl =  0;
    int                 i;

// NT : EnumPrinters(PRINTER_ENUM_NAME, NULL, 4, "", 0, &nb, &ret);
    EnumPrinters(PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL, NULL, 4, "", 0, &nb, &ret); /* JMP 22-04-02 */
    ppinfo4 = (LPPRINTER_INFO_4) SCR_malloc(nb);
    if(ppinfo4)
	EnumPrinters(PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL, NULL, 4, (LPBYTE)ppinfo4, nb, &nb, &ret); /* JMP 22-04-02 */
    else
	return(tbl);

    tbl = (char **)SCR_malloc((ret + 1) * sizeof(char *));
    for(i = 0 ; i < ret; i++)
	tbl[i] = SCR_stracpy(ppinfo4[i].pPrinterName);

    SCR_free(ppinfo4);

    return(tbl);
}

/* ======================================================================
Fournit le nom du printer par défaut.

&EN PrinterName : pointeur vers le buffer qui contiendra le nom de
    l'imprimante en sortie.

&RT numéro de l'imprimante par défaut (dans l'ordre des noms fournis par
    WscrListPrinters()), -1 en cas d'erreur.

&EX
    ListPrinters()
    {
	char **tbl = WscrListPrinters(), CurrentPrinter[80];
	int     i;

	if(tbl == 0) {
	    printf("Erreur\n");
	    return(-1);
	    }

	WscrGetDefaultPrinter(CurrentPrinter);

	for(i = 0 ; tbl[i] ; i++) {
	    if(strcmp(CurrentPrinter, tbl[i]) == 0)
		printf("(Default) ");
	    printf("Printer %d : %s\n", i, tbl[i]);
	    }

	SCR_free_tbl(tbl);
	return(0);
    }
&TX

========================================================================= */

WscrGetDefaultPrinter(char *PrinterName)
{
    LPPRINTER_INFO_5    ppinfo5; /* JMP 22-04-02 */
    DWORD               nb, ret;
    int                 i, lg = 80;

    if(WscrGetOSVersion(NULL) == 2) /* JMP 22-04-02 */
	return(GetDefaultPrinter(PrinterName, &lg));

    EnumPrinters(PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL, NULL, 5, "", 0, &nb, &ret);
    ppinfo5 = (LPPRINTER_INFO_5) SCR_malloc(nb * 2);
    if(ppinfo5)
	EnumPrinters(PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL, NULL, 5, (LPBYTE)ppinfo5, nb, &nb, &ret); /* JMP 22-04-02 */

    for(i = 0 ; i < ret; i++)
	if(ppinfo5[i].Attributes & PRINTER_ATTRIBUTE_DEFAULT) {
	    if(PrinterName) strcpy(PrinterName, ppinfo5[i].pPrinterName);
	    SCR_free(ppinfo5);
	    return(i);
	    }

    if(PrinterName) PrinterName[0] = 0;
    SCR_free(ppinfo5);
    return(-1);
}

/* ======================================================================
Fixe le nom du printer par défaut. Attention, peut ne pas fonctionner
sous NT 4.0.

&EN PrinterName : pointeur vers le buffer qui contient le nom de
    l'imprimante.

&RT 0 en cas de succès, -1 en cas d'erreur.
&SA WscrSetPrinter*(), WscrPrinterProperties()
========================================================================= */

WscrSetDefaultPrinter(char *PrinterName)
{
    HANDLE          hPrinter;
    PRINTER_INFO_4  *ppinfo4;
    DWORD           nb;
    int             rc;

    if(WscrGetOSVersion(NULL) == 2)
	return(SetDefaultPrinter(PrinterName));

    if(OpenPrinter(PrinterName, &hPrinter, NULL) == 0) return(-1);
    if(hPrinter == 0) return(-1);
    ppinfo4 = (PRINTER_INFO_4 *) SCR_malloc(12 * sizeof(PRINTER_INFO_4)); /* JMP 27-10-2003 */
    if(GetPrinter(hPrinter, 4, (LPBYTE)ppinfo4, 6 * sizeof(PRINTER_INFO_4), &nb) == TRUE) { /* TRUE == OK */
	if(nb < 6 * sizeof(PRINTER_INFO_4) && ppinfo4->pPrinterName) { /* JMP 05-08-98 */
	    ppinfo4->Attributes |= PRINTER_ATTRIBUTE_DEFAULT;
	    rc = SetPrinter(hPrinter, 4, (LPBYTE)ppinfo4, 0);
	    }
	}
    ClosePrinter(hPrinter);
    SCR_free(ppinfo4);
    return(!rc);
}


#endif











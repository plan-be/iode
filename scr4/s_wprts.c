#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"

/* ======================================================================
Fournit la liste de toutes les imprimantes d‚finies sur le systŠme
courant sous forme d'un tableau de noms (char **).

Seul le nom de chaque imprimante est retourn‚.

&RT tableau allou‚ de pointeurs termin‚ par un 0.

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
Fournit le nom du printer par d‚faut.

&EN PrinterName : pointeur vers le buffer qui contiendra le nom de
    l'imprimante en sortie.

&RT num‚ro de l'imprimante par d‚faut (dans l'ordre des noms fournis par
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
Fixe le nom du printer par d‚faut. Attention, peut ne pas fonctionner
sous NT 4.0.

&EN PrinterName : pointeur vers le buffer qui contient le nom de
    l'imprimante.

&RT 0 en cas de succŠs, -1 en cas d'erreur.
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

/*NH*/
WscrGetPrinterProps(char *PrinterName, int prop)
{
    HWND             hWnd =  WscrGetMainWnd(); /* JMP 25-09-11 */
    HGLOBAL          hGlobal = NULL;
    HANDLE           hPrinter = NULL;
    DWORD            dwNeeded = 0;
    PRINTER_INFO_2   *pi2 = NULL;
    PRINTER_DEFAULTS pd;
    BOOL              bFlag;
    LONG             lFlag;
    int              val = -1;

    /* Open printer handle */
    memset(&pd, 0, sizeof(pd));
    pd.DesiredAccess = PRINTER_ACCESS_USE;
    bFlag = OpenPrinter(PrinterName, &hPrinter, &pd);
    if(!bFlag || (hPrinter== NULL)) goto fin;

    GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
    if(dwNeeded == 0) goto fin;

    hGlobal = GlobalAlloc(GHND, dwNeeded);
    if(hGlobal == NULL) goto fin;
    pi2 = (PRINTER_INFO_2 *) GlobalLock(hGlobal);
    if(pi2 == NULL) goto fin;

    if(!GetPrinter(hPrinter, 2, (LPBYTE)pi2, dwNeeded, &dwNeeded)) goto fin;
    if(pi2->pDevMode == NULL) goto fin;
    if(!(pi2->pDevMode->dmFields & prop)) goto fin;
    switch(prop) {
	case DM_ORIENTATION : val = pi2->pDevMode->dmOrientation ; break;
	case DM_PAPERSIZE   : val = pi2->pDevMode->dmPaperSize   ; break;
	case DM_SCALE       : val = pi2->pDevMode->dmScale       ; break;
	case DM_COPIES      : val = pi2->pDevMode->dmCopies      ; break;
	case DM_PRINTQUALITY: val = pi2->pDevMode->dmPrintQuality; break;
	case DM_COLOR       : val = pi2->pDevMode->dmColor       ; break;
	case DM_DUPLEX      : val = pi2->pDevMode->dmDuplex      ; break;
	case DM_COLLATE     : val = pi2->pDevMode->dmCollate     ; break;
	case DM_DEFAULTSOURCE:val = pi2->pDevMode->dmDefaultSource; break;
	default             : break;
	}

fin:
    if(pi2 != NULL)      GlobalUnlock(hGlobal);
    if(hGlobal != NULL)  GlobalFree(hGlobal);
    if(hPrinter != NULL) ClosePrinter(hPrinter);
    return(val);
}

/* ======================================================================
Retourne l'orientation de l'impression pour l'imprimante PrinterName.
Cette fonction n'est pas disponible sur toutes les imprimantes.
Dans ce cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.

&RT
&EN -1 : erreur
&EN2 0 : Portrait
&EN2 1 : Landscape

&EX
    WscrGetPrinterOrientation("My Printer");
&TX

&SA WscrSetPrinter*(), WscrGetPrinter*()
========================================================================= */

WscrGetPrinterOrientation(char *PrinterName)
{
    int     rc;

    rc = WscrGetPrinterProps(PrinterName, DM_ORIENTATION);
    switch(rc) {
	case DMORIENT_PORTRAIT : return(0);
	case DMORIENT_LANDSCAPE : return(1);
	default : return(-1);
	}
}

/* ======================================================================
Indique si PrinterName imprime en recto-verso.

Cette fonction n'est pas disponible sur toutes les imprimantes. Dans ce
cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.

&RT
&EN -1 = erreur
&EN 0 = simplex
&EN 1 = recto-verso horizontal
&EN 2 = recto-verso vertical

&EX
    WscrGetPrinterDuplex("My Printer");
&TX

&RT 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*(), WscrGetPrinter*()
========================================================================= */

WscrGetPrinterDuplex(char *PrinterName)
{
    int     val;

    val = WscrGetPrinterProps(PrinterName, DM_DUPLEX);
    switch(val) {
	case DMDUP_HORIZONTAL : return(1);
	case DMDUP_VERTICAL   : return(2);
	case DMDUP_SIMPLEX    : return(0);
	default :               return(-1);
	}
}

/*NH*/
WscrSetPrinterProps(char *PrinterName, int prop, int val)
{
    HWND             hWnd = WscrGetMainWnd();
    HGLOBAL          hGlobal = NULL;
    HANDLE           hPrinter = NULL;
    DWORD            dwNeeded = 0;
    PRINTER_INFO_2   *pi2 = NULL;
    PRINTER_DEFAULTS pd;
    BOOL              bFlag;
    LONG             lFlag;
    int              rc = -1;


    /* Open printer handle (in Windows NT, you need full-access because you
      will eventually use SetPrinter) */
    memset(&pd, 0, sizeof(pd));
    pd.DesiredAccess = PRINTER_ACCESS_USE;
    bFlag = OpenPrinter(PrinterName, &hPrinter, &pd);
    if(!bFlag || (hPrinter== NULL)) goto fin;

    GetPrinter(hPrinter, 2, 0, 0, &dwNeeded);
    if(dwNeeded == 0) goto fin;

    hGlobal = GlobalAlloc(GHND, dwNeeded);
    if(hGlobal == NULL) goto fin;
    pi2 = (PRINTER_INFO_2 *) GlobalLock(hGlobal);
    if(pi2 == NULL) goto fin;

    if(!GetPrinter(hPrinter, 2, (LPBYTE)pi2, dwNeeded, &dwNeeded)) goto fin;
    if(pi2->pDevMode == NULL) goto fin;
    if(!(pi2->pDevMode->dmFields & prop)) goto fin;
    switch(prop) {
	case DM_ORIENTATION : pi2->pDevMode->dmOrientation  = val; break;
	case DM_PAPERSIZE   : pi2->pDevMode->dmPaperSize    = val; break;
	case DM_SCALE       : pi2->pDevMode->dmScale        = val; break;
	case DM_COPIES      : pi2->pDevMode->dmCopies       = val; break;
	case DM_PRINTQUALITY: pi2->pDevMode->dmPrintQuality = val; break;
	case DM_COLOR       : pi2->pDevMode->dmColor        = val; break;
	case DM_DUPLEX      : pi2->pDevMode->dmDuplex       = val; break;
	case DM_COLLATE     : pi2->pDevMode->dmCollate      = val; break;
	case DM_DEFAULTSOURCE: pi2->pDevMode->dmDefaultSource = val; break;
	default : goto fin;
	}

    /* Make sure the driver-dependent part of devmode is updated as
	 necessary. */
    lFlag = DocumentProperties(hWnd, hPrinter, PrinterName,
	      pi2->pDevMode, pi2->pDevMode, DM_IN_BUFFER | DM_OUT_BUFFER);
    if(lFlag != IDOK) goto fin;

    /* Update printer information. */
    SetPrinter(hPrinter, 2, (LPBYTE)pi2, 0);
    rc = 0;

fin:
    if(pi2 != NULL)      GlobalUnlock(hGlobal);
    if(hGlobal != NULL)  GlobalFree(hGlobal);
    if(hPrinter != NULL) ClosePrinter(hPrinter);
    return(rc);
}


/* ======================================================================
Fixe l'orientation de l'impression pour l'imprimante PrinterName.
Cette fonction n'est pas disponible sur toutes les imprimantes.
Dans ce cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.
&EN orient : nouvelle orientation
&EN2 0 : Portrait
&EN2 1 : Landscape

&EX
    WscrSetPrinterOrientation("My Printer", 1);
&TX

&RT 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*()
========================================================================= */

WscrSetPrinterOrientation(char *PrinterName, int orient)
{
    return(WscrSetPrinterProps(PrinterName, DM_ORIENTATION,
	    (orient == 0) ? DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE));
}

/* ======================================================================
D‚finit la taille du papier de l'imprimante PrinterName.
Cette fonction n'est pas disponible sur toutes les imprimantes.
Dans ce cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.
&EN psize : taille du papier
&EN2 0  = A4
&EN2 1  = A3
&EN2 2  = A5
&EN2 3  = B4
&EN2 4  = B5
&EN2 5  = QUARTO
&EN2 6  = ENVELOPPE DL
&EN2 7  = ENVELOPPE C3
&EN2 8  = ENVELOPPE C4
&EN2 9  = ENVELOPPE C5
&EN2 10 = ENVELOPPE C6
&EN2 11 = ENVELOPPE B4
&EN2 12 = ENVELOPPE B5
&EN2 13 = ENVELOPPE B6

&EX
    WscrSetPrinterPaperSize("My Printer", 0);
&TX

&RT 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*()
========================================================================= */

WscrSetPrinterPaperSize(char *PrinterName, int psize)
{
    int     val;

    switch(psize) {
	case 1  : val = DMPAPER_A3; break;
	case 2  : val = DMPAPER_A5; break;
	case 3  : val = DMPAPER_B4; break;
	case 4  : val = DMPAPER_B5; break;
	case 5  : val = DMPAPER_QUARTO; break;
	case 6  : val = DMPAPER_ENV_DL; break;
	case 7  : val = DMPAPER_ENV_C3; break;
	case 8  : val = DMPAPER_ENV_C4; break;
	case 9  : val = DMPAPER_ENV_C5; break;
	case 10 : val = DMPAPER_ENV_C6; break;
	case 11 : val = DMPAPER_ENV_B4; break;
	case 12 : val = DMPAPER_ENV_B5; break;
	case 13 : val = DMPAPER_ENV_B6; break;
	default : val = DMPAPER_A4; break;
	}

    return(WscrSetPrinterProps(PrinterName, DM_PAPERSIZE, val));
}

/* ======================================================================
D‚finit le facteur de compression de l'impression pour l'imprimante
PrinterName. En imprimant … 50% par exemple, on peut imprimer deux fois
plus dans les deux directions.

Cette fonction n'est pas disponible sur toutes les imprimantes. Dans ce
cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.
&EN scale : valeur de l'‚chelle

&EX
    WscrSetPrinterScale("My Printer", 72);
&TX

&RT 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*()
========================================================================= */

WscrSetPrinterScale(char *PrinterName, int scale)
{
    return(WscrSetPrinterProps(PrinterName, DM_SCALE, scale));
}

/* ======================================================================
D‚finit le nombre de copies … imprimer pour l'impression sur
l'imprimante PrinterName.

Cette fonction n'est pas disponible sur toutes les imprimantes. Dans ce
cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.
&EN copies : nombre de copies

&EX
    WscrSetPrinterCopies("My Printer", 5);
&TX

&RT 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*()
========================================================================= */

WscrSetPrinterCopies(char *PrinterName, int copies)
{
    return(WscrSetPrinterProps(PrinterName, DM_COPIES, copies));
}

/* ======================================================================
D‚finit la qualit‚ d'impression sur l'imprimante PrinterName.

Cette fonction n'est pas disponible sur toutes les imprimantes. Dans ce
cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.
&EN qualaty :
&EN2 0 : qualit‚ maximum
&EN2 1 : qualit‚ moyenne
&EN2 2 : qualit‚ faible
&EN2 3 : qualit‚ brouillon

&EX
    WscrSetPrinterQuality("My Printer", 0);
&TX

&RT 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*()
========================================================================= */

WscrSetPrinterQuality(char *PrinterName, int quality)
{
    int     val;

    switch(quality) {
	case 1  : val = DMRES_MEDIUM;   break;
	case 2  : val = DMRES_LOW;      break;
	case 3  : val = DMRES_DRAFT;    break;
	default : val = DMRES_HIGH;     break;
	}
    return(WscrSetPrinterProps(PrinterName, DM_PRINTQUALITY, val));
}

/* ======================================================================
Indique s'il faut imprimer en couleur sur l'imprimante PrinterName.

Cette fonction n'est pas disponible sur toutes les imprimantes. Dans ce
cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.
&EN color :
&EN2 0 = monochrome
&EN2 1 = couleur

&EX
    WscrSetPrinterColor("My Printer", 1);
&TX

&RT 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*()
========================================================================= */

WscrSetPrinterColor(char *PrinterName, int color)
{
    int     val;

    switch(color) {
	case 1  : val = DMCOLOR_COLOR;      break;
	default : val = DMCOLOR_MONOCHROME; break;
	}
    return(WscrSetPrinterProps(PrinterName, DM_COLOR, val));
}

/* ======================================================================
Indique s'il faut imprimer en recto-verso sur l'imprimante PrinterName.

Cette fonction n'est pas disponible sur toutes les imprimantes. Dans ce
cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.
&EN duplex :
&EN2 0 = simplex
&EN2 1 = recto-verso horizontal
&EN2 2 = recto-verso vertical

&EX
    WscrSetPrinterDuplex("My Printer", 1);
&TX

&RT 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*()
========================================================================= */

WscrSetPrinterDuplex(char *PrinterName, int duplex)
{
    int     val;

    switch(duplex) {
	case 1  : val = DMDUP_HORIZONTAL;   break;
	case 2  : val = DMDUP_VERTICAL;     break;
	default : val = DMDUP_SIMPLEX;      break;
	}
    return(WscrSetPrinterProps(PrinterName, DM_DUPLEX, val));
}

/* ======================================================================
Indique si l'imprimante PrinterName doit collationner les copies.

Cette fonction n'est pas disponible sur toutes les imprimantes. Dans ce
cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.
&EN collate :
&EN2 0 = ne pas collationner
&EN2 1 = collationner

&EX
    WscrSetPrinterCollate("My Printer", 1);
&TX

&RT 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*()
========================================================================= */

WscrSetPrinterCollate(char *PrinterName, int collate)
{
    int     val;

    switch(collate) {
	case 1  : val = DMCOLLATE_TRUE;   break;
	default : val = DMCOLLATE_FALSE;    break;
	}
    return(WscrSetPrinterProps(PrinterName, DM_COLLATE, val));
}

/* ======================================================================
Interroge le driver de PrinterName et retourne la source de papier par d‚faut.

&EN PrinterName : nom de l'imprimante.

&RT
&EN -1 = erreur
&EN >= 0 : num‚ro de la source de papier (d‚pend de l'imprimante)

&EX
    WscrGetPrinterSource("My Printer");
&TX

&RT >= 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*(), WscrGetPrinter*()
========================================================================= */

WscrGetPrinterSource(char *PrinterName)
{
    return(WscrGetPrinterProps(PrinterName, DM_DEFAULTSOURCE));
}


/* ======================================================================
Indique la source de papier … utiliser sur l'imprimante PrinterName.

Cette fonction n'est pas disponible sur toutes les imprimantes. Dans ce
cas, le code de retour est -1.

&EN PrinterName : nom de l'imprimante.
&EN source : entier indiquant la source (d‚pend de l'imprimante)

&EX
    WscrSetPrinterSource("My Printer", 2);
&TX

&RT 0 en cas de succŠs, -1 en cas d'erreur.
&SA WscrSetPrinter*()
========================================================================= */

WscrSetPrinterSource(char *PrinterName, int source)
{
    return(WscrSetPrinterProps(PrinterName, DM_DEFAULTSOURCE, source));
}

#endif











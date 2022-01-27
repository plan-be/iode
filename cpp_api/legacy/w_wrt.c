#pragma once

/* ********************************************** *
 *     copied/pasted from the /api directory      *
 * ********************************************** */

#include "../../api/iode.h"
//#include <s_a2m.h>

char    W_filename[K_MAX_FILE + 1] = "";                /* JMP 6-10-2015 */
int     W_type = 0, W_cont = 0, W_cancel = 0; /* JMP 06-02-98 */
int     W_gdiask = 0;  /* JMP 06-02-98 */
int     W_gdiorient = 0;  /* JMP 18-04-98 */
int     W_gdiduplex = 0;  /* JMP 18-04-98 */
char    W_gdiprinter[80] = ""; /* JMP 18-04-98 */
int     W_a2mapp = 0;  /* JMP 06-02-98 */
int     W_rtfhelp = 0; /* JMP 26-02-98 */
int     W_htmlhelp = 0; /* JMP 29-04-99 */
A2MFILE* W_af;


W_dest(char* filename, int type)
{
    char    ext[4];

    ext[0] = 0;
    W_close();
    if (type > 1) {
        switch (type) {
        case A2M_DESTA2M: strcpy(ext, "a2m"); break;
        case A2M_DESTFRM: strcpy(ext, "mif"); break;
        case A2M_DESTHTML: strcpy(ext, "htm"); break;
        case A2M_DESTRTF: strcpy(ext, "rtf"); break;
        case A2M_DESTCSV: strcpy(ext, "csv"); break;
#ifndef NOEMF
        case A2M_DESTGDIEMF: strcpy(ext, "emf"); break;
        case A2M_DESTGDIWMF: strcpy(ext, "wmf"); break;
#endif
        }

        SCR_strlcpy(W_filename, filename, K_MAX_FILE);  /* JMP 6-10-2015 */
        SCR_strip(W_filename);
        SCR_lower(W_filename);                  /* JMP 11-07-96 */

        if (!U_is_in('.', W_filename)) SCR_change_ext(W_filename, W_filename, ext); /* JMP 14-12-98 */
    }
    else W_filename[0] = 0;

    W_type = type;
    W_InitParms();
}

W_InitParms()
{
    B_A2mGetGnlParms(); /* JMP 19-02-98 */
    switch (W_type) {
    case W_GDI: B_A2mGetGdiParms(); break;
    case W_A2M: B_A2mGetGnlParms(); break;
    case W_RTF: B_A2mGetRtfParms();
        SCR_free(A2M_PGHEAD);   /* JMP 30-03-98 */
        SCR_free(A2M_PGFOOT);   /* JMP 30-03-98 */
        A2M_MAXPARLEVEL = 3;    /* JMP 04-09-11 */
        A2M_PGHEAD = A2M_PGFOOT = 0;  /* JMP 30-03-98 */
        break;
    case W_HTML: B_A2mGetHtmlParms();
        B_A2mGetGIFParms(); break; /* JMP 26-08-98 */
    case W_MIF: B_A2mGetMifParms(); break;
    case W_CSV: B_A2mGetCsvParms(); break;
    case W_DISP: B_A2mGetGdiParms();   /* JMP 19-02-98 */
        if (WscrGetOSVersion(0L) == 2) { /* JMP 17-11-98 */
            A2M_GWIDTH = 7.5;   /* JMP 17-11-98 */
            A2M_GHEIGHT = 5.0;  /* JMP 17-11-98 */
        }
        else {
            A2M_GWIDTH = 12.05;     /* JMP 19-02-98 */
            A2M_GHEIGHT = 8.35;    /* JMP 19-02-98 */
        }
        SCR_free(A2M_PGHEAD);
        SCR_free(A2M_PGFOOT);
        A2M_PGHEAD = A2M_PGFOOT = 0;
        break;
    default: break;
    }

    return(0);
}

/* ================================================================
Open the output printer file.

Returns 0 if OK, 1 if W_filename NULL, -1 if can not open file.
=================================================================== */

W_open()
{
    int     ask;  /* JMP 06-02-98 (parm for A2mMemBegin) */

    if (W_af) return(0);
    if (W_filename[0] == 0 && W_type > 1 && W_type < W_DISP) return(1);

    switch (W_type) {
    case A2M_DESTGDIPRT: ask = W_gdiask; break;
    case A2M_DESTA2M: ask = W_a2mapp; break;
    case A2M_DESTRTF: ask = W_rtfhelp; break;
    case A2M_DESTHTML: ask = W_htmlhelp; break;
    default:             ask = 0; break;
    }

    if (W_type == W_GDI) {
        W_SavePrinterSettings();
        W_SetPrinterSettings();
    }

    W_af = A2mMemBegin(W_type, W_filename, ask);  /* JMP 06-02-98 */
    if (W_af == NULL) {
        if (W_type == W_GDI) W_ResetPrinterSettings();
        W_cancel = 1;
        return(-1);
    }
    return(0);
}

W_close()
{
    if (W_af && W_cont) {
        A2mMemEnd(W_af);
        W_af = 0;
        W_cont = 0;
    }
    W_cancel = 0;
    if (W_type == W_GDI) W_ResetPrinterSettings();
}

W_flush()
{
    if (W_af == 0) return(-1);
    A2mMemFlush(W_af);
}

W_putc(int ch)
{
    return(W_printf("%c", ch));
}

int W_isempty(char* buf)
{
    int i;

    for (i = 0; buf[i]; i++) if (!U_is_in(buf[i], " \t\n\r")) return(0);

    return(1);
}

W_record(char* str)
{
    if (W_cancel) return(0);
    if (W_cont == 0 && W_isempty(str)) return(0);
    if (W_open()) return(-1);

    W_cont = 1;
    A2mMemRecord(W_af, str);
    return(0);
}

#ifndef UNIX
#ifdef SCRW32
extern  HWND    hMainWnd; /* JMP 01-02-98 */
#else
extern  HWND    hWndDOS;
#endif
#endif
long    W_emfnb = 0;

W_InitDisplay()
{
    char    tpath[K_MAX_FILE + 1], buf[K_MAX_FILE + 1];

    tpath[0] = 0; // !!!!!!!!!
    sprintf(buf, "%siode%ld", tpath, W_emfnb);
    W_dest(buf, W_DISP);
}

/*
W_EndDisplay(char* title, int x, int y, int w, int h)
{
    char    tpath[K_MAX_FILE + 1], buf[K_MAX_FILE + 1];
    int     wmf = (W_type == A2M_DESTGDIWMF) ? 1 : 0;

    W_close();
    if (W_type == A2M_DESTTCHRT) return;
    if (OdeGetTempPath(80, tpath) == 0) tpath[0] = 0;
    tpath[0] = 0; // !!!!!!!
    if (W_type == A2M_DESTGDIEMF) sprintf(buf, "%siode%ld.emf", tpath, W_emfnb);
    else                         sprintf(buf, "%siode%ld.wmf", tpath, W_emfnb);
    W_emfnb++;
#if !defined(UNIX) && !defined(NOEMF)
#ifdef SCRW32
    WscrEMFWMFCreateGnl(wmf, hMainWnd, buf, 0L, 1, x, y, w, h, title);  // JMP 20-03-98
#else
    //    WscrEMFPrint(hWndDOS, buf, 0L, 1);
    WscrEMFWMFCreateGnl(wmf, hWndDOS, buf, 0L, 1, x, y, w, h, title);      // JMP 20-03-98
#endif
#endif // UNIX
}
*/


char W_DftPrinterName[80]; /* JMP 31-07-98 */
int  W_DftPrinterOrient;
int  W_DftPrinterDuplex;

W_SavePrinterSettings()
{
    if (WscrGetDefaultPrinter(W_DftPrinterName) < 0) {
        W_DftPrinterName[0] = 0;
        return(0);
    }

    // W_DftPrinterOrient = WscrGetPrinterOrientation(W_DftPrinterName); // JMP 22/8/2016
    // W_DftPrinterDuplex = WscrGetPrinterDuplex(W_DftPrinterName); // JMP 22/8/2016
    return(0);
}

W_ResetPrinterSettings()
{
    char    buf[80];

    if (WscrGetDefaultPrinter(buf) < 0) return(-1);

    if (W_DftPrinterName[0] == 0) return(0);
    if (strcmp(W_DftPrinterName, buf))
        WscrSetDefaultPrinter(W_DftPrinterName);

    // WscrSetPrinterOrientation(buf, W_DftPrinterOrient); // JMP 22/8/2016
    // WscrSetPrinterDuplex(buf, W_DftPrinterDuplex); // JMP 22/8/2016
    return(0);
}

W_SetPrinterSettings()
{
    char    buf[80];

    if (W_gdiprinter[0]) WscrSetDefaultPrinter(W_gdiprinter);

    if (WscrGetDefaultPrinter(buf) < 0) return(-1);
    // WscrSetPrinterOrientation(buf, W_gdiorient); // JMP 22/8/2016
    // WscrSetPrinterDuplex(buf, W_gdiduplex); // JMP 22/8/2016
    return(0);
}

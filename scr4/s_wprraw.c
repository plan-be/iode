#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"

/*
WscrPrintRawFile(char *PrinterName, char *Filename)
{
    HANDLE     hPrinter;
    DOC_INFO_1 DocInfo;
    DWORD      dwJob;
    DWORD      dwBytesWritten = 0;
    LPBYTE     lpData = 0;
    DWORD      dwCount = -1;
    int        rc = -1;

    lpData = (LPBYTE)SCR_LoadRawFile(Filename, &dwCount);
    if(lpData == 0) return(-1);

    // Need a handle to the printer
    if(!OpenPrinter(PrinterName, &hPrinter, NULL)) goto fin;

    // Fill in the structure with info about this "document"
    DocInfo.pDocName = Filename;
    DocInfo.pOutputFile = NULL;
    DocInfo.pDatatype = "RAW";

    // Inform the spooler the document is beginning
    dwJob = StartDocPrinter(hPrinter, 1, (LPSTR)&DocInfo);
    if(dwJob == 0) goto fin;

    // Start a page
    if(!StartPagePrinter(hPrinter)) goto fin1;

    // Send the data to the printer
    if(WritePrinter(hPrinter, lpData, dwCount, &dwBytesWritten) &&
	dwBytesWritten == dwCount)
	    rc = 0;

    // End the page
    EndPagePrinter(hPrinter);

fin1 :
    // Inform the spooler that the document is ending
    EndDocPrinter(hPrinter);
fin :
    ClosePrinter(hPrinter);
    SCR_free((char *)lpData);
    return(rc);
}
*/

HANDLE     WSCR_HPRINTER;

/* =============================================================
Opens a printer to send binary data to it.

&EN PrinterName = printer name
&EN JobName     = name to be given to the job in the spool

&NO Restricted to Win32 (DOSW32 or SCRW32)

&RT 0 for success, -1 for failure
================================================================ */

WscrPrintRawInit(char *PrinterName, char *JobName)
{
    DOC_INFO_1  DocInfo;

    if(WSCR_HPRINTER) return(0);
    if(!OpenPrinter(PrinterName, &WSCR_HPRINTER, NULL)) return(-1);

    // Fill in the structure with info about this "document"
    SCR_OemToAnsi(JobName, JobName);  /* JMP 13-09-98 */
    DocInfo.pDocName = JobName;
    DocInfo.pOutputFile = NULL;
    DocInfo.pDatatype = "RAW";

    // Inform the spooler the document is beginning
    SCR_OemToAnsi(JobName, JobName);  /* JMP 13-09-98 */
    if(!StartDocPrinter(WSCR_HPRINTER, 1, (LPSTR)&DocInfo)) goto fin;

    // Start a page
    if(StartPagePrinter(WSCR_HPRINTER)) {
	SCR_AnsiToOem(JobName, JobName);  /* JMP 13-09-98 */
	return(0);
	}

    EndDocPrinter(WSCR_HPRINTER);
fin :
    ClosePrinter(WSCR_HPRINTER);
    WSCR_HPRINTER = 0;
    SCR_AnsiToOem(JobName, JobName);  /* JMP 13-09-98 */
    return(-1);
}

/* =============================================================
Sends binary data to the previously open printer.
A call to WscrPrintRawInit is required to initialise the process.

&EN data : buffer to be sent
&EN count : size of the data buffer

&NO Restricted to Win32 (DOSW32 or SCRW32)

&RT 0 for success, -1 for failure
&SA WscrPrintRawInit(), WscrPrintRawEnd(), WscrPrintRawString()
================================================================ */
WscrPrintRawData(U_ch *data, long count)
{
    DWORD   dwBytesWritten = -1;

    if(WSCR_HPRINTER == 0) return(-1);
    if(!WritePrinter(WSCR_HPRINTER, data, count, &dwBytesWritten))
	return(-1);
    if(dwBytesWritten != count) return(-1);
    return(0);
}

/* =============================================================
Sends a null terminated string to the previously open printer.
A call to WscrPrintRawInit is required to initialise the process.

&EN txt : texte to sent

&NO Restricted to Win32 (DOSW32 or SCRW32)

&RT 0 for success, -1 for failure
&SA WscrPrintRawInit(), WscrPrintRawEnd(), WscrPrintRawData()
================================================================ */
WscrPrintRawString(U_ch *txt)
{
    return(WscrPrintRawData(txt, strlen(txt)));
}

/* =============================================================
Ends the process of sending binary data to a printer.

&NO Restricted to Win32 (DOSW32 or SCRW32)

&RT 0 for success or no current print, -1 for failure
&SA WscrPrintRawInit()
================================================================ */
WscrPrintRawEnd()
{
    if(WSCR_HPRINTER == 0) return(0);

    EndPagePrinter(WSCR_HPRINTER);
    EndDocPrinter(WSCR_HPRINTER);
    ClosePrinter(WSCR_HPRINTER);
    WSCR_HPRINTER = 0;
    return(0);
}

/* =============================================================
Sends binary data directly to a printer. For example, this function
can send Postscript or PCL data to a printer, avoiding the
use of the ugly corresponding Windows drivers.

&EN PrinterName = printer name
&EN Filename    = file containing raw data

&NO Restricted to Win32 (DOSW32 or SCRW32)

&RT 0 for success, -1 for failure
&SA WscrPrintRawInit(), WscrPrintRawEnd()
================================================================ */
WscrPrintRawFile(char *filename)
{
    FILE    *fd;
    U_ch    buf[512];
    long    rbytes;
    int     rc;

    fd = fopen(filename, "rb");
    if(fd == 0) return(-1);
    while(1) {
	rbytes = fread((char *)buf, 1, 512, fd);
	if(rbytes > 0) rc = WscrPrintRawData(buf, rbytes);
	if(rc || rbytes <= 0) break;
	}

fin:
    fclose(fd);
    return(rc);
}

#endif



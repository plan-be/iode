// Fonctions annul‚es pour linux

#ifndef UNIX
#include <windows.h>
#endif

#ifdef UNIX
int WscrShellExec(char *cmd)
{
    system(cmd);
}

WscrGetOSVersion(char *version)
{
    return(-1);
}

WscrGetDefaultPrinter(char *pr)
{
    return(-1);
}

WscrSetDefaultPrinter(char *pr)
{
    return(-1);
}

WscrGetPrinterOrientation(char *pr)
{
    return(-1);
}

WscrSetPrinterOrientation(char *pr, int orient)
{
    return(-1);
}

WscrGetPrinterDuplex(char *pr)
{
    return(-1);
}

WscrSetPrinterDuplex(char *pr, int duplex)
{
    return(-1);
}


A2mGdiPrinterInit()     {return(-1); }
A2mGdiEMFInit()         {return(-1); }
A2mGdiWMFInit()         {return(-1); }
A2mGdiPrintObj()        {return(-1); }
A2mGdiPrinterEnd()      {return(-1); }
A2mGdiEMFEnd()          {return(-1); }
A2mGdiWMFEnd()          {return(-1); }


#endif

OdeGetTempPath(int lg, char *tpath)
{
#ifdef UNIX
    strcpy(tpath, ".");
    return(0);
#else
    return(GetTempPath(lg, tpath));
#endif
}










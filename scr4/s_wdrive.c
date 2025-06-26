#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"
// #include <windows.h>

/* ========================================================================
Windows Only. Fournit le nom d'un volume local (disque non monté).

&EN drive = lettre pour accéder au drive : ("C:", "A:", ...)
&EN volname = nom du drive retourné en cas de succès, sinon '' (buffer de 128 bytes requis)
&EN volsn   = long numéro de série du disque par la fonction en cas de succès

&RT : 0 en cas de succès, -1 sinon
================================================================== */

int WscrGetDriveInfos(U_ch *drive, U_ch *volname, unsigned long * volsn)
{
    int             rc;
    char            FileSystemNameBuffer[128],
		    VolumeNameBuffer[128],
		    drive2[5];
    unsigned long   nVolumeSerialNumber,
		    MaximumComponentLength,
		    FileSystemFlags;

    // remplacer drive par la forme canonique {lettre}:
    sprintf(drive2, "%c:\\", drive[0]);

    rc = GetVolumeInformation(
			drive2,
			VolumeNameBuffer, 127,
			&nVolumeSerialNumber,
			&MaximumComponentLength,
			&FileSystemFlags,
			FileSystemNameBuffer, 127
			);
    if(volname) volname[0] = 0;
    if(volsn)   *volsn  = 0;
    if(rc == 0) return(-1);

    if(volname) strcpy(volname, VolumeNameBuffer);
    if(volsn)   *volsn = nVolumeSerialNumber;
    return(0);
}

/*
BOOL WINAPI GetVolumeInformation(
  __in_opt   LPCTSTR lpRootPathName,
  __out      LPTSTR lpVolumeNameBuffer,
  __in       DWORD nVolumeNameSize,
  __out_opt  LPDWORD lpVolumeSerialNumber,
  __out_opt  LPDWORD lpMaximumComponentLength,
  __out_opt  LPDWORD lpFileSystemFlags,
  __out      LPTSTR lpFileSystemNameBuffer,
  __in       DWORD nFileSystemNameSize
);

*/

#endif

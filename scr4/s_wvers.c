#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"


/* ========================================================================
Retourne la version de l'OS sous forme d'un entier et optionnellement
dans un format caractère.

&EN version = contient après l'appel un texte reprenant l'OS, la version
    et le "build number". Version peut être nulle auquel cas seule la
    valeur de retour de la fonction indique l'OS. Si version est non
    nulle, sa taille doit être assez grande (256 caractères). La valeur
    de retour est par exemple :

&EN2 "Win3x 3.xx.xxxx"
&EN2 "Win95 4.0.950 B"
&EN2 "WinNT 4.0.1111 A"

&RT entier indiquant l'OS :
&EN -1 = erreur
&EN 0 = Windows 3.1x (Win32s)
&EN 1 = Windows 95, 98 ou me
&EN 2 = Windows NT, XP ou .Net Server
&EN 3 = Windows inconnu

================================================================== */

WscrGetOSVersion(char *version)
{
    OSVERSIONINFO   osv;
    int             rc;
    char            *vers[4] = {"Win3x", "Win95", "WinNT", "?????"};

    osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if(!GetVersionEx(&osv)) return(-1);
    switch(osv.dwPlatformId) {
	case VER_PLATFORM_WIN32s        : rc = 0 ; break;
	case VER_PLATFORM_WIN32_WINDOWS : rc = 1 ; break;
	case VER_PLATFORM_WIN32_NT      : rc = 2 ; break;
	default                         : rc = 3 ; break;
	}
    if(version == 0) return(rc);
    sprintf(version, "%s %ld.%ld.%ld%s",
		      vers[rc],
		      osv.dwMajorVersion,
		      osv.dwMinorVersion,
		      osv.dwBuildNumber & 0xFFFF,
		      osv.szCSDVersion);
    return(rc);
}

/* ========================================================================
Retourne la version de l'OS sous forme d'un entier et optionnellement
dans un format caractère. Remplace WscrGetOSVersion().

&EN version = contient après l'appel un texte reprenant l'OS, la version
    et le "build number". Version peut être nulle auquel cas seule la
    valeur de retour de la fonction indique l'OS. Si version est non
    nulle, sa taille doit être assez grande (256 caractères). La valeur
    de retour est par exemple :

&EN2 "Win3x 3.xx.xxxx"
&EN2 "Win95 4.0.950 B"
&EN2 "WinNT 4.0.1111 A"
&EN2 "WinNT 6.0.6000" (Vista)

&RT entier indiquant l'OS :
&EN -1 = erreur
&EN 1 = Windows 95
&EN 2 = Windows 98
&EN 3 = Windows me
&EN 4 = Windows NT 4.0
&EN 5 = Windows 2000
&EN 6 = Windows XP
&EN 7 = Windows 2003 Server
&EN 8 = Windows Vista ou Windows 2008 Server
&EN 9 = Windows 7
&EN 99 = Windows inconnu

================================================================== */

WscrGetOSVersionEx(char *version)
{
    OSVERSIONINFO   osv;
    int             rc, Platform, MinorV, MajorV;

    osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if(!GetVersionEx(&osv)) return(-1);

    // Version en texte
    if(version)
	sprintf(version, "Windows %ld.%ld.%ld %s",
		      osv.dwMajorVersion,
		      osv.dwMinorVersion,
		      osv.dwBuildNumber & 0xFFFF,
		      osv.szCSDVersion);

    // Version en numérique
    Platform = osv.dwPlatformId;
    MinorV = osv.dwMinorVersion;
    MajorV = osv.dwMajorVersion;

    if(MajorV == 6 && MinorV == 1 && Platform == 2) rc = 9;  /* JMP 18-06-10 */
    if(MajorV == 6 && MinorV == 0 && Platform == 2) rc = 8;
    if(MajorV == 5 && MinorV == 2 && Platform == 2) rc = 7;
    if(MajorV == 5 && MinorV == 1 && Platform == 2) rc = 6;
    if(MajorV == 5 && MinorV == 0 && Platform == 2) rc = 5;
    if(MajorV == 4 && MinorV == 0 && Platform == 2) rc = 4;
    if(MajorV == 4 && MinorV == 90 && Platform == 1) rc = 3;
    if(MajorV == 4 && MinorV == 10 && Platform == 1) rc = 2;
    if(MajorV == 4 && MinorV == 0 && Platform == 1) rc = 1;

    return(rc);
}

#endif // defined(SCRW32) || defined(DOSW32)

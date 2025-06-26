#include "s_strs.h"
#include "s_dir.h"

#ifdef DOS

#include <dos.h>
#if defined(VC) || defined(WATCOM)  /* BP_MODIF 08-10-98 */
#include <direct.h>
#else
#include <dir.h>
#endif

/*NH*/
#if !defined(WATCOM) && !defined(WINDOWS) && !defined(DOSWIN)

/*NH*/
void far SCR_harderror() {
    _hardresume(_HARDERR_IGNORE);
}
#endif /* WATCOM ... */
#endif /* DOS */

/* ====================================================================
Retourne dans dir le directory courant sur drive. drive vaut 0 pour
le disque courant, 1, pour A:, ...

&RT En cas d'erreur, retourne NULL et fixe dir[0] à 0.
    Sinon retourne un pointeur vers dir.

&NO En Unix, drive n'est pas utilisé.
&SA SCR_fullpath()
======================================================================= */

char *SCR_getcwd(int drive, char* dir)
{
    unsigned int     c_drive = drive;


#if defined(WINDOWS) || defined(DOSWIN)
    if(_getdcwd(drive, dir, 512) == 0) return((char *)0); /* JMP 19-04-10 */
//    GetCurrentDirectory(256, dir);
    return(dir);
#else

#ifdef DOS
    unsigned int     ndrive;
#if !defined(WATCOM) && !defined(WINDOWS) && !defined(DOSWIN)
    _harderr(SCR_harderror);
#endif
    if(drive) c_drive = SCR_dosgetdrive();
    if(drive && drive != c_drive) {
	SCR_dossetdrive(drive);              /* JMP 22-01-98 */
	ndrive = SCR_dosgetdrive();          /* JMP 22-01-98 */
	if(ndrive != drive) {
	    dir[0] = 0;
	    return((char *)0);
	    }
	}
    SCR_dosgetcwd(dir, SCR_FILE_LG);         /* JMP 22-01-98 */
#if !defined(DOSW32) && !defined(SCRW32)
    SCR_lower(dir);
#endif
    if(drive && drive != c_drive) SCR_dossetdrive(c_drive); /* JMP 22-01-98 */
    return(dir);
#else
    getcwd(dir, SCR_FILE_LG);
    return(dir);
#endif
#endif
}

#ifdef DOS

/*NH ==================================================================
Retourne le drive courant (DOS).
 ================================================================== */

int SCR_dosgetdrive()
{
    int     ndrive;

#ifdef VC
    ndrive = _getdrive();
#else
/*    _dos_getdrive(&ndrive); */
    ndrive = _getdrive();
#endif
    return(ndrive);
}

/*NH ==================================================================
Change le drive courant (DOS).
 ================================================================== */

int SCR_dossetdrive(int ndrive)
{
#ifdef VC
    int     cdrive;

    _chdrive(ndrive);
    cdrive = _getdrive();
    if(cdrive != ndrive) return(-1);
    return(0);
#else
    int     tot, cdrive;
    _chdrive(ndrive);
/*    _dos_setdrive(ndrive, &tot); */
    cdrive = _getdrive();
/*    _dos_getdrive(&cdrive); */
    if(cdrive != ndrive) return(-1);
    return(0);
#endif
}

/*NH ==================================================================
Retourne le drive courant (DOS).
 ================================================================== */

int SCR_dosgetcwd(char* dir, int lg)
{
#ifdef VC
    _getcwd(dir, lg);
#else
    getcwd(dir, lg);
#endif
    return(0);
}

#endif





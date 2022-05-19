#include <time.h>

#ifdef UNIX
    // #include <sys/time.h>    // JMP 6/4/2022
    #include <utime.h>          // JMP 6/4/2022
#endif

#include "scr4w.h"

/* ===============================================================
Retourne le process id
================================================================== */

WscrGetPid()
{
#ifdef DOS
#if defined(SCRW32) || defined(DOSW32)
    return(_getpid());
#else
    return(-1);
#endif
#else
    return(getpid());
#endif
}

/* ===============================================================
Fournit un nom de fichier inexistant dans le r�pertoire donn� dans dirname.
Le nom r�sultat est pr�fix� par scr4.

Limite : seulement impl�ment� en Windows

&EN dirname : nom du directory
&EN filename : nom fichier r�sultat

RT 0 en cas de succ�s, -1 en cas de probl�me
================================================================== */

WscrTempFilenameInDir(char *dirname, char *filename)
{
#if defined(SCRW32) || defined(DOSW32)
    char    dirbuf[256];
    filename[0] = 0;
    if(GetTempFileName(dirname, "scr4", 0, filename) == 0) return(-1);
    return(0);
#else
    int     i;

    strcpy(filename, "scXXXXXX");
#ifdef DOS
    if(mktemp(filename) == 0) return(-1); /* JMP 26-02-2004 */
    unlink(filename);
    return(0);
#else
    i = mkstemp(filename);
    if(i < 0) return(-1);
    close(i);
    unlink(filename);
    return(0);
#endif
#endif
}

/* ===============================================================
Fournit un nom de fichier inexistant dans le r�pertoire temp de Windows.
Le nom est pr�fix� par scr4.

Limite : seulement impl�ment� en Windows

&EN filename : nom fichier

RT 0 en cas de succ�s, -1 en cas de probl�me
================================================================== */

WscrTempFilename(char *filename)
{
#if defined(SCRW32) || defined(DOSW32)
    char    dirbuf[256];
    filename[0] = 0;
    if(GetTempPath(sizeof(dirbuf), dirbuf) == 0) return(-1);
    if(GetTempFileName(dirbuf, "scr4", 0, filename) == 0) return(-1);
    return(0);
#else
    int     i;

    strcpy(filename, "scXXXXXX");
#ifdef DOS
    if(mktemp(filename) == 0) return(-1); /* JMP 26-02-2004 */
    unlink(filename);
    return(0);
#else
    i = mkstemp(filename);
    if(i < 0) return(-1);
    close(i);
    unlink(filename);
    return(0);
#endif
#endif
}

/* ===============================================================
Fournit le nom du r�pertoire temp

&EN le nom du r�pertoire temporaire de Windows
&EN "c:" si impossible ou DOS
&EN "." si Unix
&EN filename : nom du r�pertoire

RT 0 en cas de succ�s, -1 en cas de probl�me
================================================================== */

WscrTempPath(char *dirbuf)
{
#if defined(SCRW32) || defined(DOSW32)
    if(GetTempPath(127, dirbuf) == 0) {
	strcpy(dirbuf, "c:");
	return(-1);
	}
    return(0);
#else
#ifdef DOS
    strcpy(dirbuf, "c:");
#else
    strcpy(dirbuf, ".");
#endif
    return(0);
#endif
}


/* ===============================================================
Modifie les dates et heures d'acc�s et de modification du fichier filename.

Les acc�s suffisants doivent �tre donn�s � l'utilisateur du programme.

&NO Sous Linux, il est impossible de fixer l'heure et la date d'acc�s d'un fichier
    d'une partition  FAT32 mont�e.

&EN filename : nom fichier
&EN actime : nombre de secondes depuis le 1/1/1970  (format de stat)
&EN modtime : nombre de secondes depuis le 1/1/1970 (format de stat)

RT 0 en cas de succ�s, -1 en cas de probl�me
================================================================== */


WscrSetFileTime(char *filename, long actime, long modtime)
{
    int                 rc;

#if defined(SCRW32) || defined(DOSW32)
    rc = WscrSetFileTimeWin(filename, 0, actime, modtime);
#else // Unix...
    struct         tb;

    tb.actime  = actime;
    tb.modtime = modtime;
    rc = utime(filename, &tb); /* JMP 26-02-2004  � la place de _utime pour les 16 bits ???? */
#endif
    return(rc);
}

#if defined(SCRW32) || defined(DOSW32)

/*NH*/
WscrSetFileTimeWin(char *filename, long cretime, long acttime, long modtime)
{
	FILETIME        ftc, fta, ftm, *pftc = NULL, *pfta = NULL, *pftm = NULL;
	HANDLE          hFile;
	int             rc;

	if(cretime) {
		pftc = &ftc;
		StatTimeToFileTime(&ftc, SCR_cvt_date(cretime), SCR_cvt_time(cretime));
	}
	if(acttime) {
		pfta = &fta;
		StatTimeToFileTime(&fta, SCR_cvt_date(acttime), SCR_cvt_time(acttime));
	}
	if(modtime) {
		pftm = &ftm;
		StatTimeToFileTime(&ftm, SCR_cvt_date(modtime), SCR_cvt_time(modtime));
	}

	hFile = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
	if(hFile == 0) return(-1);
	rc = SetFileTime(hFile, pftc, pfta, pftm);
	CloseHandle(hFile);
	if(rc == 0) return(-1);
	return(0);
}
#endif

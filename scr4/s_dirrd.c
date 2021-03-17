#include "s_dir.h"
#include "s_strs.h"

/* VERSION LIMITEE A WINDOWS et plus WATCOM... pour le portage en Visual C */
#if defined(DOS) 
    #ifndef VC
        #include <dirent.h>
    #endif
#endif /* DOS */

#ifdef UNIX
    #ifdef XENIX
        #include <sys/ndir.h>
    #else
        #ifdef INTAU
            #include <sys/dirent.h>
        #else
            #include <dirent.h>
        #endif
    #endif
#endif

#if defined(DOSW32) || defined(SCRW32)
#include <windows.h>

static WIN32_FIND_DATA  FindFileData;
SCRDIR *SCR_opendir_std(char *dirname);
int SCR_closedir_std(SCRDIR *sdir);
SCRSTAT *SCR_readdir_ex_std(SCRDIR *sdir, int getstat, int crc);

/*NH*/
SCRDIR *SCR_opendir_win(char *dirname)
{
    SCRDIR                  *sdir;
    int                     lg, os;
    char                    buf[SCR_STAT_NAME_LG + 1];
    HANDLE                  hFind;
    char                    buf2[SCR_STAT_NAME_LG + 5];

    os = WscrGetOSVersionEx(NULL);                  /* JMP 22-09-11 */
    SCR_dir_std(buf, dirname);
    sprintf(buf2, "%s\\*.*", buf);
    sdir = (SCRDIR *)SCR_malloc(sizeof(SCRDIR));
    if(sdir == 0) return((SCRDIR *)0);
    sdir->d_os = os;
    hFind = FindFirstFileEx(buf2, FindExInfoStandard, &FindFileData,
		 FindExSearchNameMatch, NULL, 0);
    if(hFind == INVALID_HANDLE_VALUE) {
	SCR_free(sdir);
	return((SCRDIR *)0);
    }
    SCR_strlcpy(sdir->d_name, buf, SCR_STAT_NAME_LG); /* JMP 03-11-06 */
    sdir->d_dirp = 0;
    sdir->d_handle = (char *)hFind;
    sdir->d_first = 1;
    return(sdir);
}

/*NH*/
int SCR_closedir_win(SCRDIR *sdir)
{
    if(sdir == 0) return(0);
    FindClose((HANDLE)sdir->d_handle);
    SCR_free(sdir);

    return(0);
}

static long ToStatDate(long date, long tim)
{
    long        days, secs;

    days = DT_diff_dates(date, 19700101);
    secs = DT_time_num(tim);
    return(days * 24 * 3600 + secs);
}

/*NH*/
static ToSystemDateTime(FILETIME *ft, long *dtl, long *til, long  *xti)
{
    SYSTEMTIME  stUTC, stLocal;
    FILETIME    ft2;
    long        dtu, tiu, xtl, t, diff, xt2;

    // UTC
    FileTimeToSystemTime(ft, &stLocal);
    dtu = stLocal.wDay + 100 * stLocal.wMonth + 10000L * stLocal.wYear;
    tiu = stLocal.wHour * 10000 + stLocal.wMinute * 100 + stLocal.wSecond;
    *xti = ToStatDate(dtu, tiu);
    tiu= SCR_cvt_time(*xti);
    dtu= SCR_cvt_date(*xti);
    xt2 = ToStatDate(dtu, tiu);
    diff = xt2 - *xti;

    // Local
    FileTimeToLocalFileTime(ft, &ft2);
    FileTimeToSystemTime(&ft2, &stLocal);
    *dtl = stLocal.wDay + 100 * stLocal.wMonth + 10000L * stLocal.wYear;
    *til = stLocal.wHour * 10000 + stLocal.wMinute * 100 + stLocal.wSecond;
    *xti = ToStatDate(*dtl, *til);
    *xti -= diff;
    return(0);
}

static ToSystemDateTimeU(FILETIME *ft, long *dt, long  *ti)
{
    SYSTEMTIME  stUTC, stLocal;
    FILETIME    ft2;

    FileTimeToSystemTime(ft, &stUTC);

    *dt = stUTC.wDay + 100 * stUTC.wMonth + 10000L * stUTC.wYear;
    *ti = stUTC.wHour * 10000 + stUTC.wMinute * 100 + stUTC.wSecond;
    return(0);
}


FileTimeToStatTime(FILETIME *ft)
{
    SYSTEMTIME  stUTC;
    long        dt, ti;

    FileTimeToSystemTime(ft, &stUTC);
    dt = stUTC.wDay + 100 * stUTC.wMonth + 10000L * stUTC.wYear;
    ti = stUTC.wHour * 10000 + stUTC.wMinute * 100 + stUTC.wSecond;
    return(ToStatDate(dt, ti));
}

StatTimeToFileTime(FILETIME *ft, unsigned long ss_mdate, unsigned long ss_mtime)
{
	FILETIME        ft1;
	SYSTEMTIME      st;

	st.wYear = ss_mdate / 10000;
	st.wMonth = (ss_mdate / 100) % 100;
	st.wDayOfWeek = -1;
	st.wDay = ss_mdate % 100;
	st.wHour = ss_mtime / 10000;
	st.wMinute = (ss_mtime / 100) % 100;
	st.wSecond = ss_mtime % 100;
	st.wMilliseconds = 0;

	SystemTimeToFileTime(&st, &ft1);     // converts to file time format
	LocalFileTimeToFileTime(&ft1,ft);
	return(0);
}

/*NH*/
static DirCalcFullName(SCRDIR *sdir, SCRSTAT *ss, char *fname)
{
    char    tmp[2 * SCR_STAT_NAME_LG + 1]; /* JMP 03-11-06 */

    SCR_strlcpy(fname, sdir->d_name, SCR_STAT_NAME_LG); /* JMP 03-11-06 */
    if(strcmp(ss->ss_name, "..") == 0) SCR_split_dir(fname, tmp);
    else if(strcmp(ss->ss_name, ".")) {
	if(fname[0] && !U_is_in(fname[strlen(fname) - 1], "\\/")) /* JMP 22-09-99 */
	    strcat(fname, DIR_SEPAR);
	strcat(fname, ss->ss_name);
	}
    return(0);    
}

/*NH*/
SCRSTAT *SCR_readdir_ex_win(SCRDIR *sdir, int getstat, int crc)
{
    SCRSTAT         *ss = 0, ss2;
    char            fname[SCR_STAT_NAME_LG + 1], tmp[SCR_STAT_NAME_LG + 1];
    int             mode;
    static          long diffstat = -1;
    long            dt, ti;

    if(sdir == 0) return(ss);
    if(sdir->d_first == 0) {
	if(FindNextFile((HANDLE)sdir->d_handle, &FindFileData) == 0) return(ss);
	}
    sdir->d_first = 0;

    ss = &(sdir->d_ss);
    memset(ss, 0, sizeof(SCRSTAT));
    SCR_strlcpy(ss->ss_name, FindFileData.cFileName, SCR_STAT_NAME_LG);
    ss->ss_size  = FindFileData.nFileSizeLow;
    ToSystemDateTime(&(FindFileData.ftCreationTime),   &(ss->ss_cdate), &(ss->ss_ctime), &(ss->ssx_ctime));
    ToSystemDateTime(&(FindFileData.ftLastAccessTime), &(ss->ss_adate), &(ss->ss_atime), &(ss->ssx_atime));
    ToSystemDateTime(&(FindFileData.ftLastWriteTime),  &(ss->ss_mdate), &(ss->ss_mtime), &(ss->ssx_mtime));

    /* MODE */
    mode = 6;
    if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) mode |= 1;
    if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) mode &= ~2;
    if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) mode &= ~4;
    ss->ss_mode = mode + (mode << 3) + (mode << 6);

    if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ss->ss_mode |= SCR_FTYPE_DIR;
    if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ss->ss_mode |= SCR_FTYPE_SYS;
    if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ss->ss_mode |= SCR_FTYPE_ARCH;

    if(crc == 0) return(ss);
    DirCalcFullName(sdir, ss, fname);
    ss->ssx_crc32 = ScrFileCrc32(fname); /* JMP 25-10-02 */
    return(ss);
}
#endif

/* ====================================================================
Indique si dirname correspond … un nom de directory.

&RT 1 si c'est le cas, 0 sinon

&SA SCR_dir(), SCR_opendir(), SCR_closedir(), SCR_readdir()
======================================================================= */

SCR_is_dir(dirname)
char    *dirname;
{
    SCRDIR  *sd;

    sd = SCR_opendir(dirname);
    if(sd) {
	SCR_closedir(sd);
	return(1);
	}
    return(0);
}

#if !defined(DOSW32) && !defined(SCRW32)
/*NH*/
SCRDIR *SCR_opendir_std(char *dirname)
{
    DIR     *dirp;
    SCRDIR  *sdir;
    int     lg;
    char    buf[2 * SCR_STAT_NAME_LG + 1]; /* JMP 03-11-06 */

    SCR_dir_std(buf, dirname);
    dirp = opendir(buf);
    if(dirp == 0) return((SCRDIR *)0);
    sdir = (SCRDIR *)SCR_malloc(sizeof(SCRDIR));
    if(sdir == 0) {
	closedir(dirp);
	return((SCRDIR *)0);
	}
    SCR_strlcpy(sdir->d_name, buf, SCR_STAT_NAME_LG); /* JMP 03-11-06 */
    sdir->d_dirp = (char *)dirp;
#if defined(DOS) && !defined(DOSW32) && !defined(SCRW32)
    SCR_lower(sdir->d_name);
#endif
    return(sdir);
}

#endif
/* ====================================================================
Ouvre un directory pour lecture de son contenu par SCR_readdir().

&NO Plusieurs directories peuvent ˆtre ouverts r‚cursivement … l'aide
    de cette fonction.

&RT Un pointeur vers une structure SCRDIR ou 0 en cas d'erreur
&EX
    SCRDIR *sd;

    sd = SCR_opendir("../mydir")
    if(sd) {
	printf("../mydir is a directory\n",
	SCR_closedir(sd);
	}
    else
	printf("../mydir is not a directory\n",
&TX

&SA structure SCRSTAT, SCR_dir(), SCR_closedir(), SCR_readdir()
======================================================================= */

#ifdef SCRCPP
SCRDIR *SCR_opendir(
char    *dirname
)
#else
SCRDIR *SCR_opendir(dirname)
char    *dirname;
#endif
{
#if defined(DOSW32) || defined(SCRW32)
    return(SCR_opendir_win(dirname));
#else
    return(SCR_opendir_std(dirname));
#endif
}

#if !defined(DOSW32) && !defined(SCRW32)
/*NH*/
SCRSTAT *SCR_readdir_ex_std(SCRDIR *sdir, int getstat, int crc)
{
#ifdef XENIX
    struct direct   *dirent;
#else
    struct dirent   *dirent;
#endif
    SCRSTAT         *ss = 0;
    //    char            fname[2 * SCR_FILE_LG + 1], tmp[2 * SCR_FILE_LG + 1];
    char            fname[SCR_STAT_NAME_LG + 1], tmp[2 * SCR_STAT_NAME_LG + 1]; /* JMP 03-11-06 */

    dirent = readdir((DIR *)sdir->d_dirp);
    if(dirent == 0) return(ss);
    ss = &(sdir->d_ss);
    memset(ss, 0, sizeof(SCRSTAT));
    SCR_strlcpy(ss->ss_name, dirent->d_name, SCR_STAT_NAME_LG);
    if(getstat == 0) return(ss);
//    SCR_strlcpy(ss->ss_name, dirent->d_name, SCR_STAT_NAME_LG); /* JMP 25-10-02 */
    //SCR_strlcpy(fname, sdir->d_name, SCR_FILE_LG);
    SCR_strlcpy(fname, sdir->d_name, SCR_STAT_NAME_LG); /* JMP 03-11-06 */
    if(strcmp(ss->ss_name, "..") == 0) SCR_split_dir(fname, tmp);
    else if(strcmp(ss->ss_name, ".")) {
	if(fname[0] && !U_is_in(fname[strlen(fname) - 1], "\\/")) /* JMP 22-09-99 */
	    strcat(fname, DIR_SEPAR);
	strcat(fname, ss->ss_name);
	}
#if defined(UNIX) || defined(SCRW32) || defined(DOSW32)
    SCR_stat_ex(fname, ss, crc);
#else
    SCR_dos_stat((struct find_t *)(sdir->d_dirp), ss);
    SCR_lower(ss->ss_name);
    if(crc) ss->ssx_crc32 = ScrFileCrc32(fname); /* JMP 25-10-02 */
#endif
    return(ss);
}
#endif

/* ====================================================================
Lit l'entr‚e suivante d'un directory ouvert par SCR_opendir().
Fournit les attributs du fichier si getstat est non nul. Si getstat est
nul, la fonction est plus rapide, un recours … stat() n'‚tant pas
requis en UNIX (appel lent).

Si crc vaut 1, le CRC du fichier est fournit dans le structure SCRSTAT.

&RT Un pointeur vers une structure statique SCRSTAT ou 0 en cas de fin
    de directory
&EX
    SCRDIR  *sd;
    SCRSTAT *ss;

    sd = SCR_opendir("../mydir")
    if(sd) {
	printf("Directory of ../mydir\n\n",
	while(1) {
	    ss = SCR_readdir(sd, 1);
	    if(ss == 0) break;
	    printf("%s : %ld bytes\n", ss->ss_name, ss->ss_size);
	    }
	SCR_closedir(sd);
	}
    else
	printf("../mydir is not a directory\n",
&TX

&SA structure SCRSTAT, SCR_dir(), SCR_opendir(), SCR_closedir()
======================================================================= */

#ifdef SCRCPP
SCRSTAT *SCR_readdir_ex(
SCRDIR  *sdir,
int     getstat,
int     crc
)
#else
SCRSTAT *SCR_readdir_ex(sdir, getstat, crc)
SCRDIR  *sdir;
int     getstat;
int     crc;
#endif
{
#if defined(DOSW32) || defined(SCRW32)
    return(SCR_readdir_ex_win(sdir, getstat, crc));
#else
    return(SCR_readdir_ex_std(sdir, getstat, crc));
#endif
}

/* ====================================================================
Lit l'entr‚e suivante d'un directory ouvert par SCR_opendir().
Fournit les attributs du fichier si getstat est non nul. Si getstat est
nul, la fonction est plus rapide, un recours … stat() n'‚tant pas
requis en UNIX (appel lent).

&RT Un pointeur vers une structure statique SCRSTAT ou 0 en cas de fin
    de directory
&EX
    SCRDIR  *sd;
    SCRSTAT *ss;

    sd = SCR_opendir("../mydir")
    if(sd) {
	printf("Directory of ../mydir\n\n",
	while(1) {
	    ss = SCR_readdir(sd, 1);
	    if(ss == 0) break;
	    printf("%s : %ld bytes\n", ss->ss_name, ss->ss_size);
	    }
	SCR_closedir(sd);
	}
    else
	printf("../mydir is not a directory\n",
&TX

&SA structure SCRSTAT, SCR_dir(), SCR_opendir(), SCR_closedir()
======================================================================= */

#ifdef SCRCPP
SCRSTAT *SCR_readdir(
SCRDIR  *sdir,
int     getstat
)
#else
SCRSTAT *SCR_readdir(sdir, getstat)
SCRDIR  *sdir;
int     getstat;
#endif

{
    return(SCR_readdir_ex(sdir, getstat, 0));
}

#if !defined(DOSW32) && !defined(SCRW32)
/*NH*/
int SCR_closedir_std(sdir)
SCRDIR  *sdir;
{
    if(sdir == 0) return(0);
    closedir((DIR *)sdir->d_dirp);
    SCR_free(sdir);
    return(0);
}
#endif

/* ====================================================================
Ferme le pointeur de directory sdir ouvert par SCR_opendir().

&EX
    SCRDIR *sd;

    sd = SCR_opendir("../mydir")
    if(sd) {
	printf("../mydir is a directory\n",
	SCR_closedir(sd);
	}
    else
	printf("../mydir is not a directory\n",
&TX

&SA structure SCRSTAT, SCR_dir(), SCR_opendir(), SCR_readdir()
======================================================================= */

int SCR_closedir(sdir)
SCRDIR  *sdir;
{
#if defined(DOSW32) || defined(SCRW32)
    return(SCR_closedir_win(sdir));
#else
    return(SCR_closedir_std(sdir));
#endif
}


/*NH*/
#if defined(DOS) && !defined(DOSW32) && !defined(SCRW32)
#include <dos.h>

/*NH*/ 
int SCR_dos_stat(ff, ss)
struct find_t   *ff;
SCRSTAT         *ss;
{
    int     mode = 6;
    long    date;

    ss->ss_size = ff->size;

    /* MODE */
    if(ff->attrib & _A_SUBDIR) mode |= 1;
    if(ff->attrib & _A_RDONLY) mode &= ~2;
    if(ff->attrib & _A_HIDDEN) mode &= ~4;
    ss->ss_mode = mode + (mode << 3) + (mode << 6);

    if(ff->attrib & _A_SUBDIR) ss->ss_mode |= SCR_FTYPE_DIR;
    if(ff->attrib & _A_SYSTEM) ss->ss_mode |= SCR_FTYPE_SYS;
    if(ff->attrib & _A_VOLID)  ss->ss_mode |= SCR_FTYPE_VOL;
    if(ff->attrib & _A_ARCH)   ss->ss_mode |= SCR_FTYPE_ARCH;

    /* DATE */
    date = ff->wr_date;
    ss->ss_cdate = 10000L * (1980L + (date >> 9)) +
		   100L   * ((date >> 5) % 16) +
		   date % 32;
    ss->ss_mdate = ss->ss_adate = ss->ss_cdate;

    /* TIME */
    date = ff->wr_time;
    ss->ss_ctime = 10000L * (date >> 11) +
		   100L   * ((date >> 5) % 64) +
		   2L * (date % 32);
    ss->ss_mtime = ss->ss_atime = ss->ss_ctime;
    ss->ss_gid = ss->ss_uid = ss->ss_nlink = 0;

    return(0);
}

#endif /* DOS */
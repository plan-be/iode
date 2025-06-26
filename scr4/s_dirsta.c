#include "s_dir.h"
#include "s_strs.h"

#include <sys/types.h>
#include <sys/stat.h>


#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"

/*NH*/
U_sh WscrMode32ToSCRSTAT(char *filename)
{
    DWORD   attr = GetFileAttributes(filename);
    U_sh    res = 6;
    int     lg = (int)strlen(filename);

//  DebugB("WscrMode32ToSCRSTAT: file '%s' - mode = %d -> ", filename, res);

    if(lg >= 4) {
	if(SCR_cstrcmp(filename + lg - 4, ".exe") == 0 ||
	   SCR_cstrcmp(filename + lg - 4, ".com") == 0 ||
	   SCR_cstrcmp(filename + lg - 4, ".bat") == 0 ||
	   SCR_cstrcmp(filename + lg - 4, ".cmd") == 0)
	     res |= 1;
	}

    if(attr == 0xFFFFFFFF) return(0);
    if(attr & FILE_ATTRIBUTE_DIRECTORY) res |= 1;
    if(attr & FILE_ATTRIBUTE_READONLY)  res &= ~2;
    if(attr & FILE_ATTRIBUTE_HIDDEN)    res &= ~4;

    res = res + (res << 3) + (res << 6);

    if(attr & FILE_ATTRIBUTE_DIRECTORY) res |= SCR_FTYPE_DIR;
    if(attr & FILE_ATTRIBUTE_SYSTEM)    res |= SCR_FTYPE_SYS;
    if(attr & FILE_ATTRIBUTE_ARCHIVE)   res |= SCR_FTYPE_ARCH;

//  DebugE(" %d\n", res);

    return(res);
}

#endif

/* ====================================================================
Fonction identique à SCR_stat_ex() mais qui lit un groupe de fichiers en une fois.

&EN ss : pointeur vers un tableau de structures SCRSTAT
&EN crc : calcul ou non du CRC

Lit les attributs des fichiers ss[i]->ss_name (taille, mode, ...), en standardise
les valeurs et les place dans ss[i]->ss_size, ...

Si ss[i]->ss_name[0] <= ' ', ne lit rien.

&RT -1 en cas d'erreur, 0 en cas de succès

&SA structure SCRSTAT, SCR_dir(), SCR_stat_ex(), ISC_mstat_ex()
======================================================================= */

SCR_mstat_ex_fn(ss, crc, fn)
SCRSTAT **ss;
int     crc;
int     *fn();
{
    int             nb, i;

    nb = SCR_tbl_size((U_ch **)ss);
    if(nb == 0) return(0);
    for(i = 0 ; ss[i] ; i++) {
	ss[i]->ss_mode = 0;
	ss[i]->ss_size = 0;
	ss[i]->ss_mdate = ss[i]->ss_mtime = 0;
	ss[i]->ss_cdate = ss[i]->ss_ctime = 0;
	ss[i]->ss_adate = ss[i]->ss_atime = 0;
	ss[i]->ss_gid = ss[i]->ss_uid = ss[i]->ss_nlink = 0;
	ss[i]->ssx_mtime = ss[i]->ssx_atime = ss[i]->ssx_ctime = 0;
	ss[i]->ssx_crc32 = 0;
	if(ss[i]->ss_name[0] <= ' ') continue;
	SCR_stat_ex(ss[i]->ss_name, ss[i], crc);
	if(fn) (*fn)(ss[i]); /* JMP 25-07-07 */
	}
    return(0);
}

SCR_mstat_ex(SCRSTAT **ss, int crc)
{
    return(SCR_mstat_ex_fn(ss, crc, 0));
}

/* ====================================================================
Fonction identique à SCR_stat() mais avec 4 champs suppplémentaires.

&EN ssx_atime : access time dans le format standard (nbre de sec depuis le 1/1/1970)
&EN ssx_ctime : creation time dans le format standard (nbre de sec depuis le 1/1/1970)
&EN ssx_mtime : modification time dans le format standard (nbre de sec depuis le 1/1/1970)
&EN ssx_crc32 : crc32 du fichier si le paramètre crc est non nul

Lit les attributs du fichier filename (taille, mode, ...), en standardise
les valeurs et les place dans la structure SCRSTAT pointeur par ss.

&NO Le nom du fichier n'est PAS placé dans ss->ss_name par cette fonction.

&RT -1 en cas d'erreur, 0 en cas de succès
&EX
    SCRSTATEX     ssx;

    if(SCR_stat_ex("../myfile", &ssx, 1))
	printf("../myfile not found\n",
    else
	printf("../myfile : %ld bytes - crc32 = %u\n", ssx.ss_size, ssx_crc32);
&TX

&SA structure SCRSTATEX, SCR_dir(), SCR_stat()
======================================================================= */

SCR_stat_ex(filename, ss, crc)
char        *filename;
SCRSTAT     *ss;
int         crc;
{
    struct  stat            buf;
    int                     rc = 0;
    char                    tmp[SCR_STAT_NAME_LG + 1];
    extern unsigned long    ScrFileCrc32(char *);

    ss->ss_mode = 0;
    ss->ss_size = 0;
    ss->ss_mdate = ss->ss_mtime = 0;
    ss->ss_cdate = ss->ss_ctime = 0;
    ss->ss_adate = ss->ss_atime = 0;
    ss->ss_gid = ss->ss_uid = ss->ss_nlink = 0;
    ss->ssx_mtime = ss->ssx_atime = ss->ssx_ctime = 0;
    ss->ssx_crc32 = 0;

    if(filename == 0 || filename[0] == 0) return(-1); /* JMP 22-09-99 */

    SCR_dir_std(tmp, filename);
    rc = stat(tmp, &buf);
    if(rc >= 0) { /* JMP 31-03-98 */
	ss->ss_size = buf.st_size;

	ss->ss_mdate = SCR_cvt_date((long)buf.st_mtime);
	ss->ss_mtime = SCR_cvt_time((long)buf.st_mtime);
	ss->ss_adate = SCR_cvt_date((long)buf.st_atime);
	ss->ss_atime = SCR_cvt_time((long)buf.st_atime);
	ss->ss_cdate = SCR_cvt_date((long)buf.st_ctime);
	ss->ss_ctime = SCR_cvt_time((long)buf.st_ctime);

	ss->ss_gid = buf.st_gid;
	ss->ss_uid = buf.st_uid;
	ss->ss_nlink = buf.st_nlink;
	ss->ss_mode = buf.st_mode;
	/* STATEX */
	ss->ssx_mtime = (long)buf.st_mtime;
	ss->ssx_atime = (long)buf.st_atime;
	ss->ssx_ctime = (long)buf.st_ctime;
	if(crc)
	    ss->ssx_crc32 = ScrFileCrc32(filename);
	}

#ifdef DOS
    {
#if defined(SCRW32) || defined(DOSW32)
	ss->ss_mode = WscrMode32ToSCRSTAT(filename);
#else
	unsigned int mode;

	if(rc >= 0) {                      /* JMP 31-03-98 */
	    mode = (buf.st_mode >> 6) & 7;
	    ss->ss_mode = buf.st_mode | mode | (mode << 3);
	    }
#endif
    }
#else
    if(rc >= 0) ss->ss_mode = buf.st_mode; /* JMP 31-03-98 */
    //Debug("mode(%s)=%d,%d\n", tmp, ss->ss_mode, buf.st_mode);
#endif
    return(rc);  /* JMP 21-04-98 */
}

/* ====================================================================
Lit les attributs du fichier filename (taille, mode, ...), en standardise
les valeurs et les place dans la structure SCRSTAT pointée par ss.

&NO Le nom du fichier n'est PAS placé dans ss->ss_name par cette fonction.

&RT -1 en cas d'erreur, 0 en cas de succès
&EX
    SCRSTAT     ss;

    if(SCR_stat("../myfile", &ss))
	printf("../myfile not found\n",
    else
	printf("../myfile : %ld bytes\n", ss.ss_size);
&TX

&SA structure SCRSTAT, SCR_dir(), SCR_stat_ex()
======================================================================= */

SCR_stat(filename, ss)
char    *filename;
SCRSTAT *ss;
{
    return(SCR_stat_ex(filename, ss, 0)); /* JMP 07-03-03 */
}

/*NH OLD VERSION
SCR_stat(filename, ss)
char    *filename;
SCRSTAT *ss;
{
    struct  stat    buf;
    int             rc = 0;
    char            tmp[SCR_STAT_NAME_LG + 1];

    ss->ss_mode = 0;
    ss->ss_size = 0;
    ss->ss_mdate = ss->ss_mtime = 0;
    ss->ss_cdate = ss->ss_ctime = 0;
    ss->ss_adate = ss->ss_atime = 0;
    ss->ss_gid = ss->ss_uid = ss->ss_nlink = 0;

    if(filename == 0 || filename[0] == 0) return(-1);

    SCR_dir_std(tmp, filename);
    rc = stat(tmp, &buf);
    if(rc >= 0) {
	ss->ss_size = buf.st_size;
	ss->ss_mdate = SCR_cvt_date(buf.st_mtime);
	ss->ss_mtime = SCR_cvt_time(buf.st_mtime);
	ss->ss_adate = SCR_cvt_date(buf.st_atime);
	ss->ss_atime = SCR_cvt_time(buf.st_atime);
	ss->ss_cdate = SCR_cvt_date(buf.st_ctime);
	ss->ss_ctime = SCR_cvt_time(buf.st_ctime);
	ss->ss_gid = buf.st_gid;
	ss->ss_uid = buf.st_uid;
	ss->ss_nlink = buf.st_nlink;
	ss->ss_mode = buf.st_mode;
	}

#ifdef DOS
    {
#if defined(SCRW32) || defined(DOSW32)
	ss->ss_mode = WscrMode32ToSCRSTAT(filename);
#else
	unsigned int mode;

	if(rc >= 0) {
	    mode = (buf.st_mode >> 6) & 7;
	    ss->ss_mode = buf.st_mode | mode | (mode << 3);
	    }
#endif
    }
#else
    if(rc >= 0) ss->ss_mode = buf.st_mode;
#endif
    return(rc);
}
/* JMP 24-10-02 */




































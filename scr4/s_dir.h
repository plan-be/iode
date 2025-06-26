#ifndef _S_DIR_H_
#define _S_DIR_H_

#define SCR_FTYPE        0xF000
#define SCR_FTYPE_SPEC   0x1000
#define SCR_FTYPE_CHAR   0x2000
#define SCR_FTYPE_DIR    0x4000
#define SCR_FTYPE_BLK    0x6000
#define SCR_FTYPE_REG    0x8000

#define SCR_FTYPE_SYS    0x1000
#define SCR_FTYPE_VOL    0x2000
#define SCR_FTYPE_ARCH   0x10000


#define SCR_PERM_UREAD    0000400
#define SCR_PERM_UWRITE   0000200
#define SCR_PERM_UEXEC    0000100
#define SCR_PERM_GREAD    0000040
#define SCR_PERM_GWRITE   0000020
#define SCR_PERM_GEXEC    0000010
#define SCR_PERM_OREAD    0000004
#define SCR_PERM_OWRITE   0000002
#define SCR_PERM_OEXEC    0000001

#define SCR_FILE_LG         127 /* JMP 11-04-03 */
#if defined(DOS) && !defined(DOSW32) && !defined(SCRW32)
#define SCR_STAT_NAME_LG    15
#else
//#define SCR_STAT_NAME_LG    63
//#define SCR_STAT_NAME_LG    127 /* JMP 06-02-03 */
//#define SCR_STAT_NAME_LG    256 /* JMP 06-02-03 */
#define SCR_STAT_NAME_LG    255 /* JMP 05-01-07 : alignements Linux */
#endif

typedef char SCRFILE[SCR_FILE_LG + 1];

typedef struct _scrstatpre458_ {
    char            ss_name[63 + 1]; /* Nom du fichier */
    long            ss_size;       /* Taille en bytes du fichier */
    long            ss_ctime;      /* Heure de création hhmmss (UNIX) */
    long            ss_cdate;      /* Date de création yyyymmdd (UNIX) */
    long            ss_mtime;      /* Heure de dernière modification hhmmss */
    long            ss_mdate;      /* Date de dernière modification  yyyymmdd */
    long            ss_atime;      /* Heure de dernière accès (UNIX) hhmmss */
    long            ss_adate;      /* Date de dernière accès (UNIX) yyyymmdd */
    unsigned short  ss_mode;       /* Attribut du fichier */
    short           ss_uid;        /* Owner id (UNIX) */
    short           ss_gid;        /* Group id (UNIX) */
    short           ss_nlink;      /* Nb links (UNIX) */
} SCRSTATPRE458;

typedef struct _scrstat_ {
    char            ss_name[SCR_STAT_NAME_LG + 1]; /* Nom du fichier */
    long            ss_size;       /* Taille en bytes du fichier */
    long            ss_ctime;      /* Heure de création hhmmss (UNIX) */
    long            ss_cdate;      /* Date de création yyyymmdd (UNIX) */
    long            ss_mtime;      /* Heure de dernière modification hhmmss */
    long            ss_mdate;      /* Date de dernière modification  yyyymmdd */
    long            ss_atime;      /* Heure de dernière accès (UNIX) hhmmss */
    long            ss_adate;      /* Date de dernière accès (UNIX) yyyymmdd */
    unsigned short  ss_mode;       /* Attribut du fichier */
    short           ss_uid;        /* Owner id (UNIX) */
    short           ss_gid;        /* Group id (UNIX) */
    short           ss_nlink;      /* Nb links (UNIX) */

    /* Nouvelles données */ /* JMP 25-10-02 */
    long            ssx_ctime;      /* time_t de création (UNIX) */
    long            ssx_mtime;      /* time_t de dernière modification */
    long            ssx_atime;      /* time_t de dernière accès (UNIX) */
    unsigned long   ssx_crc32;      /* crc32 du fichier */
} SCRSTAT;

typedef struct _scrstat401_ {
    char            ss_name[127 + 1]; /* Nom du fichier */
    long            ss_size;       /* Taille en bytes du fichier */
    long            ss_ctime;      /* Heure de création hhmmss (UNIX) */
    long            ss_cdate;      /* Date de création yyyymmdd (UNIX) */
    long            ss_mtime;      /* Heure de dernière modification hhmmss */
    long            ss_mdate;      /* Date de dernière modification  yyyymmdd */
    long            ss_atime;      /* Heure de dernière accès (UNIX) hhmmss */
    long            ss_adate;      /* Date de dernière accès (UNIX) yyyymmdd */
    unsigned short  ss_mode;       /* Attribut du fichier */
    short           ss_uid;        /* Owner id (UNIX) */
    short           ss_gid;        /* Group id (UNIX) */
    short           ss_nlink;      /* Nb links (UNIX) */

    /* Nouvelles données */ /* JMP 25-10-02 */
    long            ssx_ctime;      /* time_t de création (UNIX) */
    long            ssx_mtime;      /* time_t de dernière modification */
    long            ssx_atime;      /* time_t de dernière accès (UNIX) */
    unsigned long   ssx_crc32;      /* crc32 du fichier */
} SCRSTAT401;


typedef struct _scrdir_ {
//    char    d_name[SCR_FILE_LG + 1]; /* Nom du dir */
    char    d_name[SCR_STAT_NAME_LG + 1]; /* Nom du dir */ /* JMP 03-11-06 */
    char    *d_dirp;
    SCRSTAT d_ss;
    int     d_os;       /* Version de Windows JMP 07-04-03 */
    int     d_first;    /* Premier passage par la fonction read JMP 07-04-03 */
    char    *d_handle;  /* Handle pour version WIN32 */
} SCRDIR;


#ifdef DOS
#define DIR_SEPAR   "\\"
#define DIR_CHAR    '\\'
#else
#define DIR_SEPAR   "/"
#define DIR_CHAR    '/'
#endif

/* ============= Functions declarations ============= */

#ifndef SCRPROTO

SCRSTAT *SCR_readdir();
SCRSTAT *SCR_readdir_ex();
SCRDIR  *SCR_opendir();
SCRSTAT **SCR_dir();
SCRSTAT **SCR_dir_ex();
SCRSTAT *SCR_dir_edit(), *SCR_dir_edit_1();
char    *SCR_fullpath(), *SCR_getcwd(), *SCR_relpath();
char    *SCR_dir_join();

#else
#include <s_prodir.h>
#endif

#endif










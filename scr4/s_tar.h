#ifndef _S_TAR_
#define _S_TAR_

#define _S_TAR_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _tarhead_ {
    char            name[100];
    int             mode, uid, gid, extno, exttot;
    unsigned long   size, mtime, efsize;
} TARHEAD;


#define TAR_REG   0  /*  Regular file */
#define TAR_5_DD  1  /*  9 x 40 */
#define TAR_5_HD  2  /* 15 x 80 */
#define TAR_3_DD  3  /*  9 x 80 */
#define TAR_3_HD  4  /* 18 x 80 */

#define TAR_TYPE_ERR        -1 /* Device type unknown           */
#define TAR_MEM_ERR         -2 /* Memory full                   */
#define TAR_ACCESS_ERR      -3 /* Tar regular file access error */
#define TAR_DRIVE_ERR       -4 /* Drive name error (A: or B:)   */
#define TAR_DRIVETYPE_ERR   -5 /* Drive and device incompatible */
#define TAR_USERSTOP_ERR    -6 /* Tar stopped by user           */
#define TAR_READ_ERR        -7 /* Tape Read error               */
#define TAR_WRITE_ERR       -8 /* Tape Write error              */
#define TAR_FILEREAD_ERR    -9 /* File Read error               */
#define TAR_SEQUENCE_ERR   -10 /* Disk sequence error           */
#define TAR_SEEK_ERR       -11 /* Seek error                    */
#define TAR_FILECREATION_ERR  -12 /* File creation error        */
#define TAR_FILEWRITE_ERR  -13 /* File write error              */
#define TAR_CHECKSUM_ERR   -14 /* Tar Checksum error            */
#define TAR_TAPEACCESS_ERR -15 /* Tar Tape access error         */

/************* VARIABLES ***************/
extern int              TarErrno;
extern FILE             *TarFd, *TarTapeFd;
extern int              TarType, TarMode, TarDrive, TarVerbose, TarCurUnit;
extern int              TarErrno, TarCurTrack, TarCurHead;
extern long             TarCurBlk, TarMaxBlk;
extern unsigned char    *TarBuffer, *TarTapeBuf;
extern int              TarBiosErrno;
extern int              TarSectors[];
extern int              TarTracks[];
extern int              TarDensity[];

/************* FUNCTIONS ***************/
#ifndef SCRPROTO
extern int              (*TarNextFn)();
extern char             *TarError();
extern FILE             *Tarfopen();
extern int              TarDftNextFn(), TarDftTableFn();

#else /* SCRPROTO */

#ifdef __cplusplus
extern "C" {
#endif
extern int              (*TarNextFn)(int);
/*extern int              TarDftNextFn(int), TarDftTableFn(TARHEAD *); */
#ifdef __cplusplus
}
#endif

#include <s_protar.h>

#endif /* SCRPROTO */
#endif /* _S_TAR_ */



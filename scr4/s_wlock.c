#ifdef LINUX

#include <stdio.h>
#include <sys/file.h>

WscrLock(fd)
FILE    *fd;
{
    while(1) {
	if(flock(fileno(fd), LOCK_EX) != 0)
	    continue;

	fseek(fd, 0L, 2);
	return(0);
    }
    return(-1L);
}

WscrUnLock(fd)
FILE    *fd;
{
    int     nbr = 0;

    fflush(fd);
    while(1) {
	if(flock(fileno(fd), LOCK_UN) != 0)
	    continue;
	return(0);
    }
    return(-1);
}

#else

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <share.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <windows.h>

long LCK_pos;

LCKOpen(char *filename)
{
    return(sopen(filename, O_RDWR, SH_DENYNO, S_IREAD | S_IWRITE));
}

LCKClose(int fh)
{
    LCKUnlock(fh);
    close(fh);
}

LCKLockAndSeek(int fh)
{
    int     nbtry = 0;

    while(1) {
	lseek(fh, 0L, 2);
	LCK_pos = tell(fh);
	if(locking(fh, LK_NBLCK, 2000000L) == 0) break;
	nbtry++;
	if(nbtry > 200) return(-1);
	Sleep(1);
	}

    lseek(fh, 0L, 2);
    return(0);
}

LCKUnlock(int fh)
{
    lseek(fh, LCK_pos, 0);
    return(locking(fh, LK_UNLCK, 2000000L));
}

long WscrLock(FILE *fp)
{
    int fd;

    fd = fileno(fp);
    while(1)
	if(LCKLockAndSeek(fd) == 0) return(0);
}

WscrUnLock(FILE *fp)
{
    int     fd;
    long    size;

    return(LCKUnlock(fileno(fp)));   /* BP_M 13-02-2003 12:48 */
/*
    fd = fileno(fp);
    lseek(fd, 0L, 2);
    size = tell(fd);
    lseek(fd, 0L, 0);
    while(1)
	if(locking(fd, LK_UNLCK, size) == 0) return(0);
*/

}
#endif

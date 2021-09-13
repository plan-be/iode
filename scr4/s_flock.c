#include <stdio.h>
/* BP_M 09-12-2010 13:42 */
#if defined(XENIX) || defined(UNIX)
#   include <sys/types.h>
#   include <sys/file.h>
#else
#if defined(SCRW32) || defined(DOSW32)
#   include <sys/types.h>
#   include <sys/locking.h>
#endif
#endif

SCR_flock(FILE *fd, long size)
{

#if defined(XENIX) || defined(UNIX) || defined(SCRW32) || defined(DOSW32)
    int nbtry = 0;
#if defined(LINUX)
ag: if(flock(fileno(fd), LOCK_EX))
#else
    if(size == -1) size = 200000000L;       /* BP_M 01-03-2003 10:52 */
ag: if(locking(fileno(fd), LK_LOCK, size))
#endif
	{
	nbtry++;
	if(nbtry > 10) return(-1);
	// sleep(1); /* JMP 07-03-17 */
	SCR_sleep(1000); /* JMP 07-03-17 */
	goto ag;
	}
#endif
    return(0);
}

SCR_funlock(FILE *fd, long size)
{
#if defined(XENIX) || defined(UNIX)
#if defined(LINUX)
    return(flock(fileno(fd), LOCK_UN));
#else
    if(size == -1) size = 200000000L;       /* BP_M 01-03-2003 10:52 */
    return(locking(fileno(fd), LK_UNLCK, size));
#endif
#endif
}

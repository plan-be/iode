#if defined(DOSW32) || defined(SCRW32)
#include "scr4w.h"

int SCR_sleep(int ms)
{
    Sleep(ms);
    return(0);
}

#else
    #ifdef LINUX
    #include <time.h>
    #include <errno.h>

    SCR_sleep(ms)
    int     ms;
    {
	struct timespec request, remaining;

	request.tv_sec  = ms / 1000;
	request.tv_nsec = 1000000 * (ms % 1000);
	while(nanosleep(&request, &remaining) == EINTR)
	    request = remaining;

	return(0);
    }
    #else
    SCR_sleep(ms)
    int     ms;
    {
	int     nbsec = 1 + (ms - 1) / 1000;

	sleep(nbsec);
    }

    #endif

#endif

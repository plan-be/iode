#include <time.h>
#ifndef VC
    #include <utime.h>
#endif
#ifdef UNIX
#include <sys/time.h>
#endif

#include "scr4w.h"

/* ===============================================================
Retourne le nombre de milli secondes depuis le démarrage du programme.
================================================================== */

long WscrGetMS()
{
#if defined(SCRW32) || defined(DOSW32)
    return(GetTickCount());
#else
#ifdef LINUX
    struct timeval  tv;

    gettimeofday(&tv, NULL);
    return(1000L * (tv.tv_sec % 1000000L) + tv.tv_usec / 1000L);
#else
    return((1000L * clock()) / CLK_TCK);
#endif
#endif
}


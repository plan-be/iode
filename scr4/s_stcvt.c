#include <time.h>
#ifdef INTAU
#include <sys/types.h>
#endif


/* localtime ne fonctionne pas correctement en VC (retourne NULL)
  et donc wrapper */

struct tm *localtime_vc_bc(long t)
{
#ifdef VC
    __time32_t t32 = (__time32_t)t;
    return(_localtime32(&t32));
#else
    time_t tt = (time_t)t;
    return(localtime(&tt));
#endif

}
long SCR_cvt_time(long t)
{
    struct tm   *tt;
    long        l;

    tt = localtime_vc_bc(t);
    if(tt == 0) return(0L); /* JMP 26-03-98 */
    l = (long)tt->tm_hour * 10000L +
	(long)tt->tm_min * 100L +
	(long)tt->tm_sec;
    return(l);
}

long SCR_cvt_time64(long t)
{
    struct tm   *tt;
    long        l;

    tt = localtime_vc_bc(t);
    if(tt == 0) return(0L); /* JMP 26-03-98 */
    l = (long)tt->tm_hour * 10000L +
	(long)tt->tm_min * 100L +
	(long)tt->tm_sec;
    return(l);
}

long SCR_cvt_date(long t)
{
    struct tm   *tt;
    long        l;

    tt = localtime_vc_bc(t);
    if(tt == 0) return(0L); /* JMP 26-03-98 */
    l = (long)tt->tm_mday +
	100L * (long)(tt->tm_mon + 1L) +
	10000L * (long)(1900L + tt->tm_year);
    return(l);
}


#include <time.h>
#ifdef INTAU
#include <sys/types.h>
#endif


/* localtime ne fonctionne pas correctement en VC (retourne NULL)
  et donc wrapper */

struct tm *localtime_vc_bc(time_t *t)
{
#ifdef VC
    return(_localtime32(t));
#else
    return(localtime(t));
#endif

}
long SCR_cvt_time(t)
long    t;
{
    struct tm   *tt;
    long        l;

    tt = localtime_vc_bc((time_t *)&t);
    if(tt == 0) return(0L); /* JMP 26-03-98 */
    l = (long)tt->tm_hour * 10000L +
	(long)tt->tm_min * 100L +
	(long)tt->tm_sec;
    return(l);
}

long SCR_cvt_time64(t)
long    t;
{
    struct tm   *tt;
    long        l;

    tt = localtime_vc_bc((time_t *)&t);
    if(tt == 0) return(0L); /* JMP 26-03-98 */
    l = (long)tt->tm_hour * 10000L +
	(long)tt->tm_min * 100L +
	(long)tt->tm_sec;
    return(l);
}

long SCR_cvt_date(t)
long    t;
{
    struct tm   *tt;
    long        l;

    tt = localtime_vc_bc((time_t *)&t);
    if(tt == 0) return(0L); /* JMP 26-03-98 */
    l = (long)tt->tm_mday +
	100L * (long)(tt->tm_mon + 1L) +
	10000L * (long)(1900L + tt->tm_year);
    return(l);
}


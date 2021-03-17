#include <scr.h>
#include <time.h>

long SCR_cvt_time_2(t)
long    t;
{
    struct tm   *tt;
    long        l;

    tt = _localtime32((time_t *)&t);
    if(tt == 0) return(0L); /* JMP 26-03-98 */
    l = (long)tt->tm_hour * 10000L +
	(long)tt->tm_min * 100L +
	(long)tt->tm_sec;
    return(l);
}

main() {
    long t, ct, scrt;
    
    time(&t);
    ct = SCR_current_time();
    scrt = SCR_cvt_time_2(t);
    printf("t=%ld, ct=%ld, scrt=%ld\n", t, ct, scrt);
        
}



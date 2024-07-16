#include "s_date.h"
#include "s_strs.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// WARNING: the min and max macro make conflicts with the GNU implementation 
//          of the C++ standard library
#if !defined(__GNUC__) || !defined(__cplusplus)
    #ifndef WATCOM
        #ifndef max
            #define max(x,y)        ((x)<(y)?(y):(x))
        #endif
        #ifndef min
            #define min(x,y)        ((x)<(y)?(x):(y))
        #endif
    #endif
#endif

extern char     SCR_TIME_FMT[];


/* ====================================================================
Transforme une heure formatt�e dans string en long. Le format est celui
d�fini dans la variable globale SCR_TIME_FMT.

&RT un long repr�sentant l'heure
&EX
    SCR_time_to_long("12:53:12");
&TX
&SA SCR_ftime_to_long()
=======================================================================*/

long SCR_time_to_long(string)
char *string ;
{
    return(SCR_ftime_to_long(string, SCR_TIME_FMT));
}

/* ====================================================================
Transforme une heure formatt�e dans string en long. Le format est celui
d�fini dans le param�tre fmt.

Seuls les deux premiers h, s et m sont pris en compte dans le calcul.
Ainsi le format "hhH.mm" sur "24H.30" donnera 243000.

&RT un long repr�sentant l'heure (hhmmss)
&EX
    SCR_ftime_to_long("12:30", "hh-mm");

	donne

    123000
&TX
&SA SCR_time_to_long()
=======================================================================*/

long SCR_ftime_to_long(str, fmt)
char    *str;
char    *fmt;
{
    long    hour = 0, atol();
    int     h = 10, m = 10, s = 10;
    int     i;
    //char    buf[255];
    char    buf[1024]; /* JMP 10-01-11 */

    /* BP_M 07-12-1999 */
    strcpy(buf, str); SCR_strip(buf);
    if(strlen(buf) == 2) {
	hour = SCR_current_time();
	hour = atol(buf) * 10000L + hour % 10000L;
	return(hour);
    }
    if(strlen(buf) == 4) return(atol(buf) * 100L);
    /* BP_M 07-12-1999 */


    if(str == 0 || str[0] == 0 || strlen(str) != strlen(fmt)) return(0L);
    for(i = 0 ; fmt[i] != 0 ; i++)
	switch(SCR_date_upper_char(fmt[i])) {
	    case 'H': if(h < 0) break;
		      hour += (str[i] - '0') * 10000L * h;
		      h -= 9;
		      break;
	    case 'M': if(m < 0) break;
		      hour += (str[i] - '0') * 100L * m;
		      m -= 9;
		      break;
	    case 'S': if(s < 0) break;
		      hour += (str[i] - '0') * s;
		      s -= 9;
		      break;
	    default:  break;
	}
    return(hour);
}

/* ====================================================================
V�rifie qu'une heure est correcte. L'heure doit �tre comprise entre 00
et 23, les minutes et les secondes entre 00 et 59.

&RT 0 en cas d'heure correcte, -1 sinon.
&SA SCR_check_fmt_time()
=======================================================================*/

SCR_check_time(hour)
long    hour;
{
    long    h, m, s;

    h = hour / 10000L;
    m = (hour % 10000L) / 100L;
    s = hour % 100;
    if(h > 23 || m > 59 || s > 59) return(-1);
    return(0);
}

/* ====================================================================
V�rifie qu'une heure formatt�e est correcte.

&RT 0 en cas d'heure correcte, -1 sinon
&SA SCR_check_date()
=======================================================================*/

SCR_check_fmt_time(str, fmt)
char    *str, *fmt;
{
    return(SCR_check_time(SCR_ftime_to_long(str, fmt)));
}




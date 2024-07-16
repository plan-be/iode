#include "s_date.h"
#include "s_strs.h"
#include <time.h>
#include <stdlib.h>
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

char     SCR_TIME_FMT[12] = "hh:mm:ss";
char     SCR_DATE_FMT[15] = "dd/mm/yyyy";

/* ====================================================================
Retourne la date de la machine sous forme d'un long.

&EN Les unit�s et les dizaines repr�sentent le jour du mois (01-31).
&EN Les centaines et les milliers repr�sentent le mois (01-12)
&EN Les dizaines de milliers et centaines de milliers repr�sentent
    l'ann�e dans le si�cle
&EN Les millions et dizaines de millions le si�cle

Cette fa�on de repr�senter les dates est utilis�e dans les databases.
Elle pr�sente l'avantage de classer facilement en ordre croissant les
champs de date.

&RT la valeur de la date machine en format long
&EX
    printf("Date : %ld\n", SCR_current_date());

    donne par exemple :

    19910131
&TX
&SA SCR_long_to_fdate(), SCR_long_to_date()
=======================================================================*/

long _SCR_current_date()
{
#ifdef VC
    long	l;
    __time64_t	ltime64;
    struct tm   *tt;
    
    _time64(&ltime64); 
    tt = _localtime64(&ltime64);
    if(tt == 0) return(0L); /* JMP 26-03-98 */
    l = (long)tt->tm_mday +
	100L * (long)(tt->tm_mon + 1L) +
	10000L * (long)(1900L + tt->tm_year);

    return(l);
#else
    long    ltime;

    time(&ltime);
    return(SCR_cvt_date(ltime));
#endif    
}

/* ====================================================================
Retourne l'heure de la machine sous forme d'un long.

&EN Les unit�s et les dizaines repr�sentent les secondes
&EN Les centaines et les milliers la minute
&EN Les dizaines de milliers et centaines de milliers repr�sentent
    l'heure

Cette fa�on de repr�senter les heures est utilis�e dans les databases.
Elle pr�sente l'avantage de classer facilement en ordre croissant les
champs TIME.

&RT la valeur de l'heure machine en format long
&EX
    printf("Heure : %ld\n", SCR_current_time());

    donne par exemple :

    235940
&TX
&SA SCR_long_to_ftime(), SCR_long_to_time()
=======================================================================*/

long _SCR_current_time()
{
#ifdef VC        
    long	l;
    __time64_t	ltime64;
    struct tm   *tt;
    
    _time64(&ltime64); 
    tt = _localtime64(&ltime64);
    l = (long)tt->tm_hour * 10000L +
	(long)tt->tm_min * 100L +
	(long)tt->tm_sec;
    return(l);   
#else
    long    l;

    time(&l);
    return(SCR_cvt_time(l));
#endif    
}

/* ====================================================================
Formatte dans string un long (val) repr�sentant une date. Le format est
d�fini dans la variable globale SCR_DATE_FMT[15] qui peut �ventuellement
�tre adapt�e (voir SCR_long_to_fdate()). Par d�faut, ce format vaut

&CO
    "dd/mm/yyyy"
&TX
&RT un pointeur vers string (r�sultat du formattage)
&EX
    SCR_long_to_date(19901231, buf);

	donne

    buf === "31/12/1990"
&TX
&SA SCR_long_to_fdate(), SCR_current_date()
=======================================================================*/

char *SCR_long_to_date(long val, char* string)
{
    return(SCR_long_to_fdate(val, string, SCR_DATE_FMT));
}

/* ====================================================================
Formatte dans string un long (val) repr�sentant une heure. Le format est
d�fini dans la variable globale SCR_TIME_FMT[15] qui peut �ventuellement
�tre adapt�e (voir SCR_long_to_ftime()). Par d�faut, ce format vaut

&CO
    "hh/mm/ss"
&TX
&RT un pointeur vers string (r�sultat du formattage)
&EX
    SCR_long_to_time(235900, buf);

	donne

    buf === "23:59:00"
&TX
&SA SCR_long_to_fdate(), SCR_current_date()
=======================================================================*/

char *SCR_long_to_time(long val, char* string)
{
    return(SCR_long_to_ftime(val, string, SCR_TIME_FMT));
}

/* ====================================================================
Formatte dans str un long (val) repr�sentant une date. Le format est
d�fini dans le param�tre fmt.

Le format peut contenir n'importe quels caract�res. Seuls les y, Y, m,
M, d et D sont interpr�t�s. Les autres sont laiss�s comme tels.
Les Y (y) sont remplac�s par les derniers chiffres de l'ann�e, les M (m)
par les chiffres du mois et les D (d) par ceux du jour. L'ordre
d'apparition des lettres dans le format n'a pas d'importance.

Les Y au-del� du quatri�me sont laiss�s tel quels. Les M au-del� du 2�me
et les D au-del� du deuxi�me de m�me.

&RT un pointeur vers str (r�sultat du formattage)
&EX
    SCR_long_to_fdate(19901231L, buf, "mm-dd-yy");

	donne

    buf === "12/31/90"
&TX
&SA SCR_long_to_date(), SCR_current_date()
=======================================================================*/

char *SCR_long_to_fdate(long date, char* str, char* fmt)
{
    char    vyy[25], *yy = vyy, mm[5], dd[5];  /* JMP 13-05-96 */
    int     i, nby = 0, y = 0, m = 0, d = 0;

/*    if(SCR_check_date(date)) date = 0L; /* JMP38 02-10-92 */
/*    yy = "1989"; /* JMP 13-05-96 */

    if(fmt == 0) fmt = "DD/MM/YYYY";

    sprintf(yy, "%04ld", date / 10000L);
    sprintf(mm, "%02ld", (date % 10000L) / 100L);
    sprintf(dd, "%02ld", date % 100L);

    for(i = 0 ; fmt[i] != 0 ; i++)
	if(SCR_date_upper_char(fmt[i]) == 'Y') nby++;
    nby = min(4, nby);
    yy += 4 - nby;

    for(i = 0 ; fmt[i] != 0 ; i++) {
	str[i] = fmt[i];
	switch(SCR_date_upper_char(fmt[i])) {
	    case 'Y': if(y > nby - 1) break;
		      str[i] = yy[y]; y++;
		      break;
	    case 'M': if(m > 1) break;
		      str[i] = mm[m]; m++;
		      break;
	    case 'D': if(d > 1) break;
		      str[i] = dd[d]; d++;
		      break;
	    default:  break;
	    }
	}
    str[i] = 0;
    return(str);
}

/* ====================================================================
Formatte dans str un long (val) repr�sentant une heure. Le format est
d�fini dans le param�tre fmt.

Le format peut contenir n'importe quels caract�res. Seuls les H, h, m,
M, s et S sont interpr�t�s. Les autres sont laiss�s comme tels.
Les H (h) sont remplac�s par les chiffres de l'heure, les M (m)
par les chiffres des minutes et les S (s) par ceux des secondes. L'ordre
d'apparition des lettres dans le format n'a pas d'importance.

Seuls les deux premiers h, s et m sont pris en compte dans le calcul.
Ainsi le format "hhH.mm" sur 243000 donnera "24H.30".

&RT un pointeur vers str (r�sultat du formattage)
&EX
    SCR_long_to_fdate(235902L, buf, "hh:mm(ss)");

	donne

    buf === "23:59(02)"
&TX
&SA SCR_long_to_time(), SCR_current_time()
=======================================================================*/

char *SCR_long_to_ftime(long hour, char* str, char* fmt)
{
    char    hh[25], mm[25], ss[25]; /* JMP 13-05-96 */
    int     i, h = 0, m = 0, s = 0;

    if(fmt == 0) fmt = "HH:MM";

    sprintf(hh, "%02ld", hour / 10000L);
    sprintf(mm, "%02ld", (hour % 10000L) / 100L);
    sprintf(ss, "%02ld", hour % 100L);

    for(i = 0 ; fmt[i] != 0 ; i++) {
	str[i] = fmt[i];
	switch(SCR_date_upper_char(fmt[i])) {
	    case 'H': if(h > 1) break;
		      str[i] = hh[h]; h++;
		      break;
	    case 'M': if(m > 1) break;
		      str[i] = mm[m]; m++;
		      break;
	    case 'S': if(s > 1) break;
		      str[i] = ss[s]; s++;
		      break;
	    default:  break;
	    }
	}
    str[i] = 0;
    return(str);
}
/*NH*/
int SCR_fmt_dt(char* fmto, char* fmti)
{
    int     i;

    strcpy(fmto, fmti);
    for(i = 0 ; fmti[i] != 0 ; i++)
	switch(SCR_date_upper_char(fmti[i])) {
	    case 'Y':
	    case 'M':
	    case 'D':
	    case 'H':
	    case 'S': fmto[i] = '0'; break;
	}
    return(0);
}

/*NH*/
int SCR_date_upper_char(int ch)
{
    return(SCR_upper_char(ch)); /* JMP 29-03-00 */
/*    if(islower(ch)) ch = toupper(ch);
    return(ch);
*/
}

/* ====================================================================
Retourne le nombre de secondes �coul�es depuis minuit.

&EN time est pr�sent� sous forme de long : HHMMSS. Par exemple 143652.

&RT le nombre de secondes (long)
&EX
    DT_time_num(SCR_current_time());
&TX
&SA DT_date_num()
----------------------------------------------------------------------- */
long DT_time_num(long date)
{
    long    h, m, s;

    h = date / 10000L;
    m = (date % 10000L) / 100L;
    s = date % 100L;
    return(s + 60 * m + 3600 * h);
}

/* ====================================================================
Retourne l'heure � partir d'un nombre de secondes.

&EN nbs est un nombre de secondes � partir de minuit

&RT l'heure en format HHMMSS
&EX
    DT_num_time(12545);
&TX
&SA DT_date_num(), DT_time_num(), DT_num_date()
----------------------------------------------------------------------- */
long DT_num_time(long nbs)
{
    long    h, m, s;

    s   = nbs % 60;
    nbs -= s;
    m   = (nbs / 60L) % 60L;
    h   = (nbs - (m * 60L)) / 3600;
    h   = h % 24;

    return(h * 10000L + m * 100L + s);
}

long DT_diff_date_time(long d2, long t2, long d1, long t1)
{
    long    days, secs;

    days = 3600L * 24L * DT_diff_dates(d2, d1);
    secs = DT_time_num(t2) - DT_time_num(t1);

    return(days + secs);
}




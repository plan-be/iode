#include "s_date.h"
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

extern char     SCR_DATE_FMT[];

long SCR_Y2K_EPOCH = 0L; /* JMP 28-11-98 */

/* ====================================================================
Transforme une date formatt�e dans string en long. Le format est celui
d�fini dans la variable globale SCR_DATE_FMT.

&RT un long repr�sentant la date
&EX
    SCR_date_to_long("21/02/1991");
&TX
&SA SCR_fdate_to_long()
=======================================================================*/

long SCR_date_to_long(string)
char *string ;
{
    return(SCR_fdate_to_long(string, SCR_DATE_FMT));
}

/* ====================================================================
Transforme une date formatt�e dans string en long. Le format est celui
d�fini dans le param�tre fmt.

&RT un long repr�sentant la date (yyyymmss)
&EX
    SCR_fdate_to_long("02-21-91", "mm-dd-yy");
&TX

La variable SCR_Y2K_PIVOT permet de modifier le comportement lors de la
transformation de date � 2 chiffres pour l'ann�e.

&SY
   int SCR_Y2K_PIVOT = 75;
&TX

Pour conna�tre le si�cle � indiquer dans une date dont l'ann�e n'est
connue que par les deux derniers chiffres, on choisit le si�cle comme
suit :

&CO
    soit
	YY l'ann�e introduite,
    on prend :
	date = si�cle_ courant + YY;
	si date << date_courante - SCR_Y2K_PIVOT ans
	alors date += ans;
	sinon si date >>= date_courante + 100 ans)
	      alors date -= 100 ans;
&TX

&SA SCR_date_to_long()
=======================================================================*/

long SCR_fdate_to_long(str, fmt)  /* BP_M 14-07-1997 */
char    *str;
char    *fmt;
{
    long    date = 0, dt[3], atol();
    int     nby = 0, nbd = 0, nbm = 0,
	    y = 1, m = 10, d = 10;
    int     i, j;
    char    bd[5], bm[5], by[10];
    extern  int     DT_DAYS[];

    if(str == 0 || str[0] == 0) return(0L);
    if(SCR_Y2K_EPOCH == 0L)
	SCR_Y2K_EPOCH = SCR_current_date() - SCR_Y2K_PIVOT * 10000L; /* JMP 28-11-98 */

#ifdef SCRW32
    /* BP_M 14-07-1997 */
    j = strlen(str);
    for(i = 0 ; i < j ; i++)
	if(str[i] < '0' || str[i] > '9') break;
    if(i == j) {
	DT_dmy2000(SCR_current_date(), dt);
	if(i <= 3) {
	    dt[0] = atoi(str);
	    return(DT_dmy_date2000(dt));
	}
	if(i == 3) {
	    dt[0] = str[0] - '0';
	    dt[1] = atoi(str + 1);
	    return(DT_dmy_date2000(dt));
	}
	if(i == 4) {
	    dt[0] = (str[0] - '0') * 10;
	    dt[0] += (str[1] - '0');
	    dt[1] = atoi(str + 2);
	    return(DT_dmy_date2000(dt));
	}
	if(i == 6) {
	    dt[0] = (str[0] - '0') * 10;
	    dt[0] += (str[1] - '0');
	    dt[1] = (str[2] - '0') * 10;
	    dt[1] += (str[3] - '0');
	    dt[2] = atoi(str + 4) + 1900L;
	    /* if(dt[2] < 1915L) dt[2] += 100L; /* JMP 29-08-98 */
	    if(dt[2] < SCR_Y2K_EPOCH / 10000L) dt[2] += 100L;              /* JMP 28-11-98 */
	    else if(dt[2] >= SCR_Y2K_EPOCH / 10000L + 100L) dt[2] -= 100L; /* JMP 28-11-98 */
	    return(DT_dmy_date2000(dt));
	}
	if(i == 8) {
	    dt[0] = (str[0] - '0') * 10;
	    dt[0] += (str[1] - '0');
	    dt[1] = (str[2] - '0') * 10;
	    dt[1] += (str[3] - '0');
	    dt[2] = atoi(str + 4);
	    return(DT_dmy_date2000(dt));
	}
    }
#endif

    if(strlen(str) != strlen(fmt)) return(0L);


    for(i = 0 ; fmt[i] != 0 ; i++)
	switch(SCR_date_upper_char(fmt[i])) {
	    case 'Y': by[nby] = str[i]; nby++;  break;
	    case 'D': bd[nbd] = str[i]; nbd++;  break;
	    case 'M': bm[nbm] = str[i]; nbm++;  break;
	}
    by[nby] = bm[nbm] = bd[nbd] = 0;
    if(atol(by) == 0 &&
       atol(bm) == 0 &&
       atol(bd) == 0) return(0L);

    DT_dmy2000(SCR_current_date(), dt);

    y = atol(by);
    m = atol(bm);
    d = atol(bd);

    if(y > 0) {
	if(strlen(by) == 2) {
	    if(y < SCR_Y2K_PIVOT)
		y += 2000L;
	    else
		y += 1900L;
	}
	dt[2] = y;
    }
    else {  /* JMP 05-01-01 */
	if(strlen(by) < 4) dt[2] = 2000L; /* JMP 05-01-01 */
	}

    if(m > 0) {
	if(m > 12) m = 12;
	dt[1] = m;
    }

    /* Si dans le format il n'y a rien pour le jour, le laisser � 1 */
    if(d <= 0) {
	if(nbd != 0) d = dt[0]; /* JMP 05-02-01 */  /* BP_M 06-07-2003 */
	else         d = 1;
    }

    if(DT_bix((long)dt[2])) DT_DAYS[1] = 29;
    else                    DT_DAYS[1] = 28;
    dt[0] = min(d, DT_DAYS[dt[1] - 1]);

    return(DT_dmy_date2000(dt));

/*
    for(i = 0, j = 0 ; fmt[i] != 0 ; i++) {
	c = SCR_date_upper_char(fmt[i]);
	if((c == 'Y' || c == 'M' || c == 'D') && str[i] != '0') j++;
    }
    if(j == 0) return(0L);
    for(i = 0 ; fmt[i] != 0 ; i++)
	switch(SCR_date_upper_char(fmt[i])) {
	    case 'Y': nby++; y *= 10; break;
	    case 'D': nbd++;          break;
	    case 'M': nbm++;          break;
	}
    nby = min(4, nby);
    cdate = SCR_current_date();
    switch(nby) {
	case 0: date = (cdate / 10000L) * 10000L; break;
	case 1: date = (cdate / 100000L) * 100000L; break;
	case 2: date = (cdate / 1000000L) * 1000000L; break;
	case 3: date = (cdate / 10000000L) * 10000000L; break;
    }
    if(nbd == 0) date += 1L;
    if(nbm == 0) date += 100L;
    for(i = 0 ; fmt[i] != 0 ; i++)
	switch(SCR_date_upper_char(fmt[i])) {
	    case 'Y': if(y < 1) break;
		      date += (str[i] - '0') * 1000L * y;
		      y /= 10;
		      break;
	    case 'M': if(m < 0) break;
		      date += (str[i] - '0') * 100L * m;
		      m -= 9;
		      break;
	    case 'D': if(d < 0) break;
		      date += (str[i] - '0') * d;
		      d -= 9;
		      break;
	    default:  break;
	}


    if(nby <= 2) {
	if(date < SCR_Y2K_EPOCH) date += 1000000L;
	else if(date >= SCR_Y2K_EPOCH + 1000000L) date -= 1000000L;
	}
    return(date);
*/
}

/* ====================================================================
V�rifie qu'une date est correcte. Le mois doit �tre compris entre 01 et
12, le jour entre 1 et 31,  l'ann�e n'est pas v�rifi�e.

&RT 0 en cas d'heure correcte, -1 sinon
&SA SCR_check_fmt_date()
=======================================================================*/

SCR_check_date(date)
long    date;
{
    int day, month, year;

    year  = date / 10000L;
    month = (date % 10000L) / 100L;
    day   = date % 100L;

    if(date == 0) return(0);

    if(day > 31 || day < 1) return(-1);
    if(month > 12 || month < 1) return(-1);
    if(month == 2 && day > 29) return(-1);               /* feb */
    if( (day > 30) && (month == 4 ||                    /* apr,jun,sep,nov */
		       month == 6 ||
		       month == 9 ||
		       month == 11) ) return(-1);
    if(month != 2 || day != 29) return(0);                          /* feb */
    if(0 != year % 4) return(-1);
    return(0);
}

/* ====================================================================
V�rifie qu'une date formatt�e est correcte. Le mois doit �tre compris
entre 01 et 12, le jour entre 1 et 31, l'ann�e n'est pas v�rifi�e.

&RT 0 en cas de date correcte, -1 sinon
&SA SCR_check_date()
=======================================================================*/

SCR_check_fmt_date(str, fmt)
char    *str, *fmt;
{
    return(SCR_check_date(SCR_fdate_to_long(str, fmt)));
}

#include "s_date.h"

// WARNING: the min and max macro make conflicts with the GNU implementation 
//          of the C++ standard library
#if !defined(__GNUC__) || !defined(__cplusplus)
    #define max(x,y)        ((x)<(y)?(y):(x))
    #define min(x,y)        ((x)<(y)?(x):(y))
#endif
#define DT_eq_month(d1, d2)     (DT_month(d1) == DT_month(d2))

int     DT_MONTHS[] = {0,31,59,90,120,151,181,212,243,273,304,334} ;
int     DT_DAYS[]   = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* ====================================================================
Retourne l'année d'une date.
&EX
    DT_year(19900315L) vaut 1990
&TX
&SA
----------------------------------------------------------------------- */
DT_year(date)
long    date;
{
    return((int)(date / 10000L));
}

/* ====================================================================
Retourne le mois d'une date.
&EX
    DT_month(19900315L) vaut 3
&TX
&SA
----------------------------------------------------------------------- */
DT_month(date)
long    date;
{
    return((int)((date / 100L) % 100L));
}

/* ====================================================================
Retourne le jour d'une date.
&EX
    DT_day(19900315L) vaut 15
&TX
&SA
----------------------------------------------------------------------- */
DT_day(date)
long    date;
{
    return((int)(date % 100L));
}

/* ====================================================================
Retourne le nombre de jours écoulés entre le premier janvier 1900 et
date.

date est présenté sous forme de long : YYYYMMDD. Par exemple 19911010.

&RT le nombre de jours (long)
&EX
    DT_date_num(SCR_current_date());
&TX
&SA DT_num_date()
----------------------------------------------------------------------- */
long DT_date_num(date)
long    date;
{
    long    day, d[3];

    DT_dmy(date, d);

    day  = d[2] * 365L + d[2] / 4L + DT_MONTHS[d[1] - 1] + d[0];
    if(d[1] <= 2 && DT_bix(1900L + d[2])) day--;
    return(day);
}

/* ====================================================================
Transforme en date un nombre de jours écoulés (day) depuis le 1/1/1900.

Le résultat est présenté sous forme d'un long : YYYYMMDD (19911231).
&RT la date correspondant au nombre de jours
&EX
    DT_num_date(30000L);
&TX
&SA DT_date_num()
======================================================================= */
long DT_num_date(day)
long    day;
{
    long    d[3];

    d[0] = 31;
    d[1] = 12;
    d[2] = (long)(day / 365.25);
    if(DT_date_num(DT_dmy_date(d)) < day) d[2] ++;
    day -= d[2] * 365L + d[2] / 4L;
    if(day == 59 && DT_bix(1900L + d[2])) {   /* JMP 23-02-96 */
	d[1] = 2;                             /* JMP 23-02-96 */
	d[0] = 29;                            /* JMP 23-02-96 */
	}                                     /* JMP 23-02-96 */
    else {                                    /* JMP 23-02-96 */
	if(day <= 59 && DT_bix(1900L + d[2])) day++;  /* JMP 23-02-96 */
	d[1] = min(12, 1 + day / 30L);                /* JMP 23-02-96 */
	if(DT_MONTHS[d[1] - 1] >= day) d[1] --;       /* JMP 23-02-96 */
	d[0] = day - DT_MONTHS[d[1] - 1];             /* JMP 23-02-96 */
	}                                             /* JMP 23-02-96 */
    return(DT_dmy_date(d));
}
/* ====================================================================
Indique si l'année donnée sous forme d'un long est bissextile ou non.

&RT 0 si l'année n'est pas bissextile, 1 sinon.
&EX
	DT_bix(1990L);
&TX
======================================================================= */

DT_bix(year)
long    year;
{
    return(year % 400L == 0L || (year % 4L == 0L && year % 100L != 0L));
}

/*NH*/
int DT_dmy(date, d)
long    date;
long    d[];
{
    d[2] = date / 10000L;
    d[2] -= 1900L;
    d[1] = (date % 10000L) / 100L;
    d[0] = date % 100L;
    return(0);
}
/*NH*/
long DT_dmy_date(d)
long    d[];
{
    return((d[2] + 1900L) * 10000L + d[1] * 100L + d[0]);
}

/*NH*/
long DT_dmy2000(date, d)
long    date;
long    d[];
{
    d[2] = date / 10000L;
    d[1] = (date % 10000L) / 100L;
    d[0] = date % 100L;
    return(date);
}
/*NH*/
long DT_dmy_date2000(d)
long    d[];
{
    return(d[2] * 10000L + d[1] * 100L + d[0]);
}

/* ====================================================================
Retourne le jour de la semaine correspondant à la date donnée
&RT un entier compris entre 1 (lundi) et 7 (dimanche)
&EX
    DT_day_w(19911031L)
    vaut
    4 (jeudi)
&TX
======================================================================= */
DT_day_w(date)
long    date;
{
    return((int)((DT_date_num(date) - 1L) % 7L + 1L));
}

/* ====================================================================
Ajoute à une date (YYYYMMDD) un nombre de mois. Si le jour n'appartient
pas au nouveau mois, il est ramené au dernier jour du mois. nbm peut
étre négatif.

&RT la nouvelle date (YYYYMMDD)
&EX
    dt = 19920131L;
    dt = DT_add_months(dt, 1);
    printf("%ld\n", dt);
    dt = DT_add_months(dt, -1);
    printf("%ld\n", dt);

	vaut

    19920229
    19920129
&TX
&SA DT_add_days()
======================================================================= */
long DT_add_months(date, nbm)
long    date;
int     nbm;
{
    long    d[3];

    DT_dmy(date, d);
    d[1] += (long)nbm;
    while(d[1] <= 0) {
	d[2] -= 1;
	d[1] += 12;
	}
    d[2] += (d[1] - 1L) / 12L;
    d[1] -= 12L * ((d[1] - 1L) / 12L);
    if(DT_bix(d[2] + 1900L))
	DT_DAYS[1] = 29;
    else
	DT_DAYS[1] = 28;
    d[0] = min(DT_DAYS[d[1] - 1], d[0]);

    return(DT_dmy_date(d));
}

/* ====================================================================
Ajoute à une date (YYYYMMDD) un nombre de jours.

&RT la nouvelle date (YYYYMMDD)
&SA DT_add_months()
======================================================================= */
long DT_add_days(date, nbd)
long    date;
int     nbd;
{
    date = DT_date_num(date) + (long)nbd;
    return(DT_num_date(date));
}

/* ====================================================================
Modifie une date (YYYYMMDD) en fonction d'un calendrier spécifique. dayr
peut prendre les valeurs suivantes :

&EN 0 : pas de changement de date
&EN 1 : si la date est un samedi ou un week-end, ou encore le 1/1 ou le
	25/12, le jour de semaine qui précéde est retourné
&EN 2 : si la date est un samedi ou un week-end, ou encore le 1/1 ou le
	25/12, le jour de semaine qui précéde est retourné sauf si il ne
	fait pas partie du méme mois que la date d'origine. Dans ce cas,
	la régle du cas 3 est utilisé.
&EN 3 : si la date est un samedi ou un week-end, ou encore le 1/1 ou le
	25/12, le jour de semaine qui suit est retourné
&EN 4 : si la date est un samedi ou un week-end, ou encore le 1/1 ou le
	25/12, le jour de semaine qui suit est retourné sauf si il ne
	fait pas partie du méme mois que la date d'origine. Dans ce cas,
	la régle du cas 1 est utilisé.
&RT la date modifiée (éventuellement) sous le format YYYYMMDD
&EX
    DT_bus(19910101, 0) vaut 19910101
    DT_bus(19910101, 1) vaut 19901231
    DT_bus(19910101, 2) vaut 19910102
    DT_bus(19910101, 3) vaut 19910102
    DT_bus(19910101, 4) vaut 19910102
&TX
&SA DT_leave()
======================================================================= */
long DT_bus(date, dayr)
long    date;
int     dayr;
{
    long    ndate1, date1, incr;

ag:
    switch(dayr) {
	case 0: return(date);
	case 1:
	case 2: incr = -1; break;
	case 3:
	case 4: incr = 1; break;
	}

    date1 = date;
    ndate1 = DT_date_num(date1);
    while(DT_leave(date1)) {
	ndate1 += incr;
	date1 = DT_num_date(ndate1);
	if(!DT_eq_month(date, date1)) {
	    switch(dayr) {
		case 2 : dayr = 3; goto ag;
		case 4 : dayr = 1; goto ag;
		default : break;
		}
	    }
	}
    return(date1);
}


/* ====================================================================
Indique si une date correspond à un jour de travail ou non. Cette
fonction peut étre modifiée par l'utilisateur pour répondre à ses
contraintes. Elles est utilisée dans DT_bus().
&RT 1 si le jour est férié, 0 sinon.
&SA DT_bus()
======================================================================= */
DT_leave(date)
long    date;
{
    long    d[3];

    if(DT_day_w(date) > 5) return(1);
    DT_dmy(date, d);
    if(d[1] ==  1 && d[0] ==  1) return(1);
    if(d[1] == 12 && d[0] == 25) return(1);
    return(0);
}
/* ====================================================================
Retourne la différence de mois entre deux dates (sans tenir compte des
jours).
&RT le nombre de mois de différence
&EX
    DT_diff_months(19900201, 19900131) vaut 1
    DT_diff_months(19900201, 19910228) vaut -12
&TX
&SA DT_diff_dates
----------------------------------------------------------------------- */

DT_diff_months(d1, d2)
long    d1, d2;
{
    long    vd1[3], vd2[3];

    DT_dmy(d1, vd1);
    DT_dmy(d2, vd2);
    return((vd1[2] - vd2[2]) * 12 + vd1[1] - vd2[1]);
}

/* ====================================================================
Retourne le nombre de jours écoulés entre deux dates (YYYYMMDD).
&EX
    DT_diff_dates(19900201L, 19900131L) vaut 1
&TX
&SA DT_diff_months()
----------------------------------------------------------------------- */
long DT_diff_dates(d1, d2)
long    d1, d2;
{

    return(DT_date_num(d1) - DT_date_num(d2));
}

/*NH*/
int DT_tsf_30(vd1)
long    vd1[3];
{
    if(vd1[1] == 2) {
	if(DT_bix(1900L + vd1[2])) {
	    if(vd1[0] == 29) vd1[0] = 30;
	    }

	else if(vd1[0] == 28) vd1[0] = 30;
	}
    else
	vd1[0] = min(30, vd1[0]);

    return(0);
}

/* ====================================================================
Retourne le nombre de jours entre deux dates dans un calendrier 30 jours
par mois. Dans ce type de calendrier, tous les mois ont 30 jours, y
compris février et tout se passe comme si le 31 n'existait pas et était
remplacé par le 30. De méme le 28 février est compté comme 30éme jour,
sauf les années bissextiles oé c'est le 29 février qui est compté comme
300éme jour.
&RT Le nombre de jour (long)
&EX
    DT_diff_30(19900201L, 19900130L)  vaut 1 et non 2
    DT_diff_30(19900101L, 19910101L)  vaut 360
&TX
&SA
----------------------------------------------------------------------- */
long DT_diff_30(d2, d1)
long    d2, d1;
{
    long    vd1[3], vd2[3], days;

    DT_dmy(d2, vd2);
    DT_dmy(d1, vd1);
    DT_tsf_30(vd1);
    DT_tsf_30(vd2);

    days = (DT_diff_months(d2, d1) - 1) * 30L + 30L - vd1[0] + vd2[0];
    return(days);
}


/* ====================================================================
Transforme une date "normale" (format YYYYMMDD) en format julien
(YYYYDDD) oé 1990001 représente le 1er janvier 1990.

&EX
    DT_dat_julian(19900101L) vaut 1990001L
&TX
&SA DT_julian_date()
----------------------------------------------------------------------- */

long DT_date_julian(date)
long    date;
{
    long    vd[3], jul;

    jul = DT_date_num(date);
    DT_dmy(date, vd);
    vd[0] = vd[1] = 1;
    date = DT_dmy_date(vd);
    jul += 1L - DT_date_num(date);
    return((1900L + vd[2]) * 1000L + jul);
}

/* ====================================================================
Transforme une date julienne (YYYYDDD) en date normale (YYYYMMDD).

&EX
    DT_dat_julian(1990365L) vaut 19901231L
&TX
&SA DT_date_julian()
----------------------------------------------------------------------- */

long DT_julian_date(jul)
long    jul;
{
    long    tmp, date;

    date = 10000L * (jul / 1000L) + 101L;
    tmp = DT_date_num(date);
    tmp += jul % 1000L - 1;
    return(DT_num_date(tmp));
}
/*NH*/
DT_nb_2902(d1, d2)
long    d1, d2;
{
    int     i = 0;
    long    vd1[3], vd2[3];

    DT_dmy(d1, vd1);
    DT_dmy(d2, vd2);

    if(DT_bix(1900L + vd1[2]) && vd1[1] >= 3)          i--;
    if(DT_bix(1900L + vd2[2]) &&
	(vd2[1] == 1 || (vd2[1] == 2 && vd2[0] < 29))) i--;

    while(vd1[2] <= vd2[2]) {
	if(DT_bix(1900L + vd1[2])) i++;
	vd1[2] ++;
	}
    return(i);
}
/*NH*/
DT_nb_n2902(d1, d2)
long    d1, d2;
{
    int     i = 0;
    long    vd1[3], vd2[3];

    DT_dmy(d1, vd1);
    DT_dmy(d2, vd2);

    if(!DT_bix(1900L + vd1[2]) && vd1[1] >= 3)          i--;
    if(!DT_bix(1900L + vd2[2]) &&
	(vd2[1] == 1 || (vd2[1] == 2 && vd2[0] < 29)))  i--;

    while(vd1[2] <= vd2[2]) {
	if(!DT_bix(1900L + vd1[2])) i++;
	vd1[2] ++;
	}
    return(i);
}


/* ====================================================================
Retourne le nombre de jour du mois dans lequel se situe une date.
&EX
    DT_month_days(19900215L) vaut 28
&TX
----------------------------------------------------------------------- */

DT_month_days(date)
long    date;
{
    long    d[3];

    DT_dmy(date, d);
    d[0] = 1;
    date = DT_dmy_date(d);
    return((int)DT_diff_dates(DT_add_months(date, 1), date));
}


/* ====================================================================
Retourne le nombre de secondes entre le 1/1/1980 et dt + tim

&EX
    cursec = DT_secs_1180(SCR_current_date(), SCR_current_time());
&TX
----------------------------------------------------------------------- */
long DT_secs_1180(dt, tim)
long    dt, tim;
{
    long    l;

    l = DT_date_num(dt) - DT_date_num(19800101L);
    l *= 3600L * 24L;
    l += tim % 100;
    l += ((tim % 10000L) / 100L) * 60L;
    l += (tim / 10000L) * 3600L;
    return(l);
}


/* ====================================================================
Retourne le nombre de secondes écoulées depuis le 1/1/1980.

&EX
    cursec = DT_secs_1180(SCR_current_date(), SCR_current_time());
&TX
----------------------------------------------------------------------- */

long DT_current_sec()
{
    return(DT_secs_1180(_SCR_current_date(), _SCR_current_time()));
}

/* ====================================================================
Retourne le nombre de secondes entre le 1/1/2000 et dt + tim

&EX
    cursec = DT_secs_2000(SCR_current_date(), SCR_current_time());
&TX
----------------------------------------------------------------------- */
long DT_secs_1100(dt, tim)
long    dt, tim;
{
    long    l;

    l = DT_date_num(dt) - DT_date_num(20000101L);
    l *= 3600L * 24L;
    l += tim % 100;
    l += ((tim % 10000L) / 100L) * 60L;
    l += (tim / 10000L) * 3600L;
    return(l);
}


/* ====================================================================
Retourne le nombre de secondes écoulées depuis le 1/1/2000

----------------------------------------------------------------------- */

long DT_current_sec_2000()
{
    return(DT_secs_1100(_SCR_current_date(), _SCR_current_time()));
}



/* ====================================================================
Retourne le numéro de la semaine d'une date donnée (ISO-8601).

Régles :
&EN Les semaines vont de lundi à dimanche.
&EN La semaine numéro 1 est celle qui contient le premier jeudi de l'année. Elle
peut donc commencer l'année précédente.
&EN Week 0 : les premiers jours peuvent étre de la semaine 0, ce qui signifie qu'ils sont
considérés de la 53e semaine de l'année précédente.
&EN Week 53 : la semaine 53 peut étre soit réellement la 53e, soit la premiére de l'année suivante
&EN Le numéro de la semaine est <= 53  -> à corriger
&EN L'existence d'une semaine 53 dépend de la position du premier jeudi de
l'année suivante.

----------------------------------------------------------------------- */

DT_week_number(long date)
{
    long    jday, wday1, w;

    // day_nb = numéro du jour dans l'année (via julian date)
    jday = DT_date_julian(date) % 1000;

    // wday1 = jour de la semaine du 1er janvier
    wday1 = DT_day_w(101 + 10000 * (date / 10000));

    switch(wday1) {
	case 1: w = (jday - 1) / 7 + 1; break;  // lu
	case 2: w = (jday - 0) / 7 + 1; break;  // ma
	case 3: w = (jday + 1) / 7 + 1; break;  // me
	case 4: w = (jday + 2) / 7 + 1; break;  // je
	case 5: w = (jday + 3) / 7; break;      // ve
	case 6: w = (jday + 4) / 7; break;      // sa
	case 7: w = (jday + 5) / 7; break;      // di
	}
   return(w);
}

/* Conneries écrites par BP... (en Suisse pour compter des secondes ????) */

int TM_3(long hr, long *h3)
{
    h3[2] = hr % 100;
    h3[1] = (hr % 10000L) / 100L;
    h3[0] = (hr / 10000L);

    return(0);
}

long TM_nb_secs(long hr)
{
    long    l[3];

    TM_3(hr, l);
    return(l[0] * 3600L + l[1] * 60L + l[2]);
}

long TM_add_mins(long hr, long nb)
{
    long    l[3];

    TM_3(hr, l);
    l[1] += nb;
    while(l[1] > 59) {
	l[1] -= 60;
	l[0] ++;
    }
    if(l[0] > 23L) l[0] = 0L;
    return(l[0] * 10000L + l[1] * 100L + l[0]);
}

long TM_add_secs(long hr, long nb)
{
    long    l[3];

    TM_3(hr, l);
    l[2] += nb;
    while(l[2] > 59) {
	l[2] -= 60;
	l[1] ++;
    }
    while(l[1] > 59) {
	l[1] -= 60;
	l[0] ++;
    }
    if(l[0] > 23L) l[0] = 0L;
    return(l[0] * 10000L + l[1] * 100L + l[0]);
}

long TM_diff_secs(long tm1, long tm2)
{
    long    l1, l2;

    l1 = DT_secs_1100(20000101, tm1);
    l2 = DT_secs_1100(20000101, tm2);

    return(l2 - l1);
}








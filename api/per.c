#include "iode.h"

//extern   char  *SW_nalloc(); // JMP 23/8/2012

/* PERIODICITY VARS */

char    L_PERIOD_CH[] = "YSQMWD";
int     L_PERIOD_NB[] = {1, 2, 4, 12, 52, 365};


/*NH*/
/* ==+== L_POS ==========================
    Compute position of ch in string
   ====================================== */
int L_pos(str, ch)
char    *str;
int     ch;
{
    int     i;

    for(i = 0 ; str[i] != 0 ; i++)
        if(str[i] == ch) return(i);
    return(-1);
}

/*
    Gives you the nbr of periods in one time cycle
    (eg: 1 for yearly, 4 for quarterly, ....)
*/
int PER_nb(ch)
int ch;
{
    int pos;

    if(ch >= 'a') ch += 'A' -'a';
    pos = L_pos(L_PERIOD_CH, ch);
    if (pos < 0) return(-1);
    return(L_PERIOD_NB[pos]);
}

/*
    Gives you the difference in passed timespace
    of two given periods
*/
int PER_diff_per(p1, p2)
PERIOD  *p1, *p2;
{
    int     nb;

    if(p1->p_p != p2->p_p) {
        B_seterrn(85);
        return(-1);
    }
    nb = L_PERIOD_NB[L_pos(L_PERIOD_CH, p1->p_p)];
    nb = nb * (p1->p_y - p2->p_y) + p1->p_s - p2->p_s;

    return(nb);
}

/*
    Adds a nbr of timespace to a given period

    It returns a period
*/
PERIOD  *PER_addper(period, shift)
PERIOD  *period;
int     shift;
{
    int     nb_per = L_PERIOD_NB[L_pos(L_PERIOD_CH, period->p_p)],
            shiftper, backw;
    static  PERIOD  per;

    memcpy(&per, period, sizeof(PERIOD));
    if(shift >= 0) {
        per.p_y += (per.p_s + shift - 1) / nb_per;
        per.p_s  =  1 + (per.p_s + shift - 1) % nb_per;
    }
    else {
        shiftper = per.p_s + shift;
        if(shiftper <= 0) { // année recule
            backw = 1 + (-shiftper) / nb_per;
            per.p_y -= backw;
            per.p_s = backw * nb_per + shiftper;
        }
        else { // année inchangée
            per.p_s = shiftper;
        }
    }
    return(&per);
}

/*
    Gives you the representation of a period in text
*/

char *PER_pertoa(per, text)
PERIOD  *per;
char    *text;
{
    static char buf[10];

    if(per->p_y == 0 || per->p_p == 0 || per->p_s == 0)
        buf[0] = 0;
    else
        sprintf(buf, "%ld%c%ld", per->p_y, per->p_p, per->p_s); /* PERIOD LONG */
    if(text == 0) return(buf);

    strcpy(text, buf);
    return(text);
}

/*
    Interprets text as description of a period, and
    returns the so obtained period
*/
PERIOD *PER_atoper(text)
char    *text;
{
    int     i, j, nb_per, year, sub;
    PERIOD  *per = (PERIOD *) SW_nalloc(sizeof(PERIOD));

    per->p_y = per->p_p = per->p_s = 0;

    for(i = 0 ; i < 4 ; i++)
        if(!SCR_is_num(text[i])) break;
    if(i == 0) goto fin;

    nb_per = L_pos(L_PERIOD_CH, toupper(text[i]));
    if(nb_per < 0) goto fin;

    text[i] = 0;
    year = atoi(text);
    if(year < 50) year += 2000;
    else if(year < 200) year += 1900;
    text[i] = L_PERIOD_CH[nb_per];

    i++;
    j = i;
    while(SCR_is_num(text[i])) i++;
    text[i] = 0;
    sub = atoi(text + j);
    if(sub <= 0) sub = 1;                   /* JMP 03-03-2004 */
    if(L_PERIOD_NB[nb_per] < sub) goto fin;

    per->p_y = year;
    per->p_p = L_PERIOD_CH[nb_per];
    per->p_s = sub;

fin:
    if(per->p_y == 0) {
        B_seterrn(86, text);
        SW_nfree(per);
        per = 0;
    }
    return(per);
}

/*
    Calculates the common sample of two given samples
*/
PER_common_smpl(smp1, smp2, res)
SAMPLE  *smp1, *smp2, *res;
{
    if((smp1->s_p1).p_p != (smp2->s_p1).p_p) {
        B_seterrn(85);
        return(-1);
    }

    if(PER_diff_per(&(smp1->s_p1), &(smp2->s_p1)) < 0)
        res->s_p1 = smp2->s_p1;
    else
        res->s_p1 = smp1->s_p1;

    if(PER_diff_per(&(smp1->s_p2), &(smp2->s_p2)) > 0)
        res->s_p2 = smp2->s_p2;
    else
        res->s_p2 = smp1->s_p2;

    if((res->s_nb = PER_diff_per(&(res->s_p2), &(res->s_p1))) < 0) {
        B_seterrn(87);
        return(-1);
    }

    res->s_nb += 1;
    return(0);
}

/*
  Returns sample build with two given strings,
  it does the allocation of the sample
*/
SAMPLE  *PER_atosmpl(a1, a2)
char    *a1, *a2;
{
    SAMPLE  *smpl = 0;
    PERIOD  *p1, *p2 = 0;

    p1 = PER_atoper(a1);
    if(p1 == 0) goto err;
    p2 = PER_atoper(a2);
    if(p2 == 0) goto err;

    smpl = PER_pertosmpl(p1, p2);

err:
    if(smpl == 0) B_seterrn(88, a1, a2);
    SW_nfree(p1);
    SW_nfree(p2);
    return(smpl);
}

SAMPLE  *PER_pertosmpl(p1, p2)
PERIOD  *p1, *p2;
{
    SAMPLE  *smpl = (SAMPLE *) SW_nalloc(sizeof(SAMPLE));
    int     dif;

    memcpy(&(smpl->s_p1), p1, sizeof(PERIOD));
    memcpy(&(smpl->s_p2), p2, sizeof(PERIOD));
    dif = PER_diff_per(&(smpl->s_p2), &(smpl->s_p1));
    if(dif < 0) goto err;

    smpl->s_nb = dif + 1;
    goto fin;

err:
    SW_nfree(smpl);
    smpl = 0;
fin:
    return(smpl);
}

/*
    Converts the sample into a string
*/

char *PER_smpltoa(smpl, text)
SAMPLE  *smpl;
char    *text;
{
    char    b1[20], b2[20];

    sprintf(text, "%s:%s", PER_pertoa(&(smpl->s_p1), b1),
            PER_pertoa(&(smpl->s_p2), b2) );
    return(text);
}


int PER_nbper(period)
PERIOD  *period;
{
    return(PER_nb(period->p_p));
}

IODE_REAL PER_per2real(from, i)
PERIOD  *from;
int     i;
{
    int     per;
    IODE_REAL    value;

    per = PER_nbper(from);
    value = (IODE_REAL) from->p_y + (from->p_s + i - 1)/per
            + ((from->p_s + i - 1) % per)/ (IODE_REAL) per;

    return(value);
}
/* JMP 22/11/2011 report‚ dans a2mhtm
char *PER_real2a(rper, nbper)
 IODE_REAL    rper;
int     nbper;
{
    int     sper, year, i;
    double  floor();
    static char buf[15];

    year = floor(rper + 0.001);
    if(nbper == 1) sprintf(buf, "%d", year);
    else {
	sper = 1.2 + ((double) nbper) * (rper - year);
	sprintf(buf, "%d:%d", year, sper);
	}

    return(buf);
}
*/








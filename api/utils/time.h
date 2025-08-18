#pragma once

#include "api/constants.h"

/*------------------------ STRUCTS ----------------------- */

struct PERIOD 
{
    long    p_y;        // year
    long    p_s;        // position in the year (according to the periodicity)
    char    p_p;        // periodicity (Y S Q M W D)
    char    p_pad[3];       
};

struct SAMPLE 
{
    PERIOD  s_p1;       // starting period
    PERIOD  s_p2;       // ending period
    short   s_nb;       // number of periods in the sample
    char    s_pad[2];
};

/*------------------------ GLOBALS ----------------------- */

/**
 *  Variables for period definitions: 
 *      L_PERIOD_CH : list of valid periodicities :
 *          Y = Yearly
 *          S = Half-Yearly
 *          Q = Quaterly
 *          M = Monthly
 *          W = Weekly
 *          D = Daily
 *         
 *      L_PERIOD_NB : number of periods in 1 year for each periodicity (in the same defn order).
 */

inline char L_PERIOD_CH[] = "YSQMWD";
inline int  L_PERIOD_NB[] = {1, 2, 4, 12, 52, 365};

/*------------------------ FUNCS ----------------------- */

/* per.c */
int L_pos(char *,int );
int PER_nb(int );
int PER_diff_per(PERIOD *,PERIOD *);
PERIOD *PER_addper(PERIOD *,int );
char *PER_pertoa(PERIOD *,char *);
PERIOD *PER_atoper(char *);
int PER_common_smpl(SAMPLE *,SAMPLE *,SAMPLE *);
SAMPLE *PER_atosmpl(char *,char *);
SAMPLE *PER_pertosmpl(PERIOD *,PERIOD *);
char *PER_smpltoa(SAMPLE *,char *);
int PER_nbper(PERIOD *);
double PER_per2real(PERIOD *,int );
char *PER_real2a(double ,int );

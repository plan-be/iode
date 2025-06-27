#pragma once

#include "api/constants.h"

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------ STRUCTS ----------------------- */

typedef struct _period {
    long    p_y;        // year
    long    p_s;        // position in the year (according to the periodicity)
    char    p_p;        // periodicity (Y S Q M W D)
    char    p_pad[3];       
} PERIOD;

typedef struct _sample {
    PERIOD  s_p1;       // starting period
    PERIOD  s_p2;       // ending period
    short   s_nb;       // number of periods in the sample
    char    s_pad[2];
} SAMPLE;

/*------------------------ GLOBALS ----------------------- */

extern char    L_PERIOD_CH[];
extern int     L_PERIOD_NB[];

/*------------------------ FUNCS ----------------------- */

/* per.c */
extern int L_pos(char *,int );
extern int PER_nb(int );
extern int PER_diff_per(PERIOD *,PERIOD *);
extern PERIOD *PER_addper(PERIOD *,int );
extern char *PER_pertoa(PERIOD *,char *);
extern PERIOD *PER_atoper(char *);
extern int PER_common_smpl(SAMPLE *,SAMPLE *,SAMPLE *);
extern SAMPLE *PER_atosmpl(char *,char *);
extern SAMPLE *PER_pertosmpl(PERIOD *,PERIOD *);
extern char *PER_smpltoa(SAMPLE *,char *);
extern int PER_nbper(PERIOD *);
extern double PER_per2real(PERIOD *,int );
extern char *PER_real2a(double ,int );

#ifdef __cplusplus
}
#endif

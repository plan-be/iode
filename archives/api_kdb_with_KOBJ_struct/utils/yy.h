#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"

/* yy.c */
double K_read_real(YYFILE *);
long K_read_long(YYFILE *);
char *K_read_str(YYFILE *);
Period *K_read_per(YYFILE *);
Sample *K_read_smpl(YYFILE *);
int K_read_align(YYFILE* yy);
void K_stracpy(char **,char *);
int K_wrdef(FILE *,YYKEYS *,int );
int K_compare(YYKEYS *,YYKEYS *);
char *K_wrap(char *,int );

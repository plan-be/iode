#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/time/period.h"
#include "api/time/sample.h"

/* yy.c */
double K_read_real(YYFILE* yy);
long K_read_long(YYFILE* yy);
char* K_read_str(YYFILE* yy);
Period* K_read_per(YYFILE* yy);
Sample* K_read_smpl(YYFILE* yy);
int K_read_align(YYFILE* yy);
void K_stracpy(char** to, char* from);
int K_wrdef(FILE* fd, YYKEYS* table, int def);
int K_compare(YYKEYS* yy1, YYKEYS* yy2);
char* K_wrap(char* in, int lg);

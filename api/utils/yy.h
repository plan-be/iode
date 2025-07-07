#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/utils/time.h"

#ifdef __cplusplus
extern "C" {
#endif

/* yy.c */
extern double K_read_real(YYFILE *);
extern long K_read_long(YYFILE *);
extern char *K_read_str(YYFILE *);
extern PERIOD *K_read_per(YYFILE *);
extern SAMPLE *K_read_smpl(YYFILE *);
extern int K_read_align(YYFILE* yy);
extern void K_stracpy(char **,char *);
extern int K_wrdef(FILE *,YYKEYS *,int );
extern int K_compare(YYKEYS *,YYKEYS *);
extern char *K_wrap(char *,int );

#ifdef __cplusplus
}
#endif

#ifndef _S_CPP_H_
#define _S_CPP_H_

#include <stdio.h>
#include <stdlib.h>
#include <s_strs.h>

typedef struct _cppfile_ {
    U_ch    *filename;       /* File Name */
    U_ch    *rec;            /* Record string */
    int     rec_nb,          /* nb rec chars */           /* JMP 10-12-97 */
	    rec_max;         /* Max size of rec */        /* JMP 10-12-97 */
    int     cline,           /* Current line */           /* JMP 10-12-97 */
	    ccol;            /* Current col */            /* JMP 10-12-97 */
    struct _cppfile_ *incl;  /* include file */
    int     prev;            /* last char read */
    int     indef;           /* reading define */
    int     iflevel;         /* ifdef current level */
    int     skip;            /* skip characters (ifdef) */
    FILE    *fd;
    int     type;            /* 0=file, 1=stdin, 2=string */
} CPPFILE;

extern int  CPP_DIRCH, CPP_DEFCH, CPP_EOLCH;

/* FUNCTIONS */

#ifndef SCRPROTO

CPPFILE *CppOpen();
CPPFILE *CppCurrent();
U_ch    *CppReadLine();
U_ch    *CppReadToChars();
double  CppReadDbl();
U_ch    *CppReadQuotedString();
long    CppReadLong();
char    *CppPos();

#else
#include <s_procpp.h>
#endif

#endif





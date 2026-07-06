#ifndef _ARGS_
#define _ARGS_

#include <stdio.h>
//#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct   _adef_ {
    char    *a_arg;         /* Argument */
    char    *a_args;        /* Paramètres (texte libre, 9 caractères) */
    char    *a_desc;        /* Description (texte libre, 15 caractères) */
    int     a_req,          /* Requis (1) ou non (0) */
	    a_min,          /* Nombre min de paramètres */
	    a_max;          /* Nombre max de paramètres */
    int    (*a_check)(int, char **);
    int    (*a_exec)(int, char **);
    char   *a_dft;          /* Valeur par défaut */
} ADEF;

/* VARIABLES DECLARATION */

extern  int     A_NB_VAL;
extern  char    **A_VAL;
extern  char    *A_SEPS;
extern   int    A_NO_EXPANDSTAR_DFT;
extern  char    *(*A_expand_super)(char*); // JMP 16/01/2022

#ifdef __cplusplus
}
#endif

/* FUNCTIONS DECLARATION */

#ifdef __cplusplus
extern "C" {
int A_cerror(char *, ...);
}
#else /* __cplusplus */
extern int A_cerror(char *, ...);
#endif /* __cplusplus */

#include <s_proarg.h>

#endif /* _ARGS_ */





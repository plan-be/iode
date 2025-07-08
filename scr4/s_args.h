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
#ifdef SCRPROTO
    int    (*a_check)(int, char **);
    int    (*a_exec)(int, char **);
#else
    int    (*a_check)();    /* Fonction de vérification (A_check) */
    int    (*a_exec)();     /* Fonction d'exécution (A_exec) */
#endif
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

#ifndef SCRPROTO

#ifdef __cplusplus
extern "C" {
#endif

extern  char    **A_init();
extern  char    **A_initv();

extern  char    *A_expand();

extern  int     A_check();
extern  int     A_find();
extern  int     A_nb();
extern  int     A_free();
extern  int     A_exec();
extern  int     A_check_int();
extern  int     A_check_access();

#ifdef __cplusplus
}
#endif

#else

#ifdef __cplusplus
extern "C" {
int A_cerror(char *, ...);
}
#else /* __cplusplus */
extern int A_cerror(char *, ...);
#endif /* __cplusplus */

#include <s_proarg.h>

#endif /* SCRPROTO */

#endif /* _ARGS_ */





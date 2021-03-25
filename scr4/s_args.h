#ifndef _ARGS_
#define _ARGS_

#include <stdio.h>
//#include <ctype.h>

typedef struct   _adef_ {
    char    *a_arg;         /* Argument */
    char    *a_args;        /* Param�tres (texte libre, 9 caract�res) */
    char    *a_desc;        /* Description (texte libre, 15 caract�res) */
    int     a_req,          /* Requis (1) ou non (0) */
	    a_min,          /* Nombre min de param�tres */
	    a_max;          /* Nombre max de param�tres */
#ifdef SCRPROTO
    int    (*a_check)(int, char **);
    int    (*a_exec)(int, char **);
#else
    int    (*a_check)();    /* Fonction de v�rification (A_check) */
    int    (*a_exec)();     /* Fonction d'ex�cution (A_exec) */
#endif
    char   *a_dft;          /* Valeur par d�faut */
} ADEF;

/* VARIABLES DECLARATION */

extern  int     A_NB_VAL;
extern  char    **A_VAL;
extern  char    *A_SEPS;
extern   int    A_NO_EXPANDSTAR_DFT;

/* FUNCTIONS DECLARATION */

#ifndef SCRPROTO
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
#else

#ifdef SCRCPP
extern "C" {
int A_cerror(char *, ...);
}
#else /* SCRCPP */
extern int A_cerror(char *, ...);
#endif /* SCRCPP */

#include <s_proarg.h>

#endif /* SCRPROTO */

#endif /* _ARGS_ */





#include "s_mat.h"


/* ====================================================================
Gestion des messages d'erreur: la fonction retourne un pointeur sur
le dernier message d'erreur désigné par la variable globale M_errno.
La variable globale M_errno est ensuite réinitialisée à 0 (no error).

&RT Pointeur char* sur le dernier message d'erreur.

&EX
    MAT     *m1;
    int     nl = 100, nc = 100;

    m1 = M_alloc(nl, nc);
    if(M_errno != 0) {
	fprintf(stderr, "Message: ", M_error());
	exit(1);
	}
&TX
======================================================================= */

char *M_error()
{
    static char msg[256];
    extern char  *M_ERR_TEXT[];

    msg[0] = 0;
    if(M_errno <= 0) return(msg);
    sprintf(msg, "%s", M_ERR_TEXT[M_errno-1]);
    M_errno=0;
    return(msg);
}








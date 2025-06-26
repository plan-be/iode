//#include "scr4.h"
//#include <s_swap.h>
#include <string.h>
#include <stdio.h>
#include <s_swap.h>
#ifdef DOS
#include <io.h>
#include <sys\stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>

#ifdef POWERPC
#include <fcntl.h>
#else
#include <sys/fcntl.h>
#endif

#endif

#include <stdlib.h>


/*==================================================================
Fonction appelée en cas d'erreur de swap. code est un numéro
d'erreur et text est un message indiquant le type d'erreur.

Cette fonction peut être replacée par une fonction utilisateur,
permettant par exemple de sauver le travail en cours avant de
quitter le système.

La fonction de librairie effectue les opérations suivantes :

&CO
    fprintf(stderr, "\nSwap panic : %s\n", text);
    SW_end();
    exit(code);
&TX
=============================================================== */

SW_abort(code, text)
int     code;
char    *text;
{
    Wprintf("\nSwap panic : %s\n", text); /* JMP 16-02-97 */
    SW_end();
    exit(code);
    return(0);
}



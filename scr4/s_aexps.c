#include <string.h>
#include "s_args.h"

//#if defined(DOS) && !defined(VC) /* JMP 14-02-17 */
//#include <dos.h>

int A_NO_EXPANDSTAR_DFT = 0; /* JMP 12-11-2012 */

/* ==================================================================
Fonction appel‚e par A_init() et A_initv() pour remplacer les noms
contenant une ‚toile (*) en DOS.

Lorsque la fonction d'initialisation des paramŠtres en rencontre un
contenant *, elle appelle la fonction A_expandstar() qui doit
se charger d'ajouter … la liste des arguments tous ceux correspondant.

Par d‚faut, la fonction A_expandstar() ajoute tous les fichiers
correspondant au masque … l'aide de la fonction A_add1(arg).

Si la fonction retourne 0, l'argument a ‚t‚ trait‚. Si la fonction
retourne une autre valeur, l'argument n'a pas ‚t‚ trait‚ et est
conserv‚ tel quel dans le tableau A_VAL r‚sultat.

Cette fonction peut ˆtre remplac‚e par une fonction utilisateur.
Pour faciliter le travail, la fonction A_expandstar_dft()
peut ˆtre appel‚e: elle effectue le travail par d‚faut de
A_expandstar().

&IT A_NO_EXPANDSTAR_DFT

Si la variable globale A_NO_EXPANDSTAR_DFT est non nulle, il n'y a oas d'appel à la fonction par défaut
A_expandstar_dft().

&SA A_init(), A_initv(), A_expand(), A_iexpand(), A_expandstar_dft()
==================================================================== */

A_expandstar(arg)
char    *arg;
{
    if(A_NO_EXPANDSTAR_DFT == 0)        /* JMP 12-11-2012 */
        return(A_expandstar_dft(arg));  /* JMP 10-03-98 */
    else
        return(-1);                     /* JMP 12-11-2012 */
}

//#endif


#include <string.h>
#include "s_args.h"

//#if defined(DOS) && !defined(VC) /* JMP 14-02-17 */
//#include <dos.h>

int A_NO_EXPANDSTAR_DFT = 0; /* JMP 12-11-2012 */

/* ==================================================================
Fonction appelée par A_init() et A_initv() pour remplacer les noms
contenant une étoile (*) en DOS.

Lorsque la fonction d'initialisation des paramètres en rencontre un
contenant *, elle appelle la fonction A_expandstar() qui doit
se charger d'ajouter à la liste des arguments tous ceux correspondant.

Par défaut, la fonction A_expandstar() ajoute tous les fichiers
correspondant au masque à l'aide de la fonction A_add1(arg).

Si la fonction retourne 0, l'argument a été traité. Si la fonction
retourne une autre valeur, l'argument n'a pas été traité et est
conservé tel quel dans le tableau A_VAL résultat.

Cette fonction peut être remplacée par une fonction utilisateur.
Pour faciliter le travail, la fonction A_expandstar_dft()
peut être appelée: elle effectue le travail par défaut de
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


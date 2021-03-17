#include "s_args.h"

/*
     Affiche le message d'erreur en provenance d'une des fonctions de la
     librairie. Cette fonction peut ˆtre remplac‚e par une fonction
     utilisateur pour peu que l'on garde la mˆme syntaxe.

    Il suffit pour ce faire de d‚finir la fonction dans une des sources
    C. Lors du link, comme la fonction apparaŒtra dans un module objet
    avant la librairie, elle aura priorit‚ et remplacera donc la
    fonction par d‚faut de la librairie.

    La fonction de la librairie se contente d'imprimer dans le stderr le
    message pass‚ comme argument.

&EX
    A_error(txt)
    char    *txt;
    {
	PG_display_error("Args : %s", txt);
    }
&TX
*/

int A_error(txt)
char    *txt;
{
    fprintf(stderr, "%s\n", txt);
    return(0);
}

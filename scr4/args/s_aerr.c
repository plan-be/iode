#include "s_args.h"

/*
     Affiche le message d'erreur en provenance d'une des fonctions de la
     librairie. Cette fonction peut être remplacée par une fonction
     utilisateur pour peu que l'on garde la même syntaxe.

    Il suffit pour ce faire de définir la fonction dans une des sources
    C. Lors du link, comme la fonction apparaîtra dans un module objet
    avant la librairie, elle aura priorité et remplacera donc la
    fonction par défaut de la librairie.

    La fonction de la librairie se contente d'imprimer dans le stderr le
    message passé comme argument.

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

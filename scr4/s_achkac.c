#ifndef UNIX
#include <io.h>
#endif
#include "s_args.h"

/* ===================================================================
    Vérifie que les strings passés dans argv sont tous des noms de
    fichiers existant.
    Cette fonction peut être utilisée dans la définition d'une structure
    ADEF à passer comme argument à A_check().

&SA A_check()
======================================================================*/

A_check_access(argc, argv)
int     argc;
char    *argv[];
{
    int     i;

    for(i = 1 ; i < argc ; i++)
	if(access(argv[i], 0)) return(-1);
    return(0);
}


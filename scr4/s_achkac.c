#ifndef UNIX
#include <io.h>
#endif
#include "s_args.h"

/* ===================================================================
    V‚rifie que les strings pass‚s dans argv sont tous des noms de
    fichiers existant.
    Cette fonction peut ˆtre utilis‚e dans la d‚finition d'une structure
    ADEF … passer comme argument … A_check().

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


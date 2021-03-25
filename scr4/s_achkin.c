#include "s_args.h"

/* ======================================================================
    V�rifie que les strings pass�s dans argv sont tous des entiers.
    Cette fonction peut �tre utilis�e dans la d�finition d'une structure
    ADEF � passer comme argument � A_check().

&SA A_check()
=========================================================================*/

A_check_int(argc, argv)
int     argc;
char    *argv[];
{
    int     i;

    for(i = 1 ; i < argc ; i++)
	if(A_check_int_1(argv[i])) return(-1);
    return(0);
}
/*NH*/

A_check_int_1(parm)
char    *parm;
{
    int     i;

    for(i = 0 ; parm[i] ; i++)
	if(parm[i] < '0' || parm[i] > '9') return(-1);
    return(0);
}

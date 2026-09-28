#include <string.h>
#include "scr4/strs/s_strs.h"
#include "s_args.h"


/*NH*/
/* ======================================================================
    Recherche un string dans une table d'arguments généré par A_init()
    ou A_initv(). La position du string dans cette table est retournée si
    le string est présent. La fonction retourne -1 sinon.

&EX
    args = A_initv(argv);
    if(A_find(args, "-h") >= 0) {
	printf("Usage : ...\n");
	exit(0);
	}
&TX
======================================================================== */

int A_find(char ** args, char * string)
{
    int     i;

    if(string == NULL) return(-1);

    for(i = 0; args[i] != 0 ; i++)
	if(strcmp(args[i], string) == 0) return(i);

    return(-1);
}

/*======================================================================
    Recherche dans la liste des arguments générée par A_init() ou
    A_initv() le nombre de paramètres à partir d'une position donnée
    (calculée par exemple à l'aide de A_find()),
    c'est-à-dire le nombre de strings ne commençant pas par '-'.

    Si la position est hors des la liste des arguments, retourne -1.

&EX
	#include <s_args.h>

	int main(int argc, char *[] argv)
	{
	    int     i, nb, pos;
	    char    **args;

	    args = A_initv(argv);
	    pos = A_find(args, "-f");
	    nb  = A_nb(args, pos);
	    if(nb >= 0) {
		printf("Arguments de -f\n");
		for(i = 0 ; i < nb ; i++)
		    printf("%s\n", A_VAL[pos + i + 1]);
		}
	    else
		printf("Pas d'arguments de -f\n");
	}
&TX
&SA A_init(), A_find()
====================================================================== */

int A_nb(char ** args, int pos)
{
    int     i;

    if(pos == -1 || args[pos] == 0) return(-1);

    for(i = pos + 1; args[i] != 0 ; i++)
	if(args[i][0] == '-' && args[i][1] != 0) break;

    return(i - pos - 1);
}

/* =======================================================================
    Recherche l'argument -h dans la liste créée à l'aide de A_init() ou
    A_initv() et exécute la fonction passée comme argument si -h est
    présent.

    Si -h est présent, retourne -1, sinon retourne 0.

&EX
	#include <s_args.h>

	Usage()
	{
	    printf("Usage : ...\n");
	}

	int main(int argc, char *[] argv)
	{
	    char    **args;

	    args = A_initv(argv);
	    if(A_help(args, Usage)) exit(0);
	    ...
	}
&TX
&SA A_initv(), A_find()
======================================================================== */

#ifdef SCRPROTO
int A_help(
char    **args,
int     (*fn)(void)
)
#else
int A_help(args, fn)
char    **args;
int     (*fn)();
#endif
{
    if(A_find(args, "-h") < 0) return(0);
    (*fn)();
    return(-1);
}



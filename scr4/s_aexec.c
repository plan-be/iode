#include "s_args.h"

/* ======================================================================
    Ex‚cute les fonctions sp‚cifi‚es dans le tableau de structure ADEF.
    L'ex‚cution a lieu dans l'ordre de la table des ADEF's.

    Les fonctions ex‚cut‚es re‡oivent deux paramŠtres semblables aux
    argc et argv de la fonction main d'un programme.

&CO
	fn(argc, argv)
&TX
    Le premier ‚l‚ment du tableau argv est toujours l'argument
    d‚fini (-af par exemple). Les suivants sont ceux fournis par l'appel
    au programme. Le nombre d'arguments comprend toujours l'argument
    lui-mˆme.

    A la diff‚rence de argv du main, un pointeur NULL ne termine pas la
    liste des arguments.

    Si la ligne du tableau des ADEF's contient un pointeur NULL, aucune
    fonction n'est ex‚cut‚e.

    Si une ligne du tableau des ADEF's ne contient pas de paramŠtre (NULL),
    la fonction s'ex‚cute avec 0 argument.

    Si l'argument n'est pas pr‚sent, mais qu'il a une valeur par d‚faut,
    la fonction est ex‚cut‚e avec pour argument la valeur par d‚faut.
    S'il n'est pas pr‚sent et qu'il n'y a pas de valeur par d‚faut,
    la fonction ne s'ex‚cute pas.
======================================================================= */

int A_exec(args, adef)
char    **args;
ADEF    *adef;
{
    int i;

    for(i = 0; adef[i].a_arg != NULL; i++)
	if(A_exec1(args, adef + i) < 0) return(-1);
    return(0);
}

/*NH*/
/* ============================================================
    Executes fn_exec(argc, argv) on the arguments
    following string arg in the args list.
    If arg is not present default (when not NULL) is used
    Note: arg is always the first argument passed to the
    users exec function.
================================================================ */

int A_exec1(args, adef)
char    **args;
ADEF    *adef;
{
    int     pos;
    char    *argd[3];

    if(adef->a_exec == NULL) return(0);
    if(adef->a_arg[0] == '\0') return((*(adef->a_exec))(0, NULL));
    pos = A_find(args, adef->a_arg);
    if(pos < 0) {
	if(adef->a_dft == NULL) return(0);
	argd[0] = adef->a_arg;
	argd[1] = adef->a_dft;
	argd[2] = NULL;
	return((*(adef->a_exec))(2, argd));
    }

    return((*(adef->a_exec))(A_nb(args, pos) + 1, args + pos));
}



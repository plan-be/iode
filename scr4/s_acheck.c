#include <string.h>
#include "s_args.h"

/* =======================================================================
    Vérifie à partir d'une structure ADEF que les arguments sont corrects.
    Le tableau des arguments est normalement créé par la fonction
    A_initv() ou A_init().

    La vérification s'effectue en plusieurs étapes. Chacune est décrite
    ci-dessous.

&TI Vérification par ligne du tableau ADEF

    Chacun des arguments définis (certaines lignes peuvent ne pas avoir
    d'arugment, mais avoir du sens pour l'exécution) est vérifié comme
    suit :

&EN s'il est requis et non présent, erreur
&EN si le nombre de valeurs qui le suivent (jusqu'au prochain - ou
    jusqu'à la fin) n'est pas entre les bornes définies, erreur.
&EN si une fonction de vérification est spécifiée, elle est appelée et
    si elle retourne une valeur non nulle, erreur.

&TI Vérification qu'il n'y a pas d'argument indéfini

    La fonction vérifie ensuite que tous les arguments sont définis dans
    la table ADEF.

    En cas d'erreur, la fonction A_error est appelée avec un des messages
    suivants :

&CO
	"A_check : '-arg' : incorrect argument(s)"
	"A_check : '-arg' required, or incorrect nbr of parameters"
&TX

    De plus, le processus de vérification s'arrête et la fonction
    retourne -1. Si aucune erreur n'est détectée, la fonction retourne 0.

&TI La fonction de vérification

    A chaque argument, une fonction d'erreur peut être définie pour
    permettre d'en vérifier le type, le nombre ou tout autre
    vérification utile. Cette fonction est appelée avec comme arguments
    deux valeurs semblables à celles reçue par le programme main() : le
    nombre d'arguments et un pointeur vers le premier. Le premier
    argument est toujours du type "-arg" et est compris dans le nombre.

    Des fonctions type sont définies dans la librairie et peuvent être
    utilisées dans la définition de la structure ADEF. Leur code est
    défini ci-dessous. On peut s'en inspirer pour construire ses propres
    fonctions de check.

&TI Fonction de vérification que les paramètres sont numériques
&CO
    A_check_int(argc, argv)
    int     argc;
    char    *argv[];
    {
	int     i;

	for(i = 1 ; i < argc ; i++)
	    if(A_check_int_1(argv[i])) return(-1);
	return(0);
    }

    A_check_int_1(parm)
    char    *parm;
    {
	int     i;

	for(i = 0 ; parm[i] ; i++)
	    if(parm[i] < '0' || parm[i] > '9') return(-1);
	return(0);
    }
&TX
&TI Fonction de vérification de l'existence de fichiers
&CO
    A_check_access(argc, argv)
    int     argc;
    char    *argv[];
    {
	int     i;

	for(i = 1 ; i < argc ; i++)
	    if(access(argv[i], 0)) return(-1);
	return(0);
    }

&EX
  ADEF    adef[3] = {
    {"-pr", "printer", "printer name", 0, 1, 1,
					0, setprinter, "prn"},

    {"-l", "file ...", "filenames",    1, 1, 100,
					A_check_exist, printfiles, 0},

    {NULL, 0, 0, 0, 0, 0, NULL, NULL, NULL}
  };

  main(argc, argv)
  int     argc;
  char    *argv[];
  {
      char    **args;

      args = A_initv(argv);
      if(A_check(args, adef)) exit(1);
      ...
  }
&TX
&SA A_init(), A_help(), A_usage()
========================================================================= */

A_check(args, adef)
char    **args;
ADEF    *adef;
{
    int i;

    for(i = 0; adef[i].a_arg != NULL; i++)
	if(adef[i].a_arg[0] != 0 && A_check1(args, adef + i) < 0) return(-1);

    for(i = 1; args[i]; i++)
	if(args[i][0] == '-'  &&
	   args[i][1] != 0 &&
	   A_checkdef(adef, args[i])) return(-1);

    return(0);
}

/*NH*/
A_check1(args, adef)
char    **args;
ADEF    *adef;
{
    int pos, nb;

    pos = A_find(args, adef->a_arg);
    if(pos < 0)  {
	if(adef->a_req == 1) goto err;
	else return(0);
	}

    nb = A_nb(args, pos);
    if(nb < adef->a_min || nb > adef->a_max) goto err;

    if(adef->a_check == NULL) return(0);
    if((*adef->a_check)(nb + 1, args + pos)) {
	A_cerror("A_check : %s : incorrect argument(s)", adef->a_arg);
	return(-1);
	}
    return(0);
err :
    A_cerror("A_check : %s required, or incorrect nbr of parameters", adef->a_arg);
    return(-1);
}

/*NH*/
A_checkdef(adef, arg)
ADEF    *adef;
char    *arg;
{
    int i;

    for(i = 0; adef[i].a_arg != NULL; i++)
	if(strcmp(adef[i].a_arg, arg) == 0) return(0);

    A_cerror("A_check : %s : invalid argument", arg);
    return(-1);
}


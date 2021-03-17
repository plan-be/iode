#include "s_args.h"

/* ==================================================================
Fonction appel‚e par A_init() et A_initv() pour remplacer les macros
commen‡ant par $.

Lorsque la fonction d'initialisation des paramŠtres en rencontre un dont
la premiŠre lettre est $, elle appelle la fonction A_expand() qui doit
retourner la valeur de remplacement correspondante. Par d‚faut, la
fonction A_expand() retourne un pointeur nul, ce qui laisse inchang‚
tout paramŠtre commen‡ant par un $.

La substitution est r‚cursive jusqu'… un maximum de 10 niveaux
d'imbrication.

A la fin de la fonction d'initialisation, si un argument commence toujours
par $, un message d'erreur est envoy‚ et la fonction retourne -1;

&EX
    char *A_expand(text)
    char    *text;
    {
	switch(text[1]) {
	    case 'A' : return("$B $C $D");
	    case 'B' : return("Bernard");
	    case 'C' : return("Christian");
	    case 'D' : return("Daniel");
	    defautl : return(NULL);
	    }
    }
&TX

==================================================================== */
char    *A_expand(name)
char    *name;
{
    return(0L);
}

/* JMP38 20-09-92 nouveau fichier s_aiexp */

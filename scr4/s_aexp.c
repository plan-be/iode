#include "s_args.h"

char *(*A_expand_super)(char* );

/* ==================================================================
Fonction appelée par A_init() et A_initv() pour remplacer les macros
commençant par $. 

Par défaut, cette fonction ne fait rien. On peut implémenter une fonction alternative :
- soit en réécrivant la fonction
- soit en assignant un pointeur de fonction ) A_expand_super

Lorsque la fonction d'initialisation des paramètres en rencontre un dont
la première lettre est $, elle appelle la fonction A_expand() qui doit
retourner la valeur de remplacement correspondante. Par défaut, la
fonction A_expand() retourne un pointeur nul, ce qui laisse inchangé
tout paramètre commençant par un $.

La substitution est récursive jusqu'à un maximum de 10 niveaux
d'imbrication.

A la fin de la fonction d'initialisation, si un argument commence toujours
par $, un message d'erreur est envoyé et la fonction retourne -1;

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
    // Alternate implementation JMP 17/01/2022
    if(A_expand_super) 
        return((*A_expand_super)(name));

    return(NULL);
}

/* JMP38 20-09-92 nouveau fichier s_aiexp */

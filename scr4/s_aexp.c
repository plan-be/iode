#include "s_args.h"

char *(*A_expand_super)(char* );

/* ==================================================================
Fonction appel�e par A_init() et A_initv() pour remplacer les macros
commen�ant par $. 

Par d�faut, cette fonction ne fait rien. On peut impl�menter une fonction alternative :
- soit en r��crivant la fonction
- soit en assignant un pointeur de fonction ) A_expand_super

Lorsque la fonction d'initialisation des param�tres en rencontre un dont
la premi�re lettre est $, elle appelle la fonction A_expand() qui doit
retourner la valeur de remplacement correspondante. Par d�faut, la
fonction A_expand() retourne un pointeur nul, ce qui laisse inchang�
tout param�tre commen�ant par un $.

La substitution est r�cursive jusqu'� un maximum de 10 niveaux
d'imbrication.

A la fin de la fonction d'initialisation, si un argument commence toujours
par $, un message d'erreur est envoy� et la fonction retourne -1;

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

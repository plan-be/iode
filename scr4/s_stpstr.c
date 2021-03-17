#include "s_strs.h"

/* ====================================================================
Compte la longueur du string stripp‚ … l'aide de la fonction SCR_pstrip().
Les caractŠres … supprimer sont … sp‚cifier dans le second argument de
la fonction. Il faut mettre l'espace dans les caractŠres de ponctuation
si on d‚sire supprimer les blancs.

&EX
	SCR_pstrip_lg("Titre .......? ", ". :?")

	    vaut

	5
&TX
&RT La longueur … conserver dans le string
&SA SCR_pstrip()
======================================================================= */

SCR_pstrip_lg(buf, punct)
unsigned char *buf,
	      *punct;
{
    int     i;

    i = strlen(buf) - 1;
    for( ; i >= 0 ; i--)
	if(!U_is_in(buf[i], punct)) break;

    return(i + 1);
}

/* ====================================================================
Supprime les caractŠres de ponctuation de la fin d'un string. Les
caractŠres … supprimer sont … sp‚cifier dans le second argument de la
fonction. Il faut mettre l'espace dans les caractŠres de ponctuation si
on d‚sire supprimer les blancs.

&EX

	SCR_pstrip("Titre .......", ". :?")

	    vaut

	"Titre"
&TX
&RT Le string stripp‚
&SA SCR_pstrip_lg()
======================================================================= */

unsigned char *SCR_pstrip(buf, punct)
unsigned char *buf,
	      *punct;
{
    buf[SCR_pstrip_lg(buf, punct)] = 0;
    return(buf);    /* JMP 4.23 19-12-95 */
}



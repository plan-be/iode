#include "s_strs.h"
/* =+=================================================================
Supprime tous les blancs d'un string. Prolonge ensuite le string par des
blancs de telle sorte que sa longueur reste inchangée.

&RT un pointeur vers le string modifié
&EX
    printf("'%s'\n", U_sqz_text(" Jean - Marc "));

	vaut

    'Jean-Marc    '
&TX
&SA U_rjust_text(), U_ljust_text()
====================================================================== */

unsigned char *U_sqz_text(text)
unsigned char *text;
{
    int     i, j;

    for(i = 0, j = 0 ; text[i] != 0 ; i++)
	if(text[i] != ' ') text[j++] = text[i];

    for( ; text[j] != 0 ; j++) text[j] = ' ';
    return(text);
}


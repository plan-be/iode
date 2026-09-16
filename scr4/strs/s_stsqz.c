#include "s_strs.h"

/* ====================================================================
Supprime tous les caractères non alphanumériques (lettres, chiffres, ou ceux
de la liste chars) d'un string. Le string est terminé par un '\0'.

&RT le string strippé
&EX
    printf("'%s'\n", SCR_asqz(" Jean - Marc   ", "-"));
    printf("'%s'\n", SCR_asqz("\nscr_3.a2m   ", "_."));

	vaut

    'Jean-Marc'
    'scr_3.a2m'
&TX
&SA U_sqz_text() , SCR_sqz()
=======================================================================*/

unsigned char *SCR_asqz(text, chars)
unsigned char *text, *chars;
{
    int     i, j;

    if(text == 0) return(text); // JMP 3/1/2017

    for(i = 0, j = 0 ; text[i] != 0 ; i++)
	if(SCR_is_anum(text[i]) || (chars && U_is_in(text[i], chars)))
	    text[j++] = text[i];
    text[j] = 0;
    return(text);
}

/* ====================================================================
Supprime tous les blancs d'un string. Le string est terminé par un '\0'.

&RT le string strippé
&EX
    printf("'%s'\n", SCR_sqz(" Jean - Marc   "));

	vaut
    'Jean-Marc'
&TX
&SA U_sqz_text()
=======================================================================*/

unsigned char *SCR_sqz(text)
unsigned char *text;
{
    int     i, j;

    if(text == 0) return(text); // JMP 3/1/2017

    for(i = 0, j = 0 ; text[i] != 0 ; i++)
	if(text[i] != ' ') text[j++] = text[i];
    text[j] = 0;
    return(text);
}






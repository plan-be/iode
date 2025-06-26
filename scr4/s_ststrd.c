#include "s_strs.h"

/* ====================================================================
Supprime les caractères excédentaires dans un string : chaque ch
recontré dans txt est supprimé si le caractère qui le suite fait
partie du string str.

La modification a lieu sur place.

&RT le pointeur vers le string input.
&EX
    printf("'%s'\n", SCR_stripdc("  A .     AB ", ' ', " ."));

	donne

    ' A. AB '
&TX
&SA SCR_strip(), SCR_pstrip(), SCR_sqz()
=======================================================================*/

unsigned char *SCR_stripdc(txt, ch, str)
U_ch    *txt, *str;
int     ch;
{
    int     i, j = 0;

    for(i = 0 ; txt[i] ; i++)
	if(txt[i] != ch || !U_is_in(txt[i + 1], str))
	    txt[j++] = txt[i];
    txt[j] = 0;
    return(txt);
}

#include "s_strs.h"

/* ====================================================================
Retourne la position d'un caractäre ch dans un string str.

&RT la position du caractäre s'il est trouvÇ, -1 sinon.
&EX
    U_pos('.', "filename.txt");
&TX
&SA U_is_in()
=======================================================================*/

U_pos(ch, str)
int             ch;
unsigned char   *str;
{
    int     i;

    for(i = 0 ; str[i] ; i++)
	if(str[i] == ch) return(i);

    return(-1);
}


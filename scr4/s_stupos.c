#include "s_strs.h"

/* ====================================================================
Retourne la position d'un caractère ch dans un string str.

&RT la position du caractère s'il est trouvé, -1 sinon.
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


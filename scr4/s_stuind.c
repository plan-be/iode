#include "s_strs.h"
/* ====================================================================
Retourne la position d'un string sstr dans le string str.

&RT la position du premier caractère s'il est trouvé, -1 sinon.
&EX
    U_index("filename.txt", ".txt")
	vaut
    8
&TX
&SA U_pos()
=======================================================================*/

U_index(str, sstr)
unsigned char   *str, *sstr;
{
    int     i = 0, lg, slg, pos;

    lg  = (int)strlen(str);
    slg = (int)strlen(sstr);

/*    for(i = 0 ; str[i] ; i++) {
	if(slg + i > lg) return(-1);
	if(memcmp(str + i, sstr, slg) == 0) return(i);
	}
JMP 18-05-99 */

    if(lg < slg) return(-1);
    while(1) {
	pos = U_pos(sstr[0], str + i);
	if(pos < 0) return(-1);
	if(pos + i + slg > lg) return(-1);
	if(memcmp(str + pos + i, sstr, slg) == 0) return(i + pos);
	i += pos + 1;
	}

    return(-1);
}



#include "s_strs.h"
/* ====================================================================
Remplace la fin d'un string par des '\0' à partir de la position du
premier caractère < ' '.

Le dernier caractère du string (ptr[len - 1]) est toujours fixé à 0.
Cette fonction est utilisée pour les ZSTRING dans les ISAM.

&RT le string modifié
&SA SCR_strip()
=======================================================================*/

unsigned char *SCR_zstrip(ptr, len)
unsigned char *ptr;
int     len;
{
    int     i;

    ptr[len - 1] = 0;
    for(i = 0 ; i < len ; i++)
	if(ptr[i] < ' ' && (ptr[i] != '\n' && ptr[i] != '\r')) {
	    memset(ptr + i, 0, len - i);
	    break;
	    }
    i--;
    for( ; i >= 0 ; i--) {
	if(ptr[i] > ' ') break;
	ptr[i] = 0;
	}
    return(ptr);
}

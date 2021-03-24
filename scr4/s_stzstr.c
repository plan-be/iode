#include "s_strs.h"
/* ====================================================================
Remplace la fin d'un string par des '\0' � partir de la position du
premier caract�re < ' '.

Le dernier caract�re du string (ptr[len - 1]) est toujours fix� � 0.
Cette fonction est utilis�e pour les ZSTRING dans les ISAM.

&RT le string modifi�
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

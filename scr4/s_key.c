#include <stdio.h>
#include <stdlib.h>


/* ======================================================================
Calcule un clé d'activation sur base d'un string quelconque. La valeur
retournée est un long positif.

&RT long contenant une clé codifiant key
------------------------------------------------------------------------- */

long KEY_calc(key)
unsigned char   *key;
{
    int     i, j;
    long    l;
/*    long    atol(); /* JMP 06-01-98 */
    float   d;
    char    pwd[10];

    for(i = 0 ; i < 8 ; i++) {
	d = 0;
	for(j = 0 ; key[j] ; j++) d += key[j] * ((7 + i) * (23 + j)) % 9;
	d *= (i + 2.345678);
	d = d * d;
	l = d;
	pwd[i] = '1' + l % 9;
	}

    pwd[8] = 0;
    return(atol(pwd));
}

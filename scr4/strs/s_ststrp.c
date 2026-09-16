#include "s_strs.h"

/* ====================================================================
Détruit les caractères inférieurs ou égaux à blanc de la fin d'un
string. Le caractères '\0' se place après le dernier caractère supérieur
au blanc.

La modification a lieu sur place.

&RT le pointeur vers le string input.
&EX
    printf("'%s'\n", SCR_strip("  123   "));
    printf("'%s'\n", SCR_strip("  ABC\001\002\033"));

	donne

    '  123'
    '  ABC'
&TX
&SA SCR_strip_lg(), SCR_pad(), SCR_pstrip(), SCR_sqz()
=======================================================================*/

unsigned char *SCR_strip(str)
unsigned char *str;
{
    int i;

    if(str == 0) return(str);
    for(i = (int)strlen(str) - 1; i >= 0 ; i--)
	if(str[i] > ' ') {
	    if(str[i + 1] != 0) str[i + 1] = 0; /* JMP 15-03-07 pour Linux */
	    break;
	    }
    if(i < 0) str[0] = 0;
    return(str);
}


/* ====================================================================
Détruit les caractères inférieurs ou égaux à blanc de la fin d'un
string de longueur lg. Le caractères '\0' se place après le dernier caractère supérieur
au blanc.

La modification a lieu sur place.

Si tous les caractères sont > ' ', il n'y a pas de modification.

&RT le pointeur vers le string input.
&TX
&SA SCR_strip(), SCR_pad(), SCR_pstrip(), SCR_sqz()
=======================================================================*/

unsigned char *SCR_strip_lg(str, lg)
unsigned char   *str;
int             lg;
{
    int i;

    if(str == 0) return(str);
    for(i = lg - 1; i >= 0 ; i--) {
	if(str[i] > ' ') break;
	str[i] = 0;
	}

    return(str);
}








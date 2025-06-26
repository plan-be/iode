#include "s_strs.h"

/* ====================================================================
Fonction identique à strcpy, mais copie dans l'ordre inverse des bytes.
Cette fonction permet par exemple d'effectuer un shift à droite d'une
partie de string.

&RT le pointeur vers le string out.

&EX
    RightShift(char *str, int n)
    {
	SCR_strrcpy(str + n, str);
	memset(str, ' ', n);
    }
&TX
&SA SCR_strlcpy(), SCR_stracpy(), SCR_strfacpy()
=======================================================================*/

unsigned char *SCR_strrcpy(out, in)
unsigned char *out, *in;
{
    int     i, lg = (int)strlen(in);

    for(i = lg ; i >= 0 ; i--)
	out[i] = in[i];
    return(out);
}


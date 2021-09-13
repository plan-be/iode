#include "s_strs.h"
/* =+=================================================================
Recherche un caractäre dans un string.

&RT 1 si le caractäre est trouvÇ, 0 sinon.
&EX
    U_is_in(text[i], ".;:,?! ");
&TX
====================================================================== */

U_is_in(ch, string)
int     ch;
char    *string;
{
    if(U_pos(ch, string) < 0) return(0);
    return(1);
}


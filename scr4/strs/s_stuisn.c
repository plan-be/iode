#include "s_strs.h"
/* =+=================================================================
Recherche un caractère dans un string.

&RT 1 si le caractère est trouvé, 0 sinon.
&EX
    U_is_in(text[i], ".;:,?! ");
&TX
====================================================================== */

int U_is_in(int ch, char *string)
{
    if(U_pos(ch, string) < 0) return(0);
    return(1);
}


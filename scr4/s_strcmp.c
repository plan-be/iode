#include "s_strs.h"

/* ====================================================================
Compare deux strings sans tenir compte des différences entre minuscules
et majuscules.

&RT -1 si le str1 est "plus petit" que str2, 0 en cas d'égalité, 1 si
    str1 est plus grand que str2.

=======================================================================*/

SCR_cstrcmp(str1, str2)
U_ch    *str1, *str2;
{
    return(SCR_cstrncmp(str1, str2, -1));
}

/* ====================================================================
Compare deux strings sur lg caractères sans tenir compte des différences
entre minuscules et majuscules.

&RT -1 si le str1 est "plus petit" que str2, 0 en cas d'égalité, 1 si
    str1 est plus grand que str2.

=======================================================================*/

SCR_cstrncmp(str1, str2, lg)
U_ch    *str1, *str2;
{
    U_ch    *u1, *u2;
    int     rc;

    if(str1 == 0 || str2 == 0) {
	if(str1 == 0 && str2 == 0) return(0);
	return(-1);
	}

    u1 = SCR_stracpy(str1);
    u2 = SCR_stracpy(str2);
    SCR_upper(u1);
    SCR_upper(u2);
    if(lg < 0) rc = strcmp(u1, u2);
    else       rc = memcmp(u1, u2, lg);
    SCR_free(u1);
    SCR_free(u2);
    return(rc);
}


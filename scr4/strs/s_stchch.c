#include "s_strs.h"

/* ====================================================================
Remplace un caractère (ch1) par un autre (ch2) dans un Zstring (str).

Le tableau input doit se terminer par un pointeur nul.

&RT pointeur vers str
&EX
    UnixFilename(filename)
    char   *filename;
    {
	SCR_change_char(filename, '\\', '/');
    }

&TX
&SA SCR_replace()
=======================================================================*/


U_ch  *SCR_change_char(str, ch1, ch2)
U_ch    *str;
int     ch1, ch2;
{
    U_ch    *ptr = str;

    while(*str) {
	if(*str == ch1) *str = ch2;
	str++;
    }
    return(ptr);
}

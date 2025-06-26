#include "s_strs.h"

/* ====================================================================
Indique si un caractère est minuscule.

&REM
Les caractères accentués sont considérés comme des minuscules.

&RT 1 s'il s'agit d'une minuscule, 0 sinon
&EX
    SCR_is_lower('a') vaut 1
    SCR_is_lower('â') vaut 1
    SCR_is_lower('A') vaut 0
    SCR_is_lower('\n') vaut 0
&TX
&SA SCR_is_upper(), SCR_upper(), SCR_lower_char(), SCR_lower()
=======================================================================*/

SCR_is_lower(ch)
int     ch;
{
    if(SCR_upper_char(ch) != ch) return(1);
    return(0);
}


/* ====================================================================
Met un caractère en minuscule s'il s'agit d'une lettre uniquement. La
macro tolower(ch) disponible dans la librairie standard C ne vérifie pas
toujours que le caractère input est une lettre majuscule avant de
transformer le caractère.

&RT le caractère transformé
&EX
    SCR_lower_char('A') vaut 'a'
    SCR_lower_char('1') vaut '1'
    SCR_lower_char('c') vaut 'c'
&TX
&SA SCR_lower(), SCR_upper_char(), SCR_upper()
=======================================================================*/

SCR_lower_char(ch)
int     ch;
{
    if(ch >= 'A' && ch <= 'Z') return(ch - 'A' + 'a'); /* JMP 14-06-96 */
/*  if(SCR_is_upper(ch)) ch = tolower(ch);             /* JMP 14-06-96 */
    return(ch);
}

/* ====================================================================
Met un string en minuscules. La modification a lieu sur place.

&RT le string transformé
&EX
    SCR_lower("Après") vaut "après'
&TX
&SA SCR_lower_char(), SCR_upper_char(), SCR_upper()
=======================================================================*/

unsigned char *SCR_lower(str)
unsigned char    *str;
{
    int     i;

    if(str == 0) return(str);
    for(i = 0 ; str[i] ; i++)
	str[i] = SCR_lower_char(str[i]);
    return(str);
}


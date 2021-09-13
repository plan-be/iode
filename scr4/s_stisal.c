#include "s_strs.h"

/* ====================================================================
Indique si un caractäre est une lettre (majuscule ou minuscule ou
accentuÇe).

&RT 1 si c'est le cas, 0 sinon
&EX
    SCR_is_alpha('0') vaut 0
    SCR_is_alpha('Ç') vaut 1
    SCR_is_alpha('A') vaut 1
    SCR_is_alpha('_') vaut 0
&TX
&SA SCR_is_anum(), SCR_is_num(), SCR_upper_char()
=======================================================================*/

SCR_is_alpha(ch)
int     ch;
{
    int     ch1;

    ch1 = SCR_upper_char(ch);
    if('A' <= ch1 && 'Z' >= ch1) return(1);
    return(0);
}

/* ====================================================================
Indique si un caractäre est un chiffre.

&RT 1 si c'est le cas, 0 sinon
&EX
    SCR_is_num('0') vaut 1
    SCR_is_num('Ç') vaut 0
    SCR_is_num('.') vaut 0
&TX
&SA SCR_is_anum(), SCR_is_alpha()
=======================================================================*/

SCR_is_num(ch)
int     ch;
{
    if(('0' <= ch && '9' >= ch)) return(1);
    return(0);
}

/* ====================================================================
Indique si un caractäre est une lettre (majuscule ou minuscule ou
accentuÇe) ou un chiffre.

&RT 1 si c'est le cas, 0 sinon
&EX
    SCR_is_anum('0') vaut 1
    SCR_is_anum('Ç') vaut 1
    SCR_is_anum('A') vaut 1
    SCR_is_anum('!') vaut 0
    SCR_is_anum('_') vaut 0
    SCR_is_anum('.') vaut 0
    SCR_is_anum('-') vaut 0
&TX
&SA SCR_is_alpha(), SCR_is_num(), SCR_upper_char()
=======================================================================*/

SCR_is_anum(ch)
int     ch;
{
    return(SCR_is_num(ch) || SCR_is_alpha(ch));
}

/* Recherche un string f dans la liste txt */
ScrStrIsIn(U_ch **txt, char *f)
{
    int     i, j;

    if(txt == 0) return(0);
    for(i = 0 ; txt[i] ; i++)
	if(strcmp(txt[i], f) == 0) return(1);
    return(0);
}

/* Recherche un string f ce trouvant dans un des string de txt */
ScrStrIsIn2(U_ch **txt, char *f)
{
    int     i, j, len_f = strlen(f);

    if(txt == 0) {
	Debug("ScrStrIsIn2: pas de liste de comparaison\n");
	return(0);
    }
    for(i = 0 ; txt[i] ; i++) {
//	Debug("ScrStrIsIn2 - Compare : '%s' <> '%s'\n", f, txt[i]);
	if((unsigned int)len_f < strlen(txt[i])) continue;
	if(strncmp(f, txt[i], strlen(txt[i])) == 0) return(1);
    }

    return(0);
}


#include "s_strs.h"

/* ====================================================================
Indique si un caractère est majuscule (A-Z).

&RT 1 si c'est le cas, 0 sinon
&EX
    SCR_is_upper('a') vaut 0
    SCR_is_upper('A') vaut 1
    SCR_is_upper('1') vaut 0
&TX
&SA SCR_is_lower(), SCR_upper(), SCR_lower_char(), SCR_lower()
=======================================================================*/

SCR_is_upper(ch)
int     ch;
{
    if(ch >= 'A' && ch <= 'Z') return(1);       /* JMP 14-06-96 */
/*    if(isupper(ch)) return(1);                /* JMP 14-06-96 */
    return(0);
}

/* ====================================================================
Met un caractère en majuscule s'il s'agit d'une lettre uniquement. La
macro toupper(ch) disponible dans la librairie standard C ne vérifie pas
toujours que le caractère input est une lettre minuscule avant de
transformer le caractère.

&REM
Les caractères accentués sont également transformés en majuscules (é->E).

&RT le caractère transformé
&EX
    SCR_upper_char('a') vaut 'A'
    SCR_upper_char('â') vaut 'A'
    SCR_upper_char('ç') vaut 'C'
    SCR_upper_char('C') vaut 'C'
    SCR_upper_char('\n') vaut '\n'
&TX
&SA SCR_upper(), SCR_lower_char(), SCR_lower()
=======================================================================*/

SCR_upper_char(ch)
int     ch;
{
/*    if(SCR_is_lower(ch)) return(toupper(ch));        /* JMP 14-06-96 */
    if(ch >= 'a' && ch <= 'z') return(ch - 'a' + 'A'); /* JMP 14-06-96 */
    switch(ch) {
	case 210 /* E^ */ : /* BP_M 05-06-2010 12:20 */
	case 211 /* E¨ */ :
	case 212 /* E` */ :
	case 144 /* E' */ :
	case 130 /* é */ :          /* JMP 29-09-91 */
	case 138 /* è */ :
	case 137 /* ë */ :
	case 136 /* ê */ : return('E');
	case 160 /* á */ :
	case 133 /* à */ :
	case 131 /* â */ :
	case 181 /* A´*/ :
	case 183 /* A`*/ :
	case 182 /* A^*/ :
	case 142 /* A¨*/ :
	case 132 /* ä */ : return('A');
	case 214 /* I´*/ :
	case 222 /* I`*/ :
	case 215 /* I^*/ :
	case 216 /* I¨*/ :
	case 161 /* í */ :
	case 141 /* ì */ :
	case 140 /* î */ :
	case 139 /* ï */ : return('I');
	case 224 /* O´*/ :
	case 227 /* O`*/ :
	case 226 /* O^*/ :
	case 153 /* O¨*/ :
	case 162 /* ó */ :
	case 149 /* ò */ :
	case 148 /* ö */ :
	case 147 /* ô */ : return('O');
	case 233 /* U´*/ :
	case 235 /* U`*/ :
	case 234 /* U^*/ :
	case 154 /* U¨*/ :
	case 163 /* é */ :
	case 150 /* û */ :
	case 151 /* ù */ :
	case 129 /* ü */ : return('U');
	case 135 /* ç */ : return('C');
	default  : return(ch);
	}
}


/* ====================================================================
Met un string en majuscules. La modification a lieu sur place. Les
accents sont également transformés.

&RT le string transformé
&EX
    SCR_upper("Après") vaut "APRES'
&TX
&SA SCR_lower_char(), SCR_upper_char(), SCR_lower()
=======================================================================*/

unsigned char *SCR_upper(str)
unsigned char *str;
{
    int     i;

    if(str == 0) return(str);
    for(i = 0 ; str[i] ; i++)
	str[i] = SCR_upper_char(str[i]);
    return(str);
}


SCR_is_space(ch)
int     ch;
{
    if(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r'
	|| ch == 013 || ch == 014) return(1);
    return(0);
}




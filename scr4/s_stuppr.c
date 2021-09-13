#include "s_strs.h"

/* ====================================================================
Indique si un caract�re est majuscule (A-Z).

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
Met un caract�re en majuscule s'il s'agit d'une lettre uniquement. La
macro toupper(ch) disponible dans la librairie standard C ne v�rifie pas
toujours que le caract�re input est une lettre minuscule avant de
transformer le caract�re.

&REM
Les caract�res accentu�s sont �galement transform�s en majuscules (�->E).

&RT le caract�re transform�
&EX
    SCR_upper_char('a') vaut 'A'
    SCR_upper_char('�') vaut 'A'
    SCR_upper_char('�') vaut 'C'
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
	case 211 /* E� */ :
	case 212 /* E` */ :
	case 144 /* E' */ :
	case 130 /* � */ :          /* JMP 29-09-91 */
	case 138 /* � */ :
	case 137 /* � */ :
	case 136 /* � */ : return('E');
	case 160 /* � */ :
	case 133 /* � */ :
	case 131 /* � */ :
	case 181 /* A�*/ :
	case 183 /* A`*/ :
	case 182 /* A^*/ :
	case 142 /* A�*/ :
	case 132 /* � */ : return('A');
	case 214 /* I�*/ :
	case 222 /* I`*/ :
	case 215 /* I^*/ :
	case 216 /* I�*/ :
	case 161 /* � */ :
	case 141 /* � */ :
	case 140 /* � */ :
	case 139 /* � */ : return('I');
	case 224 /* O�*/ :
	case 227 /* O`*/ :
	case 226 /* O^*/ :
	case 153 /* O�*/ :
	case 162 /* � */ :
	case 149 /* � */ :
	case 148 /* � */ :
	case 147 /* � */ : return('O');
	case 233 /* U�*/ :
	case 235 /* U`*/ :
	case 234 /* U^*/ :
	case 154 /* U�*/ :
	case 163 /* � */ :
	case 150 /* � */ :
	case 151 /* � */ :
	case 129 /* � */ : return('U');
	case 135 /* � */ : return('C');
	default  : return(ch);
	}
}


/* ====================================================================
Met un string en majuscules. La modification a lieu sur place. Les
accents sont �galement transform�s.

&RT le string transform�
&EX
    SCR_upper("Apr�s") vaut "APRES'
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




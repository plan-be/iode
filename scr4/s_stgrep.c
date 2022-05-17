#include "s_strs.h"

/* =====================================================================
Teste si string est conforme … l'image d‚crite dans pattern.
pattern contient des caractŠres sp‚ciaux pour indiquer des positions
libres.

ecase est 1 si la recherche doit respecter les majuscules/minuscules, et
0 sinon.

Les caractŠres sp‚ciaux entrant dans pattern sont :
&CO
    * : n'importe quelle suite de caractŠres (mˆme vide)
    ? : un et un seul caractŠre (quelconque)
    @ : n'importe quel caractŠre alphanum‚rique
    & : n'importe quel caractŠre non alphanum‚rique
    | : n'importe quel caractŠre alphanum‚rique ou aucun en d‚but et fin
	de string
    ! : n'importe quel caractŠre non alphanum‚rique ou aucun en d‚but
	et fin de string
    \ plac‚ devant un des caractŠres sp‚ciaux supprime sa signification
&TX


&RT La fonction retourne 0 si string correspond … la description pattern et
-1 sinon

&EX
    SCR_grep("*.c",     "test.c", 0)  retourne 0  (Ok)
    SCR_grep("t*c",     "test.c", 0)  retourne 0  (Ok)
    SCR_grep("*e*c",    "test.c", 0)  retourne 0  (Ok)
    SCR_grep("*e?t*c",  "test.c", 0)  retourne 0  (Ok)

    SCR_grep("*@c",     "test.c", 0)  retourne -1 (Not Ok)
    SCR_grep("*!A1!*",  "2*A1+B", 0)  retourne 0  (Ok)
    SCR_grep("*!A1!*",  "A1", 0)      retourne 0  (Ok)
    SCR_grep("*!A1!*",  "2*CA1+B", 0) retourne -1 (Not Ok)

    SCR_grep("c",       "test.c", 0)  retourne -1 (Not Ok)
    SCR_grep("t*t",     "test.c", 0)  retourne -1 (Not Ok)
    SCR_grep("*e?t*c",  "test.c", 0)  retourne 0  (Ok)

&CO

====================================================================== */

int SCR_grep(pattern, string, ecase)
char    *pattern, *string;
int     ecase;
{
    return(SCR_grep_1(pattern, string, ecase, 0, '*'));
}

/*NH*/
int SCR_grep_gnl(char* pattern, char* string, int ecase, int all)
{
    return(SCR_grep_1(pattern, string, ecase, 0, all));
}

/*NH*/
int SCR_grep_1(pattern, string, ecase, nbeg, diese)
char    *pattern, *string;
int     ecase, nbeg, diese;
{
    int     i = 0, lg, j = 0, rc, esc = 0, s, p;

    lg = strlen(string);

    while(1) {
	if(esc) {
	    s = string[j];
	    p = pattern[i];
	    if(ecase == 0) {
		s = SCR_upper_char(s);
		p = SCR_upper_char(p);
		}
	    if(s != p) return(-1);
	    if(p == 0) return(0);
	    i++; j++;
	    esc = 0;
	    }

	s = string[j];
	p = pattern[i];
	switch(p) {
	    case '\\' :
		i++;
		esc = 1;
		break;

	    case '?' :
		if(s == 0) return(-1);
		i++; j++;
		break;

	    case '@' :
		if(!(SCR_is_anum(s) || s == '_')) return(-1);
		i++; j++;
		break;

	    case '&' :
		if(SCR_is_anum(s) || s == '_') return(-1);
		i++; j++;
		break;

	    case '!' :
		if(j == lg) {i++; break;}
		pattern[i] = '&';
		rc = SCR_grep_1(pattern + i, string + j, ecase, j, diese);
		pattern[i] = '!';
		if(rc == 0) return(0);
		if(!nbeg && j == 0) rc = SCR_grep_1(pattern + i + 1, string + j, ecase, 1, diese);
		return(rc);

	    case '|' :
		if(j == lg) {i++; break;}
		pattern[i] = '@';
		rc = SCR_grep_1(pattern + i, string + j, ecase, j, diese);
		pattern[i] = '|';
		if(rc == 0) return(0);
		if(!nbeg && j == 0) rc = SCR_grep_1(pattern + i + 1, string + j, ecase, 1, diese);
		return(rc);

	    default :
		if(p == diese) {
		    i++;
		    if(pattern[i] == 0) return(0);
		    for( ; j < lg ; j++)
			if(SCR_grep_1(pattern + i, string + j, ecase, j, diese) == 0) return(0);

		    return(-1);
		    }
		esc = 1;
		break;
	    }
	}
}




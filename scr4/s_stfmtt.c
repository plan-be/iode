#include "s_strs.h"
/* ====================================================================
Formatte un string in suivant un format fmt donn‚. Le r‚sultat out est
un string transform‚ de longueur ‚gale … celle de fmt.

Les caractŠres reconnus dans le format sont : 'X', 'x', '9', '0'.
Ils signifient qu'aux seules positions de ces caractŠres seront plac‚s
dans leur ordre d'apparition les caractŠres trouv‚s dans in. Seul cas
particulier : le caractŠre '0' qui sera remplac‚ par une '0' si le
caractŠre correspondant de in est blanc.

Les longueurs des strings ne sont pas v‚rifi‚es.

&RT le pointeur vers out
&EX
    SCR_fmt_text(out, "9990-XX-00", "123ABC  ");

	donne

    "123A-BC-00"

&TX
&SA SCR_ufmt_text(), SCR_fmt_long()
=======================================================================*/

unsigned char *SCR_fmt_text(out, fmt, in)
unsigned char *out,
	      *fmt,
	      *in;
{
    int     i, j;

    for(i = 0, j = 0 ; fmt[i] != 0 ; i++) {
	switch(fmt[i]) {
	    case '9' :
	    case 'x' :
	    case 'X' : out[i] = in[j++]; break;
	    case '0' :
		out[i] = in[j++];
		if(out[i] == ' ') out[i] = '0';
		break;
	    default : out[i] = fmt[i]; break;
	    }
	if(out[i] < ' ') out[i] = ' ';
    }

    out[i] = 0;
    return(out);
}


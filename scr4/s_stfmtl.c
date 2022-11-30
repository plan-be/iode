#include "s_strs.h"

/* ====================================================================
Formatte un long l suivant un format fmt donn‚. Le r‚sultat out est
un string transform‚ de longueur ‚gale … celle de fmt.

Les caractŠres reconnus dans le format sont : 'X', 'x', '9', '0'.
Ils signifient qu'aux seules positions de ces caractŠres seront plac‚s
dans leur ordre d'apparition les caractŠres r‚sultant du formattage
de l.

Seul cas particulier : le caractŠre '0' qui sera remplac‚ par une '0' si
le caractŠre correspondant de in est ' '.

Les longueurs des strings ne sont pas v‚rifi‚es.

&RT le pointeur vers out
&EX
    SCR_fmt_long(out, "9990-00-00", 123456L);
    SCR_fmt_long(out, "TVA : 000-000-000", 123456L);

	donnent respectivement

    "  12-34-56"
    "TVA : 000-123-456"

&TX
&SA SCR_fmt_text(), SCR_ufmt_long()
=======================================================================*/

unsigned char *SCR_fmt_long(out, fmt, l)
unsigned char *out, *fmt;
unsigned long l;
{
    int     i, j;
    char    in[30];

    sprintf(in, "%lu", l);
    i = (int)strlen(fmt) - 1;
    out[i + 1] = 0;
    j = (int)strlen(in) - 1;
    for( ; i >= 0 ; i--) {
	switch(fmt[i]) {
	    case '9' :
	    case 'x' :
	    case 'X' :
		if(j >= 0) out[i] = in[j--];
		else out[i] = ' ';
		break;
	    case '0' :
		if(j >= 0) out[i] = in[j--];
		else out[i] = '0';
		if(out[i] == ' ') out[i] = '0';
		break;
	    default :
		out[i] = fmt[i]; break;
	    }
    }
    return(out);
}


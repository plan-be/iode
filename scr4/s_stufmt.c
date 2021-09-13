#include "s_strs.h"

/* ====================================================================
D‚formatte un string in suivant un format fmt donn‚. Le r‚sultat out est
un string transform‚ de longueur inf‚rieure ou ‚gale … celle de fmt.

Les caractŠres reconnus dans le format sont : 'X', 'x', '9', '0'.
Ils signifient qu'aux seules positions de ces caractŠres seront extraits
dans leur ordre d'apparition les caractŠres trouv‚s dans in. Seul cas
particulier : le caractŠre '0' qui sera remplac‚ par une '0' si le
caractŠre correspondant de in est ' '.

Les longueurs des strings ne sont pas v‚rifi‚es.

&RT le pointeur vers out
&EX
    SCR_ufmt_text(out, "Num‚ro : 9990-XX-00", "1234-AB-C ");

	donne

    "1234ABC0"

&TX
&SA SCR_fmt_text(), SCR_fmt_long()
=======================================================================*/

unsigned char *SCR_ufmt_text(out, fmt, in)
unsigned char *out,
	      *fmt,
	      *in;
{
    int     i, j = 0;

    for(i = 0 ; fmt[i] != 0 ; i++) {
	switch(fmt[i]) {
	    case '9' :
	    case 'x' :
	    case 'X' : out[j++] = in[i]; break;
	    case '0' :
		out[j] = in[i];
		if(out[j] == ' ') out[j] = '0';
		j++;
		break;
	    default : break;
	    }
    }
    out[j] = 0;
    return(out);
}


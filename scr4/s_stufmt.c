#include "s_strs.h"

/* ====================================================================
Déformatte un string in suivant un format fmt donné. Le résultat out est
un string transformé de longueur inférieure ou égale à celle de fmt.

Les caractères reconnus dans le format sont : 'X', 'x', '9', '0'.
Ils signifient qu'aux seules positions de ces caractères seront extraits
dans leur ordre d'apparition les caractères trouvés dans in. Seul cas
particulier : le caractère '0' qui sera remplacé par une '0' si le
caractère correspondant de in est ' '.

Les longueurs des strings ne sont pas vérifiées.

&RT le pointeur vers out
&EX
    SCR_ufmt_text(out, "Numéro : 9990-XX-00", "1234-AB-C ");

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


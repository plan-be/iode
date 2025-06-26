#include "s_strs.h"

/* ====================================================================
Calcule la longueur du string extrait d'un format : effectue la somme de
tous les '0', '9', 'X' et 'x' présents dans le format.

&RT le nombre de caractères du string déformatté
&EX
    SCR_fmt_lg("Numéro : 9990-XX-00")

    vaut 8
&TX
&SA SCR_ufmt_text(), SCR_fmt_text()
=======================================================================*/

SCR_fmt_lg(fmt)
char    *fmt;
{
    int     i, nb = 0;

    for(i = 0 ; fmt[i] ; i++)
	 switch(fmt[i]) {
	    case 'x' :
	    case 'X' :
	    case '9' :
	    case '0' : nb++; break;
	    default  : break;
	    }
    return(nb);
}

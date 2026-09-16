#include "s_strs.h"
/* ====================================================================
Déformatte un long formatté dans in suivant un format fmt donné. Le
résultat est un long.

Les caractères reconnus dans le format sont : 'X', 'x', '9', '0'.
Ils signifient qu'aux seules positions de ces caractères seront extraits
dans leur ordre d'apparition les chiffres trouvés dans in. Seul cas
particulier : le caractère '0' qui sera remplacé par une '0' si le
caractère correspondant de in est ' '.

Les longueurs des strings ne sont pas vérifiées.

&RT le pointeur vers out
&EX
    SCR_ufmt_long("Numéro : 9990-XX-00", "Numéro : 123-123-123");

	donne

    123123123

&TX
&SA SCR_ufmt_text(), SCR_fmt_long()
=======================================================================*/

unsigned long SCR_ufmt_long(fmt, in)
char    *in, *fmt;
{
    char    buf[30];
    long    l;

    SCR_ufmt_text(buf, fmt, in);
    sscanf(buf, "%ld", &l);
    return(l);
}


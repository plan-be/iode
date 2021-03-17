#include "s_strs.h"
/* ====================================================================
D‚formatte un long formatt‚ dans in suivant un format fmt donn‚. Le
r‚sultat est un long.

Les caractŠres reconnus dans le format sont : 'X', 'x', '9', '0'.
Ils signifient qu'aux seules positions de ces caractŠres seront extraits
dans leur ordre d'apparition les chiffres trouv‚s dans in. Seul cas
particulier : le caractŠre '0' qui sera remplac‚ par une '0' si le
caractŠre correspondant de in est ' '.

Les longueurs des strings ne sont pas v‚rifi‚es.

&RT le pointeur vers out
&EX
    SCR_ufmt_long("Num‚ro : 9990-XX-00", "Num‚ro : 123-123-123");

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


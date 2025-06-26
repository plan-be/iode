#include "s_strs.h"

/* ======================================================================
Vérifie l'exactitude d'un numéro de registre national. Si le numéro est nul,
il est considéré comme correct. Le numéro ne doit pas contenir
de caractère de séparation entre les zônes.

&EX
    SCR_check_rn("58041511123");
&TX
&RT 0 au cas où le numéro est correct, -1 sinon.
------------------------------------------------------------------------- */

SCR_check_nb(rn)
char    *rn;
{
    long    rnnum, res;
/*  long    atol(); /* JMP 04-01-98 */
    char    buf[15];

    strncpy(buf, rn, 9);
    buf[9] = 0;
    rnnum = atol(buf);
    strncpy(buf, rn + 9, 2);
    buf[2] = 0;
    res = atol(buf);

    if(rnnum == 0 && res == 0) return(0);
    if(res == 97L - rnnum % 97L) return(0);
    return(-1);
}

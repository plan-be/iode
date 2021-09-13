#include "s_strs.h"

/* ======================================================================
V‚rifie l'exactitude d'un num‚ro bancaire. Si le num‚ro est nul,
il est consid‚r‚ comme correct. Le num‚ro ne doit pas contenir
de caractŠre de s‚paration entre les z“nes.

&EX
    SCR_check_bank("123123456712");
&TX
&RT 0 au cas o— le num‚ro est correct, -1 sinon.
------------------------------------------------------------------------- */

SCR_check_bank(ccp)
char    *ccp;
{
    long    bank, nb, res, reste;
/*  long atol(); /* JMP 04-01-98 */
    char    buf[8];

    strncpy(buf, ccp, 3);
    buf[3] = 0;
    bank = atol(buf);
    strncpy(buf, ccp + 3, 7);
    buf[7] = 0;
    nb = atol(buf);
    strncpy(buf, ccp + 10, 2);
    buf[2] = 0;
    res = atol(buf);

    if(nb == 0 && bank == 0 && res == 0) return(0);
    reste = (10000000L * (bank % 97L) + nb) % 97;
    if(reste == 0) reste = 97;
    if(res == reste) return(0);
    return(-1);
}

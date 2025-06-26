#define SCRSUPPROTO
#include "scr.h"
#ifdef WINDOWS
#include "scr4w.h"  /* JMP 06-01-98 */
#endif

/* ======================================================================
Imprime le string passé comme argument. Ce string peut contenir des \n
(saut de ligne), des \f (saut de page) et \t (tabulations sur 8
caractères). Les tabulations ne sont pas celles définies dans le
PRINTER, mais les tabulations standards alignées sur les multiples de 8
caractères.

&RT Le nombre de caractères imprimés ou -1 s'il n'y a pas le PRINTER
    ne peut être ouvert.

&NO Le nombre maximum de caractères générés ne peut excéder 511. Si
    ce n'est pas le cas, un dépassement de pointeur se produit (plantage!).
    En Windows (DOSW32 et SCRW32) et en Unix, le nombre peut atteindre 10239
    bytes (déc.2000).

&EX
    PR_string("Client : %s. Adresse %s\n", client_NAME, client_ADR);
&TX
&SA PR_char()
------------------------------------------------------------------------ */

int PR_string(c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd, ce)
char    *c1, *c2, *c3, *c4, *c5, *c6, *c7, *c8, *c9, *ca, *cb, *cc, *cd, *ce;
{
    int             i;
#if defined(UNIX) || defined(DOSW32) || defined(SCRW32)
    unsigned char   buf[10240];
#else
    unsigned char   buf[512];
#endif

    if(PR_open()) return(-1);
    sprintf(buf, c1, c2, c3, c4, c5, c6, c7, c8, c9, ca, cb, cc, cd, ce);
    for(i = 0 ; buf[i] ; i++) PR_char(buf[i]);
    return(i);
}

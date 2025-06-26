#include "scr.h"

/* ======================================================================
Descend le curseur de val lignes. Si la nouvelle position sort de la
page, un saut de page est généré.

&SA PR_up(), PR_left(), PR_right()
------------------------------------------------------------------------ */
PR_down(val)
int     val;
{
    PR_DEF  *pr = PR_CURRENT;

    if(PR_open()) return(0);
    pr->pr_cline += val;
    if(pr->pr_nl <= pr->pr_cline) PR_formfeed();

    return(0);
}

/* ======================================================================
Remonte le curseur de val lignes. Si la nouvelle position sort de la
page, la ligne est fixée à 0.

&SA PR_up(), PR_left(), PR_right()
------------------------------------------------------------------------ */
PR_up(val)
int     val;
{
    PR_DEF  *pr = PR_CURRENT;

    if(PR_open()) return(0);
    pr->pr_cline -= val;
    if(pr->pr_cline < 0) pr->pr_cline = 0;
    return(0);
}

/* ======================================================================
Déplace le curseur de val caractères vers la gauche. Si la nouvelle
position sort de la page, la colonne est fixée à 0.

&SA PR_up(), PR_down(), PR_right()
------------------------------------------------------------------------ */
PR_left(val)
int     val;
{
    PR_DEF  *pr = PR_CURRENT;

    if(PR_open()) return(0);
    pr->pr_ccol -= val;
    if(pr->pr_ccol < 0) pr->pr_ccol = 0;
    return(0);
}

/* ======================================================================
Déplace le curseur de val caractères vers la droite. Si la nouvelle
position sort de la page, un saut de ligne est généré.

&SA PR_up(), PR_down(), PR_left()
------------------------------------------------------------------------ */

PR_right(val)
int     val;
{
    PR_DEF  *pr = PR_CURRENT;

    if(PR_open()) return(0);
    pr->pr_ccol += val;
    if(pr->pr_nc >= pr->pr_ccol) PR_linefeed();
    return(0);
}




#include "scr.h"

/* ======================================================================
Effectue d'abord un TAB. Si le TAB d‚place le curseur (dans la ligne
courante), la fonction se termine. Sinon, un LINEFEED et un VTAB sont
g‚n‚r‚s pour d‚placer le curseur en colonne 0 dans le VTAB suivant dans
le PAGE.

&SA PR_tab(), PR_vtab()
------------------------------------------------------------------------ */
PR_pg_tab()
{
    if(PR_tab() == 1) return(0);
    PR_linefeed();
    PR_vtab();
    return(0);
}

/* ======================================================================
Positionne le curseur dans la page courante dans la prochaine colonne
suivant la colonne courante et d‚finie dans la liste des TABS fournie
lors de la d‚finition du PRINTER.

S'il n'y a pas de TABS d‚finis dans PRINTER, un TAB est plac‚ dans
chaque colonne multiple de 8.

Si le curseur d‚passe le dernier TAB de la ligne, il n'est pas d‚plac‚.
&EX

&TX
&RT 1 si un d‚placement a eu lieu, 0 sinon
&SA PR_pg_tab(), PR_vtab()
------------------------------------------------------------------------ */
PR_tab()
{
    PR_DEF  *pr = PR_CURRENT;
    int     i;

    if(pr->pr_nb_tabs == 0) {
	pr->pr_ccol = 8 * (1 + pr->pr_ccol / 8);
	return(1);
	}
    for(i = 0 ; i < pr->pr_nb_tabs ; i++)
	if(pr->pr_tabs[i] > pr->pr_ccol) {
	    pr->pr_ccol = pr->pr_tabs[i];
	    return(1);
	    }
    return(0);
}

/* ======================================================================
Positionne le curseur dans la page courante dans la prochaine ligne
suivant la ligne courante et d‚finie dans la liste des VTABS fournie
lors de la d‚finition du PRINTER.

S'il n'y a pas de VTABS d‚finis dans PRINTER, un VTAB est plac‚ dans
chaque ligne.

Si le curseur d‚passe le dernier VTAB de la page, une saut de page est
automatiquement g‚n‚r‚.

&SA PR_pg_tab(), PR_tab()
------------------------------------------------------------------------ */
PR_vtab()
{
    PR_DEF  *pr = PR_CURRENT;
    int     i;

    if(pr->pr_nb_vtabs == 0) {
	PR_linefeed();
	return(0);
	}
    for(i = 0 ; i < pr->pr_nb_vtabs ; i++)
	if(pr->pr_vtabs[i] > pr->pr_cline) {
	    pr->pr_cline = pr->pr_vtabs[i];
	    return(0);
	    }
    PR_formfeed();
    return(0);
}


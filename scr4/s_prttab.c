#include "scr.h"

/* ======================================================================
Effectue d'abord un TAB. Si le TAB déplace le curseur (dans la ligne
courante), la fonction se termine. Sinon, un LINEFEED et un VTAB sont
générés pour déplacer le curseur en colonne 0 dans le VTAB suivant dans
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
suivant la colonne courante et définie dans la liste des TABS fournie
lors de la définition du PRINTER.

S'il n'y a pas de TABS définis dans PRINTER, un TAB est placé dans
chaque colonne multiple de 8.

Si le curseur dépasse le dernier TAB de la ligne, il n'est pas déplacé.
&EX

&TX
&RT 1 si un déplacement a eu lieu, 0 sinon
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
suivant la ligne courante et définie dans la liste des VTABS fournie
lors de la définition du PRINTER.

S'il n'y a pas de VTABS définis dans PRINTER, un VTAB est placé dans
chaque ligne.

Si le curseur dépasse le dernier VTAB de la page, une saut de page est
automatiquement généré.

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


#include <scr.h>

int     PR_LABEL_ONL,
	PR_LABEL_ONC,
	PR_LABEL_NV,
	PR_LABEL_NH,
	PR_LABEL_NL,
	PR_LABEL_NC;
long    PR_LABEL_NB;

/* ======================================================================
S‚lectionne une imprimante pour l'impression d'‚tiquettes. Le papier
contient nh ‚tiquettes de front et nv de haut (normalement 1 pour
le papier en continu, chacune de nl lignes et nc colonnes.

Le PRINTER pr est momentan‚ment modifi‚ pour prendre en compte les
caract‚ristiques d‚finies. Il est restaur‚ lors de l'appel …
PR_label_send().

&EX
    PR_label_select(pr, 1, 2, 9, 40);
    while(..) {
	SCR_isam_to_page(p_af_client, af_client);
	PR_label_file("etiq.txt", p_af_client, 'T');
	}
    PR_label_send();
&TX
&SA PR_label_send(), PR_label_file(), PR_label_pg(), PR_file()
------------------------------------------------------------------------ */

PR_label_select(pr, nv, nh, nl, nc)
PR_DEF  *pr;
int     nv, nh, nl, nc;
{
    PR_LABEL_ONL = pr->pr_nl;
    PR_LABEL_ONC = pr->pr_nc;
    pr->pr_nl = nl * nv;
    pr->pr_nc = nc * nh;
    PR_LABEL_NB = 0;
    PR_LABEL_NC = nc;
    PR_LABEL_NL = nl;
    PR_LABEL_NV = nv;
    PR_LABEL_NH = nh;
    PR_select(pr);
    return(0);
}

/*NH*/
PR_label_pos()
{
    int line, col;

    if(PR_LABEL_NB && (PR_LABEL_NB % (PR_LABEL_NV * PR_LABEL_NH) == 0))
	PR_formfeed();
    line = PR_LABEL_NL * ((PR_LABEL_NB / PR_LABEL_NH) % PR_LABEL_NV);
    col  = PR_LABEL_NC * (PR_LABEL_NB % PR_LABEL_NH);
    PR_goto(line, col);
    PR_LABEL_NB++;
    return(0);
}


/* ======================================================================
Imprime la page pg et son contenu sur l'‚tiquette suivante selon la
d‚finition op‚r‚e par l'appel … PR_label_select().

&EX
    Voir PR_label_select().
&TX
&SA PR_label_send(), PR_label_file(), PR_label_select();
------------------------------------------------------------------------ */

PR_label_pg(pg)
PAGE    *pg;
{
    PR_label_pos();
    PR_page(pg);
    return(0);
}

/* ======================================================================
Imprime le fichier filename aprŠs interpr‚tation de son contenu
sur l'‚tiquette suivante selon la d‚finition op‚r‚e par l'appel …
PR_label_select(). Le fonctionnement est identique … celui de PR_file(),
… ceci prŠs que les lignes exc‚dant nl (d‚finies par PR_label_select())
ne sont pas imprim‚es et que les retours … la marge sont li‚s …
l'‚tiquette courante.

Seules les lignes commen‡ant par le caractŠre ch sont imprim‚es. Si ch
est nul, toutes les lignes du fichier sont imprim‚es.

&EX
    Voir PR_label_select().
&TX
&SA PR_label_send(), PR_label_pg(), PR_label_select();
------------------------------------------------------------------------ */

PR_label_file(filename, pg, ch)
char    *filename;
PAGE    *pg;
int     ch;
{
    PR_label_pos();
    PR_file_col(filename, pg,
	    (int)(PR_LABEL_NC * ((PR_LABEL_NB - 1) % PR_LABEL_NH)),
	    PR_LABEL_NL, ch);

    return(0);
}


/* ======================================================================
Termine l'impression d'‚tiquettes effectu‚e … l'aide de PR_label_select(),
et PR_label_pg() ou PR_label_file().

La d‚finition de l'imprimante momentan‚ment modifi‚e par PR_label_select()
est restaur‚e par cette fonction.

&EX
    Voir PR_label_select().
&TX
&SA PR_label_select(), PR_label_file(), PR_label_select();
------------------------------------------------------------------------ */

PR_label_send()
{
    PR_send();
    PR_CURRENT->pr_nl = PR_LABEL_ONL;
    PR_CURRENT->pr_nc = PR_LABEL_ONC;
    PR_LABEL_NB = 0;
    return(0);
}






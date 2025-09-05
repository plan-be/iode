#include "scr.h"

/* ======================================================================
Imprime une PAGE à la position actuelle du curseur dans la page
d'impression. Si la page contient un cadre, ce cadre est imprimé.

S'il n'y a pas assez de place restant sur la page courante, un saut de
page est forcé.

Après l'impression de la PAGE, le curseur d'impression est placé dans la
même ligne, mais à droite de la PAGE imprimé.

&NO
    La position de la PAGE dans l'écran (line et col) sont prises en
    compte pour décaler la page à l'impression. La fonction
    PR_page_clf() résoud ce problème.
&EX
    PR_page(pg1);
&TX
&SA PR_page_clf()
------------------------------------------------------------------------ */

PR_page(pg)
PAGE *pg;
{
    int line = pg->pg_line;
    int col  = pg->pg_column;
    int shad = pg->pg_shadow;
    int box  = (pg->pg_box && pg->pg_scr_pos < 0) ? 1 : 0; /* JMP 11-03-92 */
    int nl   = pg->pg_nb_line + 2 * box;
    int nc   = pg->pg_nb_column + 2 * box;

    if(PR_open()) return(-1);
    pg->pg_column = pg->pg_line = box;
    pg->pg_shadow = 0;

    SCR_TMP_IMAGE = SCR_alloc_image(nl, nc); /* JMP 26-09-91 */
    SCR_clear_image(SCR_TMP_IMAGE, SCR_DEFAULT);
    PG_read_flds(pg);

    if(!pg->pg_drw_mem)
	PG_read_drws(pg);

    PG_display_cadre(pg);
    if(pg->pg_scr_pos >= 0)
	PG_display_scr(pg);
    PG_display_title(pg);

    DRW_display(&(pg->pg_drws), pg->pg_line, pg->pg_column,
		  pg->pg_nb_line, pg->pg_nb_column);

    if(!pg->pg_drw_mem) PG_drw_free(pg);

    PGd_display_flds(pg);

    if(!pg->pg_fld_mem) PG_fld_free(pg);

    SCR_TMP_IMAGE->im_line   = _max_(0, line - box); /* JMP 11-03-92 */
    SCR_TMP_IMAGE->im_column = _max_(0, col - box);  /* JMP 11-03-92 */
    PR_write_im(SCR_TMP_IMAGE);
    SCR_free_image(SCR_TMP_IMAGE);
    SCR_TMP_IMAGE = 0;

    pg->pg_line   = line ;
    pg->pg_column = col  ;
    pg->pg_shadow = shad ;

    return(0);
}


/* ======================================================================
Imprime une PAGE à la position actuelle du curseur + c colonnes dans la
page d'impression. Si la page contient un cadre, ce cadre est imprimé.

S'il n'y a pas assez de place restant sur la page courante, un saut de
page est forcé.

Après l'impression de la PAGE, le curseur d'impression est placé en
première colonne sous la PAGE imprimée.

&NO
    La position de la PAGE dans l'écran (line et col) ne sont pas prises
    en compte pour décaler la page à l'impression. La fonction PR_page()
    par contre les utilise.

&EX
    PR_page_clf(pg1, 0);
&TX
&SA PR_page()
------------------------------------------------------------------------ */
PR_page_clf(pg, c)
PAGE    *pg;
int     c;
{
    int     box  = (pg->pg_box && pg->pg_scr_pos < 0) ? 1 : 0; /* JMP 11-03-92 */
    int     cc   = pg->pg_column;
    int     cl   = pg->pg_line;

    pg->pg_column   = c;
    pg->pg_line     = box; /* JMP 11-03-92 */

    PR_page(pg);

    PR_lf(pg->pg_nb_line + 2 * box);

    pg->pg_column   = cc;
    pg->pg_line     = cl;
    return(0);
}

/*NH*/
/* ===+==== PR_write_im ===================================
    write image to printer image
	im : IN : IMAGE *
   ==================================================== */

PR_write_im(im)
IMAGE   *im;
{
    PR_DEF  *pr = PR_CURRENT;

    if(PR_open()) return(-1);

    /* IF IMAGE OUT OF PAGE, FORMFEED */
    if(im->im_nb_line + im->im_line + pr->pr_cline > pr->pr_nl)
	PR_formfeed();

    /* WRITE IMAGE TO PAGE */
    SCR_im_to_im(pr->pr_im, im, pr->pr_cline, pr->pr_ccol);
    pr->pr_ccol += im->im_column + im->im_nb_column;
    return(0);
}


#include "scr.h"

/* ======================================================================
Affiche une page et attend la frappe d'une touche. Le commentaire
affiché est défini dans le fichier scr_err.txt (501).

&RT la valeur de la touche pressée
------------------------------------------------------------------------- */
int PG_display_slide(pg)
PAGE    *pg;
{
    return(PG_display_slide_cmt(pg, 0L)); /*JMP 19-05-95*/
}


/* ======================================================================
Affiche une page et attend la frappe d'une touche. Le commentaire
à afficher est défini dans cmt. Si cmt est nul, le texte repris dans
le fichier scr_err.txt (501) est affiché.

&RT la valeur de la touche pressée
------------------------------------------------------------------------- */
int PG_display_slide_cmt(pg, cmt)
PAGE    *pg;
U_ch    *cmt;
{
    STATUS  *sta;
    int     key;

    sta = SCR_save_status();
    SCR_cursor_off();
    PG_display_im(pg);
/*  if(pg->pg_cmt) {
	SCR_read_cmt_txt(pg->pg_cmt_pos);
	SCR_display_cmt(1);
	}
JMP 19-05-95*/
    if(cmt == 0) cmt = SCR_err_txt(501); /*JMP 19-05-95*/
    SCR_comment(cmt);                    /* JMP38 20-09-92 */

    /* BP_M 03-11-2001 01:06 */
    /*    while(SCR_hit_key()) SCR_get_key();     /* BP_M 03-11-2001 01:06
    /* Reporté avant PG_display_im() JMP 15-01-03 */

    PG_disable(pg);
    SCR_clear_key();            /* JMP 15-01-03 */
    key = SCR_get_key();  /* BP_M 26-12-2001 11:03 */
    PG_undisplay(pg);
    SCR_comment(0L);
    SCR_reset_status(sta);
    return(key);
}



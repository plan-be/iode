#include "scr.h"

DRW_display_nr(dw, line, col, nl, nc, rev)
DRAWS   *dw;
int     line, col, nl, nc, rev;
{
    int     i, attrf;
    DRAW    *drw;

    for(i = 0 ; i < dw->dw_nb_drws ; i++) {
	drw = dw->dw_drws + i;
	attrf = drw->drw_attr;
	if(rev < 0) drw->drw_attr = SCR_invert_attr(attrf); /* JMP_M 4.19 10-06-95 */
	if(rev > 0) drw->drw_attr = rev;                    /* JMP_M 4.19 10-06-95 */
	DRW_display_drw(drw, line, col, nl, nc);
	drw->drw_attr = attrf;
	}

    return(0);
}

DRW_display(dw, line, col, nl, nc)
DRAWS   *dw;
int     line, col, nl, nc;
{
    DRW_display_nr(dw, line, col, nl, nc, 0);
    return(0);
}

DRW_display_reverse(dw, line, col, nl, nc)
DRAWS   *dw;
int     line, col, nl, nc;
{
    DRW_display_nr(dw, line, col, nl, nc, 1);
    return(0);
}
/**/
/* =+====== DRW_display_drw ===============================
     Display one draw in a page.
     dw     : IN : pointer to the draws
     return : 0 if no problem , positive value else
 ============================================================== */

DRW_display_drw(drw, line, col, nl, nc)
DRAW    *drw;
int     line, col, nc, nl;
{
    int     r_line = 0, r_col = 0, r_nl, r_nc;


    if(drw->drw_line >= 0) r_line = drw->drw_line;
    if(drw->drw_column >= 0) r_col  = drw->drw_column;
    r_nl = drw->drw_nb_line;
    r_nc = drw->drw_nb_column;
    if(r_nl < 0)  r_nl   = nl - r_line;
    if(r_nc < 0)  r_nc   = nc - r_col;
    r_line += line;
    r_col  += col;
    SCR_set_attr(drw->drw_attr);
    switch(drw->drw_type) {
	case SCR_HLINE :
	    SCR_hline(drw->drw_size, r_line, r_col, r_nc); break;
	case SCR_VLINE :
	    SCR_vline(drw->drw_size, r_line, r_col, r_nl); break;
	case SCR_BOX :
	    SCR_box(drw->drw_size, r_line, r_col, r_nl, r_nc); break;
	case SCR_FILL:
	    SCR_fill(' ', r_line, r_col, r_nl, r_nc); break;
	case SCR_TEXT:
	    SCR_write_xattrs(r_line, r_col, 0, r_nc);
	    SCR_write(r_line, r_col, drw->drw_text, r_nc);
	    break;
	case SCR_REDRAW :
	    SCR_auto_table(r_line - 1, r_col - 1, r_nl + 2, r_nc + 2); break;
	}

    return(0);
}









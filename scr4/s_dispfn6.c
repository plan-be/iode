#include "scr.h"

#define FLD_RSIZE   (fld->fld_nb_column * fld->fld_nb_line) /* JMP 29-05-91 */
#define FLD_VSIZE   (fld->fld_ncv * fld->fld_nlv)           /* JMP 29-05-91 */
#define FLD_ISLBR   ((fld->fld_pv > 0) ? 1 : 0)
#define FLD_ISRBR   ((fld->fld_pv + FLD_VSIZE - FLD_ISLBR) < FLD_RSIZE)


/*NH*/
PG_display_all_grpbox(pg, rev)
PAGE    *pg;
int     rev;
{
    int     attr, i, j, k, pos;
    GRPBOX  *gr;
    IMAGE   *im;

    if(!pg->pg_nb_grpbox) return(0);
    PG_read_flds(pg);
    gr = pg->pg_grpbox;
    for(i = 0 ; i < pg->pg_nb_grpbox ; i++, gr++) {
	attr = gr->gr_back;
	if(rev < 0) attr = SCR_invert_attr(attr);
	if(rev > 0) attr = rev;
	PG_set_lc_grp(pg, i);
	SCR_set_attr(attr);
	if(gr->gr_box)
	    SCR_box(gr->gr_box, pg->pg_line + gr->gr_line - 1, pg->pg_column + gr->gr_col - 1, gr->gr_nbl + 2, gr->gr_nbc + 2);
	if(gr->gr_title)
	    SCR_write(pg->pg_line + gr->gr_line - 1, pg->pg_column + gr->gr_col + 1, gr->gr_title, strlen(gr->gr_title));

	if(SCR_TMP_IMAGE) im = SCR_TMP_IMAGE;
	else              im = SCR_SCREEN;
	if(attr != pg->pg_back) {
	    pos = 2 * (gr->gr_line * pg->pg_nb_column + gr->gr_col) + 1;
	    for(j = 0 ; j < gr->gr_nbl ; j++) {
		for(k = 0 ; k < gr->gr_nbc ; k++)
		    im->im_cells[pos + k * 2] = attr;
		pos += pg->pg_nb_column * 2;
	    }
	}
    }

    return(0);
}


/*NH*/
PG_edfl_isrbr(fld, edtxt)
FIELD            *fld;
unsigned char    *edtxt;
{
    int     last;

    if((fld->fld_pv + FLD_VSIZE - FLD_ISLBR) >= FLD_RSIZE) return(0);
    last = PG_edfl_calc_eof(fld, edtxt) - 1;
    last = _max_(0, last);
    if(last < fld->fld_pv + FLD_VSIZE - FLD_ISLBR) return(0);
    return(1);
}

/*NH*/
PG_edfl_calc_eof(fld, edtxt)
FIELD   *fld;
char    *edtxt;
{
    int i;

    for(i = FLD_RSIZE; i > 0 ; i--) if(edtxt[i - 1] != ' ') break;
    if(i == FLD_RSIZE) i--;
    return(i);
}




#include "scr.h"

/* =+====== SCR_extract_IMAGE ========================================
    Save a part of an image in a structure IMAGE.
	in_im   : IN : input image
	line : IN : first line to save
	column : IN : first column to save
	nb_line : IN : nb of lines to save
	nb_column : IN : nb of columns to save
	return : pointer to a structure IMAGE or 0 if error.
   ============================================================== */

#ifdef __cplusplus
IMAGE *SCR_extract_image(
IMAGE   *in_im,
int     line,
int     column,
int     nb_line,
int     nb_column
)
#else
IMAGE *SCR_extract_image(in_im, line, column, nb_line, nb_column)
IMAGE   *in_im;
int     line,
	column,
	nb_line,
	nb_column;
#endif
{
    IMAGE   *im;
    int     i,
	    j;
    int     nc;
    int     col;
    int     pos;
    unsigned char    *cl;

    if(in_im == 0) return((IMAGE *)0);

    line      = _min_(_max_(0, line),      in_im->im_nb_line - 1);
    column    = _min_(_max_(0, column),    in_im->im_nb_column - 1);
    nb_line   = _min_(_max_(1, nb_line),   in_im->im_nb_line - line);
    nb_column = _min_(_max_(1, nb_column), in_im->im_nb_column - column);

    im = SCR_alloc_image(nb_line, nb_column);
    cl = im->im_cells;

    im->im_line       = line;
    im->im_column     = column;

    nc = 2 * nb_column;
    pos = 2 * (line * in_im->im_nb_column  + column);
    for(i = 0 ; i < nb_line ; i++) {
	memcpy(cl, in_im->im_cells + pos, nc);
	cl += nc;
	pos += in_im->im_nb_column * 2;
	memcpy(im->im_xattrs + i * nb_column, column + in_im->im_xattrs + (line + i) * in_im->im_nb_column, nb_column); /* JMP 11-01-2004 */
    }

    return(im);
}
/* ====================================================================
Sauve une partie de l'écran dans une structure IMAGE.
&RT le pointeur vers la structure allouée
&EX
    IMAGE   *im;

    im = SCR_save_image(2, 45, 10, 15);
    ...
    SCR_display_image(im);
    SCR_free_image(im);
&TX
&SA SCR_free_image(), SCR_display_image()
=======================================================================*/

#ifdef __cplusplus
IMAGE *SCR_save_image(
int     line,
int     column,
int     nb_line,
int     nb_column
)
#else
IMAGE *SCR_save_image(line, column, nb_line, nb_column)
int     line,
	column,
	nb_line,
	nb_column;
#endif
{
    return(SCR_extract_image(SCR_SCREEN, line, column, nb_line, nb_column));
}

/* ====================================================================
Affiche une IMAGE dans l'écran.
&EX
    IMAGE   *im;

    im = SCR_save_image(2, 45, 10, 15);
    ...
    SCR_display_image(im);
    SCR_free_image(im);
&TX
&SA SCR_save_image()
=======================================================================*/

SCR_display_image(im)
IMAGE   *im;
{
    int     i;
    extern IMAGE   **PGIM_LIST;
    extern int     NB_PGIM;

    if(im == 0) return(0);

    for(i = 0 ; i < NB_PGIM ; i++)
	if(im == PGIM_LIST[i]) break;

    if(i == NB_PGIM) {
	SCRd_display_image(im);
	return(0);
	}

    PG_undisplay(PAGE_PTR((int)PGIM_LIST[i]));
    PGIM_LIST[i] = 0;
}

/*NH*/
SCRd_display_image(im)
IMAGE   *im;
{
    int     i, j, nl, nc, attr, col;
    unsigned char    *cl, buffer[512], *xattrs;

    if( (im == 0) || (im->im_cells == 0)) return(0);
    cl = (unsigned char *)(im->im_cells);
    xattrs = im->im_xattrs;
    nl = im->im_nb_line;
    nc = im->im_nb_column;

    for(i = 0 ; i < nl ; i++) {
	col = 0;
	while(col < nc) {
	    j = 0;
	    attr = cl[1];
	    SCR_set_attr(attr);
	    while(attr == cl[1] && col + j < nc) {
		buffer[j++] = cl[0];
		cl += 2;
		}

	    SCR_write_xattrs(i + im->im_line, col + im->im_column, xattrs + nc * i + col, j);
	    //SCR_write_xattrs(i + im->im_line, col + im->im_column, j, 0);
	    SCR_write(i + im->im_line, col + im->im_column, buffer, j);
	    col += j;
	    }
	}
    return(0);
}

/* ====================================================================
Copie une image.
&EX
    IMAGE   *im;

    im = SCR_copy_image(from_im);
&TX
&SA SCR_save_image()
=======================================================================*/

#ifdef __cplusplus
IMAGE *SCR_copy_image(
IMAGE *in
)
#else
IMAGE *SCR_copy_image(in)
IMAGE *in;
#endif
{
    return(SCR_extract_image(in, in->im_line, in->im_column,
				 in->im_nb_line, in->im_nb_column));
}










#include "scr.h"

/*NH*/
/* ===+==== SCR_im_to_im ===================================
    write image to image
	ou_im : IN : output image
	in_im : IN : input image
	line  : IN : output line
	col   : IN : output col
   ==================================================== */

SCR_im_to_im(ou_im, in_im, line, col)
IMAGE   *ou_im, *in_im;
int     line, col;
{
    unsigned char    *o_cells, *i_cells;
    int     i, nl, nc;

    line += in_im->im_line;
    col  += in_im->im_column;
    o_cells = ou_im->im_cells;
    i_cells = in_im->im_cells;
    nl = in_im->im_nb_line;
    nc = in_im->im_nb_column;
    if(nl + line > ou_im->im_nb_line)   nl = ou_im->im_nb_line - line;
    if(nc + col  > ou_im->im_nb_column) nc = ou_im->im_nb_column - col;
    if(nl <= 0 || nc <= 0) return(0);

    for(i = 0 ; i < nl ; i++) {
	memcpy(o_cells + 2 * ((line + i) * ou_im->im_nb_column + col),
		i_cells + 2 * i * in_im->im_nb_column,
		nc * 2);
	memcpy(ou_im->im_xattrs + (line + i) * ou_im->im_nb_column + col, /* JMP 11-01-2004 */
	       in_im->im_xattrs + i * in_im->im_nb_column,
		nc);

	}

    return(0);
}

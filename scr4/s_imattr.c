#include "scr.h"

/*NH*/
SCR_set_image_attr(im, line, col, nl, nc, attr)
IMAGE   *im;
int     line, col, nl, nc, attr;
{
    int     i, j, base,
	    inl = im->im_nb_line,
	    inc = im->im_nb_column;

    if(im == 0) return(0);
    if(line < 0) {nl += line; line = 0;}
    if(col  < 0) {nc += col ; col  = 0;}
    for(j = 0 ; j < nl && line + j < inl; j++) {
	base = ((j + line) * inc + col) * 2;
	for(i = 0 ; i < nc && col + i < inc; i++)
	    im->im_cells[base + 2 * i + 1] = attr;
	}

    return(0);
}



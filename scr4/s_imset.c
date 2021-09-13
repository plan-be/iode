#include "scr.h"

/*NH*/
SCR_set_image(im, text, line, col, nc)
IMAGE   *im;
U_ch    *text;
int     line, col, nc;
{
    int     i, inc = im->im_nb_column;

    if(im == 0 || line < 0 || line >= im->im_nb_line) return(0);
    if(col < 0) {nc += col ; text -= col; col = 0;}
    for(i = col ; i < inc && i - col < nc; i++)
	im->im_cells[2 * (line * inc + i)] = text[i];

    return(0);
}


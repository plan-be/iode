#include "scr.h"

/*NH*/
SCR_clear_image(im, attr)
IMAGE   *im;
int     attr;
{
    int     i, j, k, nb = 2 * im->im_nb_column;

    for(j = 0 ; j < nb ; j+=2) {
	im->im_cells[j] = ' ';
	im->im_cells[j + 1] = attr;
    }

    for(i = 1 ; i < im->im_nb_line ; i++) {
	memcpy(im->im_cells + i * nb, im->im_cells, nb);
	}

    memset(im->im_xattrs, 0, im->im_nb_line * im->im_nb_column); /* JMP 11-01-2004 */

    return(0);
}


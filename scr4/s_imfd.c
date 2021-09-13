#include "scr.h"

/*NH*/
/* =+====== SCR_DISPLAY_IM_FD ===================================
     Display an IMAGE in file descripteur
     fd : OUT: pointer to the file descriptor
     im : IN : pointer to the struct IMAGE
   ============================================================== */

SCR_display_im_fd(fd, im)
FILE    *fd;
IMAGE   *im;
{
    int     i, j, nl, nc;
    unsigned char    *cl, buffer[256];

    if( (im == 0) || (im->im_cells == 0)) return(0);
    cl = (unsigned char *)(im->im_cells);
    nl = im->im_nb_line;
    nc = im->im_nb_column;

    for(i = 0 ; i < nl ; i++) {
	for(j = 0 ; j < nc ; j++, cl += 2)
	    buffer[j] = *cl;
	fwrite(buffer, nc, 1, fd);
	fprintf(fd, "\n");
    }
    return(0);
}


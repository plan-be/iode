#include "scr.h"

/*NH*/
SCR_write_im(line, col, text, len)
int     line,
	col,
	len;
unsigned char *text;
{
    int     i;
    int     pos;
    unsigned char    *cl;

    line -= SCR_TMP_IMAGE->im_line;
    if(line < 0 || line >= SCR_TMP_IMAGE->im_nb_line)
	return(0);
    col -= SCR_TMP_IMAGE->im_column;
    if(col < 0) {
	len += col;
	col = 0;
    }
    if(col + len > SCR_TMP_IMAGE->im_nb_column)
	len = SCR_TMP_IMAGE->im_nb_column - col;


    pos = 2 * (line * SCR_TMP_IMAGE->im_nb_column + col);
    cl = SCR_TMP_IMAGE->im_cells + pos;
    for(i = 0 ; i < len ; i++) {
	*(cl++) = *(text++);
	*(cl++) = SCR_CURRENT_ATTR;
	}
    return(0);
}


SCR_write_im_xbox(int line, int col, int nl, int nc, int set)
{
    int     i, j;
    unsigned char xattrs[1024];  /* JMP 08-01-11 */

    for(i = 0 ; i < nl ; i++) {
	for(j = 0 ; j < nc ; j++) {
	    xattrs[j] = 0;
	    if(set) {
		if(i == 0) xattrs[j] += 1;
		if(j == 0) xattrs[j] += 8;
		if(i == nl - 1) xattrs[j] += 4;
		if(j == nc - 1) xattrs[j] += 2;
		}
	    }
	SCR_write_xattrs(line + i, col, xattrs, nc);
	}
    return(0);
}

SCR_write_xattrs_im(int line, int col, U_ch *xattrs, int nc)
{
    int     i, j, pos;
    IMAGE   *im;

    im = SCR_TMP_IMAGE;

    line -= im->im_line;
    if(line < 0 || line >= im->im_nb_line) return(0);
    col -= im->im_column;
    if(col < 0) {
	nc += col;
	col = 0;
	}
    if(col >= im->im_nb_column) return(0);
    if(col + nc > im->im_nb_column)
	nc = im->im_nb_column - col;

    if(xattrs == 0)
	memset(im->im_xattrs + (line * im->im_nb_column) + col, 0, nc);
    else
	memcpy(im->im_xattrs + (line * im->im_nb_column) + col, xattrs, nc);
}

SCR_clear_xattrs_im(int line, int col, int nl, int nc)
{
    int     i;

    for(i = 0 ; i < nl ; i++)
	SCR_write_xattrs_im(line + i, col, 0, nc);
}

/*
#if defined(SCRW32) || defined(DOSW32)
*/
SCR_write_xattrs_text(int line, int col, unsigned char *xattrs, int len)
{
}

SCR_clear_xattrs_text(int line, int col, int nl, int nc)
{
}
/*
#endif
*/

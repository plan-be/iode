#include "scr.h"


/* ====================================================================
Ecrit dans l'écran le texte text (null terminated string) centré
dans nc colonnes. La position est line, col.

&SA SCR_write()
=======================================================================*/

SCR_write_center(line, col, text, nc)
unsigned char *text;
int     line, col, nc;
{
    int     lg, pos;

    if(text == 0) return(0);
    lg = strlen(text);
    lg = _min_(lg, nc);
    pos = col + (nc - lg) / 2;
    SCR_write_xattrs(line, pos, 0, lg);
    SCR_write_im_xbox(line, pos, 1, lg, 1);
    SCR_write(line, pos, text, lg);
    return(0);
}

/*NH*/
SCR_write_scr_mem(mem, line, col, text, lg, attr)
char    mem[];
int     line;
int     col;
char    text[];
int     lg;
int     attr;
{
    int     i;
    int     pos;

    pos = 2 * (line * SCR_PAGE_SIZE[1] + col);

    for(i = 0 ; i < lg ; i++) {
	mem[pos++] = text[i];
	mem[pos++] = attr;
	}
    return(0);
}

/*NH*/
SCR_write_xattrs_mem(xmem, line, col, xattrs, lg) /* JMP 11-01-2004 */
char    xmem[];
int     line;
int     col;
int     lg;
unsigned char *xattrs;
{
    if(xattrs)
	memcpy(xmem + line * SCR_PAGE_SIZE[1] + col, xattrs, lg);
    else
	memset(xmem + line * SCR_PAGE_SIZE[1] + col, 0, lg);

    return(0);
}








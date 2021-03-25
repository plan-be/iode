#include "s_strs.h"
#include <stdio.h>

/* ====================================================================
Lit une ligne dans le fichier ouvert point� par fd. La ligne lue
est stock�e dans le buffer buf de longueur maximum lg. Si la longueur
de la ligne exc�de lg caract�res, les derniers caract�res sont saut�s.

Le caract�re \n est lu mais n'est pas stock� dans buf. buf se termine
par le caract�re \0.

&RT la longueur de la ligne lue ou -1 si la fin du fichier est atteinte
    avant qu'un caract�re ne soit lu.
&EX
    while(1) {
	if(SCR_read_line(fd, buf, 130) < 0) break;
	SCR_add_ptr(&tbl, &n, buf);
	}
    SCR_add_ptr(&tbl, &n, NULL);
&TX
&SA SCR_add_ptr()
=======================================================================*/

SCR_read_line(fd, buf, lg)
FILE            *fd;
unsigned char   *buf;
int             lg;
{
    int     i = 0, ch;

    while(1) {
	ch = getc(fd);
	if(ch == '\n' || ch == EOF) {
	    buf[i] = 0;
	    if(i == 0 && ch == EOF) return(-1);
	    return(i);
	    }
	if(i < lg - 1) {  /* JMP 28-05-99 */
	    buf[i] = ch;
	    i++;
	    }
	}
}




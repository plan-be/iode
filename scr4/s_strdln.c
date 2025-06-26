#include "s_strs.h"
#include <stdio.h>

/* ====================================================================
Lit une ligne dans le fichier ouvert pointé par fd. La ligne lue
est stockée dans le buffer buf de longueur maximum lg. Si la longueur
de la ligne excède lg caractères, les derniers caractères sont sautés.

Le caractère \n est lu mais n'est pas stocké dans buf. buf se termine
par le caractère \0.

&RT la longueur de la ligne lue ou -1 si la fin du fichier est atteinte
    avant qu'un caractère ne soit lu.
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




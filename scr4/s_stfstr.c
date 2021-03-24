#include "s_strs.h"

/* ===========================================================
Recherche le string str de longeur lg dans le fichier filename et
retourne la position du d�but du string.


&RT -1 si le string ne put �tre trouv�
    -2 si le fichier ne peut �tre ouvert en lecture
    position si le string est trouv�.

&NO Les cas d�licats comme la recherche de "12123" dans "1212123"
    sont correctement trait�s.

============================================================== */

long SCR_FindStringPos(filename, str, lg)
unsigned char   *filename, *str;
int             lg;
{
    FILE            *fd;
    int             ch, pos;
    long            rc = -1L;
/*    long            ftell(); /* JMP 04-01-98 */
    unsigned char   *buf;

#ifdef UNIX
    fd = fopen(filename, "r");
#else
    fd = fopen(filename, "rb");
#endif
    if(fd == 0) return(-2L);

    buf = (unsigned char *)SCR_malloc(lg + 1);
    while(1) {
	ch = getc(fd);
	if(ch == EOF) break;
	if(ch != str[0]) continue;
	buf[0] = ch;
	if(lg - 1 != fread(buf + 1, 1, lg - 1, fd)) break;
    ag: if(memcmp(str, buf, lg) == 0) {
	    rc = ftell(fd) - lg;
	    break;
	    }
	pos = U_pos(str[0], buf + 1);
	if(pos >= 0) {
	    memcpy(buf, buf + pos + 1, lg - pos - 1);
	    if(pos + 1 != fread(buf + lg - pos - 1, 1, pos + 1, fd)) break;
	    goto ag;
	    }
	}

    SCR_free(buf);
    fclose(fd);
    return(rc);
}









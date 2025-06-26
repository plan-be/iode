#include"s_mat.h"

/* ====================================================================
Dump du contenu d'une matrice.

On passe en paramètres: le file descriptor du fichier output (stdout, ...),
le pointeur sur la matrice à imprimer et un titre général.


&RT
néant (0)

&EX FILE *fd;
    MAT  *m1;
    ....
    M_dump(fd, m1, "TEST");

&TX
&SA M_dump(), M_printf(), M_tprinf(), M_scroll()

======================================================================= */

M_dump(fd, m1, tit)
FILE    *fd;
MAT     *m1;
char    *tit;
{
    int     i, j;

    fprintf(fd, "\n%s - [%d,%d]\n", tit, NL1, NC1);
    for(i = 0 ; i < NL1 ; i++) {
	fprintf(fd, "\n%3d : ", i);
	for(j = 0 ; j < NC1 ; j++) fprintf(fd, "%8.8f ", (float)MATE1(i, j));
	}
    fprintf(fd, "\n");

    return(0);
}








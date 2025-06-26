#include"s_mat.h"

/* ====================================================================
Impression d'une matrice dans un fichier au format tableau A2M.

On passe en paramètres: le file descriptor du fichier output, le pointeur
sur la matrice à imprimer, un titre général, un tableau contenant les titres
de lignes et un tableau comprenant les titres de colonnes.



&RT
La fonction retourne EOF en cas d'erreur d'accès au fichier output et 0 en
cas de succès.

&EX FILE *fd;
    MAT  *m1;
    char *ligne[] = {"l1", "l2", ...};
    char *col[] = {"c1", "c2", ...};
    ....
    M_tprint(fd, m1, "TEST", ligne, col)

&TX
&SA M_dump(), M_printf(), M_tprinf(), M_scroll()

======================================================================= */

M_tprintf(fd, m1, title, t_line, t_col)
FILE *fd;
MAT  *m1;
char *title;
char **t_line;
char **t_col;
{
    int i, j, nc_tbl;

    /* dump */
    nc_tbl = M_NL(m1)+1;

    /* entete et titre */
    if(fprintf(fd, ".tb %d\n", nc_tbl) == EOF) return(EOF);
    if(fprintf(fd, ".sep &\n") == EOF) return(EOF);
    if(fprintf(fd, ".tl\n") == EOF) return(EOF);
    if(fprintf(fd, "&%dL\\b%s\\B\n", nc_tbl, title) == EOF) return(EOF);
    if(fprintf(fd, ".tl\n") == EOF) return(EOF);

    /* premiere ligne = titre colonne*/

    if(fprintf(fd,"&1L ") == EOF) return(EOF);

    for(j = 0; j < M_NC(m1); j++)
	if(fprintf(fd, "&1C\\b%s\\B", t_col[j]) == EOF) return(EOF);

    if(fprintf(fd,"\n.tl\n") == EOF) return(EOF);

    /* lignes suivantes */

    for(i = 0; i < M_NL(m1); i++)
    {
	/* titre de la ligne */
	if(fprintf(fd,"&1L\\b%s\\B", t_line[i]) == EOF) return(EOF);

	for(j = 0; j < M_NC(m1); j++)
	    if(fprintf(fd,"&1L%f", MATE1(i,j)) == EOF) return(EOF);

	if(fprintf(fd,"\n") == EOF) return(EOF);
    }
    if(fprintf(fd, ".te\n") == EOF) return(EOF);

    return(0);
}

/* ====================================================================
Impression d'une matrice dans un fichier au format tableau A2M.

On passe en paramètres: le file descriptor du fichier output, le pointeur
sur la matrice à imprimer et un titre.

Chaque ligne et colonne sont imprimées avec un titre correspondant à
leur numéro d'ordre..
&RT
La fonction retourne EOF en cas d'erreur d'accès au fichier output et 0 en
cas de succès.

&EX FILE *fd;
    MAT  *m1;
    ....
    M_print(fd, m1, "TEST")

&TX
&SA M_dump(), M_printf(), M_tprinf(), M_scroll()

======================================================================= */

M_printf(fd, m1, title)
FILE *fd;
MAT  *m1;
char *title;
{
    int i, j, nc_tbl;

    /* dump */
    nc_tbl = M_NL(m1)+1;

    /* entete et titre */
    if(fprintf(fd, ".tb %d\n", nc_tbl) == EOF) return(EOF);
    if(fprintf(fd, ".sep &\n") == EOF) return(EOF);
    if(fprintf(fd, ".tl\n") == EOF) return(EOF);
    if(fprintf(fd, "&%dL\\b%s\\B\n", nc_tbl, title) == EOF) return(EOF);
    if(fprintf(fd, ".tl\n") == EOF) return(EOF);

    /* premiere ligne = n° colonnes */

    if(fprintf(fd,"&1L ") == EOF) return(EOF);

    for(j = 0; j < M_NC(m1); j++)
	if(fprintf(fd, "&1L\\b%d\\B", j+1) == EOF) return(EOF);

    if(fprintf(fd,"\n.tl\n") == EOF) return(EOF);

    /* lignes suivantes */

    for(i = 0; i < M_NL(m1); i++)
    {

	if(fprintf(fd,"&1L\\b%d\\B",i+1) == EOF) return(EOF);

	for(j = 0; j < M_NC(m1); j++)
	    if(fprintf(fd,"&1L%f", MATE1(i,j)) == EOF) return(EOF);

	if(fprintf(fd,"\n") == EOF) return(EOF);
    }
    if(fprintf(fd, ".te\n") == EOF) return(EOF);

    return(0);
}










#include "s_mat.h"
/* ====================================================================
Rempli les éléments de la matrice dest à lartir de la ligne l et de la
colonne c avec les éléments de la matrice source.

La matrice destination doit avoir été allouée avec les dimentions adéquates.
Les éléments qui "déborderaient" de lka matrice destination sont éliminés.
&RT
La fonction retourne un pointeur sur la matrice dest. Si une des matrices
est identifiée par un pointeur NULL, la variable globale M_errno est
positionnnée et la fonction retourne un pointeur NULL.

&EX
    M_set(dest, src, l, c);
&TX
&SA M_free(), M_alloc(), M_copy(), M_dup().
======================================================================= */

#ifdef __cplusplus
MAT *M_set(
MAT     *m2,
MAT     *m1,
int     lig,
int     col
)
#else
MAT     *M_set(m2, m1, lig, col)
MAT     *m2, *m1;
int     lig, col;
#endif
{
    int i, j, k, l;

    if(m1 == NULL || m2 == NULL)
    {
	M_errno = M_EXTRDIM_ERR;
	return(0);
    }

    for(i = lig, k = 0; (i < NL2 && k < NL1); i++, k++)
	for(j = col, l = 0; (j < NC2 && l < NC1); j++, l++)
	    MATE2(i, j) = MATE1(k, l);

    return(m2);
}








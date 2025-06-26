#include "s_mat.h"
/* ====================================================================
Annule tous les éléments d'une matrice  m1 (nl1 x nc1).
&RT
Si la matrice n'exista pas, la variable M_errno est positionnée
et retournée, sinon la fonction retourne 0.

&EX
    M_clear(m1);
&TX
&SA M_free(), M_alloc()
======================================================================= */

M_clear(m1)
MAT   *m1;
{
    register int    i, j;

    if(m1 == 0)   return(M_errno = M_CLNULL_ERR);

    for(i = 0; i < NL1; i++)
	for(j = 0; j < NC1; j++)
	    MATE1(i,j) = 0.0;

    return(0);
}



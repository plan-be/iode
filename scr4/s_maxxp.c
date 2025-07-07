#include "s_mat.h"
/* ====================================================================
Calcule le produit de m1 (nl1 x nc1) par sa transposée (nc1 x nl1)
et stocke le résultat dans m2 (nl1 x nl1).

&RT
Si m2 est nul, une matrice de dimension (nl1, nl1) est allouée et
retournée (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace mémoire insuffisant).
Si m2 n'est pas nul, le résultat y est stocké et m2 est
retourné. Les dimensions de m2 doivent être correctes sinon l'opération
n'est pas effectuée , la variable M_errno est positionnée et la
fonction retourne un pointeur nul.

&EX
    M_xxprim(m2, m1);
    m2 = M_xxprim((MAT*)0, m1)
&TX
&SA M_free(), M_alloc(), M_trans().
======================================================================= */

#ifdef __cplusplus
MAT *M_xxprim(
MAT *m2,
MAT *m1
)
#else
MAT *M_xxprim(m2, m1)
MAT *m2, *m1;
#endif
{
    int   i, j, k;
    MREAL  sum;


    if (m1 == 0)
    {
	M_errno = M_TRAN_ERR;
	return(0);
    }

    if(m2 == 0)
    {
	m2 = (MAT *) M_alloc(NL1, NL1);
	if(m2 == 0)
	{
	    M_errno = M_MEM_ERR;
	    return(m2);
	}
    }
    else
	if(NL1 != NL2 || NL1 != NC2 )
	{
	    M_errno = M_TRAN_ERR;
	    return((MAT *)0);
	}


    for(i = 0; i < NL1; i++)
	for(j = 0; j <= i; j++)
	{
	    sum = 0.0;
	    for(k = 0; k < NC1; k++) sum += MATE1(i,k) * MATE1(j,k);
	    MATE2(i,j) = MATE2(j,i) = sum;
	}
    return(m2);
}

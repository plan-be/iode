#include "s_mat.h"

/* ====================================================================
Effectue le produit des deux matrices m1 (nl1 x nc1) et m2 (nl2 x nc2).
Le résultat est stocké dans m3 (nl1 * nc2).

Si m3 est nul, une matrice de dimension (nl1, nc2) est allouée et
retournée (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace mémoire insuffisant). Si m3 n'est pas nul,
le résultat y est stocké et m3 est retourné.
Les dimensions de m3 doivent être correctes sans quoi le
produit n'est pas calculé , la variable M_errno est positionnée et la
fonction retourne un pointeur nul.

&EX
    M_prod(m3, m2, m1);
    ou
    m3 = M_prod(0L, m2, m1);
&TX
&SA M_calc(), M_free(), M_alloc().
======================================================================= */

#ifdef SCRCPP
MAT *M_prod(
MAT     *m3,
MAT     *m1,
MAT     *m2
)
#else
MAT *M_prod(m3, m1, m2)
MAT   *m3, *m1, *m2;
#endif

{
    register   int  i, j, k;
    register   MREAL sum, x, y;

    if(NC1 != NL2 )
    {
	M_errno = M_PROD_ERR;
	return((MAT *)0);
    }
    if(m3 == 0)
    {
	m3 = (MAT *) M_alloc(NL1, NC2);
	if(m3 == 0)
	{
	    M_errno = M_MEM_ERR;
	    return(m3);
	}
    }
    else
	if(NL1 != NL3 || NC2 != NC3)
	{
	    M_errno = M_PROD_ERR;
	    return((MAT *)0);
	}

    for(i = 0; i < NL1; i++)
    {
	for(j = 0; j < NC2; j++)
	{
	    sum = 0.0;
	    for(k = 0; k < NC1; k++)
	    {
		x = MATE1(i,k);
		y = MATE2(k,j);
		sum += x * y;
	    }
	    MATE3(i,j) = sum;
	}
    }
    return(m3);
}

















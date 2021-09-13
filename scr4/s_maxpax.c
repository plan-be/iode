#include "s_mat.h"
/* ====================================================================
Soient les matrices X (nl1 x nc1) et A (nl1 x nl1), la fonction calcule
le produit X'A X ( X' nc1 x nl1 ‚tant la transpos‚e de X),
et stocke le r‚sultat dans m3 (nc1 x nc1).

&RT
Si m3 est nul, une matrice de dimension (nc1, nc1) est allou‚e et
retourn‚e (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace m‚moire insuffisant).
Si m3 n'est pas nul, le r‚sultat y est stock‚ et m3 est
retourn‚. Les dimensions de m3 doivent ˆtre correctes sinon l'op‚ration
n'est pas effectu‚e , la variable M_errno est positionn‚e et la
fonction retourne un pointeur nul.

&EX
    M_xprimax(m3, m1, m2);
    m3 = M_xprimax((MAT*)0, m1, m2)
&TX
&SA M_free(), M_alloc(), M_trans().
======================================================================= */

#ifdef SCRCPP
MAT *M_xprimax(
MAT     *m1,
MAT     *m2,
MAT     *m3
)
#else
MAT *M_xprimax(m3, m1, m2)
MAT   *m1, *m2, *m3;
#endif
{
    int   i, j, k, l;
    MREAL  suml, sumk;

    if(NL1 != NL2 || m1 == 0 || m2 == 0 || NC2 != NL1)
    {
	M_errno = M_PROD_ERR;
	return((MAT *)0);
    }
    if(m3 == 0)
    {
	m3 = (MAT *) M_alloc(NL1, NL1);
	if(m3 == 0)
	{
	    M_errno = M_MEM_ERR;
	    return(m3);
	}
    }
    else
	if(NC1 != NL3 || NL3 != NC3)
	{
	    M_errno = M_PROD_ERR;
	    return((MAT *)0);
	}

    for(i = 0; i < NC1; i++)
	for(j = 0; j < NC1; j++)
	{
	    sumk = 0.0;
	    for(k = 0; k < NL1; k++)
	    {
		suml = 0.0;
		for(l = 0; l < NL1; l++) suml += MATE2(k,l) * MATE1(l,j);
		sumk +=  MATE1(k,i) * suml;
	    }
	    MATE3(i,j) = sumk;
	}
    return(m3);
}

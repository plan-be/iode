#include "s_mat.h"


/* ====================================================================
Calcule la transposée de m1 (nl1 x nc1) et stocke le résultat
dans m2 (nc1 x nl1).
&RT
Si m2 est nul, une matrice de dimension (nc1, nl1) est allouée et
retournée (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace mémoire insuffisant).
Si m2 n'est pas nul, le résultat y est stocké et m2 est
retourné. Les dimensions de m2 doivent être correctes sinon l'opération
n'est pas effectuée , la variable M_errno est positionnée et la
fonction retourne un pointeur nul.

&EX
    M_trans(m2, m1);
    m2 = M_trans((MAT*)0, m1)
&TX
&SA M_free(), M_alloc()
======================================================================= */

#ifdef __cplusplus
MAT *M_trans(
MAT     *m2,
MAT     *m1
)
#else
MAT *M_trans(m2, m1)
MAT   *m2, *m1;
#endif
{
   int          i, j;
   MREAL         sw;


    if(m2 == 0)
    {
	m2 = (MAT *) M_alloc(NC1, NL1);
	if(m2 == 0)
	{
	    M_errno = M_MEM_ERR;
	    return(m2);
	}
    }
    else
	if(NC1 != NL2 || NL1 != NC2 )
	{
	    M_errno = M_TRAN_ERR;
	    return((MAT *)0);
	}

    if(m1 == m2 && NL1 == NC1) { /* Transposition sur place */
	for(i = 0; i < NL1; i++)
	    for(j = 0; j < NC1; j++) {
		sw = MATE1(i,j);
		MATE1(i,j) = MATE1(j,i);
		MATE1(j,i) = sw;
		}
	return(m1);
	}

    for(i = 0; i < NC1; i++)
	for(j = 0; j < NL1; j++)
	    MATE2(i,j) = MATE1(j,i);

    return(m2);
}
















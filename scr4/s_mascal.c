#include "s_mat.h"
/* ====================================================================
Modifie la matrice m1 (nl1 x nc1) par un facteur d'�chelle et stocke
le r�sultat dans m2 (nl1 x nc1).
&RT
Si m2 est nul, une matrice de dimension (nl1, nc1) est allou�e et
retourn�e (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace m�moire insuffisant).
Si m2 n'est pas nul, le r�sultat y est stock� et m2 est
retourn�. Les dimensions de m2 doivent �tre correctes sinon l'op�ration
n'est pas effectu�e , la variable M_errno est positionn�e et la
fonction retourne un pointeur nul.
La matrice source n'est pas modifi�e.

&EX
    M_scale(m2, m1, 5.18);
    m2 = M_scale((MAT*)0, m1, 3.1416)
&TX
&SA M_free(), M_alloc()
======================================================================= */

#ifdef SCRCPP
MAT *M_scale(
MAT     *m1,
MAT     *m2,
MREAL   scale
)
#else
MAT *M_scale(m2, m1, scale)
MAT     *m1, *m2;
MREAL    scale;
#endif

{
    register int    i, j;

    if(m2 == 0)
    {
	m2 = M_alloc(NL1, NC1);
	if(m2 == 0) return(m2);

    }

    if(NC1 != NC2 || NL1 != NL2 )
    {
	M_errno = M_SCALEDIM_ERR;
	return((MAT *)0);
    }


    for(i = 0; i < NL1; i++)
	for(j = 0; j < NC1; j++)
	    MATE2(i,j) = MATE1(i,j) * scale;

    return(m2);
}


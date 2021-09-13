#include "s_mat.h"

/* ====================================================================
Calcule la trace de m1 (carr‚e) et stocke le r‚sultat dans m2 (matrice de
1 ligne).

&RT
Si m2 est nul, une matrice de dimension (1, nc1) est allou‚e et
retourn‚e (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace m‚moire insuffisant).
Si m2 n'est pas nul, le r‚sultat y est stock‚ et m2 est
retourn‚. Les dimensions de m2 doivent ˆtre correctes sans quoi
la fonction retourne un pointeur null et la variable M_errno est
positionn‚e.

&EX
    res = M_trace(0l, m1);
    ou M_trace(res, m1)
&TX
&SA M_free(), M_alloc()
======================================================================= */

#ifdef SCRCPP
MAT *M_trace(
MAT     *m2,
MAT     *m1
)
#else
MAT *M_trace(m2, m1)
MAT   *m2, *m1;
#endif

{
    int  i;

    if(m2 == 0)
    {
	m2 = M_alloc(1, NL1);
	if(m2 == 0)
	{
	    M_errno = M_MEM_ERR;
	    return(m2);
	}
    }
    else
	if(NC1 != NL1 || NL1 != NC2 || NL2 != 1)
	{
	    M_errno = M_TRDIM_ERR;
	    return((MAT *)0);
	}

    for(i = 0; i < NL1; i++) MATE2(0,i) = MATE1(i,i);
    return(m2);
}


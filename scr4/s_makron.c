#include "s_mat.h"

/* ====================================================================
Crée une matrice carrée scale * diagonale (kroneker) de dimention dim.

&RT
Une matrice carrée de dimension (dim x dim) est allouée et
retournée (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace mémoire insuffisant ou de dimention nulle).

&EX
    M_diag(3 , 8.25);

&TX
&SA M_free(), M_alloc().
======================================================================= */

#ifdef __cplusplus
MAT *M_diag(
int     mdim,
double  dscale
)
#else
MAT *M_diag(mdim, dscale)
int     mdim;
MREAL   dscale;
#endif

{
    int   i;
    MAT   *m1;

    if (mdim == 0)
    {
	M_errno = M_ALLOCDIM_ERR;
	return((MAT*) 0);
    }

    if((m1 = M_alloc(mdim, mdim)) == 0) return ((MAT*) 0);

    for(i = 0; i < mdim; i++) MATE1(i,i) = dscale;
    return(m1);
}



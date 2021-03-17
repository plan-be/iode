#include "s_mat.h"

/* ====================================================================
Cr‚e une matrice carr‚e scale * diagonale (kroneker) de dimention dim.

&RT
Une matrice carr‚e de dimension (dim x dim) est allou‚e et
retourn‚e (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace m‚moire insuffisant ou de dimention nulle).

&EX
    M_diag(3 , 8.25);

&TX
&SA M_free(), M_alloc().
======================================================================= */

#ifdef SCRCPP
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



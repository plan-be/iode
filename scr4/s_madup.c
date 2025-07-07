#include "s_mat.h"


/* ====================================================================
Copie une matrice dans une autre, les deux matrices doivent exister.

&RT Le pointeur (MAT *) vers la matrice cible est retourné.
    Ce pointeur est nul en cas de matrice non allouée ou
    en cas de dimentions incorrectes, la variable globale M_errno
    est positionnée.

&EX
    MAT     *m1, *m2;
    .....

    m2 = M_copy(m2, m1)
    if(M_errno != 0)
    {
	fprintf(stderr, "Message: ", M_error());
	exit(1);
    }
&TX
&SA M_dup()
======================================================================= */

#ifdef __cplusplus
MAT     *M_copy(
MAT     *m2,     // Destination
MAT     *m1      // Source
)
#else
MAT     *M_copy(m2, m1)
MAT     *m2, *m1;
#endif

{
    register int    i;

    if((m1 == 0) || (m2 == 0) || (NL2 != NL1) || (NC2 != NC1))
    {
	M_errno = M_DUP_ERR;
	return ((MAT*) 0);
    }
    for(i = 0; i < NL1; i++) memcpy(MATL(m2, i), MATL(m1, i), NC1 * sizeof(MREAL));
    return m2;
}



/* ====================================================================
Crée une seconde matrice copie de la première.

&RT Le pointeur (MAT *) vers la matrice copie est retourné. Ce pointeur est
    nul en cas d'échec (mémoire insuffisante).
    La variable globale M_errno est positionnée.

&EX
    MAT     *m1, *m2;
    .....

    m2 = M_dup(m1);
    if(M_errno != 0)
    {
	fprintf(stderr, "Message: ", M_error());
	exit(1);
    }
&TX
&SA M_copy()
======================================================================= */

#ifdef __cplusplus
MAT     *M_dup(
MAT     *m1
)
#else
MAT     *M_dup(m1)
MAT     *m1;
#endif

{
    MAT     *m2;

    m2 = M_alloc(NL1, NC1);
    if((m2 == 0) || (M_errno != 0))
    {
	M_errno = M_DUP_ERR;
	return ((MAT*) 0);
    }
    return(M_copy(m2, m1));
}















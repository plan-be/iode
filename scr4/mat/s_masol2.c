#include "s_mat.h"

/* ====================================================================
Résolution d'un système d'équations linéaires par la méthode
gaussienne d'élimination.
Le système s'écrit sous une forme matricielle:
    m1 * m3 = m2 où
    m1 est la matrice carrée du système (nl1 x nc1),
    m2 est une matrice nl1 x 1,
    m3 est la matrice résultat nl1 x 1.

&RT
Si m3 est nul, une matrice de dimension (nl1, 1) est allouée et
retournée (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace mémoire insuffisant). Si m3 n'est pas nul,
le résultat y est stocké et m3 est retourné.
Les dimensions de m1, m2 et m3 doivent être correctes sans quoi le système
n'est pas résolu , la variable M_errno est positionnée et la
fonction retourne un pointeur nul.
Si le système n'a pas de solution réelle, la variable M_errno est
positionnée et la fonction retourne un pointeur nul.
La fonction M_solve_2() ne modifie pas les matrices d'origine (elle
alloue pour ce faire une copie de ces matrices).

La fonction M_solve() est identique mais ne conserve pas les matrices
d'origine, elle est cependant moins exigante en mémoire.

&EX
    M_solve_2(m3, m1, m2);
    m2 = M_solve_2((MAT*)0, m1, m2);
&TX
&SA M_free(), M_alloc(), M_solve().

======================================================================= */

#ifdef __cplusplus
MAT *M_solve_2(
MAT     *m3,
MAT     *m,
MAT     *m4
)
#else
MAT *M_solve_2(m3, m, m4)
MAT   *m3, *m, *m4;
#endif

{
    int     dim, error, local = 0;
    MAT *m1, *m2;

    m1 = M_dup(m);
    m2 = M_dup(m4);

    if((m1 == 0) || (m2 == 0))
    {
	M_errno = M_SOLDIM_ERR;
	goto err;
    }
    if((NL1 != NC1) || (NL2 != NL1) || (NC2 != 1))
    {
	M_errno = M_SOLDIM_ERR;
	goto err;
    }

    if (m3 == 0)
    {
	m3 = (MAT *) M_alloc(NL1, NC2);
	if(m3 == 0)
	{
	    M_errno = M_MEM_ERR;
	    goto err;
	}
	local = 1;
    }
    else
	if((NL3 != NL1) || (NC3 != NC2))
	{
	    M_errno = M_SOLDIM_ERR;
	    goto err;
	}

    dim = NL1;

    error = M_gel_init(dim, m1, m2, m3);
    if (error !=0) goto err;
    if (dim > 1)
    {
	error = M_gel_ut(dim, m1, m2);
	if (error != 0)
	{
	    M_errno = M_SOLNOSOL_ERR;
	    goto err;
	}
	M_bw_sub(dim, m1, m2, m3); /* mat, vect, sol */
    }
    M_free(m1);
    M_free(m2);
    return(m3);
err:
    M_free(m1);
    M_free(m2);
    if(local) M_free(m3);
    return(NULL);
}

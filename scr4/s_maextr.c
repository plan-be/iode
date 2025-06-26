#include "s_mat.h"
/* ====================================================================
Extrait de la matrice m1 (nl1 x nc1) une matrice m2 (nl x nc) à
partir de la ligne l incluse ( l := 0, .. , nl1 - 1) et de la
colonne c incluse ( c := 0, .. , nc1 - 1).


&RT
Si m2 est nul, une matrice de dimension (nl, nc) est allouée et
retournée (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace mémoire insuffisant).
Si m2 n'est pas nul, le résultat y est stocké et m2 est
retourné. Les dimensions doivent être correctes (les dimensions de m2
doivent effectivement être incluses dans m1) sans quoi les
opérations ne sont pas effectuées, la variable globale M_errno est
positionnée, et la fonction retourne un pointeur nul.

&EX
    M_extr( ext, mat, 2, 3, 2, 2);
&TX
&SA M_free(), M_alloc(), M_copy(), M_dup().
======================================================================= */

#ifdef SCRCPP
MAT *M_extr(
MAT     *m2,
MAT     *m1,
int     l,
int     c,
int     nl,
int     nc
)
#else
MAT *M_extr(m2, m1, l, c, nl, nc)
MAT     *m2, *m1;
int     l, c, nl, nc;
#endif
{
    int i,j, k, m;
    int bl, bc;
    int el, ec;

    if ((m1 == 0))
    {
	M_errno = M_EXTRDIM_ERR;
	return((MAT *) 0);
    }
    bl = l;
    bc = c;
    el = bl + nl - 1;
    ec = bc + nc - 1;

    if( (bl > NL1 -1) || (bl < 0) || (bc > NC1 - 1) || (bc < 0))
    {
	M_errno = M_EXTRDIM_ERR;
	return((MAT *) 0);
    }
    if( (el > NL1 -1) || (el < 0) || (ec > NC1 - 1) || (ec < 0))
    {
	M_errno = M_EXTRDIM_ERR;
	return((MAT *) 0);
    }
    if( (bl > el) || (bc > ec))
    {
	M_errno = M_EXTRDIM_ERR;
	return((MAT *) 0);
    }

    if (m2 == 0)
    {
	m2 = M_alloc(nl, nc);
	if(m2 == 0) return(m2);
    }
    else
	if((NL2 != nl) || (NC2 != nc))
	{
	    M_errno = M_EXTRDIM_ERR;
	    return((MAT *) 0);
	}


    for(i = bl, k = 0; i <= el; i++, k++)
	for(j = bc, m = 0; j <= ec; j++, m++)
	    MATE2(k, m) = MATE1(i,j);
    return(m2);
}

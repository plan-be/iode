#include "s_mat.h"
/* ====================================================================
Effectue des opérations de base sur les éléments de deux matrices
m1 et m2 de même dimensions et stocke le résultat dans m3.

Les opérations sont '+', '-', '*' ou '/'.

&RT
Si m3 est nul, une matrice de dimension (nl1, nc1) est allouée et
retournée (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace mémoire insuffisant).
Si m3 n'est pas nul, le résultat y est stocké et m3 est
retourné. Les dimensions doivent être correctes sans quoi les
opérations ne sont pas effectuées, la variable globale M_errno est
positionnée, et la fonction retourne un pointeur nul.
Attention, les opérateurs  '*' et '/' ne sont pas un produit ou
une division matricielle.

&EX
    M_calc(m3, m1, m2, '+');
    ou m3 = M_calc(0L, m1, m2, '*');
&TX
&SA M_free(), M_alloc(), M_prod(), M_inv_1 ou 2 ().
======================================================================= */

#ifdef SCRCPP
MAT *M_calc(
MAT     *m3,
MAT     *m1,
MAT     *m2,
int     oper
)
#else
MAT *M_calc(m3, m1, m2, oper)
MAT   *m3, *m1, *m2;
int   oper;
#endif
{
    int  i, j;

    if((m1 == 0) || (m2 == 0) || (NC1 != NC2) || (NL1 != NL2) )
    {
	M_errno = M_OPERDIM_ERR;
	return((MAT *)0);
    }
    if(m3 == 0)
    {
	m3 = M_alloc(NL1, NC1);
	if(m3 == 0) return(m3);
    }
    else
	if(NC3 != NC1 || NL3 != NL1)
	{
	    M_errno = M_OPERDIM_ERR;
	    return((MAT *)0);
	}

    switch(oper)
    {
	case '+' :
	    for(i = 0; i < NL1; i++)
		for(j = 0; j < NC1; j++)
		    MATE3(i,j) = MATE1(i,j) + MATE2(i,j);
	    break;
	case '-' :
	    for(i = 0; i < NL1; i++)
		for(j = 0; j < NC1; j++)
		    MATE3(i,j) = MATE1(i,j) - MATE2(i,j);
	    break;
	case '*' :
	    for(i = 0; i < NL1; i++)
		for(j = 0; j < NC1; j++)
		    MATE3(i,j) = MATE1(i,j) * MATE2(i,j);
	    break;
	case '/' :
	    for(i = 0; i < NL1; i++)
		for(j = 0; j < NC1; j++)
		    MATE3(i,j) = MATE1(i,j) / MATE2(i,j);
	    break;
    }
    return(m3);
}


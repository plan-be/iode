#include "s_mat.h"
/* ====================================================================
Effectue des op‚rations de base sur les ‚l‚ments de deux matrices
m1 et m2 de mˆme dimensions et stocke le r‚sultat dans m3.

Les op‚rations sont '+', '-', '*' ou '/'.

&RT
Si m3 est nul, une matrice de dimension (nl1, nc1) est allou‚e et
retourn‚e (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace m‚moire insuffisant).
Si m3 n'est pas nul, le r‚sultat y est stock‚ et m3 est
retourn‚. Les dimensions doivent ˆtre correctes sans quoi les
op‚rations ne sont pas effectu‚es, la variable globale M_errno est
positionn‚e, et la fonction retourne un pointeur nul.
Attention, les op‚rateurs  '*' et '/' ne sont pas un produit ou
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


#include "s_mat.h"


/* ====================================================================
Effectue des op�rations de base sur une colonne d'une matrice.

Les op�rations sont '+', '*', 'm' ou 's', soit l'addition, le produit, la
moyenne et l'�cart-type estim� des �l�ments de la colonne.

On passe en param�tre un pointeur sur la matrice � traiter, l'indice de la
colonne et l'op�rateur.
&RT
La fonction retourne un MREAL.

Si m1 est nul, la variable globale est positionn�e et la fonction retourne
0.0. Il en va de m�me pour le calcul de l'�cart type pour une matrice d'une
seule ligne ou lorsque l'op�rateur n'est pas connu.

&EX
    M_cstat(m3, 2, 's');
&TX
&SA M_free(), M_alloc(), M_calc(), M_lstat()
======================================================================= */


MREAL   M_cstat(m1, c, op)
MAT *m1;
int c;
int op;
{
    int i;
    MREAL res = 0.0, sce = 0.0, m, x;

    if(m1 == NULL)
    {
	M_errno = M_OPERDIM_ERR;
	return(0);
    }

    for(i = 0; i < NL1; i++)
    {
	switch(op)
	{
	    case '+'    :
	    case 'm'    :
	    case 's'    :
		res += MATE1(i, c);
		break;
	    case '*'    :
		res *= MATE1(i, c);
		break;
	    default     :
		break;
	}
    }
    if(op == 'm') return(res / NL1);
    if(op == '+') return(res);
    if(op == 's')
    {
	if(NL1 == 1) return(0);
	m = res / NL1;
	for(i = 0; i < NL1; i++)
	{
	    x = MATE1(i, c);
	    sce += ((x - m) * (x - m));
	}
	return((MREAL) sqrt((double)( sce / (NL1 - 1))));
    }

    return(res);
}


/* ====================================================================
Effectue des op�rations de base sur une ligne d'une matrice.

Les op�rations sont '+', '*', 'm' ou 's', soit l'addition, le produit, la
moyenne et l'�cart-type estim� des �l�ments de la ligne.

On passe en param�tre un pointeur sur la matrice � traiter, l'indice de la
ligne et l'op�rateur.
&RT
La fonction retourne un MREAL.

Si m1 est nul, la variable globale est positionn�e et la fonction retourne
0.0. Il en va de m�me pour le calcul de l'�cart type pour une matrice d'une
seule colonne ou lorsque l'op�rateur n'est pas connu.

&EX
    M_lstat(m3, 2, 's');
&TX
&SA M_free(), M_alloc(), M_calc(), Mcstat()
======================================================================= */


MREAL   M_lstat(m1, l, op)
MAT *m1;
int l;
int op;
{
    int     j;
    MREAL   res = 0.0, sce = 0, m, x;

    if(m1 == NULL)
    {
	M_errno = M_OPERDIM_ERR;
	return(0);
    }

    for(j = 0; j < NC1; j++)
    {
	switch(op)
	{
	    case '+'    :
	    case 'm'    :
	    case 's'    :
		res += MATE1(l, j);
		break;
	    case '*'    :
		res *= MATE1(l, j);
		break;
	    default     :
		break;
	}
    }
    if(op == 'm') return(res / NL1);
    if(op == '+') return(res);
    if(op == 's')
    {
	if(NC1 == 1) return(0);
	m = res / NL1;
	for(j = 0; j < NC1; j++)
	{
	    x = MATE1(l, j);
	    sce += ((x - m) * (x - m));
	}
	return((MREAL) sqrt((double)( sce / (NC1 - 1))));
    }

    return(res);
}






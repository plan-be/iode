#include "s_mat.h"

int M_errno;

char  *M_ERR_TEXT[] = {
    "no error",
    "matrix product: dim error",
    "matrix allocation: memory error",
    "matrix transposition: dim error",
    "matrix invertion: dim error",
    "matrix invertion: det. error",
    "matrix copy or dupl.: dim error",
    "determinant: dim error",
    "determinant: singular matrix",
    "system's solution: dim error",
    "system's solution: no solution",
    "trace: dimention error",
    "matrix allocation: null dim ",
    "scale matrix: dim error",
    "matrix to be cleared: null",
    "elementar op. : dim. error",
    "matrix extraction: dim. error",
    };


#ifdef NOSWAP


/* ====================================================================
Alloue une matrice de nl lignes et nc colonnes et retourne le pointeur
vers la matrice allouée.

Les allocations utilisent la librairie s_swap.lib pour stocker les
données du tableau. Si la constante symbolique NOSWAP est définie,
l'allocation se fait en mémoire conventionnelle, sinon elle est faite
en EMS.

La valeur nc ne peut dépasser 2000. La valeur de nl ne peut dépasser 16000.

&RT Le pointeur vers la matrice (MAT *) est retourné. Ce pointeur est
    nul en cas de limite mémoire et en cas de dimention incorrecte (nulle),
    la variable globale M_errno est positionnée.
    Les éléments de la matrice sont initialisés à 0.0.

&EX
    MAT     *m1;
    int     nl = 100, nc = 100;

    m1 = M_alloc(nl, nc);
    if(M_errno != 0)
    {
	fprintf(stderr, "Message: ", M_error());
	exit(1);
    }
&TX
&SA M_free() , M_alloc_vec().
======================================================================= */

#ifdef SCRCPP
MAT *M_alloc(
int     nl,
int     nc
)
#else
MAT *M_alloc(nl, nc)
int     nl, nc;
#endif

{
    MAT          *m1=0;
    int          i;

    M_errno = 0;
    if ((nl == 0) || (nc == 0))
    {
	M_errno = M_ALLOCDIM_ERR;
	return((MAT*) 0);
    }
    m1 = (MAT*)SW_nalloc(sizeof(MAT));
    if(m1 == 0)  goto memerr;
    NL1 = nl;
    NC1 = nc;
    m1->m_lines =  (VECT *) SW_nalloc(nl * sizeof(VECT));
    if(m1->m_lines == NULL) goto memerr;
    for(i = 0; i < nl; i++)
    {
	m1->m_lines[i] = (MREAL *) SW_nalloc(nc * sizeof(MREAL));
	if(m1->m_lines[i] == NULL) goto memerr;
    }
   return(m1);

memerr:
   M_free(m1);
   M_errno = M_MEM_ERR;
   return((MAT *)0);
}


/* ====================================================================
Libère l'espace alloué pour une matrice.
Si le pointeur est nul, la fonction est sans effet.
&EX MAT     *m1;
    ...
    M_free(m1);
&TX

&SA M_alloc(), M_free_vec().
======================================================================= */

M_free(m1)
MAT     *m1;
{
    int          i;

    if(m1 == 0) return(0);
    for(i = 0; i < NL1; i++) SW_nfree((char *)m1->m_lines[i]);
    SW_nfree((char *)m1->m_lines);
    SW_nfree((char *)m1);
    return(0);
}

/*NH*/

/*
    transfère un vecteur du swap en mémoire conv.
*/
MREAL* M_vswp_to_cnv(row2, row1, dim)     /* dest , source */
MREAL    *row2;
VECT    row1;
int     dim;
{
    memcpy(row2, row1, dim * sizeof(MREAL));
    return(row2);
}

/*NH*/

/*
    transfère de mémoire conv. vers swap
*/
VECT M_vcnv_to_swp(row2, row1, dim)     /* dest , source */
VECT    row2;
MREAL*   row1;
int     dim;
{
    memcpy(row2, row1, dim * sizeof(MREAL));
    return(row2);
}


/*NH*/
#else     /* matrix swapped in EMS */


/*NH*/

MAT *M_alloc(nl, nc)
int     nl, nc;
{
    MAT          *m1;
    int          i;

    if ((nl == 0) || (nc == 0))
    {
	M_errno = M_ALLOCDIM_ERR;
	return((MAT*) 0);
    }
    m1 = (MAT*) SW_nalloc(sizeof(MAT));
    if(m1 == 0) goto memerr;
    NL1 = nl;
    NC1 = nc;
    m1->m_lines =  (int*) SW_nalloc(nl * sizeof(int));
    if(m1->m_lines == NULL) goto memerr;
    for(i = 0; i < nl; i++)  m1->m_lines[i] = (int) SW_alloc(nc * sizeof(MREAL));
   return(m1);

memerr:
   M_free(m1);
   M_errno = M_MEM_ERR;
   return((MAT *)0);
}


/*NH*/

M_free(m1)
MAT     *m1;
{
    int          i;

    if(m1 == 0) return(0);
    for(i = 0; i < NL1; i++) SW_free(m1->m_lines[i]);
    SW_nfree(m1->m_lines);
    SW_nfree(m1);
    return(0);
}

/*NH*/
/*
    transfère un vecteur du swap en mémoire conv.
*/
MREAL* M_vswp_to_cnv(row2, row1, dim)     /* dest , source */
MREAL    *row2;
VECT    row1;
int     dim;
{
    memcpy(row2, (MREAL*)SW_getptr(row1), dim * sizeof(MREAL));
    return(row2);
}


/*NH*/
/*
    transfère de mémoire conv. vers swap
*/
VECT M_vcnv_to_swp(row2, row1, dim)     /* dest , source */
VECT    row2;
MREAL*   row1;
int     dim;
{
    memcpy((MREAL*)SW_getptr(row2), row1, dim * sizeof(MREAL));
    return(row2);
}


#endif


/* ====================================================================
Alloue un vecteur de nc elem. en mémoire conventionnelle et retourne
un pointeur sur ce vecteur.

L'allocation utilise la librairie s_swap.lib.


&RT Le pointeur vers le vecteur (MREAL *) est retourné. Ce pointeur est
    nul en cas de limite mémoire.
    La variable globale M_errno est positionnée.
    Les éléments du vecteur sont initialisés à 0.0.



&EX
    MREAL    *vect;
    int     nc = 100;

    vect = M_alloc_vec(nc);
    if(M_errno != 0)
    {
	fprintf(stderr, "Message: ", M_error());
	exit(1);
    }
&TX
&SA M_free_vec() , M_alloc().


======================================================================= */


MREAL    *M_alloc_vec(nc)
int     nc;
{
    MREAL*   vec;

    vec = (MREAL*) SW_nalloc(nc * sizeof(MREAL));
    if(vec == NULL) goto memerr;
    return(vec);

memerr:
    M_errno = M_MEM_ERR;
    return((MREAL*) 0);
}


/* ====================================================================
Libère l'espace alloué pour un vecteur en mémoire conventionnelle.
Si le pointeur est nul, la fonction est sans effet.

&EX
    MREAL    *vect;
    ...
    M_free_vec(vect);
&TX

&SA M_alloc_vec(), M_alloc(), M_free().
======================================================================= */

M_free_vec(vec)
MREAL*    vec;
{
    if(vec == 0) return(0);
    SW_nfree((char *)vec);
    return(0);
}










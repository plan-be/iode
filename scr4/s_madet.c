#include "s_mat.h"


/* ====================================================================
Calcul du déterminant d'une matrice m1 carrée nl1 x nc1 par la méthode
de balayage.

&CO
    Soit la matrice A(aij); i,j: 1...n.
    On calcule procède à la triangulation:

		    P = amm                m := 2...n
		    q = aim / P            i := 1...m-1
		    aij = aij - q * amj    i := 1...m-1

    On obtient aij = 0 pour i < j.
    Le déterminant est calculé par det = a11 * a22 * ... * ann.
    Pendant tout le calcul, P <> 0, sinon la méthode n'est pas
    applicable.
&TX

&RT
Si m1 est nul ou de dimension incorrecte, la fonction retourne 0 et
la variable M_errno est positionnée. Sinon la fonction retourne la valeur
(MREAL) du déterminant.
Si la martice n'est pas régulière, la variable M_errno est positionnée et
la fonction retourne un (MREAL) nul.
La matrice source n'est pas modifiée.

&EX
    value = M_det_1(m1);
&TX

======================================================================= */

MREAL M_det_1(m1)
MAT *m1;
{
    int i,j,m,n;
    MREAL p, q, det = 0;
    MAT *m2;

    if ((m1 == 0) || (NL1 != NC1))
    {
	M_errno = M_DETDIM_ERR;
	return((MREAL) 0.0);
    }
    m2 = M_dup(m1);
    if(M_errno != 0)
    {
	M_free(m2);
	return((MREAL) 0);
    }
    n = NL1-1;

    for(m = n; m > 0; m--)
    {
	p = MATE2(m,m);
	if(p == 0) goto err;
	for(i = 0; i < m; i++)
	{
	    q = MATE2(i, m)/p;
	    for(j = 0; j < m+1; j++)
	    {
		MATE2(i,j) -= q * MATE2(m, j);
	    }
	}
    }
    det = MATE2(0,0);
    for(i = 1; i < NL1; i++) det *= MATE2(i, i);
    M_free(m2);
    return(det);

err:
    M_free(m2);
    M_errno = M_DETSING_ERR;
    return((MREAL) 0.0);
}



/*NH*/
/*
 This tests for errors in the value of dim
*/

M_det_init(dim, m1, det)
int     dim;
MAT     *m1;
MREAL    *det;
{
    if (dim < 1) return(-1);
    else
    if (dim == 1) *det = MATE1(0,0);

    return(0);
} /* initial */


/* ====================================================================
Calcul du déterminant d'une matrice m1 carrée nl1 x nc1 par la méthode
du pivot.

&RT
Si m1 est nul ou de dimension incorrecte, la fonction retourne 0 et
la variable M_errno est positionnée. Sinon la fonction retourne la valeur
(MREAL) du déterminant.
Si la martice n'est pas régulière, la variable M_errno est positionnée et
la fonction retourne un (MREAL) nul.
La matrice est modifiée.

&EX
    value = M_det_2(m1);
&TX

======================================================================= */

MREAL    M_det_2(m1)
MAT     *m1;
{
    MREAL    p_det, det, mult;
    int     row, r_row, sing;
    int     dim;
    MREAL    *v1_r_row, *v1_row;

    if ((m1 == 0) || (NL1 != NC1))
    {
	M_errno = M_DETDIM_ERR;
	return((MREAL) 0.0);
    }
    dim = NL1;

    sing = 0;
    p_det = 1;
    r_row = 0;

    M_det_init(dim, m1, &det);
    if (dim > 1)
    {
    /* make the matrix up triangular */
	while (!(sing) && (r_row < dim - 1))
	{
	    /* if (diagonal element is zero) switch rows */
	    if (ZERO(MATE1(r_row,r_row)))
		M_det_pivot(dim, r_row, m1, &p_det, &sing);
	    if (!sing)
		for(row = r_row + 1; row < dim; row++)
		/* make the r_row element of this row zero */
		    if (!ZERO(MATE1(row,r_row)))
		    {
			mult = -MATE1(row,r_row) / MATE1(r_row,r_row);

			v1_r_row =  M_alloc_vec(dim);
			v1_row   =  M_alloc_vec(dim);
			if (M_errno != 0) goto err;
			v1_r_row =  M_vswp_to_cnv(v1_r_row, ROW(m1, r_row), dim);
			v1_row =    M_vswp_to_cnv(v1_row, ROW(m1, row), dim);
			M_ero_ma(dim, mult, v1_r_row, v1_row);
			ROW(m1, r_row) = M_vcnv_to_swp(ROW(m1,r_row), v1_r_row, dim);
			ROW(m1, row) =   M_vcnv_to_swp(ROW(m1,row), v1_row, dim);
			M_free_vec(v1_r_row);
			M_free_vec(v1_row);
		    }
	    /* multiply the diagonal term into p_det */
	    p_det = p_det * MATE1(r_row, r_row);
	    r_row = r_row ++;
	}
	if (sing) det = 0;
	else
	det = p_det * MATE1(dim - 1,dim - 1);
    }
    return(det);
err:
    M_errno = M_DETSING_ERR;
    return((MREAL) 0);
}

/*NH*/
/*
   This searches the r_row column of the
   matrix data for(the first non-zero element below the
   diagonal. if (it finds one,) the switches
   rows so that the non-zero element is on the diagonal.
   switching rows changes the determinant by a factor of
   -1; this change is returned in p_det.
   if (it doesn't find one, the matrix is singular && the
   determinant is zero (sing == 1 is returned).
*/

M_det_pivot(dim, row, m1, p_det, sing)
int     dim, row;
MAT     *m1;
MREAL    *p_det;
int     *sing;
{
    int   n_row;
    MREAL  *v_n_row, *v_row;

    *sing = 1;
    n_row = row;
    while (sing && (n_row < dim))
    {
	if (!ZERO(MATE1(n_row,row)))
	{
	    v_n_row =  M_alloc_vec(dim);
	    v_row = M_alloc_vec(dim);;
	    if (M_errno != 0) return(M_errno);
	    v_n_row =  M_vswp_to_cnv(v_n_row, ROW(m1, n_row), dim);
	    v_row =  M_vswp_to_cnv(v_row, ROW(m1, row), dim);
	    M_ero_sw(m1->m_nl, v_n_row, v_row); /* switch these two rows */
	    ROW(m1, n_row) =   M_vcnv_to_swp(ROW(m1,n_row), v_n_row, dim);
	    ROW(m1, row) =   M_vcnv_to_swp(ROW(m1,row), v_row, dim);
	    M_free_vec(v_n_row);
	    M_free_vec(v_row);

	    *sing = 0;
	    *p_det = - *p_det;              /* switching rows changes */
					  /* the determinant by a   */
					  /* factor of -1           */
	}
	n_row = n_row++;
    }
    return(0);
} /* pivot */


























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
Les matrices sources sont modifiées.

&EX
    M_solve(m3, m1, m2);
    m2 = M_solve((MAT*)0, m1, m2);
&TX
&SA M_free(), M_alloc()

======================================================================= */

#ifdef __cplusplus
MAT *M_solve(
MAT     *m3,
MAT     *m1,
MAT     *m2
)
#else
MAT *M_solve(m3, m1, m2)
MAT   *m3, *m1, *m2;
#endif

{
    int     dim, error, local = 0;

    if((m1 == 0) || (m2 == 0))
    {
	M_errno = M_SOLDIM_ERR;
	return(m2);
    }
    if((NL1 != NC1) || (NL2 != NL1) || (NC2 != 1))
    {
	M_errno = M_SOLDIM_ERR;
	return((MAT *)0);
    }

    if (m3 == 0)
    {
	m3 = (MAT *) M_alloc(NL1, NC2);
	if(m3 == 0)
	{
	    M_errno = M_MEM_ERR;
	    return(NULL);
	}
	local = 1;
    }
    else
	if((NL3 != NL1) || (NC3 != NC2))
	{
	    M_errno = M_SOLDIM_ERR;
	    return((MAT *)0);
	}


    dim = NL1;

    error = M_gel_init(dim, m1, m2, m3);
    if (error !=0) goto err;
    if (dim > 1)
    {
	error = M_gel_ut(dim, m1, m2);
	if (error != 0) goto err;
	M_bw_sub(dim, m1, m2, m3); /* mat, vect, sol */
    }
    return(m3);
err:
    M_errno = M_SOLNOSOL_ERR;
    if(local) M_free(m3);
    return(NULL);
}

/*NH*/
/*
   input: dim, mat, vect
   output: solution, error

   this test for(errors in the value of dim.
   this also finds the solution for(the
   trivial case dim == 1.
*/

M_gel_init(dim, m1, m2, m3)    /* mat, vect, sol */
int     dim;
MAT     *m1, *m2, *m3;
{
  if (dim < 1) return(-1);
  else
    if (dim == 1)
      if (ZERO(MATE1(0,0))) return(-2);
      else
	MATE3(0,0) = MATE2(0,0) / MATE1(0,0);
   return(0);
}

/*NH*/
/*
   input: dim, mat, vect
   output: mat, vect

   this makes the coefficient matrix up triangular.
   the operations which perform this are also performed on the
   vect vector.
   if (one of the main diagonal elements of the up triangular
   matrix is zero,) the mat matrix is singular &&
   no solution exists (error == 2 is returned).
*/

M_gel_ut(dim, m1, m2)
int     dim;
MAT     *m1, *m2;
{
    int     error = 0, row, r_row = 0;
    MREAL   mult;
    MREAL   *v1_r_row = 0, *v1_row = 0;

    v1_r_row =  M_alloc_vec(dim);
    v1_row   =  M_alloc_vec(dim);
    if (M_errno != 0) goto err;

    while (error == 0 && r_row < dim - 1)
    {
	/* check to see if the main diagonal element is zero */
	if (ZERO(MATE1(r_row,r_row)))
	    error = M_gel_pivot(dim, r_row, m1, m2);
	if (error == 0)
	    for(row = r_row + 1; row < dim; row++)
		/* make the r_row element of this row zero */
		if (!ZERO(MATE1(row,r_row)))
		{
		    mult = -MATE1(row,r_row) / MATE1(r_row,r_row);
		    v1_r_row =  M_vswp_to_cnv(v1_r_row, ROW(m1, r_row), dim);
		    v1_row =    M_vswp_to_cnv(v1_row, ROW(m1, row), dim);
		    M_ero_ma(dim, mult, v1_r_row, v1_row);
		    ROW(m1, r_row) = M_vcnv_to_swp(ROW(m1,r_row), v1_r_row, dim);
		    ROW(m1, row) =   M_vcnv_to_swp(ROW(m1,row), v1_row, dim);

		    MATE2(row, 0) +=  mult * MATE2(r_row,0);
		}
	r_row++;
    } /* while */

    if (ZERO(MATE1(dim - 1,dim - 1)))  error = -2;    /* no solution */
    M_free_vec(v1_r_row);
    M_free_vec(v1_row);
    return(error);

err:
    M_free_vec(v1_r_row);
    M_free_vec(v1_row);
    return(M_errno);
}

/*NH*/
/*
   This searches the r_row column of the
   mat matrix for the first non-zero element below
   the diagonal. if it finds one, the switches
   rows so that the non-zero element is on the diagonal.
   it also switches the corresponding elements in the
   vect vector. if it doesn't find one, the matrix is
   singular && no solution exists (error == 2 is returned).
*/

M_gel_pivot(dim, r_row, m1, m2)
int     dim;
int     r_row;
MAT     *m1, *m2;
{

    int   n_row, error;
    MREAL  dummy;
    MREAL  *v_n_row, *v_r_row;

    error = 2;          /* no solution exists */
    n_row = r_row;
    while (error > 0 && n_row < dim)
    {
	if (!ZERO(MATE1(n_row,r_row)))
	{
	    v_r_row =  M_alloc_vec(dim);
	    v_n_row =  M_alloc_vec(dim);;
	    if (M_errno != 0) return(M_errno);
	    v_r_row =  M_vswp_to_cnv(v_r_row, ROW(m1, r_row), dim);
	    v_n_row =  M_vswp_to_cnv(v_n_row, ROW(m1, n_row), dim);
	    M_ero_sw(dim, v_n_row, v_r_row);
	    ROW(m1, r_row) =   M_vcnv_to_swp(ROW(m1,r_row), v_r_row, dim);
	    ROW(m1, n_row) =   M_vcnv_to_swp(ROW(m1,n_row), v_n_row, dim);
	    M_free_vec(v_r_row);
	    M_free_vec(v_n_row);
	    /* switch these two rows */
	    dummy = MATE2(n_row, 0);
	    MATE2(n_row,0) = MATE2(r_row,0);
	    MATE2(r_row,0) = dummy;
	    error = 0;    /* solution may exist */
	}
	n_row++;
    }
    return(error);
}


/*NH*/
/*
   This applies backwards substitution to the up
   triangular mat matrix && vect vector. the
   resulting vector is the solution to the set of equations &&
   is returned in the vector solution.
*/

M_bw_sub(dim, m1, m2, m3)
int     dim;
MAT     *m1, *m2, *m3;
{
    register int    term, row;
    register MREAL   sum = 0.0;

    term = dim - 1;
    while (term >= 0)
    {
	sum = 0.0;
	for(row = term + 1; row < dim; row++)
	    sum = sum + MATE1(term,row) * MATE3(row,0);
	MATE3(term,0) = (MATE2(term,0) - sum) / MATE1(term,term);
	term--;
    }
    return(0);
}


















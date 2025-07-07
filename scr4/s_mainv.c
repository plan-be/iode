#include "s_mat.h"

/* ====================================================================
Invertion d'une matrice m1 carrée nl1 x nc1 par la méthode de balayage.
Le résultat est stocké dans m2 carrée nl1 x nc1.

&CO
    Soit la matrice A(aij); i,j: 1...n.
    On calcule :

		aii = aii + 1          i := 1...n
		P = amm - 1,           m := 1...n
		amj = amj / P          j := 1...n
		aij = aij - aim * amj  i := 1...n, i <> m
		aii = aii - 1          i := 1...n

    Pendant tout le calcul, P <> 0, sinon la matrice non invertible par
    cette méthode.
&TX

&RT
    Si m2 est nul, une matrice de dimension (nl1, nc1) est allouée et
    retournée (la fonction retourne un pointeur nul et positionne
    M_errno en cas d'espace mémoire insuffisant). Si m2 n'est pas nul,
    le résultat y est stocké et m2 est retourné. Les dimensions de m1 et
    m2 doivent être correctes sans quoi l'inverse calculé , la variable
    M_errno est positionnée et la fonction retourne un pointeur nul. Si
    la matrice n'est pas inversible, la variable M_errno est positionnée
    et la fonction retourne un pointeur nul. La matrice source n'est pas
    modifiée.

&EX
    M_inv_1(m2, m1);
    m2 = M_inv_1((MAT*)0, m1)
&TX
&SA M_free(), M_alloc()

======================================================================= */

#ifdef __cplusplus
MAT *M_inv_1(
    MAT     *m2,
    MAT     *m1
)
#else
MAT *M_inv_1(m2, m1)
MAT *m2, *m1;
#endif
{
    int     i, j, m;
    int     local = 0;
    MREAL   p, q;

    if ((m1 == 0) || (NL1 != NC1))
    {
        M_errno = M_INVDIM_ERR;
        return((MAT *) 0);
    }

    if(m2 == 0)
    {
        m2 = M_dup(m1);
        if(m2 == 0)
        {
            M_errno = M_MEM_ERR;
            return(m2);
        }
        local = 1;
    }
    else {
        if((NL2 != NL1) || (NC2 != NC1))
        {
            M_errno = M_INVDIM_ERR;
            return((MAT *) 0);
        }
        M_copy(m2, m1);
    }

    for(i = 0; i < NL2; i++)
        MATE2(i, i) += 1.0;

    for(m = 0; m < NL2; m++)
    {
        if((p = MATE2(m,m) - 1) == 0) goto err;

        for(j = 0; j < NL2; j++)
            MATE2(m,j) /= p;

        for(i = 0; i < NL2; i++)
        {
            if ( i == m) continue;
            q = MATE2(i, m);

            for(j = 0; j < NL2; j++)
                MATE2(i,j) -= q * MATE2(m, j);
        }
    }

    for(i = 0; i < NL2; i++)
        MATE2(i, i) -= 1.0;

    return(m2);

err:
    M_errno = M_INVDET_ERR;
    if(local) M_free(m2);
    return((MAT *) 0);
}


/*NH*/
/* invertion par méthode pivot */
/*
   This test for errors in the value of dim
*/

M_inv_init(dim, m2, m1)  /* inv  mat */
int     dim;
MAT     *m2, *m1;
{
    int   i;

    if (dim < 1) return(-1);
    else {
        /* allocate the inverse matrix, init all elem == 0  */
        /* first make the inverse-to-be the identity matrix */

        M_clear(m2);
        for(i = 0; i < dim; i++) MATE2(i,i) =  1.0;
        if (NL1 == 1)
            if (ZERO(MATE1(i,i))) return(-1);   /* singular matrix */
            else
                MATE2(0,0) = 1.0 / MATE1(0,0);
    }
    return(0);
}


/* ====================================================================
Invertion d'une matrice m1 carrée nl1 x nc1 par la méthode du pivot.
Le résultat est stocké dans m2 carrée nl1 x nc1.

&RT
Si m2 est nul, une matrice de dimension (nl1, nc1) est allouée et
retournée (la fonction retourne un pointeur nul et positionne M_errno
en cas d'espace mémoire insuffisant). Si m2 n'est pas nul,
le résultat y est stocké et m2 est retourné.
Les dimensions de m1 et m2 doivent être correctes sans quoi l'inverse
calculé , la variable M_errno est positionnée et la
fonction retourne un pointeur nul.
Si la martice n'est pas inversible, la variable M_errno est positionnée et
la fonction retourne un pointeur nul.
Attention, la fonction travaille sur trois matrices nl1 x nc1, dont une est
allouée en cours de process. Il faut donc s'assurer que l'espace nécessaire
est disponible.
La matrice m1 n'est pas modifiée.

&EX
    M_inv_2(m2, m1);
    m2 = M_inv_2((MAT*)0, m1)
&TX
&SA M_free(), M_alloc()

======================================================================= */

#ifdef __cplusplus
MAT *M_inv_2(
    MAT     *m2,
    MAT     *m
)
#else
MAT *M_inv_2(m2, m)
MAT    *m2, *m;
#endif

{
    MREAL   *vect;
    MREAL   *v1_r_row = 0, *v1_row = 0,
             *v2_r_row = 0, *v2_row = 0;
    MREAL   div, mult;
    MAT     *m1 = 0;
    int     error, row, r_row, dim;
    int     local = 0;

    m1 = M_dup(m);
    if(m1 == 0)
    {
        M_errno = M_MEM_ERR;
        return((MAT *)0);
    }

    if((NL1 != NC1))
    {
        M_errno = M_INVDIM_ERR;
        goto err;
    }

    if (m2 == 0)
    {
        m2 = (MAT *) M_alloc(NL1, NC1);
        if(m2 == 0)
        {
            M_errno = M_MEM_ERR;
            goto err;
        }
        local = 1;
    }
    else if((NL2 != NL1) || (NC2 != NC1))
    {
        M_errno = M_INVDIM_ERR;
        goto err;
    }

    dim = NL1;
    error = M_inv_init(dim, m2, m1);
    if(error != 0) goto err;

    v1_r_row =  M_alloc_vec(dim);
    v1_row   =  M_alloc_vec(dim);
    v2_r_row =  M_alloc_vec(dim);
    v2_row   =  M_alloc_vec(dim);
    if (M_errno != 0) goto err;

    if (NL1 > 1)
    {
        /* make data matrix up triangular */
        r_row = 0;
        while ((error == 0) && (r_row < dim))
        {
            /* check to see if the diagonal element is zero */
            if (ZERO(MATE1(r_row,r_row)))
                error = M_inv_pivot(r_row, m2, m1);
            if (error == 0)
            {
                div = MATE1(r_row,r_row);
                vect = MATL(m1, r_row);
                M_ero_div(dim, div, vect);
                vect = MATL(m2, r_row);
                M_ero_div(dim, div, vect);

                for(row = 0; row < dim; row++)
                    /* make the r_row element of this row zero */
                    if (row != r_row && !ZERO(MATE1(row,r_row)))
                    {
                        mult = -MATE1(row,r_row) / MATE1(r_row,r_row);

                        v1_r_row =  M_vswp_to_cnv(v1_r_row, ROW(m1, r_row), dim);
                        v1_row =    M_vswp_to_cnv(v1_row, ROW(m1, row), dim);
                        M_ero_ma(dim, mult,v1_r_row,v1_row);
                        ROW(m1, r_row) = M_vcnv_to_swp(ROW(m1,r_row), v1_r_row, dim);
                        ROW(m1, row) =   M_vcnv_to_swp(ROW(m1,row), v1_row, dim);

                        v2_r_row =  M_vswp_to_cnv(v2_r_row, ROW(m2, r_row), dim);
                        v2_row =    M_vswp_to_cnv(v2_row, ROW(m2, row), dim);
                        M_ero_ma(dim, mult,v2_r_row,v2_row);
                        ROW(m2, r_row) = M_vcnv_to_swp(ROW(m2,r_row), v2_r_row, dim);
                        ROW(m2, row) =   M_vcnv_to_swp(ROW(m2,row), v2_row, dim);

                    }
            }
            r_row = r_row ++;
        }
        if (error != 0) goto err;
    }
    M_free(m1);
    M_free_vec(v1_r_row);
    M_free_vec(v1_row);
    M_free_vec(v2_r_row);
    M_free_vec(v2_row);

    return(m2);

err:
    M_free(m1);
    M_free_vec(v1_r_row);
    M_free_vec(v1_row);
    M_free_vec(v2_r_row);
    M_free_vec(v2_row);
    if(local) M_free(m2);

    return(0);
}


/*NH*/
/*
   This searches the r_row column of
   the data matrix for(the first non-zero element below
   the diagonal. if (it finds one,) the
   switches rows so that the non-zero element is on the
   diagonal. this same operation is applied to the inv
   matrix. if (no non-zero element exists in a column, the
   matrix is singular && no inverse exists.
*/

M_inv_pivot(r_row, m1, m2)
int     r_row;
MAT     *m1, *m2;
{
    int   error, n_row, dim;
    MREAL  *v_n_row, *v_r_row;

    dim = NL1;
    error = 2;  /* no inverse exists */
    n_row = r_row;

    while ((error > 0) && (n_row < dim))
    {
        if (!ZERO(MATE1(n_row,r_row)))
        {
            v_r_row =  M_alloc_vec(dim);
            v_n_row =  M_alloc_vec(dim);;
            if (M_errno != 0) return(M_errno);

            v_r_row =  M_vswp_to_cnv(v_r_row, ROW(m1, r_row), dim);
            v_n_row =  M_vswp_to_cnv(v_n_row, ROW(m1, n_row), dim);
            M_ero_sw(m1->m_nl, v_n_row, v_r_row);
            ROW(m1, r_row) =   M_vcnv_to_swp(ROW(m1,r_row), v_r_row, dim);
            ROW(m1, n_row) =   M_vcnv_to_swp(ROW(m1,n_row), v_n_row, dim);

            M_free_vec(v_r_row);
            M_free_vec(v_n_row);

            v_r_row =  M_alloc_vec(dim);
            v_n_row =  M_alloc_vec(dim);;
            if (M_errno != 0) return(M_errno);

            v_n_row =  M_vswp_to_cnv(v_n_row, ROW(m2, n_row), dim);
            v_r_row =  M_vswp_to_cnv(v_r_row, ROW(m2, r_row), dim);
            M_ero_sw(m2->m_nl, v_n_row,v_r_row);
            ROW(m2, r_row) =   M_vcnv_to_swp(ROW(m2,r_row), v_r_row, dim);
            ROW(m2, n_row) =   M_vcnv_to_swp(ROW(m2,n_row), v_n_row, dim);

            M_free_vec(v_r_row);
            M_free_vec(v_n_row);

            error = 0;
        }
        n_row = n_row++;
    }
    return(error);
}






































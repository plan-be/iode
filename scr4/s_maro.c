#include "s_mat.h"


/*NH*/
/*
    Elementary row operation - switching two rows
*/

int M_ero_sw(int dim, MREAL *row1, MREAL *row2)
{
    MREAL*     row;

    row = M_alloc_vec(dim);
    if (row == 0)
    {
	M_errno = M_MEM_ERR;
	return(0);  /* JMP 14-06-96 */
    }

    memcpy( row, row1, dim * sizeof(MREAL));
    memcpy( row1, row2, dim * sizeof(MREAL));
    memcpy( row2, row, dim * sizeof(MREAL));

    M_free_vec(row);
    return(0);
}





/*NH*/
/*
    Row operation - adding a multiple of one row to another

	row2 = row2 + mult * row1
*/

int M_ero_ma(int dim, MREAL mult, MREAL *row1, MREAL *row2)
{
    int   j;

    for(j = 0; j < dim; j ++)
    {
	row2[j] = row2[j] + mult * row1[j];
    }
    return(0);

}

/*NH*/

/*
   Elementary row operation - dividing by a constant
*/

int M_ero_div(int dim, MREAL div, MREAL *row)
{
    int   i;

    for(i = 0; i < dim; i++) row[i] = row[i] / div;
    return(0);
}

















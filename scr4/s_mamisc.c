#include"s_mat.h"

/*
    Makes sum of columns in matrix
*/
/*
M_s_col(dim1, dim2, mat, sum)
int     dim1, dim2;
M_MAT   mat;
M_VECT  sum;
{
   int  i,j;

   for(i = 0; i < dim1; i++) {
	sum[i] = 0;
	for(j = 0; j < dim2; j++) sum[i] += mat[i][j];
	}
}
*/
/* VOID JMP
    Makes sum of rows in matrix


M_s_row(dim1, dim2, mat, sum)
int     dim1, dim2;
M_MAT   mat;
M_VECT  sum;
{
   int  i,j;

   for(i = 0; i < dim2; i++) {
	sum[i] = 0;
	for(j = 0; j < dim1; j++) sum[i] += mat[i][j];
	}
}

*/













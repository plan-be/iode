/**
 *  @header4iode
 * 
 *  Hodrick-Prescott filter.
 * 
 *  List of functions
 *  -----------------
 * 
 *      int HP_calc(double *f_vec, double *t_vec, int nb, double lambda, int std)     Hodrick-Prescott filter. 
 *      void HP_test(double *f_vec, double *t_vec, int nb, int *beg, int *dim)           Prepares HP_calc()
 */
#include "scr4/s_mat.h"
#include "api/lec/lec.h"


/**
 *  Hodrick-Prescott filter. 
 *  The filtering of f_vec is stored in t_vec.
 *  The number of elements in f_vec must be >= 4.
 *  
 *  @param [in]  double*  f_vec  input series
 *  @param [out] double*  t_vec  resulting series, IODE_NAN if nb < 4
 *  @param [in]  int         nb      number of input data (size of f_vec)
 *  @param [in]  double   lambda  weight (?)
 *  @param [in]  int         std     1 if estimated in level, 0 for log estimation
 *  @return      int                 0 on success, -1 on error                         
 *  **TODO: add info on error
 */
int HP_calc(double *f_vec, double *t_vec, int nb, double lambda, int std)
{

    static  double y1[3] = {1.0, -2.0, 1.0},
                      y2[4] = {-2.0, 5.0, -4.0, 1.0},
                      yn[5] = {1.0, -4.0, 6.0, -4.0, 1.0};
    int     i, j;
    MAT     *yt = NULL, 
            *gt = NULL, 
            *a = NULL;

    // nb must be ge 4 (0 <= nb - 1 - j avec j = 3) // JMP 5/7/2017
    if(nb < 4) {
        for(i = 0; i < nb; i++) t_vec[i] = IODE_NAN;
        return(-1);
    }
    // JMP 5/7/2017

    yt = M_alloc(nb, 1);
    gt = M_alloc(nb, 1);
    a  = M_alloc(nb, nb);

    if(yt == NULL || gt == NULL  || a == NULL) goto err;

    // Prépare la matrice (weights)
    for(j = 0; j < 3; j ++) {
        MATE(a, 0, j) = lambda * y1[j];
        MATE(a, nb - 1, nb - 1 - j) = lambda * y1[j];
    }

    for(j = 0; j < 4; j ++) {
        MATE(a, 1, j) = lambda * y2[j];
        MATE(a, nb - 2, nb - 1 - j) = lambda * y2[j];
    }

    for(i = 2; i < nb - 2; i++) {
        for(j = 0; j < 5; j++) MATE(a, i, j + i - 2) = lambda * yn[j];
    }
    for(i = 0; i < nb; i++) MATE(a, i, i) = MATE(a, i, i) + 1;

    // Compute the vector yt to smooth
    for(i = 0; i < nb; i++)
        MATE(yt, i, 0) = f_vec[i];

    // Compute log(yt) if std == 0
    if(std == 0) {
        for(i = 0; i < nb; i++) {
            if(f_vec[i] <= 0) goto err;    // JMP 26-07-11
            MATE(yt, i, 0) = log(MATE(yt, i, 0));
        }
    }

    // Solve the system yt = a . gt and save result in t_vec
    M_solve(gt, a, yt);
    for(i = 0; i < nb; i++)
        t_vec[i] = MATE(gt, i, 0);

    // exp(t_vec) if std == 0
    if(std == 0) {
        for(i = 0; i < nb; i++)
            t_vec[i] = exp(t_vec[i]);
    }

    // Free mem
    M_free(yt);
    M_free(gt);
    M_free(a);
    return(0);

err:
    M_free(yt);
    M_free(gt);
    M_free(a);
    for(i = 0; i < nb; i++) t_vec[i] = IODE_NAN; /* JMP 26-07-11 */
    return(-1);
}


/**
 *  Prepares HP_calc(). 
 *  
 *  Computes 
 *    - *beg = pos of the first non NaN value f_vec
 *    - *dim = number of non NaN consecutive positions starting at *beg
 *  Puts NaN in t_vec before *beg and after *beg + *dim. 
 *  !Does NOT copy f_vec values in t_vec!
 *  
 *  @param [in]  double* f_vec   input data
 *  @param [out] double* t_vec   output data
 *  @param [in]  int        nb      size of f_vec
 *  @param [out] int*       beg     first non NaN position in f_vec
 *  @param [out] int*       dim     number of non NaN consecutive values in f_vec
 *  @return      void           
 *  
 */
void HP_test(double *f_vec, double *t_vec, int nb, int *beg, int *dim)
{
    int     i;

    for(*beg = 0; *beg < nb && !IODE_IS_A_NUMBER(f_vec[*beg]); (*beg)++)
        t_vec[*beg] = IODE_NAN;
    for(*dim = *beg; *dim < nb && IODE_IS_A_NUMBER(f_vec[*dim]); (*dim)++);

    for(i = *dim; i < nb; i++) t_vec[i] = IODE_NAN;
    *dim -= *beg; /* JMP 26-07-11 */
}


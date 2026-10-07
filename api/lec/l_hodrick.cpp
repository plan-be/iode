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
#include "api/iode_scr4.h"
#include <Eigen/Dense>
#include <Eigen/LU>

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
    double y1[3] = {1.0, -2.0, 1.0};
    double y2[4] = {-2.0, 5.0, -4.0, 1.0};
    double yn[5] = {1.0, -4.0, 6.0, -4.0, 1.0};

    // nb must be ge 4 (0 <= nb - 1 - j with j = 3)
    if(nb < 4) 
    {
        for(int i = 0; i < nb; i++) 
            t_vec[i] = IODE_NAN;
        return -1;
    }

    Eigen::VectorXd yt = Eigen::VectorXd::Zero(nb);
    Eigen::VectorXd gt = Eigen::VectorXd::Zero(nb);
    Eigen::MatrixXd a  = Eigen::MatrixXd::Zero(nb, nb);

    // prepare weights
    for(int j = 0; j < 3; j ++) 
    {
        a(0, j) = lambda * y1[j];
        a(nb - 1, nb - 1 - j) = lambda * y1[j];
    }

    for(int j = 0; j < 4; j ++) 
    {
        a(1, j) = lambda * y2[j];
        a(nb - 2, nb - 1 - j) = lambda * y2[j];
    }

    for(int i = 2; i < nb - 2; i++) 
        for(int j = 0; j < 5; j++) 
            a(i, j + i - 2) = lambda * yn[j];

    for(int i = 0; i < nb; i++) 
        a(i, i) += 1;

    // Compute the vector yt to smooth
    for(int i = 0; i < nb; i++)
        yt(i) = f_vec[i];

    // Compute log(yt) if std == 0
    if(std == 0) 
    {
        for(int i = 0; i < nb; i++) 
        {
            if(f_vec[i] <= 0)
            {
                for(int i = 0; i < nb; i++) 
                    t_vec[i] = IODE_NAN;
                return -1;
            } 
            
            yt(i) = log(yt(i));
        }
    }

    Eigen::FullPivLU<Eigen::MatrixXd> lu(a);
    if(!lu.isInvertible())
    {
        for(int i = 0; i < nb; i++) 
            t_vec[i] = IODE_NAN;
        return -1;
    }
    gt = lu.solve(yt);
    
    for(int i = 0; i < nb; i++)
        t_vec[i] = gt(i);

    // exp(t_vec) if std == 0
    if(std == 0) 
    {
        for(int i = 0; i < nb; i++)
            t_vec[i] = exp(t_vec[i]);
    }

    return 0;
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
    for(*beg = 0; *beg < nb && !IODE_IS_A_NUMBER(f_vec[*beg]); (*beg)++)
        t_vec[*beg] = IODE_NAN;
    
    for(*dim = *beg; *dim < nb && IODE_IS_A_NUMBER(f_vec[*dim]); (*dim)++);

    for(int i = *dim; i < nb; i++) 
        t_vec[i] = IODE_NAN;
    
    *dim -= *beg;
}


/**
 *  @header4iode
 * 
 *  Functions to compute statistical tests in the context of an IODE estimation.
 *  These functions are based on matrices created by an estimation run. 
 *  Therefore, an estimation MUST precede the call to these functions. 
 *  
 *  The computed tests are stored in various MAT object described in function E_output().
 */
#include "api/objs/scalars.h"
#include "api/estimation/estimation.h"


/**
 *  Computes the number of degrees of freedom (DF) for each estimated coefficient.
 *  If a coefficient is present in more than one equation, DF = T (number of obs in the estimation sample).
 *  If it appears in 1 equation only, DF = T - number of estimated coefficients in this equation.
 *  
 *  The result is stored in DEG_FREEDOM
 */
void Estimation::E_deg_freed()
{
    int j = 0;
    for(const std::string& scl_name : v_coef_names) 
    {
        if(E_DBS->get_obj_ptr(scl_name)->relax == 0) 
            continue;
        
        int nbr = 0, nbce = 0;
        for(int eq = 0; eq < E_NEQ; eq++) 
        {
            if(E_scl_in_eq(scl_name, eq)) 
            {
                nbr ++;
                nbce = NB_COEFS_EQ(eq);
            }

            if(nbr > 1) 
                DEG_FREEDOM(j) = E_T;
            else 
                DEG_FREEDOM(j) = E_T - nbce;
        }
        j++;
    }
}


/**
 *  Sub function of E_c_loglik().
 */
double Estimation::E_c_umu()
{
    double umu = 0.0;
    Eigen::VectorXd UM = Eigen::VectorXd::Zero(E_T);
    Eigen::VectorXd UM_TMP = Eigen::VectorXd::Zero(E_T);

    for(int i = 0 ; i < E_NEQ  ; i++) 
    {
        UM.setZero();
        for(int j = 0 ; j < E_NEQ ; j++) 
        {
            UM_TMP = U.row(j);                  // UM_TMP = U[j, 0:E_T]
            UM_TMP *= IVCU(i, j);               // UM_TMP = UM_TMP * IVCU[..]
            UM += UM_TMP;                       // UM = UM + UM_TMP
        }
        UM_TMP = U.row(i);                      // UM_TMP = U[i, 0:E_T]
        umu += UM_TMP.dot(UM);
    }

    return umu;
}


/**
 *  Computes the log-likelihood for each estimated equation of the block.
 *  
 *  @return int 0 always
 */
int Estimation::E_c_loglik()
{
    double loglik;
    switch(E_MET) 
    {
        case 0 :
        case 2 :
            for(int i = 0 ; i < E_NEQ ; i++)
                LOGLIK(i) = - (E_T * 0.5) * (1 + log(2 * M_PI * VCU(i, i)));
            break;

        default :
            loglik = - 0.5 * (E_NEQ * E_T * log(2 * M_PI)
                              + E_T * log(VCU.determinant())
                              + E_c_umu());
            for(int i = 0 ; i < E_NEQ ; i++) 
                LOGLIK(i) = loglik;
            break;
    }

    return 0;
}

/**
 *  Computes MCORR, the matrix of covariances between the estimated coefficients.
 *  
 *  @return int 0 always
 */
int Estimation::E_c_mcorr()
{
    double vii, vij, vjj;
    MCORR.setZero(E_NCE, E_NCE);
    for(int i = 0 ; i< E_NCE ; i++) 
    {
        vii = VCC(i, i);
        for(int j = 0 ; j <= i ; j++) 
        {
            vjj = VCC(j, j);
            vij = VCC(i, j);
            MCORR(i, j) = div_not_0(vij, sqrt_not_neg(vii * vjj));
            MCORR(j, i) = MCORR(i, j);
        }
    }
    return 0;
}


/**
 *  Computes MCORRU, the matrix of covariances between the residuals.
 *  
 *  @return int 0 always
 */
int Estimation::E_c_mcorru()
{
    double vii, vij, vjj;
    MCORRU.setZero(E_NEQ, E_NEQ);
    for(int i = 0 ; i < E_NEQ ; i++) 
    {
        vii = VCU(i, i);
        for(int j = 0 ; j <= i ; j++) 
        {
            vjj = VCU(j, j);
            vij = VCU(i, j);
            MCORRU(i, j) = div_not_0(vij, sqrt_not_neg(vii * vjj));
            MCORRU(j, i) = MCORRU(i, j);
        }
    }
    return 0;
}


/**
 *  Computes the standard deviation of each estimated coefficient and saves their values 
 *  in global_ws_scl.
 *  
 *  @return int 0 always
 */
int Estimation::E_c_ttests()
{
    int j = 0;
    std::shared_ptr<Scalar> scl_ptr;
    for(const std::string& scl_name : v_coef_names) 
    {
        scl_ptr = E_DBS->get_obj_ptr(scl_name);
        scl_ptr->std = 0.0;
        if(scl_ptr->relax == 0) 
            continue;
        
        if(E_MET == IodeEquationMethod::EQ_MAX_LIKELIHOOD)
            scl_ptr->std = sqrt_not_neg(VCC(j, j));
        else
            scl_ptr->std = sqrt_not_neg( div_not_0(VCC(j, j) * E_T, DEG_FREEDOM(j)) );
        
        j++;
    }

    return 0;
}


/**
 *  Computes the statistical tests after an estimation and saves the tests in MAT objects: 
 *  
 *  - DEG_FREEDOM            (1 x NCE)    : Degrees of freedom of each coefficient
 *  - STDERR        (1 x NEQ)    : Std error of each equation 
 *  - SSRES         (1 x NEQ)    : Sum of squares of residuals of each eq
 *  - MEAN_Y        (1 x NEQ)    : Mean of the LHS on each equation 
 *  - STDEV         (1 x NEQ)    : Std deviation of each equation 
 *  - RSQUARE       (1 x NEQ)    : R-square of each equation
 *  - RSQUARE_ADJ   (1 x NEQ)    : Adjusted R-square of each equation
 *  - DW            (1 x NEQ)    : Durbin-Watson test of each equation
 *  - FSTAT         (1 x NEQ)    : F-Stat of each equation
 *  - LOGLIK        (1 x NEQ)    : Log-likelihood of each equation
 *  - STD_PCT       (1 x NEQ)    : Standard errors in % for each equation
 *  - MCORR         (NCE x NCE)  : Correlation matrix bw coefficients
 *  - MCORRU        (NEQ x NEQ)  : Correlation matrix bw error terms of equations
 *  - DEV           (NEQ x T)    : Deviation between observed and calculated values
 *  
 *  @return  int    -1 if one of the tests has generated an error. 0 otherwise
 */
int Estimation::E_output()
{
    /* 1. OUTPUT BY COEFFICIENT */
    E_deg_freed();
    if(E_c_mcorr()) 
        return -1;
    if(E_c_ttests()) 
        return -1;

    /* 2. OUTPUT BY EQUATION */
    if(E_c_mcorru()) 
        return -1;
    
    double sum, tmp;
    for(int i = 0 ; i < E_NEQ ; i++) 
    {
        SSRES(i) = VCU(i, i) * E_T;
        STDERR(i) = sqrt_not_neg(div_not_0(VCU(i, i) * E_T, (E_T - NB_COEFS_EQ(i))));
        MEAN_Y(i) = LHS.row(i).sum() / E_T;

        sum = 0.0;
        for(int j = 0 ; j < E_T ; j++) 
        {
            tmp = LHS(i, j) - MEAN_Y(i);
            DEV(i, j) = tmp;
            sum += tmp * tmp;
        }

        STDEV(i) = sqrt_not_neg(sum / (E_T - 1));

        RSQUARE(i) = 1 - div_not_0(E_T * VCU(i, i), sum);

        RSQUARE_ADJ(i) = 1 - div_not_0((1 - RSQUARE(i)) * (E_T - 1), E_T - NB_COEFS_EQ(i));

        FSTAT(i) = div_not_0(RSQUARE(i) * (E_T - NB_COEFS_EQ(i)),
                              (1 - RSQUARE(i)) * (NB_COEFS_EQ(i) - 1));

        STD_PCT(i) = div_not_0(100 * STDERR(i), MEAN_Y(i));
        
        // Durbin-Watson 
        sum = 0.0;
        for(int j = 0; j < E_T - 1 ; j++) 
        {
            tmp = U(i, j + 1) - U(i, j);
            sum += tmp * tmp;
        }
        DW(i) =  div_not_0(sum, E_T * VCU(i, i));
    }

    /* LOGLIK */
    E_c_loglik();

    return 0;
}

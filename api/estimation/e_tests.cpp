/**
 *  @header4iode
 * 
 *  Functions to compute statistical tests in the context of an IODE estimation.
 *  These functions are based on matrices created by an estimation run. 
 *  Therefore, an estimation MUST precede the call to these functions. 
 *  
 *  The computed tests are stored in various MAT object described in function E_output().
 *  
 *  List of functions 
 *  -----------------
 *      int E_output()                                  Computes the statistical tests after an estimation and saves the tests in MAT objects.
 *      
 *  Utility functions
 *  -----------------
 *      double M_c_line(MAT* m1, int line, int oper)    Adds, substracts or multiplies all elements of a matrix line.
 *      double E_div_0(double a, double b)              Divides a by b if b is not null. If b is null, returns 0.0.
 *      double E_sqrt(double val)                       Returns the square root of val if val >= 0. If not, returns 0.0.
 */
#include "api/objs/scalars.h"
#include "api/estimation/estimation.h"


/**
 *  Adds, substracts or multiplies all elements of a matrix line.
 *  
 *  @param [in] MAT* m1     input matrix
 *  @param [in] int  line   line to reduce
 *  @param [in] int  oper   operator : '+', '-' or '*'
 *  @return     double      result of the operator on all elements of m1[line]
 */
double Estimation::M_c_line(MAT* m1, int line, int oper)
{
    double  res = 0.0;
    int     i;

    if(line > M_NL(m1)) return(res);
    for(i = 0; i < M_NC(m1); i++) {
        switch(oper) {
            case '+' :
                res += MATE1(line, i);
                break;
            case '-' :
                res -= MATE1(line, i);
                break;
            case '*' :
                res *= MATE1(line, i);
                break;
            default  :
                return(res);
        }
    }
    return(res);
}


/**
 *  Divides a by b if b is not null. If b is null, returns 0.0.
 *  
 *  @param [in] double  a   numerator
 *  @param [in] double  b   denominator
 *  @return     double      0.0 if b is null, a / b if not
 */
double Estimation::E_div_0(double a, double b)
{
    if(b == 0) return(0.0);

    return(a/b);
}


/**
 *  Returns the square root of val if val >= 0. If not, returns 0.0.
 */
double Estimation::E_sqrt(double val)
{
    if(val < 0)
        return(0.0);
    else
        return(sqrt(val));
}


/**
 *  Computes the number of degrees of freedom (DF) for each estimated coefficient.
 *  If a coefficient is present in more than one equation, DF = T (number of obs in the estimation sample).
 *  If it appears in 1 equation only, DF = T - number of estimated coefficients in this equation.
 *  
 *  The result is stored in E_DF
 */
void Estimation::E_deg_freed()
{
    int j, eq, cf, nbr, nbce;

    std::string coef_name;
    for(cf = 0, j = 0; cf < E_NC; cf++) 
    {
        coef_name = E_DBS->get_name(E_C_NBS[cf]);
        if(KSVAL(E_DBS, coef_name)->relax == 0) 
            continue;
        nbr = 0;
        for(eq = 0; eq < E_NEQ; eq++) 
        {
            if(E_scl_in_eq(cf, eq)) 
            {
                nbr ++;
                nbce = (int) MATE(E_NBCE, 0, eq);
            }
            if(nbr > 1) 
                MATE(E_DF, 0, j) = E_T;
            else 
                MATE(E_DF, 0, j) = E_T - nbce;
        }
        j++;
    }
}


/**
 *  Sub function of E_c_loglik().
 */
double Estimation::E_c_umu()
{
    int         i, j;
    double      umu = 0.0;
    MAT         *E_UMU = M_alloc(1, 1);

    for(i = 0 ; i < E_NEQ  ; i++) {
        M_clear(E_UM);                                    /* UM = 0 */
        for(j = 0 ; j < E_NEQ ; j++) {
            M_extr(E_UMT, E_U, j, 0, 1, E_T);           /* UMTMP = U[..] */
            M_trans(E_UMTMP, E_UMT);
            M_scale(E_UMTMP, E_UMTMP, MATE(E_IVCU, i, j));/* UMTMP=UMTMP*IVCU[..] */
            M_calc(E_UM, E_UM, E_UMTMP, '+');             /* UM = UM + ,UMTMP */
        }

        M_extr(E_UMT, E_U, i, 0, 1, E_T);           /* UMTMP = U[..] */
        M_prod(E_UMU, E_UMT, E_UM);
        umu += MATE(E_UMU, 0, 0);
    }

    M_free(E_UMU);
    return(umu);
}


/**
 *  Computes the log-likelihood for each estimated equation of the block.
 *  
 *  @return int 0 always
 */
int Estimation::E_c_loglik()
{
    int     i;
    double  loglik;

    switch(E_MET) {
        case 0 :
        case 2 :
            for(i = 0 ; i < E_NEQ ; i++)
                MATE(E_LOGLIK, 0, i) =
                    - (E_T * 0.5) * (1 + log(2 * M_PI * MATE(E_VCU, i, i)));
            break;

        default :
            loglik = - 0.5 * (E_NEQ * E_T * log(2 * M_PI)
                              + E_T * log(M_det_1(E_VCU))
                              + E_c_umu()) ;
            for(i = 0 ; i < E_NEQ ; i++) MATE(E_LOGLIK, 0, i) = loglik;
            break;
    }

    return(0);
}

/**
 *  Computes E_MCORR, the matrix of covariances between the estimated coefficients.
 *  
 *  @return int 0 always
 */
int Estimation::E_c_mcorr()
{
    int         i, j;
    double      vii, vij, vjj;

    for(i = 0 ; i< E_NCE ; i++) {
        vii = MATE(E_VCC, i, i);
        for(j = 0 ; j <= i ; j++) {
            vjj = MATE(E_VCC, j, j);
            vij = MATE(E_VCC, i, j);
            if((vii * vjj) > 0.0)
                MATE(E_MCORR, i, j) = MATE(E_MCORR, j, i) =
                                          vij / E_sqrt(vii * vjj);
            else
                MATE(E_MCORR, i, j) = MATE(E_MCORR, j, i) = 0.0;
        }
    }
    return(0);
}


/**
 *  Computes E_MCORRU, the matrix of covariances between the residuals.
 *  
 *  @return int 0 always
 */
int Estimation::E_c_mcorru()
{
    int         i, j;
    double      vii, vij, vjj;

    for(i = 0 ; i < E_NEQ ; i++) {
        vii = MATE(E_VCU, i, i);
        for(j = 0 ; j <= i ; j++) {
            vjj = MATE(E_VCU, j, j);
            vij = MATE(E_VCU, i, j);
            MATE(E_MCORRU, i, j) = MATE(E_MCORRU, j, i) =
                                       E_div_0(vij, E_sqrt(vii * vjj));  /* JMP 05-10-00 */
        }
    }
    return(0);
}


/**
 *  Computes the standard deviation of each estimated coefficient and saves their values 
 *  in global_ws_scl.
 *  
 *  @return int 0 always
 */
int Estimation::E_c_ttests()
{
    int     i, j;
    Scalar* scl;

    std::string coef_name;
    for(i = 0, j = 0 ; i < E_NC ; i++) 
    {
        coef_name = E_DBS->get_name(E_C_NBS[i]);
        scl = KSVAL(E_DBS, coef_name);
        scl->std = 0.0;
        if(scl->relax == 0) 
            continue;
        if(E_MET == 4) // ??? Max likelihood ???
            scl->std = E_sqrt(MATE(E_VCC, j, j));
        else
            scl->std = E_sqrt(E_div_0(MATE(E_VCC, j, j) * E_T, MATE(E_DF, 0, j)));
        j++;
    }

    return(0);
}


/**
 *  Computes the statistical tests after an estimation and saves the tests in MAT objects: 
 *  
 *  - E_DF            (1 x NCE)    : Degrees of freedom of each coefficient
 *  - E_STDERR        (1 x NEQ)    : Std error of each equation 
 *  - E_SSRES         (1 x NEQ)    : Sum of squares of residuals of each eq
 *  - E_MEAN_Y        (1 x NEQ)    : Mean of the LHS on each equation 
 *  - E_STDEV         (1 x NEQ)    : Std deviation of each equation 
 *  - E_RSQUARE       (1 x NEQ)    : R-square of each equation
 *  - E_RSQUARE_ADJ   (1 x NEQ)    : Adjusted R-square of each equation
 *  - E_DW            (1 x NEQ)    : Durbin-Watson test of each equation
 *  - E_FSTAT         (1 x NEQ)    : F-Stat of each equation
 *  - E_LOGLIK        (1 x NEQ)    : Log-likelihood of each equation
 *  - E_STD_PCT       (1 x NEQ)    : Standard errors in % for each equation
 *  - E_MCORR         (NCE x NCE)  : Correlation matrix bw coefficients
 *  - E_MCORRU        (NEQ x NEQ)  : Correlation matrix bw error terms of equations
 *  - E_DEV           (NEQ x T)    : Deviation between observed and calculated values
 *  
 *  @return  int    -1 if one of the tests has generated an error. 0 otherwise
 */
int Estimation::E_output()
{
    int     i, j;
    double  sum, tmp /*, sum1 */;

    /* 1. OUTPUT BY COEFFICIENT */
    E_deg_freed();
    if(E_c_mcorr()) return(-1);
    if(E_c_ttests()) return(-1);

    /* 2. OUTPUT BY EQUATION */
    if(E_c_mcorru()) return(-1);
    for(i = 0 ; i < E_NEQ ; i++) {
        MATE(E_SSRES, 0, i)  = MATE(E_VCU, i, i) * E_T;
        MATE(E_STDERR, 0, i) =
            E_sqrt(E_div_0(MATE(E_VCU, i, i) * E_T, (E_T - MATE(E_NBCE, 0, i))));
        MATE(E_MEAN_Y, 0, i) = M_c_line(E_LHS, i, '+') / E_T;

        sum = 0.0;
        /*
        	sum1 = 0.0;
        */
        for(j = 0 ; j < E_T ; j++) {
            tmp = MATE(E_LHS, i, j) - MATE(E_MEAN_Y, 0, i);
            MATE(E_DEV, i, j) = tmp;
            sum += tmp * tmp;
            /*
            	    tmp = MATE(E_RHS, i, j) - MATE(E_MEAN_Y, 0, i);
            	    sum1 += tmp * tmp;
            */
        }

        MATE(E_STDEV, 0, i) = E_sqrt(sum / (E_T - 1));

        /*
        	MATE(E_RSQUARE, 0, i) = E_div_0(sum1, sum);
        */
        MATE(E_RSQUARE, 0, i) = 1 - E_div_0(E_T * MATE(E_VCU, i, i), sum);

        MATE(E_RSQUARE_ADJ, 0, i) = 1 - E_div_0(
                                        (1 - MATE(E_RSQUARE, 0, i)) * (E_T - 1), E_T - MATE(E_NBCE, 0, i));

        MATE(E_FSTAT, 0, i) = E_div_0(
                                  MATE(E_RSQUARE, 0, i) * (E_T - MATE(E_NBCE, 0, i)),
                                  (1 - MATE(E_RSQUARE, 0, i)) * (MATE(E_NBCE, 0, i) - 1));

        MATE(E_STD_PCT, 0, i) = E_div_0(
                                    100 * MATE(E_STDERR, 0, i), MATE(E_MEAN_Y, 0, i));
        
        // Durbin-Watson 
        sum = 0.0;
        for(j = 0; j < E_T - 1 ; j++) {
            tmp = MATE(E_U, i, j + 1) - MATE(E_U, i, j);
            sum += tmp * tmp;
        }
        MATE(E_DW, 0, i) =  E_div_0(sum, E_T * MATE(E_VCU,i, i));
    }

    /* LOGLIK */
    E_c_loglik();

    return(0);
}

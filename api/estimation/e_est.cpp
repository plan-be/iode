/**
 *  @header4iode
 * 
 *  Estimation functions. 
 *  These functions do not save the results in the equations 
 *  themselves: it is the role of the upper level function estimate_sample() that 
 *  calls E_est() and saves the results in global_ws_eqs and global_ws_scl.
 *  
 *  See "Specification, Estimation and Analysis of Macroeconomic Models" [Fair, 1984]
 *  for details on the methods implemented in IODE.
 */
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/estimation/estimation.h"


/**
 *  Computes VCC, the matrix of var/covar bw the coefficients (?).
 *  
 *  @return     int 0   always
 */
int Estimation::E_c_gmg()
{
    Eigen::MatrixXd M = Eigen::MatrixXd::Zero(E_T, E_NCE);
    Eigen::MatrixXd M_TMP = Eigen::MatrixXd::Zero(E_T, E_NCE);
    Eigen::MatrixXd MT_TMP = Eigen::MatrixXd::Zero(E_NCE, E_T);
    Eigen::MatrixXd VCC_TMP = Eigen::MatrixXd::Zero(E_NCE, E_NCE);
    
    VCC.setZero();
    for(int i = 0 ; i < E_NEQ  ; i++) 
    {
        M.setZero();
        for(int j = 0 ; j < E_NEQ ; j++) 
        {
            MT_TMP = G.block(0, E_T * j, E_NCE, E_T);       // MT_TMP = G[0:E_NCE, E_T*j:E_T*(j+1)]
            M_TMP = MT_TMP.transpose();                     // M_TMP = MT_TMP^T
            M_TMP *= IVCU(i, j);                            // M_TMP = M_TMP * IVCU[i, j]
            M += M_TMP;                                     // M = M + M_TMP
        }
        if(E_NINSTR > 1) 
            M_TMP = D * M;                                  // M_TMP = D * M
        else 
            M_TMP = M;

        MT_TMP = G.block(0, E_T * i, E_NCE, E_T);           // MT_TMP = G[0:E_NCE, E_T*i:E_T*(i+1)]
        VCC_TMP = MT_TMP * M_TMP;                           // VCC_TMP = MT_TMP * M_TMP
        VCC += VCC_TMP;                                     // VCC = VCC_TMP + VCC
    }

    return 0;
}


/**
 *  Calculates the right member of the equation i at time t with the current values 
 *  of the coefficients. 
 *  
 *  @param [in] int i   equation number in the block 
 *  @param [in] int t   period of time (beginning on E_FROM
 *  @return     double  value of equation i in t 
 */
double Estimation::E_rhs_ij(int i, int t)
{
    if(!v_block_rhs[i])
        return IODE_NAN;
    else
        return v_block_rhs[i]->execute(E_DBV, E_DBS, t + E_FROM);
}


/**
 *  Calculates the right members of the equations (RHS) with the current values of 
 *  the coefficients in E_DBS.
 *  
 *  The result is stored in the RHS(NEQ x T). 
 *  
 *  @return     int     0 on success, -1 on error in the calculation of RHS
 */
int Estimation::E_c_rhs()
{
    double x;
    for(int i = 0 ; i < E_NEQ ; i++) 
    {
        for(int t = 0 ; t < E_T ; t++) 
        {
            x = E_rhs_ij(i, t);
            if(IODE_IS_A_NUMBER(x)) 
                RHS(i, t) = x;
            else  
            {
                error_manager.append_error("Estimation: NaN Generated");
                return -1;
            }
        }
    }
    return 0;
}


/**
 *  Calculates the matrix U of residuals: U = LHS - RHS. 
 *  
 *  @return     int     0 on success, -1 on error in the calculation of RHS
 */
int Estimation::E_residuals()
{
    /* COMPUTE RHS */
    if(E_c_rhs() != 0) 
        return -1;
    U = LHS - RHS;
    return 0;
}


/**
 *  Solves the linear system VCC * DELTA_COEFS = GMU.
 *  The solution is saved in the DELTA_COEFS.
 *  
 *  @return  int    0 or -1 on error
 */
int Estimation::E_deltac()
{   
    Eigen::FullPivLU<Eigen::MatrixXd> lu(VCC);
    if (!lu.isInvertible())
    {
        error_manager.append_error("Estimation: Singular Matrix (VCC)");
        return -1;
    }
    DELTA_COEFS = lu.solve(GMU);
    
    return 0;
}


/**
 *  Computes the line est_coef_nb of Jacobian matrix.
 *  
 *  The idea is to compute 
 *  
 *      f(est_coef_nb + h) - f(est_coef_nb)) / h 
 *  
 *  for each period of the estimation sample and to save the result in G[est_coef_nb,:]
 *  
 *  @note RHS must already have been computed with the current values of the coefs 
 *  @note AFTER the calculation of RHS, a step of size h must have been added to 
 *  the coefficients so that E_rhs_ij(i, j) = f(est_coef_nb + h).
 *   
 *  @param [in] int     coef_nb     coefficient position in the list of all coefs, non estimed include
 *  @param [in] int     est_coef_nb coefficient position in the list of estimated coefs
 *  @param [in] double  h           step used to compute the numerical derivative
 *  @return     int                 0 on success, -1 on error
 */
int Estimation::E_mod_residuals(const std::string& coef_name, int est_coef_nb, double h)
{
    // for each equation
    double x;
    for(int i = 0 ; i < E_NEQ ; i++) 
    {
        if(E_scl_in_eq(coef_name, i)) 
        {
            // If the scalar i is in the equation: calculate the RHS (for each year) 
            // and save the derivative in matrix G: (f(x + h) - f(x)) / h.
            for(int t = 0; t < E_T; t++) 
            {
                x = E_rhs_ij(i, t);
                if(x >= MAXFLOAT) 
                    x = IODE_NAN;
                if(IODE_IS_A_NUMBER(x))
                    G(est_coef_nb, i * E_T + t) = (x - RHS(i, t)) / h;
                else  
                {
                    error_manager.append_error("Estimation: NaN Generated");
                    return -1;
                }
            }
        }
        else
            // Otherwise, place 0s in matrix G for the coefficient in question.
            for(int t = 0; t < E_T; t++)
                G(est_coef_nb, i * E_T + t) = 0;
    }

    return 0;
}


/**
 *  Computes the numerical Jacobian of the non linear equation system.
 *  The solution is saved in the G (E_NCE, E_T * E_NEQ).
 *  
 *          EQ1:0..T-1  EQ2:T..2xT-1  ...  EQM:(M-1)xT...MxT-1
 *  coef1 |           |             |    |                      |
 *  coef2 |           |             |    |                      |
 *  ...   |           |             |    |                      |
 *  coefn |           |             |    |                      |
 *  
 *  @return  int    0 on success or -1 on error
 */
int Estimation::E_jacobian()
{
    int j = 0;
    double h = 1e-4, oldc;
    std::shared_ptr<Scalar> scl_ptr;
    for(const std::string& scl_name : v_coef_names) 
    {
        scl_ptr = E_DBS->get_obj_ptr(scl_name);
        // Only for estimated coeffs (relax <> 0)
        if(scl_ptr->relax != 0) 
        {      
            oldc = scl_ptr->value;      // store previous value of the coef
            if(fabs(oldc) < 1e-15)      // or 0.1 if coef is close to 0.0
                oldc = 0.1;                      
            scl_ptr->value = oldc * (1.0 + h);              // coef increased by h procents
            if(0 != E_mod_residuals(scl_name, j, oldc * h)) 
            {  /* compute G : (NCE, T*N) */
                // PROBLEME : reset et sort avec -1
                scl_ptr->value = oldc;                      // reset coef to previous value
                return -1;
            }
            scl_ptr->value = oldc;                          // reset coef to previous value
            j++;
        }
    }

    return 0;
}


/**
 *  Checks if the coefficient coef_nb is in the equation eq_nb.
 *  
 *  @param [in] int coef_nb     position of the coefficient to search in v_coef_names.
 *  @param [in] int eq_nb       equation position in the estimated block
 *  @return     int             1 if coef_nb is in eq_nb, 0 otherwise
 */
int Estimation::E_scl_in_eq(const std::string& coef_name, int eq_nb)
{
    std::shared_ptr<CLEC> clec = v_block_rhs[eq_nb];
    if(!clec)
        return 0;

    for(const std::string& name: clec->v_obj_names)
    {
        if(is_coefficient(name) && name == coef_name) 
            return 1;
    }

    return 0;
}


/**
 *  Computes GMU.
 *  
 *  TODO: describe GMU (from Fair's book).
 *  
 *  @return int     0   always
 *  
 */
int Estimation::E_c_gmu()
{
    Eigen::VectorXd UM = Eigen::VectorXd::Zero(E_T);
    Eigen::VectorXd UM_TMP = Eigen::VectorXd::Zero(E_T);
    Eigen::MatrixXd M_TMP = Eigen::MatrixXd::Zero(E_NCE, E_T);
    Eigen::VectorXd GMU_TMP = Eigen::VectorXd::Zero(E_NCE);
    
    GMU.setZero();
    for(int i = 0; i < E_NEQ; i++) 
    {
        UM.setZero();
        for(int j = 0; j < E_NEQ; j++) 
        {
            UM_TMP = U.row(j);                      // UM_TMP = U[j, 0:E_T] (residuals of eq j)
            UM_TMP *= IVCU(i, j);                   // UM_TMP = UM_TMP * IVCU[i, j]
            UM += UM_TMP;                           // UM = UM + UM_TMP
        }

        if(E_NINSTR >= 1)
            UM_TMP = D * UM;                        // UM_TMP = D * UM
        else 
            UM_TMP = UM;

        M_TMP = G.block(0, E_T * i, E_NCE, E_T);    // M_TMP = G[0:E_NCE, E_T*i:E_T*(i+1)]
        GMU_TMP = M_TMP * UM_TMP;                   // GMU_TMP = M_TMP * UM_TMP
        GMU += GMU_TMP;                             // GMU = GMU_TMP + GMU
    }

    return 0;
}


/**
 *  Computes E_CONV_TEST = sum of the squares of the relative differences between 2 iterations.
 *  
 *  If E_CONV_TEST < E_EPS, E_CONV if set to 1 and the returned value indicates that a solution 
 *  has been found.
 *  
 *  @return  int    1 if a solution has been reached, 
 *                  0 otherwise.
 */
int Estimation::E_testcv()
{
    double sum = 0, tmp, ci, dci;

    E_CONV = 0;
    E_get_C();
    for(int i = 0, j = 0; i < v_coef_names.size(); i++) 
    {
        // relax == 0
        if(SMO(i) == 0) 
            continue;

        ci  = COEFS(i);
        dci = DELTA_COEFS(j);
        if(ci != 0) 
        {
            tmp = fabs(dci / ci);
            if(tmp < fabs(ci)) 
                sum += tmp * tmp;
            else 
                sum += ci * ci;
        }
        j++;
    }

    E_CONV_TEST = sqrt(sum);
    if(E_CONV_TEST <= E_EPS) 
        E_CONV = 1;
    return E_CONV;
}


/**
 *  Adds to each estimated coefficient (i.e. where relax <> 0) the value 
 *  of a step (DELTA_COEFS) calculated in E_deltac() multiplied by lambda (SMO).
 *  
 *  @return int     0   always
 */
int Estimation::E_adaptcoef()
{
    for(int i = 0, j = 0; i < v_coef_names.size(); i++) 
    {
        // relax = 0
        if(SMO(i) == 0) 
            continue;

        // COEFS = COEFS + delta_COEFS * lambda
        COEFS(i) += DELTA_COEFS(j) * SMO(i);
        j++;
    }
    
    E_put_C();
    return 0;
}


/**
 *  Computes variance/covariance matrix of the residuals: 
 *  
 *      VCU = (U x U') / T  (NEQ x NEQ)
 *  
 *  @return     int     0 always
 */
int Estimation::E_c_vcu()
{
    // Note: In Eigen, aliasing refers to assignment statement in which 
    //       the same matrix (or array or vector) appears on the left and 
    //       on the right of the assignment operators. Statements like 
    //       'mat = 2 * mat' or 'mat = mat.transpose()' exhibit aliasing.
    //       The method noalias() assumes no alias and improve performance
    //       but must be used with care.
    VCU.noalias() = U * U.transpose();
    VCU /= static_cast<double>(E_T);
    return 0;
}


/**
 *  Computes the inverse of VCU: 
 *  
 *      IVCU = VCU^{-1} (NEQ x NEQ)
 *  
 *  @return     int     0 on success, -1 on error
 */
int Estimation::E_c_ivcu()
{
    Eigen::FullPivLU<Eigen::MatrixXd> lu(VCU);
    if(!lu.isInvertible())
    {
        error_manager.append_error("Estimation : Singular Matrix (VCU)");
        return -1;
    }

    IVCU = lu.inverse();
    return 0;
}


/**
 *  Computes MCU (NEQ x NEQ), (the matrix of the correlations bw residuals in a 
 *  system of equations TODO:Check this).
 *  
 *       MCU[i,j] = MCU[j,i] = VCU[i,j] / sqrt(VCU[i, i] * VCU[j,j])
 *  
 *  @return     int     0 always
 */
int Estimation::E_c_mcu()
{
    double x = 0.0;
    MCU.setZero();
    for(int i = 0 ; i < E_NEQ ; i++)
    {
        for(int j = 0 ; j <= i ; j++) 
        {
            x = sqrt(VCU(i, i) * VCU(j, j));
            if(!IODE_IS_0(x))
                MCU(i, j) = MCU(j, i) = VCU(i, j) / x;
        }
    }

    return 0;
}

/**
 *  Computes the inverse of VCC and save the result in VCC:
 *  
 *      VCC = VCC^{-1} (NCE x NCE)
 *  
 *  @return     int     0 on success, -1 on error
 */
int Estimation::E_c_ivcc()
{
    Eigen::FullPivLU<Eigen::MatrixXd> lu(VCC);
    if(!lu.isInvertible())
    {
        error_manager.append_error("Estimation : Singular Matrix (VCC)");
        return -1;
    }

    Eigen::MatrixXd VCC_TMP = lu.inverse();
    VCC = VCC_TMP;
    return 0;
}

/**
 * Computes VCC. TODO: describe
 *    
 *  @return     int     0 on success, -1 on error
 */
int Estimation::E_c_vcc()
{
    IVCU.setZero();
    E_c_ivcu();
    E_c_gmg();
    if(E_c_ivcc()) 
        return -1;
    E_c_vcu();
    return 0;
}

/**
 *  Estimates a block of equations. The parameters (method, convergence threshold...) 
 *  are read in the global variables (E_MET...). 
 *  
 *  More details in E_estim() and e_prep.c.
 *  
 *  This function can estimate coefficients by the 4 methods available 
 *  in IODE. See E_estim().
 *  
 *  @see Fair(1984) for the details on the methods.
 *   
 *  @return int 0 on success, -1 if a solution can not be reached.
 *  
 */
int Estimation::E_gls()
{
    int step = 0;
    int conv = 0;

    E_IT = 0;
    E_CONV = 0;
    E_CONV_TEST = 9999.99;

again: /* first step of all methods and second step for Zellner and 3 stages methods */
    while(conv == 0 && E_IT < E_MAXIT) 
    {
        if(E_residuals() || E_jacobian()) 
            goto err;
        
        if(E_MET == 4) 
            if(E_c_vcu() || E_c_ivcu()) 
                goto err;
        
        if(E_c_gmg() || E_c_gmu() || E_deltac()) 
            goto err;
        
        E_IT++;
        conv = E_testcv();
        kmsg("Estimating : iteration %d (||eps|| = %g)", E_IT, E_CONV_TEST);
        
        if(E_adaptcoef()) 
            goto err;
    }

    E_c_gmg(); /* TMP ??? */
    if(E_residuals() || E_c_vcu() || E_c_ivcu()) 
        goto err;

    switch(E_MET) 
    {
        case EQ_GLS :
        case EQ_ZELLNER :
            if(step == 0) 
            {
                if(E_c_mcu()) 
                    goto err;
                conv = 0;
                step = 1;
                goto again; /* next step for Zellner and GLS */
            }
            if(E_c_ivcc()) 
                goto err;
            break;

        default : /* LSQ, INSTRUMENTAL and MAX_LIKELIHOOD */
            if(E_c_mcu() || E_c_vcc()) 
                goto err;
            break;
    }

    if(E_IT < E_MAXIT && conv != 0) 
        E_CONV = 1;
    else 
        E_CONV = 0;
    
    kmsg("Solution%s reached after %d iteration(s). Creating results file ...",
        ((E_CONV == 0) ? " not" : ""), E_IT);
    
    return (E_CONV == 1) ? 0 : -1;

err :
    return -1;
}


/**
 *  Estimates a block of equations.
 *
 *  At the end of the estimation, only the estimated coefficients and their 
 *  associated tests are directly saved in dbs. 
 *  
 *  The special scalars containing the tests by equations related to the error terms
 *  are NOT saved here: this is done by estimate_sample(). Idem for the variables 
 *  _YCALC, _YOBS and _YRES. The equations with their tests and new
 *  sample, instruments and/or block are also saved by estimate_sample().
 *  
 *  The LEC expressions of each equations must be given in the parameter "lecs". 
 *  They can differ from their current value in the equation workspace.
 *  
 *  Available methods are:
 *       0: LSQ
 *       1: Zellner
 *       2: Instrumental Variables (IV)
 *       3: GLS (3SLS)
 *       4: Max.Likelihood (not implemented in IODE)
 * 
 *  @param [in] char**  endos   list of equations names (endogenous var) to estimate simultaneously
 *  @param [in] char**  lecs    list of corresponding LEC equations
 *  @param [in] char**  instrs  list of LEC formulas (instruments)
 *  @return     int             0 if a solution is found, -1 otherwise
 */
int Estimation::E_est(const std::vector<std::string>& v_block_endos, const std::vector<std::string>& v_block_lecs, 
    const std::vector<std::string>& v_block_instrs)
{
    int  rc = -1, rc_prep = -1, rc_est = -1;

    this->v_block_endos = v_block_endos;
    this->v_block_lecs = v_block_lecs;
    this->v_block_instrs = v_block_instrs;

    if(!E_DBV->get_sample() || E_DBV->get_sample()->nb_periods == 0)
        throw std::runtime_error("Variables database has no sample defined");
    if(!E_SMPL)
        throw std::runtime_error("Estimation sample has not been set for estimation");
    E_FROM = E_SMPL->start_period.difference(E_DBV->get_sample()->start_period);
    E_T    = E_SMPL->nb_periods;

    rc_prep = E_prep();
    if(rc_prep != 0)
    {
        std::string error_msg = "Could not prepare estimation:";
        error_manager.prepend_error(error_msg);
        error_manager.display_last_error();
        return -1;
    }
    
    rc_est = E_gls();
    if(rc_prep == 0 && rc_est == 0) 
        rc = E_output();

    return rc;
}

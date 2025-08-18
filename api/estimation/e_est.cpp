/**
 *  @header4iode
 * 
 *  Estimation functions. 
 *  These functions do not save the results in the equations 
 *  themselves: it is the role of the upper level function KE_est_s() that 
 *  calls E_est() and saves the results in KE_WS and KS_WS.
 *  
 *  See "Specification, Estimation and Analysis of Macroeconomic Models" [Fair, 1984]
 *  for details on the methods implemented in IODE.
 *  
 *  List of functions 
 *  -----------------
 *      int E_est(char** endos, char** lecs, char** instrs)     Estimates a block of equations
 *      int E_scl_in_eq(int coef_nb, int eq_nb)                 Checks if the coefficient coef_nb is in the equation eq_nb.
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
    int i, j;

    M_clear(E_VCC);
    for(i = 0 ; i < E_NEQ  ; i++) {
        M_clear(E_M);                                       /* M = 0 */
        for(j = 0 ; j < E_NEQ ; j++) {
            M_extr(E_MTMPP, E_G, 0, E_T * j, E_NCE, E_T);   /* MTMP = G[..] */
            M_trans(E_MTMP, E_MTMPP);                       /* MTMP = MTMP' */
            M_scale(E_MTMP, E_MTMP, MATE(E_IVCU, i, j));    /* MTMP=MTMP*IVCU[..] */
            M_calc(E_M, E_M, E_MTMP, '+');                  /* M = M + MTMP */
        }
        if(E_NINSTR > 1) M_prod(E_MTMP, E_D, E_M);          /* MTMP = D.M */
        else M_copy(E_MTMP, E_M);

        M_extr(E_MTMPP, E_G, 0, E_T * i, E_NCE, E_T);       /* M = G[..] */
        M_prod(E_VCCTMP, E_MTMPP, E_MTMP);                  /* VCCTMP = M.MTMP */
        M_calc(E_VCC, E_VCCTMP, E_VCC, '+');                /* VCC=VCCTMP+VCC */
    }
    return(0);
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
    return(L_exec(E_DBV, E_DBS, E_CRHS[i], t + E_FROM));
}


/**
 *  Calculates the right members of the equations (RHS) with the current values of 
 *  the coefficients in E_DBS.
 *  
 *  The result is stored in the E_RHS(NEQ x T). 
 *  
 *  @return     int     0 on success, -1 on error in the calculation of RHS
 */
int Estimation::E_c_rhs()
{
    int      i, t;
    double   x;

    for(i = 0 ; i < E_NEQ ; i++) {
        for(t = 0 ; t < E_T ; t++) {
            x = E_rhs_ij(i, t);
            if(IODE_IS_A_NUMBER(x)) MATE(E_RHS, i, t) = x;
            else  {
                error_manager.append_error("Estimation: NaN Generated");
                return(-1);
            }
        }
    }

    return(0);
}


/**
 *  Calculates the matrix E_U of residuals: E_U = E_LHS - E_RHS. 
 *  
 *  @return     int     0 on success, -1 on error in the calculation of RHS
 */
int Estimation::E_residuals()
{
    /* COMPUTE RHS */
    if(E_c_rhs() != 0) return(-1);
    M_calc(E_U, E_LHS, E_RHS, '-');
    return(0);
}


/**
 *  Solves the linear system E_VCC * E_dC = E_GMU.
 *  The solution is saved in the MAT E_dC.
 *  
 *  @return  int    0 or -1 on error
 */
int Estimation::E_deltac()
{
    M_solve(E_dC, E_VCC, E_GMU);
    if(M_errno)
    {
        error_manager.append_error("Estimation: Singular Matrix (VCC)");
        return(-1);
    } 
    return(0);
}


/**
 *  Computes the line est_coef_nb of Jacobian matrix.
 *  
 *  The idea is to compute 
 *  
 *      f(est_coef_nb + h) - f(est_coef_nb)) / h 
 *  
 *  for each period of the estimation sample and to save the result in E_G[est_coef_nb,:]
 *  
 *  @note E_RHS must already have been computed with the current values of the coefs 
 *  @note AFTER the calculation of E_RHS, a step of size h must have been added to 
 *  the coefficients so that E_rhs_ij(i, j) = f(est_coef_nb + h).
 *   
 *  @param [in] int     coef_nb     coefficient position in the list of all coefs, non estimed include
 *  @param [in] int     est_coef_nb coefficient position in the list of estimated coefs
 *  @param [in] double  h           step used to compute the numerical derivative
 *  @return     int                 0 on success, -1 on error
 */
int Estimation::E_mod_residuals(int coef_nb, int est_coef_nb, double h)
{
    int     i, j;
    double  x;

    // Pour toute équation
    for(i = 0 ; i < E_NEQ ; i++) {
        if(E_scl_in_eq(coef_nb, i)) {
            // Si le scalaire i est dans l'éq : calculer le RHS (pour chaque année)
            // et sauver la dérivée dans la matrice E_G  : (f(x + h) - f(x)) / h
            for(j = 0 ; j < E_T ; j++) {
                x = E_rhs_ij(i, j);
                if(x >= MAXFLOAT) x = IODE_NAN;
                if(IODE_IS_A_NUMBER(x))
                    MATE(E_G, est_coef_nb, i * E_T + j) =
                        (x - MATE(E_RHS, i, j)) / h;
                else  {
                    error_manager.append_error("Estimation: NaN Generated");
                    return(-1);
                }
            }
        }
        else
            // Sinon, placer des 0 dans la matrice E_G pour le coef en question
            for(j = 0 ; j < E_T ; j++) MATE(E_G, est_coef_nb, i * E_T + j) = 0;
    }

    return(0);
}


/**
 *  Computes the numerical Jacobian of the non linear equation system.
 *  The solution is saved in the MAT E_G (E_NCE, E_T * E_NEQ).
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
    double  h = 1e-4, oldc;
    int     i, j;

    for(i = 0, j = 0 ; i < E_NC ; i++) {
        if(KSVAL(E_DBS, E_C_NBS[i])->relax != 0) {      // Uniquement pour les coef estimés (relax <> 0)
            oldc = KSVAL(E_DBS, E_C_NBS[i])->val;       // Stocke l'ancienne valeur du coef
            if(fabs(oldc) < 1e-15) oldc = 0.1;          //   ou 0.1 si coef proche de nul
            KSVAL(E_DBS, E_C_NBS[i])->val = oldc * (1.0 + h); // coef augmenté de h pourcents
            if(0 != E_mod_residuals(i, j, oldc * h)) {  /* compute G : (NCE, T*N) */
                // PROBLEME : reset et sort avec -1
                KSVAL(E_DBS, E_C_NBS[i])->val = oldc;   // remet l'ancienne valeur du coef
                return(-1);
            }
            KSVAL(E_DBS, E_C_NBS[i])->val = oldc;       /* reset coef */
            j++;
        }
    }

    return(0);
}


/**
 *  Checks if the coefficient coef_nb is in the equation eq_nb.
 *  
 *  @param [in] int coef_nb     position of the coefficient to search in E_C_NBS.
 *  @param [in] int eq_nb       equation position in the estimated block
 *  @return     int             1 if coef_nb is in eq_nb, 0 otherwise
 */
int Estimation::E_scl_in_eq(int coef_nb, int eq_nb)
{
    CLEC    *clec = E_CRHS[eq_nb];
    int    j;

    for(j = 0 ; j < clec->nb_names ; j++)
        if(L_ISCOEF(clec->lnames[j].name) &&
                E_C_NBS[coef_nb] == clec->lnames[j].pos) return(1);

    return(0);
}


/**
 *  Computes E_GMU.
 *  
 *  TODO: describe GMU (from Fair's book).
 *  
 *  @return int     0   always
 *  
 */
int Estimation::E_c_gmu()
{
    int     i, j;

    M_clear(E_GMU);
    for(i = 0 ; i < E_NEQ  ; i++) {
        M_clear(E_UM);                                      /* UM = 0 */
        for(j = 0 ; j < E_NEQ ; j++) {
            M_extr(E_UMT, E_U, j, 0, 1, E_T);               /* UMTMP = U[j,:] = residuals of eq j*/
            M_trans(E_UMTMP, E_UMT);                            
            M_scale(E_UMTMP, E_UMTMP, MATE(E_IVCU, i, j));  /* UMTMP = UMTMP * IVCU[i;j] */
            M_calc(E_UM, E_UM, E_UMTMP, '+');               /* UM = UM + UMTMP */
        }

        if(E_NINSTR >= 1) M_prod(E_UMTMP, E_D, E_UM);       /* UMTMP = D.UM */
        else M_copy(E_UMTMP, E_UM);

        M_extr(E_MTMPP, E_G, 0, E_T * i, E_NCE, E_T);       /* M = G[:,(E_T * i) ... (E_T * (i+1)-1)] */
        M_prod(E_GMUTMP, E_MTMPP, E_UMTMP);                 /* UVCCTMP = M.UMTMP */
        M_calc(E_GMU, E_GMUTMP, E_GMU, '+');                /* GMU=UVCCTMP+GMU */
    }
    return(0);
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
    int         i, j;
    double      sum = 0, tmp, ci, dci;

    E_CONV = 0;
    E_get_C();
    for(i = 0, j = 0 ; i < E_NC ; i++) {
        if(MATE(E_SMO, i, 0) == 0) continue; /* relax == 0 */

        ci  = MATE(E_C, i, 0);
        dci = MATE(E_dC, j, 0);
        if(ci != 0) {
            tmp = fabs(dci / ci);
            if(tmp < fabs(ci)) sum += tmp * tmp;
            else sum += ci * ci;
        }
        j++;
    }
    E_CONV_TEST = sqrt(sum);
    if(E_CONV_TEST <= E_EPS) E_CONV = 1;
    return(E_CONV);
}


/**
 *  Adds to each estimated coefficient (i.e. where relax <> 0) the value 
 *  of a step (E_dC) calculated in E_deltac() multiplied by lambda (E_SMO).
 *  
 *  @return int     0   always
 */
int Estimation::E_adaptcoef()
{
    int i, j;

    for(i = 0, j = 0; i < E_NC; i++) {
        if(MATE(E_SMO, i, 0) == 0) continue;

        /* C = C + dC * lambda */
        MATE(E_C, i, 0) += MATE(E_dC, j, 0) * MATE(E_SMO, i, 0);
        j++;
    }
    E_put_C();
    return(0);
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
    M_xxprim(E_VCU, E_U);
    M_scale(E_VCU, E_VCU, 1.0 / E_T);
    return(0);
}


/**
 *  Computes the inverse of VCU: 
 *  
 *      IVCU = VCU^-1  (NEQ x NEQ)
 *  
 *  @return     int     0 on success, -1 on error
 */
int Estimation::E_c_ivcu()
{
    M_inv_1(E_IVCU, E_VCU);
    if(M_errno)
    {
        error_manager.append_error("Estimation : Singular Matrix (VCU)");
        return(-1);
    } 
    return(0);
}


/**
 *  Computes MCU (NEQ x NEQ), (the matrix of the correlations bw residuals in a 
 *  system of equations TODO:Check this).
 *  
 *       MCU[i,j] = MCU[j,i] = VCU[i,j] / (VCU[i, i] * VCU[j,j])^-2 
 *  
 *  @return     int     0 always
 */
int Estimation::E_c_mcu()
{
    int     i, j;
    double  x;

    M_clear(E_MCU);
    for(i = 0 ; i < E_NEQ ; i++)
        for(j = 0 ; j <= i ; j++) {
            x = sqrt(MATE(E_VCU, i, i) * MATE(E_VCU, j, j));
            if(!IODE_IS_0(x))
                MATE(E_MCU, i, j) = MATE(E_MCU, j, i) = MATE(E_VCU, i, j) / x;
        }

    return(0);
}

/**
 *  Computes the inverse of VCC and save the result in VCC:
 *  
 *      VCC = VCC^-1 (NCE x NCE)
 *  
 *  @return     int     0 on success, -1 on error
 */
int Estimation::E_c_ivcc()
{
    M_inv_1(E_VCCTMP, E_VCC);
    if(M_errno)
    {
        error_manager.append_error("Estimation: Singular Matrix (VCC)");
        return(-1);
    }
    M_copy(E_VCC, E_VCCTMP);
    return(0);
}

/**
 * Computes VCC. TODO: describe
 *    
 *  @return     int     0 on success, -1 on error
 */
int Estimation::E_c_vcc()
{
    M_clear(E_IVCU);
    E_c_ivcu();
    E_c_gmg();
    if(E_c_ivcc()) 
        return(-1);
    E_c_vcu();
    return(0);
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
    int         step = 0;
    int         conv = 0;

    E_IT = 0;
    E_CONV = 0;
    E_CONV_TEST = 9999.99;

again: /* first step of all methods and second step for Zellner and 3 stages methods */
    while(conv == 0 && E_IT < E_MAXIT) {
        if(E_residuals() || E_jacobian()) goto err;
        if(E_MET == 4) {
            if(E_c_vcu() || E_c_ivcu()) goto err;
        }
        if(E_c_gmg() || E_c_gmu() || E_deltac()) goto err;
        E_IT++;
        conv = E_testcv();
        kmsg("Estimating : iteration %d (||eps|| = %g)", E_IT, E_CONV_TEST);
        if(E_adaptcoef()) goto err;
    }

    E_c_gmg(); /* TMP ??? */
    if(E_residuals() || E_c_vcu() || E_c_ivcu()) goto err;

    switch(E_MET) {
        case 3 : /* 3 stages */
        case 1 : /* Zellner */
            if(step == 0) {
                if(E_c_mcu()) goto err;
                conv = 0;
                step = 1;
                goto again; /* next step for Z and 3 */
            }
            if(E_c_ivcc()) goto err;
            break;

        default : /* LSQ, IV */
            if(E_c_mcu() || E_c_vcc()) goto err;
            break;
    }

    if(E_IT < E_MAXIT && conv != 0) E_CONV = 1;
    else E_CONV = 0;
    kmsg("Solution%s reached after %d iteration(s). Creating results file ...",
        ((E_CONV == 0) ? " not" : ""), E_IT);
    return((E_CONV == 1) ? 0 : -1);

err :
    error_manager.print_last_error();
    return(-1);
}


/**
 *  Estimates a block of equations.
 *
 *  At the end of the estimation, only the estimated coefficients and their 
 *  associated tests are directly saved in dbs. 
 *  
 *  The special scalars containing the tests by equations related to the error terms
 *  are NOT saved here: this is done by KE_est_s(). Idem for the variables 
 *  _YCALC, _YOBS and _YRES. The equations with their tests and new
 *  sample, instruments and/or block are also saved by KE_est_s().
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
int Estimation::E_est(char** endos, char** lecs, char** instrs)
{
    int  rc = -1, rc_prep = -1, rc_est = -1;

    M_errno = 0;
    E_ENDOS = endos;
    E_LECS = lecs;
    E_INSTRS = instrs;

    if(E_SMPL == NULL)
        throw std::runtime_error("Estimation sample has not been set for estimation");
    E_FROM = PER_diff_per(&(E_SMPL->s_p1), &(KSMPL(E_DBV)->s_p1));
    E_T    = E_SMPL->s_nb;

    rc_prep = E_prep(lecs, instrs);
    if(rc_prep != 0)
    {
        std::string error_msg = error_manager.get_last_error();
        if(!error_msg.empty())
            throw std::runtime_error("Could not prepare estimation:\n" + error_msg);
    }
    
    rc_est = E_gls();
    if(rc_prep == 0 && rc_est == 0) 
        rc = E_output();
    
    return(rc);
}

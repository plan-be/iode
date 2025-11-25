/**
 *  @header4iode
 * 
 *  Functions to create and to free the global variables used by the estimation functions.
 *  
 *  List of functions 
 *  -----------------
 *      int E_prep(char** lecs, char** instrs)  Prepares the estimation of a group a equations.
 *      void E_get_C()                          Saves in E_C the values of the estimated coefficients. 
 *      void E_put_C()                          Stores in E_DBS the values of the coefficients in E_C.
 *      void E_get_SMO()                        Saves in E_SMO the relaxation parameters of each coefficient of the equation block. 
 *      void E_free_work()                      Frees all allocated variables for the last estimation.
 */
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/scalars.h"
#include "api/estimation/estimation.h"


/**
 *  Creates MAT struct's needed for the estimation of a block of equations.
 *  
 *  @return     int     0 or -1
 */
int Estimation::E_prep_alloc()
{
    E_U             = M_alloc(E_NEQ, E_T);          // Residuals (neq x t)
    E_VCU           = M_diag(E_NEQ, 1.0);           // Variance / covariance of the residuals (neq x neq)    
    E_IVCU          = M_diag(E_NEQ, 1.0);           // Inverse of E_VCU
    E_RHS           = M_alloc(E_NEQ, E_T);          // Right side of equations (neq x t)
    E_MCU           = M_alloc(E_NEQ, E_NEQ);        //   
    E_G             = M_alloc(E_NCE, E_T * E_NEQ);  // Jacobian matrix of the system
    E_VCC           = M_alloc(E_NCE, E_NCE);        // Var/covar of the coefficients
    E_GMU           = M_alloc(E_NCE, 1);            // 
    E_dC            = M_alloc(E_NCE, 1);            // Vector of coefficient increments
    E_M             = M_alloc(E_T, E_NCE);          // 
    E_MTMP          = M_alloc(E_T, E_NCE);          //
    E_MTMPP         = M_alloc(E_NCE, E_T);          //
    E_VCCTMP        = M_alloc(E_NCE, E_NCE);        //
    E_UM            = M_alloc(E_T, 1);              //
    E_UMT           = M_alloc(1, E_T);              //
    E_UMTMP         = M_alloc(E_T, 1);              //
    E_UVCCTMP       = M_alloc(E_NCE, E_T);          //
    E_GMUTMP        = M_alloc(E_NCE, 1);            //
        
    E_DF            = M_alloc(1, E_NCE);            // Degrees of freedom of each coefficient
    E_STDERR        = M_alloc(1, E_NEQ);            // Std error of each equation 
    E_SSRES         = M_alloc(1, E_NEQ);            // Sum of squares of residuals of each eq
    E_MEAN_Y        = M_alloc(1, E_NEQ);            // Mean of the LHS on each equation 
    E_STDEV         = M_alloc(1, E_NEQ);            // Std deviation of each equation 
    E_RSQUARE       = M_alloc(1, E_NEQ);            // R-square of each equation
    E_RSQUARE_ADJ   = M_alloc(1, E_NEQ);            // Adjusted R-square of each equation
    E_DW            = M_alloc(1, E_NEQ);            // Durbin-Watson test of each equation
    E_FSTAT         = M_alloc(1, E_NEQ);            // F-Stat of each equation
    E_LOGLIK        = M_alloc(1, E_NEQ);            // Log-likelihood of each equation
    E_STD_PCT       = M_alloc(1, E_NEQ);            // Standard errors in % for each equation
    E_MCORR         = M_alloc(E_NCE, E_NCE);        // Correlation matrix bw coefficients
    E_MCORRU        = M_alloc(E_NEQ, E_NEQ);        // Correlation matrix bw error terms of equations
    E_DEV           = M_alloc(E_NEQ, E_T);          // Deviation between observed and calculated values

    if(M_errno)
    {
        error_manager.append_error("Estimation : Memory Error");
        return(-1);
    } 
    return(0);
}


/**
 *  Analyses the LEC equations and set various variables for the estimation process:  
 *      - E_NEQ
 *      - E_LHS 
 *      - E_CRHS
 *      - ...
 *   
 *  Compiles the left members of each equations and link them with E_DBV and E_DBS.
 *  Computes the left members on [E_FROM, E_FROM+E_T] and saves the result in the array E_LHS.
 *  
 *  Compiles the right members of each eq and saves the resulting CLEC* in E_CRHS.
 *  Adds all coefficients in E_DBS if needed.
 *  Compiles and links the right members with E_DBV and E_DBS.
 *  
 *  @param [in] char**  lecs    vector of LEC equations
 *  @return     int             0 or -1           
 *  
 */
int Estimation::E_prep_lecs(char** lecs)
{
    int     i, pos, t;
    CLEC    *clec = 0;
    double  x;

    E_NEQ = SCR_tbl_size((unsigned char**) lecs);
    if(E_NEQ < 1)
    {
        error_manager.append_error("Estimation: No equation");
        return(-1);
    }

    E_LHS = M_alloc(E_NEQ, E_T);
    if(E_LHS == 0)
    {
        error_manager.append_error("Estimation: Memory Error");
        return(-1);
    }

    E_CRHS = (CLEC **) SW_nalloc(E_NEQ * sizeof(CLEC *));
    if(E_CRHS == 0)
    {
        error_manager.append_error("Estimation: Memory Error");
        return(-1);
    }

    for(i = 0 ; i < E_NEQ ; i++) {
        pos = L_split_eq(lecs[i]);
        if(pos < 0)
        {
            error_manager.append_error("Estimation: Syntax Error");
            return(-1);
        } 

        lecs[i][pos] = 0;
        clec = L_cc(lecs[i]);
        if(clec == 0)
        {
            error_manager.append_error("Estimation: Syntax Error");
            return(-1);
        }

        if(E_add_scls(clec, E_DBS))
        {
            error_manager.append_error("Estimation: Link Error");
            return(-1); // JMP 13/11/2012
        }

        if(L_link(E_DBV, E_DBS, clec) != 0) 
        {
            SW_nfree(clec); // GB 14/11/2012
            error_manager.append_error("Estimation: Link Error");
            return(-1);
        }

        for(t = 0 ; t < E_T ; t++) {
            x = L_exec(E_DBV, E_DBS, clec, t + E_FROM);
            if(!IODE_IS_A_NUMBER(x)) 
            {
                SW_nfree(clec); // GB 14/11/2012
                error_manager.append_error("Estimation: NaN Generated");
                return(-1);
            }
            MATE(E_LHS, i, t) = x;
        }
        SW_nfree(clec); // GB 14/11/2012
        lecs[i][pos] = ':';

        E_CRHS[i] = L_cc(lecs[i] + pos + 2);
        if(E_CRHS[i] == 0)
        {
            error_manager.append_error("Estimation: Syntax Error");
            return(-1);
        } 

        if(E_add_scls(E_CRHS[i], E_DBS))
        {
            error_manager.append_error("Estimation: Link Error");
            return(-1); // JMP 13/11/2012
        } 

        if(L_link(E_DBV, E_DBS, E_CRHS[i]) != 0)
        {
            error_manager.append_error("Estimation: Link Error");
            return(-1);
        }
    }

    return(0);
}


/**
 *  Adds to dbs (KDB of scalars) the coefficients found in clec (if they don't exist).
 *  
 *  @param [in] CLEC*   clec    Compiled LEC equation
 *  @param [in] KDB*    dbs     KDB of scalar
 *  @return     int             0 (TODO: check the add() return value)        
 *  
 */
int Estimation::E_add_scls(CLEC *clec, KDB *dbs)
{
    char* c_name;
    std::string name;
    for(int j = 0 ; j < clec->nb_names ; j++) 
    {
        c_name = clec->lnames[j].name;
        name = std::string(c_name);
        if(is_coefficient(name) && !dbs->contains(name))
            dbs->set(name, NULL);
    }
    
    return(0);
}


/**
 *  Computes the matrix E_D (E_T x E_T) of instruments.
 *  Each instrument is a LEC formula that is first compiled and linked. 
 *  It is then computed on [E_FROM, E_FROM+E_T] and saved in the array E_D.
 *  
 *  @param [in] char**  instrs 
 *  @return     int     0 or -1        
 *  @global     MAT*    E_D (E_T, E_T)
 */
int Estimation::E_prep_instrs(char** instrs)
{
    int     i, t;
    CLEC    *clec = 0;
    MAT     *minstr = 0, *mip = 0, *miip = 0, *miipi = 0, *mipiipi = 0;
    double  x;

    // Check if there are instruments. If not, return 0
    if(E_MET != 2 && E_MET != 3) 
        return(0);
    E_NINSTR = SCR_tbl_size((unsigned char**) instrs);
    if(E_NINSTR < 1) 
        return(0);

    // Alloc local MAT
    minstr  = M_alloc(E_T, E_NINSTR + 1);
    mip     = M_alloc(E_NINSTR + 1, E_T);
    miip    = M_alloc(E_NINSTR + 1, E_NINSTR + 1);
    miipi   = M_alloc(E_NINSTR + 1, E_NINSTR + 1);
    mipiipi = M_alloc(E_T, E_NINSTR + 1);
    E_D     = M_alloc(E_T, E_T);

    // Check allocation succeeded
    if(minstr == 0 || mip == 0 || miip == 0 ||
            miipi == 0 || mipiipi == 0 || E_D == 0) {
        error_manager.append_error("Estimation: Memory Error");
        goto fin;
    }
    for(i = 0 ; i < E_T ; i++) MATE(minstr, i, 0) = 1.0;
    for(i = 0 ; i < E_NINSTR ; i++) {
        clec = L_cc(instrs[i]);
        if(clec == 0) {
            error_manager.append_error("Estimation: Syntax Error");
            goto fin;
        }
        if(L_link(E_DBV, E_DBS, clec) != 0) {
            SW_nfree(clec);
            error_manager.append_error("Estimation: Link Error");
            goto fin;
        }
        for(t = 0 ; t < E_T ; t++) {
            x = L_exec(E_DBV, E_DBS, clec, t + E_FROM);
            if(!IODE_IS_A_NUMBER(x)) {
                SW_nfree(clec);
                error_manager.append_error("Estimation: NaN Generated");
                goto fin;
            }
            MATE(minstr, t, i + 1) = x;
        }
        SW_nfree(clec);
    }

    M_xprimx(miip, minstr);
    M_inv_1(miipi, miip);
    if(M_errno) 
    {
        error_manager.append_error("Estimation : Dreg Error");
        goto fin;
    }
    M_trans(mip, minstr);
    M_prod(mipiipi, minstr, miipi);
    M_prod(E_D, mipiipi, mip);
    return(0);

fin:
    M_free(minstr);
    M_free(mip);
    M_free(miip);
    M_free(miipi);
    M_free(mipiipi);
    return(-1);
}


/**
 *  Analyses the block of equations to determine and assign 
 *  the global variables described below.
 *  
 *  The block of equations must have been compiled/linked before and their CLEC 
 *  forms were normally saved in E_CRHS.
 *  
 *  @global     int  E_NC       Nb of coefficients (total)
 *  @global     int  E_NCE      Nb of estimated coefficients (total)
 *  @global     MAT* E_NBCE     Nb of estimated coefficients per equation (MAT(1,E_NEQ))
 *  @global     int* E_C_NBS    position in E_DBS of the estimated coefs
 *  @global     int  E_DBS      global KDB of scalars
 *  @global     MAT* E_C        MAT 1 col of estimated coefficients
 *  @global     MAT* E_SMO      MAT 1 col of relaxation params
 *  @return     int             0 on success, -1 on error
 *  
 */
int Estimation::E_prep_coefs()
{
    CLEC    *clec;
    int     i, j, pos, k;

    E_NC = E_NCE = 0;
    
    // Compute the nb of coefficients E_NC
    for(i = 0 ; i < E_NEQ ; i++) 
        E_NC += E_CRHS[i]->nb_names;
    
    // Creates the vector E_C_NBS of positions of the coefficients in E_DBS
    E_C_NBS = (int *) SW_nalloc(E_NC * sizeof(int));
    if(E_C_NBS == 0) 
    {
        error_manager.append_error("Estimation : Memory Error");
        return(-1);
    }
    
    // Loop on equations and names in each equations (linked before with E_BDS)
    E_NC = 0;
    E_NBCE = M_alloc(1, E_NEQ);
    std::string scl_name;
    for(i = 0 ; i < E_NEQ ; i++) 
    {
        clec = E_CRHS[i];                                   // linked before
        for(j = 0 ; j < clec->nb_names ; j++) 
        {
            if(is_coefficient(clec->lnames[j].name)) 
            {
                pos = clec->lnames[j].pos;
                for(k = 0 ; k < E_NC ; k++)
                    if(E_C_NBS[k] == pos) 
                        break;                              // Coef already found in E_C_NBS
                scl_name = E_DBS->get_name(pos);
                if(k == E_NC) 
                {
                    E_C_NBS[E_NC++] = pos;                  // Add a coefficient in E_C_NBS
                    if(KSVAL(E_DBS, scl_name)->relax > 0) 
                        E_NCE++;
                }
                if(KSVAL(E_DBS, scl_name)->relax > 0) 
                    MATE(E_NBCE, 0, i)++;                   // relax > 0 => estimation coef
            }
        }
    }

    if(E_NCE == 0) 
    {
        std::string error_msg = "No scalars to estimate in your block of equations";
        error_manager.append_error(error_msg);
        error_manager.append_error("Estimation: No current estimation");
        return(-1);
    }

    E_C   = M_alloc(E_NC, 1);
    E_SMO = M_alloc(E_NC, 1);
    E_get_SMO();
    E_get_C();
    return(0);
}


/**
 *  Saves in E_C the values of the estimated coefficients. 
 *  These values are retrieved from E_DBS.
 *  
 *  If the absolute value of an estimated coefficient is less than 1e-15, 
 *  it is replaced by 0.1 in E_DBS to avoid precision and convergence problems.
 *    
 *  @global     MAT*    E_C (E_NC x 1)   Array of estimated coefficient values
 *  @global     int*    E_C_NBS             position in E_DBS of the estimated coefs
 *  @global     KDB*    E_DBS               KDB of scalars for the estimation
 */
void Estimation::E_get_C()
{
    double c;
    std::string scl_name;
    for(int i = 0 ; i < E_NC ; i++) 
    {
        scl_name = E_DBS->get_name(E_C_NBS[i]);
        c = KSVAL(E_DBS, scl_name)->value;
        if(KSVAL(E_DBS, scl_name)->relax != 0.0 && fabs(c) < 1e-15) 
        {
            c = 0.1;
            KSVAL(E_DBS, scl_name)->value = c; // GB 24/01/2013
        }
        MATE(E_C, i, 0) = c;
    }
}


/**
 *  Copies the values in E_C to the KDB E_DBS.
 *  
 *  @global     MAT*    E_C       Array of estimated coefficient values: MAT(E_NC, 1)
 *  @global     int*    E_C_NBS   position in E_DBS of the estimated coefs
 *  @global     KDB*    E_DBS     KDB of scalars for the estimation
 */
void Estimation::E_put_C()
{
    std::string name;
    for(int i = 0 ; i < E_NC ; i++)
    {
        name = E_DBS->get_name(E_C_NBS[i]);
        KSVAL(E_DBS, name)->value = MATE(E_C, i, 0);
    }
}


/**
 *  Saves in E_SMO (NC x 1) the relaxation parameters of each coefficient of the equation block. 
 *  These values are searched in E_DBS.
 *  
 *  @global     MAT*    E_SMO          Array of relaxation parameters (E_NC x 1)
 *  @global     int*    E_C_NBS        position in E_DBS of the estimated coefs
 *  @global     KDB*    E_DBS          KDB of scalars for the estimation
 */
void Estimation::E_get_SMO()
{
    std::string scl_name;
    for(int i = 0 ; i < E_NC ; i++)
    {
        scl_name = E_DBS->get_name(E_C_NBS[i]);
        MATE(E_SMO, i, 0) = KSVAL(E_DBS, scl_name)->relax;
    }
}


/**
 *  Resets all global variables.
 */
void Estimation::E_prep_reset()
{
    E_CRHS = 0;
    E_C_NBS = 0;

    E_NINSTR = 0;

    E_RHS = 0;
    E_LHS = 0;
    E_U = 0;
    E_G = 0;
    E_VCC = 0;
    E_VCCTMP = 0;
    E_M = 0;
    E_MTMP = 0;
    E_MTMPP = 0;
    E_D = 0;
    E_C = 0;
    E_SMO = 0;
    E_NBCE = 0;
    E_VCU = 0;
    E_IVCU = 0;
    E_MCU = 0;
    E_GMU = 0;
    E_dC = 0;
    E_DF = 0;
    E_SSRES = 0;
    E_RSQUARE_ADJ = 0;
    E_DW = 0;
    E_LOGLIK = 0;
    E_STDERR = 0;
    E_MEAN_Y = 0;
    E_STDEV = 0;
    E_RSQUARE = 0;
    E_FSTAT = 0;
    E_STD_PCT = 0;
    E_MCORR = 0;
    E_MCORRU = 0;
    E_DEV = 0;
    E_UM = 0;
    E_UMT = 0;
    E_UMTMP = 0;
    E_UVCCTMP = 0;
    E_GMUTMP = 0;
}

/**
 *  Frees all allocated variables for the last estimation.
 */
void Estimation::E_free_work()
{
    int     i;

    if(E_CRHS) {
        for(i = 0 ; i < E_NEQ ; i++) SW_nfree(E_CRHS[i]);
        SW_nfree(E_CRHS);
    }
    SW_nfree(E_C_NBS);

    M_free(E_RHS);
    M_free(E_LHS);
    M_free(E_U);
    M_free(E_G);
    M_free(E_VCC);
    M_free(E_VCCTMP);
    M_free(E_M);
    M_free(E_MTMP);
    M_free(E_MTMPP);
    M_free(E_D);
    M_free(E_C);
    M_free(E_SMO);
    M_free(E_NBCE);
    M_free(E_VCU);
    M_free(E_IVCU);
    M_free(E_MCU);
    M_free(E_GMU);
    M_free(E_dC);
    M_free(E_DF);
    M_free(E_RSQUARE_ADJ);
    M_free(E_DW);
    M_free(E_LOGLIK);
    M_free(E_SSRES);
    M_free(E_STDERR);
    M_free(E_MEAN_Y);
    M_free(E_STDEV);
    M_free(E_RSQUARE);
    M_free(E_FSTAT);
    M_free(E_STD_PCT);
    M_free(E_MCORR);
    M_free(E_MCORRU);
    M_free(E_DEV);
    M_free(E_UM);
    M_free(E_UMT);
    M_free(E_UMTMP);
    M_free(E_UVCCTMP);
    M_free(E_GMUTMP);

    SCR_free_tbl((unsigned char**) E_ENDOS);
    SCR_free_tbl((unsigned char**) E_LECS);
    SCR_free_tbl((unsigned char**) E_INSTRS);
    E_ENDOS = 0;
    E_LECS = 0;
    E_INSTRS = 0;

    E_prep_reset();
}


/**
 *  Prepares the estimation of a group a equations.
 *      - compiles and links the equation (lecs)
 *      - computes the LHS (left members of the equations) 
 *      - compiles and links the instruments (instrs)
 *      - analyses the linked equations and determine the estimated coefficients. If needed, creates them.
 *      - allocates all global variables needed for the estimation.
 *  
 *  @param [in] char**  lecs    vector of LEC equations
 *  @param [in] char**  instrs  vector of instruments (LEC expressions)
 *  @return     int             0 on success, -1 on error
 *  
 */
int Estimation::E_prep(char** lecs, char** instrs)
{
    E_prep_reset();
    if(E_prep_lecs(lecs)) 
        return(-1);
    if(E_prep_instrs(instrs)) 
        return(-1);
    if(E_prep_coefs()) 
        return(-1);
    if(E_prep_alloc()) 
        return(-1);
    return(0);
}

#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/scalars.h"
#include "api/estimation/estimation.h"

#include <unordered_set>


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
        return -1;
    } 
    return 0;
}


/**
 *  Analyses the LEC equations and set various variables for the estimation process:  
 *      - E_NEQ
 *      - E_LHS 
 *      - v_block_rhs
 *      - ...
 *   
 *  Compiles the left members of each equations and link them with E_DBV and E_DBS.
 *  Computes the left members on [E_FROM, E_FROM+E_T] and saves the result in the array E_LHS.
 *  
 *  Compiles the right members of each eq and saves the resulting CLEC* in v_block_rhs.
 *  Adds all coefficients in E_DBS if needed.
 *  Compiles and links the right members with E_DBV and E_DBS.
 *  
 *  @param [in] char**  lecs    vector of LEC equations
 *  @return     int             0 or -1           
 *  
 */
int Estimation::E_prep_lecs()
{
    E_NEQ = (int) v_block_lecs.size();
    if(E_NEQ < 1)
    {
        error_manager.append_error("Estimation: No equation");
        return -1;
    }

    E_LHS = M_alloc(E_NEQ, E_T);
    if(!E_LHS)
    {
        error_manager.append_error("Estimation: Memory Error");
        return -1;
    }

    v_block_rhs.clear();
    v_block_rhs.resize(E_NEQ, nullptr);

    double x;
    int i = 0;
    size_t pos = 0;
    std::shared_ptr<CLEC> clec = nullptr;
    std::string left_hand_side, right_hand_side;
    for(const std::string& lec : v_block_lecs) 
    {
        // split equation into left and right hand side
        pos = lec.find(":=");

        // test if := not found -> return
        if(pos == std::string::npos) 
        {
            error_manager.append_error("Estimation: Syntax Error");
            return -1;
        } 

        left_hand_side = lec.substr(0, pos);
        try
        {
            clec = std::make_shared<CLEC>(left_hand_side);
        }
        catch(const std::exception&)
        {
            error_manager.append_error("Estimation: Syntax Error");
            return -1;
        }

        if(E_add_scls(clec, *E_DBS))
        {
            error_manager.append_error("Estimation: Link Error");
            return -1; // JMP 13/11/2012
        }

        if(clec->link(E_DBV, E_DBS) != 0) 
        {
            error_manager.append_error("Estimation: Link Error");
            return -1;
        }

        for(int t = 0 ; t < E_T ; t++) 
        {
            x = clec->execute(E_DBV, E_DBS, t + E_FROM);
            if(!IODE_IS_A_NUMBER(x)) 
            {
                error_manager.append_error("Estimation: NaN Generated");
                return -1;
            }
            MATE(E_LHS, i, t) = x;
        }
        
        right_hand_side = lec.substr(pos+2); 
        try
        {
            clec = std::make_shared<CLEC>(right_hand_side);
        }
        catch(const std::exception&)
        {
            error_manager.append_error("Estimation: Syntax Error");
            return -1;
        } 

        if(E_add_scls(clec, *E_DBS))
        {
            error_manager.append_error("Estimation: Link Error");
            return -1;
        } 

        if(clec->link(E_DBV, E_DBS) != 0)
        {
            error_manager.append_error("Estimation: Link Error");
            return -1;
        }

        v_block_rhs[i] = clec;
        i++;
    }

    return 0;
}


/**
 *  Adds to dbs (KDB of scalars) the coefficients found in clec (if they don't exist).
 *  
 *  @param [in] CLEC*   clec    Compiled LEC equation
 *  @param [in] KDB*    dbs     KDB of scalar
 *  @return     int             0        
 *  
 */
int Estimation::E_add_scls(const std::shared_ptr<CLEC> clec, KDBScalars& dbs)
{
    std::string name;
    Scalar scl(0.9, 1.0);
    for(auto& [name, _]: clec->map_objs) 
    {
        if(is_coefficient(name) && !dbs.contains(name))
            dbs.add(name, scl);
    }
    
    return 0;
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
int Estimation::E_prep_instrs()
{
    std::shared_ptr<CLEC> clec = nullptr;

    // Check if there are instruments. If not, return 0
    if(E_MET != 2 && E_MET != 3) 
        return 0;

    E_NINSTR = (int) v_block_instrs.size();
    if(E_NINSTR < 1) 
        return 0;

    // Alloc local MAT
    MAT* minstr  = M_alloc(E_T, E_NINSTR + 1);
    MAT* mip     = M_alloc(E_NINSTR + 1, E_T);
    MAT* miip    = M_alloc(E_NINSTR + 1, E_NINSTR + 1);
    MAT* miipi   = M_alloc(E_NINSTR + 1, E_NINSTR + 1);
    MAT* mipiipi = M_alloc(E_T, E_NINSTR + 1);
    E_D = M_alloc(E_T, E_T);

    // Check allocation succeeded
    if(!minstr || !mip || !miip || !miipi || !mipiipi || !E_D) 
    {
        error_manager.append_error("Estimation: Memory Error");
        goto fin;
    }

    for(int i = 0 ; i < E_T ; i++) 
        MATE(minstr, i, 0) = 1.0;
   
    double x;
    for(const std::string& instr : v_block_instrs) 
    {
        try
        {
            clec = std::make_shared<CLEC>(instr);
        }
        catch(const std::exception&) 
        {
            error_manager.append_error("Estimation: Syntax Error");
            goto fin;
        }

        if(clec->link(E_DBV, E_DBS) != 0) 
        {
            error_manager.append_error("Estimation: Link Error");
            goto fin;
        }

        for(int t = 0 ; t < E_T ; t++) 
        {
            x = clec->execute(E_DBV, E_DBS, t + E_FROM);
        }
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
    return 0;

fin:
    M_free(minstr);
    M_free(mip);
    M_free(miip);
    M_free(miipi);
    M_free(mipiipi);
    return -1;
}


/**
 *  Analyses the block of equations to determine and assign 
 *  the global variables described below.
 *  
 *  The block of equations must have been compiled/linked before and their CLEC 
 *  forms were normally saved in v_block_rhs.
 *  
 *  @global     int  E_NC       Nb of coefficients (total)
 *  @global     int  E_NCE      Nb of estimated coefficients (total)
 *  @global     MAT* E_NBCE     Nb of estimated coefficients per equation (MAT(1,E_NEQ))
 *  @global     std::vector<int> v_coef_names    position in E_DBS of the estimated coefs
 *  @global     int  E_DBS      global KDB of scalars
 *  @global     MAT* E_C        MAT 1 col of estimated coefficients
 *  @global     MAT* E_SMO      MAT 1 col of relaxation params
 *  @return     int             0 on success, -1 on error
 *  
 */
int Estimation::E_prep_coefs()
{
    v_coef_names.clear();
    std::unordered_set<std::string> coef_set;
    
    // Loop on equations and names in each equations (linked before with E_BDS)
    E_NCE = 0;
    E_NBCE = M_alloc(1, E_NEQ);

    std::shared_ptr<CLEC> clec = nullptr; 
    for(int i = 0 ; i < E_NEQ ; i++) 
    {
        clec = v_block_rhs[i];
        for(auto& [name, _]: clec->map_objs) 
        {
            if(is_coefficient(name)) 
            {
                if(!E_DBS->contains(name))
                    continue;
                
                // Coef already found in v_coef_names
                if(coef_set.contains(name))
                    continue;
                
                // Add a coefficient in v_coef_names
                v_coef_names.push_back(name);
                coef_set.insert(name);
                
                // relax > 0 => estimation coef
                if(E_DBS->get_obj_ptr(name)->relax > 0)
                {
                    E_NCE++;
                    MATE(E_NBCE, 0, i)++;
                } 
            }
        }
    }

    if(E_NCE == 0) 
    {
        std::string error_msg = "No scalars to estimate in your block of equations";
        error_manager.append_error(error_msg);
        error_manager.append_error("Estimation: No current estimation");
        return -1;
    }

    int nb_coefs = (int) v_coef_names.size();
    E_C = M_alloc(nb_coefs, 1);
    E_SMO = M_alloc(nb_coefs, 1);
    E_get_SMO();
    E_get_C();
    return 0;
}


/**
 *  Saves in E_C the values of the estimated coefficients. 
 *  These values are retrieved from E_DBS.
 *  
 *  If the absolute value of an estimated coefficient is less than 1e-15, 
 *  it is replaced by 0.1 in E_DBS to avoid precision and convergence problems.
 *    
 *  @global     MAT*    E_C (E_NC x 1)   Array of estimated coefficient values
 *  @global     std::vector<int> v_coef_names    position in E_DBS of the estimated coefs
 *  @global     KDB*    E_DBS               KDB of scalars for the estimation
 */
void Estimation::E_get_C()
{
    double c;
    int i = 0;
    for(const std::string& scl_name : v_coef_names) 
    {
        c = E_DBS->get_obj_ptr(scl_name)->value;
        if(E_DBS->get_obj_ptr(scl_name)->relax != 0.0 && fabs(c) < 1e-15) 
        {
            c = 0.1;
            E_DBS->get_obj_ptr(scl_name)->value = c;
        }
        MATE(E_C, i, 0) = c;
        i++;
    }
}


/**
 *  Copies the values in E_C to the KDB E_DBS.
 *  
 *  @global     MAT*    E_C       Array of estimated coefficient values: MAT(E_NC, 1)
 *  @global     std::vector<int> v_coef_names   position in E_DBS of the estimated coefs
 *  @global     KDB*    E_DBS     KDB of scalars for the estimation
 */
void Estimation::E_put_C()
{
    int i = 0;
    for(const std::string& scl_name : v_coef_names) 
    {
        E_DBS->get_obj_ptr(scl_name)->value = MATE(E_C, i, 0);
        i++;
    }
}


/**
 *  Saves in E_SMO (NC x 1) the relaxation parameters of each coefficient of the equation block. 
 *  These values are searched in E_DBS.
 *  
 *  @global     MAT*    E_SMO          Array of relaxation parameters (E_NC x 1)
 *  @global     std::vector<int> v_coef_names        position in E_DBS of the estimated coefs
 *  @global     KDB*    E_DBS          KDB of scalars for the estimation
 */
void Estimation::E_get_SMO()
{
    int i = 0;
    for(const std::string& scl_name : v_coef_names) 
    {
        MATE(E_SMO, i, 0) = E_DBS->get_obj_ptr(scl_name)->relax;
        i++;
    }
}


/**
 *  Resets all global variables.
 */
void Estimation::E_prep_reset()
{
    v_block_rhs.clear();
    v_coef_names.clear();

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
int Estimation::E_prep()
{
    E_prep_reset();
    if(E_prep_lecs()) 
        return -1;
    if(E_prep_instrs()) 
        return -1;
    if(E_prep_coefs()) 
        return -1;
    if(E_prep_alloc()) 
        return -1;
    return 0;
}

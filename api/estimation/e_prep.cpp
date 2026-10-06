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
int Estimation::E_prep_matrices()
{
    U.setZero(E_NEQ, E_T);                        // Residuals (neq x t)
    VCU.setIdentity(E_NEQ, E_NEQ);                // Variance / covariance of the residuals (neq x neq)    
    IVCU.setIdentity(E_NEQ, E_NEQ);               // Inverse of VCU
    RHS.setZero(E_NEQ, E_T);                      // Right side of equations (neq x t)
    MCU.setZero(E_NEQ, E_NEQ);
    G.setZero(E_NCE, E_T * E_NEQ);                // Jacobian matrix of the system
    VCC.setZero(E_NCE, E_NCE);                    // Var/covar of the coefficients
    GMU.setZero(E_NCE);
    DELTA_COEFS.setZero(E_NCE);                   // Vector of coefficient increments
    DEG_FREEDOM.setZero(E_NCE);                            // Degrees of freedom of each coefficient
    
    STDERR.setZero(E_NEQ);                    // Std error of each equation 
    SSRES.setZero(E_NEQ);                     // Sum of squares of residuals of each eq
    MEAN_Y.setZero(E_NEQ);                    // Mean of the LHS on each equation 
    STDEV.setZero(E_NEQ);                     // Std deviation of each equation 
    RSQUARE.setZero(E_NEQ);                   // R-square of each equation
    RSQUARE_ADJ.setZero(E_NEQ);               // Adjusted R-square of each equation
    DW.setZero(E_NEQ);                        // Durbin-Watson test of each equation
    FSTAT.setZero(E_NEQ);                     // F-Stat of each equation
    LOGLIK.setZero(E_NEQ);                    // Log-likelihood of each equation
    STD_PCT.setZero(E_NEQ);                   // Standard errors in % for each equation

    MCORR.setZero(E_NCE, E_NCE);              // Correlation matrix bw coefficients
    MCORRU.setZero(E_NEQ, E_NEQ);             // Correlation matrix bw error terms of equations
    DEV.setZero(E_NEQ, E_T);                  // Deviation between observed and calculated values

    return 0;
}


/**
 *  Analyses the LEC equations and set various variables for the estimation process:  
 *      - E_NEQ
 *      - LHS 
 *      - v_block_rhs
 *      - ...
 *   
 *  Compiles the left members of each equations and link them with E_DBV and E_DBS.
 *  Computes the left members on [E_FROM, E_FROM+E_T] and saves the result in the array LHS.
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

    LHS.resize(E_NEQ, E_T);
    LHS.setZero();

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
            LHS(i, t) = x;
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
    for(const std::string& name: clec->v_obj_names) 
    {
        if(is_coefficient(name) && !dbs.contains(name))
            dbs.add(name, scl);
    }
    
    return 0;
}


/**
 *  Computes the matrix D (E_T x E_T) of instruments.
 *  Each instrument is a LEC formula that is first compiled and linked. 
 *  It is then computed on [E_FROM, E_FROM+E_T] and saved in the array E_D.
 *  
 *  @param [in] char**  instrs 
 *  @return     int     0 or -1        
 *  @global     MAT*    D (E_T, E_T)
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
    D.setZero(E_T, E_T);
    
    Eigen::MatrixXd m = Eigen::MatrixXd::Zero(E_T, E_NINSTR + 1);
    for(int i = 0; i < E_T; i++) 
        m(i, 0) = 1.0;
   
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
            return -1;
        }

        if(clec->link(E_DBV, E_DBS) != 0) 
        {
            error_manager.append_error("Estimation: Link Error");
            return -1;
        }

        for(int t = 0 ; t < E_T ; t++) 
        {
            x = clec->execute(E_DBV, E_DBS, t + E_FROM);
        }
    }

    // (m^T m)
    Eigen::MatrixXd mT = m.transpose();
    Eigen::MatrixXd mTm = mT * m;
    
    // (m^T m)^{-1}
    Eigen::FullPivLU<Eigen::MatrixXd> lu(mTm);
    if(!lu.isInvertible())
    {
        error_manager.append_error("Estimation : Cannot compute the matrix of instruments");
        return -1;
    }
    Eigen::MatrixXd mTm_inv = lu.inverse();
    
    // m (m^T m)^{-1}
    Eigen::MatrixXd m_mTm_inv = m * mTm_inv;

    // D = m (m^T m)^{-1} m^T
    D = m_mTm_inv * mT;
    return 0;
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
 *  @global     MAT* NB_COEFS_EQ     Nb of estimated coefficients per equation
 *  @global     std::vector<int> v_coef_names    position in E_DBS of the estimated coefs
 *  @global     int  E_DBS      global KDB of scalars
 *  @global     MAT* COEFS    MAT 1 col of estimated coefficients
 *  @global     MAT* SMO      MAT 1 col of relaxation params
 *  @return     int             0 on success, -1 on error
 *  
 */
int Estimation::E_prep_coefs()
{
    v_coef_names.clear();
    std::unordered_set<std::string> coef_set;
    
    // Loop on equations and names in each equations (linked before with E_BDS)
    E_NCE = 0;
    NB_COEFS_EQ.setZero(E_NEQ);

    std::shared_ptr<CLEC> clec = nullptr; 
    for(int i = 0 ; i < E_NEQ ; i++) 
    {
        clec = v_block_rhs[i];
        for(const std::string& name: clec->v_obj_names) 
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
                    NB_COEFS_EQ(i)++;
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
    COEFS.setZero(nb_coefs);
    SMO.setZero(nb_coefs);
    E_get_SMO();
    E_get_C();
    return 0;
}


/**
 *  Saves in COEFS the values of the estimated coefficients. 
 *  These values are retrieved from E_DBS.
 *  
 *  If the absolute value of an estimated coefficient is less than 1e-15, 
 *  it is replaced by 0.1 in E_DBS to avoid precision and convergence problems.
 *    
 *  @global     MAT*    COEFS             Array of estimated coefficient values
 *  @global     vector<int> v_coef_names    position in E_DBS of the estimated coefs
 *  @global     KDB    E_DBS                KDB of scalars for the estimation
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
        COEFS(i) = c;
        i++;
    }
}


/**
 *  Copies the values in COEFS to the KDB E_DBS.
 *  
 *  @global     MAT*    COEFS             Array of estimated coefficient values
 *  @global     vector<int> v_coef_names    position in E_DBS of the estimated coefs
 *  @global     KDB*    E_DBS               KDB of scalars for the estimation
 */
void Estimation::E_put_C()
{
    int i = 0;
    for(const std::string& scl_name : v_coef_names) 
    {
        E_DBS->get_obj_ptr(scl_name)->value = COEFS(i);
        i++;
    }
}


/**
 *  Saves in SMO (size NC) the relaxation parameters of each coefficient of the equation block. 
 *  These values are searched in E_DBS.
 *  
 *  @global     MAT*    SMO               Vector of relaxation parameters
 *  @global     vector<int> v_coef_names    position in E_DBS of the estimated coefs
 *  @global     KDB*    E_DBS               KDB of scalars for the estimation
 */
void Estimation::E_get_SMO()
{
    int i = 0;
    for(const std::string& scl_name : v_coef_names) 
    {
        SMO(i) = E_DBS->get_obj_ptr(scl_name)->relax;
        i++;
    }
}

/**
 *  Frees all allocated variables for the last estimation.
 */
void Estimation::clear()
{
    E_NINSTR = 0;
    v_block_rhs.clear();
    v_coef_names.clear();

    RHS.resize(0, 0);
    LHS.resize(0, 0);
    U.resize(0, 0);
    VCU.resize(0, 0);
    IVCU.resize(0, 0);
    D.resize(0, 0);
    G.resize(0, 0);
    VCC.resize(0, 0);
    MCU.resize(0, 0);
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
    clear();
    if(E_prep_lecs()) 
        return -1;
    if(E_prep_instrs()) 
        return -1;
    if(E_prep_coefs()) 
        return -1;
    if(E_prep_matrices()) 
        return -1;
    return 0;
}

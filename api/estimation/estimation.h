#pragma once

#include "scr4/s_mat.h"

#include "api/constants.h"
#include "api/b_errors.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/equations.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/lec/lec.h"

#ifndef SKBUILD
    #include "gtest/gtest.h"
#endif

/* ---------------------- DEFINE ---------------------- */

#define DEFAULT_MAXIT   100
#define DEFAULT_EPS     1.0e-6

/* ---------------------- ENUMS ---------------------- */

enum IodeEstimationError
{
    EST_NO_EQ_ERR = 1,
    EST_MEM_ERR,
    EST_SYNTAX_ERR,
    EST_LINK_ERR,
    EST_DREG_ERR,
    EST_NAN_ERR,
    EST_VCC_SING_ERR,
    EST_VCU_SING_ERR,
    EST_GMG_SING_ERR,
    EST_NO_SCALARS
};

enum IodeAdjustmentMethod
{
    AM_PARTIAL_ADJUSTMENT,
    AM_ERROR_CORRECTION_METHOD
};

/* ---------------------- CLASSES ---------------------- */

class Estimation
{
#ifndef SKBUILD
    FRIEND_TEST(IodeCAPITest, Tests_IODEMSG);
    FRIEND_TEST(EstimationTest, Estimate);
#endif

    char**    est_endos;      // list of equation names (endos = eqs names)
    Sample    est_smpl;       // sample used for the estimation
    int       est_method;     // estimation method

protected:
    int       E_IT;           // Number of iterations of the last estimation
    int       E_CONV;         // Boolean. Indicates if the estimation iteration has converged
    int       E_NEQ;          // Number of equations in the current block of equations
    int       E_NCE;          // Number of estimated coefficients in the current block of equations
    int       E_NC;           // Number of coefficients (est and non est) in the current block of equations
    int       E_NINSTR;       // Number of instruments in the estimation
    int       E_T;            // Number of periods in the estimation sample
    int       E_FROM;         // Position in E_DBV of the first period in the estimation sample
    int       E_MAXIT;        // Max number of iterations for the estimation
    int       *E_C_NBS;       // Positions in E_DBS of the estimated coefs
    KDB       *E_DBE;         // KDB of equations used for the estimation
    KDB       *E_DBS;         // KDB of scalars used for the estimation
    KDB       *E_DBV;         // KDB of variables used for the estimation 
    Sample    *E_SMPL;        // Current estimation sample
    char      E_MET;          // Current estimation method
    double    E_CONV_TEST;    // Sum of the squares of the relative differences between 2 iterations
    double    E_EPS;          // Convergence criterion (threshold) for the estimation
    CLEC      **E_CRHS;       // List of CLEC corresponding to the right members of the equations
    char      **E_LECS;       // List (block) of simultaneous equations of the current estimation
    char      **E_INSTRS;     // List of instruments (LEC formulas) of the current estimation    
    char      **E_ENDOS;      // List of endogenous vars of the current estimation
      
    MAT* E_U;                 // Residuals (neq x t)
    MAT* E_VCU;               // Variance / covariance of the residuals (neq x neq)    
    MAT* E_IVCU;              // Inverse of E_VCU
    MAT* E_LHS;               // Left side of equations (neq x t)
    MAT* E_RHS;               // Right side of equations (neq x t)
    MAT* E_G;                 // Jacobian matrix of the system
    MAT* E_VCC;               // Var/covar of the coefficients
    MAT* E_VCCTMP;
    MAT* E_C;                 // Vector (MAT 1 col) of estimated coefficients
    MAT* E_SMO;               // Vector of of relaxation params
    MAT* E_D;
    MAT* E_dC;                // Vector of coefficient increments
    MAT* E_DF;                // Degrees of freedom of each coefficient
    MAT* E_STDERR;            // Std error of each equation 
    MAT* E_SSRES;             // Sum of squares of residuals of each eq
    MAT* E_MEAN_Y;            // Mean of the LHS on each equation 
    MAT* E_STDEV;             // Std deviation of each equation 
    MAT* E_RSQUARE;           // R-square of each equation
    MAT* E_RSQUARE_ADJ;       // Adjusted R-square of each equation
    MAT* E_DW;                // Durbin-Watson test of each equation
    MAT* E_FSTAT;             // F-Stat of each equation
    MAT* E_LOGLIK;            // Log-likelihood of each equation
    MAT* E_STD_PCT;           // Standard errors in % for each equation
    MAT* E_M;
    MAT* E_MTMP;
    MAT* E_MTMPP;
    MAT* E_MCORR;             // Correlation matrix bw coefficients
    MAT* E_MCORRU;            // Correlation matrix bw error terms of equations
    MAT* E_DEV;               // Deviation between observed and calculated values
    MAT* E_NBCE;
    MAT* E_MCU;
    MAT* E_GMU;
    MAT* E_UM;
    MAT* E_UMT;
    MAT* E_UMTMP;
    MAT* E_UVCCTMP;
    MAT* E_GMUTMP;

public:

    /**
     *  The LEC form of the equations, the method, the simultaneous 
     *  block, the instruments and the sample are passed as arguments. 
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
     *  @param [in] char*   endos           comma separated list of equation names (endos = eqs names)
     *  @param [in] KDB*    dbe             EQS KDB (default to KV_EQS)
     *  @param [in] KDB*    dbv             VAR KDB (default to KV_VAR)
     *  @param [in] KDB*    dbs             Scalar KDB (default to KS_Scalar)
     *  @param [in] char*   from_period     first period of the estimation sample
     *  @param [in] char*   to_period       last period of the estimation sample
     *  @param [in] char    method          estimation method (0-IODE_NB_EQ_METHODS)
     *  @param [in] int     maxit           max number of iterations
     *  @param [in] double  eps             convergence threshold
     */
    Estimation(char* endos, KDB* dbe = NULL, KDB* dbv = NULL, KDB* dbs = NULL, 
               char* from_period = NULL, char* to_period = NULL, int method = -1, 
               int maxit = DEFAULT_MAXIT, double eps = DEFAULT_EPS)
    {
        // TODO: replace hard-coded separators by a (new?) variable 
        char** tmp_endos = (char**) SCR_vtoms((unsigned char*) endos, (unsigned char*) ",; ");
        
        Sample* smpl = NULL;
        if(from_period != NULL && to_period != NULL)
        {
            try
            {
                smpl = new Sample(std::string(from_period), std::string(to_period));
            }
            catch(const std::exception& e)
            {
                throw std::invalid_argument("Cannot initialize estimation: " + std::string(e.what()));
            }
        }

        initialize(tmp_endos, dbe, dbv, dbs, smpl, method, maxit, eps);
        if(smpl != NULL) SCR_free(smpl);
    }

    Estimation(char** endos, KDB* dbe = NULL, KDB* dbv = NULL, KDB* dbs = NULL, Sample* smpl = NULL,
               int method = -1, int maxit = DEFAULT_MAXIT, double eps = DEFAULT_EPS)
    {
        initialize(endos, dbe, dbv, dbs, smpl, method, maxit, eps);
    }

    ~Estimation()
    { 
        // Destructor to clean up resources if needed
        E_free_work();
    }

    MAT* get_MCORR() const
    {
        return E_MCORR;
    }

    /**
     *  Estimates an equation or a block of equations on the specified sample.
     *  
     *  @param [in] char*   from_period       first period of the estimation sample
     *  @param [in] char*   to_period         last period of the estimation sample
     *  @return     int                       0 on success, -1 on error
     */
    int estimate(char* from_period = NULL, char* to_period = NULL)
    {
        int     rc;
        bool    free_smpl = false;
        Sample* smpl = NULL;

        if(from_period != NULL && to_period != NULL)
        {
            try
            {
                smpl = new Sample(std::string(from_period), std::string(to_period));
            }
            catch(const std::exception& e)
            {
                throw std::invalid_argument("Cannot initialize estimation: " + std::string(e.what()));
            }
            free_smpl = true;               // We will free the sample after the estimation  
        }
        else
            smpl = &est_smpl;               // Use the sample provided at initialization

        rc = KE_est_s(smpl);                // Perform the estimation
        
        if(free_smpl)
            delete smpl;

        if(rc != 0)
        {
            std::string last_error = error_manager.get_last_error();
            if(!last_error.empty())
                throw std::runtime_error("Estimation failed:\n" + last_error);
        }

        return 0;
    }

    std::vector<double> get_observed_values(const int eq_nb) const
    {
        if(eq_nb < 0 || eq_nb >= E_NEQ)
            throw std::out_of_range("Equation number out of range");

        std::vector<double> values;
        for(int t = 0; t < M_NC(E_LHS); t++) values.push_back(MATE(E_LHS, eq_nb, t));
        return values;
    }

    std::vector<double> get_fitted_values(const int eq_nb) const
    {
        if(eq_nb < 0 || eq_nb >= E_NEQ)
            throw std::out_of_range("Equation number out of range");

        std::vector<double> values;
        for(int t = 0; t < M_NC(E_RHS); t++) values.push_back(MATE(E_RHS, eq_nb, t));
        return values;
    }

    std::vector<double> get_residual_values(const int eq_nb) const
    {
        if(eq_nb < 0 || eq_nb >= E_NEQ)
            throw std::out_of_range("Equation number out of range");

        std::vector<double> values;
        for(int t = 0; t < M_NC(E_LHS); t++) 
            values.push_back(MATE(E_LHS, eq_nb, t) - MATE(E_RHS, eq_nb, t));
        return values;
    }

private:
    void initialize(char** endos, KDB* dbe, KDB* dbv, KDB* dbs, Sample* smpl, int method, 
                    int maxit, double eps)
    {
        if(endos == NULL || endos[0] == NULL)
            throw std::invalid_argument("List of equations names must be provided");
        est_endos = endos;

        if(method >= IODE_NB_EQ_METHODS)
            throw std::invalid_argument(std::string("Invalid estimation method: Must be between -1 ") + 
                "(method deduced from equations objects) and " + std::to_string(IODE_NB_EQ_METHODS - 1));
        // -1 means method is deduced from equations objects
        est_method  = (method >= 0) ? method : -1;

        if(eps <= 0.0)
            throw std::invalid_argument("Epsilon must be greater than 0.0");
        E_EPS = eps;

        if(maxit <= 0)
            throw std::invalid_argument("Max iterations must be greater than 0");
        E_MAXIT = maxit;

        E_DBE  = (dbe != NULL) ? dbe : KE_WS;
        E_DBV  = (dbv != NULL) ? dbv : KV_WS;
        E_DBS  = (dbs != NULL) ? dbs : KS_WS;

        if(smpl != NULL)
            memcpy(&est_smpl, smpl, sizeof(Sample));
        else
        {
            // If no sample is provided, we will use the one from the global variables database
            if(KSMPL(KV_WS) == NULL)
                throw std::invalid_argument("No sample provided and no global variables database available");
            memcpy(&est_smpl, KSMPL(KV_WS), sizeof(Sample));
        }
    }

    /* e_est.c */
    int E_c_gmg();
    double E_rhs_ij(int i, int t);
    int E_c_rhs();
    int E_residuals();
    int E_deltac();
    int E_mod_residuals(int coef_nb, int est_coef_nb, double h);
    int E_jacobian();
    int E_scl_in_eq(int coef_nb, int eq_nb);
    int E_c_gmu();
    int E_testcv();
    int E_adaptcoef();
    int E_c_vcu();
    int E_c_ivcu();
    int E_c_mcu();
    int E_c_ivcc();
    int E_c_vcc();
    int E_gls();
    int E_est(char** endos, char** lecs, char** instrs);

    /* e_tests.c */
    double M_c_line(MAT *,int ,int );
    double E_div_0(double ,double );
    double E_sqrt(double val);
    void E_deg_freed();
    double E_c_umu();
    int E_c_loglik();
    int E_c_mcorr();
    int E_c_mcorru();
    int E_c_ttests();
    int E_output(void);

    /* e_prep.c */
    int E_prep(char **,char **);
    int E_prep_alloc();
    int E_prep_lecs(char** lecs);
    int E_prep_instrs(char** instrs);
    int E_prep_coefs();
    int E_add_scls(CLEC *clec, KDB *dbs);
    void E_prep_reset();
    void E_get_C(void);
    void E_put_C(void);
    void E_get_SMO(void);
    void E_free_work(void);

    /* e_print.c */
    void E_print_parms();
    void E_print_eqs();
    void E_print_instrs();
    void E_print_coefs();
    void E_print_mcorr();
    void E_print_mcorru();
    void E_print_eqres_1(int eq_nb);
    void E_print_eqres_2(int eq_nb);
    void E_print_eqres(int obs);
    int E_graph(char** titles, Sample* smpl, MAT* mlhs, MAT* mrhs, int view, int res);
    int E_print_results(int ,int ,int ,int ,int );

    /* k_est.c */
    void E_tests2scl(EQ *,int ,int ,int );
    void E_savescl(double, int, char*);
    void E_savevar(char*, int, MAT*);
    int KE_est_s(Sample* smpl);
    int KE_update(char* name, char* lec, int method, Sample* smpl, float* tests);
};

/* ---------------------- FUNCS ---------------------- */

/* e_dftest.c */
double *E_UnitRoot(char *,int ,int ,int );
void E_SclToReal(char *,double *);
void E_PrintDF(char *,double *,int ,int ,int );
int E_GetLecName(char *,char *);

/* e_step.c */
double C_evallec(char *,int);
double estimate_step_wise(Sample* smpl, char* eqname, char* cond, char* test);

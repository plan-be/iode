<IODE: Estimation>
..sep |
..esc ~
..ignore ¯

IODE: Estimation
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Most functions in this group are based on the book 
"Specification, Estimation and Analysis of Macroeconomic Models" by Fair (1984).

Four methods are currently proposed in IODE for the estimation of a block of equations:

&EN 0: LSQ
&EN 1: Zellner (2SLS?)
&EN 2: Instrumental Variables (IV)
&EN 3: GLS (3SLS)

No differences are made between linear and non linear equations: in both cases,
the jacobian of the system is computed numerically.

The functions related to estimation are divided in the following source files:

&EN k_est.c: functions linking the estimation package (e_*.c) to the workspaces of EQ and SCL 
&EN e_estim.c: main estimation functions 
&EN e_prep.c: construction (and destruction) of the matrices needed during the estimation process
&EN e_tests.c: calculation of statistical tests
&EN e_error.c: functions to send error messages 
&EN e_print.c: functions to print the estimation results
&EN e_dftest.c: implementation of the Dickey-Fuller test 

&EN e_step.c: moved to the report functions

&TI Estimation process
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
The main function is KE_estim() or KE_est_s() which is a more general version of KE_estim().
KE_est_s() first extracts from the current equations in KE_WS the informations needed to start the estimation: 

&EN equations in LEC form, 
&EN instruments in LEC (if any), 
&EN estimation method, 
&EN sample if not passed as argument

KE_est_s() then calls the function E_est() to (try to) estimate the coefficients. 
E_est() first gets the needed variables and scalars from KV_WS and KS_WS and then starts the estimation iteration.
If a solution is reached, 
&EN the coefficients are saved in KS_WS 
&EN E_output() is called to save the statistical tests in global matrices

The control returns to KE_est_s() which, if the solution is reached, calls E_print_results() to save in A2m a selection
of results. Finally, the statistical tests related to each equation are stored in the equations themselves.

Moreover, the same tests are saved in "volatile" scalars and variables:
&EN equation tests in scalars e<<eqnb>>_<<testname>> (e.g.: e0_dw, e1_r2...)
&EN calculated, observed and residual values in variables _YCALC, _YOBS and _YRES

These scalars and variables are "volatile" in the sense that they will be replaced in the next estimation.

&TI k_est.c
¯¯¯¯¯¯¯¯¯¯¯¯
These functions link the data in the current workspaces (EQ, SCL) and 
the estimation package. 

Before starting the estimation itself, the values of the coefficients, 
the LEC forms, the estimation methods, etc, are read from the EQ and the SCL in the current workspaces.

After the estimation, if a solution is found, the results are stored in the workspaces by
the functions in that group.

..tb
| Syntax                                                                  | Description
| ~cint KE_est_s(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** names)| Estimates an equation or a block of equations
| ~cint KE_estim(char* veqs, char* afrom, char* ato)                      | Estimates an equation or a block of equations on the specified sample. Simplified version of KE_est_s()
| ~bUtility function                                                         
| ~cvoid E_tests2scl(EQ* eq, int j, int n, int k)                         | Creates the scalars containing the results of an estimated equation
..te 

&TI e_estim.c
¯¯¯¯¯¯¯¯¯¯¯¯¯
Main estimation functions + some utilities.

..tb
| Syntax                                                                  | Description
| ~cint E_est(char** endos, char** lecs, KDB* dbv, KDB* dbs, SAMPLE* smpl, int met, char** instrs, int maxit, double eps)  | Estimates a block of equations
| ~bUtility function
| ~cint E_scl_in_eq(int coef_nb, int eq_nb)                                                                                | Checks if the coefficient coef_nb is in the equation eq_nb.
..te 

&IT List of global variables
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
..tb
| Declaration                   | Description
| ~cint       E_errno;          | Last estimation error number (between EST_NO_EQ_ERR and EST_NO_SCALARS
| ~cint       E_NEQ;            | Number of equations in the current block of equations
| ~cint       E_NCE;            | Number of estimated coefficients in the current block of equations
| ~cint       E_NC;             | Number of coefficients (est and non est) in the current block of equations
| ~cint       E_NINSTR;         | Number of instruments in the estimation
| ~cint       E_T;              | Number of periods in the estimation sample
| ~cint       E_FROM;           | Position in E_DBV of the first period in the estimation sample
| ~cint       *E_C_NBS;         | Positions in E_DBS of the estimated coefs
| ~cKDB       *E_DBV;           | KDB of variables used for the estimation 
| ~cKDB       *E_DBS;           | KDB of scalars used for the estimation
| ~cSAMPLE    *E_SMPL;          | Current estimation sample
| ~cchar      E_MET;            | Current estimation method
| ~cdouble E_CONV_TEST;      | Sum of the squares of the relative differences between 2 iterations
| ~cdouble E_EPS;            | Convergence criterion (threshold) for the estimation
| ~cCLEC      **E_CRHS;         | List of CLEC corresponding to the right members of the equations
| ~cchar      **E_LECS;         | List (block) of simultaneous equations of the current estimation
| ~cchar      **E_INSTRS;       | List of instruments (LEC formulas) of the current estimation    
| ~cchar      **E_ENDOS;        | List of endogenous vars of the current estimation
| ~cMAT* E_C                    | Vector of of estimated coefficients
| ~cMAT* E_SMO                  | Vector of of relaxation params
| ~cMAT* E_U                    | Residuals (neq x t)
| ~cMAT* E_VCU                  | Variance / covariance of the residuals (neq x neq)    
| ~cMAT* E_IVCU                 | Inverse of E_VCU
| ~cMAT* E_RHS                  | Right side of equations (neq x t)
| ~cMAT* E_G                    | Jacobian matrix of the system
| ~cMAT* E_VCC                  | Var/covar of the coefficients
| ~cMAT* E_dC                   | Vector of coefficient increments
| ~cMAT* E_DF                   | Degrees of freedom of each coefficient
| ~cMAT* E_STDERR               | Std error of each equation 
| ~cMAT* E_SSRES                | Sum of squares of residuals of each eq
| ~cMAT* E_MEAN_Y               | Mean of the LHS on each equation 
| ~cMAT* E_STDEV                | Std deviation of each equation 
| ~cMAT* E_RSQUARE              | R-square of each equation
| ~cMAT* E_RSQUARE_ADJ          | Adjusted R-square of each equation
| ~cMAT* E_DW                   | Durbin-Watson test of each equation
| ~cMAT* E_FSTAT                | F-Stat of each equation
| ~cMAT* E_LOGLIK               | Log-likelihood of each equation
| ~cMAT* E_STD_PCT              | Standard errors in % for each equation
| ~cMAT* E_MCORR                | Correlation matrix bw coefficients
| ~cMAT* E_MCORRU               | Correlation matrix bw error terms of equations
| ~cMAT* E_DEV                  | Deviation between observed and calculated values
..te

&TI e_prep.c
¯¯¯¯¯¯¯¯¯¯¯¯¯
Construction (and destruction) of the matrices needed during the estimation process + some utility functions to 
get/put the coefficients values in KS_WS.

..tb
| Syntax                                   | Description
| ~cint E_prep(char** lecs, char** instrs) | Prepares the estimation of a group a equations.
| ~cvoid E_get_C()                         | Saves in E_C the values of the estimated coefficients. 
| ~cvoid E_put_C()                         | Stores in E_DBS the values of the coefficients in E_C.
| ~cvoid E_get_SMO()                       | Saves in E_SMO the relaxation parameters of each coefficient of the equation block. 
| ~cvoid E_free_work()                     | Frees all allocated variables for the last estimation.
..te 

&TI e_tests.c 
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Computation of statistical tests.

..tb
| Syntax                                           | Description
| ~cint E_output()                                 | Computes the statistical tests after an estimation and saves the tests in MAT objects.
| ~bUtilities                                        |
| ~cdouble M_c_line(MAT* m1, int line, int oper)   | Adds, substracts or multiplies all elements of a matrix line.
| ~cdouble E_div_0(double a, double b)             | Divides a by b if b is not null. If b is null, returns 0.0.
| ~cdouble E_sqrt(double val)                      | Returns the square root of val if val >>= 0. If not, returns 0.0.
..te 

&TI e_error.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to send information and error messages from estimation functions.

..tb
| Syntax                          | Description
| ~cvoid E_msg(char* fmt,...)     |  Displays a message using kmsg().
| ~cvoid E_error(char* fmt,...)   |  Displays an estimation error message using B_seterror().
..te 


&TI e_print.c
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
Functions to generate the results of an estimation in A2M format.

..tb
| Syntax                                                                              | Description
| ~cint E_graph(char** titles, SAMPLE* smpl, MAT* mlhs, MAT* mrhs, int view, int res) | Displays or prints the graphs of residuals or observed / fitted values 
| ~cint E_print_results(int corr, int corru, int obs, int grobs, int grres)           | Prints the estimation input and output of a block of equations. 
..te 

 
&TI e_dftest.c
¯¯¯¯¯¯¯¯¯¯¯¯
Implementation of the Dickey-Fuller test (presence of a unit root in an autoregressive time series model). 

..tb
| Syntax                                                                       | Description
| ~cdouble *E_UnitRoot(char* varname, int drift, int trend, int order)      | implementation of the Dickey-Fuller test
| ~cvoid E_PrintDF(char* lec, double* res, int drift, int trend, int order) | prints the results of the Dickey-Fuller test
| ~bUtility functions
| ~cint E_GetLecName(char* lec, char* name)                                    | retrieves the name of the first variable in a lec expression
| ~cvoid E_SclToReal(char* name, double* res)                               | stores the content of a scalar in a vector of doubles
..te 

&TI e_step.c
¯¯¯¯¯¯¯¯¯¯¯¯
Function to select the coefficients to be estimated in an equation: for a given equation, 
different combination of "estimated" (i.e. relax <<>> 0) coefficients are tested and the selected 
combination must:
    - verify a given condition and
    - give the best statistical test (fstat or r2) 

The selection is done by blocking all possible combinaisons of coefficients.

..tb 
| Syntax                                                                    | Description
| ~cdouble C_evallec(char* lec, int t)                                   | Evaluates a LEC expression at a specific period of time.
| ~cdouble E_StepWise(SAMPLE* smpl, char* eqname, char* cond, char* test)| For a given equation, tries all combinations of coefficients and saves the coefficient configuration that gives the best statistical result (for a chosen test)
..te 

>

<IODE: Equation Edition and Estimation GUI>
IODE: Equation Edition and Estimation GUI
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
In IODE, equations can be created or modified in the same window. 
There are two possible cases:

&EN If the equation exists, the user enters the edition window by selecting an equation and pressing ENTER.
&EN If an equation must be created, the user presses INS or clicks on the button "New".

In both cases, the same definition window is opened where all the informations defining the equation may be enterd/modified:
&EN endogenous variable
&EN equation in LEC
&EN optionally, estimation method and sample, equation bloc, instruments.

&IT Local and global KDBs of equations and scalars
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
During the edition of an equation, two LOCAL KDBs are maintained: one for the scalars (aka coefficients) and one for
the equations. In this way, modifications to equations / scalars remain local and are
only saved in the GLOBAL KDBs (KE_WS and KS_WS) ~bat the end of the editing process, and only 
if the user considers it relevant~B.

&IT Variables
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
If the user wants to estimate the equation, the variables found in the equation are obviously required. They are
searched in the GLOBAL kdb KV_WS. There is no LOCAL KDB of variables.

&IT Scalars
¯¯¯¯¯¯¯¯¯¯¯¯
The scalars, on the contrary, are created on the fly if they don't exist when the equation(s) they belong are estimated.
The default value for a scalar is ~c0.9~C with a relaxation parameter set to ~c1.0~C.

If they exist in the GLOBAL KS_WS, they are copied into the local KDB and they keep their values 
as starting point for the estimation.

&IT Equations
¯¯¯¯¯¯¯¯¯¯¯¯¯¯
The principle is the same as for scalars. Existing equations in the global KDB are copied into the local KDB 
before the edition. Unexisting ones are created with a default value of ~cENDO := 0~C.

&IT Block of equations
¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
If a block of more than one equation is specified in the interface, the equations belonging to the block and 
existing in the global KDB KE_WS are copied into the local KDB. 
Unexisting ones are created in the local KDB with ~cENDO := 0~C  as LEC formula.

>
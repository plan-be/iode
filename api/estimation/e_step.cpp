/**
 *  @header4iode
 * 
 *  Function to select the coefficients to be estimated in an equation: for a given equation, 
 *  different combination of "estimated" (i.e. relax <> 0) coefficients are tested and the selected 
 *  combination must:
 *      - verify a given condition and
 *      - give the best statistical test (fstat or r2) 
 *  
 *  The selection is done by blocking all possible combinaisons of coefficients.
 *  
 *  TODO: rewrite and standardize these very obscure functions.
 *  
 *  List of functions 
 *  -----------------
 *      double C_evallec(char* lec, int t)                                    Evaluates a LEC expression at a specific period of time.
 *      double estimate_step_wise(SAMPLE* smpl, char* eqname, char* cond, char* test) For a given equation, tries all combinations of coefficients and saves the 
 *                                                                                  coefficient configuration that gives 
 *                                                                                  the best statistical result (for a chosen test).
 */
#include "api/b_args.h"
#include "api/k_super.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/equations.h"
#include "api/objs/scalars.h"
#include "api/estimation/estimation.h"


// Function declarations
static int E_GetScls(CLEC* clec, char*** scl);
static void E_SetScl(int relax, char* name);
static double estimate_step_wise_1(int i, int nbscl, char** scl, SAMPLE* smpl, char** eqs, char* test);


/**
 *  Retrieves (in a list of char*) the names of all scalars in a CLEC structure.
 *  
 *  @param [in] CLEC*   clec    compiled LEC expression
 *  @param [in] char*** scl     table of SCL names found in clec
 *  @return     int             nb of scalars in clec
 */
static int E_GetScls(CLEC* clec, char*** scl)                                            
{
    int     j, nbscl = 0;
    if(clec != 0) {
        for(j = 0 ; j < clec->nb_names ; j++) {
            if(L_ISCOEF(clec->lnames[j].name) && KSVAL(K_WS[SCALARS], 
               K_find(K_WS[SCALARS],clec->lnames[j].name))->relax !=0)
                    SCR_add_ptr((unsigned char***) scl, &nbscl, (unsigned char*) clec->lnames[j].name);
        }
    }
    SCR_add_ptr((unsigned char***) scl, &nbscl, NULL);
    return(nbscl - 1);
}


/**
 *  Sets the default value of a scalar (value and relax) depending on its relax value.
 *  
 *  @param [in] int     relax   if 1, sets the scalar VALUE to 0.9 and its relax to 1.0
 *                              else, sets the scalar VALUE to 0.0 and its relax to 0.0
 *  @param [in] char*   name    name of the SCL
 */
static void E_SetScl(int relax, char* name)                                             
{
    if(relax == 1) {
        K_s_set_value(KS_WS, name, 0.9);
        K_s_set_relax (KS_WS, name, 1.0);
    }    
    else { 
        K_s_set_value(KS_WS, name, 0.0);
        K_s_set_relax (KS_WS, name, 0.0);
    }
}


/**
 *  Evaluates a LEC expression at a specific period of time.
 *  
 *  @param [in] char*       lec     LEC expression
 *  @param [in] int         t       period of calculation (starts at 0 = first period of the sample)
 *  @return     double           IODE_NAN on error, lec[t] on success
 */
double C_evallec(char* lec, int t)
{
    CLEC        *clec;
    double   x = IODE_NAN;
    char        tmplec[4096];

    // For C++ when lec is a const string (in the DATA memory segment)
    SCR_strlcpy((unsigned char*) tmplec, (unsigned char*) lec, 4094);

    SCR_strip((unsigned char*) tmplec);
    if(tmplec[0]) {
        clec = L_cc(tmplec);
        if(clec == NULL) {
            std::string error_msg = "Syntax error " + std::string(L_error());
            error_manager.append_error(error_msg);
            return(x);
        }
        if(clec != 0 && !L_link(K_WS[VARIABLES], K_WS[SCALARS], clec))
            x = L_exec(K_WS[VARIABLES], K_WS[SCALARS], clec, t);
        SW_nfree(clec);
    }

    return(x);
}


/**
 *  Sub function of E_StepWize(). 
 *  Estimates an equation after having blocked or released some coefficients according to 
 *  the current run number (see estimate_step_wise() for a description of the "runs").
 *  
 *  If a solution is reached for the combination of coefficients, returns the value of the statistical test ("r2" or "fstat") retrieved from
 *  the generated scalar e0_<test>.
 *  
 *  @param [in] int         i       current run number
 *  @param [in] int         nbscl   total number of coefficients in *eqs
 *  @param [in] char**      scl     list of scalar names
 *  @param [in] SAMPLE*     smpl    estimation sample
 *  @param [in] char**      eqs     table of equation names (only eqs[0] is used)
 *  @param [in] char*       test    name of the statistical test to optimize: "fstat" or "r2"
 *  @return     double              value of test after estimation or 0 if no coefficient is found in eqs[0] (?)
 */
static double estimate_step_wise_1(int i, int nbscl, char** scl, SAMPLE* smpl, char** eqs, char* test)   
{
    int         j, cscl, nscl;
    char        buf[512];
    double      res;
    char        etest[20];
    Estimation* est = NULL;

    cscl = 1;
    nscl = 0;
    buf[0] = 0;
    for(j = 0; j < nbscl; j++) {     /*Fixe les relax des scalaires*/
        // for the i-th run, the j-th scalar is "activated" is i & cscl
        
        if(i & cscl) {                  // ! binary &, not logical && !
            E_SetScl(1, scl[j]);
            strcat(buf, scl[j]);
            strcat(buf, " ");
            nscl++;
        }
        else
            E_SetScl(0, scl[j]);
        
        cscl = cscl * 2;               // 0001 -> 0010 -> 0100 -> 1000
    }

    if(nscl > 1) {                   /* Effectue l'estimation si plus d'un relax est != 0 */
        est = new Estimation(eqs, KE_WS, KV_WS, KS_WS, smpl);
        est->estimate();
        delete est;                 // Free the Estimation object
        etest[0]=0;
        strcat(etest, "e0_");
        strcat(etest, test);
        //E_SclToReal(etest, res);
        res = K_s_get_value(KS_WS, etest); // JMP 09/07/2022
        kmsg("%s: scalars : %s, %s=%lf", eqs[0], buf, test, res);        // JMP 08/07/2022
        L_debug("%s: scalars : %s, %s=%lf\n", eqs[0], buf, test, res);   // JMP 08/07/2022
    }
    else res = 0.0;
    return(res);
}


/**
 *  For a given equation, tries all combinations of coefficient "relax" parameters (0 or 0.9) and saves 
 *  the relax configuration that gives the best statistical result (for a chosen test).
 *  
 *  Example:
 *      endo1 := c1 + c2 * A + c3 * B
 *      nbscl : 3
 *  
 *      RUN 1 (i = 1)  => c1
 *                               RUN & CSCL
 *      j=0: 001 & 001  = 001 => c1 est
 *      j=1: 001 & 010  = 000 => c2 non est
 *      j=2: 001 & 100  = 000 => c3 non est
 *  
 *      RUN 2 (i = 2) => c2
 *      j=0: 010 & 001 = 000 => c1 non est
 *      j=1: 010 & 010 = 010 => c2 est
 *      j=2: 010 & 100 = 000 => c3 non est
 *
 *      RUN 3 (i = 3) => c1, c2
 *      j=0: 011 & 001 = 001 => c1 est
 *      j=1: 011 & 010 = 010 => c2 est
 *      j=2: 011 & 100 = 000 => c3 non est
 *
 *      RUN 4 (i = 4) => c3 
 *      j=0: 100 & 001 = 000 => c1 non est
 *      j=1: 100 & 010 = 000 => c2 non est
 *      j=2: 100 & 100 = 100 => c3 est
 * 
 *      RUN 5 (i = 5) => c1, c3
 *      j=0: 101 & 001 = 001 => c1 est
 *      j=1: 101 & 010 = 000 => c2 non est
 *      j=2: 101 & 100 = 100 => c3 est
 *
 *      RUN 6 (i = 6) => c2, c3
 *      j=0: 110 & 001 = 000 => c1 non est
 *      j=1: 110 & 010 = 010 => c2 est
 *      j=2: 110 & 100 = 100 => c3 est
 *
 *      RUN 7 (i = 7) => c1, c2, c3
 *      j=0: 111 & 001 = 001 => c1 est
 *      j=1: 111 & 010 = 010 => c2 est
 *      j=2: 111 & 100 = 100 => c3 est

 *
 *  @param [in] SAMPLE* smpl    estimation SAMPLE
 *  @param [in] char*   eqname  equation name (if more than one name, only the first is used)
 *  @param [in] char*   cond    LEC expression that must satisfy "cond[0] == 0" to accept the estimation (?)
 *  @param [in] char*   test    name of the test to optimize: "n", "k", "stdev", "meany", "ssres", "stderr", 
 *                                              "fstat" , "r2"    , "r2adj" , "dw" or "loglik"
 *  @return     double          best test "test" value
 */
double estimate_step_wise(SAMPLE* smpl, char* eqname, char* cond, char* test)
{
    int         i, l=0,nbscl, nbcom;
    int         pos, lasti;
    double   lnumtest,numtest;
    EQ          *eq;
    CLEC        *cl;
    char        **scl = NULL, **eqs = NULL;

    // Crée le tableau d'équations à partir de arg (il faut qu'une seule eqs!!)
    eqs = B_ainit_chk(eqname, NULL, 0);         
    if(eqs == NULL) return(0.0);
    pos = K_find(K_WS[EQUATIONS], eqs[0]);
    if(pos < 0) return(0.0);

    // Construit le tableau de scalaires contenus dans l'équation eqs
    eq = KEVAL(K_WS[EQUATIONS], pos);               
    cl = eq->clec;
    nbscl = E_GetScls(cl, &scl);
    E_free(eq);

    // Effectue les estimations pour toutes les combi
    nbcom = (int)pow(2.0, nbscl);
    lnumtest = -999999999999.0;
    lasti = 0;
    for(i = 1; i < nbcom; i++) {                                                
        numtest = estimate_step_wise_1(i, nbscl, scl, smpl, eqs, test);
        /*print_result(numtest,numtest,C_evallec(cond,0),scl,nbscl,i,cond);*/   /*Aide pour la programmation*/
        if(lnumtest < numtest && C_evallec(cond,0)!=0 && numtest!=0) {          /*Sauve la combi qui a le meilleur fstat*/
            lasti = i;
            lnumtest = numtest;
        }
    }

    // Refait l'estimation pour la meilleure combi
    numtest = estimate_step_wise_1(lasti, nbscl, scl, smpl, eqs,test);    
    SCR_free_tbl((unsigned char**) eqs);
    SCR_free_tbl((unsigned char**) scl);
    return(numtest);
}

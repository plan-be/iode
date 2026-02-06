/**
 * @header4iode
 *
 *  Functions to manipulate equation expressions and objects.
 *  
 *  List of functions 
 *  -----------------
 *      void E_free(Equation* eq)                                                     Frees an EQ struct and all its elements
 *      int E_split_eq(char* lec, char** lhs, char** rhs)                       Extracts the left and right sides of a lec equation
 *      int E_dynadj(int method, char* lec, char* c1, char* c2, char** adjlec)  Transforms a LEC equation to add dynamic adjustment
 *      E_DynamicAdjustment(int method, char** eqs, char*c1, char*c2)           Transforms a LEC equation "in place" to add a dynamic adjustment
 *
 */
#include "equations.h"
#include "api/report/undoc/undoc.h"

/**
 *  Extracts the left and right sides of a lec equation. 
 *  
 *  @param [in]     char*  lec  text of a LEC equation 
 *  @param [out]    char** lhs  allocated left member
 *  @param [out]    char** rhs  allocated right member
 *  @return         int         0 if ok, 
 *                              -1 if  ":=" is not found in lec  
 *  
 */
int E_split_eq(char* lec, char** lhs, char** rhs)
{
    int     pos = -1;

    *lhs = NULL;
    *rhs = NULL;

    if(lec == NULL) goto done;
    pos = L_split_eq(lec);
    if(pos < 0) goto done;

    lec[pos] = 0;
    *lhs = (char*) SCR_stracpy((unsigned char*) lec);
    *rhs = (char*) SCR_stracpy(((unsigned char*) lec) + pos + 2);

done :
    return(pos);
}


/**
 *  Transforms a LEC equation to add a dynamic adjustment.
 *  
 *  For the equation LHS := RHS:
 *  
 *      - method 0: *adjlec = "d(LHS) := c1 * (RHS - (LHS)[-1])"
 *      - method 1: *adjlec = "d(LHS) := c1 * d(RHS) + c2 * (RHS -LHS)[-1]"
 *    
 *  @param [in]  int    method    type of dyn adjust (0 or 1, see above)
 *  @param [in]  char*  lec       LEC equation
 *  @param [in]  char*  c1        name of the first coefficient
 *  @param [in]  char*  c2        name of the 2d coefficient
 *  @param [out] char** adjlec    pointer to the resulting LEC equation
 *  @return      int              0 if ok, -1 if no ":=" in lec
 *  
 */

int E_dynadj(int method, char* lec, char* c1, char* c2, char** adjlec)
{
    int     lg, rc = -1;
    char    *lhs = NULL, *rhs = NULL;

    *adjlec = NULL;
    if(E_split_eq(lec, &lhs, &rhs) < 0) goto done;
    SCR_strip((unsigned char*) lhs);
    SCR_strip((unsigned char*) rhs);

    if(method == 0) {
        if(c1 == NULL || c1[0] == 0) goto done;

        lg = 2 * (int) strlen(lhs) + (int) strlen(rhs) + 100;
        *adjlec = SCR_malloc(lg);
        sprintf(*adjlec,
                "d(%s) := %s * (%s -(%s)[-1])",
                lhs, c1, rhs, lhs);
    }
    else {
        if(c1 == NULL || c1[0] == 0
                || c2 == NULL || c2[0] == 0) goto done;

        lg = 2 * (int)strlen(lhs) + 2 * (int)strlen(rhs) + 100;
        *adjlec = SCR_malloc(lg);
        sprintf(*adjlec,
                "d(%s) := %s * d(%s) + %s * (%s -%s)[-1]",
                lhs, c1, rhs, c2, rhs, lhs);
    }

    rc = 0;

done :
    SCR_free(lhs);
    SCR_free(rhs);
    return rc;
}


/**
 *  Transforms a LEC equation "in place" to add a dynamic adjustment. See E_dynadj for details.
 *  
 *    
 *  @param [in]      int    method    type of dyn adjust (0 or 1, see above)
 *  @param [in, out] char** lec       LEC equation before and after transform
 *  @param [in]      char*  c1        name of the first coefficient
 *  @param [in]      char*  c2        name of the 2d coefficient
 *  @return          int              0 if ok, -1 if no ":=" in lec
 *  
 */

int E_DynamicAdjustment(int method, char** eqs, char*c1, char*c2)
{
    char    *ae;

    SCR_strip((unsigned char*) *eqs);
    SCR_strip((unsigned char*) c1);
    SCR_strip((unsigned char*) c2);

    E_dynadj(method, *eqs, c1, c2, &ae);

    SCR_strfacpy((unsigned char**) eqs, (unsigned char*) ae);
    SCR_free(ae);

    return 0;
}

/**
 *  Print an equation and optionally its statistical tests.
 *  
 *  if B_EQS_INFOS == 0, print only the equation
 *  if B_EQS_INFOS == 1, print the equation and its (optional) comment
 *  if B_EQS_INFOS == 2, print the equation, its (optional) comment and the estimation 
 *                       tests (if the equation was estimated)
 */
bool Equation::print_definition() const
{
    std::string warn_msg = "Cannot print definition of equation '" + endo + "'";

    if(B_EQS_INFOS > 1) 
        B_PrintRtfTopic((char*) endo.c_str());
    W_printf((char*) ".par1 enum_1\n");

    bool success = print_lec_definition(endo, lec, clec, B_EQS_LEC);
    if(B_EQS_INFOS < 1) 
        return true;
    
    if(!success)
    {
        warn_msg += ": failed to print its LEC expression";
        kwarning(warn_msg.c_str());
        return false;
    }
    
    char buf[256];
    if(B_isdef((char*) comment.c_str())) 
    {
        sprintf(buf, ".par1 par_1\n%ci ", A2M_ESCCH);
        dump_string(buf, (char*) comment.c_str());
    }    

    if(B_EQS_INFOS < 2) 
        return true;
    
    success = true;

    if(method >= 0 && method < 4 && sample.nb_periods != 0 && tests[3]) 
    {
        std::string from = sample.start_period.to_string();
        std::string to = sample.end_period.to_string();
        W_printf((char*) ".par enum_2\nEstimation : %ci%c%cI on %s-%s\n\n", A2M_ESCCH, "LZIG"[method], A2M_ESCCH, 
                 (char*) from.c_str(), (char*) to.c_str());
        if(B_isdef((char*) block.c_str()))   
            dump_string((char*) "Block : ", (char*) block.c_str());
        if(B_isdef((char*) instruments.c_str())) 
            dump_string((char*) "Instruments : ", (char*) instruments.c_str());
        
        W_printf((char*) "\nTests :\n");
        W_printf((char*) ".par enum_3\n");
        W_printf((char*) "St dev of residuals         : %f\n\n", tests[EQ_STDEV]);
        W_printf((char*) "Mean of YOBS                : %f\n\n", tests[EQ_MEANY]);
        W_printf((char*) "Sum of Squares of Residuals : %f\n\n", tests[EQ_SSRES]);
        W_printf((char*) "Standard error (%%)          : %f (%ci%f%cI)\n\n", tests[EQ_STDERR], A2M_ESCCH, tests[EQ_STDERRP], A2M_ESCCH);
        W_printf((char*) "F-stat                      : %f\n\n", tests[EQ_FSTAT]);
        W_printf((char*) "R2 (R2 adjusted)            : %f (%ci%f%cI)\n\n", tests[EQ_R2], A2M_ESCCH, tests[EQ_R2ADJ], A2M_ESCCH);
        W_printf((char*) "Durbin-Watson               : %f\n\n", tests[EQ_DW]);
        W_printf((char*) "Log Likelihood              : %f\n\n", tests[EQ_LOGLIK]);
        W_printf((char*) ".par1 enum_2\nCoefficient values %ci(relax, stderr, t-stat)%cI :\n\n", A2M_ESCCH, A2M_ESCCH);
        
        std::string sname;
        CLEC* copy_clec = (CLEC *) SW_nalloc(clec->tot_lg + 1);
        memcpy(copy_clec, clec, clec->tot_lg);
        for(int j = 0 ; j < copy_clec->nb_names ; j++) 
        {
            sname = std::string(copy_clec->lnames[j].name);
            if(is_coefficient(sname)) 
            {
                if(!global_ws_scl->contains(sname))
                {
                    W_printfReplEsc((char*) ".par1 enum_%d\n~b%s~B : ", 3, sname);
                    W_printf((char*) "?\n");
                }
                else
                {
                    W_printfReplEsc((char*) ".par1 enum_%d\n~b%s~B : ", 3, sname);
                    
                    Scalar* scl = global_ws_scl->get_obj_ptr(sname);
                    if(!scl) 
                    {
                        W_printf((char*) "?\n");
                        success = false;
                    }
                    success = scl->print_definition();
                }
            }
        }
        SW_nfree(copy_clec);
    }

    W_printf((char*) "\n");

    if(!success)
        kwarning(warn_msg.c_str());
    
    return success;
}

bool Equation::to_binary(char** pack) const
{
    if(this->lec.empty()) 
        return false;

    *pack = (char*) P_create();
    
    char* c_lec = (char*) this->lec.c_str();
    char* c_endo = (char*) this->endo.c_str();
    CLEC* clec = L_solve(c_lec, c_endo);
    if(clec == NULL)  
        return false;

    *pack = (char*) P_add(*pack, c_lec, (int) this->lec.size() + 1);
    *pack = (char*) P_add(*pack, (char*) clec, clec->tot_lg);
    *pack = (char*) P_add(*pack, (char*) &(this->solved), 1);
    *pack = (char*) P_add(*pack, (char*) &(this->method), 1);
    *pack = (char*) P_add(*pack, (char*) &(this->sample), sizeof(Sample));

    if(this->comment.empty()) 
        *pack = (char*) P_add(*pack, NULL, 1);
    else 
        *pack = (char*) P_add(*pack, (char*) this->comment.c_str(), (int) this->comment.size() + 1);          /* cmt */

    if(this->block.empty()) 
        *pack = (char*) P_add(*pack, NULL, 1);
    else 
        *pack = (char*) P_add(*pack, (char*) this->block.c_str(), (int) this->block.size() + 1);              /* blk */

    if(this->instruments.empty()) 
        *pack = (char*) P_add(*pack, NULL, 1);
    else 
        *pack = (char*) P_add(*pack, (char*) this->instruments.c_str(), (int) this->instruments.size() + 1);  /* instr */

    *pack = (char*) P_add(*pack, (char*)&(this->date), sizeof(long));                     /* date */
    *pack = (char*) P_add(*pack, (char*)&(this->tests), EQS_NBTESTS * sizeof(float));     /* tests*/ /* FLOAT 12-04-98 */

    SW_nfree(clec);
    return true;
}

Equation* binary_to_eqs(char* pack, const std::string& name)
{
    size_t len;

    std::string endo(name);

    len = (size_t) P_get_len(pack, 0);
    char* c_lec = new char[len];
    strncpy(c_lec, (char*) P_get_ptr(pack, 0), len);
    std::string lec(c_lec);

    char char_method = *(char*)(P_get_ptr(pack, 3));
    int i_method = (int) char_method;
    IodeEquationMethod method = EQ_LSQ;
    if(i_method >= 0 && i_method < IODE_NB_EQ_METHODS)
        method = (IodeEquationMethod) i_method;

    len = P_get_len(pack, 5);
    char* c_cmt = new char[len];
    strncpy(c_cmt, (char*) P_get_ptr(pack, 5), len);
    std::string comment(c_cmt);

    len = P_get_len(pack, 6);
    char* c_block = new char[len];
    strncpy(c_block, (char*) P_get_ptr(pack, 6), len);
    std::string block(c_block);

    len = P_get_len(pack, 7);
    char* c_instr = new char[len];
    strncpy(c_instr, (char*) P_get_ptr(pack, 7), len);
    std::string instruments(c_instr);

    Sample* smpl = nullptr;
    char* c_smpl = (char*) P_get_ptr(pack, 4);
    if(c_smpl != NULL && strlen(c_smpl) > 0)
    {
        smpl = new Sample();
        memcpy(smpl, c_smpl, sizeof(Sample));
    }

    Period from_period = (smpl != nullptr) ? smpl->start_period : Period();
    Period to_period = (smpl != nullptr) ? smpl->end_period : Period();
    if(smpl != nullptr)
    {
        delete smpl;
        smpl = nullptr;
    }

    Equation* eq = new Equation(endo, lec, method, from_period.to_string(), to_period.to_string(), 
                                comment, instruments, block, false);
    eq->solved = *((char*) (P_get_ptr(pack, 2)));
    eq->date = *((long*) (P_get_ptr(pack, 8)));    
    memcpy(eq->tests.data(), P_get_ptr(pack, 9), EQS_NBTESTS * sizeof(float));

    return eq;
}


std::string KDBEquations::get_lec(const std::string& name) const
{
    Equation* c_eq = this->get_obj_ptr(name);
    std::string lec = c_eq->lec;
    return lec; 
}

bool KDBEquations::add(const std::string& name, const std::string& lec)
{
    IodeEquationMethod method = EQ_GLS;
    std::string from = "";
    std::string to = "";
    std::string comment = "";
    std::string instruments = "";
    std::string block = ""; 
    bool date = true;
    
    Equation eq(name, lec, method, from, to, comment, instruments, block, date);
    bool success = KDBTemplate::add(name, eq);
    return success;
}

void KDBEquations::update(const std::string& name, const std::string& lec)
{
    IodeEquationMethod method = EQ_GLS;
    std::string from = "";
    std::string to = "";
    std::string comment = "";
    std::string instruments = "";
    std::string block = ""; 
    bool date = true;

    Equation eq(name, lec, method, from, to, comment, instruments, block, date);
    KDBTemplate::update(name, eq);
}

bool KDBEquations::binary_to_obj(const std::string& name, char* pack)
{
    Equation* eq = binary_to_eqs(pack, name);
    if(!eq) 
        return false;
    
    this->k_objs[name] = eq;
    return true;
}

/**
 * Serializes an equation object.
 *
 * @param [out] pack    (char **)   placeholder for the pointer to the serialized object
 * @param [in]  name    string      equation name
 * @return                          true if the serialization succeeded, false otherwise
 *                                  -> false if the equation does not contain a LEC formula 
 *                                  -> false if the LEC formula cannot be compiled
 *  
 * @details - The endo is needed to "solve" the LEC equation. L_solve() try to analytically
 *            solve the equation to obtain a formula in the form :
 *                ENDO := f(x)
 *          - If it is not possible, a Newton-Raphson like algorithm will be used to solve 
 *            the equation for each iteration of the simulation process.
 */
bool KDBEquations::obj_to_binary(char** pack, const std::string& name)
{
    Equation* eq = this->get_obj_ptr(name);
    bool success = eq->to_binary(pack);
    return success;
}

bool KDBEquations::grep_obj(const std::string& name, const std::string& pattern, 
    const bool ecase, const bool forms, const bool texts, const char all) const
{
    bool found = false;
    Equation* eq = this->get_obj_ptr(name);
    if(forms) 
        found = wrap_grep_gnl(pattern, eq->lec, ecase, all);
    if(!found && texts)
        found = wrap_grep_gnl(pattern, eq->comment, ecase, all);
    return found;
}

char* KDBEquations::dde_create_obj_by_name(const std::string& name, int* nc, int* nl)
{
    std::string lec = this->get_obj_ptr(name)->lec;
    char* obj = new char[lec.size() + 1];
    strncpy(obj, lec.c_str(), lec.size() + 1);
    return obj;
}

bool KDBEquations::print_obj_def(const std::string& name)
{
    Equation* eq = this->get_obj_ptr(name);
    if(!eq) 
        return false;
    
    bool success = eq->print_definition();
    return success;
}

void KDBEquations::update_reference_db()
{
    if(global_ref_eqs[0]) 
        delete global_ref_eqs[0];
    global_ref_eqs[0] = new KDBEquations(this, "*", false);      
}

/**
 *  Retrieves a statistical test stored the equation whose endo is name.
 *  
 *  @param [in] KDB*    kdb      KDB containing EQ name
 *  @param [in] char*   name     name of the equation (=endogenous)
 *  @param [in] int     test_nb  position of the test in eq->tests 
 *  @return     double           test value or IODE_NAN if equation name not found
 *  
 */
double K_etest(KDBEquations* kdb, char* c_name, int test_nb)
{   
    std::string name = std::string(c_name);
    if(!kdb->contains(name)) 
        return(IODE_NAN);
    
    std::array<float, EQS_NBTESTS> tests = kdb->get_obj_ptr(name)->tests;
    double value = (double) tests[test_nb];
    return value;
}

// Returns test calculated during the last estimation of equation name
double K_e_stdev (KDBEquations* kdb, char*name) {return(K_etest(kdb, name, EQ_STDEV));}
double K_e_meany (KDBEquations* kdb, char*name) {return(K_etest(kdb, name, EQ_MEANY));}
double K_e_ssres (KDBEquations* kdb, char*name) {return(K_etest(kdb, name, EQ_SSRES));}
double K_e_stderr(KDBEquations* kdb, char*name) {return(K_etest(kdb, name, EQ_STDERR));}
double K_e_fstat (KDBEquations* kdb, char*name) {return(K_etest(kdb, name, EQ_FSTAT));}
double K_e_r2    (KDBEquations* kdb, char*name) {return(K_etest(kdb, name, EQ_R2));}
double K_e_r2adj (KDBEquations* kdb, char*name) {return(K_etest(kdb, name, EQ_R2ADJ));}
double K_e_dw    (KDBEquations* kdb, char*name) {return(K_etest(kdb, name, EQ_DW));}
double K_e_loglik(KDBEquations* kdb, char*name) {return(K_etest(kdb, name, EQ_LOGLIK));}

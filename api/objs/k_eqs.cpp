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
    return(rc);
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

    return(0);
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
                    
                    Scalar* scl = KSVAL(global_ws_scl.get(), sname);
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

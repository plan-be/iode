/**
 *  @header4iode
 * 
 *  Estimation functions called in IODE reports.
 *    
 *  Except for the static ones (local), most functions in this group share the 
 *  same syntax: 
 *  
 *      int fn_name(char* arg) 
 *  
 *  where:  
 *      arg is the report line (without the command name) 
 *      the return code is 0 on success, any other value indicating and error
 *  
 *  List of functions 
 *  -----------------
 * int B_EqsEstimateEqs(Sample* smpl, char* pattern)            Estimates a bloc of equations on a defined Sample.
 * int B_EqsEstimate(char* arg, int unused)                     Implementation of the report function $EqsEstimate.
 * int B_EqsSetSample(char* arg, int unused)                    Implementation of the report function $EqsSetSample.
 * int B_EqsSetMethod(char* arg, int unused)                    Implementation of the report function $EqsSetMethod.   
 * int B_EqsSetBloc(char* arg, int unused)                      Implementation of the report function $EqsSetBlock
 * int B_EqsSetCmt(char* arg, int unused)                       Implementation of the report function $EqsSetCmt.
 * int B_EqsSetInstrs(char* arg, int unused)                    Implementation of the report function $EqsSetInstrs.
 */

#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/estimation/estimation.h"
#include "api/report/commands/commands.h"


/**
 *  Extracts the first 2 args of a report command and tries to translated
 *  them to a Sample. Returns the remaining args as a list of equation names.     
 */
static std::vector<std::string> B_EqsSplitSmplName(const std::string& arg, Sample **psmpl)
{
    std::vector<std::string> args = split_multi(arg, std::string(B_SEPS));
    if(args.size() < 2)
    {
        *psmpl = nullptr;
        error_manager.append_error("EqsEstimate: not enough arguments for defining the sample");
        return std::vector<std::string>();
    }

    std::string from = args[0]; 
    std::string to = args[1];
    args.erase(args.begin(), args.begin() + 2);
    try
    {
        *psmpl = new Sample(from, to);
    }
    catch(const std::exception& e)
    {
        *psmpl = nullptr;
        std::string err_msg = "EqsEstimate: invalid sample\n" + std::string(e.what()); 
        error_manager.append_error(err_msg);
        return std::vector<std::string>();
    }

    return args;
}


/**
 *  Estimates a bloc of equations on a defined Sample. On success, the results
 *  are saved in the equations themselves (tests) and the scalars (coefficients).
 *  
 *  @param [in] Sample* smpl    estimation sample
 *  @param [in] char**  eqs     block of equations to be simultaneously estimated 
 *  @return     int             -1 if some eqs are not found
 *                              rc of KE_est_s() otherwise 
 */
int B_EqsEstimateEqs(Sample* smpl, char* pattern)
{
    int rc = -1;
    if(pattern == NULL) 
        error_manager.append_error("EqsEstimate: empty equations list");
    else 
    {
        try
        {
            KDBEquations* dbe = new KDBEquations(global_ws_eqs.get(), std::string(pattern), false);

            if(dbe->size() > 0)
            {
                std::string from_period = smpl->start_period.to_string();
                std::string to_period   = smpl->end_period.to_string();
                Estimation est(pattern, dbe, global_ws_var.get(), global_ws_scl.get(), 
                               (char*) from_period.c_str(), (char*) to_period.c_str());
                rc = est.estimate();
            }
            delete dbe;
            dbe = nullptr;
        }
        catch(const std::exception& e)
        {
            std::string error_msg = "EqsEstimate: estimation failed\n" + std::string(e.what());
            error_manager.append_error(error_msg);
        }
    }

    return rc;
}

/**
 *  Implementation of the report function $EqsEstimate.
 *  
 *  Syntax : $EqsEstimate perfrom perto eqname1 ...
 *  
 *  @See https://iode.plan.be/doku.php?id=eqsestimate for details. 
 *  
 */
int B_EqsEstimate(char* arg, int unused)
{
    Sample* smpl = nullptr;
    std::vector<std::string> eq_names = B_EqsSplitSmplName(std::string(arg), &smpl);
    if(!smpl)
        return -1;

    std::string pattern = join(eq_names, ";");
    pattern = trim(pattern);
    int rc = B_EqsEstimateEqs(smpl, (char*) pattern.c_str());
    delete smpl;
    return rc;
}


/**
 *  Implementation of the report function $EqsSetSample.
 *  
 *  Syntax :  
 *      $EqsSetSample from to eqname1 eqname2 ...
 *
 *      where:
 *         from and to define the estimation Sample
 *         eqname1 eqname2... are existing the (existing equations) whose estimation Sample must be replaced.
 *  
 *  @See https://iode.plan.be/doku.php?id=eqssetsample for details. 
 *  
 */
int B_EqsSetSample(char* arg, int unused)
{
    Sample  *smpl = nullptr;

    std::vector<std::string> eq_names = B_EqsSplitSmplName(arg, &smpl);
    if(!smpl)
        return -1;

    int rc = 0;
    Equation* eq_ptr;
    for(const std::string& eq_name : eq_names) 
    {
        eq_ptr = global_ws_eqs->get_obj_ptr(eq_name);
        if(!eq_ptr)
        {
            rc = -1;
            break;
        }
        eq_ptr->sample = *smpl;
    }

    delete smpl;
    return rc;
}


/**
 *  Implementation of the report function $EqsSetMethod.
 *  
 *  Syntax :  
 *      $EqsSetMethod  {0|1|2|3} eqname1 eqname2 ...
 *
 *       where:
 *          0 indicates LSQ (least squares)
 *          1 indicates Zellner
 *          2 indicates INF (2 stages with instruments)
 *          3 indicates GLS (3 stages with instruments)
 *          4 indicates MAXLIK (Maximum likelihood - BHHH - form version 6.21)
 *
 *          eqname1 .. are existing equation names
 *  
 *  @See https://iode.plan.be/doku.php?id=eqssetmethod for details. 
 *  
 */
int B_EqsSetMethod(char* arg, int unused)
{
    int     lg1, meth, rc = 0;
    char    tmeth[16], **eqs = 0;

    lg1 = B_get_arg0(tmeth, arg, 15);
    meth = atoi(tmeth);
    eqs = B_ainit_chk(arg + lg1, NULL, 0);

    Equation* eq;
    std::string eq_name;
    for(int i = 0 ; eqs[i] ; i++) 
    {
        eq_name = eqs[i];
        eq = global_ws_eqs->get_obj_ptr(eq_name);
        if(!eq)
        {
            rc = -1;
            break;
        }
        eq->method = (char) meth;
    }

    SCR_free_tbl((unsigned char**) eqs);
    return rc;
}


/**
 *  Implementation of the report function $EqsSetBlock.
 *  
 *  Syntax :  
 *       $EqsSetBlock eqname1 eqname2 ...
 *
 *      where eqname1 eqname2 ... are existing equation names.
 *  
 *  @See https://iode.plan.be/doku.php?id=eqssetbloc for details. 
 *  
 */
int B_EqsSetBloc(char* arg, int unused)
{
    int     rc = 0;
    char    **eqs = 0, *bloc;

    eqs = B_ainit_chk(arg, NULL, 0);
    bloc = (char*) SCR_mtov((unsigned char**) eqs, (int) ';');

    Equation* eq;
    std::string eq_name;
    for(int i = 0 ; eqs[i] ; i++) 
    {
        eq_name = eqs[i];
        eq = global_ws_eqs->get_obj_ptr(eq_name);
        if(!eq)
        {
            rc = -1;
            break;
        }
        eq->block = std::string(bloc);
    }

    SCR_free_tbl((unsigned char**) eqs);
    SCR_free(bloc);
    return rc;
}


/**
 *  Implementation of the report function $EqsSetCmt.
 *  
 *  Syntax :  
 *   $EqsSetCmt eqname comment
 *
 *       where eqname is the equation name 
 *             comment is a free text
 *    
 *  @See https://iode.plan.be/doku.php?id=eqssetcmt for details. 
 *  
 */
int B_EqsSetCmt(char* arg, int unused)
{
    int     lg1;
    char    name[16];

    lg1 = B_get_arg0(name, arg, K_MAX_NAME + 1);
    Comment cmt(arg + lg1 + 1);

    std::string eq_name(name);
    Equation* eq = global_ws_eqs->get_obj_ptr(eq_name);
    if(!eq)
        return -1;
    eq->comment = cmt;
    return 0;
}


/**
 *  Implementation of the report function $EqsSetInstrs.
 *  
 *  Syntax :  
 *   $EqsSetInstrs eqname lec_intr_1;lec_instr_2;...
 
        where   eqname is the equation name 
                lec_instr_1, ... are the LEC formulas defining the instruments
 *    
 *  @See https://iode.plan.be/doku.php?id=eqssetinstrs for details. 
 *  
 */
int B_EqsSetInstrs(char* arg, int unused)
{
    int     lg1;
    char    name[16];

    lg1 = B_get_arg0(name, arg, K_MAX_NAME + 1);
    std::string instrs(arg + lg1 + 1);

    std::string eq_name(name);
    Equation* eq = global_ws_eqs->get_obj_ptr(eq_name);
    if(!eq)
        return -1;
    eq->instruments = instrs;
    return 0;
}

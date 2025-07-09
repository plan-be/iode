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
 * int B_EqsEstimateEqs(SAMPLE* smpl, char** eqs)   Estimates a bloc of equations on a defined SAMPLE.
 * int B_EqsEstimate(char* arg)                     Implementation of the report function $EqsEstimate.
 * int B_EqsSetSample(char* arg)                    Implementation of the report function $EqsSetSample.
 * int B_EqsSetMethod(char* arg)                    Implementation of the report function $EqsSetMethod.   
 * int B_EqsSetBloc(char* arg)                      Implementation of the report function $EqsSetBlock
 * int B_EqsSetCmt(char* arg)                       Implementation of the report function $EqsSetCmt.
 * int B_EqsSetInstrs(char* arg)                    Implementation of the report function $EqsSetInstrs.
 */

#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/objs/objs.h"
#include "api/estimation/estimation.h"
#include "api/report/commands/commands.h"


/**
 *  Extracts the first 2 args of a report command and tries to translated
 *  them to a SAMPLE. Returns the remaining args as a list of equation names.
 *  
 *  @param [in] char*       arg     report line (w/o command)
 *  @param [in] SAMPLE**    psmpl   pointer to the result SAMPLE
 *  @return     char**              table of equations names        
 */
static char **B_EqsSplitSmplName(char* arg, SAMPLE **psmpl)
{
    int     lg1, lg2;
    char    from[16], to[16], **eqs = 0;

    lg1 = B_get_arg0(from, arg, 15);
    lg2 = B_get_arg0(to, arg + lg1, 15);
    *psmpl = PER_atosmpl(from, to);

    if(*psmpl == NULL) {
        B_seterrn(39, from, to);
        return(eqs);
    }

    eqs = B_ainit_chk(arg + lg1 + lg2, NULL, 0);
    return(eqs);
}


/**
 *  Estimates a bloc of equations on a defined SAMPLE. On success, the results
 *  are saved in the equations themselves (tests) and the scalars (coefficients).
 *  
 *  @param [in] SAMPLE* smpl    estimation sample
 *  @param [in] char**  eqs     block of equations to be simultaneously estimated 
 *  @return     int             -1 if some eqs are not found
 *                              rc of KE_est_s() otherwise 
 */
int B_EqsEstimateEqs(SAMPLE* smpl, char** eqs)
{
    KDB* dbe;
    int  rc;

    if(eqs == NULL || SCR_tbl_size((unsigned char**) eqs) == 0) {
        B_seterrn(40);
        return(-1);
    }
    else {
        dbe = K_refer(KE_WS, SCR_tbl_size((unsigned char**) eqs), eqs);
        if(dbe == 0 || KNB(dbe) == 0)                                       
            rc = -1;                                                          
        else{
            Estimation est(eqs, dbe, KV_WS, KS_WS, smpl);
            rc = est.estimate();
        }
        K_free_kdb(dbe);
    }

    return(rc);
}

/**
 *  Implementation of the report function $EqsEstimate.
 *  
 *  Syntax : $EqsEstimate perfrom perto eqname1 ...
 *  
 *  @See https://iode.plan.be/doku.php?id=eqsestimate for details. 
 *  
 */
int B_EqsEstimate(char* arg)
{
    int     rc = -1;
    char    **eqs;
    SAMPLE  *smpl;

    eqs = B_EqsSplitSmplName(arg, &smpl);
    if(smpl == 0) return(-1);
    rc = B_EqsEstimateEqs(smpl, eqs);
    SCR_free_tbl((unsigned char**) eqs);
    SCR_free(smpl);
    return(rc);
}


/**
 *  Implementation of the report function $EqsSetSample.
 *  
 *  Syntax :  
 *      $EqsSetSample from to eqname1 eqname2 ...
 *
 *      where:
 *         from and to define the estimation SAMPLE
 *         eqname1 eqname2... are existing the (existing equations) whose estimation SAMPLE must be replaced.
 *  
 *  @See https://iode.plan.be/doku.php?id=eqssetsample for details. 
 *  
 */
int B_EqsSetSample(char* arg)
{
    int     rc = 0, i;
    char    **eqs;
    SAMPLE  *smpl;

    eqs = B_EqsSplitSmplName(arg, &smpl);
    if(smpl == 0) return(-1);
    for(i = 0 ; eqs[i] ; i++) {
        rc = K_upd_eqs(eqs[i], 0L, 0L, -1, smpl, 0L, 0L, 0L, 0);
        if(rc) break;
    }

    SCR_free_tbl((unsigned char**) eqs);
    SCR_free(smpl);
    return(rc);
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
int B_EqsSetMethod(char* arg)
{
    int     lg1, meth, rc = 0, i;
    char    tmeth[16], **eqs = 0;

    lg1 = B_get_arg0(tmeth, arg, 15);
    meth = atoi(tmeth);
    eqs = B_ainit_chk(arg + lg1, NULL, 0);
    for(i = 0 ; eqs[i] ; i++) {
        rc = K_upd_eqs(eqs[i], 0L, 0L, meth, 0L, 0L, 0L, 0L, 0);
        if(rc) break;
    }

    SCR_free_tbl((unsigned char**) eqs);
    return(rc);
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
int B_EqsSetBloc(char* arg)
{
    int     rc = 0, i;
    char    **eqs = 0, *bloc;

    eqs = B_ainit_chk(arg, NULL, 0);
    bloc = (char*) SCR_mtov((unsigned char**) eqs, (int) ';');
    for(i = 0 ; eqs[i] ; i++) {
        rc = K_upd_eqs(eqs[i], 0L, 0L, -1, 0L, 0L, bloc, 0L, 0);
        if(rc) break;
    }

    SCR_free_tbl((unsigned char**) eqs);
    SCR_free(bloc);
    return(rc);
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
int B_EqsSetCmt(char* arg)
{
    int     lg1, rc = 0;
    char    name[16];

    lg1 = B_get_arg0(name, arg, K_MAX_NAME + 1);
    rc = K_upd_eqs(name, 0L, arg + lg1 + 1, -1, 0L, 0L, 0L, 0L, 0);
    return(rc);
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
int B_EqsSetInstrs(char* arg)
{
    int     lg1, rc = 0;
    char    name[16];

    lg1 = B_get_arg0(name, arg, K_MAX_NAME + 1);
    rc = K_upd_eqs(name, 0L, 0L, -1, 0L, arg + lg1 + 1, 0L, 0L, 0);
    return(rc);
}

/**
 *  @header4iode
 * 
 *  IODE HIGH LEVEL API
 *  -------------------
 *  Set of high-level functions essentially developed for the creation of the Cython implementation of IODE.
 *  Most of these functions are (almost) aliases to other API functions but with a different and more
 *  coherent naming convention.
 *  
 *  
 *  List of functions
 *  -----------------
 *      int IodeInit()                                | Initialise an IODE session.
 *      int IodeEnd()                                 | Terminate an IODE session.
 *      char *IodeVersion()                           | Return the IODE version in a const string.
 *  
 *    WS related functions
 *        
 *      int IodeLoad(char *name, int type)            | Return the IODE version in a const string.
 *      int IodeSave(char *name, int type)            | Load an IODE workspace file.
 *      int IodeClearWs(int type)                     | Clear a workspace.
 *      int IodeClearAll()                            | Clear all workspaces.
 *      char **IodeContents(char *pattern, int type)  | Returns a table of object names corresponding to the specified pattern.
 *  
 *    SAMPLE Functions
 *  
 *      int IodeGetSampleLength()                     | Returns the current sample length (0 if undefined)
 *      int IodeIsSampleSet()                         | Indicates if the VAR sample is defined
 *      char *IodeGetSampleAsString()                 | Returns current sample in an ALLOCATED string in the form "per1 per2".
 *      char **IodeGetSampleAsPeriods()               | Return all periods of the current KV_WS sample in a table of strings.
 *      char **IodeCreateSampleAsPeriods(char* aper_from, char* aper_to) | Return all periods from aper_from to aper_to in a table of strings.
 *      double *IodeGetSampleAsDoubles(int *lg)                          | Return a sample as a list of doubles.
 *      int IodeSetSampleStr(char* str_from, char* str_to)               | Set the WS sample from periods as strings
 *      int IodeSetSample(int from, int to)                              | Set the WS sample from integers (yearly only).
 *  
 *    OBJECTS MANIPULATION FUNCTIONS
 *  
 *      int IodeDeleteObj(char* obj_name, int obj_type)                  | Delete the object obj_name of type obj_type.
 *      char *IodeGetCmt(char *name)                                     | Return a pointer to the comment "name". The pointer cannot be freed.
 *      int IodeSetCmt(char *name, char *cmt)                            | Save a the comment cmt under the name "name".
 *      int IodeGetEqs(char *name, char**lec, int *method, char*sample_from, char* sample_to, char**blk, char**instr, float *tests) | Retrieve an equation and its elements.
 *      char *IodeGetEqsLec(char *name)                                  | Retrieve the (non allocated) LEC form of the equation "name".
 *      int IodeSetEqs(char *name, char *eqlec)                          | Change the LEC form of an equation.
 *      char *IodeGetIdt(char *name)                                     | Return the (non allocated) pointer to an identity.
 *      int IodeSetIdt(char *name, char *idt)                            | Set the LEC form of an identity.
 *      char *IodeGetLst(char *name)                                     | Return the (non allocated) pointer to a list.
 *      int IodeSetLst(char *name, char *lst)                            | Set a list value.
 *      int IodeGetScl(char *name, double* value, double *relax, double *std_err) | Change the values of an existing scalar. Return -1 if the scalar does not exist.
 *      int IodeSetScl(char *name, double value, double relax, double std) | Set the values of a scalar. The scalar is created if it does not exist.
 *      char* IodeGetTbl(char *name, char *gsmpl)                        | Compute a table on the GSAMPLE gsmpl and return a string containing the result.  
 *      char* IodeGetTblTitle(char *name)                                | Return a table (first) title in an not allocated string.
 *      TBL* IodeGetTblDefinition(char *name)                            | Return a table struct
 *      int IodeSetTblFile(int ref, char *filename)                      | Load the var file filename and set its reference number to ref.
 *      double IodeGetVarT(char *name, int t, int mode)                  | Get the value of the variable name at position t where t is the position in the KV_WS SAMPLE.
 *      int IodeSetVarT(char *name, int t, int mode, double value)       | Set the value of the variable name at position t, possibly after recalulation based on value and mode.
 *      double *IodeGetVector(char *name, int *lg)                       | Returns a pointer to the first value of the VAR name. 
 *      int IodeCalcSamplePosition(char *str_pyper_from, char* str_pyper_to, int *py_pos, int *ws_pos, int *py_lg) |
 *      int IodeSetVector(char *la_name, double *la_values, int la_pos, int ws_pos, int la_lg) | Determines the position to copy from (python object), where to copy to (KV_WS) and the nb of elements to copy 
 *  
 *   ESTIMATION
 *  
 *      int IodeEstimate(char* veqs, char* afrom, char* ato)             | Estimate an equation of a given sample. 
 *  
 *   SIMULATION
 *  
 *      int IodeModelSimulate(char *per_from, char *per_to, char *eqs_list, char *endo_exo_list, double eps, double relax, int maxit, int init_values, int sort_algo, int nb_passes, int debug, double newton_eps, int newton_maxit, int newton_debug) | Simulate of model.
 *  
 *   REPORTS
 *  
 *      int IodeExecArgs(char *filename, char **args)                    | Execute a report with optionnal parameters.
 *      int IodeExec(char *filename)                                     | Execute a report with no parameters.
 *  
 *   LEC DIRECT EXECUTION
 *  
 *      static CLEC* IodeLinkLec(char* lec)                              | Compile and link a LEC expression with the KV_WS and KS_WS.
 *      double IodeExecLecT(char* lec, int t)                            | Calc a LEC expression in t.
 *      double *IodeExecLec(char* lec)                                   | Calculate a LEC expression on the full KV_WS sample.
 *  
 *   MESSAGES
 *  
 *      void IodeSuppressMsgs()                                          | Suppress all messages from the A2M interpretor and from the IODE functions.
 *      void IodeResetMsgs()                                             | Reset the messages from the A2M interpretor and from the IODE functions.
 *  
 *   MISCELLANEOUS
 *  
 *      int IodeSetNbDec(int nbdec)                                      | Define the number of decimals in the tables and variables to be printed.
 *      int IodeGetNbDec()                                               |  Returns the number of decimals currently set for the printing of tables and variables.
 *  
 */
 
#include "iode.h"
#include "iodeapi.h"

/**
 *  Initialise an IODE session.
 *  
 *  @return int     always 0
 */
int IodeInit()
{
    // To define the iode.msg file BEFORE scr4 (in SCR_init())
    B_IodeMsgPath();            
    
    // Initialize chrono for report functions
    RPF_ChronoReset();      
    
    // Initialize scr4 SWAP memory management
    SW_MIN_MEM = 120 * 1024L;
    SW_MIN_SEGS = 2;
    SW_SEG_SIZE = B_IniReadNum("GENERAL", "SEGSIZE", 65500);
    if(SW_SEG_SIZE < 16384) SW_SEG_SIZE = 16384;                
    SW_init(1);
    
    // Create Empty WS
    K_init_ws(0);

    // Default values for language, nbdec, separators
    K_LANG = 0;     
    K_NBDEC = -1;   
    A_SEPS = " ,;\n\t\r";
        
    // Suppress default filename wildcard behavior
    A_NO_EXPANDSTAR_DFT = 1; // JMP 12-11-2012 

    // Assign default "super" function pointers
    IODE_assign_super_API(); // JMP 15/2/2023 
    
    return(0);
}


/**
 *  Terminates an IODE session.
 *  
 *  @return int     always 0
 */

int IodeEnd()
{
    SW_end();
    W_close();
    K_end_ws(0);                
    L_free_anames();            
    BUF_free();                 
    RP_proc_free_all();         
    RP_free_bufs();             
    // B_reset_error();  
    B_clear_last_error();      // JMP 16/01/2022 
    SCR_free(ODE_INIFILE);     // JMP 30/10/2022
    ODE_INIFILE = 0;
    
    return(0);
}

/**
 *  Return the IODE version in a const string.
 *  
 *  @return char*  IODE version
 */
char *IodeVersion()
{
   return(IODE_VERSION);
}


// --------------------
// WS related functions
// --------------------

/**
 *  Load an IODE workspace file.
 *  
 *  @param [in] name    char*   filename 
 *  @param [in] type    int     file type (K_CMT...K_VAR)
 *  @return             int     nb of objects read or -1 on error
 */
int  IodeLoad(char *name, int type)
{
    KDB     *kdb;

    if(B_WsLoad(name, type) < 0) return(-1);
    else {
        kdb = K_WS[type];
        return(KNB(kdb));
    }
}


/**
 *  Save an IODE workspace in a binary file.
 *  
 *  @param [in] name    char*   filename
 *  @param [in] type    int     object type
 *  @return             int     return value of B_WsSave()
 */
int  IodeSave(char *name, int type)
{
    return(B_WsSave(name, type));
}


/**
 *  Clear a workspace.
 *  
 *  @param [in] type int    type of the workspace to be cleared
 *  @return          int    always 0
 */
int IodeClearWs(int type)
{
    if(type >= K_CMT && type <= K_VAR) 
        K_clear(K_WS[type]);
    return(0);
}


/**
 *  Clear all workspaces.
 *  
 *  @return          int    always 0
 */
int IodeClearAll()
{
    int type;

    for(type = K_CMT; type <= K_VAR; type++)
        K_clear(K_WS[type]);
    
    return(0);
}


/**
 *  Returns a table of object names corresponding to the specified pattern.
 *  
 *  @param [in] pattern char*   object name pattern (may include * or ?)
 *  @param [in] type    int     workspace type to be searched
 *  @return             char**  table of object names or NULL if no objects found
 */
char **IodeContents(char *pattern, int type)
{
    return(K_grep(K_WS[type], pattern, 0, 1, 0, 0, '*'));
}


// ----------------
// SAMPLE Functions
// ----------------

//  Returns the current sample length (0 if undefined)
int IodeGetSampleLength()
{
    SAMPLE  *smpl;

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
    return(smpl->s_nb);
}


//  Indicates if the VAR sample is defined
int IodeIsSampleSet()
{
    return(IodeGetSampleLength() != 0);
}


/**
 *  Returns current sample in an ALLOCATED string in the form "per1 per2".
 *  
 *  DONT FORGET TO FREE THE RESULT !!
 *  
 *  @return char*   allocated string: "" if sample is not defined
 */
char *IodeGetSampleAsString()
{
    SAMPLE  *smpl;
    char	*text;
    char    b1[20], b2[20];

    if(!IodeIsSampleSet()) return(NULL);
    text = SW_nalloc(50);
    
    smpl = KSMPL(KV_WS);
    
    sprintf(text, "%s %s", PER_pertoa(&(smpl->s_p1), b1),
            PER_pertoa(&(smpl->s_p2), b2));
    return(text);
}


/**
 *  Return all periods of the current KV_WS sample in a table of strings.
 *  
 *  @return  char**     allocated table of char* 
 *  
 *  TODO: call IodeCreateSampleAsPeriods() 
 */
char **IodeGetSampleAsPeriods() 
{
    SAMPLE  *smpl = KSMPL(KV_WS);
    char    **periods;
    PERIOD	*per_t;
    int		t;

    if(smpl == NULL || smpl->s_nb == 0) return(NULL);
    periods = (char**) SW_nalloc((1 + smpl->s_nb) * sizeof(char*));
	for(t = 0;  t < smpl->s_nb; t++) {
        per_t = PER_addper(&(smpl->s_p1), t);
        periods[t] = (char*)SCR_stracpy(PER_pertoa(per_t, NULL));
	}

    return(periods);
}


/**
 *  Return all periods from aper_from to aper_to in a table of strings.
 *  
 *  @return  char** allocated table of char* 
 *    
 *  @param [in] aper_from char*     starting period
 *  @param [in] aper_to   char*     ending period
 *  @return               char**    list of periods {"1990Y1", "1991Y1"..., NULL}
 */
char **IodeCreateSampleAsPeriods(char* aper_from, char* aper_to) 
{
    SAMPLE  *smpl;
    char    **periods;
    PERIOD	*per_t;
    int		t;

    smpl = PER_atosmpl(aper_from, aper_to);
    if(smpl == NULL || smpl->s_nb == 0) return(NULL);
    periods = (char**) SW_nalloc((1 + smpl->s_nb) * sizeof(char*));
	for(t = 0;  t < smpl->s_nb; t++) {
        per_t = PER_addper(&(smpl->s_p1), t);
        periods[t] = (char*)SCR_stracpy(PER_pertoa(per_t, NULL));
	}
    SW_nfree(smpl);
    return(periods);
}


/**
 *  Returns a sample as a list of doubles.
 *  
 *  @param [out] lg     int*     number of periods
 *  @return             double*  list of periods in the form {2000.0, 2000.25, 2000.5, 2000.75, 2001.0, ...}   
 */
double *IodeGetSampleAsDoubles(int *lg)
{
    double  *value = NULL;
    SAMPLE  *smpl;
    PERIOD	*from;
    int		t, per;

    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
    *lg = smpl->s_nb;

    value = (double *) SW_nalloc(smpl->s_nb * sizeof(double));
    from = &smpl->s_p1;
    for(t = 0;  t < smpl->s_nb; t++) {
        per = PER_nbper(from);
        value[t] = (IODE_REAL) from->p_y + (from->p_s + t - 1)/per
                   + ((from->p_s + t - 1) % per)/ (IODE_REAL) per;
    }

    return(value);
}


/**
 * Set the WS sample from periods as strings
 */
int IodeSetSampleStr(char* str_from, char* str_to)
{
    SAMPLE  *smpl;
    int     rc;
    
    //printf("Setting sample to %s-%s: ", str_from, str_to);
    smpl = PER_atosmpl(str_from, str_to);
    if(smpl) {
        rc = KV_sample(KV_WS, smpl);
        //printf("done. rc = %d\n", rc);
        SW_nfree(smpl);
        //printf("Sample re-set to %s-%s\n", str_from, str_to);
        return(rc);
    }    
    //printf("error\n");
    return(-1); 
}

/**
 * Set the WS sample from integers (yearly only).
 */
int IodeSetSample(int from, int to)
{
    char    afrom[64];
    char    ato[64];
    SAMPLE  *smpl;
    int     rc;
    
    sprintf(afrom, "%dY1", from);
    sprintf(ato,   "%dY1", to);
    printf("Setting sample to %s-%s: ", afrom, ato);
    smpl = PER_atosmpl(afrom, ato);
    if(smpl) {
        rc = KV_sample(KV_WS, smpl);
        printf("done. rc = %d\n", rc);
        SW_nfree(smpl);
        printf("Sample re-set to %s-%s\n", afrom, ato);
        return(rc);
    }    
    printf("error\n");
    return(-1); 
}

// End of SAMPLE


// -------------------------------
// OBJECTS MANIPULATION FUNCTIONS
// -------------------------------

// Delete the object obj_name of type obj_type.
int IodeDeleteObj(char* obj_name, int obj_type)
{
    return(K_del_by_name(K_WS[obj_type], obj_name));
}


// -------------------------------
// COMMENTS MANIPULATION FUNCTIONS
// -------------------------------

// Return a pointer to the comment "name". The pointer cannot be freed.
char *IodeGetCmt(char *name)
{
    return(KCPTR(name));        // Ptr to SWAP mem. Do not free!
}

// Save a the comment cmt under the name "name".
int IodeSetCmt(char *name, char *cmt)
{
    int     pos;

    pos = K_add(KC_WS, name, cmt); 
    if(pos < 0) return(-1);
    return(0);
}


// --------------------------
// EQS MANIPULATION FUNCTIONS
// --------------------------

/**
 *  Retrieve an equation and its elements.
 *  
 *  @param [in]  name        char*   equation name
 *  @param [out] lec         char**  lec form (not allocated)
 *  @param [out] method      int*    estimation method
 *  @param [out] sample_from char*   buffer to store estimation period "from"
 *  @param [out] sample_to   char*   buffer to store estimation period "to"
 *  @param [out] blk         char**  equation block (not allocated)
 *  @param [out] instr       char**  list of instruments (not allocated)
 *  @param [out] tests       float*  buffer to store the (10) estimation tests
 *  @return 
 */
int IodeGetEqs(char *name, char**lec, int *method, char*sample_from, char* sample_to, char**blk, char**instr, float *tests)
{
    int     pos;
    SAMPLE  *smpl;

    pos = K_find(KE_WS, name);
    if(pos < 0) return(-1);

    *lec = KELEC(KE_WS, pos);
    // static  char met[6] = {"LZIGM"};
    *method = KEMETH(KE_WS, pos);
    *method = max(0, *method);
    *method = min(4, *method);
    smpl = &KESMPL(KE_WS, pos);
    PER_pertoa(&(smpl->s_p1), sample_from);
    PER_pertoa(&(smpl->s_p2), sample_to);
    *blk = KEBLK(KE_WS, pos);
    *instr = KEINSTR(KE_WS, pos);
    memcpy(tests, KETESTS(KE_WS, pos), sizeof(float) * EQS_NBTESTS);
    
    return(0);
}

// Retrieve the (non allocated) LEC form of the equation "name".
char *IodeGetEqsLec(char *name)
{
    int     pos;

    pos = K_find(KE_WS, name);
    if(pos < 0) return(NULL);

    return(KELEC(K_WS[K_EQS], pos));
}

// Change the LEC form of an equation.
int IodeSetEqs(char *name, char *eqlec)
{
    int     rc;

    rc = K_upd_eqs(name, eqlec, NULL, 'L', NULL, NULL, NULL, NULL, 0);
    return(rc);
}


// --------------------------
// IDT MANIPULATION FUNCTIONS
// --------------------------


// Return the (non allocated) pointer to an identity.
char *IodeGetIdt(char *name)
{
    return(KIPTR(name));        // Ptr to SWAP mem. Do not free!
}


// Set the LEC form of an identity.
int IodeSetIdt(char *name, char *idt)
{
    int     pos;

    pos = K_add(KI_WS, name, idt);
    if(pos < 0) return(-1);
    return(0);
}


// --------------------------
// LST MANIPULATION FUNCTIONS
// --------------------------

// Return the (non allocated) pointer to a list.
char *IodeGetLst(char *name)
{
    return(KLPTR(name));        // Ptr to SWAP mem. Do not free!
}


// Set a list value.
int IodeSetLst(char *name, char *lst)
{
    int     pos;

    pos = K_add(KL_WS, name, lst);
    if(pos < 0) return(-1);
    return(0);
}


// --------------------------
// SCL MANIPULATION FUNCTIONS
// --------------------------

// Change the values of an existing scalar. Return -1 if the scalar does not exist.
int IodeGetScl(char *name, double* value, double *relax, double *std_err)
{
    int     pos;
    SCL     *scl;

    pos = K_find(K_WS[K_SCL], name);
    if(pos < 0) return(-1);

    scl = KSVAL(K_WS[K_SCL], pos);
    *value = scl->val;
    *relax = scl->relax;
    *std_err = scl->std;
    return(0);
}

// Set the values of a scalar. The scalar is created if it does not exist.
int  IodeSetScl(char *name, double value, double relax, double std)
{
    int     pos;
    SCL     scl;

    scl.val = value;
    scl.relax = relax;
    scl.std = std;
    pos = K_add(KS_WS, name, &scl); 
    if(pos < 0) return(-1);
    return(0);
}


// --------------------------
// TBL MANIPULATION FUNCTIONS
// --------------------------

/**
 *  Compute a table on the GSAMPLE gsmpl and return a string containing the result.
 *  
 *  @param [in] name  char*     table name
 *  @param [in] gsmpl char*     GSAMPLE or null. If null, the GSAMPLE is the current WS sample.
 *  @return           char*     allocated string in the form "c11 \t c12 \t ... \n c21 \t c22 ..."
 */
char* IodeGetTbl(char *name, char *gsmpl)
{
    int     pos, nc, nl;
    char    *ptr;

    pos = K_find(K_WS[K_TBL], name);
    if(pos < 0) return("Not in WS");

    ptr = IodeDdeCreateTbl(pos, gsmpl, &nc, &nl, K_NBDEC);  
    SCR_OemToAnsi(ptr, ptr);
    return(ptr);
}

// Return a table (first) title in an not allocated string.
char* IodeGetTblTitle(char *name)
{
    int     pos;
    char    *ptr;
    TBL     *tbl;

    pos = K_find(K_WS[K_TBL], name);
    if(pos < 0 || (tbl = KTVAL(K_WS[K_TBL], pos)) == 0)
        return("Not in WS");

    /* returns only ptr to string, NO ALLOC is done */
    ptr = T_get_title(tbl);
    SCR_OemToAnsi(ptr, ptr);
    return(ptr);
}

// Return a table struct. 
TBL* IodeGetTblDefinition(char *name)
{
    int     pos;
    TBL     *tbl;

    if(name ==  NULL) pos = 0;
    else pos = K_find(K_WS[K_TBL], name);
    if(pos < 0 || (tbl = KTVAL(K_WS[K_TBL], pos)) == 0)
        return(NULL);
    else return(tbl);
}

// Load the var file filename and set its reference number to ref.
// Used in GSAMPLE eg: 2000Y1[1;2]
int IodeSetTblFile(int ref, char *filename)
{
    return(K_load_RWS(ref, filename));
}


// --------------------------
// VAR MANIPULATION FUNCTIONS
// --------------------------

/**
 *  Get the value of the variable name at position t where t is the position in the KV_WS SAMPLE.
 *  The returned is the result of a calculation based on the value of mode.
 *  
 *  @param [in] name char*  variable name
 *  @param [in] t    int    position in the sample
 *  @param [in] mode int    see below
 *  @return          double  depends on the param mode (can be L_NAN if the operation is impossible):
 *                               K_LEVEL : no modification    x[t]
 *                               K_DIFF  : diff on one period (x[t]-x[t-1])
 *                               K_DIFFY : diff on one year   (x[t]-x[t-{nb sub periods}])
 *                               K_GRT   : grt on one period  (x[t]/x[t-1] - 1)*100
 *                               K_GRTY  : grt on one year    (x[t]/x[t-{nb sub periods}] - 1) * 100
 */
double IodeGetVarT(char *name, int t, int mode)
{
    int     pos;
    double  value;
    SAMPLE  *smpl;

    smpl = KSMPL(KV_WS);
    if(t > smpl->s_nb) return(L_NAN);

    pos = K_find(K_WS[K_VAR], name);
    if(pos < 0) return(L_NAN);

    value = KV_get(K_WS[K_VAR], pos, t, mode);
    return(value);
}


/**
 *  Set the value of the variable name at position t, possibly after recalulation based on value and mode.
 *  
 *  @param [in] name    char*   variable name
 *  @param [in] t       int     position in the sample
 *  @param [in] mode    int     one of the defines below. x is the new value of name[t]
 *                                  K_LEVEL : no modification    x[t] = new
 *                                  K_DIFF  : diff on one period x[t] = x[t-1] + new
 *                                  K_DIFFY : diff on one year   x[t] = x[t-{nb sub periods}] + new
 *                                  K_GRT   : grt on one period  x[t] = (1 + 0.01*new) * x[t-1]
 *                                  K_GRTY  : grt on one year    x[t] = (1 + 0.01*new) * x[t-{nb sub periods}] 
 *  @param [in] value   double  value on which, in combination with mode, the new value is calculated
 *                              
 */
int IodeSetVarT(char *name, int t, int mode, double value)
{
    int     pos;
    SAMPLE  *smpl;

    smpl = KSMPL(KV_WS);
    if(t > smpl->s_nb) return(-1);

    pos = K_find(KV_WS, name);
    if(pos < 0) return(-1);

    KV_set(K_WS[K_VAR], pos, t, mode, value);
    return(0);
}


/**
 *  Returns a pointer to the first value of the VAR name. 
 *  
 *  @param [in]  name   char*   variable name   
 *  @param [out] lg     int     sample size
 *  @return             double* pointer to the VAR name
 */
double *IodeGetVector(char *name, int *lg)
{
    int     pos;
    double  *value = NULL;
    SAMPLE  *smpl;

    smpl = KSMPL(KV_WS);
    *lg = smpl->s_nb;

    pos = K_find(K_WS[K_VAR], name);
    if(pos >= 0) {
        value = KVVAL(K_WS[K_VAR], pos, 0);
    }
    return(value);
}


/**
 *  In the context of copying data between IODE (the current KV_WS) and python (LArray, pandas, numpy...), 
 *  determines the positions to copy from (python object), where to copy to (KV_WS) and the length 
 *  of the intersection between the python object and KV_WS.
 *  
 *  Schematically : 
 *      KV_WS[var, wspos + i] = PyObject[var, py_pos + i] for i = 0..py_lg
 *  
 *  @param [in] str_pyper_from  char*   start period in the python object
 *  @param [in] str_pyper_to    char*   end period in the python object
 *  @param [in] py_pos          int*    position in the python object of the first element to copy 
 *  @param [in] ws_pos          int*    position in KV_WS of the first element 
 *  @param [in] py_lg           int*    py_lg
 *  @return                     int     
 */
int IodeCalcSamplePosition(char *str_pyper_from, char* str_pyper_to, int *py_pos, int *ws_pos, int *py_lg)
{
    SAMPLE          *smpl = KSMPL(KV_WS);
    PERIOD          *per_la_from, 
                    *per_la_to;
    static int      ws_lg = 0;
    
    // Compute where to copy la_values
    // ws_var[ws_pos + i] = la_values[py_pos + i] for i=0..py_lg
    per_la_from = PER_atoper(str_pyper_from);
    per_la_to   = PER_atoper(str_pyper_to);
    
    // idea: copy py_lg values from la[py_pos] to ws[ws_pos] 
    
    // Calc position and length as if ws starts before la
    *ws_pos = PER_diff_per(per_la_from, &(smpl->s_p1));
    *py_pos = 0;                                
    *py_lg = 1 + PER_diff_per(per_la_to, per_la_from);
    
    // if la starts before ws, start at -ws_pos and decrease py_lg
    if(*ws_pos < 0) {
        *py_pos = -*ws_pos;
        *ws_pos = 0;
        *py_lg -= *py_pos;
    }    
    
    // if la_to > ws_to, decrease py_lg
    if(PER_diff_per(per_la_to, &(smpl->s_p2)) > 0){
        *py_lg -= PER_diff_per(per_la_to, &(smpl->s_p2));
    }
   
    //printf("py_pos:%d - ws_pos:%d - py_lg:%d\n", *py_pos, *ws_pos, *py_lg);
    return(*py_lg);
}


/**
 *  Set the values of a subset of a variable.
 *  
 *    ws_var[ws_pos + i] = la_values[la_pos + i] for i=0..la_lg - 1
 *  
 *  @param [in] la_name     char*   name of the variable to set
 *  @param [in] la_values   double* pointer to the first element to copy into the VAR la_name
 *  @param [in] la_pos      int     position in la_values of the first value to copy into the variable
 *  @param [in] ws_pos      int     position in the variable la_name where the first la_values[la_pos] must be copied
 *  @param [in] la_lg       int     number of values to copy
 *  @return                 int     position in KV_WS of the new variable or -1 on error
 */
int IodeSetVector(char *la_name, double *la_values, int la_pos, int ws_pos, int la_lg)
{
    int             i, pos;
    SAMPLE          *smpl = KSMPL(KV_WS);
    static int      ws_lg = 0;
    static double*  ws_var = NULL; // expandable static buffer to avoid to repeated allocations
    
    // Allocate static buffer if not yet done or if to small for a full series
    if(ws_lg < smpl->s_nb) {
        SW_nfree(ws_var);
        ws_lg = smpl->s_nb;
        ws_var = (double *) SW_nalloc(ws_lg * sizeof(double));
    }
    
    pos = K_find(KV_WS, la_name);
    
    // If series does not exist, set all values to L_NAN
    if(pos < 0) {
        for(i = 0; i < ws_lg; i++) ws_var[i] = L_NAN;
    }
    
    // Copy la_lg values to ws_var
    if(la_lg <= 0) la_lg = smpl->s_nb;
    for(i = 0; i < la_lg; i++) {
        ws_var[ws_pos + i] = la_values[la_pos + i];
        //printf("%s[%d] - %lf\n", la_name, i, la_values[la_pos + i]);
    }
    
    // Save series in KV_WS
    pos = K_add(KV_WS, la_name, ws_var, &ws_lg);
    return(pos);
}


// --------------------
// ESTIMATION FUNCTIONS
// --------------------

// Estimate an equation of a given sample. 
// Only defined to standardize the API functions
// TODO: add all estimation parameters
int IodeEstimate(char* veqs, char* afrom, char* ato)
{
    return(KE_estim(veqs, afrom, ato));
}    


// --------------------
// SIMULATION FUNCTIONS
// --------------------

/**
 *  Simulate of model.
 *  
 *  @param [in] per_from      char*     per_from
 *  @param [in] per_to        char*     per_to
 *  @param [in] eqs_list      char*     eqs_list
 *  @param [in] endo_exo_list char*     endo_exo_list
 *  @param [in] eps           double    eps
 *  @param [in] relax         double    relax
 *  @param [in] maxit         int       maxit 
 *  @param [in] init_values   int       init_values
 *  @param [in] sort_algo     int       sort_algo
 *  @param [in] nb_passes     int       nb_passes
 *  @param [in] debug         int       debug
 *  @param [in] newton_eps    double    newton_eps
 *  @param [in] newton_maxit  int       newton_maxit
 *  @param [in] newton_debug  int       newton_debug      
 *  @return                   int       0 in success, -1 on error
 */
int IodeModelSimulate(char *per_from, char *per_to, char *eqs_list, char *endo_exo_list,
                 double eps, double relax, int maxit, 
                 int init_values, int sort_algo, int nb_passes, int debug, 
                 double newton_eps, int newton_maxit, int newton_debug)
{
    char    **eqs = NULL, **endo_exo = NULL;
    SAMPLE  *smpl;
    int     rc;
    double  ksim_eps            = KSIM_EPS         ;
    double  ksim_relax          = KSIM_RELAX       ;
    int     ksim_maxit          = KSIM_MAXIT       ;
    int     ksim_start          = KSIM_START       ;
    int     ksim_sort           = KSIM_SORT        ;
    int     ksim_passes         = KSIM_PASSES      ;
    int     ksim_debug          = KSIM_DEBUG       ;
    double  ksim_newton_eps     = KSIM_NEWTON_EPS  ;
    int     ksim_newton_maxit   = KSIM_NEWTON_MAXIT;
    int     ksim_newton_debug   = KSIM_NEWTON_DEBUG;

    KSIM_EPS   = eps;
    KSIM_RELAX = relax;
    KSIM_MAXIT = maxit;
    KSIM_START = init_values;
    KSIM_SORT = sort_algo;
    KSIM_PASSES = nb_passes;
    KSIM_DEBUG = debug;
    KSIM_NEWTON_EPS = newton_eps;
    KSIM_NEWTON_MAXIT = newton_maxit;
    KSIM_NEWTON_DEBUG = newton_debug;

    smpl = PER_atosmpl(per_from, per_to);
    if(smpl == NULL) return(-1);
    if(eqs_list) {
       eqs = SCR_vtoms(eqs_list," ,;");
    }   

    if(endo_exo_list) {
       endo_exo = SCR_vtoms(endo_exo_list," ,;");
    }   


    rc = K_simul(KE_WS, KV_WS, KS_WS, smpl, endo_exo, eqs);
    
    SCR_free(smpl);
    SCR_free_tbl(eqs);
    SCR_free_tbl(endo_exo);

    // Restore old values
    KSIM_EPS           = ksim_eps         ;
    KSIM_RELAX         = ksim_relax       ;
    KSIM_MAXIT         = ksim_maxit       ;
    KSIM_START         = ksim_start       ;
    KSIM_SORT          = ksim_sort        ;
    KSIM_PASSES        = ksim_passes      ;
    KSIM_DEBUG         = ksim_debug       ;
    KSIM_NEWTON_EPS    = ksim_newton_eps  ;
    KSIM_NEWTON_MAXIT  = ksim_newton_maxit;
    KSIM_NEWTON_DEBUG  = ksim_newton_debug;

    return(rc);
}


// ----------------
// REPORT FUNCTIONS
// ----------------

/**
 *  Execute a report with optionnal parameters.
 *  
 *  @param [in] filename    char*   retpor filename
 *  @param [in] args        char**  table of args or NULL
 *  @return                 int     return code of the report execution
 */
int IodeExecArgs(char *filename, char **args)
{
    int     rc;
    char    buf[10240], *targs;

    rc = B_PrintDest("dummy D");
    SCR_strlcpy(buf, filename, 240);
    K_set_ext(buf, buf, K_REP);

    if(args) {
        targs = SCR_mtov(args, ' ');
        if(strlen(targs) > 10000) targs[10000] = 0;
        strcat(buf, " ");
        strcat(buf, targs);
        SCR_free(targs);
    }

    rc = B_ReportExec(buf);
    W_close();

    return(rc);
}


/**
 *  Execute a report with no parameters.
 *  
 *  @return         int     return code of the report execution
 */
int IodeExec(char *filename)
{
    return(IodeExecArgs(filename, NULL));
}



// -------------
// LEC EXECUTION
// -------------

/**
 *  Compile and link a LEC expression with the KV_WS and KS_WS.
 *  
 *  @param [in] lec     char*   lec expression
 *  @return             CLEC*   compiled and linked lec or NULL on error.
 */
static CLEC* IodeLinkLec(char* lec)
{
    CLEC*   clec;

    clec = L_cc(lec);
    if(clec == NULL) return(clec);
    
    if(L_link(KV_WS, KS_WS, clec)) {
        SCR_free(clec);
        return(NULL);
    }    
    return(clec);
}


/**
 *  Calc a LEC expression in t.
 *  
 *  @param [in] lec     char*   lec expression 
 *  @param [in] t       int     position in the sample to calculate
 *  @return             double  value of lec in t or L_NAN on error
 */
double IodeExecLecT(char* lec, int t)
{
    CLEC*   clec;
    double  res;

    clec = IodeLinkLec(lec);
    if(clec == NULL) return(L_NAN);
    res = L_exec(KV_WS, KS_WS, clec, t);
    SCR_free(clec);
    return(res);
}

/**
 *  Calculate a LEC expression on the full KV_WS sample.
 *  
 *  @param [in] lec     char*       lec expression 
 *  @return             double*     allocated vector containing the calculated values
 */

double *IodeExecLec(char* lec)
{
    CLEC*   clec;
    double  *res;
    int     nb, t;
    
    nb = KSMPL(KV_WS)->s_nb;
    if(nb == 0) return(NULL);
    
    clec = IodeLinkLec(lec);
    if(clec == NULL) return(NULL);

    res = (double*) SW_nalloc(sizeof(double) * nb);
    for(t = 0; t < nb; t++) 
        res[t] = L_exec(KV_WS, KS_WS, clec, t);
    
    SCR_free(clec);
    return(res);
}


// ----------------------------------------
// GRAPH MANIPULATION FUNCTIONS (see GB ?)
// ----------------------------------------

int IodeGetChart(char *name, char *gsmpl)
{
    int     pos;
    TBL     *tbl;

    pos = K_find(K_WS[K_TBL], name);
    if(pos < 0 || (tbl = KTVAL(K_WS[K_TBL], pos)) == 0)
        return(-1);

    return(APIPrepareChart(tbl, gsmpl));
}

int IodeFreeChart(int hdl)
{
    return(APIChartFree(hdl));
}

int IodeChartNl(int hdl)
{
    return(APIChartNl(hdl));
}

int IodeChartNc(int hdl)
{
    return(APIChartNc(hdl));
}

char    IodeChartType(int hdl, int i)
{
    return(APIChartType(hdl, i));
}

int    IodeChartAxis(int hdl, int i)
{
    return(APIChartAxis(hdl, i));
}

char    *IodeChartTitle(int hdl, int i)
{
    return(APIChartTitle(hdl, i));
}

double  *IodeChartData(int hdl, int i)
{
    return(APIChartData(hdl, i));
}


// ---------------
// OUTPUT MESSAGES
// ---------------
   
// Suppress all messages from the A2M interpretor and from the IODE functions.
void IodeSuppressMsgs()
{
    kmsg_toggle(0);
    A2mMessage_toggle(0);
    //IodeSetMsgs(0);
}

// Reset the messages from the A2M interpretor and from the IODE functions.
void IodeResetMsgs()
{
    kmsg_toggle(1);
    A2mMessage_toggle(1);
}

// --------------
// MISC FUNCTIONS
// --------------

/**
 *  Define the number of decimals in the tables and variables to be printed.
 *  
 *  @param [in] nbdec   int   new nb of decimals
 */
int IodeSetNbDec(int nbdec)
{
    K_NBDEC = nbdec; // JMP 18-04-2022
    return(0);
}

/**
 *  Returns the number of decimals currently set for the printing of tables and variables.
 *  
 *  @param [in] nbdec   int   new nb of decimals
 */
int IodeGetNbDec()
{
    return(K_NBDEC);    // JMP 18-04-2022
}




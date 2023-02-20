/**
 *  @header4iode
 * 
 *  IODE API developed for iode.pyx (Cython implementation of some IODE functions).
 *  
 *    
 *  List of functions
 *  -----------------
 *  
 */
 
#include "iode.h"
#include "iodeapi.h"

/**
 *  Initialise an IODE session.
 *  
 *  @return int 0 always
 */
int IodeInit()
{
    RPF_ChronoReset();
    SW_MIN_MEM = 120 * 1024L;
    SW_MIN_SEGS = 2;
    SW_init(1);
    K_init_ws(0);

    K_LANG = 0;     
    K_NBDEC = -1;   
    A_SEPS = " ,;\n\t\r";
    A_NO_EXPANDSTAR_DFT = 1; // JMP 12-11-2012 -> suppress default filename wildcard behavior

    IODE_assign_super_API(); // JMP 15/2/2023 -> default fns automatically assigned
    
    return(0);
}


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

char *IodeVersion()
{
   return(IODE_VERSION);
}



// ------------
// WS Functions
// ------------

//int  IodeLoadVar(char *name)
//{
//    return(IodeLoad(name, K_VAR));
//}

int  IodeLoad(char *name, int type)
{
    KDB     *kdb;

    if(B_WsLoad(name, type) < 0) return(-1);
    else {
        kdb = K_WS[type];
        return(KNB(kdb));
    }
}

//int  IodeSaveVar(char *name)
//{
//    return(IodeSave(name, K_VAR));
//}

int  IodeSave(char *name, int type)
{
    return(B_WsSave(name, type));
}

int IodeClearWs(int type)
{
    if(type >= K_CMT && type <= K_VAR) 
        K_clear(K_WS[type]);
    return(0);
}

int IodeClearAll()
{
    int type;

    for(type = K_CMT; type <= K_VAR; type++)
        K_clear(K_WS[type]);
    
    return(0);
}

char **IodeContents(char *pattern, int type)
{
    return(K_grep(K_WS[type], pattern, 0, 1, 0, 0, '*'));
}


//char *IodeContentsStr(char *pattern, int type)
//{
//	char *str;
//	char **res = K_grep(K_WS[type], pattern, 0, 1, 0, 0, '*');
//    
//	if(res != NULL)
//		str = SCR_mtov(res, ',');
//    SCR_free_tbl(res);
//    return(str);
//}


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


//char *IodeGetPeriodAsString(int t)
//{
//    SAMPLE  *smpl;
//    PERIOD  per;
//
//    if(!IodeIsSampleSet()) return(NULL);
//    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
//    memcpy(&per, PER_addper(&smpl->s_p1, t), sizeof(PERIOD));;
//    return(PER_pertoa(&per, 0));
//}
//

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
 *  @return  char** allocated table of char* 
 *  
 *  TODO: call IodeCreateSampleAsPeriods() 
 */

char **IodeGetSampleAsPeriods() 
{
    SAMPLE  *smpl = KSMPL(KV_WS);
    char    **periods, *aper;
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
 *  @return               char**    
 */
char **IodeCreateSampleAsPeriods(char* aper_from, char* aper_to) 
{
    SAMPLE  *smpl;
    char    **periods, *aper;
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


// Sample for yearly series only => Obsolete ?

// Returns a sample as a list of double
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

//int IodeGetSampleFrom()
//{
//    SAMPLE *ksmpl = KSMPL(KV_WS);
//    
//    return(ksmpl->s_p1.p_y);
//}
//
//int IodeGetSampleTo()
//{
//    SAMPLE *ksmpl = KSMPL(KV_WS);
//    
//    return(ksmpl->s_p2.p_y);
//}
//

//int IodeGett(char *period)
//{
//    int     t;
//    SAMPLE  *smpl;
//    PERIOD  *per, p;
//
//    smpl = (SAMPLE *) KDATA(K_WS[K_VAR]);
//    p = smpl->s_p1;
//    per = PER_atoper(period);
//    if(per == 0) return(-1);
//
//    t = PER_diff_per(per, &p);
//    SW_nfree(per);
//    return(t);
//}
    
//int IodeGetSampleBounds(int from, int to, int *start1, int *start2)
//{
//        if(PER_common_smpl(ksmpl, nsmpl, &smpl) < 0) return(-1);
//        if(smpl.s_nb > 0) {
//            start1 = PER_diff_per(&(smpl.s_p1), &(ksmpl->s_p1));
//            start2 = PER_diff_per(&(smpl.s_p1), &(nsmpl->s_p1));
//        }
//    }
//
//    memcpy(ksmpl, nsmpl, sizeof(SAMPLE));
//    for(i = 0 ; i < KNB(kdb); i++) {
//        new_val = KV_alloc_var(ksmpl->s_nb);
//        ptr = SW_getptr(new_val);
//        if(KSOVAL(kdb, i) != 0) {
//            if(smpl.s_nb > 0)
//                memcpy((IODE_REAL *)(P_get_ptr(ptr, 0)) + start2,
//                       KVVAL(kdb, i, start1),
//                       sizeof(IODE_REAL) * smpl.s_nb);
//            SW_free(KSOVAL(kdb, i));
//        }
//        KSOVAL(kdb, i) = new_val;
//    }
//    return(0);
//}    



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



int IodeDeleteObj(char* obj_name, int obj_type)
{
    return(K_del_by_name(K_WS[obj_type], obj_name));
}


// -------------------------------
// COMMENTS MANIPULATION FUNCTIONS
// -------------------------------


char *IodeGetCmt(char *name)
{
    return(KCPTR(name));        // Ptr to SWAP mem. Do not free!
}

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

int IodeGetEqs(char *name, char**lec, int *method, char*sample_from, char* sample_to, char**blk, char**instr, float *tests)
{
    int     pos;
    SAMPLE  *smpl;
    char    txt[80];

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

char *IodeGetEqsLec(char *name)
{
    int     pos;

    pos = K_find(KE_WS, name);
    if(pos < 0) return(NULL);

    return(KELEC(K_WS[K_EQS], pos));
}


int IodeSetEqs(char *name, char *eqlec)
{
    int     rc;

    rc = K_upd_eqs(name, eqlec, NULL, 'L', NULL, NULL, NULL, NULL, 0);
    return(rc);
}


// --------------------------
// IDT MANIPULATION FUNCTIONS
// --------------------------

char *IodeGetIdt(char *name)
{
    return(KIPTR(name));        // Ptr to SWAP mem. Do not free!
}

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

char *IodeGetLst(char *name)
{
    return(KLPTR(name));        // Ptr to SWAP mem. Do not free!
}

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

//double  *IodeGetScls(char *name)
//{
//    int     pos;
//    SCL     *scl;
//
//    pos = K_find(K_WS[K_SCL], name);
//    if(pos < 0) return(0);
//
//    scl = KSVAL(K_WS[K_SCL], pos);
//    return((double *) scl);
//}

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

char* IodeGetTbl(char *name, char *gsmpl)
{
    int     pos, nc, nl;
    char    *ptr;

    pos = K_find(K_WS[K_TBL], name);
    if(pos < 0) return("Not in WS");

    //ptr = IodeDdeCreateTbl(pos, gsmpl, &nc, &nl, B_NBDEC); // JMP 18-04-2022
    ptr = IodeDdeCreateTbl(pos, gsmpl, &nc, &nl, K_NBDEC);   // JMP 18-04-2022
    SCR_OemToAnsi(ptr, ptr);
    return(ptr);
}

char* IodeGetTblTitle(char *name)
{
    int     pos;
    char    *ptr;
    TBL     *tbl;

    pos = K_find(K_WS[K_TBL], name);
    if(pos < 0 || (tbl = KTVAL(K_WS[K_TBL], pos)) == 0)
        return("Not in WS");

    /* returns anly ptr to string, NO ALLOC is done */
    ptr = T_get_title(tbl);
    SCR_OemToAnsi(ptr, ptr);
    return(ptr);
}

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

int IodeSetTblFile(int ref, char *filename)
{
    return(K_load_RWS(ref, filename));
}





// --------------------------
// VAR MANIPULATION FUNCTIONS
// --------------------------

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
 *      KV_WS[var, wspos + i] = LA[var, la_pos + i] for i = 0..la_lg
 *  
 *  @param [in] str_la_from char*   start period in the python object
 *  @param [in] str_la_to   char*   end period in the python object
 *  @param [in] la_pos      int*    position in the python object of the first element to copy 
 *  @param [in] ws_pos      int*    position in KV_WS of the first element 
 *  @param [in] la_lg       int*    la_lg
 *  @return                 int     
 */
 
int IodeCalcSamplePosition(char *str_la_from, char* str_la_to, int *la_pos, int *ws_pos, int *la_lg)
{
    int             pos, i;
    SAMPLE          *smpl = KSMPL(KV_WS);
    PERIOD          *per_la_from, 
                    *per_la_to;
    static int      ws_lg = 0;
    
    // Compute where to copy la_values
    // ws_var[ws_pos + i] = la_values[la_pos + i] for i=0..la_lg
    per_la_from = PER_atoper(str_la_from);
    per_la_to   = PER_atoper(str_la_to);
    
    // idea: copy la_lg values from la[la_pos] to ws[ws_pos] 
    
    // Calc position and length as if ws starts before la
    *ws_pos = PER_diff_per(per_la_from, &(smpl->s_p1));
    *la_pos = 0;                                
    *la_lg = 1 + PER_diff_per(per_la_to, per_la_from);
    
    // if la starts before ws, start at -ws_pos and decrease la_lg
    if(*ws_pos < 0) {
        *la_pos = -*ws_pos;
        *ws_pos = 0;
        *la_lg -= *la_pos;
    }    
    
    // if la_to > ws_to, decrease la_lg
    if(PER_diff_per(per_la_to, &(smpl->s_p2)) > 0){
        *la_lg -= PER_diff_per(per_la_to, &(smpl->s_p2));
    }
   
    //printf("la_pos:%d - ws_pos:%d - la_lg:%d\n", *la_pos, *ws_pos, *la_lg);
    return(*la_lg);
}


//int IodeSetVector_v1(char *la_name, double *la_values, int la_year_from, int la_year_to)
//{
//    int             pos, i, ws_pos, la_pos, la_lg;
//    SAMPLE          *smpl = KSMPL(KV_WS);
//    static int      ws_lg = 0;
//    static double*  ws_var = NULL;
//    
//    // Allocate static buffer if not yet done or if to small for a full series
//    if(ws_lg < smpl->s_nb) {
//        SW_nfree(ws_var);
//        ws_lg = smpl->s_nb;
//        ws_var = (double *) SW_nalloc(ws_lg * sizeof(double));
//    }
//    
//    // Compute where to copy la_values: 
//    // ws_var[ws_pos + i] = la_values[la_pos + i] for i=0..la_lg
//    ws_pos = la_year_from - smpl->s_p1.p_y;    
//    la_pos = 0;                                
//    la_lg = 1 + la_year_to - la_year_from;
//
//    if(ws_pos < 0) {
//        la_pos = -ws_pos;
//        ws_pos = 0;
//        la_lg -= la_pos;
//    }    
//    
//    if(la_year_to > smpl->s_p2.p_y) {
//        la_lg -= la_year_to - smpl->s_p2.p_y;
//    }
//   
//    for(i = 0; i < ws_lg; i++) ws_var[i] = L_NAN;
//    for(i = 0; i < la_lg; i++) ws_var[ws_pos + i] = la_values[la_pos + i];
//    
//    // printf("Saving %s...", la_name);
//    pos = K_add(KV_WS, la_name, ws_var, &ws_lg);
//    //if (pos < 0 ) {
//    //    printf("Error\n");
//	//}
//    //else {
//    //    printf("Ok.\n");
//    //}    
//    return(pos);
//}

    
//int IodeSetVector_v2(char *la_name, double *la_values, char *str_from, char* str_to)
//{
//    int             pos, i, ws_pos, la_pos, la_lg;
//    SAMPLE          *smpl = KSMPL(KV_WS);
//    PERIOD          *per_from, *per_to;
//    static int      ws_lg = 0;
//    static double*  ws_var = NULL;
//    
//    // Allocate static buffer if not yet done or if to small for a full series
//    if(ws_lg < smpl->s_nb) {
//        SW_nfree(ws_var);
//        ws_lg = smpl->s_nb;
//        ws_var = (double *) SW_nalloc(ws_lg * sizeof(double));
//    }
//    
//    // Compute where to copy la_values: 
//    // ws_var[ws_pos + i] = la_values[la_pos + i] for i=0..la_lg
//    per_from = PER_atoper(str_from);
//    per_to   = PER_atoper(str_to);
//    ws_pos = PER_diff_per(per_from, &(smpl->s_p1));
//    la_pos = 0;                                
//    la_lg = 1 + PER_diff_per(per_to, per_from);
//
//    if(ws_pos < 0) {
//        la_pos = -ws_pos;
//        ws_pos = 0;
//        la_lg -= la_pos;
//    }    
//    
//    if(PER_diff_per(per_from, &(smpl->s_p2)) > 0){
//        la_lg -= PER_diff_per(per_to, &(smpl->s_p2));
//    }
//   
//    for(i = 0; i < ws_lg; i++) ws_var[i] = L_NAN;
//    for(i = 0; i < la_lg; i++) ws_var[ws_pos + i] = la_values[la_pos + i];
//    
//    // printf("Saving %s...", la_name);
//    pos = K_add(KV_WS, la_name, ws_var, &ws_lg);
//    //if (pos < 0 ) {
//    //    printf("Error\n");
//	//}
//    //else {
//    //    printf("Ok.\n");
//    //}     
//    SW_nfree(per_from);
//    SW_nfree(per_to);
//    return(pos);
//}


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

// Only defined to standardize the API functions
int IodeEstimate(char* veqs, char* afrom, char* ato)
{
    return(KE_estim(veqs, afrom, ato));
}    

// --------------------
// SIMULATION FUNCTIONS
// --------------------

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

int IodeExec(char *filename)
{
    return(IodeExecArgs(filename, NULL));
    /*    int     rc;
        char    buf[1024];

        rc = B_PrintDest("dummy D");
        SCR_strlcpy(buf, filename, 1023);
        K_set_ext(buf, buf, K_REP);
        rc = B_ReportExec(buf);
        W_close();
        return(rc);
    */
}



// -------------
// LEC EXECUTION
// -------------

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
 *  Calc a LEC expression in t 
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
 *  Calc a LEC expression on the full sample
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
// GRAPH MANIPULATION FUNCTIONS (from GB ?)
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
   
void IodeSuppressMsgs()
{
    kmsg_toggle(0);
    A2mMessage_toggle(0);
    //IodeSetMsgs(0);
}

void IodeResetMsgs()
{
    kmsg_toggle(1);
    A2mMessage_toggle(1);
}

// --------------
// MISC FUNCTIONS
// --------------

int IodeSetNbDec(int nbdec)
{
    K_NBDEC = nbdec; // JMP 18-04-2022
    return(0);
}

int IodeGetNbDec()
{
    return(K_NBDEC);    // JMP 18-04-2022
}




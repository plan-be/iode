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
 *      void IodeAddErrorMsg(char* msg)                                  | Add a message in the error message stack
 *      void IodeDisplayErrorMsgs()                                      | Display the error messages accumulated during the previous function calls.
 *      void IodeClearErrorMsgs()                                        | Clear the stack of messages.
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
    extern int      SW_ACTIVE;      // JMP 07/06/2023           
    
    // Initialize chrono for report functions
    RPF_ChronoReset();      
    
    // Initialize scr4 SWAP memory management if not yet done // JMP 07/06/2023
    if(SW_ACTIVE == 0) {            // JMP 08/06/2023
        SW_MIN_MEM = 120 * 1024L;
        SW_MIN_SEGS = 2;
        SW_SEG_SIZE = B_IniReadNum("GENERAL", "SEGSIZE", 65500);
        if(SW_SEG_SIZE < 16384) SW_SEG_SIZE = 16384;            
    }
    
    //printf("SW_SEG_SIZE=%d\n", SW_SEG_SIZE);

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
    K_set_ext(buf, buf, FILE_REP);

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
 *  @return             double  value of lec in t or IODE_NAN on error
 */
double IodeExecLecT(char* lec, int t)
{
    CLEC*   clec;
    double  res;

    clec = IodeLinkLec(lec);
    if(clec == NULL) return(IODE_NAN);
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

    pos = K_find(K_WS[TABLES], name);
    if(pos < 0 || (tbl = KTVAL(K_WS[TABLES], pos)) == 0)
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


// Manage the messages generated via B_seterror() and B_seterrn() (see b_errors.c)  

/**
 *  Add a new error message to the stack of messages.
 *  
 *  @param [in] msg char* text of the message
 */
void IodeAddErrorMsg(char* msg)
{   
    B_add_error(msg);
}    

/**
 *  Display the error messages accumulated during the previous function calls.
 */
void IodeDisplayErrorMsgs()
{
    B_display_last_error();
}    

/**
 *  Clear the stack of messages.
 */
 void IodeClearErrorMsgs()
{
    B_clear_last_error();
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



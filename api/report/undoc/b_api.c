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
 *      int IodeInit()              | Initialise an IODE session.
 *      int IodeEnd()               | Terminate an IODE session.
 *   
 *   MESSAGES
 *  
 *      void IodeSuppressMsgs()     | Suppress all messages from the A2M interpretor and from the IODE functions.
 *      void IodeResetMsgs()        | Reset the messages from the A2M interpretor and from the IODE functions.
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

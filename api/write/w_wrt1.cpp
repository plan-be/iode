/**
 *  @header4iode
 *
 *  Shortcuts to A2M commands.
 *  
 *  List of functions
 *  -----------------
 *      void W_print_enum(int n)             Starts an A2M enum paragraph (enum_*) of level n
 *      void W_print_cmd(int n)              Sends commands to A2M to start a paragraph containing code 
 *      void W_print_par(int n)              Starts an A2M normal paragraph (par_*) of level n
 *      void W_print_tit(int n)              Starts an A2M title paragraph (tit_*) of level n
 *      void W_print_pg_header(char* arg)    Sends the A2M command to change the page header
 *      void W_print_pg_footer(char* arg)    Sends the A2M command to change the page footer
 *      void W_print_rtf_topic(char* arg)    Sends the A2M command to set the next help topic
 *      void W_print_tb(char* title, int nc) Starts a new table in A2M format
 */

#include "api/print/print.h" 
#include "api/write/write.h"


/**
 *  Starts an A2M enum paragraph (enum_*) of level n (only the next paragraph is changed). 
 *  
 *  @param [in]     int     n   enum level 
 */
void W_print_enum(int n)
{
    W_printf(".par1 enum_%d\n", n);
}


/**
 *  Sends commands to A2M to start a paragraph containing code:
 *      - starts an A2M cmd paragraph (cmd_*) of level n 
 *      - sends the A2M command bl_on to keep spaces in the result
 *      - sends the A2M command lf_on to keep CRLF in the result
 * 
 *  If n is << 0, these commands are reset to their standard values:
 *      - starts an A2M par_1 paragraph 
 *      - sends the A2M command bl_off to suppress multiple spaces in the result
 *      - sends the A2M command lf_off to suppress multiple CRLF in the result
 *    
 *  @param [in]     int     n   cmd level (>>= 0) => cmd_n
 *                              -1 to reset lf_off, bl_off and paragraph to par_1.
 *  
 */
 
void W_print_cmd(int n)                       
{
    /* JMP 13-07-96 */
    if(n >= 0)
        W_printf(".lf_on\n.bl_on\n.par cmd_%d\n", n);
    else
        W_printf(".lf_off\n.bl_off\n.par par_1\n");
}


/**
 *  Starts an A2M normal paragraph (par_*) of level n.
 *  
 *  @param [in]     int     n   enum level 
 */
void W_print_par(int n)
{
    W_printf(".par1 par_%d\n", n);
}


/**
 *  Starts an A2M title paragraph (tit_*) of level n.
 *  
 *  @param [in]     int     n   enum level 
 */
void W_print_tit(int n)
{
    W_printf(".par1 tit_%d\n", n);
}

/**
 *  Sends the A2M command to change the page header.
 *  
 *  Each occurence of %d in arg will be replaced by the page number.
 *  
 *  @param [in] char*   arg     new page header
 */
void W_print_pg_header(char* arg)
{
    SCR_free(A2M_PGHEAD);
    A2M_PGHEAD = SCR_stracpy((unsigned char*) arg);
    SCR_strip(A2M_PGHEAD);
    W_printf(".pghead %s\n", A2M_PGHEAD);
}


/**
 *  Sends the A2M command to change the page footer.
 *  
 *  Each occurence of %d in arg will be replaced by the page number.
 *  
 *  @param [in] char*   arg     new page footer
 */
void W_print_pg_footer(char* arg)
{
    SCR_free(A2M_PGFOOT);
    A2M_PGFOOT = SCR_stracpy((unsigned char*) arg);
    SCR_strip(A2M_PGFOOT);
    //BGUI_PrintPageFooter();
    W_printf(".pgfoot %s\n", A2M_PGFOOT); 
}


/**
 *  Sends the A2M command to set the next help topic. 
 *  Only used for generating an HTML Help (.chm file) or RTF Help (Windows .hlp file).
 *   
 *  @param [in] char*   arg     title of the topic
 */

void W_print_rtf_topic(char* arg)
{
    if(arg) {
        W_printf(".topic %d %d %s\n", KT_CUR_TOPIC++, KT_CUR_LEVEL, arg);
        if(W_type == A2M_DESTRTF && W_rtfhelp) W_printf(".par1 tit_%d\n%s\n\n", KT_CUR_LEVEL, arg);
        // if(W_type == A2M_DESTHTML && W_htmlhelp) W_printf( ".par1 tit_%d\n%s\n\n", KT_CUR_LEVEL, arg); /* JMP 06-01-02 */
    }
}


/**
 *  Starts a new table in A2M format.
 *  
 *  @param [in] char*   title  table title
 *  @param [in] int     nc     number of columns
 */
void W_print_tb(char* title, int nc)
{
    W_printf(".tb %d\n", nc);
    W_printf(".sep %c\n", A2M_SEPCH);
    W_printf(".ttitle %s\n", title);
}

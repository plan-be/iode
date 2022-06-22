/**
 *  @header4iode
 *
 *  Shortcuts to A2M commands.
 *  
 *  List of functions
 *  -----------------
 *      void W_print_enum(int n)    Starts an A2M enum paragraph (enum_*) of level n.
 *      void W_print_cmd(int n)     Sends commands to A2M to start a paragraph containing code 
 *      void W_print_par(int n)     Starts an A2M normal paragraph (par_*) of level n.
 *      void W_print_tit(int n)     Starts an A2M title paragraph (tit_*) of level n.
 */

#include "iode.h"


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


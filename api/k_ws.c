/**
 * @header4iode
 * 
 * Variables and functions for initializing and cleaning up the "in memory" workspaces.
 *
 * Globals
 * -------
 *    - K_WS = table with pointers to the 7 KDB in memory, 1 per object type (CEILSTV)
 *      - K_WS[0] : C=comments 
 *      - K_WS[1] : E=equations
 *      - ...
 *  
 *    - K_RWS = table of max 5 KDB* per object type, used for ws comparison, for printing...
 *      Only used for Vars at the moment (print vars, print tables with comparison)
 *      - K_RWS[6][0] = first WS of VARS for comparison (ws)  
 *      - K_RWS[6][1] = second WS of VARS for comparison (file 1)
 *      - ...
 *    - K_PWS =  table of "current" K_RWS number. Set to 0 and never used (yet).
 *
 * 
 * Functions
 * ---------
 *      - void K_init_ws(int ws)        Initialises the "in mem" KDB structures and optionnaly loads the ws.* files
 *      - void K_end_ws(int ws)         Deletes the current workspaces defined in K_WS[] and their content after having optionnaly 
 *                                      saved their content in ws.* files.
 */
 
#include "iode.h"

/* Gobals */
KDB     *K_WS[7];                           // Current workspaces
KDB     *K_RWS[7][5];                       // Currently loaded workspaces (for printing and identity execution)
int     K_PWS[7] = { 0, 0, 0, 0, 0, 0, 0 }; // ??? TODO: check if still in use

//extern  int K_AUTO_WS;

/**
 *  @brief Initialises the "in mem" KDB structures adn optionnaly loads the ws.* files. 
 *  
 *  If ws is no NULL, the files ws.*, if they are found in the current dir, are loaded as initial values for the in memory KDBs.
 *  If ws is NULL, the KDB are left empty.
 *  
 *  @param [in] ws  int     indicates if the files ws.ac, ws.ae..., ws.av must be loaded as initial values for the KDB in memory
 */
 
void K_init_ws(int ws)
{
    int     i;

    memset(K_RWS, 0, sizeof(K_RWS));
    for(i = 0 ; i < 7 ; i++) {
        K_WS[i] = K_RWS[i][0] = K_init_kdb(i, "ws");
        if(ws) K_cat(K_WS[i], "ws");
    }
}


/**
 *  @brief Deletes the current workspaces defined in K_WS[] and their content after having optionnaly saved their content in ws.* files.
 *  
 *  If ws is not null, saves first the KDBs in 7 files ws.cmt, ws.eqs...ws.var before cleaning up.
 *  
 *  @param [in] ws int  if 1, saves the KDB on disk in the file defined in KNAMEPTR(kdb).
 *  
 */
void K_end_ws(int ws)
{
    int i, j;

    for(i = 0; i < 7; i++) {
        if(ws) K_save_ws(K_WS[i]);
        for(j = 0; K_RWS[i][j]; j ++) {
            K_free(K_RWS[i][j]);
            K_RWS[i][j] = 0; // JMP 9/11/2012
        }
    }
}

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
 *      - void K_init_ws(int ws)                    Initialises the "in mem" KDB structures and optionnaly loads the ws.* files
 *      - void K_end_ws(int ws)                     Deletes the current workspaces defined in K_WS[] and their content after having optionnaly 
 *                                                  saved their content in ws.* files.
 *      - int K_load_RWS(int ref, char *filename)   Load a VAR file for use in GSample (print tables and graphs)
 */
#include "api/b_errors.h" 
#include "api/objs/kdb.h" 
#include "api/objs/objs.h" 


/**
 *  @brief Initialises the "in mem" KDB structures adn optional loads the ws.* files. 
 *  
 *  If ws is not NULL, the files I_DEFAULT_FILENAME.* (default "ws.*"), 
 *      if they are found in the current dir, are loaded as initial values for the in memory KDBs.
 *  If ws is NULL, the KDB are left empty.
 *  
 *  @param [in] ws  int     indicates if the files ws.ac, ws.ae..., ws.av must be loaded as initial values for the KDB in memory
 */
 
void K_init_ws(int ws)
{
    int     i;

    memset(K_RWS, 0, sizeof(K_RWS));
    for(i = 0 ; i < 7 ; i++) {
        K_WS[i] = K_RWS[i][0] = K_init_kdb(i, I_DEFAULT_FILENAME);
        if(ws) K_cat(K_WS[i], I_DEFAULT_FILENAME);
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
        // K_WS[i] = NULL;  // TODO: check this: in the previous version, K_WS[] remained untouched, is it normal ?
    }
}


/**
 *  Loads a VAR file for use in GSample (print tables and graphs).
 *  Stores its KDB pointer in K_RWS[VARIABLES][ref - 1]. 
 *  If filename is NULL, frees K_RWS[VARIABLES][ref - 1].
 *  
 *  Example: the files [1], [2] and [3] used in a GSample "2000Y1[1,2,3]" are loaded:
 *      - [1] == current content of the VAR workspace
 *      - [2] by a call to K_load_RWS(2, "file2.var")
 *      - [3] by a call to K_load_RWS(3, "file3.var")
 *  
 *  @param [in] int     ref         reference number that will be used in GSample. 
 *                                  Restriction : 2 <= ref <= 5
 *  @param [in] char*   filename    file to load. 
 *                                  If NULL, frees K_RWS[VARIABLES][ref-1]
 *  @return     int                 0 on success, -1 on error (file not found, ref out of range...)
 *  
 */
 
int K_load_RWS(int ref, char *filename)
{
    KDB     *kdb = NULL;

    if(ref < 2 || ref > 5) {
        error_manager.append_error(std::string("Invalid Reference number. Must be between 2 and 5. Got") +
                                   std::to_string(ref));
        return(-1);
    }
    
    if(filename == NULL) {
        K_free(K_RWS[VARIABLES][ref - 1]);
        K_RWS[VARIABLES][ref - 1] = NULL;
        return(0);
    }

    kdb = K_interpret(VARIABLES, filename);
    if(kdb == NULL) return(-1);

    K_free(K_RWS[VARIABLES][ref - 1]);
    K_RWS[VARIABLES][ref - 1] = kdb;

    return(0);
}

/**
 * @header4iode
 * 
 * Variables and functions for initializing and cleaning up the "in memory" workspaces.
 *
 * Globals
 * -------
 * global_ref_var = table of max 5 KDB* per object type, used for ws comparison, for printing...
 * Only used for Vars at the moment (print vars, print tables with comparison)
 *     - global_ref_var[6][0] = first WS of VARS for comparison (ws)  
 *     - global_ref_var[6][1] = second WS of VARS for comparison (file 1)
 *     - ...
 *
 * 
 * Functions
 * ---------
 *      - void K_init_ws(int ws)                    Initialises the "in mem" KDB structures and optionally loads the ws.* files
 *      - void K_end_ws(int ws)                     Deletes the global workspaces and their content after having optionally 
 *                                                  saved their content in ws.* files.
 *      - int K_load_RWS(int ref, char *filename)   Load a VAR file for use in GSample (print tables and graphs)
 */
#include "api/b_errors.h" 
#include "api/objs/kdb.h" 
#include "api/objs/objs.h" 
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"


/**
 *  @brief If ws is not null, set I_DEFAULT_FILENAME as the filename 
 *  for each of the global workspaces
 */
void K_init_ws(int ws)
{
    if(ws == 0) 
        return;

    for(int i = 0; i < IODE_NB_TYPES; i++)
    {
        try
        {
            get_global_db(i).merge_from(I_DEFAULT_FILENAME);
        }
        catch(const std::exception& e)
        {
            std::string error_msg = "Error initializing the global " + v_iode_types[i] + " database from file '";
            error_msg += std::string(I_DEFAULT_FILENAME) + "':\n\t";
            error_msg += e.what();
            kwarning(error_msg.c_str());
        }
    } 
}

/**
 *  @brief If ws is not null, saves the KDBs in files ws.cmt, ws.eqs...ws.var 
 */
void K_end_ws(int ws)
{
    if(ws == 0) 
        return;

    for(int i = 0; i < IODE_NB_TYPES; i++)
    {
        try
        {
            get_global_db(i).save_binary(I_DEFAULT_FILENAME, false);
        }
        catch(const std::exception& e)
        {
            std::string error_msg = "Error saving the global " + v_iode_types[i] + " database to file '";
            error_msg += std::string(I_DEFAULT_FILENAME) + "':\n\t";
            error_msg += e.what();
            kwarning(error_msg.c_str());
        }
    }
}


/**
 *  Loads a VAR file for use in GSample (print tables and graphs).
 *  Stores its KDB pointer in global_ref_var[ref - 1]. 
 *  If filename is NULL, frees global_ref_var[ref - 1].
 *  
 *  Example: the files [1], [2] and [3] used in a GSample "2000Y1[1,2,3]" are loaded:
 *      - [1] == current content of the VAR workspace
 *      - [2] by a call to K_load_RWS(2, "file2.var")
 *      - [3] by a call to K_load_RWS(3, "file3.var")
 *  
 *  @param [in] int     ref         reference number that will be used in GSample. 
 *                                  Restriction : 2 <= ref <= 5
 *  @param [in] char*   filename    file to load. 
 *                                  If NULL, frees global_ref_var[ref-1]
 *  @return     int                 0 on success, -1 on error (file not found, ref out of range...)
 *  
 */
 
int K_load_RWS(int ref, char *filename)
{
    if(ref < 2 || ref > 5) 
    {
        error_manager.append_error(std::string("Invalid Reference number. Must be between 2 and 5. Got") +
                                   std::to_string(ref));
        return(-1);
    }
    
    if(filename == NULL) 
    {
        if(global_ref_var[ref - 1])
            delete global_ref_var[ref - 1];
        global_ref_var[ref - 1] = nullptr;
        std::string msg = "Filepath for the Variables 'reference file' number " + 
                          std::to_string(ref) + " is empty";
        kwarning(msg.c_str());
        return 0;
    }

    if(global_ref_var[ref - 1])
        delete global_ref_var[ref - 1];
    global_ref_var[ref - 1] = new KDBVariables(false);
    bool success = global_ref_var[ref - 1]->load(std::string(filename));
    if(!success)
    {
        delete global_ref_var[ref - 1];
        global_ref_var[ref - 1] = nullptr;
        std::string msg = "Error loading the variables file '";
        msg += std::string(filename) + "' required to compute the table";
        kwarning(msg.c_str());
        return -1;
    }

    return 0;
}

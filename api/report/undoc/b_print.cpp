/**
 * @header4iode
 *
 * Functions to print IODE object definitions.
 *
 *  List of functions 
 *  -----------------
 *    int B_PrintVal(double val)                                    | Print a double with the function T_print_val() and with the number of decimals set to -1
 *    int B_replesc(unsigned char* out, unsigned char* in)             | Replace \ by / in a string
 *    int B_isdef(char* txt)                                           | Checks if a string contains non space charaters.
 *    int B_get1int(char* arg)                                         | Return the integer value of the beginning of a string.
 *    int B_ScrollSet(char* arg, long *plong, int inf, int sup)        | Interprets the first part of a string as a integer and check that the value is between 2 boundaries.
 *    int B_PrintObjTblTitle(char* arg, int unused)                                | $PrintObjTitle 0 or 1
 *    int B_PrintObjLec(char* arg, int unused)                                     | $PrintObjLec {0|1|2}
 *    int B_PrintObjEqsInfos(char* arg, int unused)                                | $PrintObjInfos {0|1|2}
 *    int B_PrintObjDef_1(char* arg, int* type)                        | Print the definition of the object named arg of the given type
 *    int B_PrintObjDef(char* arg, int type)                           | $PrintObjDefXxx object_list
 *    int B_PrintObjDefArgs(char* arg, int type)                       | Print a list of objects of a given type.
 */
#include "scr4/s_prost.h"

#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_a2mini.h"
#include "api/b_errors.h"
#include "api/k_super.h"
#include "api/print/print.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"
#include "api/write/write.h"
#include "api/report/undoc/undoc.h"

#include <algorithm>    // for std::min, std::max

/*================================= UTILITIES ===============================*/

/**
 *  Print a double with the function T_print_val() and with the number of decimals set to -1.
 *  
 *  @param [in] val double  value to print
 *  @return         int     0
 */
int B_PrintVal(double val)
{
    // B_NBDEC replaced by K_NBDEC JMP 18/04/2022
    int     nbdec = K_NBDEC;

    K_NBDEC = -1;
    T_print_val(val);
    K_NBDEC = nbdec;
    return(0);
}


/**
 *  Replace \ by / in a string.
 *  
 *  @param [out] out    unsigned char*      output string
 *  @param [in]  in     unsigned char*      input string
 *  @return             int                 0
 *  
 *  TODO: replace by SCR_replace()
 */
int B_replesc(unsigned char* out, unsigned char* in)
{
    int     i;

    if(in == 0 || out == 0) return(0); /* JMP 29-09-2015 */
    for(i = 0 ; in[i] ; i++) {
        out[i] = in[i];
        if(out[i] == '\\') out[i] = '/';
    }
    out[i] = 0;
    return(0);
}


/**
 *  Checks if a string contains non space charaters.
 *  
 *  @param [in] txt char*   string to check
 *  @return         int     0 if txt is NULL or contains only spaces, 1 otherwise        
 */
int B_isdef(char* txt)
{
    if(txt == NULL || txt[0] == 0) return(0);
    for(; *txt ; txt++) if(!isspace(*txt)) return(1);
    return(0);
}

/* ============================ PRINT ==================================== */

/**
 *  Return the integer value of the beginning of a string.
 *  
 *  @param [in] arg char*   string beginning with a number
 *  @return         int     value of the first number in the string         
 *                          -100 if arg is empty
 *  
 *  TODO: rewrite this!
 *  
 */
int B_get1int(char* arg)
{
    char    **args;
    int     n;

    args = B_ainit_chk(arg, 0L, 1);
    if(args == 0) return(-100);
    n = atoi(args[0]);
    SCR_free_tbl((unsigned char**) args);
    return(n);
}



/**
 *  Interprets the first part of a string as a integer and check that the value 
 *  is between 2 boundaries.
 *  
 *  @note Moved from sb_global.c
 *  
 *  @param [in]  arg   char*    string beginning with an integer 
 *  @param [out] plong long*    pointer to the calculated value
 *  @param [in]  inf   int      min value
 *  @param [in]  sup   int      max value
 *  @return            int      0 on success, -1 if arg is NULL
 */
int B_ScrollSet(char* arg, long *plong, int inf, int sup)
{
    int     n;

    n = B_get1int(arg);
    if(n <= -100) return(-1);
    n = std::max(inf, n);
    n = std::min(sup, n);
    *plong = n;
    return(0);
}


// $PrintObjTitle 0 or 1
//      0 : print table full definitions
//      1 : print only table titles
int B_PrintObjTblTitle(char* arg, int unused)
{
    long    l;
    int     rc;

    rc  = B_ScrollSet(arg, &l, 0, 2);
    if(rc) return(rc);
    B_TABLE_TITLE = l;
    return(0);
}


// $PrintObjLec {0|1|2}
//     0 : print the LEC equation
//     1 : replace all scalars by their values
//     2 : replaced all scalars by their values + t-tests
int B_PrintObjLec(char* arg, int unused)
{
    long    l;
    int     rc;

    rc  = B_ScrollSet(arg, &l, 0, 2);
    if(rc) return(rc);
    B_EQS_LEC = l;
    return(0);
}


// $PrintObjInfos {0|1|2}
//    0: equation only 
//    1: equation + comment
//    2: equation + comment + estimation results
int B_PrintObjEqsInfos(char* arg, int unused)
{
    long    l;
    int     rc;

    rc  = B_ScrollSet(arg, &l, 0, 2);
    if(rc) return(rc);
    B_EQS_INFOS = l;
    return(0);
}

static int     BEG = 0;     // Nb of the current printed variable (to limit each group of variable to 47)


/**
 * Print the definition of the object named arg of the given type.
 *  
 * If the object does not exist or type is illegal, 
 * an error message is added to the stack using IodeErrorManager::append_error().
 *  
 *  @param [in] arg     char*   object name
 *  @param [in] type    int     object type
 *  @return             int     0 on success, -1 on error
 */
int B_PrintObjDef_1(char* arg, int* type)
{
    int rc = 0;
    bool success = false;

    try
    {
        KDB& kdb = get_global_db(*type);
        std::string name = std::string(arg);
        bool found = kdb.contains(name);
        if(!found) 
            goto err;
    
        kmsg("Printing %s ...", arg);
        if(khitkey() != 0) 
            kgetkey();
    
        if (*type < COMMENTS || *type > VARIABLES) 
            goto err;
    
        if(*type == VARIABLES && BEG % 47 == 0)
        {
            if(BEG > 0) 
            {
                W_printf((char*) ".tl\n.te\n\n");
                W_flush();
            }
    
            Sample* smpl = kdb.sample;
            if(!smpl || smpl->nb_periods == 0) 
            {
                std::string msg = "Cannot print the variable '" + std::string(arg) + "' because ";
                msg += "the variable database has no sample defined";
                kwarning((char*) msg.c_str());
                return -1;
            }
            Period start_period = smpl->start_period;
            W_printfRepl((char*) ".tb %d\n.sep &\n", smpl->nb_periods + 1);
            W_printfRepl((char*) ".tl\n&1LName");
            std::string txt;
            for(int j = 0; j < smpl->nb_periods; j++) 
            {
                txt = start_period.shift(j).to_string();
                W_printfRepl((char*) "&1C%s", (char*) txt.c_str());
            }
            W_printf((char*) "\n.tl\n");
        }
        
        success = kdb.print_obj_def(name);
    
        if(*type == VARIABLES)
            BEG++;
        else
            W_flush();
    
        return success ? 0 : -1;
    }
    catch(const std::exception& e)
    {
        error_manager.append_error("Error while printing object '" + std::string(arg) + "': " + std::string(e.what()));
        return -1;
    }

err:
    error_manager.append_error("Object '" + std::string(arg) + "' not found !");
    return -1;
}



// $PrintObjDefXxx object_list
//   where Xxx = {cmt | idt | lst | scl | tbl | var}
int B_PrintObjDef(char* arg, int type)
{
    return(B_PrintObjDefArgs(arg, type));
}


int wrapper_B_PrintObjDef_1(char* arg, void* type)
{
    return B_PrintObjDef_1(arg, (int*) type);
}

/**
 *  Print a list of objects of a given type.
 *  
 *  @param [in] arg  char*      list of objects to print. If NULL or empty, prints all objects in the WS.
 *  @param [in] type int        object type
 *  @return          int        0 on success, -1 on error: return code of B_PrintObjDef_1()   
 */
int B_PrintObjDefArgs(char* arg, int type)
{
    int rc = 0;

    kmsg("Printing IODE objects definition to file '%s'...", W_filename);
    if(arg == 0 || arg[0] == 0)
    {
        try
        {
            KDB& kdb = get_global_db(type);
            for(const std::string& name : kdb.get_names())
            {
                rc = B_PrintObjDef_1((char*) name.c_str(), &type);
                if(rc) break;
            }
        }
        catch(const std::exception& e)
        {
            error_manager.append_error("Error while printing objects: " + std::string(e.what()));
            return -1;
        }
    }
    else
        rc = B_ainit_loop(arg, wrapper_B_PrintObjDef_1, (char *) &type);

    if(BEG) 
    {
        BEG = 0;
        W_printf((char*) ".te\n\n");
    }

    W_flush();
    kmsg("Print done");
    return(rc);
}

/**
 *  @header4iode
 * 
 *  Functions acting on IODE objects called by the report engine (see b_rep_syntax.c for a complete list of functions).
 *    
 *  These functions all have a similar syntax and always 
 *  return an integer as return code, 0 indicating success, other values an error.
 *  
 *  There are 3 groups of functions:
 *      - functions requiring as 2d arg an object type like B_DataCreate() for the report commands $DataCreateIdt, $DataCreateVar...
 *      - functions requiring as 2d arg a file type like B_FileDelete() for the report commands $FileDeleteCsv, $FileDeleteTxt...
 *      - functions with only one arg, like B_DataListCount() or B_DataCalcVar()
 *  
 *  Functions with an IODE object suffix (cmt, eqs...)
 *  --------------------------------------------------
 *  Some functions need a suffix in the report syntax. For example $DataDelete and $DataUpdate 
 *  require a suffix indicating which type of objects is their target (one of the 7 IODE objects).
 *  In that way, only one function is needed for $DataDeleteVar or $DataDeleteIdt...
 *  
 *  When called by the report engine, these functions have 2 parameters: 
 *      - the argument of the function (the remaining of the report line)
 *      - the type of object treated (COMMENTS <= type <= VARIABLES)
 *  
 *  For these functions, the parameters and return values are as follows: 
 *  
 *      @param [in] char*   arg     report line without the command 
 *      @param [in] int     type    type of object whose names are to be saved in the list (bw COMMENTS and VARIABLES)
 *      @return     int             0 on success, -1 on error 
 *
 *  For instance, the report command 
 *  
 *      $DataDeleteVar A B C
 *  
 *  generates the C call: 
 *  
 *      B_DataDelete("A B C", VARIABLES);
 *
 *  where arg == "A B C" and type == VARIABLES   
 *      
 *  
 *  Functions with a file extension suffix (csv, txt...)
 *  ----------------------------------------------------
 *  The principle is the same as above but for filename extensions instead of object types.
 *    
 *  When called by the report engine, these functions have 2 parameters: 
 *      - the argument of the function (the remaining of the report line)
 *      - the type of object treated (COMMENTS <= type <= FILE_CSV)
 *  
 *  The parameters and return values are the same as for the functions with an IODE object type parameter.
 *
 *  For instance, the report command 
 *  
 *      $FileDeleteCsv myfile
 *  
 *  calls the C function 
 *  
 *      B_FileDelete(arg, FILE_CSV);
 *
 *  where arg == "myfile" and type == VARIABLES   
 *      
 *  
 *  Other functions
 *  ---------------
 *  All other functions receive only the argument on the report line. 
 *  B_DataListSort(), for example, has only one argument.
 *
 *  For these functions, the parameters and return values are as follows: 
 *  
 *   @param [in] char*   arg     report line without the command
 *   @return     int             0 on success, -1 on error (not enough args)
 *  
 *  List of functions 
 *  -----------------
 *      int B_DataPattern(char* arg,int type)       Creates an IODE list with all existing objects of a given type having the name constructed by the combinations of 2 lists.
 *      int B_DataRasVar(char* arg, int unused)                 RAS method implementation. 
 *      int B_DataCalcVar(char* arg, int unused)                Computes a new variable based on a LEC expression.
 *      int B_DataCreate(char* arg, int type)       Creates one or more new objects.
 *      int B_DataDelete(char* arg, int type)       Deletes one or more objects.
 *      int B_DataRename(char* arg, int type)       Renames an object. Equations cannot be renamed.
 *      int B_DataDuplicate(char* arg, int type)    Duplicates an object. Equations cannot be duplicated.
 *      int B_DataUpdate(char* arg, int type)       Updates an object. The syntax can differ according to the object type.
 *      int B_DataSearch(char* arg, int type)       Searches all objects containing a given string in their names and/or definitions.
 *      int B_DataListSort(char* arg, int unused)               Sorts a list alphanumerically. 
 *      int B_DataScan(char* arg, int type)         Analyses a KDB content and creates 2 lists _EXO and _SCAL with all VAR and all Scalar found in the kdb objects.
 *      int B_DataExist(char* arg, int type)        Checks that an object exists. Returns -1 if not, the object position in WS otherwise.
 *      int B_DataAppend(char* arg, int type)       Appends data (a string) to a CMT or a LST.
 *      int B_DataList(char* arg, int type)         Constructs a list of objects corresponding to a given name pattern. Objects can be in WS or in a file.
 *      int B_DataCalcLst(char* arg, int unused)                List calculus: 4 operations between 2 lists.
 *      int B_DataListCount(char* arg, int unused)              Returns the number of elements in a list.
 *      int B_DataCompareEps(char* arg, int unused)             Defines the threshold under which the difference between 2 variables are considered equal.
 *      int B_DataCompare(char* arg, int type)      Compares the objects in the current WS to the content of an IODE file and stores the results in 4 lists.
 *      int B_DataDisplayGraph(char* arg, int unused)           This function allows you to view VARs or combinations of VARS in graphical form.
 *      int B_DataPrintGraph(char* arg, int unused)             This function allows you to print VARs or combinations of VARS in graphical form.
 */
#include "scr4/s_args.h"

#include "api/constants.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/grep.h"
#include "api/objs/comments.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/objs/compare.h"
#include "api/print/print.h"
#include "api/report/commands/commands.h"
#include <algorithm>    // for std::min, std::max


/**
 *  Creates an IODE list with all existing objects of a given type having the name constructed
 *  by the combinations of 2 lists.
 *  
 *  Syntax:
 *      $DataPattern<type> list_name pattern X-dimension [Y-dimension]
 *  
 *  Example:
 *      $$WsLoadVar ras.av    
 *      $DataUpdateLst X R1;R2;R3    
 *      $DataUpdateLst Y C1;C2
 *      $DataPatternVar RC xy $X $Y 
 *  
 *  Result:
 *      List RC == "R1C1,R1C2,R2C1,R2C2,R3C1,R3C2"
 *      if R1C1... exist in the WS of vars.
 *    
 *  @param [in] char*   arg     report line 
 *  @param [in] int     type    type of object whose names are to be saved in the list
 *  @return     int             0 on success, -1 on error (not enough args)
 */
 
int B_DataPattern(char* arg,int type)
{
    char    **args, *lstname, *pattern;
    char    **xvars = NULL, **yvars = NULL;
    char    cvar[K_MAX_NAME + 1], rvar[K_MAX_NAME + 1], toappend[2*K_MAX_NAME +3];
    int     rc = -1, nrows = 0, ncols = 0, crow, ccol;
    int     nbargs;

    args = (char**) SCR_vtom((unsigned char*) arg, (int) ' ');
    nbargs = SCR_tbl_size((unsigned char**) args);
    if(nbargs < 3) goto cleanup;
    else {
        lstname = args[0];
        pattern = args[1];
        xvars = B_ainit_chk(args[2], NULL, 0);
        nrows = SCR_tbl_size((unsigned char**) xvars);

        if(nbargs > 3) {
            yvars = B_ainit_chk(args[3], NULL, 0);
            ncols = SCR_tbl_size((unsigned char**) yvars);
        }

        if(nrows == 0) goto cleanup;
        for(crow = 0; crow < nrows; crow++) {
            strcpy(rvar, pattern);
            SCR_replace((unsigned char*) rvar, (unsigned char*) "x", (unsigned char*) xvars[crow]);
            if(ncols == 0) {
                sprintf(toappend, "%s %s", lstname, rvar);
                if(crow == 0) B_DataUpdate(toappend, LISTS);
                else B_DataAppend(toappend, LISTS);
            }
            for(ccol = 0; ccol < ncols; ccol++) {
                strcpy(cvar, rvar);
                SCR_replace((unsigned char*) cvar, (unsigned char*) "y", (unsigned char*) yvars[ccol]);
                if(B_DataExist(cvar, type) >= 0) {
                    sprintf(toappend, "%s %s", lstname, cvar);
                    if(crow == 0 && ccol == 0) B_DataUpdate(toappend, LISTS); // Creates the list
                    else B_DataAppend(toappend, LISTS);                       // Appends to the list  
                }
            }
        }
        rc = 0;
    }

cleanup:
    SCR_free_tbl((unsigned char**) args);
    SCR_free_tbl((unsigned char**) xvars);
    SCR_free_tbl((unsigned char**) yvars);
    return(rc);
}


/**
 *  RAS method implementation. 
 *  See https://iode.plan.be/doku.php?id=datarasvar for details.
 *  
 *  Syntax: $DataRasVar pattern X-dimension Y-dimension ref_year h_year [maxit [eps]]
 *  
 *  @params See file header 
 */
int B_DataRasVar(char* arg, int unused)
{
    char    **args;
    int     rc = 0, nbargs, maxit = 100;
    double  eps = 0.001;

    args = (char**) SCR_vtom((unsigned char*) arg, (int) ' ');
    nbargs = SCR_tbl_size((unsigned char**) args);
    if(nbargs < 5) 
        rc = -1;
    else 
    {
        if(nbargs > 5) 
            maxit = atoi(args[5]);
        
        if(nbargs > 6) 
            eps = atof(args[6]);

        Period *rper = new Period(std::string(args[3]));
        Period *cper = new Period(std::string(args[4]));
        rc = RasExecute(args[0], args[1], args[2], rper, cper, maxit, eps);
        delete rper;
        delete cper;
        rper = nullptr;
        cper = nullptr;
    }

    SCR_free_tbl((unsigned char**) args);
    return rc;
}


/**
 *  Computes a new variable based on a LEC expression.
 *  
 *  Syntax:  $DataCalcVar varname lec
 *  
 * for details
 *  
 *  @params See file header 
 *  @return int     0 on success, 
 *                  -1 on error: variable cannot be created, error in LEC formula, link impossible...
 */
int B_DataCalcVar(char* arg, int unused)
{
    char    name[K_MAX_NAME + 1];
    KDBVariables* kdb = global_ws_var.get();

    int lg = B_get_arg0(name, arg, K_MAX_NAME + 1);

    char* lec = arg + lg + 1;
    SCR_strip((unsigned char*) lec);

    if(!kdb->contains(std::string(name)))
    {
        bool success = kdb->set_obj(name, (double*) NULL);
        if(!success) 
            return(-1);
    }

    if(lec[0]) 
    {
        CLEC* clec = L_cc(lec);
        if(clec != 0 && !L_link(kdb, global_ws_scl.get(), clec)) 
        {
            double d;
            for(int t = 0 ; t < kdb->sample->nb_periods ; t++) 
            {
                d = L_exec(kdb, global_ws_scl.get(), clec, t);
                kdb->get_var_ptr(name)[t] = d;
            }
            SW_nfree(clec);
        }
        else 
        {
            std::string error_msg = std::string(L_error()) + " : " + std::string(lec);
            error_manager.append_error(error_msg);
            SW_nfree(clec);
            return(-1);
        }
    }

    return(0);
}


/**
 *  Creates one new object. Sub function of B_DataCreate().
 *  
 *  @params [in]    char*   arg     new object name
 *  @params [in]    int*    ptype   new object type
 *  @return         int             0 on success, -1 on error
 *  
 */
int B_DataCreate_1(char* arg, int* ptype)
{
    try 
    {
        KDB& kdb = get_global_db(*ptype);

        std::string name = std::string(arg);
        if(kdb.contains(name)) 
            return -1;

        int res;
        bool success = false;
        char deflt[41];
        switch(*ptype) 
        {
            case COMMENTS :
                success = global_ws_cmt->set_obj(arg, "");
                break;
            case EQUATIONS :
                sprintf(deflt, "%s := %s", arg, arg);
                res = K_upd_eqs(arg, deflt, 0L, 0, 0L, 0L, 0L, 0L, 0);
                return res; 
            case IDENTITIES :
            {
                Identity idt(name);
                success = global_ws_idt->set_obj(arg, &idt);
                break;  
            }
            case LISTS :
                success = global_ws_lst->set_obj(arg, "");
                break;
            case SCALARS :
            {
                Scalar scl;
                success = global_ws_scl->set_obj(arg, &scl);
                break;
            }
            case TABLES :
                res = K_upd_tbl(arg, "TITLE;LEC");
                return res;
            case VARIABLES :
                success = global_ws_var->set_obj(arg, (double*) NULL); 
                break;
        }

        return success ? 0 : -1;
    } 
    catch (const std::runtime_error& e) 
    {
        error_manager.append_error("DataCreate: " + std::string(e.what()));
        return -1;
    }
}

int wrapper_B_DataCreate_1(char* arg, void* ptype)
{
    return B_DataCreate_1(arg, (int*) ptype);
}

/**
 *  Creates one or more new objects.
 *  
 *  Syntax:  $DataCreate<type> name ...
 *  
 * for details
 *  
 *  @params See file header 
 *  @return int     0 on success, 
 *                  -1 on error: variable cannot be created
 */

int B_DataCreate(char* arg, int type)
{
    return(B_ainit_loop(arg, wrapper_B_DataCreate_1, (char *) &type));
}


/**
 *  Deletes an object. Sub function of B_DataDelete().
 *  
 *  
 *  @params [in]    char*   arg     new object name
 *  @params [in]    int*    ptype   new object type
 *  @return         int             0 on success, -1 on error
 *  
 */
int B_DataDelete_1(char* arg, int* ptype)
{
    std::string name = std::string(arg);
    try
    {
        KDB& kdb = get_global_db(*ptype);
    
        if(!kdb.contains(name)) 
            return -1;
        
        bool success = kdb.remove(name);
        if(!success) 
        {
            error_manager.append_error("Failed to delete '" + name + "'");
            return -1;
        }
    
        return 0;
    }
    catch (const std::runtime_error& e) 
    {
        error_manager.append_error("DataDelete: " + std::string(e.what()));
        return -1;
    }
}

int wrapper_B_DataDelete_1(char* arg, void* ptype)
{
    return B_DataDelete_1(arg, (int*) ptype);
}

/**
 *  Deletes one or more objects. Name may contain * to delete all objects with the same pattern.
 *  
 *  Syntax:  $DataDelete<type> name ...
 *  
 * for details
 *  
 *  @params See file header 
 *  @return int     0 on success, 
 *                  -1 on error: variable cannot be deleted (does not exist)
 */
int B_DataDelete(char* arg, int type)
{
    char    *lst;
    int     rc;

    lst = K_expand(type, NULL, arg, '*');
    rc = B_ainit_loop(lst, wrapper_B_DataDelete_1, (char*) &type);
    SCR_free(lst);
    return(rc);
}


/**
 *  Renames an object. Equations cannot be renamed.
 *  
 *  Syntax:  $DataRename<type> old_name new_name
 *  
 * for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be renamed
 */
int B_DataRename(char* arg, int type)
{
    int     rc = 0;
    char    **args;

    if(type == EQUATIONS) 
        return(-1); /* Rename of EQS has no sense */

    args = B_ainit_chk(arg, NULL, 2);
    if(args == NULL) 
        return(-1);

    try
    {
        bool success = get_global_db(type).rename(std::string(args[0]), std::string(args[1]));
        if(!success) 
        {
            std::string error_msg = "DataRename '" + std::string(args[0]) + "' to '";
            error_msg += std::string(args[1]) + "' failed";
            error_manager.append_error(error_msg);
            rc = -1;
        }
    }
    catch (const std::runtime_error& e) 
    {
        error_manager.append_error("DataRename: " + std::string(e.what()));
        rc = -1;
    }

    A_free((unsigned char**) args);
    return(rc);
}


/**
 *  Duplicates an object. Equations cannot be duplicated.
 *  
 *  Syntax:  $DataDuplicate<type> old_name new_name
 *  
 * for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be renamed
 */
 int B_DataDuplicate(char* arg, int type)
{   
    if(type == EQUATIONS) 
    {
        error_manager.append_error("DataDuplicate of Equations has no sense");
        return -1; /* Duplicate of EQS has no sense */
    }
    
    char** args = B_ainit_chk(arg, NULL, 2);
    if(args == NULL) 
        return -1;
    std::string old_name = std::string(args[0]);
    std::string new_name = std::string(args[1]);
    A_free((unsigned char**) args);
    
    try
    {
        KDB& kdb = get_global_db(type);
        bool success = kdb.duplicate(kdb, old_name, new_name);
        if(!success) 
        {
            std::string error_msg = "DataDuplicate '" + old_name + "' as '";
            error_msg += new_name + " failed";
            error_manager.append_error(error_msg);
            return -1;
        }
    }
    catch (const std::runtime_error& e) 
    {
        error_manager.append_error("DataDuplicate: " + std::string(e.what()));
        return -1;
    }

    return 0;
}


/**
 *  Updates an object. The syntax can differ according to the object type. 
 *  
 *  Syntax
 *  ------
 *   $DataUpdateCmt name comment
 *   $DataUpdateEqs name lec
 *   $DataUpdateIdt name lec
 *   $DataUpdateLst name list
 *   $DataUpdateScl name value [relax]
 *   $DataUpdateTbl name title;lec1;lec2;...
 *   $DataUpdateVar name [L,l|D,d|G,g] period valuer1 value2 ...   
 *  
 *  Example for vars: 
 *      B_DataUpdate("A L 2001Y1 12 13 14"); // Create or update A in Level : 20001Y1 => 12, 2002Y1 => 13, ...
 *  
 * for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be updated
 */

int B_DataUpdate(char* arg, int type)
{
    bool    success;
    int     shift, rc,
            nb_args, nb_upd, nb_p,
            i, mode;
    double  var;
    Scalar  scl;
    Period  *per = nullptr;
    char    name[K_MAX_NAME + 1], **args = NULL;
    std::string var_name;
    
    int lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    
    try
    {
        KDB& kdb = get_global_db(type);
        if(!kdb.contains(std::string(name))) 
        {
            if(B_DataCreate(name, type)) 
            return -1;
        }
        
        char* value = arg + lg + 1;
        switch(type) 
        {
        case COMMENTS :
            success = global_ws_cmt->set_obj(name, value);
            break;
        case EQUATIONS :
            rc = K_upd_eqs(name, value, NULL, -1, NULL, NULL, NULL, NULL, 0);
            success = (rc == 0);
            break;
        case IDENTITIES :
        {
            std::string lec = std::string(value);
            Identity idt(lec);
            success = global_ws_idt->set_obj(name, &idt);
            break;
        }
        case LISTS :
            success = global_ws_lst->set_obj(name, value);
            break;
        case TABLES :
            rc = K_upd_tbl(name, value);
            success = (rc == 0);
            break;
        case SCALARS :
            args = (char**) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
            nb_args = SCR_tbl_size((unsigned char**) args);
            scl.value = 0.9;
            scl.relax = 1.0;
            scl.std = IODE_NAN;

            success = true;
            switch(nb_args) 
            {
            case 2:
                scl.value = (double) atof(args[1]);
                break;
            case 3:
                scl.value = (double) atof(args[1]);
                scl.relax = (double) atof(args[2]);
                break;
            default :
                error_manager.append_error("DataUpdateScl : Invalid Argument");
                success = false;
                break;
            }

            if(success) 
                success = global_ws_scl->set_obj(std::string(args[0]), &scl);
            break;

        case VARIABLES : /* Name [D|d|G|g|L|l] Period nVal */
            args = (char**) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
            nb_args = SCR_tbl_size((unsigned char**) args);
            if(nb_args > 1) 
            {
                nb_p = 1;
                switch(args[1][0]) 
                {
                case 'd' :
                case 'D' :
                    nb_p = 2;
                    mode = VAR_MODE_DIFF;
                    if(U_is_in(args[1][1], "Yy")) 
                        mode = VAR_MODE_Y0Y_DIFF;
                    break;
                case 'g' :
                case 'G' :
                    nb_p = 2;
                    mode = VAR_MODE_GROWTH_RATE;
                    if(U_is_in(args[1][1], "Yy")) 
                        mode = VAR_MODE_Y0Y_GROWTH_RATE;
                    break;

                case 'l' :
                case 'L' :
                    nb_p = 2;
                default  :
                    mode = VAR_MODE_LEVEL;
                    break;
                }
                nb_upd = nb_args - nb_p - 1;

                per = new Period(std::string(args[nb_p]));
                if(per == 0) 
                {
                    error_manager.append_error("Syntax error: Period not defined"); /* JMP 23-05-00 */
                    success = false;
                    break;
                }
                int nb_periods = global_ws_var->sample->end_period.difference(*per) + 1;
                nb_upd = std::min(nb_upd, nb_periods);
                shift = per->difference(global_ws_var->sample->start_period);
                if(per == NULL || shift < 0)
                    success = false;
                else 
                {
                    var_name = std::string(name);
                    nb_p++;
                    for(i = 0; i < nb_upd; i++) 
                    {
                        var = (double) atof(args[i + nb_p]);
                        if(var == 0.0 && !U_is_in(args[i + nb_p][0], "-0.+")) 
                            var = IODE_NAN;
                        KV_set(global_ws_var.get(), var_name, shift + i, mode, var);
                    }
                    success = true;
                }
            }
            else 
                success = false;

            if(!success) 
                error_manager.append_error("DataUpdateVar : '" + std::string(arg) + "' Invalid Argument");
            break;
        }
    }
    catch(const std::runtime_error& e)
    {
        error_manager.append_error("DataUpdate: " + std::string(e.what()));
        success = false;
    }

    A_free((unsigned char**) args);
    if(per) 
        delete per;
    per = nullptr;

    return success ? 0 : -1;
}


/**
 *  Searches all objects containing a given string in their names and/or definitions.
 *  
 *  Syntax
 *  ------
 *      $DataSearch<type> mask word ecase in_name in_formula in_text list_result
 *        (word, ecase, in_name, in_formula, in_text := 0 ou 1)
 *        (mask := suite of chars or ?, *)
 *        (list_result := name of the resulting list
 *  
 * for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be renamed
 */
char** B_DataSearchParms(char* name, int word, int ecase, int names, int forms, int texts, int type)
{
    int rc = 0;
    char **args = NULL, buf[81];
    
    if(word == 1) 
    {
        strcpy(buf, "*!");
        SCR_strlcpy(((unsigned char*) buf) + 2, (unsigned char*) name, 76);
        strcat(buf, "!*");
    }
    else 
    {
        buf[0] = '*';
        SCR_strlcpy(((unsigned char*) buf) + 1, (unsigned char*) name, 76);
        strcat(buf, "*");
    }

    try
    {
        KDB& kdb = get_global_db(type);
        std::vector<std::string> lst = kdb.grep(buf, ecase, names, forms, texts, '*');
        char** c_lst = vector_to_double_char(lst);
        return c_lst;
    }
    catch (const std::runtime_error& e) 
    {
        error_manager.append_error("DataSearch: " + std::string(e.what()));
        return NULL;
    }
}


/**
 *  Searches all objects containing a given string in their names and/or definitions.
 *  
 *  Syntax
 *  ------
 *      $DataSearch<type> mask word ecase in_name in_formula in_text list_result
 *        (word, ecase, in_name, in_formula, in_text := 0 ou 1)
 *        (mask := suite of chars or ?, *)
 *        (list_result := name of the resulting list
 *  
 * for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be renamed
 */
int B_DataSearch(char* arg, int type)
{
    int rc = 0, word, ecase, names, forms, texts;
    char **args = NULL, **lst;

    args = B_vtom_chk(arg, 7); /* pattern list */
    if(args == NULL) 
        return(-1);
    
    word  = atoi(args[1]);
    ecase = atoi(args[2]);
    names = atoi(args[3]);
    forms = atoi(args[4]);
    texts = atoi(args[5]);

    lst = B_DataSearchParms(args[0], word, ecase, names, forms, texts, type);
  
    rc = KL_lst(args[6], lst, 200);
    SCR_free_tbl((unsigned char**) lst);

    A_free((unsigned char**) args);

    return(rc);
}

/**
 *  Obsolete. Was used only with a GUI to specify the way variables must be displayed.
 */
int B_DataEditCnf(char* arg, int unused)
{
    int     rc = 0, VM, VN;
    char    **args = NULL;

    args = B_vtom_chk(arg, 2); // pattern list 
    if(args == NULL) return(-1);

    switch(toupper(args[0][0])) {
    case 'L' :
        VM = 0;
        break;
    case 'D' :
        VM = 1;
        break;
    case 'G' :
        VM = 2;
        break;
    default  :
        VM = 0;
        break;
    }

    VN = atoi(args[1]);          /* JMP38 01-10-92 */
    VN = std::max(-1, VN);
    VN = std::min(6, VN);
    //BGUI_DataEditGlobal(VM, VN);  // JMP 2/8/2022 => no used 
    A_free((unsigned char**) args);
    return(rc);
}

/**
 *  Sub-function of B_DataListSort(). Only defined to used correct parameters for strcmp().
 *  
 *  @param [in] void*   pa  string 1
 *  @param [in] void*   pb  string 2
 *  @return     int     result of strcmp(pa, pb)
 */
static int my_strcmp(const void *pa, const void *pb)
{
    const char    **a, **b;      // TODO: warning 'initializing': different 'const' qualifiers
    
    a = (const char**) pa;
    b = (const char**) pb;      

    return(strcmp(*a, *b));
    
    //const char    *a = pa, *b = pb; // JMP 1/10/2021 TODO: Check this => Does NOT work ???
    //return(strcmp(a, b));           // JMP 1/10/2021 TODO: Check this => Does NOT work ???
}


/**
 *  Sorts a list alphanumerically. The resulting list can replace the original one or be an new list.
 *  
 *  Syntax:  $DataListSort input_list [output_list]
 *  
 * for details
 *  
 *  @params See file header 
 *  @return int     0 on success, 
 *                  -1 on error: variable cannot be created
 */

int B_DataListSort(char* arg, int unused)
{
    int     rc = 0;
    char    *in, *out, *lst,
            **args = NULL, **lsti = NULL,
            *old_A_SEPS;

    args = B_vtom_chk(arg, 2);
    if(args == NULL) 
        return(-1);
    else 
    {
        in = args[0];
        out = args[1];
    }

    if(!global_ws_lst->contains(in)) 
    {
        error_manager.append_error("List '" + std::string(args[0]) + 
                                   "' not found in the Lists workspace");
        rc = -1;
        goto done;
    }
    else 
        lst = global_ws_lst->get_obj(in);
    
    if(lst == NULL) 
    {
        error_manager.append_error("List '" + std::string(args[0]) + 
                                   "' not found in the Lists workspace");
        rc = -1;
        goto done;
    }

    // Changed A_SEPS to allow ; as separator
    old_A_SEPS = A_SEPS;
    A_SEPS = " \t\n\r;, ";  
    lsti = B_ainit_chk(lst, NULL, 0);
    A_SEPS = old_A_SEPS;
    if(lsti == NULL) 
    {
        rc = -1;
        goto done;
    }

    qsort(lsti, SCR_tbl_size((unsigned char**) lsti), sizeof(char **), my_strcmp);
    lst = (char*) SCR_mtov((unsigned char**) lsti, ';');  /* JMP 09-03-95 */

    if(!global_ws_lst->set_obj(out, lst)) 
    {
        error_manager.append_error("Sorted List '" + std::string(out) + "' cannot be created");
        rc = -1;
    }

    SCR_free_tbl((unsigned char**) lsti);
    SCR_free(lst);

done:
    A_free((unsigned char**) args);
    return(rc);
}

template<typename T>
int template_data_scan(const int type, const std::string& objs, const std::string& var_list_name, 
    const std::string& scl_list_name)
{
    try
    {
        KDB& global_kdb = get_global_db(type);
        
        // create a temporary subset with only the selected objects
        bool copy = false;
        T* global_kdb_ptr = dynamic_cast<T*>(&global_kdb);
        T kdb(global_kdb_ptr, objs, copy);
        if(kdb.size() == 0)
            return -1;
        
        int rc = K_scan(&kdb, (char*) var_list_name.c_str(), (char*) scl_list_name.c_str());
        return rc;
    }
    catch (const std::runtime_error& e) 
    {
        error_manager.append_error("DataScan : " + std::string(e.what()));
        return -1;
    }
}

/**
 *  Analyses a KDB content and creates 2 lists _EXO and _SCAL with all VAR and all Scalar found in the kdb objects (limited to IDT, EQ or Table).
 *  
 *  Syntax
 *  ------
 *      $DataScan<type> [name1 name2...]
 *  
 * for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be renamed
 */

int B_DataScan(char* arg, int type)
{
    if(type != IDENTITIES && type != EQUATIONS && type != TABLES) 
    {
        error_manager.append_error("DataScan : only IDT, EQ and Table are supported");
        return -1;
    }

    int rc = -1;
    /* Exo whole WS */
    if(arg == NULL || arg[0] == 0) 
    {
        try
        {
            KDB& kdb = get_global_db(type);
            rc = K_scan(&kdb, "_EXO", "_SCAL");
        }
        catch (const std::runtime_error& e) 
        {
            error_manager.append_error("DataScan : " + std::string(e.what()));
            return -1;
        }
    }
    else 
    {
        std::string objs = std::string(arg);
        if(objs.empty())
        {
            try
            {
                KDB& kdb = get_global_db(type);
                rc = K_scan(&kdb, "_EXO", "_SCAL");
            }
            catch (const std::runtime_error& e) 
            {
                error_manager.append_error("DataScan : " + std::string(e.what()));
                return -1;
            }
        }
        else 
        {
            switch(type) 
            {
                case COMMENTS:
                    rc = template_data_scan<KDBComments>(type, objs, "_EXO", "_SCAL");
                    break;
                case EQUATIONS:
                    rc = template_data_scan<KDBEquations>(type, objs, "_EXO", "_SCAL");
                    break;
                case IDENTITIES:
                    rc = template_data_scan<KDBIdentities>(type, objs, "_EXO", "_SCAL");
                    break;
                case LISTS:
                    rc = template_data_scan<KDBLists>(type, objs, "_EXO", "_SCAL");
                    break;
                case SCALARS:
                    rc = template_data_scan<KDBScalars>(type, objs, "_EXO", "_SCAL");
                    break;
                case TABLES:
                    rc = template_data_scan<KDBTables>(type, objs, "_EXO", "_SCAL");
                    break;
                case VARIABLES:
                    rc = template_data_scan<KDBVariables>(type, objs, "_EXO", "_SCAL");
                    break;
                default:
                    {
                        std::string msg = "scan: invalid object type " + std::to_string(type);
                        kwarning(msg.c_str());
                        return -1;
                    }
            }
        }
    }

    return rc;
}


/**
 *  Checks that an object exists. Returns -1 if not, the object position in WS otherwise.
 *  
 *  Syntax
 *  ------
 *      $DataExist<type> object_name
 *  
 * for details
 *  
 *  @params See file header 
 *  
 *  @return int     >= if object exist (return its position)
 *                  -1 if object is not found
 */
int B_DataExist(char* arg, int type)
{
    try
    {
        KDB& kdb = get_global_db(type);
        return kdb.contains(std::string(arg));
    }
    catch (const std::runtime_error& e) 
    {
        error_manager.append_error("DataExist: " + std::string(e.what()));
        return -1;
    }
}


/**
 *  Appends data (a string) to a CMT or a LST.
 *  
 *  Syntax
 *  ------
 *      $DataAppend<type> name txt 
 *  
 * for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success
 *                  -1 if type is incorrect or the object cannot be created / expanded 
 */
int B_DataAppend(char* arg, int type)
{
    bool    success;
    int     lg;
    char    name[K_MAX_NAME + 1], *ptr, *nptr, *text;
    
    switch(type) 
    {
    case COMMENTS :
    case LISTS :
        break;

    case EQUATIONS :
    case IDENTITIES :
    case SCALARS :
    case TABLES :
    case VARIABLES :
        error_manager.append_error("DataAppend : only lists and comments");
        return(-1);
    }

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    text = arg + lg + 1;

    try
    {
        KDB& kdb = get_global_db(type);
        bool found = kdb.contains(std::string(name));
        if(!found)
            nptr = text;
        else 
        {
            if(strlen(text) == 0) 
                return(0);
    
            ptr = K_optr0(&kdb, name);
            nptr = SW_nalloc((int)strlen(ptr) + (int)strlen(text) + 2);
            if(type == COMMENTS)
                sprintf(nptr, "%s %s", ptr, text);
            else
                sprintf(nptr, "%s,%s", ptr, text);
        }
    
        switch(type) 
        {
        case COMMENTS :
            success = global_ws_cmt->set_obj(name, nptr);
            break;
        case LISTS :
            success = global_ws_lst->set_obj(name, nptr);
            break;
        }
    
        if(nptr != text) 
            SW_nfree(nptr);
    
        return success ? 0 : -1;
    }
    catch (const std::runtime_error& e) 
    {
        error_manager.append_error("DataAppend: " + std::string(e.what()));
        return -1;
    }
}

template<typename T>
std::vector<std::string> template_data_list(const std::string& filename, const std::string& pattern)
{
    std::vector<std::string> lst;

    T kdb(false);
    kdb.load(filename);
    if(kdb.size() == 0)
        return lst;

    lst = kdb.grep(pattern, false, true, false, false, '*');
    return lst;
}

/**
 *  Constructs a list of objects matching a given name pattern. 
 *  Objects can be in WS or in a file. 
 *  The pattern can contain * to allow any suite of characters.
 *  
 *  Syntax
 *  ------
 *      $DataList<type> listname pattern [filename]
 *  
 * for details
 *  
 *  @params See file header 
 *  @return int     0 on success
 *                  -1 if type is incorrect or the object cannot be created / expanded 
 */
int B_DataList(char* arg, int type)
{
    std::string name;
    std::string filename;
    std::string pattern;

    char** args = B_vtom_chk(arg, 3);
    if(args == NULL) 
    {
        args = B_vtom_chk(arg, 2);
        if(args == NULL) 
            return -1;
        else 
        {
            name    = std::string(args[0]);
            pattern = std::string(args[1]);
        }
    }
    else 
    {
        name     = std::string(args[0]);
        pattern  = std::string(args[1]);
        filename = std::string(args[2]);
    }

    A_free((unsigned char**) args);

    std::vector<std::string> lst;
    if(filename.empty())
    {
        try
        {
            KDB& kdb = get_global_db(type);
            lst = kdb.grep(pattern, false, true, false, false, '*');
        }
        catch (const std::runtime_error& e) 
        {
            error_manager.append_error("DataList: " + std::string(e.what()));
            return -1;
        }
    }
    else 
    {
        switch(type) 
        {
        case COMMENTS:
            lst = template_data_list<KDBComments>(filename, pattern);
            break;
        case EQUATIONS:
            lst = template_data_list<KDBEquations>(filename, pattern);
            break;
        case IDENTITIES:
            lst = template_data_list<KDBIdentities>(filename, pattern);
            break;
        case LISTS:
            lst = template_data_list<KDBLists>(filename, pattern);
            break;
        case SCALARS:
            lst = template_data_list<KDBScalars>(filename, pattern);
            break;
        case TABLES:
            lst = template_data_list<KDBTables>(filename, pattern);
            break;
        case VARIABLES:
            lst = template_data_list<KDBVariables>(filename, pattern);
            break;
        default:
            {
                std::string msg = "DataList: invalid IODE type " + std::to_string(type);
                kwarning(msg.c_str());
                return -1;
            }
        }
    }

    int rc = -1;
    if(lst.empty()) 
        rc = KL_lst((char*) name.c_str(), NULL, 200);
    else
    {
        char** c_lst = vector_to_double_char(lst);
        rc = KL_lst((char*) name.c_str(), c_lst, 200);
        SCR_free_tbl((unsigned char**) c_lst);
    }

    return rc;
}

/**
 *  Creates a new list of strings as the product of 2 lists of strings. ["A", "B"] x ["C", "D"] => ["AB", "AC", "BC", "BD"]
 *  
 *  @param [in] char**  l1  first list of strings
 *  @param [in] char**  l2  second list of strings
 *  @return     char**      l1 x l2
 */
static unsigned char **Lst_times(unsigned char **l1, unsigned char **l2)
{
    int             i, inb, j, jnb, nl, ll3, ll1, ll2;
    unsigned char   **res = NULL;
    unsigned char   *l3 = NULL;

    inb = SCR_tbl_size(l1);
    jnb = SCR_tbl_size(l2);

    nl = 0;
    ll3 = 0;
    for(i = 0; i < inb; i++) 
    {
        ll1 = (int) strlen((char*) l1[i]);
        for(j = 0; j < jnb; j++) 
        {
            ll2 = (int) strlen((char*) l2[j]);

            if(ll3 < ll1 + ll2 + 1) 
            {
                ll3 = ll1 + ll2 + 1;
                SCR_free(l3);
                l3 = (unsigned char*) SCR_malloc(ll3);
            }

            if(l3 != NULL) sprintf((char*) l3, "%s%s", l1[i], l2[j]);
            SCR_add_ptr(&res, &nl, l3);
        }
    }

    SCR_free(l3);
    SCR_add_ptr(&res, &nl, NULL);
    return(res);
}

/**
 *  List calculus: 4 operations between 2 lists. Union (+), Intersection (*), Difference (-) and Product (x).
 *
 *  Syntax
 *  ------
 *      $DataCalcLst res lst1 op lst2
 *            where op =  + : union
 *                        * : intersection
 *                        - : difference
 *                        x : product
 *  
 * for details
 *  
 *  @params See file header 
 *  @return int     0 on success
 *                  -1 on error (list non found...)
 */
int B_DataCalcLst(char* arg, int unused)
{
    int rc = 0;
    unsigned char **args = NULL, **l1 = NULL, **l2 = NULL, **lst = NULL,
                  *res, *list1, *list2, *op;

    /* arg: res list1 op list2 */
    args = (unsigned char**) B_vtom_chk(arg, 4);
    if(args == NULL) 
        return -1;
    
    res   = args[0];
    list1 = args[1];
    op    = args[2];
    list2 = args[3];

    if(!global_ws_lst->contains((char*) list1))
    {
        std::string error_msg = "List '" + std::string((char*) list1);
        error_msg += "' not found in the Lists workspace";
        error_manager.append_error(error_msg);
        rc = -1;
        goto done;
    }

    if(!global_ws_lst->contains((char*) list2))
    {
        std::string error_msg = "List '" + std::string((char*) list2);
        error_msg += "' not found in the Lists workspace";
        error_manager.append_error(error_msg);
        rc = -1;
        goto done;
    }

    l1 = (unsigned char**) B_ainit_chk(global_ws_lst->get_obj((char*) list1), NULL, 0);
    l2 = (unsigned char**) B_ainit_chk(global_ws_lst->get_obj((char*) list2), NULL, 0);
    switch(op[0]) 
    {
    case '+' :
        lst = SCR_union(l1, l2);
        break;
    case '*' :
        lst = SCR_inter(l1, l2);
        break;
    case '-' :
        lst = SCR_dif(l1, l2);
        break;
    case 'x' :
        lst = Lst_times(l1, l2);
        break;
    default  :
        rc = -1;
        goto done;
    }
    
    rc = KL_lst((char*) res, (char**) lst, -1); 

done :
    A_free((unsigned char**) args);
    A_free((unsigned char**) l1);
    A_free((unsigned char**) l2);
    SCR_free_tbl(lst);
    return(rc);
}

/**
 *  Returns the number of elements in a list.
 *  
 *  @param [in] char*   name    list_name
 *  @return     int             nb of elements in the list or -1 if the list does not exist
 */
int B_DataListCount(char* name, int unused)
{
    char* lst = (char*) SCR_stracpy((unsigned char*) global_ws_lst->get_obj(name));
    if(lst == NULL) 
        return -1;

    char** lsti = B_ainit_chk(lst, NULL, 0);
    int nb = SCR_tbl_size((unsigned char**) lsti);
    SCR_free_tbl((unsigned char**) lsti);
    SCR_free(lst);

    return nb;
}

/**
 *  Defines the threshold under which the difference between 2 variables are considered equal.
 *
 *  Syntax
 *  ------
 *      $DataCompareEps eps
 *  
 * for details
 *  
 *  @params See file header 
 *  @return int     0 always
 */
int B_DataCompareEps(char* arg, int unused)
{
    // Set Vars threshold
    K_CMP_EPS = atof(arg);
    if(K_CMP_EPS < 1e-15) K_CMP_EPS = 1e-15;
    if(K_CMP_EPS > 1) K_CMP_EPS = 1;
    return(0);
}

template<typename T>
int template_data_compare(const std::string& filename, const std::string& one, const std::string& two, 
    const std::string& three, const std::string& four)
{
    T kdb2(false);
    kdb2.load(filename);
    if(kdb2.size() == 0)
        return -1;

    int rc = 0;
    char* c_name = NULL;
    int n1 = 0, n2 = 0, n3 = 0, n4 = 0;
    char **l1 = NULL, **l2 = NULL, **l3 = NULL, **l4 = NULL;

    try
    {
        int type = (int) kdb2.k_type;
        KDB& kdb1 = get_global_db(type);
    
        // K_cmp() return codes:
        //      0 -> if name neither in global_db nor in file
        //      1 -> if name in global_db but not in file
        //      2 -> if name not in global_db but in file
        //      3 -> if name in both global_db and file, IODE object in global_db == IODE object in file
        //      4 -> if name in both global_db and file, IODE object in global_db != IODE object in file
        std::string name;
        for(int i = 0; i < kdb1.size(); i++) 
        {
            name = kdb1.get_name(i);
            c_name = (char*) name.c_str();
            rc = K_cmp(c_name, &kdb1, &kdb2);
            switch(rc)
            {
            // name neither in global_db nor in file
            case 0 :
                break;
            // name in global_db but not in file
            case 1 :
                SCR_add_ptr((unsigned char***) &l1, &n1, (unsigned char*) c_name);
                break;
            // if name not in global_db but in file
            case 2 :
                SCR_add_ptr((unsigned char***) &l2, &n2, (unsigned char*) c_name);
                break;
            // name in both global_db and file, IODE object in global_db == IODE object in file
            case 3 :
                SCR_add_ptr((unsigned char***) &l3, &n3, (unsigned char*) c_name);
                break;
            // name in both global_db and file, IODE object in global_db != IODE object in file
            case 4 :
                SCR_add_ptr((unsigned char***) &l4, &n4, (unsigned char*) c_name);
                break;
            }
    
            if(rc > 2) 
                kdb2.remove(name);
        }
    }
    catch (const std::runtime_error& e) 
    {
        error_manager.append_error("DataCompare: " + std::string(e.what()));
        return -1;
    }

    for(int i = 0; i < kdb2.size(); i++)
    {
        c_name = (char*) kdb2.get_name(i).c_str();
        SCR_add_ptr((unsigned char***) &l2, &n2, (unsigned char*) c_name);
    }

    SCR_add_ptr((unsigned char***) &l1, &n1, NULL);
    SCR_add_ptr((unsigned char***) &l2, &n2, NULL);
    SCR_add_ptr((unsigned char***) &l3, &n3, NULL);
    SCR_add_ptr((unsigned char***) &l4, &n4, NULL);

    rc = KL_lst((char*) one.c_str(),   l1, 10000);
    rc = KL_lst((char*) two.c_str(),   l2, 10000);
    rc = KL_lst((char*) three.c_str(), l3, 10000);
    rc = KL_lst((char*) four.c_str(),  l4, 10000);

    SCR_free_tbl((unsigned char**) l1);
    SCR_free_tbl((unsigned char**) l2);
    SCR_free_tbl((unsigned char**) l3);
    SCR_free_tbl((unsigned char**) l4);

    return rc;
}

/**
 *  Compares the objects in the current WS to the content of an IODE file and stores the results in 4 lists.
 *    
 *  Syntax
 *  ------
 *      $DataCompare<type> filename ONE TWO THREE FOR
 *      where
 *            ONE         in WS only
 *            TWO         in file only
 *            THREE       in both, equal
 *            FOUR        in both, different
 *  
 * for details
 *  
 *  @params See file header 
 *  @return int     -1 on error (incorrect syntax, file not found...
 *                  0 on success
 */
int B_DataCompare(char* arg, int type)
{
    char** args = B_vtom_chk(arg, 5);
    if(args == NULL) 
        return -1;
    
    std::string filename = std::string(args[0]);

    std::string one   = std::string(args[1]);
    std::string two   = std::string(args[2]);
    std::string three = std::string(args[3]);
    std::string four  = std::string(args[4]);

    SCR_free_tbl((unsigned char**) args);

    int rc = -1;
    switch(type) 
    {
    case COMMENTS:
        rc = template_data_compare<KDBComments>(filename, one, two, three, four);
        break;
    case EQUATIONS:
        rc = template_data_compare<KDBEquations>(filename, one, two, three, four);
        break;
    case IDENTITIES:
        rc = template_data_compare<KDBIdentities>(filename, one, two, three, four);
        break;
    case LISTS:
        rc = template_data_compare<KDBLists>(filename, one, two, three, four);
        break;
    case SCALARS:
        rc = template_data_compare<KDBScalars>(filename, one, two, three, four);
        break;
    case TABLES:
        rc = template_data_compare<KDBTables>(filename, one, two, three, four);
        break;
    case VARIABLES:
        rc = template_data_compare<KDBVariables>(filename, one, two, three, four);
        break;
    default:
        {
            std::string msg = "DataCompare: invalid object type " + std::to_string(type);
            kwarning(msg.c_str());
            return -1;
        }
    }

    return rc;
}

/**
 *  Sub function of B_DataDisplayGraph() and B_DataPrintGraph().
 */
static int B_DataEditGraph(int view, char* arg)
{
    int     rc = 0, nb_args, mode, type, xgrid, ygrid, axis;
    double  ymin, ymax;
    char    **args = NULL;
    Sample  *smpl = 0;

    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size((unsigned char**) args);    /* JMP 16-12-93 */
    if(nb_args < 10) 
    {
        error_manager.append_error("DataEditGraph : Syntax error");
        rc = -1;
        goto fin;
    }

    mode = get_pos_in_char_array("LDGdg", args[0][0]); /* GB 10/08/98 */
    mode = std::max(0, mode);

    type  = B_argpos("LSBM", args[1][0]);
    //xgrid = B_argpos("MNJ",  args[2][0]);
    //ygrid = B_argpos("MNJ",  args[3][0]);
    xgrid = B_argpos("JNM",  args[2][0]); // Corr VL 4/6/2018
    ygrid = B_argpos("JNM",  args[3][0]); // Id
 
    axis  = B_argpos("LGSP", args[4][0]);
    if(memcmp(args[5], "--", 2) == 0) ymin = IODE_NAN;
    else                              ymin = atof(args[5]);
    if(memcmp(args[6], "--", 2) == 0) ymax = IODE_NAN;
    else                              ymax = atof(args[6]);

    try
    {
        smpl = new Sample(std::string(args[7]), std::string(args[8]));
    }
    catch(const std::exception& e)
    {
        error_manager.append_error(std::string(e.what()));
        rc = -1;
        goto fin;
    }

    rc = V_graph(view, mode, type, xgrid, ygrid, axis, ymin, ymax,
		         smpl, args + 9);

fin:
    A_free((unsigned char**) args);
    if(smpl) delete smpl;
    smpl = nullptr;
    return rc;
}


/**
 *  This function allows you to view VARs or combinations of VARS in graphical form.
 *
 *  Syntax
 *  ------
 *      $DataDisplayGraph {Level | Diff | Grt | diffYoY | grtYoY}
 *                     {Line | Scatter | Bar | Mixt}
 *                     {NoXGrids | MinorXGrids | J(MajorXGrids)}
 *                     {NoYGrids | MinorYGrids | J(MajorYGrids)}
 *                     {Level | G(Log) | Semi-Log | Percent}
 *                     {ymin | --} {ymax | --}
 *                     perfrom perto varname1 varname2 ...
 *  
 * for details
 *  
 *  @params See file header 
 */
 
int B_DataDisplayGraph(char* arg, int unused)
{
    return(B_DataEditGraph(1, arg));
}


/**
 *  This function allows you to print VARs or combinations of VARS in graphical form.
 *
 *  Syntax
 *  ------
 *      $DataPrintGraph   {Level | Diff | Grt | diffYoY | grtYoY}
 *                 {Line | Scatter | Bar | Mixt}
 *                 {NoXGrids | MinorXGrids | J(MajorXGrids)}
 *                 {NoYGrids | MinorYGrids | J(MajorYGrids)}
 *                 {Level | G(Log) | Semi-Log | Percent}
 *                 {ymin | --} {ymax | --}
 *                 perfrom perto varname1 varname2 ...
 *  
 *  Example
 *  -------
 *      $DataPrintGraph  Level Line No No Level -- -- 1980Y1 1995Y1 X1 Y1 X1+Y1 
 *  
 * for details
 *  
 *  @params See file header 
 */

int B_DataPrintGraph(char* arg, int unused) 
{
    return(B_DataEditGraph(0, arg));
}


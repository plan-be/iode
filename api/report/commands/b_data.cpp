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
 *  @see https://iode.plan.be/doku.php?id=datacalcvar for details
 *  
 *  @params See file header 
 *  @return int     0 on success, 
 *                  -1 on error: variable cannot be created, error in LEC formula, link impossible...
 */
int B_DataCalcVar(char* arg, int unused)
{
    char        name[K_MAX_NAME + 1], *lec;
    int         lg, t, nb;
    KDB         *kdb = K_WS[VARIABLES];
    CLEC        *clec = 0;
    int         pos;
    double      d;

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);

    lec = arg + lg + 1;
    SCR_strip((unsigned char*) lec);

    nb = kdb->sample->nb_periods;
    pos = kdb->index_of(name);
    if(pos < 0) 
        pos = K_add(kdb, name, NULL, &nb);
    if(pos < 0) 
        return(-1);

    if(lec[0]) 
    {
        clec = L_cc(lec);
        if(clec != 0 && !L_link(kdb, K_WS[SCALARS], clec)) 
        {
            for(t = 0 ; t < kdb->sample->nb_periods ; t++) 
            {
                d = L_exec(kdb, K_WS[SCALARS], clec, t);
                *(KVVAL(kdb, pos, t)) = d;
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
    int     nb_per;
    char    deflt[41];
    KDB     *kdb = K_WS[*ptype];

    if(kdb->index_of(arg) >= 0) return(-1);

    switch(*ptype) {
        case COMMENTS :
        case LISTS :
        case SCALARS :
            if(K_add(kdb, arg, NULL) < 0) return(-1);
            else return(0);

        case VARIABLES :
            nb_per = kdb->sample->nb_periods;
            if(K_add(kdb, arg, NULL, &nb_per) < 0) return(-1);
            else return(0);

        case EQUATIONS :
            sprintf(deflt, "%s := %s", arg, arg);
            return(K_upd_eqs(arg, deflt, 0L, 0, 0L, 0L, 0L, 0L, 0));

        case IDENTITIES :
            sprintf(deflt, "%s", arg);
            if(K_add(kdb, arg, deflt) < 0) return(-1);
            else return(0);
        case TABLES :
            return(K_upd_tbl(arg, "TITLE;LEC"));
    }
    return(-1);
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
 *  @see https://iode.plan.be/doku.php?id=datacalcvar for details
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
    int     pos;
    KDB     *kdb = K_WS[*ptype];

    pos = kdb->index_of(arg);
    if(pos < 0 || K_del(kdb, pos) < 0) {
        error_manager.append_error("Failed to delete '" + std::string(arg) + "'");
        return(-1);
    }
    return(0);
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
 *  @see https://iode.plan.be/doku.php?id=datadelete for details
 *  
 *  @params See file header 
 *  @return int     0 on success, 
 *                  -1 on error: variable cannot be deleted (does not exist)
 */
int B_DataDelete(char* arg, int type)
{
    char    *lst;
    int     rc;

    lst = K_expand(type, NULL, arg, '*');  /* JMP 23-12-98 */
    rc = B_ainit_loop(lst, wrapper_B_DataDelete_1, (char *)&type);
    SCR_free(lst);
    return(rc);
}


/**
 *  Renames an object. Equations cannot be renamed.
 *  
 *  Syntax:  $DataRename<type> old_name new_name
 *  
 *  @see https://iode.plan.be/doku.php?id=datarename for details
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

    if(type == EQUATIONS) return(-1); /* Rename of EQS has no sense */

    args = B_ainit_chk(arg, NULL, 2);
    if(args == NULL) return(-1);

    if(K_ren(K_WS[type], args[0], args[1]) < 0) {
        std::string error_msg = "DataRename '" + std::string(args[0]) + "' to '";
        error_msg += std::string(args[1]) + "' failed";
        error_manager.append_error(error_msg);
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
 *  @see https://iode.plan.be/doku.php?id=dataduplicate for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be renamed
 */
 int B_DataDuplicate(char* arg, int type)
{
    int     rc = 0;
    char    **args;
    KDB     *kdb = K_WS[type];

    if(type == EQUATIONS) {
        error_manager.append_error("DataDuplicate of Equations has no sense");
        return(-1); /* Duplicate of EQS has no sense */
    }

    args = B_ainit_chk(arg, NULL, 2);
    if(args == NULL) return(-1);

    if(K_dup(kdb, args[0], kdb, args[1]) < 0) {
        std::string error_msg = "DataDuplicate '" + std::string(args[0]) + "' as '";
        error_msg += std::string(args[1]) + " failed";
        error_manager.append_error(error_msg);
        rc = -1;
    }

    A_free((unsigned char**) args);
    return(rc);
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
 *  @see https://iode.plan.be/doku.php?id=dataupdate for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be updated
 */

int B_DataUpdate(char* arg, int type)
{
    int     pos, shift, lg, rc = 0,
                            nb_args, nb_upd, nb_p,
                            i, mode;
    double  var;
    KDB     *kdb = K_WS[type];
    Scalar     scl;
    Period  *per = NULL;
    char    name[K_MAX_NAME + 1], **args = NULL;

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    pos = kdb->index_of(name);
    if(pos < 0) {
        if(B_DataCreate(name, type)) 
            return(-1);
        pos = kdb->index_of(name);
    }

    switch(type) {
    case COMMENTS : /* Name Val */
    case IDENTITIES :
    case LISTS :
        rc = K_add(kdb, name, arg + lg + 1, name);
        break;

    case EQUATIONS :
        rc = K_upd_eqs(name, arg + lg + 1, NULL, -1, NULL, NULL, NULL, NULL, 0);
        break;

    case TABLES :
        rc = K_upd_tbl(name, arg + lg + 1);
        break;

    case SCALARS : /* Name Val [Relax] */
        args = (char**) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
        nb_args = SCR_tbl_size((unsigned char**) args);
        scl.value = 0.9;
        scl.relax = 1.0;
        scl.std = IODE_NAN;

        switch(nb_args) {
        case 2:
            scl.value = (double) atof(args[1]);
            break;

        case 3:
            scl.value = (double) atof(args[1]);
            scl.relax = (double) atof(args[2]);
            break;
        default :
            error_manager.append_error("DataUpdateScl : Invalid Argument");
            rc = -1;
            break;
        }
        if(rc == 0) 
            rc = K_add(kdb, args[0], &scl);
        break;

    case VARIABLES : /* Name [D|d|G|g|L|l] Period nVal */
        args = (char**) SCR_vtoms((unsigned char*) arg, (unsigned char*) B_SEPS);
        nb_args = SCR_tbl_size((unsigned char**) args);
        if(nb_args > 1) {
            nb_p = 1;
            switch(args[1][0]) {
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
            if(per == 0) {
                error_manager.append_error("Syntax error: Period not defined"); /* JMP 23-05-00 */
                rc = -1;
                break;
            }
            nb_upd = std::min(nb_upd, (kdb->sample->end_period.difference(*per) + 1));
            shift = per->difference(kdb->sample->start_period);
            if(per == NULL || shift < 0)
                rc = -1;
            else 
            {
                nb_p ++;
                for(i = 0; i < nb_upd; i++) {
                    var = (double) atof(args[i + nb_p]);
                    if(var == 0.0 && !U_is_in(args[i + nb_p][0], "-0.+")) 
                        var = IODE_NAN; /* JMP 06-09-2004 */
                    KV_set(kdb, pos, shift + i, mode, var);
                }
            }
        }
        else 
            rc = -1;

        if(rc < 0) 
            error_manager.append_error("DataUpdateVar : '" + std::string(arg) + "' Invalid Argument");
        break;
    }

    A_free((unsigned char**) args);
    delete per;
    per = nullptr;

    if(rc >= 0) 
        rc = 0;

    if(rc < 0) 
        rc = -1; // Pour éviter return dans les rapports si rc = -2

    return(rc);
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
 *  @see https://iode.plan.be/doku.php?id=datasearch for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be renamed
 */
char** B_DataSearchParms(char* name, int word, int ecase, int names, int forms, int texts, int type)
{
    int     rc = 0;
    char    **args = NULL, buf[81];
    KDB     *kdb = K_WS[type];

    
    if(word == 1) {
        strcpy(buf, "*!");
        SCR_strlcpy(((unsigned char*) buf) + 2, (unsigned char*) name, 76);
        strcat(buf, "!*");
    }
    else {
        buf[0] = '*';
        SCR_strlcpy(((unsigned char*) buf) + 1, (unsigned char*) name, 76);
        strcat(buf, "*");
    }

    return(K_grep(kdb, buf, ecase, names, forms, texts, '*'));
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
 *  @see https://iode.plan.be/doku.php?id=datasearch for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be renamed
 */
int B_DataSearch(char* arg, int type)
{
    int     rc = 0, word, ecase, names, forms, texts;
    char    **args = NULL, **lst;
    char    **K_grep();
    KDB     *kdb = K_WS[type];

    args = B_vtom_chk(arg, 7); /* pattern list */
    if(args == NULL) return(-1);
    
    word  = atoi(args[1]);
    ecase = atoi(args[2]);
    names = atoi(args[3]);
    forms = atoi(args[4]);
    texts = atoi(args[5]);

/*
    SCR_strlcpy(buf + 1, args[0], 76);
    if(word == 1) {
        strcpy(buf, "*!");
        SCR_strlcpy(buf + 2, args[0], 76);
        strcat(buf, "!*");
    }
    else {
        buf[0] = '*';
        SCR_strlcpy(buf + 1, args[0], 78);
        strcat(buf, "*");
    }

    lst = K_grep(kdb, buf, ecase, names, forms, texts, '*');
*/
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
 *  @see https://iode.plan.be/doku.php?id=datalistsort for details
 *  
 *  @params See file header 
 *  @return int     0 on success, 
 *                  -1 on error: variable cannot be created
 */

int B_DataListSort(char* arg, int unused)
{
    int     rc = 0, p;
    char    *in, *out, *lst,
            **args = NULL, **lsti = NULL,
            *old_A_SEPS;

    args = B_vtom_chk(arg, 2); /* in out */
    //if(args == NULL) in = out = arg;
    if(args == NULL) return(-1);
    else {
        in = args[0];
        out = args[1];
    }

    p = KL_WS->index_of(in);

    if(p < 0) {
        error_manager.append_error("List '" + std::string(args[0]) + 
                                   "' not found in the Lists workspace");
        rc = -1;
        goto done;
    }
    else lst = KLVAL(KL_WS, p);
    if(lst == NULL) {
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
    if(lsti == NULL) {
        rc = -1;
        goto done;
    }

    qsort(lsti, SCR_tbl_size((unsigned char**) lsti), sizeof(char **), my_strcmp);
    lst = (char*) SCR_mtov((unsigned char**) lsti, ';');  /* JMP 09-03-95 */

    if(K_add(K_WS[LISTS], out, lst) < 0) {
        error_manager.append_error("Sorted List '" + std::string(out) + "' cannot be created");
        rc = -1;
    }

    SCR_free_tbl((unsigned char**) lsti);
    SCR_free(lst);

done:
    A_free((unsigned char**) args);
    return(rc);
}


/**
 *  Analyses a KDB content and creates 2 lists _EXO and _SCAL with all VAR and all Scalar found in the kdb objects (limited to IDT, EQ or Table).
 *  
 *  Syntax
 *  ------
 *      $DataScan<type> [name1 name2...]
 *  
 *  @see https://iode.plan.be/doku.php?id=datascan for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success, 
 *                  -1 on error: object cannot be renamed
 */

int B_DataScan(char* arg, int type)
{
    KDB     *tkdb = NULL;
    char    **objs;
    int     rc = -1;

    if(type != IDENTITIES && type != EQUATIONS && type != TABLES) 
    {
        error_manager.append_error("DataScan : only IDT, EQ and Table are supported");
        return(-1);
    }

    if(arg == NULL || arg[0] == 0) 
    {
        /* Exo whole WS */
        return(K_scan(K_WS[type], "_EXO", "_SCAL"));
    }
    else 
    {
        objs = B_ainit_chk(arg, NULL, 0);
        if(objs == NULL || SCR_tbl_size((unsigned char**) objs) == 0)
            return(K_scan(K_WS[type], "_EXO", "_SCAL"));
        else 
        {
            tkdb = K_refer(K_WS[type], SCR_tbl_size((unsigned char**) objs), objs);
            if(tkdb == 0 || tkdb->size() == 0)            /* JMP 12-05-11 */
                rc = -1;                                /* JMP 12-05-11 */
            else                                        /* JMP 12-05-11 */
                rc = K_scan(tkdb, "_EXO", "_SCAL");     /* JMP 12-05-11 */

            delete tkdb;
            tkdb = nullptr;
            SCR_free_tbl((unsigned char**) objs);
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
 *  @see https://iode.plan.be/doku.php?id=dataexist for details
 *  
 *  @params See file header 
 *  
 *  @return int     >= if object exist (return its position)
 *                  -1 if object is not found
 */
int B_DataExist(char* arg, int type)
{
    KDB     *kdb = K_WS[type];

    return(kdb->index_of(arg));
}


/**
 *  Appends data (a string) to a CMT or a LST.
 *  
 *  Syntax
 *  ------
 *      $DataAppend<type> name txt 
 *  
 *  @see https://iode.plan.be/doku.php?id=dataappend for details
 *  
 *  @params See file header 
 *  
 *  @return int     0 on success
 *                  -1 if type is incorrect or the object cannot be created / expanded 
 */
int B_DataAppend(char* arg, int type)
{
    int     pos, lg;
    KDB     *kdb = K_WS[type];
    char    name[K_MAX_NAME + 1], *ptr, *nptr, *text;

    switch(type) {
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
    pos = kdb->index_of(name);

    if(pos < 0)
        nptr = text;
    else {
        if(strlen(text) == 0) return(0); /* empty append */

        ptr = KOVAL(kdb, pos);
        nptr = SW_nalloc((int)strlen(ptr) + (int)strlen(text) + 2);
        if(type == COMMENTS)
            sprintf(nptr, "%s %s", ptr, text);
        else
            sprintf(nptr, "%s,%s", ptr, text);
    }

    pos = K_add(kdb, name, nptr);
    if(nptr != text) SW_nfree(nptr);

    if(pos < 0) return(-1);
    else return(0);
}


/**
 *  Constructs a list of objects matching a given name pattern. Objects can be in WS or in a file. 
 *  The pattern can contain * to allow any suite of characters.
 *  
 *  Syntax
 *  ------
 *      $DataList<type> listname pattern [filename]
 *  
 *  @see https://iode.plan.be/doku.php?id=datalistxxx for details
 *  
 *  @params See file header 
 *  @return int     0 on success
 *                  -1 if type is incorrect or the object cannot be created / expanded 
 */
int B_DataList(char* arg, int type)
{
    int     rc = 0;
    char    **args = NULL, **lst,
            *name, *pattern, *file;
    KDB*    kdb = new KDB(*K_WS[type]);

    if((args = B_vtom_chk(arg, 3)) == NULL) 
    {
        if((args = B_vtom_chk(arg, 2)) == NULL) 
            return(-1);
        else 
        {
            name    = args[0];
            pattern = args[1];
            file    = NULL;
        }
    }
    else 
    {
        name    = args[0];
        pattern = args[1];
        file    = args[2];
    }

    if(file == NULL) 
        lst = K_grep(kdb, pattern, 0, 1, 0, 0, '*');
    else 
    {
        kdb = K_interpret(type, file, 0);
        if(!kdb) 
            return(-1);
        lst = K_grep(kdb, pattern, 0, 1, 0, 0, '*');
        delete kdb;
        kdb = nullptr;
    }

    rc = KL_lst(name, lst, 200);
    SCR_free_tbl((unsigned char**) lst);
    A_free((unsigned char**) args);

    return(rc);
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
    for(i = 0; i < inb; i++) {
        ll1 = (int) strlen((char*) l1[i]);
        for(j = 0; j < jnb; j++) {
            ll2 = (int) strlen((char*) l2[j]);

            if(ll3 < ll1 + ll2 + 1) {
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
 *  @see https://iode.plan.be/doku.php?id=datacalclst for details
 *  
 *  @params See file header 
 *  @return int     0 on success
 *                  -1 on error (list non found...)
 */
int B_DataCalcLst(char* arg, int unused)
{
    int             rc = 0, p1, p2;
    unsigned char   **args = NULL, **l1 = NULL, **l2 = NULL, **lst = NULL,
                      *res, *list1, *list2, *op;

    /* arg: res list1 op list2 */
    if((args = (unsigned char**) B_vtom_chk(arg, 4)) == NULL) return(-1);
    res  = args[0];
    list1 = args[1];
    op   = args[2];
    list2 = args[3];

    p1 = K_WS[LISTS]->index_of((char*) list1);
    p2 = K_WS[LISTS]->index_of((char*) list2);
    if(p1 < 0 || p2 < 0) {
        std::string error_msg = "List '";
        error_msg += (p1 < 0) ? std::string((char*) list1) : std::string((char*) list2);
        error_msg += "' not found in the Lists workspace";
        error_manager.append_error(error_msg); // JMP 4/02/09
        rc = -1;
        goto done;
    }

    l1 = (unsigned char**) B_ainit_chk(KLVAL(K_WS[LISTS], p1), NULL, 0);
    l2 = (unsigned char**) B_ainit_chk(KLVAL(K_WS[LISTS], p2), NULL, 0);
    switch(op[0]) {
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
 *  @param [in] char*   arg     list_name
 *  @return     int             nb of elements in the list or -1 if the list does not exist
 */
int B_DataListCount(char* arg, int unused)
{
    int     nb = 0;
    char    *lst,
            **lsti = NULL;

    lst = (char*) SCR_stracpy((unsigned char*) KLVAL(KL_WS, KL_WS->index_of(arg)));
    if(lst == NULL) return(-1);

    lsti = B_ainit_chk(lst, NULL, 0);
    nb = SCR_tbl_size((unsigned char**) lsti);
    SCR_free_tbl((unsigned char**) lsti);
    SCR_free(lst);

    return(nb);
}


/**
 *  Defines the threshold under which the difference between 2 variables are considered equal.
 *
 *  Syntax
 *  ------
 *      $DataCompareEps eps
 *  
 *  @see https://iode.plan.be/doku.php?id=datacompareeps for details
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
 *  @see https://iode.plan.be/doku.php?id=datacomparexxx for details
 *  
 *  @params See file header 
 *  @return int     -1 on error (incorrect syntax, file not found...
 *                  0 on success
 */
 
int B_DataCompare(char* arg, int type)
{
    int     i, rc = 0,
               n1 = 0, n2 = 0, n3 = 0, n4 = 0;
    char    **args = NULL, *name,
            **l1 = NULL, **l2 = NULL, **l3 = NULL, **l4 = NULL,
            *file, *one, *two, *three, *fr;
    KDB*    kdb1 = K_WS[type];
    KDB*    kdb2 = new KDB((IodeType) type, DB_STANDALONE);

    if((args = B_vtom_chk(arg, 5)) == NULL) 
        return(-1);
    
    file  = args[0];
    one   = args[1];
    two   = args[2];
    three = args[3];
    fr    = args[4];

    kdb2 = K_interpret(type, file, 0);
    if(!kdb2) 
        return(-1);

    for(i = 0; i < kdb1->size(); i++) 
    {
        name = KONAME(kdb1, i);
        rc = K_cmp(name, kdb1, kdb2);
        switch(rc) {
        case 1 :
            SCR_add_ptr((unsigned char***) &l1, &n1, (unsigned char*) name);
            break;
        case 2 :
            SCR_add_ptr((unsigned char***) &l2, &n2, (unsigned char*) name);
            break; /* never reached */
        case 3 :
            SCR_add_ptr((unsigned char***) &l3, &n3, (unsigned char*) name);
            break;
        case 4 :
            SCR_add_ptr((unsigned char***) &l4, &n4, (unsigned char*) name);
            break;
        }

        if(rc > 2) 
            K_del(kdb2, kdb2->index_of(name)) ;
    }

    for(i = 0; i < kdb2->size(); i++) 
        SCR_add_ptr((unsigned char***) &l2, &n2, (unsigned char*) KONAME(kdb2, i));

    SCR_add_ptr((unsigned char***) &l1, &n1, NULL);
    SCR_add_ptr((unsigned char***) &l2, &n2, NULL);
    SCR_add_ptr((unsigned char***) &l3, &n3, NULL);
    SCR_add_ptr((unsigned char***) &l4, &n4, NULL);

    /*
        rc = KL_lst(one,    l1, 200);
        rc = KL_lst(two,    l2, 200);
        rc = KL_lst(three,  l3, 200);
        rc = KL_lst(fr,     l4, 200);
    */
    rc = KL_lst(one,    l1, 10000);
    rc = KL_lst(two,    l2, 10000);
    rc = KL_lst(three,  l3, 10000);
    rc = KL_lst(fr,     l4, 10000);

    SCR_free_tbl((unsigned char**) l1);
    SCR_free_tbl((unsigned char**) l2);
    SCR_free_tbl((unsigned char**) l3);
    SCR_free_tbl((unsigned char**) l4);

    A_free((unsigned char**) args);
    if(kdb2)
    {
        delete kdb2;
        kdb2 = nullptr;
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
 *  @see https://iode.plan.be/doku.php?id=datadisplaygraph for details
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
 *  @see https://iode.plan.be/doku.php?id=dataprintgraph for details
 *  
 *  @params See file header 
 */

int B_DataPrintGraph(char* arg, int unused) 
{
    return(B_DataEditGraph(0, arg));
}


/**
 *  @header4iode
 * 
 *  Report functions related to WS management (clear, load, save, sample...). 
 *  
 *  See b_data.c for more details on the report function syntax and logic.
 *  
 *  List of functions 
 *  -----------------
 *  
 * int B_WsLoad(char* arg, int type)                 $WsLoad<type> filename
 * int X_findtype(char* filename)                    Returns the type of content of filename according to its extension
 * int B_WsDump(KDB* kdb, char* filename)            Dumps the content of KDB in a file
 * int B_WsSave(char* arg, int type)                 $WsSave<type> filename
 * int B_WsSaveCmp(char* arg, int type)              $WsSaveCmp<type> filename
 * int B_WsExport(char* arg, int type)               $WsExport<type> filename
 * int B_WsImport(char* arg, int type)               $WsImport<type> filename
 * int B_WsSample(char* arg, int unused)                         $WsSample period_from period_to
 * int B_WsClear(char* arg, int type)                $WsClear<type> 
 * int B_WsClearAll(char* arg, int unused)                       $WsClearAll
 * int B_WsDescr(char* arg, int type)                $WsDescr<type> free text
 * int B_WsName(char* arg, int type)                 Sets the WS name. Obsolete as report function.
 * int B_WsCopy(char* arg, int type)                 $WsCopy<type> fichier;fichier;.. obj1 obj2... or $WsCopyVar file;file;.. [from to] obj1 obj2...
 * int B_WsMerge(char* arg, int type)                $WsMerge<type> filename
 * int B_WsExtrapolate(char* arg, int unused)                    $WsExtrapolate [method] from to [variable list]
 * int B_WsAggrChar(char* arg, int unused)                       $WsAggrChar char
 * int B_WsAggrSum(char* arg, int unused)                        $WsAggrSum pattern filename
 * int B_WsAggrProd(char* arg, int unused)                       $WsAggrProd pattern filename
 * int B_WsAggrMean(char* arg, int unused)                       $WsAggrMean pattern filename
 * double *B_StatUnitRoot_1(char* arg, int print) Sub function of B_StatUnitRoot() with an optional parameter to print the result (or not).
 * int B_StatUnitRoot(char* arg, int unused)                     $StatUnitRoot drift trend order expression
 * int B_CsvSave(char* arg, int type)                $CsvSave<type> file name1 name2 ...
 * int B_CsvNbDec(char *nbdec, int unused)                       $CsvNbDec nn
 * int B_CsvSep(char *sep, int unused)                           $CsvSep char
 * int B_CsvNaN(char *nan, int unused)                           $CsvNaN text
 * int B_CsvAxes(char *var, int unused)                          $CsvAxes AxisName
 * int B_CsvDec(char *dec, int unused)                           $CsvDec char
 */
#include "scr4/s_args.h"
#include "api/k_super.h"
#include "api/b_args.h"
#include "api/b_errors.h"
#include "api/ascii/ascii.h"
#include "api/objs/objs.h"
#include "api/objs/grep.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"
#include "api/estimation/estimation.h"
#include "api/report/commands/commands.h"


/**
 *  Syntax: $WsLoad<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @param [in] char*   arg     parameters if the command on the report line
 *  @param [in] int     type    object type
 *  @return     int             0 on success, -1 on error
 *  
 *  @see https://iode.plan.be/doku.php?id=wsload
 */
 
int B_WsLoad(char* arg, int type)
{
    char buf[K_MAX_FILE + 1];

    SCR_strlcpy((unsigned char*) buf, (unsigned char*) arg, K_MAX_FILE);
    K_strip(buf);
    if(buf[0] == 0) 
        return 0;

    KDB* kdb = K_interpret(type, buf, 1);
    if(!kdb) 
        return -1;

    switch(type) 
    {
        case COMMENTS :    
            KC_WS.reset(kdb); 
            break;
        case EQUATIONS :   
            KE_WS.reset(kdb);
            break;
        case IDENTITIES :  
            KI_WS.reset(kdb);
            break;
        case LISTS :       
            KL_WS.reset(kdb);
            break;
        case SCALARS :     
            KS_WS.reset(kdb);
            break;
        case TABLES :      
            KT_WS.reset(kdb);
            break;
        case VARIABLES :   
            KV_WS.reset(kdb);
            break;
        default:
            kerror(0, "B_WsLoad: unknown type %d", type);
            return -1;
    }

    // get the list of all object names in 'kdb'
    int i = 0;
    int nb_names = (int) kdb->size();
    char** all_names = new char*[nb_names];
    for (const auto& [name, _] : kdb->k_objs)
        all_names[i++] = (char*) SCR_stracpy((unsigned char*) name.c_str());

    if(K_RWS[type][0])
        delete K_RWS[type][0];
    K_RWS[type][0] = K_quick_refer(kdb, nb_names, all_names);

    delete[] all_names;
    return 0;
}


/**
 *  Returns the type of content of filename according to its extension:
 *    - 0 -> 6 = .cmt, ..., .var (binary format)
 *    - 10 -> 16 = ac, ... av
 *    - 26 = csv
 *    - 22 = rep (TODO: check this)
 *    - autres formats (rep, a2m, ..., csv)
 *    - -1 -> undefined
 *  
 *  @global     char**  k_ext       recognized extensions
 *  @param [in] char*   filename    file to analyse
 *  @return     int                 file type (see above)
 */
int X_findtype(char* filename)
{
    int         i, lg = (int)strlen(filename);
    char        buf[5];

    // Check std extensions .cmt => .var
    if(lg > 4) {
        for(i = 0 ; i < 7 ; i++) {
            if(filename[lg - 4] == '.' &&
                    SCR_cstrcmp((unsigned char*) k_ext[i], ((unsigned char*) filename) + lg - 3) == 0) return(i);
        }
    }

    // Check ascii extensions .ac => .av
    if(lg > 3) {
        strcpy(buf, ".ac");
        for(i = 0 ; i < 7 ; i++) {
            buf[2] = k_ext[i][0];
            if(SCR_cstrcmp(((unsigned char*) filename) + lg - 3, (unsigned char*) buf) == 0) return(10 + i);
        }
    }

    // Other extensions
    //
    //if(lg > 4 && SCR_cstrcmp(filename + lg - 4, ".csv") == 0) return(21); // Correction JMP 16/1/2019
    //if(lg > 4 && SCR_cstrcmp(filename + lg - 4, ".rep") == 0) return(22); // Correction JMP 16/1/2019
    if(lg > 4 && SCR_cstrcmp(((unsigned char*) filename) + lg - 4, (unsigned char*) ".csv") == 0) return(FILE_CSV); // Correction JMP 25/3/2019
    if(lg > 4 && SCR_cstrcmp(((unsigned char*) filename) + lg - 4, (unsigned char*) ".rep") == 0) return(22); // ??? pas trés cohérent...

    // Sais plus a quoi ca peut servir... => a supprimer
    for(i = 16 ; strcmp(k_ext[i], "xxx") !=0 ; i++) {
        if(lg > 4 && SCR_cstrcmp(((unsigned char*) filename) + lg - 4, (unsigned char*) k_ext[i]) == 0) return(i); // Correction JMP 16/1/2019 : lg - 4 au lieu de -3
    }

    return(-1);
}


/**
 *  Dumps the content of KDB in a file: internal IODE format, ascii IODE format or CSV. 
 *  The extension of the filename determines if the output file must be in internal, ascii or csv format.
 *  
 *  @param [in] KDB*    kdb         KDB to dump
 *  @param [in] char*   filename    filename with extension
 *  @return     int                 return code of the K_save_*() function called according to filename extension and KDB type.
 *  
 */
int B_WsDump(KDB* kdb, char* filename)
{
    int     rc = -1, ftype, type = kdb->k_type;

    kmsg("Saving %s", filename); 
    ftype = X_findtype(filename);

    if(ftype >= 10 && ftype <= 17)
        rc = ascii_handlers[type]->save_asc(kdb, filename);
    else if(ftype <= 6)
        rc = K_save(kdb, filename);
    else if(ftype == FILE_CSV)
        rc = ascii_handlers[type]->save_csv(kdb, filename, NULL, NULL);

    return(rc);
}


/**
 *  Syntax: $WsSave<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 *  
 *  @see https://iode.plan.be/doku.php?id=wssave
 */
 
int B_WsSave(char* arg, int type)
{
    char    buf[K_MAX_FILE + 1];

    SCR_strip((unsigned char*) arg);
    if(strlen(arg) >= sizeof(FNAME)) {
        error_manager.append_error("Save workspace: filename too long");
        return(-1);
    }
    SCR_strlcpy((unsigned char*) buf, (unsigned char*) arg, K_MAX_FILE); /* JMP 18-04-98 */
    SCR_strip((unsigned char*) buf);
    if(buf[0] == 0) return(0);
    return(B_WsDump(get_global_db(type), buf));
}


/**
 *  Syntax: $WsSaveCmp<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}. 
 *  
 *  Same as $WsSave<type> but with LZH compression.
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 *  @see https://iode.plan.be/doku.php?id=wssave
 */
 
 int B_WsSaveCmp(char* arg, int type)
{
    char        buf[K_MAX_FILE + 1];
    int         rc = 0, klzh;

    SCR_strlcpy((unsigned char*) buf, (unsigned char*) arg, K_MAX_FILE); /* JMP 18-04-98 */
    SCR_strip((unsigned char*) buf);
    if(buf[0] == 0) return(0);
    klzh = K_LZH;
    K_LZH = 2;
    rc = B_WsDump(get_global_db(type), buf);
    K_LZH = klzh;
    return(rc);
}


/**
 *  Syntax: $WsExport<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 *  
 *  @see https://iode.plan.be/doku.php?id=wsexport
 */

int B_WsExport(char* arg, int type)
{
    return ascii_handlers[type]->save_asc(get_global_db(type), arg);
}


/**
 *  Syntax: $WsImport<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 *  
 *  @see https://iode.plan.be/doku.php?id=wsimport
 */

int B_WsImport(char* arg, int type)
{
    return(B_WsLoad(arg, type));
}


/**
 *  Syntax: $WsSample period_from period_to
 *  
 *  @see https://iode.plan.be/doku.php?id=wssample
 */

int B_WsSample(char* arg, int unused)
{
    Sample* new_smpl = nullptr;
    
    char** args = B_ainit_chk(arg, NULL, 2);
    if(args == NULL) 
        goto err;

    try
    {
        new_smpl = new Sample(std::string(args[0]), std::string(args[1]));
    }
    catch(const std::exception& e)
    {
        error_manager.append_error(e.what());
        goto err;
    }

    if(new_smpl->nb_periods <= 0)
    {
        error_manager.append_error("New sample invalid");
        goto err;
    }

    if(KV_sample(KV_WS.get(), new_smpl) < 0)
    {
        error_manager.append_error("New sample invalid");
        goto err;
    } 

    delete new_smpl;
    new_smpl = nullptr;
    A_free((unsigned char**) args);
    return 0;

err:
    if(new_smpl) delete new_smpl;
    new_smpl = nullptr;
    A_free((unsigned char **) args);
    return -1;
}


/**
 *  Syntax: $WsClear<type> 
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 *  
 *  @see https://iode.plan.be/doku.php?id=wsclear
 */

int B_WsClear(char* arg, int type)
{
    B_WsDescr("", type);
    B_WsName("", type);
    get_global_db(type)->clear();
    return 0;
}


/**
 *  Syntax: $WsClearAll
 *    
 *  @see https://iode.plan.be/doku.php?id=wsclearall
 */
int B_WsClearAll(char* arg, int unused)
{
    int i;

    for(i = COMMENTS; i <= VARIABLES; i++) B_WsClear("", i);
    return(0);
}


/**
 *  Syntax: $WsDescr<type> free text
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 *  
 *  @see https://iode.plan.be/doku.php?id=wsdescr
 */
 
int B_WsDescr(char* arg, int type)
{
    get_global_db(type)->description = std::string(arg);
    return 0;
}


/**
 *  Sets the WS name. Obsolete as report function.
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 */

int B_WsName(char* arg, int type)
{
    get_global_db(type)->filepath = std::string(arg);
    return 0;
}


/**
 *  Syntax: 
 *      $WsCopy<type> fichier;fichier;.. obj1 obj2... 
 *          where type is in {cmt | eqs | idt | lst | scl | tbl}
 *      $WsCopyVar file;file;.. [from to] obj1 obj2...
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 *  
 *  @see https://iode.plan.be/doku.php?id=wscopy
 */

int B_WsCopy(char* arg, int type)
{
    int     lg, shift = 0, rc = 0;
    char    file[K_MAX_FILE + 1], **files;
    char    **data, **data0, *lst;
    Sample  *smpl = nullptr;
    char    *oldseps = A_SEPS; // JMP 27/09/2022

    lg = B_get_arg0(file, arg, K_MAX_FILE);
    files = B_ainit_chk(file, NULL, 0);
    if(files == 0) 
        return(-1);

    //A_SEPS = " ;\t\n"; // JMP 27/09/2022
    A_SEPS = " ,;\t\n"; // JMP 25/04/2023
    lst = K_expand(type, files[0], arg + lg, '*');
    data0 = B_ainit_chk(lst, NULL, 0);
    SCR_free(lst);
    A_SEPS = oldseps; // JMP 27/09/2022   
    
    /*    data0 = B_ainit_chk(arg + lg, NULL, 0); */
    if(data0 == 0 || data0[0] == 0) 
    {   /* JMP 24-06-98 */
        error_manager.append_error("WsCopy : invalid argument(s)");
        SCR_free_tbl((unsigned char**) files);
        SCR_free_tbl((unsigned char**) data0);
        return(-1);
    }

    if(type == VARIABLES && SCR_tbl_size((unsigned char**) data0) >= 2) 
    {
        // check if 'from' and 'to' passed as arguments
        if(SCR_is_num(data0[0][0]) && SCR_is_num(data0[1][0]))
        {
            try
            {
                smpl = new Sample(std::string(data0[0]), std::string(data0[1]));
                shift = 2;
            }
            catch(const std::exception& e)
            {
                error_manager.append_error("WsCopy : invalid sample\n" + std::string(e.what()));
                SCR_free_tbl((unsigned char**) files);
                SCR_free_tbl((unsigned char**) data0);
                return(-1);
            }
        }
    }

    if(data0 + shift == NULL) 
    {
        error_manager.append_error("WsCopy : object list required");
        rc = -1;
    }
    else 
    {
        data = (char**) SCR_inter(((unsigned char**) data0) + shift, ((unsigned char**) data0) + shift);
        rc = K_copy(get_global_db(type),
                    SCR_tbl_size((unsigned char**) files), files,
                    SCR_tbl_size((unsigned char**) data), data,
                    smpl);
        SCR_free_tbl((unsigned char**) data);
    }
    SCR_free_tbl((unsigned char**) data0);
    SCR_free_tbl((unsigned char**) files);
    if(smpl)
        delete smpl;
    smpl = nullptr;

    if(rc < 0) 
        return(-1);
    else 
        return(0);
}


/**
 *  Syntax: $WsMerge<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 *  
 *  @see https://iode.plan.be/doku.php?id=wsmerge
 */
 
int B_WsMerge(char* arg, int type)
{
    int     lg, shift = 0, rc;
    char    file[K_MAX_FILE + 1];

    lg = B_get_arg0(file, arg, K_MAX_FILE);
    rc = K_cat(get_global_db(type), file);
    return(rc);
}


/**
 *  Syntax: $WsExtrapolate [method] from to [variable list]
 *          where 
 *              method : 0 ... 6
 *                  0 = Y := Y[-1], if Y null or NA
 *                  1 = Y := Y[-1], always
 *                  2 = Y := extrapolation, if Y null or NA
 *                  3 = Y := extrapolation, always
 *                  4 = Y := unchanged, always
 *                  5 = Y := Y[-1], if Y is NA
 *                  6 = Y := extrapolation, if Y is NA
 *              from, to := periods
 *
 *  
 *  @see https://iode.plan.be/doku.php?id=wsextrapolate
 */
 
int B_WsExtrapolate(char* arg, int unused)
{
    int     p = 0, method = 0, rc = -1;
    char    **vars;
    Sample* smpl = nullptr;

    char** args = B_ainit_chk(arg, NULL, 0);
    int nb_args = SCR_tbl_size((unsigned char**) args);
    if(nb_args < 2) 
    {
        error_manager.append_error("WsExtrapolate: syntax error (method from to vars ...)");
        goto done;
    }
    else 
    {
        method = atoi(args[0]);
        if(method < 0 || method > 6) 
            method = 0;
        else 
            p = 1;
    }

    try
    {
        smpl = new Sample(std::string(args[p]), std::string(args[p + 1]));
    }
    catch(const std::exception& e)
    {
        error_manager.append_error("WsExtrapolate: invalid sample\n" + std::string(e.what()));
        goto done;
    }
    
    if(smpl->nb_periods <= 0) 
    {
        error_manager.append_error("WsExtrapolate : invalid sample");
        goto done;
    }

    vars = args + p + 2;
    rc = KV_extrapolate(KV_WS.get(), method, smpl, vars);

done:
    if(smpl) delete smpl;
    smpl = nullptr;
    SCR_free_tbl((unsigned char**) args);
    return rc;
}


/**
 *  Sub-function of B_WsAggrSum, Prod and Mean.
 *  
 *  @param [in] int     method  aggregation method (see KV_aggregate())
 *  @param [in] char*   arg     pattern and optional filename
 *  @return     int             0 on success, < 0 on error
 */
 
int B_WsAggr(int method, char* arg)
{
    int     rc = -1;
    char    *pattern = NULL;
    KDB     *kdb = KV_WS.get(), *nkdb = NULL;

    char** args = B_ainit_chk(arg, NULL, 0);
    int nb_args = SCR_tbl_size((unsigned char**) args);
    if(nb_args < 1) 
    {
        error_manager.append_error("WsAggr* : syntax error (pattern [filename])");
        goto done;
    }
    pattern = (char*) SCR_stracpy((unsigned char*) args[0]);
    nkdb = KV_aggregate(kdb, method, pattern, args[1]);
    if(nkdb) 
    {
        KV_merge_del(kdb, nkdb, 1);
        rc = 0;
    }    
    
done:
    SCR_free(pattern);
    SCR_free_tbl((unsigned char**) args);
    return(rc);
}


/**
 *  Syntax: $WsAggrChar char
 *  
 *  @see https://iode.plan.be/doku.php?id=wsaggrchar 
 *  
 *  @see https://iode.plan.be/doku.php?id=wsaggrsum for an example.
 *  
 */
 
int B_WsAggrChar(char* arg, int unused)
{
    if(arg == NULL || arg[0] == 0) K_AggrChar = ' ';
    else K_AggrChar = arg[0];
    return(0);
}


/**
 *  Syntax: $WsAggrSum pattern filename
 *  
 *  @see https://iode.plan.be/doku.php?id=wsaggrsum
 */
 
int B_WsAggrSum(char* arg, int unused)
{
    return(B_WsAggr(0, arg));
}


/**
 *  Syntax: $WsAggrProd pattern filename
 *  
 *  @see https://iode.plan.be/doku.php?id=wsaggrprod
 *  @see https://iode.plan.be/doku.php?id=wsaggrsum for an example.
 */

int B_WsAggrProd(char* arg, int unused)
{
    return(B_WsAggr(1, arg));
}


/**
 *  Syntax: $WsAggrMean pattern filename
 *  
 *  @see https://iode.plan.be/doku.php?id=wsaggrmean
 *  @see https://iode.plan.be/doku.php?id=wsaggrsum for an example.
 */

int B_WsAggrMean(char* arg, int unused)
{
    return(B_WsAggr(2, arg));
}




/**
 *  Sub function of B_StatUnitRoot() with an optional parameter to print the result (or not).
 *  
 *  @param [in] arg         See B_StatUnitRoot()
 *  @param [in] print       0 to not print, print otherwise
 *  @return     double*  result of E_UnitRoot()
 */

double *B_StatUnitRoot_1(char* arg, int print)
{
    int     lg, drift, trend, order, rc = -1;
    char    name[80], buf[1024];
    double    *df = NULL;

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    drift = atoi(name);

    lg = B_get_arg0(name, arg + lg + 1, K_MAX_NAME + 1) + lg;
    trend = atoi(name);

    lg = B_get_arg0(name, arg + lg + 1, K_MAX_NAME + 1) + lg;
    order = atoi(name);

    df = E_UnitRoot(arg + lg + 1, drift, trend, order);
    if(print && df != NULL) 
        E_PrintDF(arg + lg + 1, df, drift, trend, order);

    if(df) 
    {
        E_GetLecName(arg + lg + 1, name);
        // set variable name to lower case
        for (int i = 0; i < 80 && name[i]; i++)
            name[i] = tolower((unsigned char) name[i]);
        // generate scalar name as df_<var_name_lower_case>
        sprintf(buf, "df_%s %lf", name, df[2]);
        // create scalar in the scalar WS if not existing
        B_DataUpdate(buf, SCALARS);
    }
    return(df);
}


/**
 *  Syntax: $StatUnitRoot drift trend order var_name
 *  where 
 *      drift : 0 or 1 : 1 to include a constant term in the formula
 *      trend : 0 or 1 : 1 to include a trend in the formula 
 *      order : order of the estimated polynomial fn 
 *      lec_expression : LEC expression to test
 *  
 // *  Warning: unlike E_UnitRoot(), only a variable name can be used as lec expression
 *  because the df test is saved in the scalar df_<var_name>.
 *  
 *  @see https://iode.plan.be/doku.php?id=statunitroot
 */

int B_StatUnitRoot(char* arg, int unused)
{
    int     rc = -1;
    double    *df;

    df = B_StatUnitRoot_1(arg, 1);
    if(df) rc = 0;

    SCR_free(df);
    return(rc);
}



//=============  CSV ============= //

/**
 *  Syntax: $CsvSave<type> file name1 name2 ...
 *  
 *  For Variables: $CsvSave<type> file [sample] [vars]
 *  
 *  @see https://iode.plan.be/doku.php?id=csvsave
 */

int B_CsvSave(char* arg, int type)
{
    int     lg, shift = 0, rc = 0;
    char    file[K_MAX_FILE + 1], file_ext[K_MAX_FILE + 1];
    char    **data0, *lst;
    Sample  *smpl = nullptr;
    char    *oldseps = A_SEPS; // JMP 27/09/2022

    // filename
    lg = B_get_arg0(file, arg, K_MAX_FILE);
    K_set_ext(file_ext, file, FILE_CSV);

    // [sample] [vars]
    //A_SEPS = " ;\t\n"; // JMP 27/09/2022
    A_SEPS = " ,;\t\n"; // JMP 25/04/2023
    lst = K_expand(type, NULL, arg + lg, '*');
    data0 = B_ainit_chk(lst, NULL, 0);
    SCR_free(lst);
    A_SEPS = oldseps; // JMP 27/09/2022   
    
    if(SCR_tbl_size((unsigned char**) data0) == 0) 
    {
        SCR_free_tbl((unsigned char**) data0);
        data0 = 0;
    }

    if(data0 && type == VARIABLES && SCR_tbl_size((unsigned char**) data0) >= 2) 
    {
        // check if from to passed as arguments
        if(SCR_is_num(data0[0][0]) && SCR_is_num(data0[1][0])) 
        {
            try
            {
                smpl = new Sample(std::string(data0[0]), std::string(data0[1]));
                shift = 2;
            }
            catch(const std::exception& e)
            {
                error_manager.append_error("CsvSave : invalid sample\n" + std::string(e.what()));
                SCR_free_tbl((unsigned char**) data0);
                data0 = 0;
            }
        }
        
        if(SCR_tbl_size((unsigned char**) data0) <= shift) 
        {
            SCR_free_tbl((unsigned char**) data0);
            data0 = 0;
            shift = 0;
        }
    }

    rc = ascii_handlers[type]->save_csv(get_global_db(type), file_ext, smpl, data0 + shift);

    SCR_free_tbl((unsigned char**) data0);
    if(smpl) delete smpl;
    smpl = nullptr;

    if(rc < 0) 
        return(rc);
    else 
        return(0);
}


/**
 *  Syntax: $CsvNbDec nn
 *  
 *  @see https://iode.plan.be/doku.php?id=csvdigits
 */

int B_CsvNbDec(char *nbdec, int unused)
{
    AsciiVariables::CSV_NBDEC = atoi(nbdec);
    if(AsciiVariables::CSV_NBDEC > 99 || (AsciiVariables::CSV_NBDEC < 0 && AsciiVariables::CSV_NBDEC != -1)) {
        error_manager.append_error(std::string(nbdec) + ": invalid number of decimals (value = 2)");
        AsciiVariables::CSV_NBDEC = 10;
        return(-1);
    }
    return(0);
}


/**
 *  Syntax: $CsvSep char
 *  
 *  @see https://iode.plan.be/doku.php?id=csvsep
 */

int B_CsvSep(char *sep, int unused)
{
    SCR_free(AsciiVariables::CSV_SEP);
    AsciiVariables::CSV_SEP = (char*) SCR_stracpy((unsigned char*) sep);
    return(0);
}


/**
 *  Syntax: $CsvNaN text
 *  
 *  @see https://iode.plan.be/doku.php?id=csvnan
 */

int B_CsvNaN(char *nan, int unused)
{
    SCR_free(AsciiVariables::CSV_NAN);
    AsciiVariables::CSV_NAN = (char*) SCR_stracpy((unsigned char*) nan);
    return(0);
}

/**
 *  Syntax: $CsvAxes AxisName
 *  
 *  Defines the name of the axis for the variable names (default "var")
 *  
 *  @see https://iode.plan.be/doku.php?id=csvsaxes
 */

int B_CsvAxes(char *var, int unused)
{
    SCR_free(AsciiVariables::CSV_AXES);
    AsciiVariables::CSV_AXES = (char*) SCR_stracpy((unsigned char*) var);
    return(0);
}

/**
 *  Syntax: $CsvDec char
 *  
 *  Defines the decimal separator in the generated CSV files.
 *  
 *  @see https://iode.plan.be/doku.php?id=csvdec
 */

int B_CsvDec(char *dec, int unused)
{
    SCR_free(AsciiVariables::CSV_DEC);
    AsciiVariables::CSV_DEC = (char*) SCR_stracpy((unsigned char*) dec);
    return(0);
}

/* =========== Fin CSV ============= */
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


template<typename T>
static bool load_global_database(std::unique_ptr<T>& global_ptr, 
        const std::string& filepath)
{
    T* kdb = new T(true);
    bool success = kdb->load(filepath);
    if(!success)
    {
        delete kdb;
        return false;
    }

    global_ptr.reset(kdb); 
    return true;
}


/**
 *  Syntax: $WsLoad<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @param [in] char*   arg     parameters if the command on the report line
 *  @param [in] int     type    object type
 *  @return     int             0 on success, -1 on error
 *  
 *
 */

int B_WsLoad(char* arg, int type)
{
    char buf[K_MAX_FILE + 1];

    SCR_strlcpy((unsigned char*) buf, (unsigned char*) arg, K_MAX_FILE);
    K_strip(buf);
    if(buf[0] == 0) 
        return 0;
    std::string filepath = std::string(buf);

    bool success;
    switch(type) 
    {
        case COMMENTS: 
            success = load_global_database<KDBComments>(global_ws_cmt, filepath);
            break;
        case EQUATIONS: 
            success = load_global_database<KDBEquations>(global_ws_eqs, filepath);
            break;
        case IDENTITIES: 
            success = load_global_database<KDBIdentities>(global_ws_idt, filepath);
            break;
        case LISTS: 
            success = load_global_database<KDBLists>(global_ws_lst, filepath);
            break;
        case SCALARS: 
            success = load_global_database<KDBScalars>(global_ws_scl, filepath);
            break;
        case TABLES: 
            success = load_global_database<KDBTables>(global_ws_tbl, filepath);
            break;
        case VARIABLES: 
            success = load_global_database<KDBVariables>(global_ws_var, filepath);
            break;
        default:
            kerror(0, "B_WsLoad: unknown type %d", type);
            return -1;
    }

    return success ? 0 : -1;
}


/**
 *  Dumps the content of KDB in a file: internal IODE format, ascii IODE format or CSV. 
 *  The extension of the filename determines if the output file must be in internal, ascii or csv format.
 *  
 *  @param [in] KDB*    kdb         KDB to dump
 *  @param [in] char*   filename    filename with extension
 *  @return     int                 return 0 if success, -1 on error
 *  
 */
int B_WsDump(KDB* kdb, char* filename)
{
    bool success = kdb->save(std::string(filename));
    return (success) ? 0 : -1;
}


/**
 *  Syntax: $WsSave<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 */
int B_WsSave(char* arg, int type)
{
    char buf[K_MAX_FILE + 1];

    SCR_strip((unsigned char*) arg);
    if(strlen(arg) >= sizeof(FNAME)) 
    {
        error_manager.append_error("WsSave: filename too long");
        return -1;
    }
    SCR_strlcpy((unsigned char*) buf, (unsigned char*) arg, K_MAX_FILE);
    SCR_strip((unsigned char*) buf);
    if(buf[0] == 0) 
        return 0;
    
    try
    {
        KDB& kdb = get_global_db(type);
        int res = B_WsDump(&kdb, buf);
        return res;
    }
    catch(const std::runtime_error& e)
    {
        error_manager.append_error("WsSave: " + std::string(e.what()));
        return -1;
    }
}


/**
 *  Syntax: $WsSaveCmp<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}. 
 *  
 *  Same as $WsSave<type> but with LZH compression.
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 *
 */
 
 int B_WsSaveCmp(char* arg, int type)
{
    char        buf[K_MAX_FILE + 1];
    int         rc = 0, klzh;

    SCR_strlcpy((unsigned char*) buf, (unsigned char*) arg, K_MAX_FILE); /* JMP 18-04-98 */
    SCR_strip((unsigned char*) buf);
    if(buf[0] == 0) 
        return 0;
    
    klzh = K_LZH;
    K_LZH = 2;
    
    try
    {
        KDB& kdb = get_global_db(type);
        rc = B_WsDump(&kdb, buf);
    }
    catch(const std::runtime_error& e)
    {
        error_manager.append_error("WsSaveCmp: " + std::string(e.what()));
        rc = -1;
    }
    
    K_LZH = klzh;
    return rc;
}


/**
 *  Syntax: $WsExport<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 */
int B_WsExport(char* arg, int type)
{
    try
    {
        KDB& kdb = get_global_db(type);
        kdb.save_asc(arg);
        return 0;
    }
    catch(const std::runtime_error& e)
    {
        error_manager.append_error("WsExport: " + std::string(e.what()));
        return -1;
    }
}


/**
 *  Syntax: $WsImport<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 *  
 *
 */

int B_WsImport(char* arg, int type)
{
    return(B_WsLoad(arg, type));
}


/**
 *  Syntax: $WsSample period_from period_to
 *  
 *
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

    if(KV_sample(global_ws_var.get(), new_smpl) < 0)
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
 */
int B_WsClear(char* arg, int type)
{
    B_WsDescr("", type);
    B_WsName("", type);
    try
    {
        KDB& kdb = get_global_db(type);
        kdb.clear();
        return 0;
    }
    catch(const std::runtime_error& e)
    {
        error_manager.append_error("WsClear: " + std::string(e.what()));
        return -1;
    }
}


/**
 *  Syntax: $WsClearAll
 *    
 *
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
 */
int B_WsDescr(char* arg, int type)
{
    try
    {
        KDB& kdb = get_global_db(type);
        kdb.description = std::string(arg);
        return 0;
    }
    catch(const std::runtime_error& e)
    {
        error_manager.append_error("WsDescr: " + std::string(e.what()));
        return -1;
    }
}


/**
 *  Sets the WS name. Obsolete as report function.
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 */
int B_WsName(char* arg, int type)
{
    try
    {
        KDB& kdb = get_global_db(type);
        kdb.filepath = std::string(arg);
        return 0;
    }
    catch(const std::runtime_error& e)
    {
        error_manager.append_error("WsName: " + std::string(e.what()));
        return -1;
    }
}


/**
 *  Syntax: 
 *      $WsCopy<type> fichier;fichier;.. obj1 obj2... 
 *          where type is in {cmt | eqs | idt | lst | scl | tbl}
 *      $WsCopyVar file;file;.. [from to] obj1 obj2... 
 */
int B_WsCopy(char* arg, int type)
{   
    char c_files[K_MAX_FILE + 1];
    int lg = B_get_arg0(c_files, arg, K_MAX_FILE);
    std::vector<std::string> v_files = split_multi(std::string(c_files), " ,;\t\n");
    if(v_files.size() == 0)
    {
        error_manager.append_error("WsCopy : filename(s) required");
        return -1;
    }

    std::string data = std::string(arg + lg);
    data = trim(data);
    if(data.empty())
    {
        error_manager.append_error("WsCopy : object(s) name(s)/pattern required");
        return -1;
    }

    bool success = false;
    if(type == VARIABLES)
    {
        std::string from, to;
        std::vector<std::string> v_data = split_multi(data, " ,;\t\n");
        if(v_data.size() >= 2) 
        {
            // check if 'from' and 'to' passed as arguments
            std::string first_arg = v_data[0];
            std::string second_arg = v_data[1];
            if(SCR_is_num(first_arg[0]) && SCR_is_num(second_arg[0]))
            {
                size_t pos;
                from = first_arg;
                to = second_arg;
                // remove 'from' from the list of arguments
                pos = data.find(from);
                if(pos != std::string::npos)
                    data.erase(pos, from.length());
                // remove 'to' from the list of arguments
                pos = data.find(to);
                if(pos != std::string::npos)
                    data.erase(pos, to.length());
                // check that 'from' and 'to' are valid periods for an IODE sample
                try
                {
                    Sample * tmp_smpl = new Sample(from, to);
                }
                catch(const std::exception& e)
                {
                    std::string error_msg = "WsCopy : invalid values for the 'from' 'to' arguments\n";
                    error_manager.append_error(error_msg + e.what());
                    return -1;
                }
            }
            else
            {
                Sample* vars_sample = global_ws_var->get_sample();
                if(vars_sample)
                {
                    from = vars_sample->start_period.to_string();
                    to = vars_sample->end_period.to_string();
                }
            }
        }

        data = trim(data);
        if(data.empty())
        {
            error_manager.append_error("WsCopy : filename(s) required");
            return -1;
        }

        success = global_ws_var->copy_from(v_files, from, to, data);
    }
    else
    {
        KDB& kdb = get_global_db(type);
        success = kdb.copy_from(v_files, data);
    }

    return success ? 0 : -1;
}


/**
 *  Syntax: $WsMerge<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 */
int B_WsMerge(char* arg, int type)
{
    char file[K_MAX_FILE + 1];
    int lg = B_get_arg0(file, arg, K_MAX_FILE);

    try
    {
        KDB& kdb = get_global_db(type);
        kdb.merge_from(file);
        return 0;
    }
    catch(const std::runtime_error& e)
    {
        error_manager.append_error("WsMerge: " + std::string(e.what()));
        return -1;
    }
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
 *
 */
int B_WsExtrapolate(char* arg, int unused)
{
    int     p = 0, method = 0, nb_vars = 0, rc = -1;
    char**  vars = NULL;
    Sample* smpl = nullptr;
    std::string pattern;

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
    nb_vars = SCR_tbl_size((unsigned char**) vars);
    if(nb_vars == 0) 
        pattern = "*";
    else 
    {
        for(int i = 0; i < nb_vars; i++) 
        {
            pattern += vars[i];
            if(i < nb_vars - 1) 
                pattern += ",";
        }
    }
    SCR_free_tbl((unsigned char**) args);

    rc = KV_extrapolate(global_ws_var.get(), method, smpl, (char*) pattern.c_str());

done:
    if(smpl) 
        delete smpl;
    smpl = nullptr;

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
    KDBVariables* kdb = global_ws_var.get();
    KDBVariables* nkdb = NULL;

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
 *
 */

int B_WsAggrSum(char* arg, int unused)
{
    return(B_WsAggr(0, arg));
}


/**
 *  Syntax: $WsAggrProd pattern filename
 *  
 *
 * for an example.
 */
int B_WsAggrProd(char* arg, int unused)
{
    return(B_WsAggr(1, arg));
}


/**
 *  Syntax: $WsAggrMean pattern filename
 *  
 *
 * for an example.
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
 *
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
 */
int B_CsvSave(char* arg, int type)
{
    int     lg, shift = 0;
    char    file[K_MAX_FILE + 1], file_ext[K_MAX_FILE + 1];
    char    **data0, *lst;
    Sample  *smpl = nullptr;
    char    *oldseps = A_SEPS; // JMP 27/09/2022

    if(type != VARIABLES)
        return -1;

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

    std::vector<std::string> vars;
    if(data0 + shift != NULL) 
    {
        for(int i = 0; data0[shift + i] != NULL; i++) 
            vars.push_back(std::string(data0[shift + i]));
    }

    int rc = 0;
    try
    {
        KDB& kdb = get_global_db(type);
        kdb.save_csv(file_ext, vars, smpl);
    }
    catch(const std::runtime_error& e)
    {
        error_manager.append_error("CsvSave: " + std::string(e.what()));
        rc = -1;
    }

    SCR_free_tbl((unsigned char**) data0);
    if(smpl) delete smpl;
    smpl = nullptr;

    return rc;
}


/**
 *  Syntax: $CsvNbDec nn
 *  
 *
 */
int B_CsvNbDec(char *nbdec, int unused)
{
    KDBVariables::CSV_NBDEC = atoi(nbdec);
    if(KDBVariables::CSV_NBDEC > 99 || (KDBVariables::CSV_NBDEC < 0 && KDBVariables::CSV_NBDEC != -1)) {
        error_manager.append_error(std::string(nbdec) + ": invalid number of decimals (value = 2)");
        KDBVariables::CSV_NBDEC = 10;
        return(-1);
    }
    return(0);
}


/**
 *  Syntax: $CsvSep char
 *  
 *
 */
int B_CsvSep(char *sep, int unused)
{
    SCR_free(KDBVariables::CSV_SEP);
    KDBVariables::CSV_SEP = (char*) SCR_stracpy((unsigned char*) sep);
    return(0);
}


/**
 *  Syntax: $CsvNaN text
 *  
 *
 */
int B_CsvNaN(char *nan, int unused)
{
    SCR_free(KDBVariables::CSV_NAN);
    KDBVariables::CSV_NAN = (char*) SCR_stracpy((unsigned char*) nan);
    return(0);
}

/**
 *  Syntax: $CsvAxes AxisName
 *  
 *  Defines the name of the axis for the variable names (default "var")
 *  
 *
 */
int B_CsvAxes(char *var, int unused)
{
    SCR_free(KDBVariables::CSV_AXES);
    KDBVariables::CSV_AXES = (char*) SCR_stracpy((unsigned char*) var);
    return(0);
}

/**
 *  Syntax: $CsvDec char
 *  
 *  Defines the decimal separator in the generated CSV files.
 *  
 *
 */
int B_CsvDec(char *dec, int unused)
{
    SCR_free(KDBVariables::CSV_DEC);
    KDBVariables::CSV_DEC = (char*) SCR_stracpy((unsigned char*) dec);
    return(0);
}

/* =========== Fin CSV ============= */

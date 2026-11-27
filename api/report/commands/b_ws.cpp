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
 * int B_WsLoad(const std::string& arg, int type)                 $WsLoad<type> filename
 * int B_WsDump(KDB* kdb, char* filename)            Dumps the content of KDB in a file
 * int B_WsSave(const std::string& arg, int type)                 $WsSave<type> filename
 * int B_WsSaveCmp(const std::string& arg, int type)              $WsSaveCmp<type> filename
 * int B_WsExport(const std::string& arg, int type)               $WsExport<type> filename
 * int B_WsImport(const std::string& arg, int type)               $WsImport<type> filename
 * int B_WsSample(const std::string& arg, int unused)                         $WsSample period_from period_to
 * int B_WsClear(const std::string& arg, int type)                $WsClear<type> 
 * int B_WsClearAll(const std::string& arg, int unused)                       $WsClearAll
 * int B_WsDescr(const std::string& arg, int type)                $WsDescr<type> free text
 * int B_WsName(char* arg, int type)                 Sets the WS name. Obsolete as report function.
 * int B_WsCopy(const std::string& arg, int type)                 $WsCopy<type> fichier;fichier;.. obj1 obj2... or $WsCopyVar file;file;.. [from to] obj1 obj2...
 * int B_WsMerge(const std::string& arg, int type)                $WsMerge<type> filename
 * int B_WsExtrapolate(const std::string& arg, int unused)                    $WsExtrapolate [method] from to [variable list]
 * int B_WsAggrChar(const std::string& arg, int unused)                       $WsAggrChar char
 * int B_WsAggrSum(const std::string& arg, int unused)                        $WsAggrSum pattern filename
 * int B_WsAggrProd(const std::string& arg, int unused)                       $WsAggrProd pattern filename
 * int B_WsAggrMean(const std::string& arg, int unused)                       $WsAggrMean pattern filename
 * double *B_StatUnitRoot_1(const std::string& arg, int print) Sub function of B_StatUnitRoot() with an optional parameter to print the result (or not).
 * int B_StatUnitRoot(const std::string& arg, int unused)                     $StatUnitRoot drift trend order expression
 * int B_CsvSave(const std::string& arg, int type)                $CsvSave<type> file name1 name2 ...
 * int B_CsvNbDec(const std::string& nbdec, int unused)                       $CsvNbDec nn
 * int B_CsvSep(const std::string& sep, int unused)                           $CsvSep char
 * int B_CsvNaN(const std::string& nan, int unused)                           $CsvNaN text
 * int B_CsvAxes(const std::string& var, int unused)                          $CsvAxes AxisName
 * int B_CsvDec(const std::string& dec, int unused)                           $CsvDec char
 */
#include "api/iode_scr4.h"

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
static bool load_global_database(std::shared_ptr<T>& global_ptr, const std::string& filepath)
{
    global_ptr->clear();
    bool success = global_ptr->load(filepath);
    if(!success)
    {
        global_ptr->clear();
        return false;
    }
 
    return true;
}


/**
 *  Syntax: $WsLoad<type> filename
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @param [in] const std::string&  arg     parameters if the command on the report line
 *  @param [in] int                 type    object type
 *  @return     int                         0 on success, -1 on error
 */
int B_WsLoad(const std::string& arg, int type)
{
    std::string buf(arg);
    if(buf.empty())
    {
        kwarning("Syntax error: no argument passed to $WsLoad<type>");
        return 0;
    }

    std::string filepath = trim(buf);

    if(filepath.length() >= K_MAX_FILE) 
    {
        std::string error_msg = "Syntax error: filename too long in $WsLoad<type> ";
        error_msg += "(max length is " + std::to_string(K_MAX_FILE - 1) + " characters)";
        kwarning(error_msg.c_str());
        return -1;
    }

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
int B_WsSave(const std::string& arg, int type)
{
    char buf[K_MAX_FILE + 1];

    std::string filename = arg;
    SCR_strip((unsigned char*) filename.data());
    if(strlen(filename.c_str()) >= sizeof(FNAME))
    {
        error_manager.append_error("WsSave: filename too long");
        return -1;
    }
    SCR_strlcpy((unsigned char*) buf, (unsigned char*) filename.c_str(), K_MAX_FILE);
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
 
 int B_WsSaveCmp(const std::string& arg, int type)
{
    char        buf[K_MAX_FILE + 1];
    int         rc = 0, klzh;

    SCR_strlcpy((unsigned char*) buf, (unsigned char*) arg.c_str(), K_MAX_FILE); /* JMP 18-04-98 */
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
int B_WsExport(const std::string& arg, int type)
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

int B_WsImport(const std::string& arg, int type)
{
    return(B_WsLoad(arg, type));
}


/**
 *  Syntax: $WsSample period_from period_to
 *  
 *
 */

int B_WsSample(const std::string& arg, int unused)
{
    bool success = false;
    Sample* new_smpl = nullptr;
    
    std::vector<std::string> v_args = expand_args(arg, 2);
    if(v_args.empty())
        goto err;

    try
    {
        new_smpl = new Sample(v_args[0], v_args[1]);
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

    success = global_ws_var->set_sample(*new_smpl);
    if(!success)
    {
        error_manager.append_error("Failed to set new sample");
        goto err;
    }

    delete new_smpl;
    new_smpl = nullptr;
    return 0;

err:
    if(new_smpl) delete new_smpl;
    new_smpl = nullptr;
    return -1;
}


/**
 *  Syntax: $WsClear<type> 
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 */
int B_WsClear(const std::string& arg, int type)
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
int B_WsClearAll(const std::string& arg, int unused)
{
    int i;

    for(i = COMMENTS; i <= VARIABLES; i++) B_WsClear("", i);
    return 0;
}


/**
 *  Syntax: $WsDescr<type> free text
 *  where type is in {Cmt, Eqs, Idt, Lst, Scl, Tbl, Var}
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 */
int B_WsDescr(const std::string& arg, int type)
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
int B_WsCopy(const std::string& arg, int type)
{   
    std::string args(arg);
    std::string files = get_next_arg(args, K_MAX_FILE);
    std::vector<std::string> v_files = split_multi(files, " ,;\t\n");
    if(v_files.size() == 0)
    {
        error_manager.append_error("WsCopy : filename(s) required");
        return -1;
    }

    std::string data = trim(args);
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
                std::shared_ptr<Sample> vars_sample = global_ws_var->get_sample();
                if(!vars_sample)
                {
                    error_manager.append_error("WsCopy : the Variables sample is not yet defined");
                    return -1;
                }
                else
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
int B_WsMerge(const std::string& arg, int type)
{
    std::string args(arg);
    std::string file = get_next_arg(args, K_MAX_FILE);

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
int B_WsExtrapolate(const std::string& arg, int unused)
{
    int     p = 0, method = 0, nb_vars = 0, rc = -1;
    Sample* smpl = nullptr;
    std::string pattern;

    std::vector<std::string> v_args = expand_args(arg, 0);
    if(v_args.size() < 2) 
    {
        error_manager.append_error("WsExtrapolate: syntax error (method from to vars ...)");
        goto done;
    }
    else 
    {
        method = atoi(v_args[0].c_str());
        if(method < 0 || method > 6) 
            method = 0;
        else 
            p = 1;
    }

    try
    {
        smpl = new Sample(v_args[p], v_args[p + 1]);
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

    nb_vars = (int) v_args.size() - p - 2;
    if(nb_vars == 0)
        pattern = "*";
    else 
    {
        for(int i = 0; i < nb_vars; i++) 
        {
            pattern += v_args[p + 2 + i];
            if(i < nb_vars - 1) 
                pattern += ",";
        }
    }

    rc = KV_extrapolate(global_ws_var, method, smpl, (char*) pattern.c_str());

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
int B_WsAggr(int method, const std::string& arg)
{
    std::vector<std::string> v_args = expand_args(arg, 0);
    if(v_args.empty()) 
    {
        error_manager.append_error("WsAggr* : syntax error (pattern [filename])");
        return -1;
    }

    std::string pattern = v_args[0];
    std::string filename;
    if(v_args.size() > 1)
        filename = v_args[1];

    KDBVariablesPtr kdb = global_ws_var;
    KDBVariablesPtr nkdb = KV_aggregate(kdb, method, pattern, filename);
    if(!nkdb)
    {
        error_manager.append_error("WsAggr* : aggregation failed");
        return -1;
    }

    KV_merge_del(kdb, nkdb, 1);
    return 0;    
}


/**
 *  Syntax: $WsAggrChar char
 *  
 */
int B_WsAggrChar(const std::string& arg, int unused)
{
    if(arg.empty() || arg[0] == 0) K_AggrChar = ' ';
    else K_AggrChar = arg[0];
    return 0;
}


/**
 *  Syntax: $WsAggrSum pattern filename
 *  
 *
 */

int B_WsAggrSum(const std::string& arg, int unused)
{
    return(B_WsAggr(0, arg));
}


/**
 *  Syntax: $WsAggrProd pattern filename
 *  
 *
 * for an example.
 */
int B_WsAggrProd(const std::string& arg, int unused)
{
    return(B_WsAggr(1, arg));
}


/**
 *  Syntax: $WsAggrMean pattern filename
 *  
 *
 * for an example.
 */
int B_WsAggrMean(const std::string& arg, int unused)
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
double *B_StatUnitRoot_1(const std::string& arg, int print)
{
    int     drift, trend, order, rc = -1;
    char    name[80], buf[1024];
    double    *df = NULL;
    // LEC expression: rest of the arguments after drift, trend and order
    std::string lec(arg);
    drift = atoi(get_next_arg(lec, K_MAX_NAME + 1).c_str());
    trend = atoi(get_next_arg(lec, K_MAX_NAME + 1).c_str());
    order = atoi(get_next_arg(lec, K_MAX_NAME + 1).c_str());

    df = E_UnitRoot(lec.data(), drift, trend, order);
    if(print && df != NULL)
        E_PrintDF(lec.data(), df, drift, trend, order);

    if(df)
    {
        E_GetLecName(lec.data(), name);
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
int B_StatUnitRoot(const std::string& arg, int unused)
{
    int     rc = -1;
    double    *df;

    df = B_StatUnitRoot_1(arg, 1);
    if(df) rc = 0;

    SCR_free(df);
    return rc;
}



//=============  CSV ============= //

/**
 *  Syntax: $CsvSave<type> file name1 name2 ...
 *  
 *  For Variables: $CsvSave<type> file [sample] [vars]
 */
int B_CsvSave(const std::string& arg, int type)
{
    if(type != VARIABLES)
        return -1;

    // filename
    std::string data(arg);
    std::string filepath = get_next_arg(data, K_MAX_FILE);
    filepath = set_file_extension(filepath, FILE_CSV);

    // [sample] [vars]
    char* lst = K_expand(type, NULL, data.data(), '*');
    std::vector<std::string> v_data0 = lst ? expand_args(lst, 0, " ,;\t\n") : std::vector<std::string>();
    SCR_free(lst);
    
    int shift = 0;
    std::shared_ptr<Sample> smpl = nullptr;
    if(type == VARIABLES && v_data0.size() >= 2)
    {
        // check if from to passed as arguments
        if(SCR_is_num(v_data0[0][0]) && SCR_is_num(v_data0[1][0])) 
        {
            try
            {
                smpl = std::make_shared<Sample>(v_data0[0], v_data0[1]);
                shift = 2;
            }
            catch(const std::exception& e)
            {
                error_manager.append_error("CsvSave : invalid sample\n" + std::string(e.what()));
                v_data0.clear();
            }
        }
        
        if(v_data0.size() <= shift)
        {
            v_data0.clear();
            shift = 0;
        }
    }

    std::vector<std::string> vars(v_data0.begin() + shift, v_data0.end());

    int rc = 0;
    try
    {
        KDB& kdb = get_global_db(type);
        kdb.save_csv(filepath, vars, smpl);
    }
    catch(const std::runtime_error& e)
    {
        error_manager.append_error("CsvSave: " + std::string(e.what()));
        rc = -1;
    }

    return rc;
}


/**
 *  Syntax: $CsvNbDec nn
 *  
 *
 */
int B_CsvNbDec(const std::string& nbdec, int unused)
{
    KDBVariables::CSV_NBDEC = atoi(nbdec.c_str());
    if(KDBVariables::CSV_NBDEC > 99 || (KDBVariables::CSV_NBDEC < 0 && KDBVariables::CSV_NBDEC != -1)) {
        error_manager.append_error(nbdec + ": invalid number of decimals (value = 2)");
        KDBVariables::CSV_NBDEC = 10;
        return -1;
    }
    return 0;
}


/**
 *  Syntax: $CsvSep char
 *  
 *
 */
int B_CsvSep(const std::string& sep, int unused)
{
    SCR_free(KDBVariables::CSV_SEP);
    KDBVariables::CSV_SEP = (char*) SCR_stracpy((unsigned char*) sep.c_str());
    return 0;
}


/**
 *  Syntax: $CsvNaN text
 *  
 *
 */
int B_CsvNaN(const std::string& nan, int unused)
{
    SCR_free(KDBVariables::CSV_NAN);
    KDBVariables::CSV_NAN = (char*) SCR_stracpy((unsigned char*) nan.c_str());
    return 0;
}

/**
 *  Syntax: $CsvAxes AxisName
 *  
 *  Defines the name of the axis for the variable names (default "var")
 *  
 *
 */
int B_CsvAxes(const std::string& var, int unused)
{
    SCR_free(KDBVariables::CSV_AXES);
    KDBVariables::CSV_AXES = (char*) SCR_stracpy((unsigned char*) var.c_str());
    return 0;
}

/**
 *  Syntax: $CsvDec char
 *  
 *  Defines the decimal separator in the generated CSV files.
 *  
 *
 */
int B_CsvDec(const std::string& dec, int unused)
{
    SCR_free(KDBVariables::CSV_DEC);
    KDBVariables::CSV_DEC = (char*) SCR_stracpy((unsigned char*) dec.c_str());
    return 0;
}

/* =========== Fin CSV ============= */

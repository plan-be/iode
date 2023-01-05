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
 * int B_WsSample(char* arg)                         $WsSample period_from period_to
 * int B_WsClear(char* arg, int type)                $WsClear<type> 
 * int B_WsClearAll(char* arg)                       $WsClearAll
 * int B_WsDescr(char* arg, int type)                $WsDescr<type> free text
 * int B_WsName(char* arg, int type)                 Sets the WS name. Obsolete as report function.
 * int B_WsCopy(char* arg, int type)                 $WsCopy<type> fichier;fichier;.. obj1 obj2... or $WsCopyVar file;file;.. [from to] obj1 obj2...
 * int B_WsMerge(char* arg, int type)                $WsMerge<type> filename
 * int B_WsExtrapolate(char* arg)                    $WsExtrapolate [method] from to [variable list]
 * int B_WsAggrChar(char* arg)                       $WsAggrChar char
 * int B_WsAggrSum(char* arg)                        $WsAggrSum pattern filename
 * int B_WsAggrProd(char* arg)                       $WsAggrProd pattern filename
 * int B_WsAggrMean(char* arg)                       $WsAggrMean pattern filename
 * IODE_REAL *B_StatUnitRoot_1(char* arg, int print) Sub function of B_StatUnitRoot() with an optional parameter to print the result (or not).
 * int B_StatUnitRoot(char* arg)                     $StatUnitRoot drift trend order expression
 * int B_CsvSave(char* arg, int type)                $CsvSave<type> file name1 name2 ...
 * int B_CsvNbDec(char *nbdec)                       $CsvNbDec nn
 * int B_CsvSep(char *sep)                           $CsvSep char
 * int B_CsvNaN(char *nan)                           $CsvNaN text
 * int B_CsvAxes(char *var)                          $CsvAxes AxisName
 * int B_CsvDec(char *dec)                           $CsvDec char
 */
 
#include "iode.h"
 

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
    KDB     *kdb = NULL;
    int     pos = K_PWS[type];
    char    buf[K_MAX_FILE + 1];

    SCR_strlcpy(buf, arg, K_MAX_FILE);
    K_strip(buf);   /* JMP 19-04-98 */
    if(buf[0] == 0) return(0);

    kdb = K_interpret(type, buf);
    if(kdb == NULL) return(-1);

    K_free(K_RWS[type][pos]);
    K_WS[type] = K_RWS[type][pos] = kdb;

    return(0);
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
    extern char k_ext[][4];
    char        buf[5];

    // Check std extensions .cmt => .var
    if(lg > 4) {
        for(i = 0 ; i < 7 ; i++) {
            if(filename[lg - 4] == '.' &&
                    SCR_cstrcmp(k_ext[i], filename + lg - 3) == 0) return(i);
        }
    }

    // Check ascii extensions .ac => .av
    if(lg > 3) {
        strcpy(buf, ".ac");
        for(i = 0 ; i < 7 ; i++) {
            buf[2] = k_ext[i][0];
            if(SCR_cstrcmp(filename + lg - 3, buf) == 0) return(10 + i);
        }
    }

    // Other extensions
    //
    //if(lg > 4 && SCR_cstrcmp(filename + lg - 4, ".csv") == 0) return(21); // Correction JMP 16/1/2019
    //if(lg > 4 && SCR_cstrcmp(filename + lg - 4, ".rep") == 0) return(22); // Correction JMP 16/1/2019
    if(lg > 4 && SCR_cstrcmp(filename + lg - 4, ".csv") == 0) return(K_CSV); // Correction JMP 25/3/2019
    if(lg > 4 && SCR_cstrcmp(filename + lg - 4, ".rep") == 0) return(22); // ??? pas tr�s coh�rent...

    // Sais plus a quoi ca peut servir... => a supprimer
    for(i = 16 ; strcmp(k_ext[i], "xxx") !=0 ; i++) {
        if(lg > 4 && SCR_cstrcmp(filename + lg - 4, k_ext[i]) == 0) return(i); // Correction JMP 16/1/2019 : lg - 4 au lieu de -3
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
    int     rc = -1, ftype, type = KTYPE(kdb);

    // kmsg("Saving %s", filename); // JMP 13/12/2022 (msg already in with K_save_kdb)
    ftype = X_findtype(filename);

    if(ftype >= 10 && ftype <= 17)
        rc = (*K_save_asc[type])(kdb, filename);
    //else if(ftype >= 0 && ftype <= 6)
    else if(ftype <= 6)
        rc = K_save(kdb, filename);
    else if(ftype == K_CSV)
        rc = (*K_save_csv[type])(kdb, filename, NULL, NULL);

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

    SCR_strip(arg);
    if(strlen(arg) >= sizeof(FNAME)) {
        B_seterror(B_msg(256));   /* File name too long */
        return(-1);
    }
    SCR_strlcpy(buf, arg, K_MAX_FILE); /* JMP 18-04-98 */
    SCR_strip(buf);
    if(buf[0] == 0) return(0);
    return(B_WsDump(K_WS[type], buf));
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
    extern int  K_LZH;                  /* JMP 28-05-00 */
    int         rc = 0, klzh;

    SCR_strlcpy(buf, arg, K_MAX_FILE); /* JMP 18-04-98 */
    SCR_strip(buf);
    if(buf[0] == 0) return(0);
    klzh = K_LZH;
    K_LZH = 2;
    rc = B_WsDump(K_WS[type], buf);
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

    return((*K_save_asc[type])(K_WS[type], arg));
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

int B_WsSample(char* arg)
{
    char    **args;
    SAMPLE  *new = NULL;
    KDB     *kdb = K_WS[K_VAR];

    args = B_ainit_chk(arg, NULL, 2);
    if(args == NULL) goto err;

    new = PER_atosmpl(args[0], args[1]);
    if(new == 0 || new->s_nb <= 0) goto err; /* JMP 25-05-92 */
    if(KV_sample(kdb, new) < 0) goto err;

    SW_nfree(new);
    A_free(args);
    return(0);

err:
    SW_nfree(new); /* JMP 25-05-92 */
    A_free(args);
    B_seterror(B_msg(4));   /* New sample invalid */
    return(-1);
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
    return(K_clear(K_WS[type]));
}


/**
 *  Syntax: $WsClearAll
 *    
 *  @see https://iode.plan.be/doku.php?id=wsclearall
 */
int B_WsClearAll(char* arg)
{
    int i;

    for(i = K_CMT; i <= K_VAR; i++) B_WsClear("", i);
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
    SCR_strlcpy(KDESC(K_WS[type]), arg, 50);
    return(0);
}


/**
 *  Sets the WS name. Obsolete as report function.
 *  
 *  @see B_WsLoad() for the syntax of that group of functions.
 */

int B_WsName(char* arg, int type)
{
    //char	dir[K_MAX_FILE], file[K_MAX_FILE];				// JMP 3/6/2015

    // Save filename only (not path) in KDB
    //SCR_split_name(arg, dir, file)						// JMP 3/6/2015
    //SCR_strlcpy(KNAME(K_WS[type]), file, K_MAX_FILE - 1);	// JMP 3/6/2015

    // Save full name in K_WSNAME
    K_set_kdb_name(K_WS[type], arg);  // JMP 3/6/2015
    return(0);
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
    char    **data, **data0, *K_expand(), *lst;
    U_ch    **SCR_inter();
    SAMPLE  *smpl = NULL;
    char    *oldseps = A_SEPS; // JMP 27/09/2022

    lg = B_get_arg0(file, arg, K_MAX_FILE);
    files = B_ainit_chk(file, NULL, 0);
    if(files == 0) return(-1);

    A_SEPS = " ;\t\n"; // JMP 27/09/2022
    lst = K_expand(type, files[0], arg + lg, '*');
    data0 = B_ainit_chk(lst, NULL, 0);
    SCR_free(lst);
    A_SEPS = oldseps; // JMP 27/09/2022   
    
    /*    data0 = B_ainit_chk(arg + lg, NULL, 0); */
    if(data0 == 0 || data0[0] == 0) {   /* JMP 24-06-98 */
        B_seterror(B_msg(134));
        SCR_free_tbl(files);
        SCR_free_tbl(data0);
        return(-1);
    }

    if(type == K_VAR && SCR_tbl_size(data0) >= 2) {
        smpl = PER_atosmpl(data0[0], data0[1]);
        if(smpl != NULL) shift = 2;
        else B_clear_last_error();
    }

    if(data0 + shift == NULL) {
        B_seterror(B_msg(135));
        rc = -1;
    }
    else {
        data = SCR_inter(data0 + shift, data0 + shift);
        rc = K_copy(K_WS[type],
                    SCR_tbl_size(files), files,
                    SCR_tbl_size(data), data,
                    smpl);
        SCR_free_tbl(data);
    }
    SCR_free_tbl(data0);
    SCR_free_tbl(files);
    SCR_free(smpl);

    if(rc < 0) return(-1);
    else return(0);
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
    rc = K_cat(K_WS[type], file);
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
 
int B_WsExtrapolate(char* arg)
{
    int     nb_args, p = 0, method = 0, rc = -1;
    char    **args, **vars;
    SAMPLE  *smpl;
    KDB     *kdb = K_WS[K_VAR];

    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size(args);
    if(nb_args < 2) {
        B_seterror(B_msg(130));
        goto done;
    }
    else {
        method = atoi(args[0]);
        if(method < 0 || method > 6) method = 0;
        else p = 1;
    }

    smpl = PER_atosmpl(args[p], args[p + 1]);
    if(smpl == 0 || smpl->s_nb <= 0) {
        B_seterror(B_msg(131));
        goto done;
    }

    vars = args + p + 2;
    rc = KV_extrapolate(kdb, method, smpl, vars);

done:
    SW_nfree(smpl);
    SCR_free_tbl(args);
    return(rc);
}


/**
 *  Sub-function of B_WsAggrSum, Prod and Mean.
 *  
 *  @param [in] int     method  aggregation method (see KV_aggregate())
 *  @param [in] char*   arg     pattern and optional filename
 *  @return     int             0 on success, < 0 on error
 */
 
static int B_WsAggr(int method, char* arg)
{
    int     nb_args, rc = -1;
    char    **args, *pattern = NULL;
    KDB     *kdb = K_WS[K_VAR], *nkdb = NULL;

    args = B_ainit_chk(arg, NULL, 0);
    nb_args = SCR_tbl_size(args);
    if(nb_args < 1) {
        B_seterror(B_msg(136));
        goto done;
    }
    pattern = SCR_stracpy(args[0]);
    nkdb = KV_aggregate(kdb, method, pattern, args[1]);
    if(nkdb) {
        KV_merge_del(kdb, nkdb, 1);
        rc = 0;
    }    
    
done:
    SCR_free(pattern);
    SCR_free_tbl(args);
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
 
int B_WsAggrChar(char* arg)
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
 
int B_WsAggrSum(char* arg)
{
    return(B_WsAggr(0, arg));
}


/**
 *  Syntax: $WsAggrProd pattern filename
 *  
 *  @see https://iode.plan.be/doku.php?id=wsaggrprod
 *  @see https://iode.plan.be/doku.php?id=wsaggrsum for an example.
 */

int B_WsAggrProd(char* arg)
{
    return(B_WsAggr(1, arg));
}


/**
 *  Syntax: $WsAggrMean pattern filename
 *  
 *  @see https://iode.plan.be/doku.php?id=wsaggrmean
 *  @see https://iode.plan.be/doku.php?id=wsaggrsum for an example.
 */

int B_WsAggrMean(char* arg)
{
    return(B_WsAggr(2, arg));
}




/**
 *  Sub function of B_StatUnitRoot() with an optional parameter to print the result (or not).
 *  
 *  @param [in] arg         See B_StatUnitRoot()
 *  @param [in] print       0 to not print, print otherwise
 *  @return     IODE_REAL*  result of E_UnitRoot()
 */

IODE_REAL *B_StatUnitRoot_1(char* arg, int print)
{
    int     lg, drift, trend, order, rc = -1;
    char    name[80], buf[1024];
    IODE_REAL    *df = NULL;
    extern  IODE_REAL *E_UnitRoot();

    lg = B_get_arg0(name, arg, K_MAX_NAME + 1);
    drift = atoi(name);

    lg = B_get_arg0(name, arg + lg + 1, K_MAX_NAME + 1) + lg;
    trend = atoi(name);

    lg = B_get_arg0(name, arg + lg + 1, K_MAX_NAME + 1) + lg;
    order = atoi(name);

//    sprintf(buf, "_DF %s", arg + lg + 1);
//    rc = B_DataCalcVar(buf);
//    if(rc) return(NULL);

//    df = E_UnitRoot("_DF", drift, trend, order);
    df = E_UnitRoot(arg + lg + 1, drift, trend, order);
    if(print && df != NULL) 
        E_PrintDF(arg + lg + 1, df, drift, trend, order);

//    B_DataDelete("_DF", K_VAR);

    if(df) {
        E_GetLecName(arg + lg + 1, name);
        sprintf(buf, "df_%s %lf", name, df[2]);
        B_DataUpdate(buf, K_SCL);
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

int B_StatUnitRoot(char* arg)
{
    int     rc = -1;
    IODE_REAL    *df;

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
    char    **data0, *K_expand(), *lst;
    SAMPLE  *smpl = NULL;
    char    *oldseps = A_SEPS; // JMP 27/09/2022

    // filename
    lg = B_get_arg0(file, arg, K_MAX_FILE);
    K_set_ext(file_ext, file, K_CSV);

    // [sample] [vars]
    A_SEPS = " ;\t\n"; // JMP 27/09/2022
    lst = K_expand(type, NULL, arg + lg, '*');
    data0 = B_ainit_chk(lst, NULL, 0);
    SCR_free(lst);
    A_SEPS = oldseps; // JMP 27/09/2022   
    
    if(SCR_tbl_size(data0) == 0) {
        SCR_free_tbl(data0);
        data0 = 0;
    }

    if(data0 && type == K_VAR && SCR_tbl_size(data0) >= 2) {
        if(SCR_is_num(data0[0][0]) && SCR_is_num(data0[1][0])) {
            shift = 2;
            smpl = PER_atosmpl(data0[0], data0[1]);
            B_clear_last_error();
        }
        if(SCR_tbl_size(data0) <= shift) {
            SCR_free_tbl(data0);
            data0 = 0;
            shift = 0;
        }
    }

    rc = (*K_save_csv[type])(K_WS[type], file_ext, smpl, data0 + shift);
    SCR_free_tbl(data0);
    SCR_free(smpl);

    if(rc < 0) return(rc);
    else return(0);
}


/**
 *  Syntax: $CsvNbDec nn
 *  
 *  @see https://iode.plan.be/doku.php?id=csvdigits
 */

int B_CsvNbDec(char *nbdec)
{
    extern int KV_CSV_NBDEC;

    KV_CSV_NBDEC = atoi(nbdec);
    if(KV_CSV_NBDEC > 99 || (KV_CSV_NBDEC < 0 && KV_CSV_NBDEC != -1)) {
        B_seterrn(53, nbdec);
        KV_CSV_NBDEC = 10;
        return(-1);
    }
    return(0);
}


/**
 *  Syntax: $CsvSep char
 *  
 *  @see https://iode.plan.be/doku.php?id=csvsep
 */

int B_CsvSep(char *sep)
{
    extern char  *KV_CSV_SEP;

    SCR_free(KV_CSV_SEP);
    KV_CSV_SEP = SCR_stracpy(sep);
    return(0);
}


/**
 *  Syntax: $CsvNaN text
 *  
 *  @see https://iode.plan.be/doku.php?id=csvnan
 */

int B_CsvNaN(char *nan)
{
    extern char  *KV_CSV_NAN;

    SCR_free(KV_CSV_NAN);
    KV_CSV_NAN = SCR_stracpy(nan);
    return(0);
}

/**
 *  Syntax: $CsvAxes AxisName
 *  
 *  Defines the name of the axis for the variable names (default "var")
 *  
 *  @see https://iode.plan.be/doku.php?id=csvsaxes
 */

int B_CsvAxes(char *var)
{
    extern char  *KV_CSV_AXES;

    SCR_free(KV_CSV_AXES);
    KV_CSV_AXES = SCR_stracpy(var);
    return(0);
}

/**
 *  Syntax: $CsvDec char
 *  
 *  Defines the decimal separator in the generated CSV files.
 *  
 *  @see https://iode.plan.be/doku.php?id=csvdec
 */

int B_CsvDec(char *dec)
{
    extern char  *KV_CSV_DEC;

    SCR_free(KV_CSV_DEC);
    KV_CSV_DEC = SCR_stracpy(dec);
    return(0);
}

/* =========== Fin CSV ============= */
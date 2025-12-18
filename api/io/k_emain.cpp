/**
 *  @header4iode
 * 
 *  Functions to export (partial) KDBs into external formats like csv, rotated csv... 
 *  
 *  The 2 main export functions calls virtual functions implemented in source files specific to each format:
 *      - k_edif.c
 *      - k_ecsv.c
 *      - k_etsp.c
 *      - k_ewks.c
 *  
 *  For each output format, a table of function pointers must be given as argument to 
 *  one of the general functions EXP_Ws() or EXP_Rev_Ws() found in k_emain.c.
 *  
 *  
 *  ExportToFile structure
 *  ----------------
 *  The ExportToFile structure contains a set of function pointers that implements the export procedure for a 
 *  specific output format. It is defined in iode.h.
 *  
 *      typedef struct _expdef_ { 
 *              int     (*write_header)();     // Pointer to the function that creates the output file and writes the header
 *              char    *(*write_object_name)();  // Pointer to the function to create the output object name (aka code) + the separator
 *              char    *(*extract_comment)();   // Pointer to the function to create the output object comment (if it exists in global_ws_cmt) + the separator for the output file 
 *              char    *(*get_variable_value)();  // Pointer to the function constructing an allocated vector of one VAR values
 *              int     (*write_variable_and_comment)();    // Pointer to the function saving the VAR and CMT in the output file
 *              int     (*close)();    // Pointer to the function that closes the output file after having written its footer
 *              FILE    *file_descriptor;            // Output file descriptor (output of fopen)
 *      } ExportToFile
 *  
 *  Function pointer write_object_name()
 *  ------------------------------
 *  The term "code" is a generalisation of the term "name" used for IODE objects.
 *  In the output file, a piece of information like a series or a comment will be identified by its "code".
 *  The purpose of the function (pointer) write_object_name() is to transform the IODE name into a "code" which 
 *  will be written in the output file.
 *  
 *  
 *  A set of rules can also be defined to select and/or determine the name each object is given in the output file.
 *  
 *  Definition of "Rules"
 *  ---------------------
 *  Rules are defined in an external file and allow essentially two things :
 *  
 *      - limit the variables to be imported 
 *      - determine an object name (code) from the information available in the Ascii format. 
 *  
 *  Syntax of "Rules"
 *  -----------------
 *  Each rule is composed of two fields:
 *  
 *      - the selection mask (pattern) containing a description of the names concerned by the rule. 
 *        This mask is defined in the same way as the search in IODE (Data menu), i.e. by including * and ? to specify sets. 
 *      - the name transcoding algorithm for this rule, which may contain: 
 *          + : indicating that the character at this position should be included as is in the name 
 *          - : any character: indicates that the character at this position should be skipped 
 *          any other character: included in the name 
 *  
 *  
 *  Example
 *  -------
 *      B*  C+-+            => changes B1234 to CB2, BCDEF to CBE, etc 
 *      *X  ++++++++++      => keeps names ending in X unchanged
 *  
 *  Selection via rules
 *  -------------------
 *  The defined rules allow to keep only the series whose name meets at least one of the rules. All others are eliminated.
 *  In particular, the rule 
 *  
 *      * ++++++++++ 
 *      
 *  selects all series while keeping the name (truncated here to 10 first characters). 
 *  
 *  If the rule file is empty or undefined, the rule below is used by default:
 *  
 *          * ++++++++++
 *  
 *  
 *  Order of execution
 *  ------------------
 *  Rules are executed in the order in which they are defined in the "rule file". 
 *  As soon as a rule can be applied to a name, it is used. 
 *  It is therefore important to position "exceptions" at the beginning of the file and to put general rules to the end.
 *    
 *  List of functions 
 *  -----------------
 *      void write_value(char* tmp, double val)                                         Formats a double value on 20 positions in general format.  
 *      char *write_pre_post(char* pre, char* post, char* src, char** tg)           Creates an allocated string formatted as {pre}{src}{post}.
 *      char *write_separator(char* src, char** tg)                                      Creates an allocated string formatted as {src}{EXP_SEP}.
 *      int EXP_Ws(ExportToFile* expdef, KDB* dbv, KDB* dbc, char* rulefile, char* outfile, char* na, char* sep)      Exports a KDB of VARs (and optionally of CMTs) in the format defined by the virtual functions in ExportToFile
 *      int EXP_Rev_Ws(ExportToFile* expdef, KDB* dbv, KDB* dbc, char* rulefile, char* outfile, char* na, char* sep)  Same as EXP_Ws() but the output is "rotated", i.e each column is a VAR and each line a period.    
 *      int EXP_RuleExport(char* trace, char* rule, char* out, char* vfile, char* cfile, char* from, char* to, char* na, char* sep, int fmt)   Exports VAR files into an external format. 
 *
 *  TODO: create report functions 
 */
#include "api/constants.h"
#include "api/b_a2mini.h"
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/objs/variables.h"
#include "api/write/write.h"
#include "api/io/import.h"
#include "api/io/export.h"


/**
 *  Replaces the value of EXP_SEP and EXP_NA. Strips the 2 strings and/or set a default value if needed.
 * 
 *  @param [in]  char*   na            string to indicated a NaN value in an exported file
 *  @param [in]  char*   sep           string to separate values in an exported file
 *  @gobal [out] char    EXP_SEP[11]   separator used in CSV
 *  @gobal [out] char    EXP_NA[11]    string to indicate a NaN value in CSV
 */
static void EXP_set(char* na, char* sep)
{
    //strcpy(EXP_SEP, sep);
    //strcpy(EXP_NA, na);
    SCR_strlcpy((unsigned char*) EXP_SEP, (unsigned char*) sep, sizeof(EXP_SEP) - 1);     // JMP 31/08/2022
    SCR_strlcpy((unsigned char*) EXP_NA, (unsigned char*) na, sizeof(EXP_NA) - 1);
    
    SCR_strip((unsigned char*) EXP_SEP);
    if(EXP_SEP[0] == 0) strcpy(EXP_SEP, " ");
    SCR_strip((unsigned char*) EXP_NA);
    if(EXP_NA[0] == 0) strcpy(EXP_NA, "#N/A");
}


/**
 *  Formats a double value on 20 positions in general format. 
 *  
 *  @see SCR_fmt_dbl()
 *  
 *  @param [in] char*   tmp     formatted val
 *  @param [in] double  val     input value
 */
void write_value(char* tmp, double val)
{
    if(IODE_IS_A_NUMBER(val)) {
        SCR_fmt_dbl(val, (unsigned char*) tmp, 20, -1);
        SCR_sqz((unsigned char*) tmp);
    }
    else strcpy(tmp, EXP_NA);
}


/**
 *  Creates an allocated string formatted as {pre}{src}{post}.
 *  
 *  @param [in]  char*   pre     prefix
 *  @param [in]  char*   post    postfix
 *  @param [in]  char*   src     main content
 *  @param [out] char**  tg      allocated result
 *  @return      char*           value of tg   
 */
char *write_pre_post(char* pre, char* post, char* src, char** tg)
{
    char    *buf = NULL;

    buf = (char*) SCR_stracpy((unsigned char*) src);
    SCR_strip((unsigned char*) buf);
    *tg = SW_nalloc((int)strlen(buf) +
                    (pre == NULL ? 0 : (int)strlen(pre)) +
                    (post == NULL ? 0 : (int)strlen(post)) + 1);
    if(*tg != NULL)
        sprintf(*tg, "%s%s%s",
                (pre == NULL ? "" : pre),
                buf,
                (post == NULL ? "" : post));

    SCR_free(buf);

    return(*tg);
}


/**
 *  Creates an allocated string formatted as {src}{EXP_SEP}.
 *  
 *  @param [in]  char*   src            main content
 *  @param [out] char**  tg             allocated result
 *  @gobal [in]  char    EXP_SEP[11]    separator 
 *  @return      char*                  value of tg   
 */
char *write_separator(char* src, char** tg)
{
    return(write_pre_post("", EXP_SEP, src, tg));
}


/**
 *  Exports a KDB of VARs (and optionally of CMTs) in the format defined by the virtual functions in ExportToFile (see iode.h for details). 
 *  The output file contains one VAR by line, one period by column.
 *   
 *  A selection can be made through the rules in a rule file. 
 *  
 *  @param [in] ExportToFile* expdef      struct containing the implementation of the virtual functions used to export the data
 *  @param [in] KDB*    dbv         input VAR KDB
 *  @param [in] KDB*    dbc         input CMT KDB
 *  @param [in] char*   rulefile    rule file
 *  @param [in] char*   outfile     output file
 *  @param [in] char*   na          string indicating NaN in the output file
 *  @param [in] char*   sep         string separating values in the output file
 *  @return     int                 0 on success, -1 on error
 *  
 */
int EXP_Ws(ExportToFile* expdef, CKDBVariables* dbv, CKDBComments* dbc, char* rulefile, 
           char* outfile, char* na, char* sep)
{
    int     i, j, dim, rc;
    char    *code = NULL, *cmt = NULL, *vec = NULL;
    ONAME   iname;
    char    oname[81];

    EXP_set(na, sep);

    if(dbv == NULL && dbc == NULL) 
        goto err;

    if(IMP_readrule(rulefile) < 0) 
        goto err;

    rc = expdef->write_header(expdef, dbv, dbc, outfile);
    if(rc < 0)
        goto err;

    dim = dbv->sample->nb_periods;
    for(i = 0; i < dbv->size(); i++) 
    {
        strcpy(iname, (char*) dbv->get_name(i).c_str());
        if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) 
            continue;

        expdef->write_object_name(oname, &code);

        if(dbc != NULL)
            expdef->extract_comment(dbc, iname, &cmt) ;

        if(dbv != NULL)
            for(j = 0; j < dim; j++)
                expdef->get_variable_value(dbv, i, j, &vec);

        expdef->write_variable_and_comment(expdef, code, cmt, vec);

        SW_nfree(code);
        code = NULL;
        SW_nfree(cmt);
        cmt = NULL;
        SW_nfree(vec);
        vec = NULL;
    }

    rc = expdef->close(expdef, dbv, dbc, outfile);
    if(rc < 0) 
        goto err;

    return(0);

err:
    return(-1);
}


/**
 *  Same as EXP_Ws() but the output is "rotated", i.e each column is a VAR and each line a period.
 *  
 */
int EXP_Rev_Ws(ExportToFile* expdef, CKDBVariables* dbv, CKDBComments* dbc, char* rulefile, 
               char* outfile, char* na, char* sep)
{
    int     i, j, nl, nc, rc;
    char    *code = NULL;
    ONAME   iname;
    char    oname[81];

    EXP_set(na, sep);

    if(dbv == NULL && dbc == NULL) 
        goto err;

    if(IMP_readrule(rulefile) < 0) 
        goto err;

    rc = expdef->write_header(expdef, dbv, dbc, outfile);
    if(rc < 0) 
        goto err;

    nl = dbv->sample->nb_periods;
    nc = dbv->size();

    expdef->write_variable_and_comment(expdef, EXP_SEP, 0, 0);

    for(i = 0; i < nc; i++) 
    {
        strcpy(iname, (char*) dbv->get_name(i).c_str());
        if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) 
            continue;
        expdef->write_object_name(oname, &code);
        expdef->write_variable_and_comment(expdef, code, 0, 0);
        SW_nfree(code);
        code = NULL;
    }

    expdef->write_variable_and_comment(expdef, 0, 0, 0);

    for(j = 0; j < nl; j++) 
    {
        Period per = dbv->sample->start_period.shift(j);
        sprintf(oname, "%s%s", (char*) per.to_string().c_str(), EXP_SEP);
        expdef->write_variable_and_comment(expdef, oname, 0, 0);

        for(i = 0; i < nc; i++) 
        {
            strcpy(iname, (char*) dbv->get_name(i).c_str());
            if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) 
                continue;
            expdef->get_variable_value(dbv, i, j, &code);
            expdef->write_variable_and_comment(expdef, code, 0, 0);
            SW_nfree(code);
            code = NULL;
        }
        expdef->write_variable_and_comment(expdef, 0, 0, 0);
    }

    rc = expdef->close(expdef, dbv, dbc, outfile); 
    if (rc < 0) 
        goto err;

    return(0);

err:
    return(-1);
}

/**
 *  Exports VAR files into an external format. 
 *  See EXP_Ws() for details.
 *  
 *  Function called in the context of IODE GUI.
 *  
 *  @param [in] char*   trace   if trace[0] != 0, prints a list of the object name modifications
 *  @param [in] char*   rule    rule file
 *  @param [in] char*   out     output file name
 *  @param [in] char*   vfile   input VAR file name. If NULL or empty, global_ws_var is used
 *  @param [in] char*   cfile   input CMT file name. If NULL or empty, global_ws_cmt is used
 *  @param [in] char*   from    first period to export
 *  @param [in] char*   to      last period to export
 *  @param [in] char*   na      string to use for IODE_NAN values
 *  @param [in] char*   sep     separators between cells
 *  @param [in] int     fmt     output format: 0=CSV, 1=DIF, 2=WKS, 3=TSP, default=Rotated CSV
 *  @return     int             0 on success, 
 */
int EXP_RuleExport(char* trace, char* rule, char* out, char* vfile, char* cfile, 
                   char* from, char* to, char* na, char* sep, int fmt)
{
    int rc = 0;
    bool success = false;

    ExportToFile *expdef;
    Sample *smpl = nullptr;

    SCR_strip((unsigned char*) trace);
    SCR_strip((unsigned char*) rule);
    SCR_strip((unsigned char*) out);
    SCR_strip((unsigned char*) vfile);
    SCR_strip((unsigned char*) cfile);

    SCR_strip((unsigned char*) from);
    SCR_strip((unsigned char*) to);
    if(from[0] != 0 && to[0] != 0)
    {
        try
        {
            smpl = new Sample(std::string(from), std::string(to));
        }
        catch(const std::exception& e)
        {
            error_manager.append_error("RuleExport:\n" + std::string(e.what()));
            error_manager.display_last_error(); 
        }
    }

    if(trace[0] != 0) 
    {
        IMP_trace = 1;
        K_WARN_DUP = 0;
        W_dest(trace, W_A2M);
    }
    else 
    {
        IMP_trace = 0;
        K_WARN_DUP = 1;
    }

    if(fmt < 0 || fmt >= IODE_NB_EXPORT_FORMATS)
        return(-1);

    // Get the ExportToFile handler for the requested format
    expdef = export_handlers[fmt].get();

    CKDBVariables* dbv = new CKDBVariables(false);
    if(vfile && vfile[0] != 0) 
    {
        success = dbv->load(std::string(vfile));
        if(!success) 
            return -1;
        if(smpl) 
            KV_sample(dbv, smpl);
    }

    CKDBComments* dbc = new CKDBComments(false);
    if(cfile && cfile[0] != 0)
        success = dbc->load(std::string(cfile)); 
    
    if(fmt < 4)
        rc = EXP_Ws(expdef, dbv, dbc, rule, out, na, sep);
    else
        rc = EXP_Rev_Ws(expdef, dbv, dbc, rule, out, na, sep);

    if(dbv)
    {
        delete dbv;
        dbv = nullptr;
    }
    if(dbc)
    {
        delete dbc;
        dbc = nullptr;
    }
    
    K_WARN_DUP = 0;
    if(rc)
        error_manager.display_last_error(); 
    
    return(rc);
}

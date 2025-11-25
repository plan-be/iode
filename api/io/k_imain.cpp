/**
 *  @header4iode
 * 
 *  Functions to import variables and comments from files in various external formats.
 *  
 *  For each input format, a structure ImportFromFile, which is essentially a table of function pointers,
 *  must be given as argument to one of the general functions IMP_InterpretVar() or IMP_InterpretCmt().
 *  
 *  Available input formats
 *  -----------------------
 *      - k_iasc.c:  IODE ASCII for comments and variables
 *      - k_irasc.c: rotated ASCII variables
 *      - k_ibst.c:  Bistel format (obsolete)
 *      - k_idif.c:  DIF format (Data Interchange Format) 
 *      - k_igem.c:  Chronos format (obsolete)   
 *      - k_inis.c:  INS format (obsolete)
 *      - k_iprn.c:  Aremos prn format (obsolete)
 *      - k_itxt.c:  Belgostat format (Obsolete)
 *    
 *  ImportFromFile structure
 *  ----------------
 *  The ImportFromFile structure contains a set of keywords and function pointers that implements the 
 *  import procedure for each specific input format. It is defined in iode.h.
 *       
 *     typedef struct _impdef_ {
 *         YYKEYS  *imp_keys;          // Table of keywords (see YY group of functions in scr4 libs)
 *         int     imp_dim;            // Nb of keys in imp_keys 
 *         int     (*read_header)();     // Pointer to the fn to open the input file and to read its header
 *         int     (*read_variable)();    // Pointer to the fn to read full variable (i.e. a name + a series of values)
 *         int     (*read_numerical_value)();   // Pointer to the fn to read a single numerical value (a double)
 *         int     (*close)();    // Pointer to the fn to close the input file
 *     } ImportFromFile;
 *
 *  
 *  Rule file
 *  ---------
 *  A set of rules can also be defined to select and/or determine (transform) the name of each read object.
 *  See k_emain.c for details on rule files.
 *      
 *  List of functions 
 *  -----------------
 *    KDB *IMP_InterpretVar(ImportFromFile* impdef, char* rulefile, char* vecfile, Sample* smpl)              Interprets a text file containing VAR definitions
 *    KDB *IMP_InterpretCmt(ImportFromFile* impdef, char* rulefile, char* cfile, int lang)                    Interprets an ASCII file containing COMMENTS definitions
 *    int IMP_RuleImport(int type, char* trace, char* rule, char* ode, char* asc, char* from, char* to, int fmt, int lang)    Imports variables or comments in various formats.
 *  
 */
#include "api/b_a2mini.h"
#include "api/k_super.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/comments.h"
#include "api/objs/variables.h"
#include "api/write/write.h"
#include "api/io/import.h"


static int compare(const void *a, const void *b)
{
    return YY_strcmp((char*) a, (char*) b);
}

/**
 *  Interprets an ASCII file containing VAR definitions and returns a new allocated KDB of vars (or NULL on error). 
 *  The format is implicitly defined by the ImportFromFile struct that contains a group of function 
 *  pointers in line with the expected ASCII format.
 *  
 *  @param [in] ImportFromFile* impdef      struct containing the fn pointers to interpret the content of the ascii file (see iode.h)
 *  @param [in] char*   rulefile    (opt.) rule file. By default, the rule "* ++++++++++++++" is used
 *  @param [in] char*   vecfile     input ASCII file
 *  @param [in] Sample* smpl        output Sample (required)
 *  @return     KDB*                new KDB containing the read variables or NULL on error    
 */
KDB *IMP_InterpretVar(ImportVarFromFile* impdef, char* rulefile, char* vecfile, Sample* smpl)
{
    bool    success;
    bool    found;
    int     i, nb, size, shift = 0, cmpt = 0, rc;
    char    iname[256];
    ONAME   oname;
    double  *vector = NULL, value;
    YYFILE  *yy;
    KDB*    kdb = nullptr;
    std::string var_name;

    if(!smpl)
        return nullptr;

    if(IMP_readrule(rulefile) < 0) 
        return nullptr;

    YY_CASE_SENSITIVE = 1;
    if(impdef->imp_keys != NULL) 
    {
        size = impdef->imp_dim;
        qsort(impdef->imp_keys, size, sizeof(YYKEYS), compare);
    }
    else 
        size = 0;

    SCR_strip((unsigned char*) vecfile);
    yy = YY_open(vecfile, impdef->imp_keys, size, YY_FILE);
    if(yy == 0) 
    {
        kerror(0,"Cannot open '%s'", vecfile);
        return nullptr;
    }

    rc = impdef->read_header(yy, smpl);
    if(rc < 0) 
        goto err;

    kdb = new KDB(VARIABLES, false);
    kdb->sample = new Sample(*smpl);
    nb = smpl->nb_periods;

    if(impdef->read_variable_implemented) 
    {
        vector = (double *) SW_nalloc(nb * sizeof(double));
        if(vector == NULL) goto err;

        while(1) 
        {
            for(i = 0; i < nb; i++) 
                vector[i] = IODE_NAN;

            rc = impdef->read_variable(yy, iname, nb, vector);
            if(rc < 0) 
                break;

            if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) 
                continue;
            kmsg("Reading object %d : %s", ++cmpt, oname);
            if(!kdb->set(oname, vector, nb))
                kerror(0, "Unable to create '%s'", oname);
        }
    }
    else 
    {
        while(1) 
        {
            rc = impdef->read_numerical_value(yy, iname, &shift, &value);
            if(rc < 0) 
                break;

            if(shift < 0 || shift > nb)     
                continue;

            if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0)     
                continue;
    
            if(SW_BLKS[7].blk_space > 100000L) 
                Debug("%s\n", oname);
              
            var_name = std::string(oname);
            found = kdb->contains(var_name);
            if(found) 
            {
                kmsg("Reading object %d : %s", ++cmpt, oname);
                success = kdb->set(oname, (double*) NULL, nb);
                if(!success) 
                {
                    kerror(0, "Unable to create '%s'", oname);
                    goto err;
                }
            }

            KV_set(kdb, var_name, shift, VAR_MODE_LEVEL, value);
        }
    }

    rc = impdef->close();
    if(rc < 0) 
        goto err;

    YY_close(yy);
    SW_nfree(vector);
    return kdb;

err:
    delete kdb;
    kdb = nullptr;
    kerror(0, "%s : incorrect filter (%s : offending entry)", YY_error(yy), yy->yy_text);
    YY_close(yy);
    SW_nfree(vector);
    return nullptr;
}


/**
 *  Interprets an ASCII file containing COMMENTS definitions and returns a new allocated KDB of CMTs (or NULL on error). 
 *  The format is implicitly defined by the ImportFromFile struct that contains a group of function 
 *  pointers in line with the expected ASCII format.
 *  
 *  @param [in] ImportFromFile* impdef      struct containing the fn pointers to interpret the content of the ascii file
 *  @param [in] char*   rulefile    (opt.) rule file. By default, the rule "* ++++++++++++++" is used
 *  @param [in] char*   cfile       ASCII input file
 *  @param [in] int     lang        0=English, 1=French , 2=Dutch 
 *  @return     KDB*                new KDB containing the read variables or NULL on error    
 */
KDB *IMP_InterpretCmt(ImportCmtFromFile* impdef, char* rulefile, char* cfile, int lang)
{
    CKDBComments *kdb = nullptr;
    int          size, cmpt = 0, rc;
    char         iname[256], *cmt = NULL;
    ONAME        oname;

    if(IMP_readrule(rulefile) < 0) 
        return nullptr;

    YY_CASE_SENSITIVE = 1;
    if(impdef->imp_keys != NULL) 
    {
        size = impdef->imp_dim;
        qsort(impdef->imp_keys, size, sizeof(YYKEYS), compare);
    }
    else 
        size = 0;

    rc = impdef->read_header(impdef, cfile, lang);
    if(rc < 0) 
        goto err;

    kdb = new CKDBComments(false);

    while(1) 
    {
        rc = impdef->read_comment(iname, &cmt);
        if(rc < 0) 
            break;

        if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) 
        {
            SW_nfree(cmt);
            continue;
        }

        if(SW_BLKS[7].blk_space > 100000L) 
            Debug((char*) "CMT:%s\n", oname);
        kmsg("Reading object %d : %s", ++cmpt, oname);
        SCR_strip((unsigned char*) cmt);
        if(!kdb->set(oname, cmt))
            kerror(0, "Unable to create '%s'", oname);
        SW_nfree(cmt);
    }

    rc = impdef->close();
    if(rc < 0) 
        goto err;

    return(kdb);

err:
    delete kdb;
    return nullptr;
}

/**
 *  Imports comments in various formats.
 *  
 *  @param [in] char*   trace   if trace[0] != 0, prints a list of the object name modifications
 *  @param [in] char*   rule    rule file
 *  @param [in] char*   ode     output IODE file
 *  @param [in] char*   asc     input filename
 *  @param [in] int     fmt     input format: 0=ASCII_CMT (=default), 1=-, 2=-, 3=Bistel_CMT 4=-, 5=-, 6=PRN_CMT, 7=TXT_CMT
 *  @param [in] int     lang    0=English, 1=French , 2=Dutch 
 *  @return     int             0 on success, -1 on error (IMP_InterpretCmt() ==  NULL or save() return code)
 */
static int IMP_RuleImportCmt(char* trace, char* rule, char* ode, char* asc, int fmt, int lang)
{
    KDB* kdb;
    ImportCmtFromFile* impdef;

    SCR_strip((unsigned char*) trace);
    SCR_strip((unsigned char*) rule);
    SCR_strip((unsigned char*) ode);
    SCR_strip((unsigned char*) asc);

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
   
    impdef = import_comments[fmt].get();

    int rc = 0;
    if(impdef)
    {
        kdb = IMP_InterpretCmt(impdef, rule, asc, lang);
        if(kdb) 
        {
            kdb->save_binary(ode);
            delete kdb;
            kdb = nullptr;
        }
        else
            rc = -1;
    }
    else
        rc = -1;

    if(IMP_trace) 
        W_close();
    
    return rc;
}


/**
 *  Imports variables in various formats.
 *  
 *  @param [in] char*   trace   if trace[0] != 0, prints a list of the object name modifications
 *  @param [in] char*   rule    rule file
 *  @param [in] char*   ode     output IODE file
 *  @param [in] char*   asc     input filename
 *  @param [in] char*   from    starting period of the sample to be read
 *  @param [in] char*   to      ending period of the sample
 *  @param [in] int     fmt     input format: 0=ASCII, 1=ROT_ASCII, 2=DIF, 3=Bistel, 4=NIS, 5=GEM, 6=PRN, 7=TXT
 *  @return     int             0 on success, -1 on error (IMP_InterpretVar() ==  NULL or save() return code)
 */
static int IMP_RuleImportVar(char* trace, char* rule, char* ode, char* asc, char* from, char* to, int fmt)
{
    Sample* smpl;
    KDB* kdb;
    ImportVarFromFile* impdef;

    SCR_strip((unsigned char*) trace);
    SCR_strip((unsigned char*) rule);
    SCR_strip((unsigned char*) ode);
    SCR_strip((unsigned char*) asc);

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

    impdef = import_variables[fmt].get();

    SCR_strip((unsigned char*) from);
    SCR_strip((unsigned char*) to);
    if(from[0] == 0 || to[0] == 0)
        return -1;
    try
    {
        smpl = new Sample(std::string(from), std::string(to));
    }
    catch(const std::exception& e)
    {
        std::string error_msg = "Cannot imports variables:\n" + std::string(e.what());
        error_manager.append_error(error_msg);
        return -1;
    }
    
    int rc = 0;
    kdb = IMP_InterpretVar(impdef, rule, asc, smpl);
    if(kdb) 
    {
        kdb->save_binary(ode);
        delete kdb;
        kdb = nullptr;
    }
    else 
        rc = -1;

    if(IMP_trace) 
        W_close();
    
    return rc;
}

/**
 *  Imports variables or comments in various formats.
 *  
 *  @param [in] int     type    type of objects to import: COMMENTS or VARIABLES
 *  @param [in] char*   trace   if trace[0] != 0, prints a list of the object name modifications
 *  @param [in] char*   rule    rule file
 *  @param [in] char*   ode     output IODE file
 *  @param [in] char*   asc     input filename
 *  @param [in] char*   from    starting period of the sample to be read
 *  @param [in] char*   to      ending period of the sample
 *  @param [in] int     fmt     input format: 0=ASCII, 1=ROT_ASCII, 2=DIF, 3=Bistel, 4=NIS, 5=GEM, 6=PRN, 7=TXT
 *  @param [in] int     lang    0=English, 1=French , 2=Dutch  
 *  @return     int             0 on success, -1 on error
 */
int IMP_RuleImport(int type, char* trace, char* rule, char* ode, char* asc, char* from, char* to, int fmt, int lang)
{
    int     rc = -1;
    
    switch(type) 
    {
        case COMMENTS   :
            rc = IMP_RuleImportCmt(trace, rule, ode, asc, fmt, lang);
            break;

        case VARIABLES   :
            rc = IMP_RuleImportVar(trace, rule, ode, asc, from, to, fmt);
            break;

        default :
            break;
    }

    K_WARN_DUP = 0;
    return(rc);
}

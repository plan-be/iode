/**
 *  @header4iode
 * 
 *  Functions to import variables and comments from files in various external formats.
 *  
 *  For each input format, a structure IMPDEF, which is essentially a table of function pointers,
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
 *  IMPDEF structure
 *  ----------------
 *  The IMPDEF structure contains a set of keywords and function pointers that implements the 
 *  import procedure for each specific input format. It is defined in iode.h.
 *       
 *     typedef struct _impdef_ {
 *         YYKEYS  *imp_keys;          // Table of keywords (see YY group of functions in scr4 libs)
 *         int     imp_dim;            // Nb of keys in imp_keys 
 *         int     (*imp_hd_fn)();     // Pointer to the fn to open the input file and to read its header
 *         int     (*imp_vec_var_fn)();    // Pointer to the fn to read full variable (i.e. a name + a series of values)
 *         int     (*imp_elem_fn)();   // Pointer to the fn to read a single numerical value (a double)
 *         int     (*imp_end_fn)();    // Pointer to the fn to close the input file
 *     } IMPDEF;
 *
 *  
 *  Rule file
 *  ---------
 *  A set of rules can also be defined to select and/or determine (transform) the name of each read object.
 *  See k_emain.c for details on rule files.
 *      
 *  List of functions 
 *  -----------------
 *    KDB *IMP_InterpretVar(IMPDEF* impdef, char* rulefile, char* vecfile, SAMPLE* smpl)              Interprets a text file containing VAR definitions
 *    KDB *IMP_InterpretCmt(IMPDEF* impdef, char* rulefile, char* cfile, int lang)                    Interprets an ASCII file containing COMMENTS definitions
 *    int IMP_RuleImport(int type, char* trace, char* rule, char* ode, char* asc, char* from, char* to, int fmt, int lang)    Imports variables or comments in various formats.
 *  
 */
#include "api/b_a2mini.h"
#include "api/k_super.h"
#include "api/utils/time.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/variables.h"
#include "api/write/write.h"
#include "api/conversion/import.h"

static int compare(const void *a, const void *b)
{
    return YY_strcmp((char*) a, (char*) b);
}

/**
 *  Interprets an ASCII file containing VAR definitions and returns a new allocated KDB of vars (or NULL on error). 
 *  The format is implicitly defined by the IMPDEF struct that contains a group of function 
 *  pointers in line with the expected ASCII format.
 *  
 *  @param [in] IMPDEF* impdef      struct containing the fn pointers to interpret the content of the ascii file (see iode.h)
 *  @param [in] char*   rulefile    (opt.) rule file. By default, the rule "* ++++++++++++++" is used
 *  @param [in] char*   vecfile     input ASCII file
 *  @param [in] SAMPLE* smpl        output SAMPLE (required)
 *  @return     KDB*                new KDB containing the read variables or NULL on error    
 */

KDB *IMP_InterpretVar(IMPDEF* impdef, char* rulefile, char* vecfile, SAMPLE* smpl)
{
    KDB     *kdb = 0;
    int     i, nb, size, pos, shift = 0, cmpt = 0;
    char    iname[256];
    ONAME   oname;
    double    *vector = NULL, value;
    YYFILE  *yy;

    if(smpl == NULL || IMP_readrule(rulefile) < 0) return(kdb);

    YY_CASE_SENSITIVE = 1;
    if(impdef->imp_keys != NULL) {
        size = impdef->imp_dim;
        qsort(impdef->imp_keys, size, sizeof(YYKEYS), compare);
    }
    else size = 0;

    SCR_strip((unsigned char*) vecfile);
    yy = YY_open(vecfile, impdef->imp_keys, size, YY_FILE);
    if(yy == 0) {
        kerror(0,"Cannot open '%s'", vecfile);
        return(kdb);
    }

    if(impdef->imp_hd_sample_fn != NULL
            && (*(impdef->imp_hd_sample_fn))(yy, smpl) < 0) goto err;

    kdb = K_create(VARIABLES, UPPER_CASE);
    memcpy((SAMPLE *) KDATA(kdb), smpl, sizeof(SAMPLE));
    nb = smpl->s_nb;

    if(impdef->imp_vec_var_fn != NULL) {
        vector = (double *) SW_nalloc(nb * sizeof(double));
        if(vector == NULL) goto err;

        while(1) {
            for(i = 0; i < nb; i++) vector[i] = IODE_NAN;

            if((*(impdef->imp_vec_var_fn))(yy, iname, nb, vector) < 0) break;

            if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) continue;
            kmsg("Reading object %d : %s", ++cmpt, oname);
            if(K_add(kdb, oname, vector, &nb) < 0)
                kerror(0, "Unable to create '%s'", oname);
        }
    }
    else {
        if(impdef->imp_elem_fn == NULL) {
            kerror(0, "Both vector and element function unavailable");
            goto err;
        }

        while(1) {
            if((*(impdef->imp_elem_fn))(yy, iname, &shift, &value) < 0) break;
            if(shift < 0 || shift > nb) continue;

            if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) continue;


            pos = K_find(kdb, oname);

            if(SW_BLKS[7].blk_space > 100000L) Debug("%s\n", oname);

            if(pos < 0) {
                kmsg("Reading object %d : %s", ++cmpt, oname);
                pos = K_add(kdb, oname, NULL, &nb);
                if(pos < 0) {
                    kerror(0, "Unable to create '%s'", oname);
                    goto err;
                }
            }
            KV_set(kdb, pos, shift, VAR_MODE_LEVEL, value);
        }

    }

    if(impdef->imp_end_fn != NULL
            && (*(impdef->imp_end_fn))() < 0) goto err;

    YY_close(yy);
    SW_nfree(vector);
    return(kdb);

err:
    K_free(kdb);
    kdb = NULL;
    kerror(0, "%s : incorrect filter (%s : offending entry)",
           YY_error(yy), yy->yy_text);
    YY_close(yy);
    SW_nfree(vector);
    return(kdb);
}


/**
 *  Interprets an ASCII file containing COMMENTS definitions and returns a new allocated KDB of CMTs (or NULL on error). 
 *  The format is implicitly defined by the IMPDEF struct that contains a group of function 
 *  pointers in line with the expected ASCII format.
 *  
 *  @param [in] IMPDEF* impdef      struct containing the fn pointers to interpret the content of the ascii file
 *  @param [in] char*   rulefile    (opt.) rule file. By default, the rule "* ++++++++++++++" is used
 *  @param [in] char*   cfile       ASCII input file
 *  @param [in] int     lang        0=English, 1=French , 2=Dutch 
 *  @return     KDB*                new KDB containing the read variables or NULL on error    
 */

KDB *IMP_InterpretCmt(IMPDEF* impdef, char* rulefile, char* cfile, int lang)
{
    KDB     *kdb = NULL;
    int     size, cmpt = 0;
    char    iname[256],
            *cmt = NULL;
    ONAME   oname;

    if(IMP_readrule(rulefile) < 0) return(kdb);

    YY_CASE_SENSITIVE = 1;
    if(impdef->imp_keys != NULL) {
        size = impdef->imp_dim;
        qsort(impdef->imp_keys, size, sizeof(YYKEYS), compare);
    }
    else size = 0;

    if(impdef->imp_hd_fn != NULL
            && (*(impdef->imp_hd_fn))(impdef, cfile, lang) < 0) goto err;

    kdb = K_create(COMMENTS, ASIS_CASE);

    if(impdef->imp_vec_cmt_fn != NULL) {

        while(1) {
            if((*(impdef->imp_vec_cmt_fn))(iname, &cmt) < 0) break;

            if(IMP_change(IMP_rule, IMP_pat, iname, oname) < 0) {
                SW_nfree(cmt);
                continue;
            }

            if(SW_BLKS[7].blk_space > 100000L) Debug("CMT:%s\n", oname);
            kmsg("Reading object %d : %s", ++cmpt, oname);
            SCR_strip((unsigned char*) cmt);
            if(K_add(kdb, oname, cmt) < 0)
                kerror(0, "Unable to create '%s'", oname);
            SW_nfree(cmt);
        }
    }

    if(impdef->imp_end_fn != NULL
            && (*(impdef->imp_end_fn))() < 0) goto err;

    return(kdb);

err:
    K_free(kdb);
    kdb = NULL;
    return(kdb);
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
 *  @return     int             0 on success, -1 on error (IMP_InterpretCmt() ==  NULL or K_save() return code)
 */
static int IMP_RuleImportCmt(char* trace, char* rule, char* ode, char* asc, int fmt, int lang)
{
    int     rc = -1;
    KDB     *kdb;
    IMPDEF  *impdef;

    SCR_strip((unsigned char*) trace);
    SCR_strip((unsigned char*) rule);
    SCR_strip((unsigned char*) ode);
    SCR_strip((unsigned char*) asc);

    if(trace[0] != 0) {
        IMP_trace = 1;
        K_WARN_DUP = 0;
        W_dest(trace, W_A2M);
    }
    else {
        IMP_trace = 0;
        K_WARN_DUP = 1;
    }
    switch(fmt) {
         case 1:
            impdef = NULL;
            break;
        case 2:
            impdef = NULL;
            break;
        case 3:
            impdef = &IMP_BST_CMT;
            break;
        case 4:
            impdef = NULL;
            break;
        case 5:
            impdef = NULL;
            break;
        case 6:
            impdef = &IMP_PRN_CMT;
            break;
        case 7:
            impdef = &IMP_TXT_CMT;
            break;
        default:        // JMP 05/01/2023         
            impdef = &IMP_ASC_CMT;
            break; 
    }

    kdb = IMP_InterpretCmt(impdef, rule, asc, lang);
    if(kdb != NULL) {
        rc = K_save(kdb, ode);
        K_free(kdb);
    }

    if(IMP_trace) W_close();
    return(rc);
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
 *  @return     int             0 on success, -1 on error (IMP_InterpretVar() ==  NULL or K_save() return code)
 */
static int IMP_RuleImportVar(char* trace, char* rule, char* ode, char* asc, char* from, char* to, int fmt)
{
    int     rc = -1;
    SAMPLE  *smpl;
    KDB     *kdb;
    IMPDEF  *impdef;

    SCR_strip((unsigned char*) trace);
    SCR_strip((unsigned char*) rule);
    SCR_strip((unsigned char*) ode);
    SCR_strip((unsigned char*) asc);

    if(trace[0] != 0) {
        IMP_trace = 1;
        K_WARN_DUP = 0;
        W_dest(trace, W_A2M);
    }
    else {
        IMP_trace = 0;
        K_WARN_DUP = 1;
    }

    switch(fmt) {
        case 1:
            impdef = &IMP_RASC;
            break;
        case 2:
            impdef = &IMP_DIF;
            break;
        case 3:
            impdef = &IMP_BST;
            break;
        case 4:
            impdef = &IMP_NIS;
            break;
        case 5:
            impdef = &IMP_GEM;
            break;
        case 6:
            impdef = &IMP_PRN;
            break;
        case 7:
            impdef = &IMP_TXT;
            break;
        default :
            impdef = &IMP_ASC;
            break;
             
    }

    SCR_strip((unsigned char*) from);
    SCR_strip((unsigned char*) to);
    if(from[0] == 0 || to[0] == 0)
        smpl = (SAMPLE *) SW_nalloc(sizeof(SAMPLE));
    else smpl = PER_atosmpl(from, to);
    if(smpl == 0) return(-1);

    kdb = IMP_InterpretVar(impdef, rule, asc, smpl);
    if(kdb != NULL) {
        rc = K_save(kdb, ode);
        K_free(kdb);
    }

    if(IMP_trace) W_close();
    return(rc);
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
    
    switch(type) {
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

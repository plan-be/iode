/**
 *  @header4iode
 * 
 *  Execution of IODE identities 
 *  ----------------------------
 *  IODE identities are simple LEC expressions. Each identity has a name which is the name of the variable that 
 *  will be created or updated by the "execution" (i.e. calculation) of that identity. 
 *  
 *  Order of execution
 *  ------------------
 *  The order of execution of a set of identities is computed by IODE in such a way 
 *  that the "dependent" identities are calculated after their predecessors.
 *  
 *      Example
 *      -------
 *      Suppose we have 3 identities to execute :
 *      
 *          Idt name    Formula
 *          --------    -------    
 *          A           B+C
 *          B           C * 2 * alpha
 *          E           X + Y
 *  
 *  The order of execution is automatically calcuted => [1, 0, 2].
 *  Indeed, B must be computed before A because A is a successor of B. E remains in 3d position.
 *
 * VAR and SCL source
 * ------------------
 *  The variables and scalars needed to compute the identities are read either in the current WS or 
 *  in a set of external files. 
 *  
 *  In the example above, the variables X, Y and C, and the scalar alpha, are needed to perform the calculation. 
 *  If no information on external files are given, X, Y and C are searched in the current KV_WS and alpha in the KS_WS.
 *  If one of them cannot be found, the process stops.
 *  
 *  If these objects are not present in KV_WS / KS_WS, the user must thus provide one or more source files. 
 *  Note the order in which these files are specified is important because as soon as an object is found, 
 *  it is not searched in the following files.
 *  
 *  To specify that an object must be searched in the current WS before the other files, the special name WS must be given.
 *
 *  Let's say that 
 *      - X and C are in the current WS, 
 *      - X and Y can be found in filexy.var 
 *      - alpha is in filealpha.scl.
 *  
 *  So, if we provide as VAR source files "WS filexy.var":
 *      - X and C will be read in WS, 
 *      - Y will be read in filexy,
 *  
 *  On the other hand, if we provide as VAR source files "filexy.var" without "WS", C cannot be found and 
 *  the execution will stop.
 *  
 *  Finally, if we provide as VAR source files "filexy.var WS", 
 *      - X and Y will be read in filexy.var
 *      - C will be read in WS, 
 *  
 *  The same principle applies to scalar source files.
 *  
 *  Execution sample
 *  ----------------
 *  When VARs are read from external files, they are shortened or expanded to match the current WS sample.
 *  On the other hand, one can execute identities on a sub-sample. In this case, there are 2 possibilities:
 *      - if a calculated VAR already exists in the current WS, its values are left unchanged outside the calculation sample
 *      - if the VAR is created, the values outside the calculation sample are set to IODE_NAN.
 *   
 *  
 *  List of functions 
 *  -----------------
 *      KDB *KI_exec(KDB* dbi, KDB* dbv, int nv, char* vfiles[], KDB* dbs, int ns, char* sfiles[], SAMPLE* smpl)   Executes all identities in dbi using the input series of dbv and scalars of dbs.
 *                                                                                                                 Missing vars and scalars are collected from vfiles and sfiles.
 */
#include "api/b_errors.h"
#include "api/constants.h"
#include "api/objs/pack.h"
#include "api/lec/lec.h"
#include "api/write/write.h"
#include "api/objs/kdb.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/k_exec.h"


// Function declarations
static int KI_strcmp(const char *pa, const char *pb);
static KDB *KI_series_list(KDB* dbi);
static KDB *KI_scalar_list(KDB* dbi);
static int *KI_reorder(KDB* dbi);
static int KI_read_vars_db(KDB* dbv, KDB* dbv_tmp, char* source_name);
static int KI_read_vars_file(KDB* dbv, char* file);
static int KI_read_vars(KDB* dbi, KDB* dbv, KDB* dbv_ws, int nb, char* files[]);
static int KI_read_scls_db(KDB* dbs, KDB* dbs_tmp, char* source_name);
static int KI_read_scls_file(KDB* dbs, char* file);
static int KI_read_scls(KDB* dbs, KDB* dbs_ws, int nb, char* files[]);
static int KI_execute(KDB* dbv, KDB* dbs, KDB* dbi, int* order, SAMPLE* smpl);
static int KI_quick_extract(KDB* dbv, KDB* dbi);
KDB *KI_exec(KDB* dbi, KDB* dbv, int nv, char* vfiles[], KDB* dbs, int ns, char* sfiles[], SAMPLE* smpl);

// Global variables
char    **KEXEC_VFILES = NULL;
char    **KEXEC_SFILES = NULL;
int     KEXEC_TRACE = 0;


/**
 *  Helper function used to compare 2 strings in KI_series_list().
 *  
 *  @param [in] const char* pa  first string
 *  @param [in] const char* pb  second string
 *  @return     int             output of strcmp() if a and b are not null
 *                              -1 if a is NULL
 *                              1 if b is NULL and a is not null  
 */
static int KI_strcmp(const char *pa, const char *pb)
{
    char *a = *(char **)pa, *b = *(char **)pb;

    if(a == 0) return -1;
    else if(b == 0) return 1;
    else return(strcmp(a, b));
}


/**
 *  Creates a KDB containing all variables found in the IDT KDB dbi.
 *  
 *  @param [in] KDB*    dbi     KDB of identities
 *  @return     KDB*            KDB of all vars found in dbi. All vars are initialised to L_NaN
 */
static KDB *KI_series_list(KDB* dbi)
{
    int     i, j, nb_names, ntbl;
    KDB     *dbv;
    LNAME   *lname;
    CLEC    *clec;
    char    **tbl;
    int     o_add_ptr_chunck = SCR_ADD_PTR_CHUNCK;

    SCR_ADD_PTR_CHUNCK = 1000;
    tbl = 0;
    ntbl = 0;

    // Ajoute dans un tableau toutes les noms de vars rencontrés **sans vérifier les doublons (will eliminated by the call to K_add_entry() below).  
    for(i = 0; i < KNB(dbi); i++) {
        SCR_add_ptr(&tbl, &ntbl, KONAME(dbi, i));
        clec = KICLEC(dbi, i);
        lname    = &(clec->lnames[0]);
        nb_names = clec->nb_names;

        for(j = 0; j < nb_names; j++) {
            if(L_ISCOEF(lname[j].name)) continue;
            SCR_add_ptr(&tbl, &ntbl, lname[j].name);
        }
    }
    SCR_add_ptr(&tbl, &ntbl, 0);

    // Sort the names (is it really required?)
    qsort(tbl, ntbl - 1, sizeof(char *), KI_strcmp);

    // Create a new KDB of vars with all the names in tbl
    dbv = K_create(VARIABLES, UPPER_CASE);
    for(i = 1; i < ntbl; i++) {
        if(tbl[i] == 0 || strcmp(tbl[i], tbl[i -1]))
            K_add_entry(dbv, tbl[i - 1]);
    }

    SCR_free_tbl(tbl);
    SCR_ADD_PTR_CHUNCK = o_add_ptr_chunck;

    return(dbv);
}


/**
 *  Creates a KDB containing all scalars found in the IDT KDB dbi.
 *  
 *  @param [in] KDB*    dbi     KDB of identities
 *  @return     KDB*            KDB of all scalars found in dbi.
 */
static KDB *KI_scalar_list(KDB* dbi)
{
    int     i, j, nb_names;
    KDB     *dbs;
    LNAME   *lname;
    CLEC    *clec, *tclec;

    dbs = K_create(SCALARS, LOWER_CASE);
    for(i = 0; i < KNB(dbi); i++) {
        clec = KICLEC(dbi, i);
        tclec = (CLEC *)SW_nalloc(clec->tot_lg);
        memcpy(tclec, clec, clec->tot_lg);
        lname    = &(tclec->lnames[0]);
        nb_names = tclec->nb_names;
        for(j = 0 ; j < nb_names ; j++) {
            if(!L_ISCOEF(lname[j].name)) continue;
            K_add_entry(dbs, lname[j].name);
        }
        SW_nfree(tclec); // JMP 26/8/2012
    }
    return(dbs);
}


// /*********************************************************************
// Delete from dbv the variables not existing in dbi. The result is thus
// a KDB dbv containing the vars computed from the  identities in dbi.
// (identities have the name of the output var).
// => Old version replaced by KI_quick_extract() (see below)
// **********************************************************************/
// int KI_extract(KDB* dbv, KDB* dbi)
// {
//     int     i, pos;
// 
//     //for(i = KNB(dbv) - 1; i >= 0 ; i--) {
//     for(i = 0 ; i < KNB(dbv) ; i++) {
//         pos = K_find(dbi, KONAME(dbv, i));
//         if(pos < 0) {
//             K_del(dbv, i);
//             i--;
//         }
//     }
//     return(0);
// }
// 


/**
 *  Reconstructs dbv with the variables whose names are found in dbi. The result is 
 *  a KDB (dbv modified) containing the vars computed from the identities in dbi
 *  (identities have the name of the output var).
 *  
 *  @param [in, out] KDB*   dbv     
 *  @param [in]      KDB*   dbi     
 *  @return          int    0 always
 */
static int KI_quick_extract(KDB* dbv, KDB* dbi) 
{
    int     i, j, pos, *objsnb, nbres;
    KOBJ    *newobjs;

    // Computes in objsnb the nb of VARs that have the same name as an IDT in dbi and that already exist in dbv.
    nbres = 0;
    objsnb = (long *) SW_nalloc(sizeof(long) * KNB(dbv));
    for(i = 0 ; i < KNB(dbv); i++) {
        pos = K_find(dbi, KONAME(dbv, i));
        if(pos >= 0) {
            objsnb[i] = 1; 
            nbres++;
        }
    }

    // Copy left objs in a new tmp table of KOBJ and delete the others
    newobjs = (KOBJ *) SW_nalloc((unsigned int)(sizeof(KOBJ) * K_CHUNCK * (1 + (nbres - 1) / K_CHUNCK)));
    for(i = j = 0 ; i < KNB(dbv); i++) {
        if(objsnb[i]) {
            memcpy(newobjs + j, KOBJS(dbv) + i, sizeof(KOBJ));
            j++;
        }
        else {
            SW_free(KSOVAL(dbv, i));
        }
    }

    SW_nfree(KOBJS(dbv));
    KOBJS(dbv) = newobjs;
    KNB(dbv) = nbres;
    SW_nfree(objsnb);
    return(0);
}



/**
 *  Computes the execution order of a KDB of identities by placing child identities after their "parents".
 *  If reordering is impossible, returns NULL.
 *  
 *  Example
 *  -------
 *      Idt     Formula
 *      ---     -------    
 *      A       B+C
 *      B       C * 2
 *      E       X + Y
 *  
 *  Order => [1, 0, 2] i.e. B is computed before A because A is a successor of B. E remains in 3d position.
 *    
 *  @param [in] KDB*    dbi     KDB of identities to reorder
 *  @return     int*            execution order or NULL if reordering is impossible             
 */

static int *KI_reorder(KDB* dbi)
{
    int     i, j, nb, *order,
            nb_order = 0,
            nb_names, mod, pos;
    char    *mark;
    LNAME   *lname;
    CLEC    *clec;

    nb = KNB(dbi);
    order = (int *)SW_nalloc(sizeof(int) * nb);
    mark  = SW_nalloc(nb);

    while(nb_order < nb) {
        mod = 0;
        for(i = 0; i < nb ; i++) {
            if(mark[i]) continue;
            clec = KICLEC(dbi, i);
            lname    = clec->lnames;
            nb_names = clec->nb_names;
            for(j = 0 ; j < nb_names ; j++) {
                if(strcmp(KONAME(dbi, i), lname[j].name) == 0) continue;
                pos = K_find(dbi, lname[j].name);
                if(pos < 0) continue;
                if(mark[pos]) continue;
                break;
            }

            if(j == nb_names) {
                mark[i] = mod = 1;
                order[nb_order++] = i;
            }
        }

        if(mod == 0) {
            /* IDENTITIES LOOP */
            if(KEXEC_TRACE) { /* GB TRACE */
                for(i = 0; i < nb; i++) {
                    if(mark[i])
                        W_printfDbl(".par1 enum_1\nIdt %s Ok\n", KONAME(dbi, i));
                    else
                        W_printfDbl(".par1 enum_1\nIdt %s Circular\n", KONAME(dbi, i));
                }
            }
            SW_nfree(order);
            SW_nfree(mark);
            return((int *)0);
        }
    }

    SW_nfree(mark);
    return(order);
}


/**
 *  Copies from the KDB dbv_tmp the unallocated VARs of dbv (i.e. the vars with no associated object).
 *  The output sample is dbv's.
 *  
 *  @param [in] KDB*    dbv         KDB of vars to read
 *  @param [in] KDB*    dbv_tmp     temporary KDB (read from an external file) where the needed VARs must be copied from
 *  @param [in] char*   source_name name of the input source (WS or filename)
 *  @return     int                 nb of VARs copied
 *                                  -3 if there is no common sample between dbv_tmp and dbv
 *  
 */
static int KI_read_vars_db(KDB* dbv, KDB* dbv_tmp, char* source_name)
{
    int     j, pos, nb_found = 0,
            start, start_tmp;
    SAMPLE  smpl, *vsmpl, *tsmpl;
    //char    *filename;

    vsmpl = (SAMPLE *) KDATA(dbv);
    tsmpl = (SAMPLE *) KDATA(dbv_tmp);

    PER_common_smpl(vsmpl, tsmpl, &smpl);
    if(smpl.s_nb > 0) {
        start     = PER_diff_per(&(smpl.s_p1), &(vsmpl->s_p1)); /* always >= 0 */
        start_tmp = PER_diff_per(&(smpl.s_p1), &(tsmpl->s_p1)); /* always >= 0 */
    }
    else return(-3);

    if(KEXEC_TRACE) {
        //W_printfDbl(".par1 enum_1\nFile %s : ", KNAMEPTR(dbv_tmp)); 
        W_printfDbl(".par1 enum_1\nFrom %s : ", source_name); 
    }    
    for(j = 0 ; j < KNB(dbv); j++) {
        if(KSOVAL(dbv, j) != 0) continue;  /* series already present */

        pos = K_find(dbv_tmp, KONAME(dbv, j));
        if(pos < 0) continue;
        KSOVAL(dbv, j) = KV_alloc_var(vsmpl->s_nb);
        memcpy(KVVAL(dbv, j, start), KVVAL(dbv_tmp, pos, start_tmp),
               sizeof(double) * smpl.s_nb);
        if(KEXEC_TRACE)
            W_printf("%s ", KONAME(dbv, j));
        nb_found++;
    }

    if(KEXEC_TRACE) W_printf("\n");
    return(nb_found);
}


/**
 *  Tries to read in file the unallocated VARs of dbv (i.e. the vars with no associated object).
 *  
 *  @param [in] KDB*    dbv     KDB of all needed VARs for calculating the identities
 *  @param [in] char*   file    name of a VAR file
 *  @return     int             nb of VARs read on success
 *                              -1 if the file cannot be opened
 *                              -3 if there is no common sample between dbv and file
 *  
 */
static int KI_read_vars_file(KDB* dbv, char* file)
{
    KDB     *kdb;
    char    **vars = NULL;
    int     j, nbv = 0, nbf,
               o_add_ptr_chunck = SCR_ADD_PTR_CHUNCK;

    SCR_ADD_PTR_CHUNCK = 1000;
    for(j = 0 ; j < KNB(dbv); j++) {
        if(KSOVAL(dbv, j) != 0) continue;
        SCR_add_ptr(&vars, &nbv, KONAME(dbv, j));
    }
    SCR_add_ptr(&vars, &nbv, NULL);
    SCR_ADD_PTR_CHUNCK = o_add_ptr_chunck;

    kdb = K_load(VARIABLES, file, nbv, vars);
    if(kdb == 0) {
        B_seterrn(96, file);
        return(-1);
    }

    nbf = KI_read_vars_db(dbv, kdb, file);
    SCR_free_tbl(vars);
    K_free(kdb);

    return(nbf);
}


/**
 *  Reads from a list of files, the VARs needed to compute identities. 
 *  For the variables to be read in the current KDB of VARs, specify "WS" as filename (required unless nb ==0).
 *  
 *  @param [in] KDB*    dbi         identities to be calculated
 *  @param [in] KDB*    dbv         list of series needed to calculate the identities in dbi
 *  @param [in] KDB*    dbv_ws      current VAR KDB (KV_WS)
 *  @param [in] int     nb          Number of VAR input files
 *  @param [in] char*   files[]     list of input VAR files, including "WS" for the current KDB of VARs 
 *                                  if nb == 0, the needed VARs are read from dbv_ws
 *  @return     int                 0 on success (all vars have been found)
 *                                  -1 if one of the files is not found
 *                                  -2 if some vars are not found in the files
 */
static int KI_read_vars(KDB* dbi, KDB* dbv, KDB* dbv_ws, int nb, char* files[])
{
    int     i, j, dim, nbf, nb_found = 0;

    if(nb == 0) {
        // No filename given => read in dbv_ws (normally KV_WS)
        nbf = KI_read_vars_db(dbv, dbv_ws, "WS");
        if(nbf < 0) return(-1);
        nb_found += nbf;
    }
    else {
        // Files given, search in files in the same order as they are listed
        for(i = 0;  i < nb && nb_found < KNB(dbv); i++) {
            if(strcmp(files[i], "WS") == 0)
                // Special name "WS" => read in dbv_ws 
                nbf = KI_read_vars_db(dbv, dbv_ws, "WS");
            else
                // Regular VAR file
                nbf = KI_read_vars_file(dbv, files[i]);

            if(nbf < 0) return(-1);
            nb_found += nbf;
        }
    }

    // If all target VARs are not found, creates them with NaN values
    if(nb_found < KNB(dbv)) {
        dim = KSMPL(dbv)->s_nb;
        for(i = 0, j = 0 ; i < KNB(dbv) && j < 10; i++) {
            if(KSOVAL(dbv, i) != 0) continue;               // series already present in dbv
            if(K_find(dbi, KONAME(dbv, i)) >= 0) {          // series = identity ("endogenous") => creates an IODE_NAN VA
                K_add(dbv, KONAME(dbv, i), NULL, &dim);      
                continue;
            }
            j++;
            //B_seterrn(90, KONAME(dbv, i));                     // Exogenous series not found => error
            B_seterror("Variable %s not found", KONAME(dbv, i));   // Exogenous series not found => error
        }
        if(j == 0) return(0);                   // all VARs found
        // if(j == 10) B_seterrn(91);                 // more than 10 exogenous vars not found => special msg 
        if(j == 10) B_seterror("... others skipped"); // more than 10 exogenous vars not found => special msg 
        return(-2);
    }
    return(0);
}


/**
 *  Copies from the KDB dbs_tmp the unallocated SCLs of dbs (i.e. the SCL with no associated object).
 *  
 *  @param [in] KDB*    dbs         KDB of SCLs to read
 *  @param [in] KDB*    dbs_tmp     temporary KDB (read from an external file) where the needed SCLs must be copied from
 *  @param [in] char*   source_name name of the input source (WS or filename)
 *  @return     int                 nb of SCLs copied
 *                                  -3 if there is no common sample between dbv_tmp and dbv
 */
static int KI_read_scls_db(KDB* dbs, KDB* dbs_tmp, char* source_name)
{
    //char    filename[K_MAX_FILE + 1];
    int     j, pos, nb_found = 0;

    //strcpy(filename, KNAMEPTR(dbs_tmp));      /* JMP 29-09-2015 */
    //SCR_replace(filename, "\\", "\\\\");

    if(KEXEC_TRACE) W_printfDbl(".par1 enum_1\nFrom %s : ", source_name); /* JMP 19-10-99 */
    for(j = 0 ; j < KNB(dbs); j++) {
        if(KSOVAL(dbs, j) != 0) continue;

        pos = K_find(dbs_tmp, KONAME(dbs, j));
        if(pos < 0) continue;

        KSOVAL(dbs, j) = KS_alloc_scl();
        memcpy(KSVAL(dbs, j), KSVAL(dbs_tmp, pos), sizeof(SCL));
        if(KEXEC_TRACE) W_printf("%s ", KONAME(dbs, j));
        nb_found++;
    }

    if(KEXEC_TRACE) W_printf("\n");
    return(nb_found);
}


/**
 *  Tries to read in file the unallocated SCLs of dbs (i.e. the SCLs with no associated object).
 *  
 *  @param [in] KDB*    dbs     KDB of all needed SCLs for calculating the identities
 *  @param [in] char*   file    name of a SCL file
 *  @return     int             nb of SCLs read 
 *                              -1 if the file cannot be opened
 *  
 */
static int KI_read_scls_file(KDB* dbs, char* file)
{
    KDB     *kdb;
    char    **scls = NULL;
    int     j, nbs = 0, nbf,
               o_add_ptr_chunck = SCR_ADD_PTR_CHUNCK;

    SCR_ADD_PTR_CHUNCK = 1000;
    for(j = 0 ; j < KNB(dbs); j++) {
        if(KSOVAL(dbs, j) != 0) continue;
        SCR_add_ptr(&scls, &nbs, KONAME(dbs, j));
    }
    SCR_add_ptr(&scls, &nbs, NULL);
    SCR_ADD_PTR_CHUNCK = o_add_ptr_chunck;

    kdb = K_load(SCALARS, file, nbs, scls);
    if(kdb == 0) {
        B_seterrn(96, file);
        return(-1);
    }

    nbf = KI_read_scls_db(dbs, kdb, file);
    SCR_free_tbl(scls);
    K_free(kdb);

    return(nbf);
}


/**
 *  Reads, from a list of files, the SCLs needed to compute identities. 
 *  For the SCLs to be read in the current KDB of VARs, specify "WS" as filename (required unless nb ==0).
 *  
 *  @param [in] KDB*    dbs         SCLs to be read
 *  @param [in] KDB*    dbs_ws      current SCL KDB (KS_WS)
 *  @param [in] int     nb          Number of SCL input files
 *  @param [in] char*   files[]     list of input SCL files, including "WS" for the current KDB of SCLs 
 *                                  if nb == 0, the needed SCLs are read from dbs_ws
 *  @return     int                 0 on success (all SCLs have been found)
 *                                  -1 if one of the files is not found
 *                                  -2 if some SCLs were not found in the files
 */

static int KI_read_scls(KDB* dbs, KDB* dbs_ws, int nb, char* files[])
{
    int     i, j, nbf, nb_found = 0;

    if(nb == 0) {
        nbf = KI_read_scls_db(dbs, dbs_ws, "WS");
        if(nbf < 0) return(-1);
        nb_found += nbf;
    }
    else {
        for(i = 0;  i < nb && nb_found < KNB(dbs); i++) {
            if(strcmp(files[i], "WS") == 0)
                nbf = KI_read_scls_db(dbs, dbs_ws, "WS");
            else
                nbf = KI_read_scls_file(dbs, files[i]);

            if(nbf < 0) return(-1);
            nb_found += nbf;
        }
    }

    if(nb_found < KNB(dbs)) {
        for(i = 0, j = 0 ; i < KNB(dbs) && j < 10; i++) {
            if(KSOVAL(dbs, i) != 0) continue;  /* series already present */
            j++;
            //B_seterrn(92, KONAME(dbs, i));
            B_seterror("Scalar %s not found", KONAME(dbs, i));   // scalar not found
        }
        //if(j == 10) B_seterrn(91);
        if(j == 10) B_seterror("... others skipped");
        return(-2);
    }
    return(0);
}


/*
    Adds to dbv series to be calculated (from dbi)
*/
/*
KI_dbi_to_dbv(dbv, dbi)
KDB     *dbv,  *dbi;
{
    int     i, nb, pos;

    nb = KSMPL(dbv)->s_nb;
    for(i = 0; i < KNB(dbi); i++)  {
	if(K_find(dbv, KONAME(dbi, i)) < 0) {
	    K_add(dbv, KONAME(dbi, i), NULL, &nb);
	    }
    }

}
*/


/**
 *  Sub function of KI_exec() that links and computes all identities in dbi after 
 *  all needed VARs and SCLs have been read and saved in dbv and dbs.
 *  
 *  
 *  @param [in] KDB*    dbv   Input VAR KDB
 *  @param [in] KDB*    dbs   Input SCL KDB
 *  @param [in] KDB*    dbi   KDB of identities to compute
 *  @param [in] int*    order order of execution of the identities
 *  @param [in] SAMPLE* smpl  execution SAMPLE
 *  @return     int           0 on success
 *                            -1 on LEC execution error (DIV/0...)
 *  
 */
static int KI_execute(KDB* dbv, KDB* dbs, KDB* dbi, int* order, SAMPLE* smpl)
{
    int     i, t, pos, tot_lg, start;
    char    *tmp;
    double  d;

    start = PER_diff_per(&(smpl->s_p1), &(KSMPL(dbv)->s_p1));
    if(start < 0) start = 0;

    for(i = 0; i < KNB(dbi); i++) {
        tot_lg = KICLEC(dbi, order[i])->tot_lg;
        tmp = SW_nalloc(tot_lg);
        memcpy(tmp, KICLEC(dbi, order[i]), tot_lg);
        if(L_link(dbv, dbs, (CLEC *)tmp)) return(-1);
        pos = K_find(dbv, KONAME(dbi, order[i]));
        for(t = start ; t < start + smpl->s_nb ; t++) {
            d = L_exec(dbv, dbs, (CLEC *)tmp, t);
            *(KVVAL(dbv, pos, t)) = d;
        }
        SW_nfree(tmp);
    }

    return(0);
}


/**
 *  Executes all identities in dbi using the input series of dbv and scalars of dbs.
 *  Missing vars and scalars are collected from vfiles and sfiles.
 *
 *  @param [in] KDB*    dbi         KDB of identities to be calculated
 *  @param [in] KDB*    dbv         Input VAR KDB
 *  @param [in] int     nv          Number of input VAR files
 *  @param [in] char*   vfiles[]    Input VAR files
 *  @param [in] KDB*    dbs         Input SCL KDB
 *  @param [in] int     ns          number of input SCL files
 *  @param [in] char*   sfiles[]    Input SCL files
 *  @param [in] SAMPLE* smpl        execution SAMPLE or NULL to select the current VAR KDB sample
 *  @return     KDB*                NULL on error (illegal SAMPLE, empty dbi, vars or scls not found...).
 *                                  The specific message is added via B_seterrn().
 */
KDB *KI_exec(KDB* dbi, KDB* dbv, int nv, char* vfiles[], KDB* dbs, int ns, char* sfiles[], SAMPLE* in_smpl)
{
    KDB     *dbv_i, *dbs_i;
    SAMPLE  *smpl;
    int     *order;
    int     *KI_reorder();
    char    buf[80];


    smpl = KSMPL(KV_WS);
    if(in_smpl != 0) smpl = in_smpl;
    
    if(smpl->s_nb == 0) {
        B_seterrn(93);
        return((KDB *)0);
    }
    
    if(KSMPL(KV_WS)->s_nb != 0 &&
            (PER_diff_per(&(KSMPL(KV_WS)->s_p2), &(smpl->s_p2)) < 0
             || PER_diff_per(&(smpl->s_p1), &(KSMPL(KV_WS)->s_p1)) < 0)) {
        B_seterrn(93);
        return((KDB *)0);
    }
    if(KNB(dbi) == 0) {
        B_seterrn(94);
        return((KDB *)0);
    }

    order = KI_reorder(dbi);
    if(order == 0) {
        B_seterrn(95);
        return((KDB *)0);
    }

    dbv_i = KI_series_list(dbi);
    if(KSMPL(KV_WS)->s_nb == 0) memcpy(KDATA(dbv_i), smpl, sizeof(SAMPLE));
    else  memcpy(KDATA(dbv_i), KSMPL(KV_WS), sizeof(SAMPLE));

    if(KEXEC_TRACE) {
        W_printf("\n.par1 tit_0\nExecution of identities\n");
        W_printf(".par1 tit_1\nParameters\n");
        /*        W_printf(".par1 par_1\nList of identities :\n"); */
        W_printf(".par1 par_1\nExecution sample : %s\n",
                 PER_smpltoa(smpl, buf));
        W_printf(".par1 tit_1\nVariables loaded\n");
    }
    if(KI_read_vars(dbi, dbv_i, dbv, nv, vfiles)) {
        SW_nfree(order);
        K_free(dbv_i);
        return((KDB *)0);
    }

    dbs_i = KI_scalar_list(dbi);
    if(KEXEC_TRACE) W_printf(".par1 tit_1\nScalars loaded\n");
    if(KI_read_scls(dbs_i, dbs, ns, sfiles)) {
        SW_nfree(order);
        K_free(dbv_i);
        K_free(dbs_i);
        return((KDB *)0);
    }
    /*    if(KEXEC_TRACE) W_close();*/
    if(KEXEC_TRACE) W_flush();


    /*    KI_dbi_to_dbv(dbv_i, dbi); */
    KI_execute(dbv_i, dbs_i, dbi, order, smpl);
    //KI_extract(dbv_i, dbi);
    KI_quick_extract(dbv_i, dbi); // JMP 19/11/2012
    SW_nfree(order);
    K_free(dbs_i);
    return(dbv_i);
}

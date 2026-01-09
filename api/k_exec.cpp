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
 * VAR and Scalar source
 * ------------------
 *  The variables and scalars needed to compute the identities are read either in the current WS or 
 *  in a set of external files. 
 *  
 *  In the example above, the variables X, Y and C, and the scalar alpha, are needed to perform the calculation. 
 *  If no information on external files are given, X, Y and C are searched in the current global_ws_var and alpha in the global_ws_scl.
 *  If one of them cannot be found, the process stops.
 *  
 *  If these objects are not present in global_ws_var / global_ws_scl, the user must thus provide one or more source files. 
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
 *      KDB *KI_exec(KDB* dbi, KDB* dbv, int nv, char* vfiles[], KDB* dbs, int ns, char* sfiles[], Sample* smpl)   Executes all identities in dbi using the input series of dbv and scalars of dbs.
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
#include "api/utils/utils.h"
#include "api/k_exec.h"

#include <string>
#include <vector>


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
static int KI_execute(CKDBVariables* dbv, CKDBScalars* dbs, KDB* dbi, int* order, Sample* smpl);
static int KI_quick_extract(KDB* dbv, KDB* dbi);


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

static int wrapper_KI_strcmp(const void *pa, const void *pb)
{
    return KI_strcmp((char*) pa, (char*) pb);
}

/**
 *  Creates a KDB containing all variables found in the IDT KDB dbi.
 *  
 *  @param [in] KDB*    dbi     KDB of identities
 *  @return     KDB*            KDB of all vars found in dbi. All vars are initialised to L_NaN
 */
static CKDBVariables* KI_series_list(CKDBIdentities* dbi)
{
    int     nb_names;
    LNAME   *lname;
    CLEC    *clec;
    CKDBVariables* dbv;
    
    // Ajoute dans un tableau toutes les noms de vars rencontrés **sans vérifier les doublons 
    // (will eliminated by the call to add_entry() below). 
    std::string name;  
    std::vector<std::string> vars_to_compute;
    for(auto& [idt_name, idt_handle] : dbi->k_objs) 
    {
        vars_to_compute.push_back(idt_name);

        clec = dbi->get_obj(idt_handle)->get_compiled_lec();
        lname    = &(clec->lnames[0]);
        nb_names = clec->nb_names;

        for(int j = 0; j < nb_names; j++) 
        {
            name = std::string(lname[j].name);
            if(is_coefficient(name)) 
                continue;
            vars_to_compute.push_back(name);
        }
    }

    // remove duplicates
    sort_and_remove_duplicates(vars_to_compute);

    // Create a new KDB of vars with all the names in tbl
    dbv = new CKDBVariables(false);
    for(const std::string& name : vars_to_compute)
        dbv->add_entry(name);

    return dbv;
}


/**
 *  Creates a KDB containing all scalars found in the IDT KDB dbi.
 *  
 *  @param [in] KDB*    dbi     KDB of identities
 *  @return     KDB*            KDB of all scalars found in dbi.
 */
static CKDBScalars* KI_scalar_list(CKDBIdentities* dbi)
{
    int     nb_names;
    LNAME   *lname;
    CLEC    *clec, *tclec;
    CKDBScalars* dbs;

    std::string name;
    dbs = new CKDBScalars(false);
    for(auto& [idt_name, idt_handle] : dbi->k_objs) 
    {
        clec = dbi->get_obj(idt_handle)->get_compiled_lec();
        tclec = (CLEC*) SW_nalloc(clec->tot_lg);
        memcpy(tclec, clec, clec->tot_lg);
        lname    = &(tclec->lnames[0]);
        nb_names = tclec->nb_names;
        for(int j = 0 ; j < nb_names ; j++) 
        {
            name = std::string(lname[j].name);
            if(!is_coefficient(name)) 
                continue;
            dbs->add_entry(name);
        }
        SW_nfree(tclec);
    }

    return dbs;
}


/**
 *  Reconstructs dbv with the variables whose names are found in dbi. The result is 
 *  a KDB (dbv modified) containing the vars computed from the identities in dbi
 *  (identities have the name of the output var).
 *  
 *  @param [in, out] KDB*   dbv     
 *  @param [in]      KDB*   dbi     
 *  @return          int    0 always
 */
static int KI_quick_extract(CKDBVariables* dbv, CKDBIdentities* dbi) 
{
    // get list of VARs names
    std::vector<std::string> names;
    names.reserve(dbv->size());
    for(auto& [name, handle] : dbv->k_objs)
        names.push_back(name);

    // keep only VARs that have the same name as an IDT in dbi
    for(const std::string& name : names)
    {
        if(!dbi->contains(name))
            dbv->remove(name);
    }

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

static int *KI_reorder(CKDBIdentities* dbi)
{
    int     nb, *order,
            nb_order = 0,
            nb_names, mod, pos;
    char    *mark;
    LNAME   *lname;
    CLEC    *clec;

    nb = dbi->size();
    order = (int *)SW_nalloc(sizeof(int) * nb);
    mark  = SW_nalloc(nb);

    int i, j;
    std::string name;
    while(nb_order < nb) 
    {
        i = 0;
        mod = 0;
        for(auto& [idt_name, idt_handle] : dbi->k_objs) 
        {
            if(mark[i]) 
                continue;
            clec = dbi->get_obj(idt_handle)->get_compiled_lec();
            lname    = clec->lnames;
            nb_names = clec->nb_names;
            for(j = 0 ; j < nb_names ; j++) 
            {
                name = std::string(lname[j].name);
                if(idt_name == name) 
                    continue;
                pos = dbi->index_of(name);
                if(pos < 0) 
                    continue;
                if(mark[pos]) 
                    continue;
                break;
            }

            if(j == nb_names) 
            {
                mark[i] = mod = 1;
                order[nb_order++] = i;
            }

            i++;
        }

        if(mod == 0) 
        {
            /* IDENTITIES LOOP */
            if(KEXEC_TRACE) { /* GB TRACE */
                for(i = 0; i < nb; i++) 
                {
                    if(mark[i])
                        W_printfDbl(".par1 enum_1\nIdt %s Ok\n", dbi->get_name(i));
                    else
                        W_printfDbl(".par1 enum_1\nIdt %s Circular\n", dbi->get_name(i));
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
 *  Copies VARS from the KDB dbv_tmp the KDB dbv.
 *  The output sample is dbv's.
 *  
 *  @param [in] KDB*    dbv         KDB of vars to read
 *  @param [in] KDB*    dbv_tmp     temporary KDB (read from an external file) where the needed VARs must be copied from
 *  @param [in] char*   source_name name of the input source (WS or filename)
 *  @return     int                 nb of VARs copied
 *                                  -3 if there is no common sample between dbv_tmp and dbv
 *  
 */
static int KI_read_vars_db(CKDBVariables* dbv, CKDBVariables* dbv_tmp, char* source_name)
{
    int start, start_tmp;

    if(!dbv)
    {
        error_manager.append_error("Function KI_read_vars_db: input Variables database is null");
        return -3;
    }

    // get list of VARs to be read (from dbv_tmp)
    std::vector<std::string> vars_to_read;
    for(auto& [name, handle] : dbv->k_objs)
    {
        // series already present
        if(handle > 0)
            continue;
        if(dbv_tmp->contains(name)) 
            vars_to_read.push_back(name);
    }

    // no VARs to be read
    if(vars_to_read.size() == 0)
        return 0;

    Sample* vsmpl = dbv->sample;
    Sample* tsmpl = dbv_tmp->sample;
    if(!tsmpl)
    {
        std::string msg = "Function KI_read_vars_db: the sample of the ";
        if(std::string(source_name) == "WS")
            msg += "current Variables workspace";
        else
            msg += "database read from the file '" + std::string(source_name) + "'";
        msg += " is empty";
        error_manager.append_error(msg);
        return -3;
    }

    // The sample of the KDB of the variables to read is empty 
    if(!vsmpl) 
    {
        dbv->sample = new Sample(*tsmpl);
        vsmpl = dbv->sample;
    }
    
    Sample smpl = vsmpl->intersection(*tsmpl);
    if(smpl.nb_periods > 0) 
    {
        start     = smpl.start_period.difference(vsmpl->start_period);  /* always >= 0 */
        start_tmp = smpl.start_period.difference(tsmpl->start_period);  /* always >= 0 */
    }
    else 
        return -3;

    if(KEXEC_TRACE) 
        W_printfDbl(".par1 enum_1\nFrom %s : ", source_name);

    SWHDL handle;
    int nb_found = 0;
    for(const std::string& name : vars_to_read)
    {
        // series already present (in dbv)
        handle = dbv->get_handle(name);
        if(handle > 0) 
            continue;

        // allocate the VAR in dbv
        handle = (SWHDL) KV_alloc_var(vsmpl->nb_periods);
        dbv->k_objs[name] = handle;

        // copy the VAR from dbv_tmp to dbv
        memcpy(dbv->get_var_ptr(name, start), dbv_tmp->get_var_ptr(name, start_tmp), 
               sizeof(double) * smpl.nb_periods);
        
        if(KEXEC_TRACE)
            W_printf((char*) "%s ", name.c_str());
        nb_found++;
    }

    if(KEXEC_TRACE) 
        W_printf((char*) "\n");
    
    return(nb_found);
}


/**
 *  Tries to read in file the VARs of dbv (i.e. the vars with no associated object).
 *  
 *  @param [in] KDB*    dbv     KDB of all needed VARs for calculating the identities
 *  @param [in] char*   file    name of a VAR file
 *  @return     int             nb of VARs read on success
 *                              -1 if the file cannot be opened
 *                              -3 if there is no common sample between dbv and file
 *  
 */
static int KI_read_vars_file(CKDBVariables* dbv, char* file)
{
    char    **vars = NULL;
    int     nbv = 0, nbf;

    std::vector<std::string> vars_to_read;
    for(auto& [name, handle] : dbv->k_objs) 
    {
        // series already loaded in dbv
        if(handle > 0) 
            continue;
        vars_to_read.push_back(name);
    }

    // no variables to be read
    if(vars_to_read.size() == 0)
        return 0;

    if(vars_to_read.size() > 0 && std::string(file).empty())
    {
        error_manager.append_error("The path of file to read the Variables is empty");
        return -1;
    }
    
    CKDBVariables* kdb = new CKDBVariables(false);
    bool success = kdb->load(std::string(file));
    if(!success) 
    {
        std::string msg = "Variables file '" + std::string(file) + "' not found";
        error_manager.append_error(msg);
        return -1;
    }

    if(kdb->size() == 0) 
    {
        delete kdb;
        kdb = nullptr;
        std::string msg = "Variables file '" + std::string(file) + "' contains no variable";
        error_manager.append_error(msg);
        return -1;
    }

    nbf = KI_read_vars_db(dbv, kdb, file);
    delete kdb;
    kdb = nullptr;

    return nbf;
}


/**
 *  Reads from a list of files, the VARs needed to compute identities. 
 *  For the variables to be read in the current KDB of VARs, specify "WS" as filename (required unless nb ==0).
 *  
 *  @param [in] KDB*    dbi         identities to be calculated
 *  @param [in] KDB*    dbv         list of series needed to calculate the identities in dbi
 *  @param [in] KDB*    dbv_ws      current VAR KDB (global_ws_var)
 *  @param [in] int     nb          Number of VAR input files
 *  @param [in] char*   files[]     list of input VAR files, including "WS" for the current KDB of VARs 
 *                                  if nb == 0, the needed VARs are read from dbv_ws
 *  @return     int                 0 on success (all vars have been found)
 *                                  -1 if one of the files is not found
 *                                  -2 if some vars are not found in the files
 */
static int KI_read_vars(CKDBIdentities* dbi, CKDBVariables* dbv, CKDBVariables* dbv_ws, 
    int nb, char* files[])
{
    int i, j, dim, nbf, nb_found = 0;

    if(nb == 0) 
    {
        // No filename given => read in dbv_ws (normally global_ws_var)
        nbf = KI_read_vars_db(dbv, dbv_ws, "WS");
        if(nbf < 0)
            return -1;
        nb_found += nbf;
    }
    else 
    {
        // Files given, search in files in the same order as they are listed
        for(i = 0;  i < nb && nb_found < dbv->size(); i++) 
        {
            if(strcmp(files[i], "WS") == 0)
                // Special name "WS" => read in dbv_ws 
                nbf = KI_read_vars_db(dbv, dbv_ws, "WS");
            else
                // Regular VAR file
                nbf = KI_read_vars_file(dbv, files[i]);
            
            if(nbf < 0)
                return -1;
            nb_found += nbf;
        }
    }

    // If all target VARs are not found, creates them with NaN values
    if(nb_found < dbv->size()) 
    {
        dim = dbv->sample->nb_periods;
        j = 0;
        // WARNING: we create a copy of the map dbv->k_objs since add() may modify dbv->k_objs.
        //          It is not safe to use for(auto& [key, value] : map) when the keys or values 
        //          can be modify inside the loop.
        std::map<std::string, SWHDL> k_objs_copy = dbv->k_objs;
        for(const auto& [name, handle] : k_objs_copy) 
        {
            // more than 10 exogenous vars not found => stop listing
            if(j > 10)
                break;

            // series already present in dbv
            if(handle > 0) 
                continue;

            // series = identity ("endogenous") => creates an IODE_NAN VA
            if(dbi->contains(name)) 
            {          
                dbv->set_obj(name, (double*) NULL);      
                continue;
            }

            j++;

            // Exogenous series not found => error
            std::string msg = "Exogenous variable '" + name + "' not found";
            error_manager.append_error(msg);
        }

        // all VARs found or created with NaN values
        if(j == 0) 
            return(0);
        
        // more than 10 exogenous vars not found => special msg
        if(j == 10) 
            error_manager.append_error("... others skipped"); 
        
        return(-2);
    }
    return(0);
}


/**
 *  Copies from the KDB dbs_tmp the unallocated Scalars of dbs (i.e. the Scalar with no associated object).
 *  
 *  @param [in] KDB*    dbs         KDB of Scalars to read
 *  @param [in] KDB*    dbs_tmp     temporary KDB (read from an external file) where the needed Scalars must be copied from
 *  @param [in] char*   source_name name of the input source (WS or filename)
 *  @return     int                 nb of Scalars copied
 *                                  -3 if there is no common sample between dbv_tmp and dbv
 */
static int KI_read_scls_db(CKDBScalars* dbs, CKDBScalars* dbs_tmp, char* source_name)
{
    if(KEXEC_TRACE) 
        W_printfDbl(".par1 enum_1\nFrom %s : ", source_name); /* JMP 19-10-99 */
    
    int nb_found = 0;
    for(auto& [name, handle] : dbs->k_objs) 
    {
        // scalar already loaded in dbs
        if(handle > 0) 
            continue;

        // scalar not present in dbs_tmp
        if(!dbs_tmp->contains(name)) 
            continue;

        // allocate the Scalar in dbs
        handle = (SWHDL) KS_alloc_scl();
        dbs->k_objs[name] = handle;
        
        // copy the scalar from dbs_tmp to dbs
        memcpy(dbs->get_obj(name), dbs_tmp->get_obj(name), sizeof(Scalar));

        if(KEXEC_TRACE) 
            W_printf((char*) "%s ", name.c_str());
        nb_found++;
    }

    if(KEXEC_TRACE) 
        W_printf((char*) "\n");
    return nb_found;
}


/**
 *  Tries to read in file the unallocated Scalars of dbs (i.e. the Scalars with no associated object).
 *  
 *  @param [in] KDB*    dbs     KDB of all needed Scalars for calculating the identities
 *  @param [in] char*   file    name of a Scalar file
 *  @return     int             nb of Scalars read 
 *                              -1 if the file cannot be opened
 *  
 */
static int KI_read_scls_file(CKDBScalars* dbs, char* file)
{
    char    **scls = NULL;
    int     nbs = 0, nbf;

    std::vector<std::string> scls_to_read;
    for(auto& [name, handle] : dbs->k_objs) 
    {
        // scalar already present
        if(handle > 0) 
            continue;
        scls_to_read.push_back(name);
    }

    // no Scalars to be read
    if(scls_to_read.size() == 0)
        return 0;

    CKDBScalars* kdb = new CKDBScalars(false);
    bool success = kdb->load(std::string(file));
    if(!success) 
    {
        std::string msg = "Scalar file '" + std::string(file) + "' not found";
        error_manager.append_error(msg);
        return -1;
    }

    nbf = KI_read_scls_db(dbs, kdb, file);
    delete kdb;
    kdb = nullptr;

    return nbf;
}


/**
 *  Reads, from a list of files, the Scalars needed to compute identities. 
 *  For the Scalars to be read in the current KDB of VARs, specify "WS" as filename (required unless nb ==0).
 *  
 *  @param [in] KDB*    dbs         Scalars to be read
 *  @param [in] KDB*    dbs_ws      current Scalar KDB (global_ws_scl)
 *  @param [in] int     nb          Number of Scalar input files
 *  @param [in] char*   files[]     list of input Scalar files, including "WS" for the current KDB of Scalars 
 *                                  if nb == 0, the needed Scalars are read from dbs_ws
 *  @return     int                 0 on success (all Scalars have been found)
 *                                  -1 if one of the files is not found
 *                                  -2 if some Scalars were not found in the files
 */

static int KI_read_scls(CKDBScalars* dbs, CKDBScalars* dbs_ws, int nb, char* files[])
{
    int nbf;
    int nb_found = 0;
    if(nb == 0) 
    {
        nbf = KI_read_scls_db(dbs, dbs_ws, "WS");
        if(nbf < 0) 
            return -1;
        nb_found += nbf;
    }
    else 
    {
        for(int i = 0;  i < nb && nb_found < dbs->size(); i++) 
        {
            if(strcmp(files[i], "WS") == 0)
                nbf = KI_read_scls_db(dbs, dbs_ws, "WS");
            else
                nbf = KI_read_scls_file(dbs, files[i]);

            if(nbf < 0) 
                return -1;
            nb_found += nbf;
        }
    }

    int j = 0;
    if(nb_found < dbs->size()) 
    {
        for(auto& [name, handle] : dbs->k_objs) 
        {
            if(j > 10)
                break;
            if(handle > 0) 
                continue;
            j++;
            
            std::string msg = "Scalar '" + name + "' not found";
            error_manager.append_error(msg);
        }

        if(j == 10) 
            error_manager.append_error("... others skipped");
        
        return -2;
    }

    return 0;
}


/**
 *  Sub function of KI_exec() that links and computes all identities in dbi after 
 *  all needed VARs and Scalars have been read and saved in dbv and dbs.
 *  
 *  
 *  @param [in] KDB*    dbv   Input VAR KDB
 *  @param [in] KDB*    dbs   Input Scalar KDB
 *  @param [in] KDB*    dbi   KDB of identities to compute
 *  @param [in] int*    order order of execution of the identities
 *  @param [in] Sample* smpl  execution Sample
 *  @return     int           0 on success
 *                            -1 on LEC execution error (DIV/0...)
 *  
 */
static int KI_execute(CKDBVariables* dbv, CKDBScalars* dbs, CKDBIdentities* dbi, 
    int* order, Sample* smpl)
{
    int     tot_lg, start;
    char    *tmp;
    double  d;

    start = smpl->start_period.difference(dbv->sample->start_period);
    if(start < 0) 
        start = 0;

    std::string idt_name;
    CLEC* idt_clec;
    for(int i = 0; i < dbi->size(); i++) 
    {
        idt_name = dbi->get_name(order[i]);
        idt_clec = dbi->get_obj(idt_name)->get_compiled_lec();
        tot_lg = idt_clec->tot_lg;
        tmp = SW_nalloc(tot_lg);
        memcpy(tmp, idt_clec, tot_lg);
        if(L_link(dbv, dbs, (CLEC *)tmp)) 
            return(-1);
        
        for(int t = start ; t < start + smpl->nb_periods ; t++) 
        {
            d = L_exec(dbv, dbs, (CLEC *)tmp, t);
            dbv->get_var_ptr(idt_name)[t] = d;
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
 *  @param [in] KDB*    dbs         Input Scalar KDB
 *  @param [in] int     ns          number of input Scalar files
 *  @param [in] char*   sfiles[]    Input Scalar files
 *  @param [in] Sample* in_smpl     execution Sample or NULL to select the current VAR KDB sample
 *  @return     KDB*                Variables KDB containing the variables calculated using the identities
 *                                  NULL on error (illegal Sample, empty dbi, vars or scls not found...).
 *                                  The specific message is added via IodeErrorManager::append_error().
 */
CKDBVariables* KI_exec(CKDBIdentities* dbi, CKDBVariables* dbv, int nv, char* vfiles[], CKDBScalars* dbs, 
    int ns, char* sfiles[], Sample* in_smpl)
{
    int* order;
    int  res;

    Sample* var_sample = global_ws_var->sample;
    Sample* exec_sample = nullptr; 
    if(in_smpl)
        exec_sample = new Sample(*in_smpl);
    else if(var_sample)
        exec_sample = new Sample(*var_sample);
    
    if(!exec_sample)
    {
        error_manager.append_error("Empty execution sample");
        return nullptr;
    }

    if(var_sample)
    {
        // execution sample ends after the end of the current Variables workspace sample 
        if(var_sample->end_period.difference(exec_sample->end_period) < 0) 
        {
            delete exec_sample;
            std::string msg = "Execution sample '" + exec_sample->to_string() + "' ";
            msg += "ends after the current Variables workspace sample '" + var_sample->to_string() + "'";
            error_manager.append_error(msg);
            return nullptr;
        }

        // execution sample starts before the start of the current Variables workspace sample 
        if(exec_sample->start_period.difference(var_sample->start_period) < 0) 
        {
            delete exec_sample;
            std::string msg = "Execution sample '" + exec_sample->to_string() + "' ";
            msg += "starts before the current Variables workspace sample '" + var_sample->to_string() + "'";
            error_manager.append_error(msg);
            return nullptr;
        }
    }
    
    if(dbi->size() == 0) 
    {
        delete exec_sample;
        error_manager.append_error("Empty set of identities");
        return nullptr;
    }

    order = KI_reorder(dbi);
    if(order == 0) 
    {
        delete exec_sample;
        error_manager.append_error("Circular identity definition");
        return nullptr;
    }

    CKDBVariables* dbv_i = KI_series_list(dbi);
    if(var_sample) 
        dbv_i->sample = new Sample(*var_sample);
    else  
        dbv_i->sample = new Sample(*exec_sample);

    if(KEXEC_TRACE) 
    {
        W_printf((char*) "\n.par1 tit_0\nExecution of identities\n");
        W_printf((char*) ".par1 tit_1\nParameters\n");
        std::string str_exec_sample = exec_sample->to_string();
        W_printf((char*) ".par1 par_1\nExecution sample : %s\n", (char*) str_exec_sample.c_str());
        W_printf((char*) ".par1 tit_1\nVariables loaded\n");
    }
    
    res = KI_read_vars(dbi, dbv_i, dbv, nv, vfiles);
    if(res != 0) 
    {
        SW_nfree(order);
        delete dbv_i;
        dbv_i = nullptr;
        delete exec_sample;
        return nullptr;
    }

    CKDBScalars* dbs_i = KI_scalar_list(dbi);
    if(KEXEC_TRACE) 
        W_printf((char*) ".par1 tit_1\nScalars loaded\n");
    res = KI_read_scls(dbs_i, dbs, ns, sfiles);
    if(res != 0) 
    {
        SW_nfree(order);
        delete dbv_i;
        dbv_i = nullptr;
        delete dbs_i;
        dbs_i = nullptr;
        delete exec_sample;
        return nullptr;
    }

    if(KEXEC_TRACE) 
        W_flush();

    KI_execute(dbv_i, dbs_i, dbi, order, exec_sample);
    KI_quick_extract(dbv_i, dbi);
    SW_nfree(order);
    delete dbs_i;
    dbs_i = nullptr;
    delete exec_sample;
    return dbv_i;
}

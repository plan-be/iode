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
 *      - if the VAR is created, the values outside the calculation sample are set to IODE_NAN.                                                                                                                Missing vars and scalars are collected from vfiles and sfiles.
 */
#include "api/pch.h"
#include "api/b_errors.h"
#include "api/utils/utils.h"
#include "api/lec/lec.h"
#include "api/write/write.h"
#include "api/objs/pack.h"
#include "api/objs/kdb.h"
#include "api/objs/identities.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"

#include <unordered_set>


void KDBIdentities::set_scl_files(char* arg)
{
    v_scl_files.clear();
    char** tbl_scl_files = B_ainit_chk(arg, NULL, 0);
    int nb_scl_files = SCR_tbl_size((unsigned char**) tbl_scl_files);
    if(nb_scl_files > 0)
    {
        for(int i=0; i < nb_scl_files; i++)
            v_scl_files.push_back(tbl_scl_files[i]);
    }
    SCR_free_tbl((unsigned char**) tbl_scl_files);
}

void KDBIdentities::set_var_files(char* arg)
{
    v_var_files.clear();
    char** tbl_var_files = B_ainit_chk(arg, NULL, 0);
    int nb_var_files = SCR_tbl_size((unsigned char**) tbl_var_files);
    if(nb_var_files > 0)
    {
        for(int i=0; i < nb_var_files; i++)
            v_var_files.push_back(tbl_var_files[i]);
    }
    SCR_free_tbl((unsigned char**) tbl_var_files);
}

void KDBIdentities::clear_files()
{
    v_scl_files.clear();
    v_var_files.clear();
}

/**
 *  Helper function used to compare 2 strings in series_list().
 *  
 *  @param [in] const char* pa  first string
 *  @param [in] const char* pb  second string
 *  @return     int             output of strcmp() if a and b are not null
 *                              -1 if a is NULL
 *                              1 if b is NULL and a is not null  
 */
int KDBIdentities::strcmp_helper(const char *pa, const char *pb)
{
    char *a = *(char **)pa, *b = *(char **)pb;

    if(a == 0) return -1;
    else if(b == 0) return 1;
    else return(strcmp(a, b));
}

int KDBIdentities::wrapper_strcmp(const void *pa, const void *pb)
{
    return strcmp_helper((char*) pa, (char*) pb);
}

/**
 *  Creates a KDB containing all variables found in this IDT KDB.
 *  
 *  @return     KDB*            KDB of all vars found. All vars are initialised to L_NaN
 */
KDBVariablesPtr KDBIdentities::series_list() const
{
    // Creates a list with all variable names encountered
    // (without checking for duplicates)
    std::shared_ptr<CLEC> clec;
    std::set<std::string> vars_to_compute;
    for(const auto& [idt_name, idt] : this->k_objs) 
    {
        vars_to_compute.insert(idt_name);
        clec = idt->get_compiled_lec();
        for(const std::string& name : clec->v_obj_names) 
        {
            if(is_coefficient(name)) 
                continue;
            vars_to_compute.insert(name);
        }
    }

    // Create a new KDB of vars with all the names in tbl
    Variable new_var;
    KDBVariablesPtr dbv_ptr = KDBVariables::Create(false);
    for(const std::string& name : vars_to_compute)
        dbv_ptr->set(name, new_var);

    return dbv_ptr;
}


/**
 *  Creates a KDB containing all scalars found in this IDT KDB.
 *  
 *  @return     KDB*            KDB of all scalars found.
 */
KDBScalarsPtr KDBIdentities::scalar_list() const
{
    Scalar new_scl;
    std::shared_ptr<CLEC> clec;
    KDBScalarsPtr dbs = KDBScalars::Create(false);
    for(const auto& [idt_name, idt] : this->k_objs) 
    {
        clec = idt->get_compiled_lec();
        CLEC clec_copy(*clec);
        for(const std::string& name : clec_copy.v_obj_names) 
        {
            if(!is_coefficient(name)) 
                continue;
            dbs->set(name, new_scl);
        }
    }

    return dbs;
}


/**
 *  Reconstructs dbv with the variables whose names are found in this database. The result is 
 *  a KDB (dbv modified) containing the vars computed from the identities
 *  (identities have the name of the output var).
 *  
 *  @param [in, out] KDB*   dbv     
 *  @return          int    0 always
 */
int KDBIdentities::quick_extract(KDBVariablesPtr dbv_ptr) const
{
    // get list of VARs names
    std::vector<std::string> names;
    names.reserve(dbv_ptr->size());
    for(const auto& [name, _] : dbv_ptr->k_objs)
        names.push_back(name);

    // keep only VARs that have the same name as an IDT
    for(const std::string& name : names)
    {
        if(!this->contains(name))
            dbv_ptr->remove(name);
    }

    return 0;
}


/**
 *  Computes the execution order of this KDB of identities by placing child identities after their "parents".
 *  If reordering is impossible, returns an empty vector.
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
 *  @return     std::vector<std::string>    execution order or empty vector if reordering is impossible             
 */
std::vector<std::string> KDBIdentities::reorder() const
{
    int nb_identities = this->size();
    std::vector<std::string> v_order(nb_identities, "");
    std::unordered_set<std::string> ordered;

    std::string name;
    int nb_ordered = 0;
    std::shared_ptr<CLEC> clec = nullptr;
    while(nb_ordered < nb_identities) 
    {
        bool success = false;
        for(const auto& [idt_name, idt] : this->k_objs) 
        {
            if(ordered.contains(idt_name)) 
                continue;
            
            clec = idt->get_compiled_lec();

            bool break_reached = false;
            for(const std::string& name : clec->v_obj_names) 
            {
                // name represents a coefficient -> continue looping
                if(is_coefficient(name)) 
                    continue;
                // identity name == variable name -> continue looping
                if(idt_name == name) 
                    continue;
                // variable name not found -> continue looping
                if(!this->contains(name)) 
                    continue;
                // identity already marked for execution -> continue looping
                if(ordered.contains(name)) 
                    continue;
                // variable name found and not marked for execution 
                // -> exit looping
                break_reached = true;
                break;
            }

            // add identity to execution order + mark it
            if(!break_reached) 
            {
                success = true;
                ordered.insert(idt_name);
                v_order[nb_ordered] = idt_name;
                nb_ordered++;
            }
        }

        if(!success) 
        {
            /* IDENTITIES LOOP */
            if(KEXEC_TRACE) 
            {
                for(const auto& [idt_name, idt] : this->k_objs) 
                {
                    if(ordered.contains(idt_name))
                        W_printfDbl(".par1 enum_1\nIdt %s Ok\n", idt_name);
                    else
                        W_printfDbl(".par1 enum_1\nIdt %s Circular\n", idt_name);
                }
            }
            
            v_order.clear();
            return v_order;
        }
    }

    return v_order;
}


/**
 *  Copies VARS from the dbv_tmp KDB to the dbv KDB
 *  The output sample is dbv's.
 *  
 *  @param [in] KDB*    dbv         KDB of vars to read
 *  @param [in] KDB*    dbv_tmp     temporary KDB (read from an external file) where the needed VARs must be copied from
 *  @param [in] char*   source_name name of the input source (WS or filename)
 *  @return     int                 nb of VARs copied
 *                                  -3 if there is no common sample between dbv_tmp and dbv
 *  
 */
int KDBIdentities::read_vars_db(KDBVariablesPtr dbv_ptr, KDBVariablesPtr dbv_tmp, const std::string& source_name)
{
    int start, start_tmp;

    // get list of VARs to be read (from dbv_tmp)
    std::set<std::string> vars_to_copy;
    for(const auto& [name, var_ptr] : dbv_ptr->k_objs)
    {
        // series already present
        if(idt_exec_loaded_vars.contains(name))
            continue;
        
        if(dbv_tmp->contains(name)) 
            vars_to_copy.insert(name);
    }

    // no VARs to be read
    if(vars_to_copy.size() == 0)
        return 0;

    auto vsmpl = dbv_ptr->get_sample();
    auto tsmpl = dbv_tmp->get_sample();
    if(!tsmpl)
    {
        std::string msg = "Function read_vars_db: the sample of the ";
        if(source_name == "WS")
            msg += "current Variables workspace";
        else
            msg += "database read from the file '" + source_name + "'";
        msg += " is empty";
        error_manager.append_error(msg);
        return -3;
    }

    // The sample of the KDB of the variables to read is empty 
    if(!vsmpl) 
    {
        dbv_ptr->set_sample(*tsmpl);
        vsmpl = dbv_ptr->get_sample();
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
        W_printfDbl(".par1 enum_1\nFrom %s : ", (char*) source_name.c_str());

    int nb_found = 0;
    for(const std::string& name : vars_to_copy)
    {
        std::shared_ptr<Variable> var_ptr = dbv_ptr->get_obj_ptr(name);
        // NOTE: should not happen because we check above that the VAR is present 
        //       in dbv before, but we put this check just in case to avoid a crash 
        if(!var_ptr)
        {
            std::string msg = "Execution of identities: something went wrong";
            msg += " when trying to read the variable '" + name + "'";
            throw std::runtime_error(msg); 
        }

        // series already loaded
        if(idt_exec_loaded_vars.contains(name))
            continue;

        // get values to be copied from dbv_tmp
        var_ptr = dbv_tmp->get_obj_ptr(name);
        if(!var_ptr)
        {
            std::string msg = "Execution of identities: the variable '" + name + "' has not been found in the ";
            if(source_name == "WS")
                msg += "current Variables workspace";
            else
                msg += "Variables workspace read from the file '" + source_name + "'";
            error_manager.append_error(msg);
            continue;
        }

        // copy the VAR from dbv_tmp to dbv
        Variable var(vsmpl->nb_periods, IODE_NAN);
        for(int t = 0; t < smpl.nb_periods; t++)
            var[start + t] = (*var_ptr)[start_tmp + t];

        // update dbv with the copied VAR
        dbv_ptr->remove(name);
        dbv_ptr->set(name, var);

        // update the list of loaded VARs
        idt_exec_loaded_vars.insert(name);
        
        if(KEXEC_TRACE)
            W_printf((char*) "%s ", name.c_str());
        
        nb_found++;
    }

    if(KEXEC_TRACE) 
        W_printf((char*) "\n");
    
    return nb_found;
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
int KDBIdentities::read_vars_file(KDBVariablesPtr dbv_ptr, const std::string& file)
{
    char    **vars = NULL;
    int     nbv = 0, nb_found;

    std::set<std::string> vars_to_read;
    for(const auto& [name, var_ptr] : dbv_ptr->k_objs) 
    {
        // series already loaded in dbv
        if(idt_exec_loaded_vars.contains(name)) 
            continue;
        
        vars_to_read.insert(name);
    }

    // no variables to be read
    if(vars_to_read.size() == 0)
        return 0;

    if(vars_to_read.size() > 0 && file.empty())
    {
        error_manager.append_error("The path of file to read the Variables is empty");
        return -1;
    }
    
    auto dbv_tmp_ptr = KDBVariables::KDBVariables::Create(false);
    bool success = dbv_tmp_ptr->load(file);
    if(!success) 
    {
        std::string msg = "Variables file '" + file + "' not found";
        error_manager.append_error(msg);
        return -1;
    }

    if(dbv_tmp_ptr->size() == 0) 
    {
        std::string msg = "Variables file '" + file + "' contains no variable";
        error_manager.append_error(msg);
        return -1;
    }

    nb_found = this->read_vars_db(dbv_ptr, dbv_tmp_ptr, file);
    return nb_found;
}


/**
 *  Reads from a list of files, the VARs needed to compute identities. 
 *  For the variables to be read in the current KDB of VARs, specify "WS" as filename (required unless nb ==0).
 *  
 *  @param [in] KDB*    dbv         list of series needed to calculate the identities
 *  @param [in] KDB*    dbv_ws      current VAR KDB (global_ws_var)
 *  @return     int                 0 on success (all vars have been found)
 *                                  -1 if one of the files is not found
 *                                  -2 if some vars are not found in the files
 */
int KDBIdentities::read_vars(KDBVariablesPtr dbv_ptr, KDBVariablesPtr dbv_ws)
{
    idt_exec_loaded_vars.clear();

    int nb_found = 0;
    int nb_found_total = 0;
    if(v_var_files.empty()) 
    {
        // No filename given => read in dbv_ws (normally global_ws_var)
        nb_found = this->read_vars_db(dbv_ptr, dbv_ws, "WS");
        if(nb_found < 0)
            return -1;
        nb_found_total += nb_found;
    }
    else 
    {
        for(const std::string& file : v_var_files) 
        {
            if(nb_found_total >= dbv_ptr->size())
                break;

            if(file == "WS")
                // Special name "WS" => read in dbv_ws 
                nb_found = this->read_vars_db(dbv_ptr, dbv_ws, "WS");
            else
                // Regular VAR file
                nb_found = this->read_vars_file(dbv_ptr, file);
            
            if(nb_found < 0)
                return -1;
            nb_found_total += nb_found;
        }
    }

    // If all target VARs are not found, creates them with NaN values
    if(nb_found_total < dbv_ptr->size()) 
    {
        int j = 0;
        int dim = dbv_ptr->get_sample()->nb_periods;
        // using iterator to avoid concurrent modification of dbv when we add the 
        // missing VARs with NaN values
        std::string name;
        std::shared_ptr<Variable> var_ptr;
        std::shared_ptr<Variable> new_var_ptr;
        for(auto it = dbv_ptr->k_objs.begin(); it != dbv_ptr->k_objs.end(); ++it)
        {
            name = it->first;
            var_ptr = it->second;

            // more than 10 exogenous vars not found => stop listing
            if(j > 10)
                break;

            // series already present in dbv
            if(idt_exec_loaded_vars.contains(name))
                continue;

            // series = identity ("endogenous") => creates an IODE_NAN Variable
            if(this->contains(name)) 
            {
                it->second.reset();
                new_var_ptr = std::make_shared<Variable>(dim, IODE_NAN);
                it->second = new_var_ptr;     
                continue;
            }

            j++;

            // Exogenous series not found => error
            std::string msg = "Exogenous variable '" + name + "' not found";
            error_manager.append_error(msg);
        }

        // all VARs found or created with NaN values
        if(j == 0)
            return 0;
        
        // more than 10 exogenous vars not found => special msg
        if(j == 10) 
            error_manager.append_error("... others skipped"); 
        
        return -2;
    }

    return 0;
}


/**
 *  Copies from the KDB dbs_tmp the unallocated Scalars of dbs (i.e. the Scalar with no associated object).
 *  
 *  @param [out] KDB*   dbs         KDB of Scalars to read
 *  @param [in] KDB*    dbs_tmp     temporary KDB (read from an external file) where the needed Scalars must be copied from
 *  @param [in] char*   source_name name of the input source (WS or filename)
 *  @return     int                 nb of Scalars copied
 *                                  -3 if there is no common sample between dbv_tmp and dbv
 */
int KDBIdentities::read_scls_db(KDBScalarsPtr& dbs_ptr, const KDBScalarsPtr dbs_tmp, const std::string& source_name)
{
    if(KEXEC_TRACE) 
        W_printfDbl(".par1 enum_1\nFrom %s : ", (char*) source_name.c_str());
    
    int nb_found = 0;
    for(const std::string& name : dbs_ptr->get_names()) 
    {
        // scalar already loaded in dbs
        if(dbs_ptr->contains(name)) 
            continue;

        // scalar not present in dbs_tmp
        if(!dbs_tmp->contains(name)) 
            continue;
        
        // copy the scalar from dbs_tmp to dbs
        dbs_ptr->copy_obj_from(dbs_tmp, name, name);

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
 *  @param [out] KDB*   dbs     KDB of all needed Scalars for calculating the identities
 *  @param [in] char*   file    name of a Scalar file
 *  @return     int             nb of Scalars read 
 *                              -1 if the file cannot be opened
 *  
 */
int KDBIdentities::read_scls_file(KDBScalarsPtr dbs_ptr, const std::string& file)
{
    char    **scls = NULL;
    int     nbs = 0, nb_found;

    std::set<std::string> scls_to_read;
    for(const auto& [name, scl_ptr] : dbs_ptr->k_objs) 
    {
        // scalar already present
        if(scl_ptr) 
            continue;
        
        scls_to_read.insert(name);
    }

    // no Scalars to be read
    if(scls_to_read.size() == 0)
        return 0;

    auto kdb_tmp_ptr = KDBScalars::KDBScalars::Create(false);
    bool success = kdb_tmp_ptr->load(file);
    if(!success) 
    {
        std::string msg = "Scalar file '" + file + "' not found";
        error_manager.append_error(msg);
        return -1;
    }

    nb_found = this->read_scls_db(dbs_ptr, kdb_tmp_ptr, file);
    return nb_found;
}


/**
 *  Reads, from a list of files, the Scalars needed to compute identities. 
 *  For the Scalars to be read in the current KDB of VARs, specify "WS" as filename (required unless nb ==0).
 *  
 *  @param [out] KDB*   dbs         Scalars to be read
 *  @param [in] KDB*    dbs_ws      current Scalar KDB (global_ws_scl)
 *  @return     int                 0 on success (all Scalars have been found)
 *                                  -1 if one of the files is not found
 *                                  -2 if some Scalars were not found in the files
 */

int KDBIdentities::read_scls(KDBScalarsPtr& dbs_ptr, const KDBScalarsPtr dbs_ws)
{
    int nb_found;
    int nb_found_total = 0;
    if(v_scl_files.empty()) 
    {
        nb_found = this->read_scls_db(dbs_ptr, dbs_ws, "WS");
        if(nb_found < 0) 
            return -1;
        nb_found_total += nb_found;
    }
    else 
    {
        for(const std::string& file : v_scl_files) 
        {
            if(nb_found_total >= dbs_ptr->size())
                break;

            if(file == "WS")
                nb_found = this->read_scls_db(dbs_ptr, dbs_ws, "WS");
            else
                nb_found = this->read_scls_file(dbs_ptr, file);

            if(nb_found < 0) 
                return -1;
            nb_found_total += nb_found;
        }
    }

    int j = 0;
    if(nb_found_total < dbs_ptr->size()) 
    {
        for(const auto& [name, scl_ptr] : dbs_ptr->k_objs) 
        {
            if(j > 10)
                break;
            
            if(scl_ptr) 
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
 *  Sub function of exec() that links and computes all identities after 
 *  all needed VARs and Scalars have been read and saved in dbv and dbs.
 *  
 *  
 *  @param [in] KDB*    dbv   Input VAR KDB
 *  @param [in] KDB*    dbs   Input Scalar KDB
 *  @param [in] int*    order order of execution of the identities
 *  @param [in] Sample* smpl  execution Sample
 *  @return     int           0 on success
 *                            -1 on LEC execution error (DIV/0...)
 *  
 */
int KDBIdentities::execute(KDBVariablesPtr dbv_ptr, KDBScalarsPtr dbs_ptr, 
    std::vector<std::string> v_order, const std::shared_ptr<Sample> smpl) const
{
    if(!smpl) 
    {
        error_manager.append_error("Empty execution sample");
        return -1;
    }

    int start = smpl->start_period.difference(dbv_ptr->get_sample()->start_period);
    if(start < 0) 
        start = 0;

    double d;
    std::shared_ptr<Identity> idt_ptr = nullptr;
    std::shared_ptr<CLEC> idt_clec = nullptr;
    std::shared_ptr<CLEC> clec_copy = nullptr;
    for(const std::string& idt_name : v_order) 
    {
        idt_ptr = this->get_obj_ptr(idt_name);
        idt_clec = idt_ptr->get_compiled_lec();
        if(!idt_clec) 
            return -1;
        
        clec_copy = std::make_shared<CLEC>(*idt_clec);
        if(clec_copy->link(dbv_ptr, dbs_ptr)) 
            return -1;
        
        for(int t = start ; t < start + smpl->nb_periods ; t++) 
        {
            d = clec_copy->execute(dbv_ptr, dbs_ptr, t);
            dbv_ptr->get_var_ptr(idt_name)[t] = d;
        }
    }

    return 0;
}


/**
 *  Executes all identities using the input series of dbv and scalars of dbs.
 *  Missing vars and scalars are collected from vfiles and sfiles.
 *
 *  @param [in] KDB*    dbv         Input VAR KDB
 *  @param [in] KDB*    dbs         Input Scalar KDB
 *  @param [in] Sample* in_smpl     execution Sample or NULL to select the current VAR KDB sample
 *  @return     KDB*                Variables KDB containing the variables calculated using the identities
 *                                  NULL on error (illegal Sample, empty dbi, vars or scls not found...).
 *                                  The specific message is added via IodeErrorManager::append_error().
 */
KDBVariablesPtr KDBIdentities::exec(KDBVariablesPtr dbv_ptr, KDBScalarsPtr dbs_ptr, Sample* in_smpl)
{
    std::shared_ptr<Sample> var_sample = global_ws_var->get_sample();
    std::shared_ptr<Sample> exec_sample = nullptr; 
    if(in_smpl)
        exec_sample = std::make_shared<Sample>(*in_smpl);
    else if(var_sample)
        exec_sample = std::make_shared<Sample>(*var_sample);
    
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
            std::string msg = "Execution sample '" + exec_sample->to_string() + "' ";
            msg += "ends after the current Variables workspace sample '" + var_sample->to_string() + "'";
            error_manager.append_error(msg);
            return nullptr;
        }

        // execution sample starts before the start of the current Variables workspace sample 
        if(exec_sample->start_period.difference(var_sample->start_period) < 0) 
        {
            std::string msg = "Execution sample '" + exec_sample->to_string() + "' ";
            msg += "starts before the current Variables workspace sample '" + var_sample->to_string() + "'";
            error_manager.append_error(msg);
            return nullptr;
        }
    }
    
    if(this->size() == 0) 
    {
        error_manager.append_error("Empty set of identities");
        return nullptr;
    }

    std::vector<std::string> v_order = this->reorder();
    if(v_order.empty()) 
    {
        error_manager.append_error("Circular identity definition");
        return nullptr;
    }

    KDBVariablesPtr dbv_i_ptr = this->series_list();
    if(var_sample) 
        dbv_i_ptr->set_sample(*var_sample);
    else  
        dbv_i_ptr->set_sample(*exec_sample);

    if(KEXEC_TRACE) 
    {
        W_printf((char*) "\n.par1 tit_0\nExecution of identities\n");
        W_printf((char*) ".par1 tit_1\nParameters\n");
        std::string str_exec_sample = exec_sample->to_string();
        W_printf((char*) ".par1 par_1\nExecution sample : %s\n", (char*) str_exec_sample.c_str());
        W_printf((char*) ".par1 tit_1\nVariables loaded\n");
    }
    
    int res = this->read_vars(dbv_i_ptr, dbv_ptr);
    idt_exec_loaded_vars.clear();
    if(res != 0) 
        return nullptr;

    KDBScalarsPtr dbs_i = this->scalar_list();
    if(KEXEC_TRACE) 
        W_printf((char*) ".par1 tit_1\nScalars loaded\n");
    
    res = this->read_scls(dbs_i, dbs_ptr);
    if(res != 0) 
        return nullptr;

    if(KEXEC_TRACE) 
        W_flush();

    this->execute(dbv_i_ptr, dbs_i, v_order, exec_sample);
    this->quick_extract(dbv_i_ptr);
    return dbv_i_ptr;
}

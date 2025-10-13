/**
 * @header4iode
 *
 * KDB management functions
 * ------------------------
 *
 * KDB structs contain the handles of all IODE objects of the same type plus some technical information like the 
 * filename the kdb has been read from, the processor architecture...
 *
 * For each IODE object type, there is one KDB (also called "workspace") in memory. 
 * 
 * To simplify the use of these structs, some MACRO have been defined for KDB in iode.h: for example "KV_WS" for the KDB of variables...
 *
 * For some very specific operations (comparison of workspaces for example), temporary KDB may be created for the duration 
 * of the operation.
 * 
 *      int K_key(char* name, int mode):                     // Checks the validity of an object name and modify its "case" according to the value of mode.
 *      KDB *K_refer(KDB* kdb, int nb, char* names[])        // creates a new kdb containing the **handles** of the objects listed in names.
 *      KDB *K_quick_refer(KDB *kdb, char *names[])          // same as K_refer() but more efficient for large databases.
 *      int K_merge(KDB* kdb1, KDB* kdb2, int replace)       // merges two databases : kdb1 <- kdb1 + kdb2. 
 *      int K_merge_del(KDB* kdb1, KDB* kdb2, int replace)   // merges two databases : kdb1 <- kdb1 + kdb2 then deletes kdb2. 
 */
#include "scr4/s_dir.h"

#include "api/b_errors.h"
#include "api/time/period.h"
#include "api/time/sample.h"
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/variables.h"


// Utilities 
// ---------

/**
 *  Checks the validity of an object name and modify its "case" according to the value of mode.
 *  The name is truncated if it exceeds K_MAX_NAME characters.
 *  
 *  
 *  
 *  @param [in, out] name    char*   name to check
 *  @param [in]      mode    int     UPPER_CASE, LOWER_CASE of ASIS_CASE 
 *  @return                  int     0 if the name is valid
 *                                   -1 otherwise (illegal character)
 *  
 *  @details 
 */
 
int K_key(char* name, int mode)
{
    int     i;

    SCR_sqz((unsigned char*) name);
    if(!SCR_is_alpha(name[0]) && name[0] != '_') 
        return(-1);
    
    if(strlen(name) > K_MAX_NAME) 
        name[K_MAX_NAME] = 0;
    for(i = 1 ; name[i] ; i++)
        if(!SCR_is_anum(name[i]) && name[i] != '_' && name[i] != K_SECRETSEP) 
            return(-1); // JMP 14/2/2013 pour les macros pushed A#n

    switch(mode) 
    {
        case UPPER_CASE :
            SCR_upper((unsigned char*) name);
            break;
        case LOWER_CASE :
            SCR_lower((unsigned char*) name);
            break;
        case ASIS_CASE  :
            break;
    }

    return(0);
}


// API 
// ---

/**
 *  Searches the position of an object in a KDB by its name.
 *  
 *  @param [in] kdb     KDB*    where to search for name
 *  @param [in] name    char*   name to be searched
 *  @return             int     position of the name in kdb
 *                              -1 if not found or if the name does not comply to the rules of kdb->k_type.
 */
 
SWHDL KDB::find(const char* name) const
{
    try 
    {
        return k_objs.at(std::string(name));
    }
    catch(const std::out_of_range&) 
    {
        return -1;
    }
}


int KDB::duplicate(const KDB& other, const std::string& name)
{
    char* c_name = (char*) name.c_str();
    int pos = K_dup(&other, c_name, this, c_name);
    return pos;
}


/**
 *  Sets the KDB full path name. 
 *  
 *  The current filename stored in the KDB is freed and space for the new filename is allocated in the KDB.
 *  
 *  @param [in, out]    kdb      KDB*   KDB whose name will be changed
 *  @param [in]         filename char*  new filename
 *  
 *  @details More details   
 */
void K_set_kdb_fullpath(KDB *kdb, U_ch *filename) 
{
    char    *ptr, fullpath[1024];
    
    ptr = SCR_fullpath((char*) filename, fullpath);
    if(ptr == 0) 
        ptr = (char*) filename;
    kdb->filepath = std::string((char*) ptr);  
}


/**
 *  Creates a new kdb containing the handles of the objects listed in names. 
 *  
 *  The data is **not** duplicated ("shallow copy") .
 *  
 *  On error, calls IodeErrorManager::append_error() with the following messages:
 *      - " %.80s : not found" if one of the names is not found
 *      - " %.80s : skipped (low memory)" if an entry cannot be created in the new DB
 *  
 *  @param [in] kdb   KDB*      source kdb
 *  @param [in] nb    int       number of names 
 *  @param [in] names char*[]   null terminated list of names
 *  @return           KDB*      shallow copy of kdb[names] on success
 *                              NULL if kdb is null or one of the names cannot be found
 */
KDB *K_refer(KDB* kdb, int nb, char* names[])
{
    int   err = 0;
    bool  found;
    SWHDL handle, thandle;
    KDB   *tkdb;
    std::string name;

    if(kdb == NULL) 
        return(NULL);
    
    tkdb = new KDB((IodeType) kdb->k_type, DB_SHALLOW_COPY);
    if(kdb->sample)
        tkdb->sample = new Sample(*kdb->sample);
    tkdb->description = kdb->description;
    tkdb->k_compressed = kdb->k_compressed;
    tkdb->filepath = kdb->filepath;

    for(int i = 0 ; i < nb && names[i]; i++) 
    {
        name = std::string(names[i]);
        handle = kdb->find(name);
        if(handle <= 0)  
        {
            error_manager.append_error(v_iode_types[kdb->k_type] + name + " not found: ");
            err = 1;
            continue;
        }

        thandle = K_add_entry(tkdb, names[i]);
        if(thandle <= 0) 
        {
            error_manager.append_error(v_iode_types[kdb->k_type] + name + " skipped: (low memory)");
            delete tkdb;
            tkdb = nullptr;
            err = 1;
            break;
        }

        KSOVAL(thandle) = KSOVAL(handle);
    }

    if(err) 
        error_manager.display_last_error();

    return tkdb;
}


/**
 *  Creates a new kdb containing the handles of the objects listed in names. 
 *  
 *  Same as K_refer() but more efficient for large databases.
 *  
 *  The data is **not** duplicated ("shallow copy") .
 *  
 *  @param [in] kdb   KDB*      source kdb
 *  @param [in] names char*[]   null terminated list of names
 *  @return           KDB*      shallow copy of kdb[names]
 *                              NULL if kdb is null or one of the names cannot be found
 *  
 *  @note Quicker version of K_refer() (JMP 16/3/2012) by allocating KOBJS in one call instead
 *        of calling K_add_entry for each name.
 *  @note Programmed for Institut Erasme and Nemesis model (> 250.000 Vars)
 */

KDB *K_quick_refer(KDB *kdb, char *names[])
{
    int   i, nb = SCR_tbl_size((unsigned char**) names);
    bool  found, success;
    KDB   *tkdb;
    std::string name;

    if(!kdb) 
        return nullptr;

    // Crée la nouvelle kdb avec le nombre exact d'entrées
    tkdb = new KDB((IodeType) kdb->k_type, DB_SHALLOW_COPY);
    if(kdb->sample)
        tkdb->sample = new Sample(*kdb->sample);
    tkdb->description = kdb->description;
    tkdb->k_compressed = kdb->k_compressed;
    tkdb->filepath = kdb->filepath;;

    // copy the pointers to IODE objects from kdb to tkdb
    for(i = 0; i < nb; i++) 
    {
        name = std::string(names[i]);
        found = kdb->contains(name);
        if(!found) 
        {
            delete tkdb;
            return nullptr;
        }
        tkdb->k_objs[name] = kdb->k_objs[name];
    }

    return tkdb;
}


/**
 *  Merges two databases : kdb1 <- kdb1 + kdb2. 
 *   If replace == 0, existing elements in kdb1 are not overwritten.
 *   If replace == 1, the values of existing elements are replaced.
 *
 *  kdb2 is kept unmodified.
 *  
 *  @param [in, out]    kdb1    KDB*      source and target KDB
 *  @param [in]         kdb2    KDB*      source KDB
 *  @param [in]         replace int       0 to keep elements of kdb1 when duplicate objects exist
 *                                        1 to replace values in kdb1 by these in kdb2
 *  @return                     int       -1 if kdb1 or kdb2 is null
 */

int K_merge(KDB* kdb1, KDB* kdb2, int replace)
{
    int   i;
    bool  found, success;
    char  *ptr;
    SWHDL handle_kdb1;

    if(kdb1 == NULL || kdb2 == NULL) 
        return(-1);
    
    for(auto& [name, handle] : kdb2->k_objs) 
    {
        found = kdb1->contains(name);
        if(!found) 
            handle_kdb1 = K_add_entry(kdb1, name);
        else 
        {
            if(!replace) continue;
            SW_free(handle_kdb1);
        }

        if(pos < 0) 
            return(-1);
        ptr = KGOVAL(kdb2, i);
        KSOVAL(kdb1, pos) = SW_alloc(P_len(ptr));

        ptr = KGOVAL(kdb2, i); /* GB 26/01/98 */
        memcpy(KGOVAL(kdb1, pos), ptr, P_len(ptr));
    }
    return(0);
}


/**
 *  Merges two databases : kdb1 <- kdb1 + kdb2 then deletes kdb2. 
 *   If replace == 0, existing elements in kdb1 are not overwritten.
 *   If replace == 1, the values of existing elements are replaced.
 *
 *  
 *  @param [in, out]    kdb1    KDB*      source and target KDB
 *  @param [in, out     kdb2    KDB*      source KDB. Delete at the end of the function
 *  @param [in]         replace int       0 to keep elements of kdb1 when duplicate objects exist
 *                                        1 to replace values in kdb1 by these in kdb2
 *  @return                     int       -1 if kdb1 or kdb2 is null or if kdb2 is empty.
 */

int K_merge_del(KDB* kdb1, KDB* kdb2, int replace)
{

    if(kdb1 == NULL || kdb2 == NULL) 
        return(-1);
    
    if(kdb2->size() == 0) 
        return(-1);
    
    if(kdb1->size() == 0) 
    {
        kdb1->k_objs = kdb2->k_objs;
        kdb2->k_objs = 0;
        delete kdb2;
        kdb2 = nullptr;
        return 0;
    }

    K_merge(kdb1, kdb2, replace);
    delete kdb2;
    kdb2 = nullptr;
    return 0;
}

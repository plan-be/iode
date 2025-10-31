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
 *      KDB *K_refer(KDB* kdb, int nb, char* names[])        // creates a new kdb containing the **handles** of the objects listed in names.
 *      KDB *K_quick_refer(KDB *kdb, int nb char *names[])   // same as K_refer() but more efficient for large databases.
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


// API 
// ---


int KDB::duplicate(const KDB& other, const std::string& name)
{
    int pos = K_dup(&other, name, this, name);
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
    bool  err = false;
    SWHDL handle;
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

    for(int i = 0; i < nb && names[i]; i++) 
    {
        name = std::string(names[i]);
        handle = kdb->get_handle(name);
        if(handle == 0)  
        {
            error_manager.append_error(v_iode_types[kdb->k_type] + name + " not found: ");
            err = true;
            continue;
        }
        tkdb->k_objs[name] = handle;
    }

    if(err)
    {
        delete tkdb;
        tkdb = nullptr;
        error_manager.display_last_error();
    }

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
 *  @param [in] nb    int       number of names 
 *  @param [in] names char*[]   null terminated list of names
 *  @return           KDB*      shallow copy of kdb[names]
 *                              NULL if kdb is null or one of the names cannot be found
 *  
 *  @note Quicker version of K_refer() (JMP 16/3/2012) by allocating KOBJS in one call instead
 *        of calling K_add_entry for each name.
 *  @note Programmed for Institut Erasme and Nemesis model (> 250.000 Vars)
 */

KDB *K_quick_refer(KDB *kdb, int nb, char *names[])
{
    bool  err = false;
    bool  found;
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
    for(int i = 0; i < nb; i++) 
    {
        name = std::string(names[i]);
        found = kdb->contains(name);
        if(!found) 
        {
            error_manager.append_error(v_iode_types[kdb->k_type] + std::string(names[i]) + " not found");
            err = true;
            break;
        }
        tkdb->k_objs[name] = kdb->k_objs[name];
    }

    if(err)
    {
        delete tkdb;
        tkdb = nullptr;
        error_manager.display_last_error();
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
    bool  found;
    char  *ptr_1, *ptr_2;
    SWHDL handle_1;

    if(!kdb1)
    {
        kwarning("Cannot merge into a NULL database");
        return -1;
    }

    if(!kdb2)
    {
        kwarning("Cannot merge from a NULL database");
        return -1;
    }
    
    for(auto& [name, handle_2] : kdb2->k_objs) 
    {
        if(handle_2 == 0) 
            continue;

        found = kdb1->contains(name);
        if(found)
        {
            // do not replace existing object in kdb1 if replace == 0
            if(replace == 0) 
                continue;
            // delete object in kdb1 if replace == 1
            handle_1 = kdb1->get_handle(name);
            if(handle_1 > 0)
                SW_free(handle_1);
        }
        
        ptr_2 = SW_getptr(handle_2);

        // allocate new object in kdb1 on the SCR swap
        handle_1 = SW_alloc(P_len(ptr_2));
        if(handle_1 == 0) 
            return -1;
        kdb1->k_objs[name] = handle_1;

        // pointer behind handle_2 may have changed after allocation of handle_1
        ptr_2 = SW_getptr(handle_2);
        
        ptr_1 = SW_getptr(handle_1);
        memcpy(ptr_1, ptr_2, P_len(ptr_2));
    }

    return 0;
}


/**
 *  Merges two databases : kdb1 <- kdb1 + kdb2 then deletes kdb2 (but not the IODE objects in memory). 
 *   If replace == 0, existing elements in kdb1 are not overwritten.
 *   If replace == 1, the values of existing elements are replaced.
 *  
 *  @param [in, out]    kdb1    KDB*      source and target KDB
 *  @param [in, out     kdb2    KDB*      source KDB. Delete at the end of the function
 *  @param [in]         replace int       0 to keep elements of kdb1 when duplicate objects exist
 *                                        1 to replace values in kdb1 by these in kdb2
 *  @return                     int       -1 if kdb1 or kdb2 is null or if kdb2 is empty.
 */

int K_merge_del(KDB* kdb1, KDB* kdb2, int replace)
{

    if(!kdb1)
    {
        kwarning("Cannot merge into a NULL database");
        return -1;
    }

    if(!kdb2)
    {
        kwarning("Cannot merge from a NULL database");
        return -1;
    }
    
    if(kdb2->size() == 0) 
        return -1;
    
    if(kdb1->size() == 0) 
    {
        kdb1->k_objs = kdb2->k_objs;
        kdb2->clear(false);
        kdb2 = nullptr;
        return 0;
    }

    K_merge(kdb1, kdb2, replace);
    kdb2->clear(false);
    kdb2 = nullptr;
    return 0;
}

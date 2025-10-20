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


// Utilities 
// ---------

/**
 *  Compares the names of 2 KOBJs. Used by K_sort().
 *  
 *  @param [in] p1  void*   pointer to the first KOBJ 
 *  @param [in] p2  void*   pointer to the 2d KOBJ 
 *  @return         int     0 if name of p1 = name of p2 (the names ares equal)
 *                          -1 if p1 is < p2 or p1 is null
 *                          1 if p1 is > p2 or p2 is null and is not null
 *  
 *  @details Calls strcmp() for string comparison (case sensitive)
 */

static int K_objnamecmp(const void *p1, const void *p2)
{
    KOBJ    *ko1 = (KOBJ *)p1, *ko2 = (KOBJ *)p2;

    if(ko1 == 0) return(-1);
    if(ko2 == 0) return(1);
    return(strcmp(ko1->o_name, ko2->o_name));
}


/**
 *  Sorts a KDB by object names. Names are case sensitive.
 *  
 *  @param [in, out] kdb     KDB*    kdb to be sorted
 *  
 */
 
void K_sort(KDB* kdb)
{
    qsort(kdb->k_objs, (int) kdb->size(), sizeof(KOBJ), K_objnamecmp);
}

/**
 *  Compares a name with the name of an object (kobjs->o_name).
 *  
 *  @param [in] name    const void*     name to compare
 *  @param [in] kobjs   const KOBJ*     pointer to a general object (KOBJ)
 *  @return             int             0 if name of p1 = name of p2 (the names ares equal)
 *                                      -1 if p1 is < p2 or p1 is null
 *                                      1 if p1 is > p2 or p2 is null and is not null
 *  @return 
 *  
 *  @details 
 */

int K_find_strcmp(const void *name, const void *kobjs)
{
    KOBJ    *ko2 = (KOBJ *)kobjs;
    return(strcmp((char *)name, ko2->o_name));
}

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
 
int KDB::find(const char* name) const
{
    char    *res;
    ONAME   oname;

    if(this->size() == 0) 
        return(-1);

    SCR_strlcpy((unsigned char*) oname, (unsigned char*) name, K_MAX_NAME);  
    if(K_key(oname, this->k_mode) < 0) 
        return(-1);

    res = (char *) bsearch(oname, this->k_objs, (int) this->size(),
                           sizeof(KOBJ), K_find_strcmp);
    if(res != 0) 
        return((int)((res - (char *) this->k_objs) / sizeof(KOBJ)));
    else 
        return(-1);
}


int KDB::duplicate(const KDB& other, char* name)
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
    int     pos1, pos2, err = 0;
    KDB     *tkdb;

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
        pos2 = kdb->find(names[i]);
        if(pos2 < 0)  
        {
            error_manager.append_error(v_iode_types[kdb->k_type] + std::string(names[i]) + " not found: ");
            err = 1;
            continue;
        }

        pos1 = K_add_entry(tkdb, names[i]);
        if(pos1 < 0) 
        {
            error_manager.append_error(v_iode_types[kdb->k_type] + std::string(names[i]) + " skipped: (low memory)");
            delete tkdb;
            tkdb = nullptr;
            err = 1;
            break;
        }

        KSOVAL(tkdb, pos1) = KSOVAL(kdb, pos2);
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
    bool    err = false;
    int     pos;
    KDB     *tkdb;

    if(!kdb) 
        return nullptr;

    // Crée la nouvelle kdb avec le nombre exact d'entrées
    tkdb = new KDB((IodeType) kdb->k_type, DB_SHALLOW_COPY);
    if(kdb->sample)
        tkdb->sample = new Sample(*kdb->sample);
    tkdb->description = kdb->description;
    tkdb->k_compressed = kdb->k_compressed;
    tkdb->filepath = kdb->filepath;

    tkdb->k_objs = (KOBJ *) SW_nalloc(sizeof(KOBJ) * K_CHUNCK * (1 + nb / K_CHUNCK));
    tkdb->k_nb = nb;
    for(int j = 0; j < nb; j++) 
    {
        tkdb->k_objs[j].o_val = 0;
        memset(tkdb->k_objs[j].o_name, 0, sizeof(ONAME));
    }

    // Copie les entrées dans tkdb
    for(int i = 0; i < nb && names[i]; i++)  
    {
        pos = kdb->find(names[i]);
        if(pos < 0) 
        {
            error_manager.append_error(v_iode_types[kdb->k_type] + std::string(names[i]) + " not found");
            delete tkdb;
            err = true;
            break;
        }
        memcpy(tkdb->k_objs + i, kdb->k_objs + pos, sizeof(KOBJ));
    }

    if(err) 
    {
        error_manager.display_last_error();
        return nullptr;
    }

    // Sort tkdb
    K_sort(tkdb);

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
    int     i, pos;
    char    *ptr;

    if(kdb1 == NULL || kdb2 == NULL) return(-1);
    for(i = 0; i < kdb2->size(); i++) {
        pos = kdb1->find(KONAME(kdb2, i));
        if(pos < 0) pos = K_add_entry(kdb1, KONAME(kdb2, i));
        else {
            if(!replace) continue;
            SW_free(KSOVAL(kdb1, pos));
        }

        if(pos < 0) return(-1);
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
        kdb1->k_nb = kdb2->k_nb;
        kdb1->k_objs = kdb2->k_objs;
        kdb2->k_nb = 0;
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

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
 *      KDB *K_init_kdb(int type, char* filename);           // allocates and initialises a KDB struct
 *      char *K_get_kdb_nameptr(KDB *kdb)                    // gets the KDB filename pointer stored in kdb->k_nameptr. 
 *      void K_set_kdb_name(KDB* kdb, U_ch* filename);       // changes the filename in a KDB
 *      KDB *K_create(int type, int mode)                    // allocates and initialises a KDB object.    
 *      int K_free_kdb(KDB* kdb)                             // frees a KDB but leaves its contents untouched.
 *      int K_free(KDB* kdb)                                 // frees a KDB and its contents.
 *      int K_clear(KDB* kdb)                                // deletes all objects in a KDB, reset the SAMPLE and replaces the filename by I_DEFAULT_FILENAME. 
 *      KDB *K_refer(KDB* kdb, int nb, char* names[])        // creates a new kdb containing the **handles** of the objects listed in names.
 *      KDB *K_quick_refer(KDB *kdb, char *names[])          // same as K_refer() but more efficient for large databases.
 *      int K_merge(KDB* kdb1, KDB* kdb2, int replace)       // merges two databases : kdb1 <- kdb1 + kdb2. 
 *      int K_merge_del(KDB* kdb1, KDB* kdb2, int replace)   // merges two databases : kdb1 <- kdb1 + kdb2 then deletes kdb2. 
 */
#include "scr4/s_dir.h"

#include "api/b_errors.h"
#include "api/utils/time.h"
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
    qsort(KOBJS(kdb), (int) KNB(kdb), sizeof(KOBJ), K_objnamecmp);
}

// API 
// ---


/**
 *  @brief Allocates and initialise a KDB struct.
 *  
 *  Object "mode" (UPPER_CASE, LOWER_CASE or ASIS_CASE) is assigned according to object type. 
 *  
 *  @param [in] type        int     KDB object type (COMMENTS...VARIABLES).
 *  @param [in] filename    char*   file where the KDB will be saved (can be changed before saving the KDB).
 *  @return                 KDB*    allocated KDB. 
 *  
 */
KDB *K_init_kdb(int type, char* filename)
{
    int     mode;
    KDB     *kdb;

    switch(type) {
        case COMMENTS :
            mode = ASIS_CASE;
            break;
        case EQUATIONS :
        case IDENTITIES :
        case LISTS :
        case TABLES :
        case VARIABLES :
            mode = UPPER_CASE;
            break;
        case SCALARS :
            mode = LOWER_CASE;
            break;
    }
    kdb = K_create(type, mode);
    if(kdb == NULL) return(kdb);
    //strcpy(KNAME(kdb), filename);
    K_set_kdb_name(kdb, (unsigned char*) filename); // JMP 3/6/2015
    return(kdb);
}


/**
 *  Gets the KDB filename pointer stored in kdb->k_nameptr. 
 *  
 *  Because the k_nameptr is aligned on 4 bytes in the KDB structs,
 *  this function must be compiled with an alignment parameter on 4 bytes max (for ex. /Zp1 in VS).
 *  
 *  @param [in]    kdb      KDB*   input KDB 
 *  @return                 char*  value of k_nameptr of NULL if kdb is NULL
 */
char *K_get_kdb_nameptr(KDB *kdb)
{
    if(kdb) return(KNAMEPTR(kdb));
    else    return(NULL);
}



/**
 *  Sets the KDB filename. 
 *  
 *  The current filename stored in the KDB is freed and space for the new filename is allocated in the KDB.
 *  
 *  @param [in, out]    kdb      KDB*   KDB whose name will be changed
 *  @param [in]         filename char*  new filename
 *  
 *  @details More details   
 */
void K_set_kdb_name(KDB *kdb, U_ch *filename) 
{
    if(kdb) {
        SCR_free(KNAMEPTR(kdb));
        KNAMEPTR(kdb) = (char*) SCR_stracpy(filename);
    }
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
    if(ptr == 0) ptr = (char*) filename;
    K_set_kdb_name(kdb, (unsigned char*) ptr);  
}


/**
 *  Allocates and initialises a KDB object.
 *  
 *  Depending on value of the mode parameter, the object names will be automatically translated to lower or upper case or let unmodified.
 *    
 *  @param [in] type    int     type of object the KDB will contain
 *  @param [in] mode    int     case of the object name: UPPER_CASE, LOWER_CASE or ASIS_CASE 
 *  @return             KDB*    new allocated KDB
 *  
 */

KDB *K_create(int type, int mode)
{
    KDB   *kdb;

    kdb = (KDB *) SW_nalloc(sizeof(KDB));
    if(kdb == NULL) return(NULL);
    strcpy(KARCH(kdb), ARCH);
    KMODE(kdb) = mode;
    KTYPE(kdb) = type;
    KNAMEPTR(kdb) = (char*) SCR_stracpy((unsigned char*) I_DEFAULT_FILENAME); // JMP 29/9/2015
    return(kdb);
}


/**
 *  Frees a KDB but leaves its contents untouched. 
 *  
 *  Usage: in the function K_refer() for example, object handles are copied (shallow copy) into a (temporary) new allocated KDB. 
 *  When this temporary KDB in not needed anymore, the struct is deleted (freed), but not the objects.
 *  
 *  @param [in, out] kdb    KDB* kdb to be deleted.
 *  @return                 int  0
 *  
 *  TODO: free KNAMEPTR ?
 */

int K_free_kdb(KDB* kdb)
{
    if(kdb == NULL) return(0); // JMP 28/09/2022
    SW_nfree(KOBJS(kdb));
    SW_nfree(kdb);
    return(0);
}


/**
 *  Frees a KDB and its contents.  But does not free the KDB struct.
 *  
 *  @param [in, out] kdb    KDB* kdb to be deleted
 *  @return                 int  0
 */

int K_free(KDB* kdb)
{
    int i;

    if(kdb == NULL) return(0);
    for(i = 0; i < KNB(kdb); i++)
        if(KOBJS(kdb)[i].o_val != 0) SW_free(KOBJS(kdb)[i].o_val);

    SCR_free(KNAMEPTR(kdb)); 
    KNAMEPTR(kdb) = 0;       
    K_free_kdb(kdb);
    return(0);
}


/**
 *  Deletes all objects in a KDB, reset the SAMPLE and replaces the filename by I_DEFAULT_FILENAME. 
 *  But does not free the KDB struct.
 *  
 *  @param [in, out] kdb    KDB* kdb to be cleared
 *  @return                 int  0
 */

int K_clear(KDB* kdb)
{
    int i;

    for(i = 0; i < KNB(kdb); i++) SW_free(KOBJS(kdb)[i].o_val);
    SW_nfree(KOBJS(kdb));
    KOBJS(kdb) = NULL;
    KNB(kdb) = 0;
    SCR_free(KNAMEPTR(kdb)); // JMP 3/6/2015
    KNAMEPTR(kdb) = 0;              // JMP 3/6/2015
    memset(KSMPL(kdb), 0, sizeof(SAMPLE)); /* JMP 28-03-92 */
    KNAMEPTR(kdb) = (char*) SCR_stracpy((unsigned char*) I_DEFAULT_FILENAME); // JMP 29/9/2015
    return(0);
}


/**
 *  Creates a new kdb containing the handles of the objects listed in names. 
 *  
 *  The data is **not** duplicated ("shallow copy") .
 *  
 *  On error, calls B_seterrn: 
 *      - 98 if one of the names is not found
 *      - 99 if an entry cannot be created in the new DB
 *  
 *  @param [in] kdb   KDB*      source kdb
 *  @param [in] nb    int       number of names 
 *  @param [in] names char*[]   null terminated list of names
 *  @return           KDB*      shallow copy of kdb[names] on success
 *                              NULL if kdb is null or one of the names cannot be found
 *  
 *  TODO: replace B_seterrn() by sth else ?
 */
 
KDB *K_refer(KDB* kdb, int nb, char* names[])
{
    int     i, pos1, pos2, err = 0;
    KDB     *tkdb;

    if(kdb == NULL) return(NULL);
    tkdb = K_create(KTYPE(kdb), KMODE(kdb));
    memcpy(KDATA(tkdb), KDATA(kdb), K_MAX_DESC);

    for(i = 0 ; i < nb && names[i]; i++) {
        pos2 = K_find(kdb, names[i]);
        if(pos2 < 0)  {
            B_seterrn(98, names[i]);
            err = 1;
            continue;
        }

        pos1 = K_add_entry(tkdb, names[i]);
        if(pos1 < 0) {
            B_seterrn(99, names[i]);
            K_free_kdb(tkdb);
            tkdb = NULL;
            err = 1;
            break;
        }
        KSOVAL(tkdb, pos1) = KSOVAL(kdb, pos2);
    }
    if(err) B_display_last_error();
    return(tkdb);
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
 *          of calling K_add_entry for each name.
 *  @note Programmed for Institut Erasme and Nemesis model (> 250.000 Vars)
 */

KDB *K_quick_refer(KDB *kdb, char *names[])
{
    int     i, pos, nb = SCR_tbl_size((unsigned char**) names);
    KDB     *tkdb;

    if(kdb == NULL) return(NULL);

    // Crée la nouvelle kdb avec le nombre exact d'entrées
    tkdb = K_create(KTYPE(kdb), KMODE(kdb));
    memcpy(KDATA(tkdb), KDATA(kdb), K_MAX_DESC);
    KOBJS(tkdb) = (KOBJ *) SW_nalloc(sizeof(KOBJ) * K_CHUNCK * (1 + nb / K_CHUNCK));
    KNB(tkdb) = nb;

    // Copie les entrées dans tkdb
    for(i = 0 ; i < nb; i++) {
        pos = K_find(kdb, names[i]);
        if(pos < 0) {
            K_free_kdb(tkdb);
            return(NULL);
        }
        memcpy(KOBJS(tkdb) + i, KOBJS(kdb) + pos, sizeof(KOBJ));
    }

    // Sort tkdb
    K_sort(tkdb);

    return(tkdb);
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
    for(i = 0; i < KNB(kdb2); i++) {
        pos = K_find(kdb1, KONAME(kdb2, i));
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

    if(kdb1 == NULL || kdb2 == NULL) return(-1);
    if(KNB(kdb2) == 0) return(-1);
    if(KNB(kdb1) == 0) {
        KNB(kdb1) = KNB(kdb2);
        KOBJS(kdb1) = KOBJS(kdb2);
        KNB(kdb2) = 0;
        KOBJS(kdb2) = 0;
        K_free(kdb2);
        return(0);
    }

    K_merge(kdb1, kdb2, replace);
    K_free(kdb2);
    return(0);
}

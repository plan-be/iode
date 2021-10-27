/**
 * @header4iode
 *
 * Add an object to a KDB of any type
 * ----------------------------------
 *     int K_add(KDB* kdb, char* name, char* a1, char* a2, char* a3, char* a4, char* a5, char* a6, char* a7, char* a8, char* a9):
 *                      Adds an object to a KDB. The number of arguments depends on object type.
 *
 */

#define _IODEVARG_
#include "iode.h"

/**
 *  Adds an object to a KDB. The number of arguments depends on object type. 
 *   
 *  The object is first packed (see k_pack.c). The resulting pack is then copied in the swap memory. 
 *  The handle of the allocated swap memory is stored in the kdb.
 *
 *  If name exists in kdb, the existing object is deleted and replaced by the new one.
 *  It returns the position of the object in the DB.
 *  
 *  Example
 *  -------
 *      EQ*     eq;
 *      char*   name;
 *      ...
 *      rc = K_add(K_WS[K_EQS], name, eq, name);
 *  
 *  
 *  @param [in, out]    kdb     KDB*    kdb to which the object is added
 *  @param [in]         name    char*   object name
 *  @param [in]         a1..a9  char*   elements defining object content
 *  @return                     int     -1 if kdb is NULL
 *                                      -2 if the object cannot be created (LEC syntax error for example)
 *                                      on success, position of the new object in kdb 
 *  
 */
int K_add(KDB* kdb, char* name, char* a1, char* a2, char* a3, char* a4, char* a5, char* a6, char* a7, char* a8, char* a9)
{
    char    *pack;
    int     pos, lg;

    if(kdb == NULL) return(-1);
    if(K_pack[KTYPE(kdb)](&pack, a1, a2, a3, a4, a5, a6, a7, a8, a9) < 0)
        return(-2);
    pos = K_add_entry(kdb, name);
    if(pos < 0) {
        B_seterror("%.80s cannot be created (syntax ?)", name);
        goto einde;
    }

    lg = * (OSIZE *) pack;
    if(KSOVAL(kdb, pos) != 0) SW_free(KSOVAL(kdb, pos));
    KSOVAL(kdb, pos) = SW_alloc(lg);
    memcpy(KGOVAL(kdb, pos), pack, lg);
einde:
    SW_nfree(pack);
    return(pos);
}


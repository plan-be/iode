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
#include <stdarg.h>

#include "api/constants.h"
#include "api/b_errors.h"
#include "api/objs/kdb.h"
#include "api/objs/pack.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"
#include "api/objs/objs.h"


/**
 *  Adds an object to a KDB. The number of arguments depends on object type. 
 *   
 *  The object is first packed (see k_pack.c). The resulting pack is then copied in the swap memory. 
 *  The handle of the allocated swap memory is stored in the kdb.
 *
 *  If name exists in kdb, the existing object is deleted and replaced by the new one.
 *  It returns the position of the object in the DB.
 *  
 *  How to create IODE objects with K_add()
 *  ---------------------------------------
 *    - Comments    K_add(KDB* kdb, char* name, CMT cmt)
 *    - Equations   K_add(KDB* kdb, char* name, EQ* eq, char* endo) [where endo = name]
 *    - Identities  K_add(KDB* kdb, char* name, char* lec)
 *    - Lists       K_add(KDB* kdb, char* name, char* list)
 *    - Scalars     K_add(KDB* kdb, char* name, SCL* scalar)
 *    - Tables      K_add(KDB* kdb, char* name, TBL *tbl) 
 *    - Variables   K_add(KDB* kdb, char* name, double* var, int* nb_obs) [nb_obs = kdb SAMPLE size]
 *  
 *  @note: the name of an equation MUST be the name of its endogenous variable
 *  
 *  Example
 *  -------
 *      EQ*     eq;
 *      char*   name;
 *      ...
 *      rc = K_add(K_WS[EQUATIONS], name, eq, name);
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
 
// OLD VERSION 
//
//int K_add(KDB* kdb, char* name, char* a1, char* a2, char* a3, char* a4, char* a5, char* a6, char* a7, char* a8, char* a9)
//{
//    char    *pack;
//    int     pos, lg;
//
//    if(kdb == NULL) return(-1);
//    if(K_pack[KTYPE(kdb)](&pack, a1, a2, a3, a4, a5, a6, a7, a8, a9) < 0)
//        return(-2);
//    pos = K_add_entry(kdb, name);
//    if(pos < 0) {
//        B_seterror("%.80s cannot be created (syntax ?)", name);
//        goto einde;
//    }
//
//    lg = * (OSIZE *) pack;
//    if(KSOVAL(kdb, pos) != 0) SW_free(KSOVAL(kdb, pos));
//    KSOVAL(kdb, pos) = SW_alloc(lg);
//    memcpy(KGOVAL(kdb, pos), pack, lg);
//einde:
//    SW_nfree(pack);
//    return(pos);
//}


// NEW VERSION: K_add() prototype is correct and corresponds to the implementation
// -------------------------------------------------------------------------------

int K_add(KDB* kdb, char* name, ...)
{
    va_list vargs;
    char    *pack = NULL;
    int     pos = -1, lg, rc;
    EQ*     eq;
    char*   endo;
    char*   txt;
    char*   lec;
    SCL*    scl;
    TBL*    tbl;
    double* var;
    int*    lgptr;

    va_start(vargs, name);

    if(kdb == NULL) {
        pos = -1;
        goto einde;
    }    

    switch(KTYPE(kdb)) {
      case COMMENTS: 
          txt = va_arg(vargs, char *);
          rc = K_cpack(&pack, txt);
          break;            
      case EQUATIONS: 
          eq = va_arg(vargs, EQ *);
          endo = va_arg(vargs, char *);
          rc = K_epack(&pack, (char*) eq, endo);
          break;            
      case IDENTITIES: 
          lec = va_arg(vargs, char *);
          rc = K_ipack(&pack, lec);
          break;            
      case LISTS: 
          txt = va_arg(vargs, char *);
          rc = K_lpack(&pack, txt);
          break;            
      case SCALARS: 
          scl = va_arg(vargs, SCL *);
          rc = K_spack(&pack, (char*) scl);
          break;            
      case TABLES: 
          tbl = va_arg(vargs, TBL *);
          rc = K_tpack(&pack, (char*)tbl);
          break;            
      case VARIABLES: 
          var = va_arg(vargs, double *);  
          lgptr = va_arg(vargs, int*);
          rc = K_vpack(&pack, var, lgptr);
          break;            
      case K_OBJ: 
          txt = va_arg(vargs, char*);     
          lgptr = va_arg(vargs, int*);
          rc = K_opack(&pack, txt, lgptr);
          break;            
     
    } 
    if(rc < 0) {
        pos = -2;
        goto einde;
    }    


    // Add entry (name) into kdb
    pos = K_add_entry(kdb, name);
    if(pos < 0) {
        B_seterror("%.80s cannot be created (syntax ?)", name);
        goto einde;
    }

    // Copy allocated pack into kdb entry pos (SWAP memory)
    lg = * (OSIZE *) pack;
    if(KSOVAL(kdb, pos) != 0) SW_free(KSOVAL(kdb, pos));
    KSOVAL(kdb, pos) = SW_alloc(lg);
    memcpy(KGOVAL(kdb, pos), pack, lg);
    
einde:
    // Frees the allocated pack in regular MEM
    SW_nfree(pack);
    va_end(vargs);
    
    return(pos);
}

/**
 * @header4iode
 *
 * Add an object to a KDB of any type
 * ----------------------------------
 *     bool K_add(KDB* kdb, char* name, char* a1, char* a2, char* a3, char* a4, char* a5, char* a6, char* a7, char* a8, char* a9):
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
 *  It returns true on success, false on error.
 *  
 *  How to create IODE objects with K_add()
 *  ---------------------------------------
 *    - Comments    K_add(KDB* kdb, char* name, CMT cmt)
 *    - Equations   K_add(KDB* kdb, char* name, Equation* eq, char* endo) [where endo = name]
 *    - Identities  K_add(KDB* kdb, char* name, char* lec)
 *    - Lists       K_add(KDB* kdb, char* name, char* list)
 *    - Scalars     K_add(KDB* kdb, char* name, Scalar* scalar)
 *    - Tables      K_add(KDB* kdb, char* name, Table *tbl) 
 *    - Variables   K_add(KDB* kdb, char* name, double* var, int* nb_obs) [nb_obs = kdb Sample size]
 *  
 *  @note: the name of an equation MUST be the name of its endogenous variable
 *  
 *  Example
 *  -------
 *      Equation*     eq;
 *      char*   name;
 *      ...
 *      success = K_add(K_WS[EQUATIONS], name, eq, name);
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


// NEW VERSION: K_add() prototype is correct and corresponds to the implementation
// -------------------------------------------------------------------------------

bool K_add(KDB* kdb, char* c_name, ...)
{
    va_list   vargs;
    char*     pack = NULL;
    int       rc;
    bool      success;
    Equation* eq;
    char*     endo;
    char*     txt;
    char*     lec;
    Scalar*   scl;
    Table*    tbl;
    double*   var;
    int*      lgptr;
    int       lg;
    SWHDL     handle;
    char*     ptr_swap;
    std::string name;

    va_start(vargs, c_name);

    if(kdb == NULL) 
    {
        success = false;
        goto einde;
    }    

    switch(kdb->k_type) {
      case COMMENTS: 
          txt = va_arg(vargs, char*);
          rc = K_cpack(&pack, txt);
          break;            
      case EQUATIONS: 
          eq = va_arg(vargs, Equation*);
          endo = va_arg(vargs, char *);
          rc = K_epack(&pack, (char*) eq, endo);
          break;            
      case IDENTITIES: 
          lec = va_arg(vargs, char*);
          rc = K_ipack(&pack, lec);
          break;            
      case LISTS: 
          txt = va_arg(vargs, char*);
          rc = K_lpack(&pack, txt);
          break;            
      case SCALARS: 
          scl = va_arg(vargs, Scalar*);
          rc = K_spack(&pack, (char*) scl);
          break;            
      case TABLES: 
          tbl = va_arg(vargs, Table*);
          rc = K_tpack(&pack, (char*) tbl, c_name);
          break;            
      case VARIABLES: 
          var = va_arg(vargs, double*);  
          lgptr = va_arg(vargs, int*);
          rc = K_vpack(&pack, var, lgptr);
          break;            
      case OBJECTS: 
          txt = va_arg(vargs, char*);     
          lgptr = va_arg(vargs, int*);
          rc = K_opack(&pack, txt, lgptr);
          break;
    }
    
    if(rc < 0) 
    {
        success = false;
        goto einde;
    }
    
    name = std::string(c_name);

    // Add an entry (name) if not present yet
    success = kdb->add_entry(name);
    if(!success) 
    {
        error_manager.append_error(v_iode_types[kdb->k_type] + " " + name + 
                                   " cannot be created (syntax ?)");
        goto einde;
    }
    
    // frees existing object but keeps the entry in kdb
    handle = kdb->get_handle(name);
    if(handle > 0) 
        SW_free(handle);

    // allocate space in SCR SWAP memory 
    // NOTE: if handle > 0 (object previously present in the KDB), 
    //       SW_alloc() will return the same handle as SW_alloc()
    //       first searches for handle associated with freed item 
    //       and reallocates it
    lg = * (OSIZE *) pack;
    handle = SW_alloc(lg);
    if(handle == 0) 
    {
        error_manager.append_error("Low memory: cannot allocate " + std::to_string(lg) + 
                                   " bytes for " + v_iode_types[kdb->k_type] + " " + std::string(name));
        success = false;
        goto einde;
    }
    kdb->k_objs[name] = handle;

    // Copy allocated pack into the SCR SWAP memory
    ptr_swap = SW_getptr(handle);
    memcpy(ptr_swap, pack, lg);

    success = true;
    
einde:
    // Frees the allocated pack in regular MEM
    SW_nfree(pack);
    va_end(vargs);
    
    return success;
}

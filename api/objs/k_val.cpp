/**
 *  @header4iode
 *
 *  Basic functions to retrieve object data based on their position or name in the kdb.
 *  If the object is packed (EQ, Table...) the position (n) of the element in the pack must be given.
 *  
 *  List of functions 
 *  -----------------
 *    Equation* K_eptr(KDB* kdb, char* name)
 *    Table* K_tptr(KDB* kdb, char* name)
 */
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/equations.h"
#include "api/objs/tables.h"


/**
 *  Retrieves an EQ in a kdb based on its name. Allocates a new EQ with its content.  
 *  
 *  @param [in] kdb     KDB*    KDB of equations
 *  @param [in] name    int     equation object name
 *  @return             Equation*     pointer to an allocated EQ ~ kdb[name]
 *  
 */
Equation* K_eptr(KDBEquations* kdb, char* c_name)
{    
    char* ptr = kdb->get_ptr_obj(std::string(name));
    return K_eunpack(ptr, name);
}

/**
 *  Retrieves a Table in a kdb based on its name. Allocates a new Table with its content.  
 *  
 *  @param [in] kdb     KDB*    KDB of tables
 *  @param [in] name    int     table object name
 *  @return             Equation*     pointer to an allocated Table ~ kdb[name]
 *  
 */
Table* K_tptr(KDB* kdb, char* name)
{        
    char* ptr = kdb->get_ptr_obj(std::string(name));
    return K_tunpack(ptr, name);
}

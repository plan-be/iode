/**
 *  @header4iode
 *
 *  Basic functions to retrieve object data based on their position or name in the kdb.
 *  If the object is packed (EQ, Table...) the position (n) of the element in the pack must be given.
 *  
 *  List of functions 
 *  -----------------
 *    char *K_oval(KDB* kdb, int pos, int n)        ~ kdb[pos][n]
 *    char* K_optr(KDB *kdb, char* name, int n)     ~ kdb[name][n]
 *    char *K_oval0(KDB* kdb, int pos)              ~ kdb[pos][0]
 *    char* K_optr0(KDB *kdb, char* name)           ~ kdb[name][0]
 *    char *K_oval1(KDB* kdb, int pos)              ~ kdb[pos][1]
 *    char* K_optr1(KDB *kdb, char* name)           ~ kdb[name][1]
 *    double *K_vval(KDB* kdb, int pos, int t)      ~ kdb[pos][t]
 *    double *K_vptr(KDB* kdb, char* name, int t)   ~ kdb[name][t]
 *    Equation* K_eptr(KDB* kdb, char* name)        ~ kdb[name]
 *    Table* K_tptr(KDB* kdb, char* name)           ~ kdb[name] 
 */
#include "api/objs/kdb.h"
#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/equations.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"


/**
 *  Retrieves the nth element of a packed object based on its position in kdb. 
 *  
 *  @param [in] kdb     KDB*    KDB of any type
 *  @param [in] pos     int     object position in kdb
 *  @param [in] n       int     element position in the pack
 *  @return             char*   pointer to the nth element 
 *  
 */
char *K_oval(KDB* kdb, int pos, int n)
{
    std::string name = kdb->get_name(pos);
    if(name.empty())
        return NULL;
    
    SWHDL handle = kdb->get_handle(name);
    if(handle == 0) 
        return NULL;
    
    return (char*) P_get_ptr(SW_getptr(handle), n);
}


/**
 *  Retrieves the nth element of a packed object based on its **name**. 
 *  
 *  @param [in] kdb     KDB*    KDB of any type
 *  @param [in] name    int     object name
 *  @param [in] n       int     element position in the pack
 *  @return             char*   pointer to the nth element or NULL if not found
 *  
 */
char* K_optr(KDB *kdb, char* c_name, int n)
{    
    SWHDL handle = kdb->get_handle(std::string(c_name));
    if(handle == 0) 
        return NULL;

    return (char*) P_get_ptr(SW_getptr(handle), n);
}    

/**
 *  Retrieves the 1st element of a packed object based on its position in kdb. 
 *  
 *  @param [in] kdb     KDB*    KDB of any type
 *  @param [in] pos     int     object position in kdb
 *  @return             char*   pointer to the 1st element 
 *  
 */
char *K_oval0(KDB* kdb, int pos)
{
    return K_oval(kdb, pos, 0);
}


/**
 *  Retrieves the 1st element of a packed object based on its name. 
 *  
 *  @param [in] kdb     KDB*    KDB of any type
 *  @param [in] name    int     object name
 *  @return             char*   pointer to the 1st element or NULL if not found.
 *  
 */
char* K_optr0(KDB *kdb, char* name)
{
    return K_optr(kdb, name, 0);
}    


/**
 *  Retrieves the 2d element of a packed object based on its position in kdb. 
 *  
 *  @param [in] kdb     KDB*    KDB of any type
 *  @param [in] pos     int     object position in kdb
 *  @return             char*   pointer to the 2d element 
 *  
 */
char *K_oval1(KDB* kdb, int pos)
{
    return K_oval(kdb, pos, 1);
}


/**
 *  Retrieves the 2st element of a packed object based on its name. 
 *  
 *  @param [in] kdb     KDB*    KDB of any type
 *  @param [in] name    int     object name
 *  @return             char*   pointer to the 2d element or NULL if not found.
 *  
 */
char* K_optr1(KDB *kdb, char* name)
{
    return K_optr(kdb, name, 1);
}

/**
 *  Retrieves the pointer to a variable in a kdb based on its position and then shifts the pointer t elements to the right.
 *  
 *  @param [in] kdb     KDB*         KDB of any type
 *  @param [in] pos     int          object position in kdb
 *  @param [in] t       int          shift in the variable
 *  @return             double*   pointer to "kdb[pos][t]"
 *  
 */
double *K_vval(KDB* kdb, int pos, int t)
{
    double* var = (double*) K_oval(kdb, pos, 0);
    if(var == NULL)
        return NULL;
    return var + t;
}


/**
 *  Retrieves the pointer to a variable in a kdb based on its name and then shifts the pointer t elements to the right.
 *  
 *  @param [in] kdb     KDB*         KDB of any type
 *  @param [in] name    int          object name
 *  @param [in] t       int          shift in the variable
 *  @return             double*   pointer to "kdb[name][t]"
 *  
 */
double *K_vptr(KDB* kdb, char* name, int t)
{
    return ((double*) K_optr(kdb, name, 0)) + t;
}


/**
 *  Retrieves an EQ in a kdb based on its name. Allocates a new EQ with its content.  
 *  
 *  @param [in] kdb     KDB*    KDB of equations
 *  @param [in] name    int     equation object name
 *  @return             Equation*     pointer to an allocated EQ ~ kdb[name]
 *  
 */
Equation* K_eptr(CKDBEquations* kdb, char* c_name)
{
    std::string name = std::string(c_name);
    if(!kdb->contains(name)) 
        return nullptr;
    
    return KEVAL(kdb, name);
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

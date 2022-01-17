/**
 *  @header4iode
 *
 *  Basic functions to retrieve object data based on their position or name in the kdb.
 *  If the object is packed (EQ, TBL...) the position (n) of the element in the pack must be given.
 *  
 *  List of functions 
 *  -----------------
 *    char *K_oval(KDB* kdb, int pos, int n)           ~ kdb[pos][n]
 *    char* K_optr(KDB *kdb, char* name, int n)        ~ kdb[name][n]
 *    char *K_oval0(KDB* kdb, int pos)                 ~ kdb[pos][0]
 *    char* K_optr0(KDB *kdb, char* name)              ~ kdb[name][0]
 *    char *K_oval1(KDB* kdb, int pos)                 ~ kdb[pos][1]
 *    char* K_optr1(KDB *kdb, char* name)              ~ kdb[name][1]
 *    IODE_REAL *K_vval(KDB* kdb, int pos, int t)      ~ kdb[pos][t]
 *    IODE_REAL *K_vptr(KDB* kdb, char* name, int t)   ~ kdb[name][t]
 *    EQ* K_eptr(KDB* kdb, char* name)                 ~ kdb[name]
 *  
 *  See also defines in iode.h.
 */

#include "iode.h"

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
    return(P_get_ptr(SW_getptr(kdb->k_objs[pos].o_val), n));
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
char* K_optr(KDB *kdb, char* name, int n)
{
    int pos;
    
    pos = K_find(kdb, name);
    if(pos < 0) return(NULL);       // name not found
    return(K_oval(kdb, pos, n));
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
    return(K_oval(kdb, pos, 0));
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
    int pos;
    
    pos = K_find(kdb, name);
    if(pos < 0) return(NULL);       // name not found
    return(K_oval0(kdb, pos));
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
    return(K_oval(kdb, pos, 1));
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
    int pos;
    
    pos = K_find(kdb, name);
    if(pos < 0) return(NULL);       // name not found
    return(K_oval1(kdb, pos));
}    


/**
 *  Retrieves the pointer to a variable in a kdb based on its position and then shifts the pointer t elements to the right.
 *  
 *  @param [in] kdb     KDB*         KDB of any type
 *  @param [in] pos     int          object position in kdb
 *  @param [in] t       int          shift in the variable
 *  @return             IODE_REAL*   pointer to "kdb[pos][t]"
 *  
 */
IODE_REAL *K_vval(KDB* kdb, int pos, int t)
{
    return(((IODE_REAL *)K_oval(kdb, pos, 0)) + t);
}


/**
 *  Retrieves the pointer to a variable in a kdb based on its name and then shifts the pointer t elements to the right.
 *  
 *  @param [in] kdb     KDB*         KDB of any type
 *  @param [in] name    int          object name
 *  @param [in] t       int          shift in the variable
 *  @return             IODE_REAL*   pointer to "kdb[name][t]"
 *  
 */
IODE_REAL *K_vptr(KDB* kdb, char* name, int t)
{
    int pos;
    
    pos = K_find(kdb, name);
    if(pos < 0) return(NULL);         // name not found
    return(K_vval(kdb, pos, t));
}


/**
 *  Retrieves an EQ in a kdb based on its name. Allocates a new EQ with its content.  
 *  
 *  @param [in] kdb     KDB*    KDB of equations
 *  @param [in] name    int     equation object name
 *  @return             EQ*     pointer to an allocated EQ ~ kdb[name]
 *  
 */
EQ* K_eptr(KDB* kdb, char* name)
{
    int pos;
    
    pos = K_find(kdb, name);
    if(pos < 0) return(NULL);         // name not found
    return(K_eunpack(SW_getptr(kdb->k_objs[pos].o_val)));
}


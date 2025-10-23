/**
 * @header4iode
 *
 * Functions implementing the interface between the LEC functions and the IODE KDB's.
 *  
 *      double *L_getvar(KDB* kdb, int pos)  Retrieves a pointer to the first element of a VAR.
 *      double L_getscl(KDB* kdb, int pos)   Retrieves a scalar value.
 *      Sample *L_getsmpl(KDB* kdb)             Retrieves the sample of a KDB.
 *      int L_findscl(KDB* kdb, char *name)     Retrieves a scalar position.
 *      int L_findvar(KDB* kdb, char* name)     Retrieves a variable position.
 */

#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"


/**
 *  Implementation of L_getvar() in the context of IODE objects. Retrieves a pointer to the first element of a VAR.
 *  
 *  @param [in] kdb     KDB*        KDB of variables from which the variable value has to be retrieved
 *  @param [in] pos     int         pos of the var in kdb
 *  @return             IODE_READ   pointer to the first value of the variable
 *  
 */
double *L_getvar(KDB* kdb, int pos)
{
    return(KVVAL(kdb, pos, 0));
}


/**
 *  Implementation of L_getscl() in the context of IODE objects. Retrieves a scalar value.
 *  
 *  @param [in] kdb     KDB*        KDB of Scalar from which the scalar value has to be retrieved
 *  @param [in] pos     int         pos of the Scalar in kdb
 *  @return             IODE_READ   value of the scalar
 *  
 */
double L_getscl(KDB* kdb, int pos)
{
    Scalar *scl;

    scl = KSVAL(kdb, pos);
    return(scl->value);
}


/**
 *  Implementation of L_getsmpl() in the context of IODE objects. 
 *  Retrieves the pointer to the sample a the KDB.
 *  
 *  @param [in] kdb     KDB*    KDB of VAR from which the Period has to be retrieved
 *  @return             Sample* pointer to the Sample struct (not allocated)
 *  
 */
Sample *L_getsmpl(KDB* kdb)
{
    return(kdb->sample);
}


/**
 *  Implementation of L_findscl() in the context of IODE objects. Retrieves a scalar position.
 *  
 *  @param [in] kdb     KDB*        KDB of Scalar from which the scalar postion has to be retrieved
 *  @param [in] name    char*       name of the scalar
 *  @return             int         position of name in KDB 
 *  
 */
int L_findscl(KDB* kdb, char *name)
{
    return(kdb->find(name));
}


/**
 *  Implementation of L_findvar() in the context of IODE objects. Retrieves a variable position.
 *  
 *  @param [in] kdb     KDB*        KDB of VAR from which the position has to be retrieved
 *  @param [in] name    char*       name of the VAR
 *  @return             int         position of name in KDB 
 *  
 */
int L_findvar(KDB* kdb, char* name)
{
    return(kdb->find(name));
}

/**
 *  Implentation of L_expand()
 *  
 *  @param [in] name 
 *  @return 
 *  
 */
 
char* L_expand(char* list_name)
{
    if(L_expand_super) 
        return((*L_expand_super)(list_name));
    else {
        int     pos;

        pos = KL_WS->find(list_name);
        //printf("pos=%d\n");
        if (pos < 0) return(NULL);
        return((char *)KLVAL(KL_WS, pos));
    }    
}

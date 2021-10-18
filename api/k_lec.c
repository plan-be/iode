/*
 * @author Jean-Marc PAUL
 * @author Geert BRYON
 *
 * Functions implementing the interface between the LEC library and the IODE objects.
 *  
 *      IODE_REAL *L_getvar(KDB* kdb, int pos)
 *      IODE_REAL L_getscl(KDB* kdb, int pos)
 *      SAMPLE *L_getsmpl(JDB* kdb)
 *      int L_findscl(KDB* kdb, char *name)
 *      int L_findvar(KDB* kdb, char* name)
 */

#include "iode.h"


/**
 *  Implementation of L_getvar() in the context of IODE objects. Retrieves a pointer to the first element of a VAR.
 *  
 *  @param [in] kdb     KDB*        KDB of variables from which the variable value has to be retrieved
 *  @param [in] pos     int         pos of the var in kdb
 *  @return             IODE_READ   pointer to the first value of the variable
 *  
 */
IODE_REAL    *L_getvar(KDB* kdb, int pos)
{
    return(KVVAL(kdb, pos, 0));
}


/**
 *  Implementation of L_getscl() in the context of IODE objects. Retrieves a scalar value.
 *  
 *  @param [in] kdb     KDB*        KDB of SCL from which the scalar value has to be retrieved
 *  @param [in] pos     int         pos of the SCL in kdb
 *  @return             IODE_READ   value of the scalar
 *  
 */
IODE_REAL L_getscl(KDB* kdb, int pos)
{
    SCL *scl;

    scl = KSVAL(kdb, pos);
    return(scl->val);
}


/**
 *  Implementation of L_getsmpl() in the context of IODE objects. 
 *  Retrieves the pointer to the sample a the KDB.
 *  
 *  @param [in] kdb     KDB*    KDB of VAR from which the PERIOD has to be retrieved
 *  @return             SAMPLE* pointer to the SAMPLE struct (not allocated)
 *  
 */
SAMPLE *L_getsmpl(KDB* kdb)
{
    return((SAMPLE *) KDATA(kdb));
}


/**
 *  Implementation of L_findscl() in the context of IODE objects. Retrieves a scalar position.
 *  
 *  @param [in] kdb     KDB*        KDB of SCL from which the scalar postion has to be retrieved
 *  @param [in] name    char*       name of the scalar
 *  @return             int         position of name in KDB 
 *  
 */
int L_findscl(KDB* kdb, char *name)
{
    return(K_find(kdb, name));
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
    return(K_find(kdb, name));
}

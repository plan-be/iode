/**
 * @header4iode
 *
 * Functions implementing the interface between the LEC functions and the IODE KDB's.
 *  
 *      double *L_getvar(CKDBVariables* kdb, int pos)  Retrieves a pointer to the first element of a VAR.
 *      double L_getscl(CKDBScalars* kdb, int pos)   Retrieves a scalar value.
 *      Sample *L_getsmpl(KDB* kdb)             Retrieves the sample of a KDB.
 *      int L_findscl(CKDBScalars* kdb, char *name)     Retrieves a scalar position.
 *      int L_findvar(CKDBVariables* kdb, char* name)     Retrieves a variable position.
 */

#include "api/lec/lec.h"
#include "api/objs/objs.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/variables.h"
#include "api/print/print.h"

#include <algorithm>    // for std::max

/**
 *  Implementation of L_getvar() in the context of IODE objects. Retrieves a pointer to the first element of a VAR.
 *  
 *  @param [in] kdb     KDB*        KDB of variables from which the variable value has to be retrieved
 *  @param [in] pos     int         pos of the var in kdb
 *  @return             IODE_READ   pointer to the first value of the variable
 *  
 */
double* L_getvar(CKDBVariables* kdb, int pos)
{
    std::string name = kdb->get_name(pos);
    return KVVAL(kdb, name, 0);
}


/**
 *  Implementation of L_getscl() in the context of IODE objects. Retrieves a scalar value.
 *  
 *  @param [in] kdb     KDB*        KDB of Scalar from which the scalar value has to be retrieved
 *  @param [in] pos     int         pos of the Scalar in kdb
 *  @return             IODE_READ   value of the scalar
 *  
 */
double L_getscl(CKDBScalars* kdb, int pos)
{
    std::string name = kdb->get_name(pos);
    Scalar* scl = kdb->get_obj(name);
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
Sample *L_getsmpl(CKDBVariables* kdb)
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
int L_findscl(CKDBScalars* kdb, char *name)
{
    return kdb->index_of(name);
}


/**
 *  Implementation of L_findvar() in the context of IODE objects. Retrieves a variable position.
 *  
 *  @param [in] kdb     KDB*        KDB of VAR from which the position has to be retrieved
 *  @param [in] name    char*       name of the VAR
 *  @return             int         position of name in KDB 
 *  
 */
int L_findvar(CKDBVariables* kdb, char* name)
{
    return kdb->index_of(name);
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
    else 
    {
        if(!global_ws_lst->contains(list_name)) 
            return NULL;
        return global_ws_lst->get_obj(list_name);
    }    
}

/**
 *  Print a LEC expression. Set the engogenous (name) in bold.
 *  
 *  @param [in] name   char* name    endogenous name   
 *  @param [in] eqlec  char* eqlec   LEC expression 
 *  @param [in] eqclec CLEC* eqclec  CLEC (compiled LEC) equivalent to LEC
 *  @param [in] coefs  int   coefs   if 1: replace scalars by their value   
 *                                   if 2: replace scalars by their value and their t-test   
 *  @return            bool
 */
bool print_lec_definition(const std::string& name, const std::string& eqlec, 
    CLEC* eqclec, int coefs)
{
    // create a char* array containing a copy of the string eqlec
    int lg = (int) eqlec.size();
    lg = std::max(512, 4 * lg);
    char* c_lec = new char[lg];
    strcpy(c_lec, eqlec.c_str());
    
    // create a copy of the CLEC* eqclec
    CLEC* clec = (CLEC*) SCR_malloc(eqclec->tot_lg + 1);
    memcpy(clec, eqclec, eqclec->tot_lg);

    char buf[80];
    sprintf(buf, "%cb%s%cB", A2M_ESCCH, name.c_str(), A2M_ESCCH);
    SCR_replace_gnl((unsigned char*) c_lec, (unsigned char*) name.c_str(), 
                    (unsigned char*) buf, (unsigned char*) "_\\");
    
    std::string sname;
    Scalar* scl = nullptr;
    char tcoef[128], ttest[128];
    for(int j = 0 ; j < clec->nb_names ; j++) 
    {
        sname = std::string(clec->lnames[j].name);
        buf[0] = 0;
        if(coefs && is_coefficient(sname)) 
        {
            if(global_ws_scl->contains(sname)) 
            {
                scl = global_ws_scl->get_obj(sname);
                T_fmt_val(tcoef, scl->value, 15, K_NBDEC);
                T_fmt_val(ttest, scl->calculate_t_test(), 15, K_NBDEC);
                if(coefs == 1) 
                    sprintf(buf, "%ci%s%cI", A2M_ESCCH, tcoef, A2M_ESCCH);
                if(coefs == 2) 
                    sprintf(buf, "%ci%s(%s)%cI", A2M_ESCCH, tcoef, ttest, A2M_ESCCH);
            }
        }
        if(buf[0] == 0) 
            sprintf(buf, "%ci%s%cI", A2M_ESCCH, sname.c_str(), A2M_ESCCH);
        SCR_replace_gnl((unsigned char*) c_lec, (unsigned char*) sname.c_str(), 
                        (unsigned char*) buf, (unsigned char*) "_\\");
    }

    dump_string((char*) " ", c_lec);

    delete[] c_lec;
    SCR_free(clec);
    return true;
}

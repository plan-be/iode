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
 *    double *K_vval(KDB* kdb, int pos, int t)      ~ kdb[pos][t]
 *    double *K_vptr(KDB* kdb, char* name, int t)   ~ kdb[name][t]
 *    EQ* K_eptr(KDB* kdb, char* name)                 ~ kdb[name]
 *    TBL* K_tptr(KDB* kdb, char* name)                ~ kdb[name] 
 *  
 *    double K_etest(KDB* kdb, char*name, int test_nb)  Retrieves a statistical test stored the equation whose endo is name.
 *    double K_e_stdev (KDB* kdb, char*name)            Returns stdev calculated during the last estimation of equation name
 *    double K_e_meany (KDB* kdb, char*name)            Returns meany calculated during the last estimation of equation name       
 *    double K_e_ssres (KDB* kdb, char*name)            Returns ssres calculated during the last estimation of equation name
 *    double K_e_stderr(KDB* kdb, char*name)            Returns stderr calculated during the last estimation of equation name
 *    double K_e_fstat (KDB* kdb, char*name)            Returns fstat calculated during the last estimation of equation name
 *    double K_e_r2    (KDB* kdb, char*name)            Returns r2    calculated during the last estimation of equation name
 *    double K_e_r2adj (KDB* kdb, char*name)            Returns r2adj calculated during the last estimation of equation name
 *    double K_e_dw    (KDB* kdb, char*name)            Returns dw    calculated during the last estimation of equation name
 *    double K_e_loglik(KDB* kdb, char*name)            Returns loglik calculated during the last estimation of equation name
 *
 *    double K_s_get_info(KDB* kdb, char*name, int info_nb)             Retrieves a SCL info
 *    double K_s_get_value (KDB* kdb, char*name)                        Retrieves a SCL value
 *    double K_s_get_relax (KDB* kdb, char*name)                        Retrieves a SCL relax
 *    double K_s_get_stderr(KDB* kdb, char*name)                        Retrieves a SCL stderr
 *    double K_s_get_ttest (KDB* kdb, char*name)                        Retrieves a SCL ttest
 *  
 *    int K_s_set_info(KDB* kdb, char*name, int info_nb, double val)    Sets a SCL info
 *    int K_s_set_value (KDB* kdb, char*name, double val)               Sets a SCL value
 *    int K_s_set_relax (KDB* kdb, char*name, double val)               Sets a SCL relax
 *    int K_s_set_stderr(KDB* kdb, char*name, double val)               Sets a SCL stderr

 *  See also defines in iode.h.
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
 *  @return             double*   pointer to "kdb[pos][t]"
 *  
 */
double *K_vval(KDB* kdb, int pos, int t)
{
    return(((double *)K_oval(kdb, pos, 0)) + t);
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
    return KEVAL(kdb, pos);
}

/**
 *  Retrieves a TBL in a kdb based on its name. Allocates a new TBL with its content.  
 *  
 *  @param [in] kdb     KDB*    KDB of tables
 *  @param [in] name    int     table object name
 *  @return             EQ*     pointer to an allocated TBL ~ kdb[name]
 *  
 */
TBL* K_tptr(KDB* kdb, char* name)
{
    int pos;
    
    pos = K_find(kdb, name);
    if(pos < 0) return(NULL);         // name not found
    return(K_tunpack(SW_getptr(kdb->k_objs[pos].o_val)));
}

/**
 *  Retrieves a statistical test stored the equation whose endo is name.
 *  
 *  @param [in] KDB*    kdb      KDB containing EQ name
 *  @param [in] char*   name     name of the equation (=endogenous)
 *  @param [in] int     test_nb  position of the test in eq->tests 
 *  @return     double           test value or IODE_NAN if equation name not found
 *  
 */
double K_etest(KDB* kdb, char*name, int test_nb)
{
    int     pos;
    float   *tests;
    
    pos = K_find(kdb, name);
    if(pos < 0) return(IODE_NAN);         // name not found
    
    tests = KETESTS(kdb, pos);
    return((double) tests[test_nb]);
}

// Returns test calculated during the last estimation of equation name
double K_e_stdev (KDB* kdb, char*name) {return(K_etest(kdb, name, 1));}
double K_e_meany (KDB* kdb, char*name) {return(K_etest(kdb, name, 2));}
double K_e_ssres (KDB* kdb, char*name) {return(K_etest(kdb, name, 3));}
double K_e_stderr(KDB* kdb, char*name) {return(K_etest(kdb, name, 4));}
double K_e_fstat (KDB* kdb, char*name) {return(K_etest(kdb, name, 6));}
double K_e_r2    (KDB* kdb, char*name) {return(K_etest(kdb, name, 7));}
double K_e_r2adj (KDB* kdb, char*name) {return(K_etest(kdb, name, 8));}
double K_e_dw    (KDB* kdb, char*name) {return(K_etest(kdb, name, 9));}
double K_e_loglik(KDB* kdb, char*name) {return(K_etest(kdb, name, 10));}


/**
 *  Retrieves a SCL info.
 *  
 *  @param [in] KDB*    kdb      KDB containing SCL name
 *  @param [in] char*   name     name of the scalar 
 *  @param [in] int     info_nb  0 = scalar, 1 = relax , 2 = std, 3: t-test
 *  @return     double           value or IODE_NAN if scalar name not found or t-test undefined
 *  
 */
double K_s_get_info(KDB* kdb, char*name, int info_nb)
{
    int     pos;
    SCL     *scl;
    double  val = IODE_NAN;
    
    pos = K_find(kdb, name);
    if(pos < 0) return(IODE_NAN);         // name not found
    
    scl = KSVAL(kdb, pos);
    switch(info_nb) {
        case 1 :  val = scl->relax; break;
        case 2 :  val = scl->std; break;
        case 3 :  
                if(IODE_IS_A_NUMBER(scl->val) && IODE_IS_A_NUMBER(scl->std) && !IODE_IS_0(scl->std))
                    val = scl->val / scl->std;
                else 
                    val = IODE_NAN;
                break;    
            
        default: val = scl->val; break;
    }    
    return(val);
}

double K_s_get_value (KDB* kdb, char*name) {return(K_s_get_info(kdb, name, 0));}
double K_s_get_relax (KDB* kdb, char*name) {return(K_s_get_info(kdb, name, 1));}
double K_s_get_stderr(KDB* kdb, char*name) {return(K_s_get_info(kdb, name, 2));}
double K_s_get_ttest (KDB* kdb, char*name) {return(K_s_get_info(kdb, name, 3));}


/**
 *  Sets a SCL info.
 *  
 *  @param [in] KDB*    kdb      KDB containing SCL name
 *  @param [in] char*   name     name of the scalar 
 *  @param [in] int     info_nb  0 = scalar, 1 = relax , 2 = std
 *  @param [in] double  val      value to set to info_nb
 *  @return     int              -1 if scalar not found, -2 if info_nb illegal, 0 otherwise
 */
int K_s_set_info(KDB* kdb, char*name, int info_nb, double val)
{
    int     pos;
    SCL     *scl;
    
    pos = K_find(kdb, name);
    if(pos < 0) return(-1);         // name not found
    
    scl = KSVAL(kdb, pos);
    switch(info_nb) {
        case 0 :  scl->val = val; break;
        case 1 :  scl->relax = val; break;
        case 2 :  scl->std = val; break;
        default:  return(-2);
    }    
    return(0);
}

int K_s_set_value (KDB* kdb, char*name, double val) {return(K_s_set_info(kdb, name, 0, val));}
int K_s_set_relax (KDB* kdb, char*name, double val) {return(K_s_set_info(kdb, name, 1, val));}
int K_s_set_stderr(KDB* kdb, char*name, double val) {return(K_s_set_info(kdb, name, 2, val));}


/**
 * @header4iode
 *
 * Function to compare 2 IODE objects.
 * -----------------------------------
 *
 *      int K_compare(char* name, KDB* kdb1, KDB* kdb2) : compare object named name in 2 kdb.
 */ 
#include <math.h>

#include "api/objs/objs.h"
#include "api/objs/pack.h"
#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"
#include "api/objs/compare.h"


/**
 *  Compares 2 double values. 
 *  
 *  The values are considered equal if their relative difference is less than K_COMPARE_EPS.
 *  
 *  @param [in] v1      double   first value
 *  @param [in] v2      double   second value
 *  @return             bool     true if |(v1 - v2)/v1| < K_COMPARE_EPS, false otherwise. 
 */
static bool K_cmpvar_1(double v1, double v2)
{
    double  diff;
    if(v1 == v2) 
        return true;
    
    if(v1 == 0) 
    {
        if(fabs(v2) < K_COMPARE_EPS) 
            return true;
        else 
            return false;
    }

    diff = fabs((v1 - v2) / v1);
    if(diff < K_COMPARE_EPS) 
        return true;
    return false;
}

bool K_compare_var(const Variable& var1, const Variable& var2)
{
    int nb_periods = global_ws_var->sample->nb_periods;
    for(int i = 0; i < nb_periods; i++)
        if(!K_cmpvar_1(var1[i], var2[i])) 
            return false;

    return true;
}

/**
 *  Compares IODE objects having the same name in two KDB. 
 *  Function used to compare an IODE file with the content of the current workspace.
 *  
 *  See above for more details on the comparison methods for each object type.
 *  
 *  @param [in] name    string      object name
 *  @param [in] kdb1    KDB*        first KDB
 *  @param [in] kdb2    KDB*        second KDB
 *  @return             int         0 -> if name neither in kdb1 nor in kdb2
 *                                  1 -> if name in kdb1 but not in kdb2
 *                                  2 -> if name not in kdb1 but in kdb2
 *                                  3 -> if name in both kdb1 and kdb2, IODE object in kdb1 == IODE object in kdb2
 *                                  4 -> if name in both kdb1 and kdb2, IODE object in kdb1 != IODE object in kdb2
 */
int K_compare(const std::string& name, KDB* kdb1, KDB* kdb2)
{
    if(!kdb1 || !kdb2) 
        return -1;

    if(kdb1->k_type != kdb2->k_type) 
        return -1;

    bool found_in_kdb1 = kdb1->contains(name);
    bool found_in_kdb2 = kdb2->contains(name);

    if(!found_in_kdb1 && !found_in_kdb2) 
        return 0;

    if(found_in_kdb1 && !found_in_kdb2) 
        return 1;

    if(!found_in_kdb1 && found_in_kdb2) 
        return 2;

    // ---- name in both kdb1 and kdb2 ----
    // res = 0 -> IODE object in kdb1 == IODE object in kdb2
    // res = 1 -> IODE object in kdb1 != IODE object in kdb2
    bool success = false;
    switch(kdb1->k_type)
    {
    case COMMENTS:
    {
        KDBComments* kdb_cmt1 = dynamic_cast<KDBComments*>(kdb1);
        KDBComments* kdb_cmt2 = dynamic_cast<KDBComments*>(kdb2);
        Comment* cmt1_ptr = kdb_cmt1->get_obj_ptr(name);
        Comment* cmt2_ptr = kdb_cmt2->get_obj_ptr(name);
        success = *cmt1_ptr == *cmt2_ptr;
        break;
    }
    case EQUATIONS:
    {
        KDBEquations* kdb_eqs1 = dynamic_cast<KDBEquations*>(kdb1);
        KDBEquations* kdb_eqs2 = dynamic_cast<KDBEquations*>(kdb2);
        Equation* eqs1_ptr = kdb_eqs1->get_obj_ptr(name);
        Equation* eqs2_ptr = kdb_eqs2->get_obj_ptr(name);
        success = *eqs1_ptr == *eqs2_ptr;
        break;
    }
    case IDENTITIES:
    {
        KDBIdentities* kdb_idt1 = dynamic_cast<KDBIdentities*>(kdb1);
        KDBIdentities* kdb_idt2 = dynamic_cast<KDBIdentities*>(kdb2);
        Identity* idt1_ptr = kdb_idt1->get_obj_ptr(name);
        Identity* idt2_ptr = kdb_idt2->get_obj_ptr(name);
        success = *idt1_ptr == *idt2_ptr;
        break;
    }
    case LISTS:
    {
        KDBLists* kdb_lst1 = dynamic_cast<KDBLists*>(kdb1);
        KDBLists* kdb_lst2 = dynamic_cast<KDBLists*>(kdb2);
        List* lst1_ptr = kdb_lst1->get_obj_ptr(name);
        List* lst2_ptr = kdb_lst2->get_obj_ptr(name);
        success = *lst1_ptr == *lst2_ptr;
        break;
    }
    case SCALARS:
    {
        KDBScalars* kdb_scl1 = dynamic_cast<KDBScalars*>(kdb1);
        KDBScalars* kdb_scl2 = dynamic_cast<KDBScalars*>(kdb2);
        Scalar* scl1_ptr = kdb_scl1->get_obj_ptr(name);
        Scalar* scl2_ptr = kdb_scl2->get_obj_ptr(name);
        success = *scl1_ptr == *scl2_ptr;
        break;
    }
    case TABLES:
    {
        KDBTables* kdb_tbl1 = dynamic_cast<KDBTables*>(kdb1);
        KDBTables* kdb_tbl2 = dynamic_cast<KDBTables*>(kdb2);
        Table* tbl1_ptr = kdb_tbl1->get_obj_ptr(name);
        Table* tbl2_ptr = kdb_tbl2->get_obj_ptr(name);
        success = *tbl1_ptr == *tbl2_ptr;
        break;
    }
    case VARIABLES:
    {
        KDBVariables* kdb_var1 = dynamic_cast<KDBVariables*>(kdb1);
        KDBVariables* kdb_var2 = dynamic_cast<KDBVariables*>(kdb2);
        Variable* var1_ptr = kdb_var1->get_obj_ptr(name);
        Variable* var2_ptr = kdb_var2->get_obj_ptr(name);
        success = K_compare_var(*var1_ptr, *var2_ptr);
        break;
    }
    // should not happen, but we never know. Better be safe than sorry
    default:
        success = false; 
        break;
    }

    // 3 -> if name in both kdb1 and kdb2 AND IODE object in kdb1 == IODE object in kdb2
    // 4 -> if name in both kdb1 and kdb2 AND IODE object in kdb1 != IODE object in kdb2
    return success ? 3 : 4;
}

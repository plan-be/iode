/**
 * @header4iode
 *
 * Functions to evaluate LEC "values". 
 * 
 * The table (*L_VAL_FN)[] contains the function pointers to evaluate L_PI to L_EURO (see iode.h).
 *
 * Function signatures:
 *
 *      double  <fnname>(int t);
 *  or
 *      double  <fnname>(void);
 *  
 *  List of functions
 *  -----------------
 *      double L_pi ()
 *      double L_euro()
 *      double L_e ()
 *      double L_time(int t)
 *      double L_i(int t)
 *     
 */

#include "api/lec/lec.h"


/*---------- L_VAL_FN ---------------*/

double L_pi(int t) 
{ 
    return (double) M_PI; 
}

double L_euro(int t) 
{ 
    return (double) 40.3399; 
}

double L_e(int t) 
{ 
    return (double) M_E; 
}

double L_time(int t) 
{ 
    return (double) t; 
}

double L_i(int t)    
{ 
    return (double) t - (double) L_curt; 
}

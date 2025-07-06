/**
 * @header4iode
 *
 * Functions to evaluate LEC "values". 
 * 
 * The table (*L_VAL_FN)[] contains the function pointers to evaluate L_PI to L_EURO (see iode.h).
 *
 * Function signatures:
 *
 *      L_REAL  <fnname>(int t);
 *  or
 *      L_REAL  <fnname>(void);
 *  
 *  List of functions
 *  -----------------
 *      L_REAL L_pi ()
 *      L_REAL L_euro()
 *      L_REAL L_e ()
 *      L_REAL L_time(int t)
 *      L_REAL L_i(int t)
 *     
 */

#include "api/lec/lec.h"


/*---------- L_VAL_FN ---------------*/

L_REAL L_pi  ()      {return((L_REAL)M_PI);}
L_REAL L_euro()      {return((L_REAL)40.3399);}
L_REAL L_e   ()      {return((L_REAL)M_E);}
L_REAL L_time(int t) {return((L_REAL)t);}

L_REAL L_i(int t) {
    extern int L_curt;
    return((L_REAL)t - (L_REAL)L_curt);
}

L_REAL (*L_VAL_FN[])() = { 
    L_pi,           // L_PI        L_VAL + 0
    L_e,            // L_E         L_VAL + 1
    L_time,         // L_TIME      L_VAL + 2
    L_i,            // L_I         L_VAL + 3
    L_euro          // L_EURO      L_VAL + 4
};

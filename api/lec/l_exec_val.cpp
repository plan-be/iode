#include "api/lec/lec.h"


/*---------- L_VAL_FN ---------------*/

double L_pi(int t) 
{ 
    return M_PI; 
}

double L_euro(int t) 
{ 
    return 40.3399; 
}

double L_e(int t) 
{ 
    return M_E; 
}

double L_time(int t) 
{ 
    return (double) t; 
}

double L_i(int t)    
{ 
    return (double) t - (double) L_curt; 
}

#include "api/lec/lec.h"


/*---------- L_VAL_FN ---------------*/

double L_pi(int t, int exec_t) 
{ 
    return M_PI; 
}

double L_euro(int t, int exec_t) 
{ 
    return 40.3399; 
}

double L_e(int t, int exec_t) 
{ 
    return M_E; 
}

double L_time(int t, int exec_t) 
{ 
    return (double) t; 
}

double L_i(int t, int exec_t)    
{ 
    return (double) t - (double) exec_t; 
}

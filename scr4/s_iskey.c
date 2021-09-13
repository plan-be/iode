#include "scr4_key.h"

/*NH*/
SCR_is_key(key)
int     key;
{
    if((key >= SCR_F1   && key <= SCR_F10)   ||
       (key >= SCR_C_F1 && key <= SCR_C_F10) ||
       (key >= SCR_A_F1 && key <= SCR_A_F10) ||
       (key >= SCR_A_A  && key <= SCR_A_Z)   ||
       (key >= SCR_C_A  && key <= SCR_C_Z)   ||
       (key >= SCR_S_F1 && key <= SCR_S_F10) ||
	key == SCR_C_CSR_HOME  || key == SCR_C_CSR_END ||
	key == SCR_CSR_HOME  || key == SCR_CSR_END ||
	key == SCR_CSR_PG_DN || key == SCR_CSR_PG_UP ||
	key == SCR_C_CSR_RIGHT || key == SCR_C_CSR_LEFT ||
	key == SCR_C_CSR_PG_DN || key == SCR_C_CSR_PG_UP)
	    return(1);
    return(0);
}


#include "scr4.h"

#ifndef DOS

/*NH*/
SCR_map_box_2()
{
    int     i;
    unsigned char *ptr;
    extern unsigned char SCR_MAP_BOX_12[];  /* JMP 19-02-96 */
    extern unsigned char SCR_MAP_BOX_11[];  /* JMP 19-02-96 */

    if(SCR_MAP_OUT == 0) return;

    for(i = 0 ; SCR_MAP_BOX_12[i] ; i++) {
	ptr = SCR_MAP_OUT[SCR_MAP_BOX_11[i]].yk_word;
	if(ptr == 0) {
	    ptr = SCR_malloc(2);
	    ptr[0] = SCR_MAP_BOX_11[i];
	    }
	SCR_MAP_OUT[SCR_MAP_BOX_12[i]].yk_word = ptr;
	}

    return(0);
}
#endif


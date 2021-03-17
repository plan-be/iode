#include "iode.h"


/*NH*/
/*
    Allocates lg bytes in conventional memory. Set buffer to null.
*/

char *L_malloc(lg)
int     lg;
{
//    extern char *SCR_malloc(); /* JMP 23-08-2012 */

    return(SCR_malloc(lg));
/*
    char    *ptr;
    ptr = malloc(lg);
    if(ptr == 0) {
	L_errno = L_MEMORY_ERR;
	fprintf(stderr, "%s\n", L_error(L_MEMORY_ERR));
	exit(1);
	return((char *)0);
	}
    memset(ptr, 0, lg);
    return(ptr);
*/
}
/*NH*/
/*
    Frees ptr if ptr != NULL.
*/

L_free(ptr)
char    *ptr;
{
    //    if(ptr != 0) free(ptr);
    if(ptr != 0) SCR_free(ptr); /* JMP 23-08-2012 */
    return(0);
}









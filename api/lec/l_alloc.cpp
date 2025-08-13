/**
 *  @header4iode
 * 
 *  Functions to allocate and free standard memory (on the "heap"). 
 *  These functions are separated to allow and easy adaption if required.
 *  
 *      - char *L_malloc(int lg)    Allocates lg bytes in conventional memory. Fills the allocated space with nulls.
 *      - void L_free(void *ptr)    Frees an allocated buffer. If ptr is null, does nothing.
 */

#include "api/lec/lec.h"


/**
 *  Allocates lg bytes of conventional memory. Fills the allocated space with nulls.
 *  
 *  @detail Calls SCR_malloc() and therefore exits if the memory is full.
 *  
 *  @param [in]     lg  int     number of bytes to allocate
 *  @return             char*   pointer to the allocated space
 *                              NULL on error
 *  
 */
char *L_malloc(int lg)
{
    return(SCR_malloc(lg));
}


/*
    Frees ptr if ptr != NULL.
*/

/**
 *  Frees an allocated buffer. If ptr is null, does nothing.
 *  
 *  @param [in] ptr     void*   NULL or pointer to an allocated buffer
 */
void L_free(void *ptr)
{
    if(ptr != 0) SCR_free(ptr); /* JMP 23-08-2012 */

}









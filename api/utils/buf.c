/**
 * @header4iode
 *
 * Group of functions for managing one global buffer to avoid multiple 
 * reallocations thus limiting memory fragmentation.
 *
 *   - char *BUF_alloc(int len) :  allocates or extends a global buffer of at least len bytes. 
 *   - void BUF_free() : free the buffer
 *   - void BUF_lock() : reserve the buffer utilisation
 *   - void BUF_unlock() : unlock the buffer
 *   - char *BUF_memcpy(char *ptr, int lg) : copy the the first lg bytes following address ptr to the buffer
 *   - char *BUF_strcpy(char *ptr) : copy a null terminated string to the buffer
 * 
 *   - char *BUF_DATA : NULL or pointer to the allocated buffer
 */
#include <s_swap.h>
#include <stdio.h>
#include "api/utils/buf.h"


char    *BUF_DATA = NULL;
static int     BUF_LEN = 0;
static int     BUF_LCK = 0;

/**
 * @brief Allocates or extends a global buffer of at least len bytes. 
 * 
 * @details 
 *   - The buffer can be locked to garantee that successive operations can be achieved without interference.
 *   - If the buffer is "locked", the program stops with a message on stdout.
 *   - The content of the buffer is not set to 0. Use memset() to fill it.
 * 
 * @param   len     int required buffer length 
 * @return          char * BUF_DATA : pointer to the allocated buffer 
 * 
 * TODO: throw an exception if allocation error instead of exiting.
 */
char *BUF_alloc(int len)
{
    if(BUF_LCK) {
        fprintf(stderr, "Internal BUFFER LOCKED (BUF_alloc)");
        exit(100);
    }

    if(BUF_LEN < len) {
        SW_nfree(BUF_DATA);
        BUF_DATA = SW_nalloc(len + 10240);
        BUF_LEN = len + 10240;
    }

    if(BUF_DATA == NULL) {
        fprintf(stderr, "Internal allocation error (BUF_alloc)");
        exit(100);
    }
    return(BUF_DATA); 
}


/**
 * @brief Frees the global buffer BUF_DATA.
 */
void BUF_free()
{
    SW_nfree(BUF_DATA);
    BUF_DATA = 0;
    BUF_LEN = 0;
}

/**
 * @brief Locks the global buffer BUF_DATA.
 */
void BUF_lock()
{
    BUF_LCK = 1;
}

/**
 * @brief Unlocks the global buffer BUF_DATA.
 */
void BUF_unlock()
{
    BUF_LCK = 0;
}

/**
 * @brief Extends the global buffer BUF_DATA to at least lg bytes and copies the lg first bytes of ptr to the global buffer.
 * 
 * @param ptr   char *  address of the buffer to copy
 * @param lg    int     nb of bytes to copy
 * @return      char *  pointer to the global buffer BUF_DATA
 *
 * @example 
 *
 *      BUF_memcpy("ABCD", 5);
 *      printf("%s\n", BUF_DATA); // prints ABCD
 */
char *BUF_memcpy(char *ptr, int lg)
{
    char    *buf;

    buf = BUF_alloc(lg);
    memcpy(buf, ptr, lg);
    return(buf);
}

/**
 * Copies a NULL terminated string  to the global buffer. Reallocates the buffer if needed.
 *  
 * @param ptr   char *  address of the buffer to copy
 * @return      char *  pointer to the global buffer BUF_DATA
 *
 *      
 */

char *BUF_strcpy(char *ptr)
{
    return(BUF_memcpy(ptr, (int)strlen(ptr) + 1));
}

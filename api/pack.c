/**
 * @author Jean-Marc PAUL
 * @author Geert BRYON
 *
 * Functions for "packing" (serialize) and "unpacking" (deserialize) objects.
 * 
 * A packed object is a binary string structured as described below.
 *
 * Requires 
 *    scr4iode.lib for the swap functions (sw_*).
 *    BUF_alloc()  
 * 
 * |total size of pack|# elements|el1 size|...|eln size|el1 value|pad to 4 bytes|...|eln value|
 * |long              |long      |long    |...|long    |bytes    |pad           |...|bytes    |
 *  
 *  - long (4 bytes) : total size of pack
 *  - long (4 bytes) : nb of elements in the pack
 *  - long (4 bytes) : length of element 1 = lg1
 *  - ...
 *  - long (4 bytes) : length of element n = lgn
 *  - blob (lg1 bytes) : element 1 value
 *  - bytes : padding to align element 2 to a 4 bytes boundary
 *  - ...
 *  - blob (lgn bytes) : element n value
 *  
 *  
 *  List of functions
 *  -----------------
 *      void *P_create() : Creates and initializes a "pack"
 *      int P_free(char *ptr) : Frees a pack.
 *      void *P_add(void *vptr1, void *vptr2, int lg) : Appends to the pack pointed to by ptr1 the content of ptr2 of length lg.
 *      void *P_get_ptr(void *vptr, int i) : Retrieves the pointer to the i'th element of a pack.
 *      OSIZE P_get_len(void *vptr, int i) : Retrieves the length of the i'th element of a pack or length of the full pack if i < 0.
 *      void *P_alloc_get_ptr(void *ptr, int p) : Allocates a new buffer and copies the p'th element of pack in the new buffer.
 *      int P_nb(char *ptr) : Retrieves the number of elements in a pack pointed to by ptr.
 */

#include "iode.h"

/**
 * @brief Creates and initializes a "pack".
 * 
 * The packs are allocated in the heap via a SW_nalloc().
 * 
 * @return char *   Pointer to the empty pack or NULL if allocation fails
*/
void *P_create()
{
    U_sh    *ptr;

    ptr = (U_sh *)SW_nalloc(2 * sizeof(OSIZE));     
    if(ptr == NULL) return(NULL);
    ptr[0] = 2 * sizeof(OSIZE);                     
    return(ptr);
}


/**
 * @brief Frees a pack.
 * 
 * @param ptr char * Pointer to a pack.
 * @return result of SW_nfree()
*/

int P_free(char *ptr)
{
    return(SW_nfree(ptr));
}


/**
 *  @brief Appends to the pack pointed to by ptr1 the content of ptr2 of length lg.
 *  
 *  @param [in] char * vptr1    pointer to a pack
 *  @param [in] char * vptr2    pointer to a buffer to add to the pack vptr1
 *  @param [in] int    lg       length of vptr2
 *  @return     char *          pointer to the newly allocated pack or NULL if memory allocation error 
 *  
 *  @note Uses BUF_alloc() to limit memory fragmentation.
 */

void *P_add(void *vptr1, void *vptr2, int lg)
{
    int     i, lg2, dif, n_p = 0;
    OSIZE   *pos, pos1;
    char    *ptr1 = vptr1, *ptr2 = vptr2;

    lg2 = lg + P_ALIGN - lg % P_ALIGN;
    /* pos = tot_lg, n, pos1, pos2, ...., posn, char1, char2 ... */
    pos = (OSIZE *) ptr1;
    if(pos[1] % 2 == 0)  n_p = 2 * sizeof(OSIZE);

    ptr1 = SW_nrealloc(ptr1,
                       (int) pos[0], (int) pos[0] + lg2 + n_p);
    pos = (OSIZE *) ptr1;

    if(pos[1] == 0) {
        pos = (OSIZE *) ptr1;
        pos[0] += lg2 + n_p;
        pos[1] = 1;
        pos[2] = 2 * sizeof(OSIZE) + n_p;
        if(lg != 0) {
            if(ptr2 != NULL) memcpy(ptr1 + pos[2], ptr2, lg);
            else memset(ptr1 + pos[2], 0, lg2);
        }
        return(ptr1);
    }

    dif = pos[0] - pos[2];
    BUF_alloc(dif + lg2);
    memcpy(BUF_DATA, ptr1 + pos[2], dif);
    if(lg != 0) {
        if(ptr2 != NULL) memcpy(BUF_DATA + dif, ptr2, lg);
        else memset(BUF_DATA + dif, 0, lg2);
    }

    for(i = 0; i < pos[1]; i++) pos[i + 2] += n_p;  /* old pos */
    pos[pos[1] + 2] = pos[0] + n_p;                 /* new pos */
    pos[0] += lg2 + n_p;                            /* new tot_lg */
    pos[1] ++;                                      /* new nb */
    memcpy(ptr1 + pos[2], BUF_DATA, pos[0] - pos[2]);
    return(ptr1);
}

/**
 * @brief Retrieves the pointer to the i'th element of a pack.
 * 
 * @param vptr  char *  Pointer to a pack. If null, the returned value is NULL.
 * @param i     int     element position in the pack (index 0)
 * @return      char *  pointer to the ith element or NULL if vptr is NULL or i > nb of elements in the pack
 */

void *P_get_ptr(void *vptr, int i)
{
    char    *ptr = vptr;
    OSIZE   *pos = (OSIZE *) ptr;


    if(ptr == 0) return(NULL);  /* JMP 08-09-99 */
    if(i < 0 || i > pos[1] - 1) return(NULL);
    return((char *) ptr + pos[i + 2]);
}


/**
 * @brief Retrieves the length of the i'th element of a pack or length of the full pack if i < 0.
 * 
 * @param vptr  char *  Pointer to a pack. If null, the returned value is NULL.
 * @param i     int     element position in the pack (index 0)
 * @return      OSIZE   length of the i'th element of a pack. Length of the full pack if i < 0. -1 if i > nb elements in the pack.
 */

OSIZE P_get_len(void *vptr, int i)
{
    OSIZE   len, *pos;
    char    *ptr = vptr;

    if(ptr == 0) return(0);
    pos = (OSIZE *) ptr;

    /* GB 02/98 */
    if(pos[0] <= 0) return(0);

    if(i < 0) return(pos[0]); /* i == -1 returns full lg */

    if(i > pos[1] - 1) return(-1);
    if(i == pos[1] - 1) len = pos[0] - pos[i + 2];
    else len = pos[i + 3] - pos[i + 2];

    return(len);
}


/**
 * @brief Allocates a new buffer and copies the p'th element of pack in the new buffer.
 * 
 * @param ptr   char *  Pointer to a pack. If null, the returned value is NULL.
 * @param p     int     element position in the pack (index 0)
 * @return      char *  pointer to an allocated buffer containing the p'th element or NULL if ptr is NULL or i > nb of elements in the pack
 */

void *P_alloc_get_ptr(void *ptr, int p)
{
    int     len;
    char    *val;

    len = P_get_len(ptr, p);
    if(len <= 0) return(NULL);              // JMP 6/10/2021
    val = SW_nalloc(len);
    memcpy(val, P_get_ptr(ptr, p), len);    // plus de pointeur
    return(val);
}

/**
 * @brief Retrieves the number of elements in a pack pointed to by ptr.
 * 
 * @param ptr   char *  Pointer to a pack. If null, the returned value is NULL.
 * @return      number of elements in the pack or 0 if ptr is NULL.
 */

int P_nb(char *ptr)
{
    OSIZE   *pos;

    if(ptr == 0) return(0);          // JMP 6/10/2021
    pos = (OSIZE *) ptr;
    return(pos[1]);
}

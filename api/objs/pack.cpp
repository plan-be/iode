/**
 * @header4iode
 *
 * Functions for "packing" (serializing) and "unpacking" (deserializing) objects.
 * 
 * A 'pack' is a binary string structured as described below. A pack represents for example
 * an IODE object (the C struct, e.g. an equation) as a string concatenating 
 * the struct elements and their positions in the pack. 
 * An IODE object is packed before being saved in a file  
 * and a pack in 'unpacked' into an IODE object when reread from the disk. 
 * 
 * Objects saved in memory (in "swap segments") are also packed / unpacked via these functions.
 *
 * Requires 
 *    iode_scr4.lib for the swap functions (sw_*).
 *    BUF_alloc()
 * 
 * Type (size)=> |long (4)            |long (4)  |long (4)|...|long (4)|blob (el1_size bytes)|pad           |...|blob (eln_size bytes)|
 * Content    => |total size of pack  |# elements|el1_pos |...|eln_pos |el1_value            |pad to 4 bytes|...|eln_value            |
 *  
 *  - long (4 bytes) : total size of pack in bytes
 *  - long (4 bytes) : nb of elements in the pack
 *  - long (4 bytes) : pos of element 1
 *  - ...
 *  - long (4 bytes) : pos of element n
 *  - [pad (4 bytes)] : if odd number of elements]
 *  
 *  - blob (lg1 bytes) : element 1 value
 *  - pad : padding to align element 2 to a 4 bytes boundary
 *  - ...
 *  - blob (lgn bytes) : element n value
 *  
 *  Example 1
 *  ---------
 *  Pack with 2 strings: "AAAAA" and "BB" 
 *      => |25|2|16|22|AAAAA|BB| 
 *  
 *  which means:
 *      25 bytes, 
 *      2 elements, 
 *      16 = pos of el1, 
 *      22 = pos of el2, 
 *      AAAAA = value of element1  (including ending 0), 
 *      BB = value of element2 (including ending 0)
 *
 *  Example 2
 *  ---------
 *  
 *  Pack with 3 strings: "AAAAAA" and "BB" and "C"
 *      => |32|3|24|29|31|00|AAAAA|BB|C|
 *  
 *  which means:
 *      35 bytes, 
 *      3 elements, 
 *      24 = pos of el1, 
 *      30 = pos of el2, 
 *      33 = pos of el3,
 *      00 = padding to align el1 on multiple of 8 bytes
 *      AAAAA = value of element1, 
 *      BB = value of element2
 *      C = value of element3
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
#include "api/constants.h"
#include "api/utils/buf.h"
#include "api/objs/pack.h"


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
 * @return result of SW_nfree(), which is more or less equivalent to free()
*/

int P_free(char *ptr)
{
    return(SW_nfree(ptr));
}


/**
 *  Appends to the pack pointed to by vptr1 the content pointed to by vptr2 which has a length of lg bytes. 
 *  The result is a pointer to the new pack. 
 *  
 *  Uses void* instead of char* because packs can point to any object (Equation*, TBL*...). 
 *  
 *  @param [in] void* vptr1    pointer to a pack
 *  @param [in] void* vptr2    pointer to a buffer to add to the pack vptr1
 *  @param [in] int    lg       length of vptr2
 *  @return     void*          pointer to the newly allocated pack or NULL if memory allocation error 
 *  
 *  @note Uses BUF_alloc() to limit memory fragmentation.
 */

void *P_add(void *vptr1, void *vptr2, int lg)
{
    int     i, lg2, dif, n_p = 0;
    OSIZE   *pos;
    char    *ptr1 = (char*) vptr1;
    char    *ptr2 = (char*) vptr2;

    // lg2 = real length that will be allocated to insure alignment on P_ALIGN (4) bytes 
    lg2 = lg + P_ALIGN - lg % P_ALIGN;
    
    /* pos = tot_lg, n, pos1, pos2, ...., posn, char1, char2 ... */
    pos = (OSIZE *) ptr1;
    
    // The number of elements must be even (alignment again ?)
    if(pos[1] % 2 == 0)  n_p = 2 * sizeof(OSIZE);

    // add lg2 + n_p to the buffer
    ptr1 = (char*) SW_nrealloc(ptr1, (int) pos[0], (int) pos[0] + lg2 + n_p);
    pos = (OSIZE *) ptr1; // new pointer to the first long 

    if(pos[1] == 0) { // No element in the pack
        pos = (OSIZE *) ptr1;
        pos[0] += lg2 + n_p;        // new len
        pos[1] = 1;                 //1 element
        pos[2] = 2 * sizeof(OSIZE) + n_p; // pos of element = 2 longs + 2 longs
        if(lg != 0) {
            if(ptr2 != NULL) memcpy(ptr1 + pos[2], ptr2, lg);
            else memset(ptr1 + pos[2], 0, lg2);
        }
        return(ptr1);
    }

    // At least one existing element before adding the new one
    dif = pos[0] - pos[2];  // current size of the data
    BUF_alloc(dif + lg2);   // Temp buffer for the old+new data
    memcpy(BUF_DATA, ptr1 + pos[2], dif); // copy old data into buffer
    if(lg != 0) {
        if(ptr2 != NULL) memcpy(BUF_DATA + dif, ptr2, lg);  // append new data to buffer
        else memset(BUF_DATA + dif, 0, lg2);                // or set 0s 
    }

    for(i = 0; i < pos[1]; i++) pos[i + 2] += n_p;  // shift old positions from n_p bytes */
    pos[pos[1] + 2] = pos[0] + n_p;                 // pos of the new element = old size + n_p (shift if odd number of elements)
    pos[0] += lg2 + n_p;                            // new tot_lg */
    pos[1] ++;                                      // new nb of elements */
    memcpy(ptr1 + pos[2], BUF_DATA, pos[0] - pos[2]); // copy buffer to pack
    return(ptr1);
}

/**
 * Retrieves the pointer to the i'th element of a pack.
 * 
 * @param vptr  char *  Pointer to a pack. If null, the returned value is NULL.
 * @param i     int     element position in the pack (index 0)
 * @return      char *  pointer to the ith element or NULL if vptr is NULL or i > nb of elements in the pack
 */

void *P_get_ptr(void *vptr, int i)
{
    char    *ptr = (char*) vptr;
    OSIZE   *pos = (OSIZE *) ptr;


    if(ptr == 0) return(NULL);  /* JMP 08-09-99 */
    if(i < 0 || i > pos[1] - 1) return(NULL);
    return((char *) ptr + pos[i + 2]);
}


/**
 * Retrieves the length (in bytes) of the i'th element of a pack or the length of the full pack if i < 0.
 * 
 * @param vptr  char *  Pointer to a pack. If null, the returned value is NULL.
 * @param i     int     element position in the pack (index 0)
 * @return      OSIZE   length of the i'th element of a pack. 
 *                      length of the full pack if i < 0. 
 *                      -1 if i > nb elements in the pack.
 */

OSIZE P_get_len(void *vptr, int i)
{
    OSIZE   len, *pos;
    char    *ptr = (char*) vptr;

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
 * Allocates space for the p'th element of a pack. Copies the pth element in the allocated space.
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
 * Retrieves the number of elements in the pack pointed to by ptr.
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

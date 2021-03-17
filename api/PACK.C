#include "iode.h"

/*
    Creates an empty pack of information.
    Returns NULL if allocation fails
    or the pointer to the created stack.
*/
void *P_create()
{
    U_sh    *ptr;

//    ptr = SW_nalloc(2 * sizeof(U_sh));
    ptr = (U_sh *)SW_nalloc(2 * sizeof(OSIZE)); /* JMP 23-05-00 */
    if(ptr == NULL) return(NULL);
//    ptr[0] = 2 * sizeof(U_sh);
    ptr[0] = 2 * sizeof(OSIZE); /* JMP 23-05-00 */
    return(ptr);
}

/*
    Frees a pack.
*/
P_free(ptr)
char   *ptr;
{
    return(SW_nfree(ptr));
}

/*
    Adds ptr2 of length lg to pack ptr1.
    Returns pointer to the new allocated pack
    or NULL if allocation error occurs.

PS : use the macros P_add_int(ptr, &i) and P_add_real(ptr, &r)
    to add integers or reals
*/
void *P_add(vptr1, vptr2, lg)
void    *vptr1, *vptr2;
int     lg;
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

/*
    Gets the pointer to the i'th element of the pack.
    Returns the pointer or NULL if i > element in the pack.

PS : copy the str in a local variable before processing or changing it.
    Use the macros P_get_int(ptr, i) and P_get_real(ptr, i) for the
    retrieval of integers and reals
*/
void *P_get_ptr(vptr, i)
void    *vptr;
int     i;
{
    char    *ptr = vptr;
    OSIZE   *pos = (OSIZE *) ptr;
    

    if(ptr == 0) return(NULL);  /* JMP 08-09-99 */
    if(i > pos[1] - 1) return(NULL);
    return((char *) ptr + pos[i + 2]);
}

/*
    Returns the length of the i'th element of the pack.
    If i < 0 the total length of the pack is returned.
    (P_len(ptr) is defined as P_get_len(ptr, -1)).
    Returns -1 if i > number of elements.
*/

OSIZE P_get_len(vptr, i)
void    *vptr;
int     i;
{
    OSIZE   len, *pos;
    char    *ptr = vptr;

    if(ptr == 0) return(0);
    pos = (OSIZE *) ptr;

    /* GB 02/98 */
    if(pos[0] <= 0) return(0);

    if(i < 0) return(pos[0]); /* i == -1 returns full lg */

    if(i > pos[1] - 1) return(-1);
    if (i == pos[1] - 1) len = pos[0] - pos[i + 2];
    else len = pos[i + 3] - pos[i + 2];

    return(len);
}

//void *P_alloc_get_ptr(char *ptr, int p)
void *P_alloc_get_ptr(void *ptr, int p)
{
    int     len;
    char    *val;

    len = P_get_len(ptr, p);
    val = SW_nalloc(len);
    memcpy(val, P_get_ptr(ptr, p), len); // plus de pointeur
    return(val);
}

int P_nb(ptr)
char    *ptr;
{
    OSIZE   *pos;

    pos = (OSIZE *) ptr;
    return(pos[1]);
}









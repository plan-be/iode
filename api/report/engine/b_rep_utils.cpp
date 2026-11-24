/**
 *  @header4iode
 * 
 *  IODE report utility functions
 *  -----------------------------
 *  The main part of this file contains allocation functions that are aimed to
 *  optimize allocations in report. These functions superseed SCR_malloc(), SCR_free()...
 *  
 *  WARNING: in the most recent versions of IODE, the define RP_STDMALLOC is set to 1.
 *  Therefore, the standard functions (SCR_malloc()...) are used inside RP_alloc(), RP_free()...
 */
#include "api/report/engine/engine.h"

//  RP_ALLOCATIONS
// ================
// New test library for report content memory optimisation.
// The idea is to reuse obsolete memory blocks to avoid too numerous alloc/free.

// Only used if RP_STDALLOC is null.

#define RP_STDALLOC     1
#define MEMBLK_BLK      100
#define MEMBLK_MAXLOST  512
#define MEMBLK_ADDSIZE  10


// Structure to store one allocated memory block. 
// Used (only when RP_STDALLOC is null) by RP_alloc() and RP_free().
struct MEMBLK {
    int  used;      // 1 if in use, 0 otherwise
    int  size;      // allocated size
    char *ptr;      // pointer to an allocated memory block
};

// Table of allocated memory blocs
static MEMBLK *RP_MEMBLKS = 0;     // Table of allocated MEMBLKs
static int     RP_NBMEMBLKS = 0;   // Nb of MEMBLKs

/**
 *  Allocates or reallocates RP_MEMBLKS (the table of MEMBLKs) by blocks of MEMBLK_BLK.
 *  
 *  @return int     always 0
 */
int RP_alloc_ptrs()
{
    if(RP_MEMBLKS == 0)
        RP_MEMBLKS = (MEMBLK *) SCR_malloc(MEMBLK_BLK * sizeof(MEMBLK)); // JMP 26/8/2012
    else
        RP_MEMBLKS = (MEMBLK *) SCR_realloc((char *)RP_MEMBLKS, sizeof(MEMBLK), RP_NBMEMBLKS, RP_NBMEMBLKS + MEMBLK_BLK); // JMP 26/8/2012

    // memset(RP_MEMBLKS + RP_NBMEMBLKS, 0, MEMBLK_BLK * sizeof(MEMBLK)); // JMP 26/8/2012
    RP_NBMEMBLKS += MEMBLK_BLK;
    return 0;
}


/**
 *  Mimics SCR_malloc() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *  
 *  @param [in] int     size    nb of bytes to allocate    
 *  @return     char*           allocated string filled with 0
 */  
char *RP_alloc(int size)
{
    int         i, pos, iunallocated = -1, iminsize = -1;

    // Taille nulle ==> retour NULL
    if(size <= 0) return((char *)0);

    if(RP_STDALLOC) {
        char *ptr = SCR_malloc(size); // JMP 26/8/2012
        //memset(ptr, 0, size); // JMP 26/8/2012
        return(ptr);
    }

    // 1. Recherche le plus petit pointeur >= size et t.q. used=0 --> minsize
    //    Note également le premier pointeur non alloué --> iunallocated
    for(i = 0 ; i < RP_NBMEMBLKS ; i++) {
        if(RP_MEMBLKS[i].used == 0) {
            if(RP_MEMBLKS[i].ptr == 0) iunallocated = i;
            else if(RP_MEMBLKS[i].size >= size && RP_MEMBLKS[i].size <= size + MEMBLK_MAXLOST) { // On ne veut pas perdre plus de MAXLOST - 1 bytes
                if(iminsize < 0) iminsize = i;
                else if(RP_MEMBLKS[iminsize].size > RP_MEMBLKS[i].size) iminsize = i;
            }
        }
    }

    // 2. Si iminsize >= 0, utiliser celui-là
    if(iminsize >= 0) {
        RP_MEMBLKS[iminsize].used = 1;
        memset(RP_MEMBLKS[iminsize].ptr, 0, size);
        return(RP_MEMBLKS[iminsize].ptr);
    }

    // 3. Position du premier libre (si iunallocated < 0, on ajoute des pointeurs)
    if(iunallocated >= 0) {
        pos = iunallocated;
    }
    else {
        pos = RP_NBMEMBLKS; // Position du premier libre (qui va être alloué)
        RP_alloc_ptrs();
    }

    // Allocation d'un bloc
    RP_MEMBLKS[pos].used = 1;
    RP_MEMBLKS[pos].size = size + MEMBLK_ADDSIZE;
    RP_MEMBLKS[pos].ptr = SCR_malloc(size + MEMBLK_ADDSIZE); // JMP 26/8/2012
    if(RP_MEMBLKS[pos].ptr == 0) return NULL;
    memset(RP_MEMBLKS[pos].ptr, 0, size + MEMBLK_ADDSIZE);
    return(RP_MEMBLKS[pos].ptr);
}


/**
 *  Mimics SCR_free() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *  
 *  @param [in] char*   ptr     buffer to free or NULL
 *  @return     int             always 0
 */  
int RP_free(char *ptr)
{
    int i;

    if(RP_STDALLOC) {
        if(ptr) SCR_free(ptr); // JMP 26/8/2012
        return 0;
    }

    for(i = 0 ; i < RP_NBMEMBLKS ; i++) {
        if(RP_MEMBLKS[i].ptr == ptr) {
            RP_MEMBLKS[i].used = 0;
            return 0;
        }
    }
    return -1;
}


/**
 *  Frees all memory buffers allocated by the RP_ALLOCATIONS library.
 */
void RP_free_bufs()
{
    int i;

    if(RP_VSEPS) SCR_free(RP_VSEPS);
    RP_VSEPS = 0;

    // MEMBLKS
    for(i = 0 ; i < RP_NBMEMBLKS ; i++) {
        SCR_free(RP_MEMBLKS[i].ptr);
        RP_MEMBLKS[i].ptr = 0;
    }

    SCR_free(RP_MEMBLKS);
    RP_MEMBLKS = 0;
    RP_NBMEMBLKS = 0;
}


// OTHER MISCELLANEOUS FUNCTIONS
// -----------------------------

/**
 *  Checks that a report line is a command ($command or #command).
 *  
 *  @param [in] char*   line    report line
 *  @return     int             1 if line is a command, 0 otherwise    
 */
int RP_is_cmd(char *line)
{
    if(line == 0) return 0;
    if((line[0] == '#' || line[0] == '$') &&
            line[1] != line[0] &&
            line[1] != 0 &&
            !U_is_in(line[1], " \n\r\t")
      )
        return(1);
    else
        return 0;
}

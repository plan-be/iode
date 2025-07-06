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
 *   
 *  List of functions
 *  -----------------
 *      char *RP_alloc(int size)                                             Mimics SCR_malloc() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *      int RP_free(char *ptr)                                               Mimics SCR_free() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *      void RP_free_bufs()                                                  Frees all memory buffers allocated by the RP_ALLOCATIONS library.
 *      char *RP_stracpy(char *ptr)                                          Mimics SCR_stracpy() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *      unsigned char **RP_vtoms(unsigned char* str, unsigned char *seps)    Mimics SCR_vtoms() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *      unsigned char **RP_vtom(unsigned char* str, int sep)                 Mimics SCR_vtom() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *      int RP_free_tbl(unsigned char **tbl)                                 Mimics SCR_free_tbl() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *      int RP_tbl_size(unsigned char **tbl)                                 Mimics SCR_tbl_size().
 *      int RP_is_cmd(char *line)                                            Checks that a report line is a command ($command or #command).
 *      U_ch **SCR_vtomsq(char* str, char* seps, int quote)                  Splits a string on one of separators. Text enclosed in quote char are not split.
 *      
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
// Used (only when RP_STDALLOC is null) by RP_alloc(), RP_stracpy(), RP_vtoms()... 
typedef struct _memblk_ {
    int  used;      // 1 if in use, 0 otherwise
    int  size;      // allocated size
    char *ptr;      // pointer to an allocated memory block
} MEMBLK;

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
    return(0);
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
    if(RP_MEMBLKS[pos].ptr == 0) return(NULL);
    memset(RP_MEMBLKS[pos].ptr, 0, size + MEMBLK_ADDSIZE);
    return(RP_MEMBLKS[pos].ptr);
}


// Not used
// --------
//  int RP_find_ptr(char *ptr)
//  {
//      int i;
//  
//      for(i = 0 ; i < RP_NBMEMBLKS ; i++) {
//          if(RP_MEMBLKS[i].ptr == ptr) {
//              return(i);
//          }
//      }
//      return(-1);
//  }


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
        return(0);
    }

    for(i = 0 ; i < RP_NBMEMBLKS ; i++) {
        if(RP_MEMBLKS[i].ptr == ptr) {
            RP_MEMBLKS[i].used = 0;
            return(0);
        }
    }
    return(-1);
}


/**
 *  Frees all memory buffers allocated by the RP_ALLOCATIONS library.
 */
void RP_free_bufs()
{
    extern char *RP_VSEPS;
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


/**
 *  Mimics SCR_stracpy() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *  
 *  @param [in] char* ptr   null or C string
 *  @return     char*       allocated copy of ptr
 */
char *RP_stracpy(char *ptr)
{
    char        *ptr1;

    if(RP_STDALLOC) return(SCR_stracpy(ptr));
    ptr1 = RP_alloc((int)strlen(ptr) + 1);
    strcpy(ptr1, ptr);
    return(ptr1);
}


/**
 *  Mimics SCR_vtoms() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *  
 *  @param [in] unsigned char *     str     string to split
 *  @param [in] unsigned char *     seps    accepted separators
 *  @return     unsigned char **            table of strings 
 */
unsigned char **RP_vtoms(unsigned char* str, unsigned char *seps)
{
    U_ch    **tbl = 0, *str2 = 0, *str1;
    int     ntbl = 0, i;

    if(RP_STDALLOC) {
        tbl = SCR_vtoms(str, seps);
        return(tbl);
    }

    if(str == 0) return(tbl);
    str1 = str; // Mémoire du ptr initial

    // 1. Compter les strings entre séparateurs
    while(1) {
        for(i = 0 ; str[i] ; i++)
            if(!U_is_in(str[i], seps)) break; /* SKIP SEPS */
        if(str[i] == 0) break;
        str += i;
        for(i = 0 ; str[i] ; i++)
            if(U_is_in(str[i], seps)) break;
        ntbl++;
        if(str[i] == 0) break; /* JMP 07-06-96 */
        str += i + 1;
    }

    tbl = (unsigned char **)RP_alloc((ntbl + 1) * sizeof(char *));
    str = str1; // reset valeur initiale
    ntbl = 0;
    while(1) {
        for(i = 0 ; str[i] ; i++)
            if(!U_is_in(str[i], seps)) break; /* SKIP SEPS */
        if(str[i] == 0) break;
        str += i;
        for(i = 0 ; str[i] ; i++)
            if(U_is_in(str[i], seps)) break;

        if(str2 == 0) {
            str2 = RP_stracpy(str);
            str = str2;
        }
        tbl[ntbl] = str;
        ntbl++;
        if(str[i] == 0) break; /* JMP 07-06-96 */
        str[i] = 0;
        str += i + 1;
    }

    return(tbl);
}

/**
 *  Mimics SCR_vtom() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *  
 *  @param [in] unsigned char *     str     string to split
 *  @param [in] int      sep                separator
 *  @return     unsigned char **            table of strings 
 */
unsigned char **RP_vtom(unsigned char* str, int sep)
{
    U_ch    seps[2];

    seps[0] = sep;
    seps[1] = 0;

    return(RP_vtoms(str, seps));
}

/**
 *  Mimics SCR_free_tbl() using RP_ALLOCATIONS if RP_STDALLOC is null.
 *  
 *  @param [in] unsigned char **    tbl     table of strings terminated by a NULL
 *  @return     int                         always 0
 */
int RP_free_tbl(unsigned char **tbl)
{
    if(RP_STDALLOC) {
        SCR_free_tbl(tbl);
        return(0);
    }

    RP_free(tbl[0]);
    RP_free((unsigned char *)tbl);
    return(0);
}


/**
 *  Mimics SCR_tbl_size().
 *  
 *  @param [in] unsigned char **    tbl     table of strings terminated by a NULL
 *  @return     int                         number of strings in tbl
 */
int RP_tbl_size(unsigned char **tbl)
{
    int i;

    if(tbl == 0) return(0);
    for(i = 0 ; tbl[i] ; i++);
    return(i);
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
    if(line == 0) return(0);
    if((line[0] == '#' || line[0] == '$') &&
            line[1] != line[0] &&
            line[1] != 0 &&
            !U_is_in(line[1], " \n\r\t")
      )
        return(1);
    else
        return(0);
}


/**
 *  Splits a string on one of separators. Text enclosed in quote char are not split.
 *  
 *  SCR_vtomsq("abc,def,\"hij,klm\",z", ",", "\"") 
 *  =>  abc
 *      def
 *      hij,klm
 *      z
 *  
 *  @param [in] char*   str     string
 *  @param [in] char*   seps    list of allowed seps
 *  @param [in] int     quote   character opening and closing non split substrings.
 *  @return     U_ch**          table of strings
 */
U_ch **SCR_vtomsq(char* str, char* seps, int quote)
{
    U_ch    **tbl = 0, tmp;
    int     ntbl = 0, i, inquote;

    inquote = 0;
    while(1) {
        for(i = 0; str[i] && !inquote; i++) {
            if(U_is_in(str[i], seps)) continue; /* SKIP SEPS */
            if(str[i] == quote) {
                inquote = 1;
                continue;
            }
            break;
        }
        if(str[i] == 0) break;

        str += i;
        if(inquote) {
            for(i = 0; str[i]; i++)
                if(str[i] == quote) break;
            inquote = 0;
        }
        else {
            for(i = 0; str[i]; i++)
                if(U_is_in(str[i], seps)) break;
            if(str[i] == quote) {
                inquote = 1;
                break;
            }
        }

        tmp = str[i];
        str[i] = 0;

        SCR_add_ptr(&tbl, &ntbl, str);
        str[i] = tmp;
        if(tmp == 0) break;
        str += i + 1;
    }

    SCR_add_ptr(&tbl, &ntbl, NULL);
    return(tbl);
}


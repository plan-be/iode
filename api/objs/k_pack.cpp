/**
 * @author Jean-Marc PAUL 
 * @author Geert BRYON
 * 
 * Functions for "packing" and "unpacking" IODE objects.
 * 
 * IODE stores objects in a "packed" form both in memory and on disk. 
 *   - to minimize memory usage
 *   - to avoid memory fragmentation (alloc/realloc/free...)
 * 
 * Object are unpacked "just in time" and repacked when they are not immediately needed.

 * A "packed" object is independent from the architecture (32|64 bits).
 * An object in its "struct" form, at the contrary, is different in 64 bits and 32 bits 
 * because of the different pointer lengths (4 bytes vs 8 bytes). 
 * Consequently, translations 64-32 and 32-64 are required both to pack and unpack objects.
 *
 * @note The 32-64 and 64-32 "translation" functions are local and hidden from the exposed API.
 * 
 * Packing functions
 * ----------------- 
 * bool K_opack(char** pack, char* a1, int* a2)     Reserved for future new objects
 * 
 * @note The "modern" terminology for pack and unpack is "serialize" and "deserialize".
 * @see scr4/s_swap.h (http://www.xon.be/scr4/libs1/libs1236.htm) for more details.
 */
#include "api/constants.h"
#include "api/lec/lec.h"
#include "api/objs/pack.h"
#include "api/objs/structs_32.h"

#include "api/objs/comments.h"
#include "api/objs/equations.h"
#include "api/objs/identities.h"
#include "api/objs/lists.h"
#include "api/objs/scalars.h"
#include "api/objs/tables.h"
#include "api/objs/variables.h"


/**
 *  Reserved for future new objects.
 *  
 *  @param [out] pack  char **      placeholder for the pointer to the serialized object
 *  @param [in]  a1    char*        NULL or pointer the the object.
 *  @param [in]  a2    char*        extra info 
 *  @return 0 for compatibily with other functions 
 */
bool K_opack(char** pack, char* a1, int* a2)
{
    *pack = (char*) P_create();
    if(a1 == NULL || *a2 == 0) 
        *pack = (char*) P_add(*pack, (char*) "", 1);
    else 
        *pack = (char*) P_add(*pack, a1, *a2);
    return true;
}

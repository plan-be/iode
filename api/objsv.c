#define _IODEVARG_
#include "iode.h"


/*
    Adds an object with a given name to the DB.
    It returns -1 if name1 cannot be added, -2 if the given list
    of arguments is incorrect.
    It returns the position of the object in the DB.
*/
K_add(kdb, name, a1, a2, a3, a4, a5, a6, a7, a8, a9)
KDB     *kdb;
char    *name, *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8, *a9;
{
    char    *pack;
    int     pos, lg;

    if(kdb == NULL) return(-1);
    if (K_pack[KTYPE(kdb)](&pack, a1, a2, a3, a4, a5, a6, a7, a8, a9) < 0)
        return(-2);
    pos = K_add_entry(kdb, name);
    if(pos < 0) {
        B_seterror("%.80s cannot be created (syntax ?)", name);
        goto einde;
    }

    lg = * (OSIZE *) pack;
    if(KSOVAL(kdb, pos) != 0) SW_free(KSOVAL(kdb, pos));
    KSOVAL(kdb, pos) = SW_alloc(lg);
    memcpy(KGOVAL(kdb, pos), pack, lg);
    /*
        if(lg < SW_SEG_SIZE) {
    	if(KSOVAL(kdb, pos) != 0) SW_free(KSOVAL(kdb, pos));
    	KSOVAL(kdb, pos) = SW_alloc(lg);
    	memcpy(KGOVAL(kdb, pos), pack, lg);
        }
        else {
    	kerror(0, "Segment exceeds %dk, %s too big", SW_SEG_SIZE/1024L, name);
    	kmsg("Old value restored!");
    	goto einde;
        }
    /* IODE64K */
einde:
    SW_nfree(pack);
    return(pos);
}


#include "scr4.h"
#include <s_swap.h>

/*NH*/

#ifndef SNGL_TASK
int     SW_NB_RES = 0, SW_MAX_RES = 0;
char    **SW_RES = 0;
#endif

/*NH*/
char *SW_alloc_seg(SWSIZE size)
{
    long    resmem;
    int     i;
    char    *ptr1;

#ifdef SNGL_TASK
    long    SCR_free_mem();

    if(SW_MIN_SEGS <= SW_NB_SEGS && SCR_free_mem() < /* JMP 08-03-92 */
	SW_MIN_MEM + SW_SEG_SIZE + 1024L +
	sizeof(SW_SEGS) * (SW_NB_SEGS + 10L))
	    return((char *)0);      /* LET AT LEAST SW_MIN_MEM bytes FREE */
//    ptr1 = (char *)malloc(SW_SEG_SIZE);
    //ptr1 = (char *)malloc(size);
    ptr1 = (char *)SCR_malloc(size); /* JMP 28-09-2012 */
    if(ptr1 == 0) return((char *)0);
    return(ptr1);
#else
//    ptr1 = malloc(SW_SEG_SIZE);
    //ptr1 = malloc(size);
    ptr1 = SCR_malloc(size);
    if(ptr1 == 0) return((char *)0);
    return(ptr1);

//////// OBSOLETE ////////////////////
    if(SW_MAX_RES == 0) {
#ifdef DOS
	resmem = _max_(64000L, SW_MIN_MEM);
#else
	resmem = _max_(1024000L, SW_MIN_MEM);
#endif
	SW_MAX_RES = 1 + resmem / SW_SEG_SIZE;
	//SW_RES = malloc((2 * SW_MAX_RES) * sizeof(char *));
    SW_RES = (char **)SCR_malloc((2 * SW_MAX_RES) * sizeof(char *)); /* JMP 28-09-2012 */
	if(SW_MAX_RES == 0) return((char *)0);
	SW_NB_RES = 0;
	}
    if(SW_NB_RES <= 0) {
	//ptr1 = malloc(resmem);
	ptr1 = SCR_malloc(resmem);
	if(ptr1 == 0) return((char *)0);
	for(i = 0 ; i < SW_MAX_RES ; i++) {
	    // SW_RES[i] = malloc(SW_SEG_SIZE);
	    SW_RES[i] = SCR_malloc(SW_SEG_SIZE);
	    if(SW_RES[i] == 0) break;
	    }
	SW_NB_RES = i;
	// free(ptr1);
	SCR_free(ptr1); /* JMP 22-08-2012 */
	if(SW_NB_RES <= 0) return((char *)0);
	}
    SW_NB_RES--;
    return(SW_RES[SW_NB_RES]);
#endif
}

/*NH*/

void SW_Debug(int size)
{
//    extern int DebugActif;

    DebugActif = 1;
    Debug("Impossible d'allouer : size=%d - SW_NB_SEGS=%d - free_mem=%d\n",
	    size, SW_NB_SEGS, SCR_free_mem());
}

/*NH*/
SW_create_seg(size)
SWSIZE  size;
{
    char    *ptr1, *ptr2;

    ptr1 = SW_alloc_seg(size);
    if(ptr1 == 0) {
	SW_Debug(size);
	return(-1);
	}
    memset(ptr1, 0, size);

    if(SW_NB_SEGS % 10 == 0) {
	    ptr2 = SW_nrealloc(SW_SEGS, sizeof(SWSEG) * SW_NB_SEGS,
				    sizeof(SWSEG) * (SW_NB_SEGS + 10));
	    if(ptr2 == 0) {
		SW_nfree(ptr1);
		return(-1);
		}
	    SW_SEGS = (SWSEG *) ptr2;
	    }

    SW_SEGS[SW_NB_SEGS].seg_ptr = ptr1;
#ifndef NOEMS
    SW_SEGS[SW_NB_SEGS].seg_type = 1; /* CONVENTIONAL MEMORY */
#endif
    SW_NB_SEGS++;
    return(0);
}

/*NH*/
SW_del_seg()
{
    SW_Debug(0);
#ifndef NOEMS
    if(SW_NB_SEGS == SW_NB_EMS) return(-1);
#endif
    if(SW_NB_SEGS < 2) return(-1);
    SW_NB_SEGS --;
    SW_append_file(SW_SEGS[SW_NB_SEGS].seg_ptr);
    REF(SW_SEGS[SW_NB_SEGS].seg_blk) = SW_FILE_POS - 1;
    SEG(SW_SEGS[SW_NB_SEGS].seg_blk) = -1;
    SW_nfree(SW_SEGS[SW_NB_SEGS].seg_ptr);
    return(0);
}

/*NH*/
SW_oldest_seg()
{
    long    age = AGE(0);
    int     old = 0, i;

    for(i = 1 ; i < SW_NB_SEGS ; i++)
	if(AGE(i) < age) {
	    age = AGE(i);
	    old = i;
	    }

    return(old);
}

/*NH*/
#ifndef NOEMS

/*NH*/
SW_oldest_ems_seg()
{
    long    age = SW_AGE, age1;
    int     old = 0, i, old1;

    for(i = 0 ; i < _min_(4, SW_NB_EMS) ; i++) {
	old1 = EMS_DEF.log_pgs[i];
	age1 = SW_SEGS[old1].seg_age;
	if(age1 < age) {
		age = age1;
		old = old1;
		}
	}

    return(old);
}
#endif

/*NH*/
void SW_swap_ems(seg_nb)
int     seg_nb;
{
#ifndef NOEMS
    int     physp, ems_seg;

    if(SW_SEGS[seg_nb].seg_type  != 0) return; /* NOT AN EMS SEG */
    if(SW_SEGS[seg_nb].seg_physp >= 0) return; /* IN PAGE FRAME */

    /* FIND OLDEST EMS SEG IN PAGE FRAME */
    ems_seg = SW_oldest_ems_seg();
    physp = SW_SEGS[ems_seg].seg_physp;
    EMS_map(seg_nb , physp);
    SW_SEGS[seg_nb].seg_physp = physp;
    SW_SEGS[seg_nb].seg_ptr = EMS_SEG(physp);
    SW_SEGS[ems_seg].seg_physp = -1;
#endif
}


#include <s_swap.h>
#include "s_lst.h"

int     SW_FILE_POS;
long    SW_AGE = 0L, SW_MIN_MEM = 120000L; /*65536L;*/

SWBLK   *SW_BLKS = 0;
int     SW_NB_BLKS = 0;

SWSEG   *SW_SEGS = 0;
int     SW_NB_SEGS = 0, SW_MIN_SEGS = 1;

LST     *SW_ITEMS,
	*SW_FREES;

/* char    *SW_BUF = 0; */
char    SW_FILE[65] = "";

#ifndef NOEMS
int     SW_NB_EMS = 0;
#endif


int     SW_SEG_SIZE = 32500; /* JMP 27-11-07 */







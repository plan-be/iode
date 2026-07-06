#ifndef _LST_
#define _LST_

typedef struct _lst_ {
    long    nb_els;
    int     nb_tbls;
    int     tbl_size;
    int     el_size;
    char    **tbls;
    char    *tmp_el;
} LST;

/* ==== VARIABLES ===== */

extern int LST_NO_TABS;

/* ====== FUNCTIONS ========*/
#include <s_prolst.h>   /* JMP 30-12-97 */

#include "s_alldoc.h"

#endif /* _LST_ */








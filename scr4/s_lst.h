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

#ifndef SCRPROTO
extern LST *LST_create();
extern int LST_add();
extern int LST_del();
extern int LST_copy();
extern int LST_move();
extern void *LST_get();
extern int LST_set();
extern LST *LST_read_in();
extern LST *LST_read_file();
extern LST *LST_vtol();
extern unsigned char *LST_ltov();

#else

#include <s_prolst.h>   /* JMP 30-12-97 */

#endif /* SCRPROTO*/

#include "s_alldoc.h"

#endif /* _LST_ */








#include "scr.h"

#ifndef SCRPROTO
void    *O_read_text();
#endif

/* U_ch      *OBJ_search_ptr();
char      *OBJ_read_record_key();
char      *SCR_palloc();
inutile /* JMP 05-01-98 */

/* === OBJ_READ_PRT =============================================
   ============================================================== */
OBJ_read_prt(pr)
PR_DEF  *pr;
{
    int     i;

    /* READ STRUCTURE */
    O_read(pr, sizeof(PR_DEF));

    O_read_text(&(pr->pr_name        ));
    O_read_text(&(pr->pr_normal      ));
    O_read_text(&(pr->pr_bold        ));
    O_read_text(&(pr->pr_e_bold      ));
    O_read_text(&(pr->pr_underl      ));
    O_read_text(&(pr->pr_e_underl    ));
    O_read_text(&(pr->pr_reverse     ));
    O_read_text(&(pr->pr_e_reverse   ));
    O_read_text(&(pr->pr_condensed   ));
    O_read_text(&(pr->pr_e_condensed ));
    O_read_text(&(pr->pr_enlarged    ));
    O_read_text(&(pr->pr_e_enlarged  ));
    O_read_text(&(pr->pr_begin       ));
    O_read_text(&(pr->pr_end         ));
    O_read_text(&(pr->pr_formfeed    ));
    O_read_text(&(pr->pr_linefeed    ));

    for(i = 0 ; i < 256 ; i++)
	O_read_text(&(pr->pr_maps[i]));

    return(0);
}


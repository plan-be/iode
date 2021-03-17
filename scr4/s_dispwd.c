#include "scr.h"

/*NH*/
/* =+====== PG_display_cadre ===================================
     Display the box around a page, and its background
     pg     : IN : pointer to the page
     return : 0 if no problem , positive value else
   ============================================================== */

PG_display_cadre(pg)
PAGE    *pg;
{
    int     lag = (pg->pg_scr_pos >= 0) ? 0:1;

    SCR_set_attr(pg->pg_back);
    if(pg->pg_fill != 0)
	SCR_fill((char)pg->pg_fill,
		 pg->pg_line,
		 pg->pg_column,
		 pg->pg_nb_line,
		 pg->pg_nb_column);

    if(pg->pg_box != 0)
	SCR_box(pg->pg_box,
				 pg->pg_line - 1,
				 pg->pg_column - 1,
				 pg->pg_nb_line + 2,
				 pg->pg_nb_column + 2);
    if(pg->pg_shadow != 0)  {
	SCR_shadow(pg->pg_shadow - 1, 1,
		   pg->pg_line - lag,
		   pg->pg_column - lag,
		   pg->pg_nb_line + lag*2, pg->pg_nb_column + lag*2,
		   SCR_SHADOW);
    }

    return(0);
}
/*NH*/
/* =+====== PG_display_title ===================================
     Display the title and the footnote of a page, if any
     pg     : IN : pointer to the page
     return : 0 if no problem , positive value else
   ============================================================== */

PG_display_title(pg)
PAGE    *pg;
{
    int     lg, pos, attr;

    attr = SCR_REVERSE;
    if(pg->pg_back == attr) attr = SCR_WHITE;        /* JMP 17-01-2004 */
    SCR_set_attr(attr);                             /* JMP 17-01-2004 */
    //if(CNF_REVERSE_TITLE == 1) SCR_set_attr(SCR_REVERSE);
    if(pg->pg_title != 0) {
	SCR_write_center(pg->pg_line - 1, pg->pg_column,
			 pg->pg_title, pg->pg_nb_column);
	}

    if(pg->pg_footnote != 0)
	SCR_write_center(pg->pg_line + pg->pg_nb_line, pg->pg_column,
			 pg->pg_footnote, pg->pg_nb_column);

    return(0);
}









#include "scr.h"
#include "s_udfld.h"


/*NH*/
/* =+======= PG_UPDN_FIELD ====================================
    Computes the upper or lower closed editable field.
    If page wrap is on and nothing found, continue scanning else return(-1)
	pg     : IN : pointer to the page
	c_line : IN : current line
	c_col  : IN : current col
	direct : IN : direction : 0 UP, 1 DOWN
	return : nb of the next field or nb_flds + 1 if none
		    and wrap off
   ================================================================= */

PG_updn_field(pg, c_line, c_col, direct)
PAGE    *pg;
int     c_line,
	c_col,
	direct;
{
    int     i,
	    best = -1,
	    b_line, line, o_line,
	    b_col, col, o_col;
    FIELD   *fld;

    o_line = c_line;
    o_col  = c_col;
ag:
    for(i = 0 ; i < pg->pg_nb_flds ; i++) {
	fld = pg->pg_flds + i;
	col = fld->fld_column;
	line = fld->fld_line;
	if(IS_INPUT(fld) &&
	      ((direct == UP && line < c_line) ||
	       (direct == DOWN && line > c_line))) {
	    if(best < 0) {
		best = i;
		b_line = line;
		b_col  = col;
		}
	    else {
		if((direct == UP && line > b_line) ||
		   (direct == DOWN && line < b_line) ||
		   (line == b_line && IS_BEST_COL)) {
		    best = i;
		    b_line = line;
		    b_col = col;
		    }
		}
	    }
	}

    if(best >= 0) {
	if(b_line != o_line || b_col != o_col) return(best);
	return(PG_leftright_field(pg, o_line, o_col, direct));
	}
    if(pg->pg_wrap == SCR_OFF) return(-1);
    else {
	c_line = (direct == UP) ? 10000 : -1;
	goto ag;
	}
}
/*NH*/
/* =+======= PG_leftright_FIELD ====================================
    Computes the leftmost or rightmost editable field.
    If page wrap is on, continue scanning if nothing found from
    begin (end) of page
	pg : IN : pointer to the page
	nb : IN : nb of the current field
	direct : IN : 0 = left , 1 = rigth
	return : nb of the prev field or -1 if none and wrap off
   ================================================================= */

PG_leftright_field(pg, c_line, c_col, direct)
PAGE    *pg;
int     c_line, c_col;
int     direct;
{
    int     i,
	    best = -1,
	    b_line, line,
	    b_col, col;
    FIELD   *fld;

    for(i = 0 ; i < pg->pg_nb_flds ; i++) {
	fld = pg->pg_flds + i;
	col = fld->fld_column;
	line = fld->fld_line;
	if(IS_INPUT(fld) && line == c_line &&
	   ((direct == LEFT && col < c_col) ||
	    (direct == RIGHT && col > c_col)) ) {
	    if(best < 0) {
		best = i;
		b_col  = col;
		}
	    else {
		if((direct == LEFT && col > b_col) ||
		   (direct == RIGHT && col < b_col) ) {
		    best = i;
		    b_col = col;
		    }
		}
	    }
	}

    if(best >= 0) return(best);
    return(PG_updn_field(pg, c_line, ((direct == LEFT) ? 10000 : -1), direct));
}


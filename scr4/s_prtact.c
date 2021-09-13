#include "scr.h"

/*NH*/
PR_action(aa)
A_ACTION  *aa;
{
    int     i, nk, *keys;
    char    *ptr = SCR_PTR(ACT_find_lg(aa, 0));
    char    *buf = ACT_find_str(aa, 0);
    int     val = ACT_find_lg(aa, 0);

    switch(aa->aa_fn) {
	case ACT_PR_PAGE :      PR_page((PAGE *)ptr); break;
	case ACT_PR_SELECT  :   PR_select((PR_DEF *)ptr); break;
	case ACT_PR_STRING  :   PR_string(buf); break;
	case ACT_PR_SEND :      PR_send(); break;
	case ACT_PR_LINEFEED:   PR_linefeed(); break;
	case ACT_PR_FORMFEED:   PR_formfeed(); break;
	case ACT_PR_TAB:        PR_tab(); break;
	case ACT_PR_VTAB:       PR_vtab(); break;
	case ACT_PR_PG_TAB:     PR_pg_tab(); break;
	case ACT_PR_BOLD:       PR_bold(); break;
	case ACT_PR_ENLARGED:   PR_enlarged(); break;
	case ACT_PR_CONDENSED:  PR_condensed(); break;
	case ACT_PR_UNDERLINE:  PR_underline(); break;
	case ACT_PR_NORMAL:     PR_normal(); break;
	case ACT_PR_UP    :     PR_up(val); break;
	case ACT_PR_DOWN  :     PR_down(val); break;
	case ACT_PR_LEFT  :     PR_left(val); break;
	case ACT_PR_RIGHT :     PR_right(val); break;
	case ACT_PR_GOTO  :
				PR_goto(val, (int)ACT_find_lg(aa, 1));
				break;
	default : return(-2);
    }
    return(0);
}


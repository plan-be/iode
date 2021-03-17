#include "scr.h"

/*NH*/
PG_help_stack(pg, fld_nb)   /* BP_MODIF 14-04-95 */
PAGE    *pg;
int     fld_nb;
{
    char    *txt;

    HLP_stack_add(pg->pg_help);
    if(fld_nb < 0)
	HLP_stack_add(0L);
    else {      /* BP_MODIF 14-04-95 */
	txt = PG_fld_help(pg, fld_nb);
	if(txt == 0L && SCR_HLP_NOPGNAME == 0)  /* JMP 01-02-00 */
	    txt = PG_search_fld_name(pg, fld_nb);
	if(txt != 0L)
	    HLP_stack_add(txt);
	else
	    HLP_stack_add(0L);
    }
    return(0);
}
/*NH*/
char *PG_search_fld_name(pg, fld_nb) /* BP_MODIF 14-04-95 */
PAGE    *pg;
int     fld_nb;
{
    char    *txt, *SCR_find_ptr_name();
    char    buf[256], buf2[50];
    static  char fld_name[30];

    if(fld_nb < 0) return(0L);
    txt = SCR_find_ptr_name(pg);
    if(txt == 0) return(0L);
    strcpy(buf2, txt);
    buf2[strlen(buf2) - 2] = 0;

    if(pg->pg_fld_name != 0) {
	txt = PG_fld_name(pg, fld_nb);
	if(txt != 0L) {
	    sprintf(fld_name, "%s_%s", buf2, txt);
	    SCR_lower(fld_name);
	    return(fld_name);
	}
	return(0L);
    }
    if(pg->pg_name_pos != 0) {
	pg->pg_fld_name = (unsigned char *)SCR_malloc(1024);
	SCR_read_cmt_len(pg->pg_name_pos, pg->pg_fld_name);
	txt = PG_fld_name(pg, fld_nb);
	if(txt != 0L) {
	    sprintf(fld_name, "%s_%s", buf2, txt);
	    SCR_lower(fld_name);
	}
	SCR_free(pg->pg_fld_name);
	pg->pg_fld_name = 0L;
	if(txt != 0L) return(fld_name);
    }
    return(0L);
}


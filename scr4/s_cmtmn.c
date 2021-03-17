#include "scr.h"
extern FILE *fd_cmt;

/*NH*/
MN_read_opt(mn)
MENU    *mn;
{
    int     i;
    int     nb_opt = mn->mn_nb_opts;
    OPTION  *opt;

    if(mn->mn_opt_mem || mn->mn_in_mem == 1) return(0);

    SCR_open_cmt(0);
    fseek(fd_cmt, mn->mn_opt_pos + SCR_CMT_FILE_POS, 0); /* JMP 16-04-95 */
    /* READ TITLE AND FOOTNOTE */
    CMT_read_text((char **)&(mn->mn_title));
    CMT_read_text((char **)&(mn->mn_foot));
    CMT_read_text((char **)&(mn->mn_help));

    /* READ OPTION */
    if(nb_opt > 0) {
	mn->mn_opts = (OPTION *)SCR_malloc(sizeof(OPTION) * nb_opt);
	opt = mn->mn_opts;
	fread(opt, sizeof(OPTION) * nb_opt, 1, fd_cmt);
	for(i = 0 ; i < nb_opt ; i++, opt++) {
	    /* READ TEXT OF OPTION */
	    CMT_read_text((char **)&(opt->opt_text));
	    CMT_read_text((char **)&(opt->opt_help));
	}
    }
    mn->mn_in_mem = 1;
    return(0);
}

/*NH*/
MN_free_opt(mn)
MENU    *mn;
{
    int     i;
    OPTION  *opt = mn->mn_opts;

    if(mn->mn_opt_mem || mn->mn_in_mem == 0) return(0);
    SCR_free(mn->mn_title);
    SCR_free(mn->mn_foot);
    SCR_free(mn->mn_help);
    for(i = 0 ; i < mn->mn_nb_opts ; i++, opt++) {
	/* READ TEXT OF OPTION */
	SCR_free(opt->opt_text);
	SCR_free(opt->opt_help);
    }
    SCR_free(mn->mn_opts);
    mn->mn_in_mem = 0;
    mn->mn_nb_opts = 0; /* BP_M 22-08-2011 12:01 */
    return(0);
}



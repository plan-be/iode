#include "scr.h"

extern FILE    *fd_cmt;

/*NH*/
PG_read_flds(pg)
PAGE    *pg;
{
    int     i;
    int     j;
    FIELD   *fld;
    int     nb_fld = pg->pg_nb_flds;

    /* READ STRUCTURE FIELD */
    if(SCR_level != 0 ||
       pg->pg_fld_mem || pg->pg_fld_in_mem == 1 || nb_fld == 0)
	    return(0);

    pg->pg_fld_in_mem = 1;

    SCR_open_cmt(0);
    fseek(fd_cmt, pg->pg_fld_posit + SCR_CMT_FILE_POS, 0); /* JMP 16-04-95 */

    pg->pg_flds = (FIELD *)SCR_malloc(sizeof(FIELD) * nb_fld);
    fld = pg->pg_flds;
    fread(fld, sizeof(FIELD) * nb_fld, 1, fd_cmt);

    for(j = 0 ; j < nb_fld ; j++, fld++) {
	if(fld->fld_nb_isam > 0) {
	    fld->fld_isam = (ISFLD *)SCR_malloc(sizeof(ISFLD) * fld->fld_nb_isam);
	    fread(fld->fld_isam, sizeof(ISFLD) * fld->fld_nb_isam, 1, fd_cmt);
	}

	/* READ DEFAULT TEXT */
	if(fld->fld_dtext != 0)
		CMT_read_text((char **)&(fld->fld_dtext));

	if(fld->fld_acmp != 0)
	    CMT_read_len((char **)&(fld->fld_acmp));
    }
    CMT_read_len((char **)&(pg->pg_fld_help   ));
    CMT_read_len((char **)&(pg->pg_fld_allowed));
    CMT_read_len((char **)&(pg->pg_fld_fmt    ));
    CMT_read_len((char **)&(pg->pg_fld_cmt    ));
    CMT_read_len((char **)&(pg->pg_fld_pos    ));
    return(0);
}

/*NH*/
PG_fld_free(pg)
PAGE    *pg;
{
    int     i, nb_fld = pg->pg_nb_flds;
    FIELD   *flds = pg->pg_flds, *fld;

    /* READ STRUCTURE FIELD */
    if(pg->pg_fld_in_mem == 0) return(0);

    pg->pg_fld_in_mem = 0;

    if(nb_fld == 0) return(0);
    fld = flds;

    for(i = 0 ; i < nb_fld ; i++, fld++) {
	SCR_free(fld->fld_isam);

	/* READ DEFAULT TEXT */
	if(fld->fld_dtext != 0)
		SCR_free(fld->fld_dtext);
    }
    SCR_free(flds);
    SCR_free(pg->pg_fld_help   );
    SCR_free(pg->pg_fld_allowed);
    SCR_free(pg->pg_fld_fmt    );
    SCR_free(pg->pg_fld_cmt    );
    SCR_free(pg->pg_fld_pos    );
    return(0);
}


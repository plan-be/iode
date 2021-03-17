#include "scr.h"

extern FILE    *fd_cmt;

PG_read_drws(pg)
PAGE    *pg;
{
    int     i, j;
    DRAWS   *dw = &(pg->pg_drws);
    DRAW    *drw;

    /* READ STRUCTURE */
    if(dw->dw_nb_drws == 0 && pg->pg_title == 0 && pg->pg_footnote == 0)
	return(0);

    SCR_open_cmt(0);
    fseek(fd_cmt, pg->pg_drw_pos + SCR_CMT_FILE_POS, 0); /* JMP 16-04-95 */
    if(dw->dw_nb_drws > 0) {
	dw->dw_drws = (DRAW *) SCR_malloc(sizeof(DRAW) * dw->dw_nb_drws);
	fread(dw->dw_drws, sizeof(DRAW) * dw->dw_nb_drws, 1, fd_cmt);
	/* IF TYPE TEXT READ TEXT */
	drw = dw->dw_drws;
	for(i = 0 ; i < dw->dw_nb_drws ; i++, drw++)
	    if(drw->drw_type == SCR_TEXT)
		CMT_read_text((char **)&(drw->drw_text));
    }
    CMT_read_text((char **)&(pg->pg_title));
    CMT_read_text((char **)&(pg->pg_footnote));
    return(0);
}

/*NH*/
PG_drw_free(pg)
PAGE    *pg;
{
    int     i;
    DRAW    *drw;
    DRAWS   *dw = &(pg->pg_drws);

    drw = dw->dw_drws;
    for(i = 0 ; i < dw->dw_nb_drws ; i++, drw++)
	if(drw->drw_type == SCR_TEXT)
	    SCR_free(drw->drw_text);
    SCR_free(dw->dw_drws);
    SCR_free(pg->pg_title);
    SCR_free(pg->pg_footnote);
    return(0);
}






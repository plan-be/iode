#include "scr.h"

#define FLD_RSIZE   (fld->fld_nb_column * fld->fld_nb_line) /* JMP 29-05-91 */
#define FLD_VSIZE   (fld->fld_ncv * fld->fld_nlv)           /* JMP 29-05-91 */
#define FLD_ISLBR   ((fld->fld_pv > 0) ? 1 : 0)
#define FLD_ISRBR   ((fld->fld_pv + FLD_VSIZE - FLD_ISLBR) < FLD_RSIZE)

extern int FLD_mark[2];

/*NH*/
PGd_write_field_cmp(pg, fld_nb, edtxt, flag_cmp)
PAGE    *pg;
int     fld_nb;
char    *edtxt;
int     flag_cmp;
{
    FIELD           *fld = pg->pg_flds + fld_nb, *fldr; /* BP_M 21-01-97 */
    int             i, basel, basec, isrbr, j;
    //unsigned char   *bumpl, *bumpr, buf[133], radiob[255], c; /* BP_M 19-01-97 */
    unsigned char   *bumpl, *bumpr, buf[1024], radiob[255], c; /* JMP 10-01-11 */

    basel = pg->pg_line + fld->fld_line;
    basec = pg->pg_column + fld->fld_column;

    if(fld->fld_type == SCR_EDITOR) return(0);
    if(fld->fld_io == SCR_HIDDEN - SCR_INPUT) goto fin; /* JMP 21-06-91 */
    SCR_set_attr(fld->fld_attributes);

    SCR_clear_xattrs(basel, basec, fld->fld_nlv, fld->fld_ncv);     /* JMP 11-01-2004 */
    if(fld->fld_io != SCR_OUTPUT - SCR_INPUT)
	SCR_write_im_xbox(basel, basec, fld->fld_nlv, fld->fld_ncv, 1); /* JMP 11-01-2004 */
    isrbr = PG_edfl_isrbr(fld, edtxt);
    switch(fld->fld_type) {     /* BP_M 19-01-97 */
	case SCR_PASSWD:
	    memset(buf, '*', 132);
	    for(i = 0 ; i < fld->fld_nlv ; i++)
		SCR_write(basel + i, basec, buf, fld->fld_ncv);
	    break;
	case SCR_CHECK_BOX:     /* BP_M 19-01-97 */
/*            if(SCR_CHECK_BOX_CHAR == 0) SCR_CHECK_BOX_CHAR = "X";
	    if(SCR_CHECK_BOX_TXT  == 0) SCR_CHECK_BOX_TXT = "[%c]";
--> reporté dans base_init /* JMP 06-02-97 */
	    if(edtxt[0] == ' ') c = ' ';
	    else                c = SCR_CHECK_BOX_CHAR[0];
	    sprintf(radiob, SCR_CHECK_BOX_TXT, c);
	    SCR_write(basel, basec, radiob, strlen(radiob));
	    break;
	case SCR_TYPE_RADIO:    /* BP_M 21-01-97 */
/*            if(SCR_RADIO_CHAR == 0) SCR_RADIO_CHAR = ".";
	    if(SCR_RADIO_TXT  == 0) SCR_RADIO_TXT = "(%c)";
--> reporté dans base_init /* JMP 06-02-97 */
	    j = PG_calc_nummer_radio(pg, fld_nb);
	    i = atoi(edtxt);
	    if(i == j) c = SCR_RADIO_CHAR[0];
	    else       c = ' ';
	    sprintf(radiob, SCR_RADIO_TXT, c);
	    SCR_write(basel, basec, radiob, strlen(radiob));
	    break;
	default:
	    /* LIGNE 1 */
	    PGd_write_field_line(fld->fld_attributes, basel, basec + FLD_ISLBR,
				 edtxt, fld->fld_pv, fld->fld_ncv - FLD_ISLBR);

	    /* LIGNES SUIVANTES */
	    for(i = 1 ; i < fld->fld_nlv - 1 ; i++)
		PGd_write_field_line(fld->fld_attributes, basel + i,
		      basec,
		      edtxt,
		      fld->fld_pv + i * fld->fld_ncv - FLD_ISLBR, fld->fld_ncv);
/*                SCR_write(basel + i,
		      basec,
		      edtxt + fld->fld_pv + i * fld->fld_ncv - FLD_ISLBR,
		      fld->fld_ncv);
*/
	    /* DERNIERE LIGNE */
	    if(fld->fld_nlv > 1)     /* BP_MODIF 25-11-98 */
		PGd_write_field_line(fld->fld_attributes, basel + fld->fld_nlv - 1,
		      basec,
		      edtxt,
		      fld->fld_pv + i * fld->fld_ncv - FLD_ISLBR,
		      fld->fld_ncv - isrbr);

/*                SCR_write(basel + fld->fld_nlv - 1,
		      basec,
		      edtxt + fld->fld_pv + i * fld->fld_ncv - FLD_ISLBR,
		      fld->fld_ncv - isrbr);
*/
	    break;
    }
    /* BUMPERS */
#ifdef DOS
    bumpl = (unsigned char *)"\256";
    bumpr = (unsigned char *)"\257";
#else
    bumpl = (unsigned char *)"<";
    bumpr = (unsigned char *)">";
#endif

    if(FLD_ISLBR) {                                   /* JMP 29-05-91 */
	SCR_set_attr(SCR_CHELP1);                           /* JMP 29-05-91 */
	SCR_write(basel, basec, bumpl, 1);                    /* JMP 29-05-91 */
	}                                                   /* JMP 29-05-91 */

    if(isrbr) {                                             /* JMP 29-05-91 */
	SCR_set_attr(SCR_CHELP1);                           /* JMP 29-05-91 */
	SCR_write(basel + fld->fld_nlv - 1,                 /* JMP 29-05-91 */
		  basec + fld->fld_ncv - 1,                 /* JMP 29-05-91 */
		  bumpr, 1);                                /* JMP 29-05-91 */
	}                                                   /* JMP 29-05-91 */
fin:
    if(flag_cmp) SCR_apply_cmp(pg, fld_nb);
    if(fld->fld_box)
	SCR_shadow_0(0, basel, basec, fld->fld_nlv, fld->fld_ncv, pg->pg_back);

    return(0);
}


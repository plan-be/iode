#include "iode.h"

/*
    Reads an rotated ascii file with the following format

	sample 1980Y1 1990Y1
	A B C
	1 2 3
	na na 4 5
	.....
*/


YYKEYS IMP_RASC_KEYS[] = {
    "sample",   ASC_SMPL
};

char    **RASC_toc = NULL;
int     RASC_cv = 0, RASC_cy = 0;

int IMP_hd_rasc(yy, smpl)
YYFILE  *yy;
SAMPLE  *smpl;
{
    int     done = 0, nbtoc = 0;
    SAMPLE  *rsmpl;

    if (YY_lex(yy) != ASC_SMPL)  return(-1);

    rsmpl = K_read_smpl(yy);
    if(rsmpl == NULL) return(-1);
    memcpy(smpl, rsmpl, sizeof(SAMPLE));

    while(!done) {
	switch(YY_lex(yy)) {
	    case YY_EOF :
		SCR_free_tbl(RASC_toc); RASC_toc = 0;
		return(-1);
	    case YY_WORD :
		SCR_add_ptr(&RASC_toc, &nbtoc, yy->yy_text);
		break;
	    default :
		SCR_add_ptr(&RASC_toc, &nbtoc, NULL);
		YY_unread(yy);
		done = 1;
		break;
	}
    }

    SW_nfree(rsmpl);
    return(0);
}


int IMP_elem_rasc(yy, name, shift, value)
YYFILE  *yy;
char    *name;
int     *shift; IODE_REAL    *value;
{
    int rc;

    if(YY_lex(yy) == YY_EOF) return(-1);
    else YY_unread(yy);

    if(RASC_cv >= SCR_tbl_size(RASC_toc)) {
	RASC_cv = 0;
	RASC_cy ++;
	*shift = RASC_cy;
    }
    strcpy(name, RASC_toc[RASC_cv]);
    *value = K_read_real(yy);
    RASC_cv ++;

    return(0);
}

int IMP_end_rasc()
{
    SCR_free_tbl(RASC_toc);
    RASC_toc = 0;
    RASC_cv = RASC_cy = 0;
    return(0);
}

IMPDEF IMP_RASC = {
    IMP_RASC_KEYS,
    1,
    IMP_hd_rasc,
    NULL,
    IMP_elem_rasc,
    IMP_end_rasc
};

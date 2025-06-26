#include "scr.h"

/*NH*/
/* Check que l'index to n'est pas dépassé */
SC_check_index(is, to, to_def, cmp_nb, direct) /* JMP_M 4.19 31-05-95 */
ISAM    *is;
char    *to, *to_def;
int     cmp_nb, direct;                        /* JMP_M 4.19 31-05-95 */
{
    int         i, type, start, lg, diff;
    COMPOSITE   *cmp;
    ISFIELD     *isf;
    char        *rec;

    if(cmp_nb < 0 || to == 0) return(0);
    rec = is->is_rec;
    cmp = is->is_cmps + cmp_nb;
    for(i = 0 ; i < cmp->cmp_nb_flds ; i++) {
     /* if(to_def[cmp->cmp_flds[i]]) return(0); */
	if(SC_chk_1(to_def[cmp->cmp_flds[i]]) == 0) return(0); /* JMP 23-05-97 */
	isf   = is->is_flds + cmp->cmp_flds[i];
	type  = isf->isf_type;
	start = isf->isf_start;
	lg    = isf->isf_length;
	diff = SC_cmp_flds(rec + start, to + start, type, lg);
	if(diff * direct > 0) return(-1); /* JMP_M 4.19 31-05-95 */
	}
    return(0);
}

/*NH*/
/* Check que le record correspond aux critères */
SC_check_rec(is, from, to, from_def, to_def)
ISAM    *is;
char    *from, *to,
	*from_def, *to_def;
{
    int     i, type, start, lg, diff;
    ISFIELD *isf;
    char    *rec;

    if(from == 0 || to == 0) return(0);
    rec = is->is_rec;
    isf = is->is_flds;
    for(i = 0 ; i < is->is_nb_flds ; i++, isf++) {
	type  = isf->isf_type;
	start = isf->isf_start;
	lg    = isf->isf_length;
       /* if(!from_def[i]) { */
	if(SC_chk_1(from_def[i])) { /* JMP 23-05-97 */
	    diff = SC_cmp_flds(rec + start, from + start, type, lg);
	    if(diff < 0) return(-1);
	    }
	/*if(!to_def[i]) { */
	if(SC_chk_1(to_def[i])) { /* JMP 23-05-97 */
	    diff = SC_cmp_flds(rec + start, to + start, type, lg);
	    if(diff > 0) return(-1);
	    }
	}
    return(0);
}

/*NH*/
SC_cmp_flds(rec1, rec2, type, lg)
unsigned char   *rec1, *rec2;           /* JMP 17-02-92 */
int             type, lg;
{
    int         i;
    short       bufs1, bufs2;
    long        bufl1, bufl2;
    float       buff1, buff2;
    double      bufd1, bufd2;
    char        bufc1[121], bufc2[121];

    switch(type) {
	case SCR_ZSTRING :
	case SCR_STRING :
	    memcpy(bufc1, rec1, lg); /* JMP 04-02-2004 */
	    memcpy(bufc2, rec2, lg); /* JMP 04-02-2004 */
	    bufc1[lg] = bufc2[lg] = 0; /* JMP 04-02-2004 */
	    if(U_is_in('%', bufc1) || U_is_in('%', bufc2)) { // LIKE   JMP 23-11-2003
		SCR_strip(bufc1);
		SCR_strip(bufc2);
		return(SCR_grep_1(bufc2, bufc1, 0, 0, '%'));           /* JMP 23-11-2003 */
		}
	    lg = min(lg, 120);                  /* JMP 17-02-92 */
	    SCR_compress(bufc1, rec1, lg);      /* JMP 17-02-92 */
	    SCR_compress(bufc2, rec2, lg);      /* JMP 17-02-92 */
	    return(strncmp(bufc1, bufc2, lg));  /* JMP 17-02-92 */

	case SCR_CHAR :
	case SCR_SHORT1:
	    if(rec1[0] < rec2[0]) return(-1);   /* JMP 17-02-92 */
	    if(rec1[0] > rec2[0]) return(1);    /* JMP 17-02-92 */
	    return(0);                          /* JMP 17-02-92 */

	case SCR_SHORT :
	    memcpy(&bufs1, rec1, lg);
	    memcpy(&bufs2, rec2, lg);
	    if(bufs1 < bufs2) return(-1);
	    if(bufs1 > bufs2) return(1);
	    return(0);

	case SCR_AUTO :
	case SCR_REF  :
	case SCR_LONG :
	case SCR_DATE :
	case SCR_TIME :
	    memcpy(&bufl1, rec1, lg);
	    memcpy(&bufl2, rec2, lg);
	    if(bufl1 < bufl2) return(-1);
	    if(bufl1 > bufl2) return(1);
	    return(0);

	case SCR_FLOAT :
	    memcpy(&buff1, rec1, lg);
	    memcpy(&buff2, rec2, lg);
	    if(buff1 < buff2) return(-1);
	    if(buff1 > buff2) return(1);
	    return(0);

	case SCR_DOUBLE :
	    memcpy(&bufd1, rec1, lg);
	    memcpy(&bufd2, rec2, lg);
	    if(bufd1 < bufd2) return(-1);
	    if(bufd1 > bufd2) return(1);
	    return(0);

	default :
	    return(-1);
	}
}

/*NH  Retourne 1 s'il faut checker, 0 sinon */
SC_chk_1(chk)
int     chk;        /* JMP 09-10-97 */
{
    if(chk / 2 == SCR_ISF_CHECKALWAYS - SCR_ISF_CHECK) return(1);
    if(chk / 2 == SCR_ISF_CHECKNEVER - SCR_ISF_CHECK) return(0);
    if(chk % 2) return(0);
    return(1);
}

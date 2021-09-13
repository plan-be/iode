#include "scr.h"

int  ScrDumpIdx = -1;       /* BP_M 12-12-96 */
char *ScrDumpFirst = 0;      /* BP_M 12-12-96 */
char *ScrDumpLast = 0;       /* BP_M 12-12-96 */
long ScrDumpNbr = 2000000000L;  /* BP_M 12-12-96 */
char *ScrDumpCond = 0;       /* BP_M 12-12-96 */
char *ScrDumpWhere = 0;       /* BP_M 24/09/2002 */

extern int  SCR_fdbsys;     /* BP_M 13-06-2008 */
extern int  SCR_dft_dbsys;  /* BP_M 13-06-2008 */

IS_dump_rec_msg(is, filename, msg)     /* BP_MODIF 08-02-99 */
ISAM    *is;
char    *filename;
int     msg;
{
    int     i, cond = SCR_DB, len, nfld = 0;
    char    buf[4096], *buf2;   /* BP_M 27-02-1998 */
    long    number = 0;
    FILE    *fd_out;
    char    *ptr, lastval[255], tmpval[255];

    fd_out = fopen(filename, "w+");
    if(fd_out == 0) {
	PG_display_error("cannot open output file '%s'\n", filename);
	return(-1);
    }

    if(SCR_fdbsys == 1) {
	Debug("Force dbsys to : %d\n", SCR_dft_dbsys);
	is->is_type_dbd = SCR_dft_dbsys;
    }
    if(IS_open_files(is, 0L) != 0) {
	PG_display_error( "cannot open file '%s'\n", is->is_filename);
	fclose(fd_out);
	return(-1);
    }
    IS_empty_rec(is);

    if(ScrDumpCond != 0L) {
	if(strcmp(ScrDumpCond, "GE") == 0) cond = SCR_GE;
	if(strcmp(ScrDumpCond, "LE") == 0) cond = SCR_LE;
	if(strcmp(ScrDumpCond, "EQ") == 0) cond = SCR_EQ;
	if(strcmp(ScrDumpCond, "DB") == 0) cond = SCR_DB;
    }
    if(ScrDumpIdx != -1) {
	nfld = is->is_cmps[ScrDumpIdx].cmp_flds[0];
	len = is->is_flds[nfld].isf_length;
	PG_display_error( "Field of index : %d, len of field : %d, Condition : %d\n", nfld, len, cond);
	if(ScrDumpLast != 0L) {
	    IS_set_field_txt(is, nfld, ScrDumpLast);
	    IS_get_field(is, nfld, lastval);
	}
	IS_empty_rec(is);
	if(ScrDumpFirst != 0L)
	    IS_set_field_txt(is, nfld, ScrDumpFirst);
    }
    if(ScrDumpIdx == -1) ScrDumpIdx = 0;

//  IS_set_limit(is, -1);   /* BP_M 05-02-2011 20:10 */
    if(ScrDumpWhere == 0) {
	if(IS_search(is, ScrDumpIdx, cond, (char *)0) != 0) {
	    PG_display_error( "record not found in file '%s'\n", is->is_filename);
	    fclose(fd_out);
	    return(-1);
	}
    }
    else {
	if(IS_search_sql(is, ScrDumpWhere) != 0) {
	    PG_display_error( "record not found in file '%s' where clause = %s\n", is->is_filename, ScrDumpWhere);
	    fclose(fd_out);
	    return(-1);
	}
    }

    while(1) {
	if(ScrDumpLast != 0L) {
	    IS_get_field(is, nfld, tmpval);
	    if(cond == SCR_GE && memcmp(tmpval, lastval, len) > 0) break;
	    if(cond == SCR_LE && memcmp(tmpval, lastval, len) < 0) break;
	}

	IS_dump_rec_isam(fd_out, is, 0);

	if(number % 10 == 0)
	    fprintf(stderr, "'%ld' record dumped\r", number);
	number++;
	if(ScrDumpNbr <= number) break;
	if(IS_next(is) != 0) break;
    }
    IS_close(is);
    fclose(fd_out);
    if(msg) PG_display_error( "'%d' record dumped to output\n", number);
    return(0);
}


/* ======================================================================
Cr‚e un fichier ascii … partir de la base de donn‚es d‚crite par l'ISAM is.
Le fichier ascii aura pour nom filename.

Chaque record occupe une ligne du fichier ascii. Chaque champ est s‚par‚ du
pr‚c‚dent par un pipe (|).

Cette fonction envoie des messages dans le standard output. Elle est
utilis‚e par scr4 avec l'option -dump.

&RT -1 si un des fichiers ne peut ˆtre ouvert ou si l'ISAM est vide. 0
    sinon.
&SA IS_load_rec()
------------------------------------------------------------------------- */
IS_dump_rec(ISAM *is, char *filename)
{
    return(IS_dump_rec_msg(is, filename, 1));
}

IS_dump_rec_nomsg(ISAM *is, char *filename)
{
    return(IS_dump_rec_msg(is, filename, 0));
}


/*NH*/
IS_dump_rec_interne(is, filename)       /* BP_M 17-06-1993 11:15 */
ISAM    *is;
char    *filename;
{
    int     i;
    char    buf[2000];
    ISFIELD *isf;
    ISFINFO *isi;
    short   s;
    long    l, number = 0;
    double  d;
    float   f;
    FILE    *fd_out;

    fd_out = fopen(filename, "wb+");
    if(fd_out == 0) {
	PG_display_error( "cannot open output file '%s'\n", filename);
	return(-1);
    }

    if(IS_open(is, SCR_READ, SCR_NO_LOCK) != 0) {
	PG_display_error( "cannot open file '%s'\n", is->is_filename);
	fclose(fd_out);
	return(-1);
    }
    /* BP_M 12-12-2007 */
    IS_empty_rec(is);
//  IS_set_limit(is, -1);   /* BP_M 05-02-2011 20:10 */
    if(ScrDumpWhere == 0) {
	if(IS_search(is, 0, SCR_DB, (char *)0) != 0) {
	    PG_display_error( "record not found in file '%s'\n", is->is_filename);
	    fclose(fd_out);
	    return(-1);
	}
    }
    else {
	if(IS_search_sql(is, ScrDumpWhere) != 0) {
	    PG_display_error( "record not found in file '%s' where clause = %s\n", is->is_filename, ScrDumpWhere);
	    fclose(fd_out);
	    return(-1);
	}
    }
    /* BP_M 12-12-2007 */
    while(1) {
	fwrite(is->is_rec, is->is_rec_len, 1, fd_out);
	number++;
	if(IS_next(is) != 0) break;
    }
    IS_close(is);
    fclose(fd_out);
    PG_display_error( "'%d' record dumped to output\n", number);
    return(0);
}

/*NH*/
IS_dump_long_txt(fd, txt)       /* BP_M 26-10-95 */
FILE    *fd;
char    *txt;
{
    while(*txt) {
	if(*txt == '\n') fputc('~', fd);
	else             fputc(*txt, fd);
	txt++;
    }
    return(0);
}

/*NH*/
IS_dump_rec_isam(fd_out, is, start)
FILE    *fd_out;
ISAM    *is;
int     start;
{
    int     i, len, nfld = 0, j, len_txt = 0;
    U_ch    buf[2000], *buf2 = 0, *txt = 0;
    ISFIELD *isf;
    ISFINFO *isi;
    COMPOSITE *cmp;
    short   s;
    long    l;
    double  d;
    float   f;
    char    *ptr;
    int     *mfld = 0;

    isi = is->is_info+start;

    for(i = start, isf = is->is_flds+start ; i < is->is_nb_flds ; i++, isf++) {
	if(isf->isf_var) {
	    switch(isf->isf_type) {
		case SCR_EDITOR:
		    IS_get_vfield(is, i, &ptr);
		    if(ptr != 0) IS_dump_long_txt(fd_out, ptr);
		    break;

		case SCR_DATE:
		case SCR_LONG:
		case SCR_TIME:
		case SCR_AUTO:
		    l = 0L;
		    IS_get_vfield(is, i, &ptr);
		    if(ptr != 0) memcpy(&l, ptr, sizeof(long));
		    fprintf(fd_out, "%ld", l);
		    break;
		case SCR_MENU:
		case SCR_SHORT:
		case SCR_TYPE_RADIO: /* BP_M 21-01-97 */
		case SCR_CHECK_BOX:  /* BP_M 21-01-97 */
		    s = 0;
		    IS_get_vfield(is, i, &ptr);
		    if(ptr != 0) memcpy(&s, ptr, sizeof(short));
		    fprintf(fd_out, "%d", s);
		    break;
		case SCR_STRING:
		case SCR_ZSTRING:
		    buf[0] = 0;
		    IS_get_vfield(is, i, &ptr);
		    if(ptr != 0) strcpy(buf, ptr);
		    fwrite(buf, strlen(buf), 1, fd_out);
		    break;
		case SCR_CHAR:
		    buf[0] = 0;
		    IS_get_vfield(is, i, &ptr);
		    if(ptr != 0) buf[0] = ptr[0];
		    fprintf(fd_out, "%c", buf[0]);
		    break;
		case SCR_FLOAT:
		    d = 0.0;
		    IS_get_vfield(is, i, &ptr);
		    if(ptr != 0) d = *(double *)ptr;
		    fprintf(fd_out, "%lf", (double)d);
		    break;
		case SCR_DOUBLE:
		    d = 0.0;
		    IS_get_vfield(is, i, &ptr);
		    if(ptr != 0) memcpy(&d, ptr, sizeof(double));
		    fprintf(fd_out, "%lf", d);
		    break;
	    }
	}
	else {
	    switch(isf->isf_type) {
		case SCR_DATE:
		case SCR_LONG:
		case SCR_TIME:
		case SCR_AUTO:
		    IS_get_field(is, i, (char *)&l);
		    fprintf(fd_out, "%ld", l);
		    break;
		case SCR_MENU:
		case SCR_SHORT:
		case SCR_TYPE_RADIO: /* BP_M 21-01-97 */
		case SCR_CHECK_BOX:  /* BP_M 21-01-97 */
		    IS_get_field(is, i, (char *)&s);
		    fprintf(fd_out, "%d", s);
		    break;
		case SCR_STRING:
		case SCR_ZSTRING:
		    /* BP_M 27-02-1998 */
		    buf2 = SCR_malloc(isf->isf_length * 2);
		    memset(buf2, 0, isf->isf_length);
		    IS_get_field(is, i, buf2);
		    if(isf->isf_type == SCR_ZSTRING)
			buf2[isf->isf_length - 1] = 0;
		    else
			buf2[isf->isf_length] = 0;

		    SCR_conv_txt(buf2);

		    fwrite(buf2, strlen(buf2), 1, fd_out);

		    SCR_free(buf2);

		    break;
		case SCR_CHAR:
		    IS_get_field(is, i, buf);
		    if(buf[0] == '\\') buf[0] = ' '; /* BP_M 25-10-2002 */
		    fprintf(fd_out, "%c", buf[0]);
		    break;
		case SCR_SHORT1:
		    IS_get_field(is, i, buf);
		    fprintf(fd_out, "%d", (short)buf[0]);
		    break;
		case SCR_FLOAT:
		    IS_get_field(is, i, (char *)&f);
		    fprintf(fd_out, "%lf", (double)f);
		    break;
		case SCR_DOUBLE:
		    IS_get_field(is, i, (char *)&d);
		    fprintf(fd_out, "%lf", d);
		    break;
	    }
	}
	if(i != is->is_nb_flds - 1) fprintf(fd_out, "|");
    }

    txt  = SCR_malloc(1000);
    buf2 = SCR_malloc(1000);
    len_txt = 1000;

    for(i = 0, cmp = is->is_cmps ; i < is->is_nb_cmps ; i++, cmp++) {
	for(j = 0 ; j < cmp->cmp_nb_flds ; j++) {
	    if(cmp->cmp_sqz[j] == SCR_SQZ ||
	       is->is_flds[cmp->cmp_flds[j]].isf_length != cmp->cmp_lgs[j]) {
		if(mfld == 0)
		    mfld = (int *)SCR_malloc(sizeof(int) * is->is_nb_flds);
		if(mfld[cmp->cmp_flds[j]]) continue;
		mfld[cmp->cmp_flds[j]] = 1;
		isf = is->is_flds + cmp->cmp_flds[j];
		if(isf->isf_length > len_txt) {
		    txt  = SCR_realloc(txt , sizeof(char), len_txt, isf->isf_length + 1000);
		    buf2 = SCR_realloc(buf2, sizeof(char), len_txt, isf->isf_length + 1000);
		    len_txt = isf->isf_length + 1000;
		}
		IS_get_field(is, cmp->cmp_flds[j], txt);
		IS_create_write_buf(is, txt, buf2, isf->isf_type, cmp->cmp_lgs[j], 1, 1);   /* BP_M 26-02-2003 13:41 */
		fprintf(fd_out, "|%s", buf2);
	    }
	}
    }

    SCR_free(txt);   /* JMP 07-06-05 */
    SCR_free(buf2);  /* JMP 07-06-05 */
    SCR_free(mfld);  /* JMP 07-06-05 */
    fprintf(fd_out, "\n");

    return(0);
}

SCR_conv_txt(U_ch *txt)
{
    int     i, j = 0;
    U_ch    *buf;

    i = strlen(txt);
    if(i == 0) return;
    buf = SCR_malloc(strlen(txt) * 2);
//    Debug("Len = %d - txt = '%s'\n", strlen(txt), txt);
    for(i = 0 ; txt[i] ; i++) {
	if(txt[i] < ' ' || txt[i] > 126 || txt[i] == '|' || txt[i] == '\\') /* BP_M 09-04-2007 */
	    buf[j++] = '\\';
	buf[j++] = txt[i];
    }
    buf[j] = 0;
    strcpy(txt, buf);
    SCR_free(buf);
}

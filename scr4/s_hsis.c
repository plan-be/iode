#include <scr4.h>

#ifndef UNIX
#include <io.h>
#endif

/* ===============================================================
	programme de gestion des historiques de transaction

   =============================================================== */

FILE    *fd_trans = 0;
int     flag_trans = 0;
char    *SCR_find_ptr_name();
extern char *SCR_transfile;

/* ==============================================================
    Ouverture du fichier des historiques de transaction
    pas de paramètre
   ============================================================== */

IS_init_history()               /* BP 08-10-91 10:42 */
{
    flag_trans = 1;
    return(0);
}

IS_open_trans_()        /* BP_M 01-03-2003 10:43 */
{
    char    buf[SCR_MAX_FILE_LG + 1];
    char    m_read[10], m_write[10];

    if(fd_trans != 0) return(-1);

#ifdef DOS
    strcpy(m_read, "rb+");
    strcpy(m_write, "wb+");
#else
    strcpy(m_read, "r+");
    strcpy(m_write, "w+");
#endif

    if(SCR_transfile == 0) {
	strcpy(buf, "scr.hsi");
	SCR_transfile = SCR_stracpy(buf);
    }
    if(ISC_access(SCR_transfile, 0) == 0)   /* BP_M 01-03-2003 10:41 */
	fd_trans = ISC_fopen(SCR_transfile, m_read);
    else
	fd_trans = ISC_fopen(SCR_transfile, m_write);

    if(fd_trans == 0) {
	Debug("HSI: Fichier '%s' inaccessible\n", SCR_transfile);
	return(-1);
    }
    ISC_fseek(fd_trans, 0L, 2);
    ISC_flock(fd_trans, -1);        /* BP_M 01-03-2003 10:43 */

    return(0);
}

/* ==============================================================
    Fermeture du fichier des historiques de transaction
    pas de paramètre
   ============================================================== */

IS_end_history()
{
    flag_trans = 0;
    return(0);
}

IS_close_trans_()
{
    if(fd_trans != 0) {
	ISC_funlock(fd_trans, -1);
	ISC_fclose(fd_trans);
    }
    fd_trans = 0;

    return(0);
}

/* ================================================================
	Ecriture d'un record dans le fichier des historiques
	IN : is : pointeur sur le fichier ISAM
   ================================================================ */

IS_trans_write(is)
ISAM    *is;
{
    short   i;
    char    buf[255];

    if(!is->is_hist) return(0);
    if(flag_trans == 0) return(0);

    if(IS_open_trans_()) return(-1);

    sprintf(buf, "&%s@W@", SCR_find_ptr_name(is));
    i = is->is_rec_len; /* JMP 17-10-02 */

    ISC_fwrite(buf, strlen(buf), 1, fd_trans);
    ISC_fwrite(&i, sizeof(short), 1, fd_trans);
    ISC_fwrite(is->is_rec, is->is_rec_len, 1, fd_trans);
    ISC_fprintf(fd_trans, "#");

    IS_close_trans_();

    return(0);
}

int SCR_HSIS_ALLREC = 0;
int SCR_CHECK_RREC = 1;

/* ================================================================
	Modification d'un record dans le fichier des historiques
	IN : is : pointeur sur le fichier ISAM
   ================================================================ */

IS_trans_rewrite(is)        /* BP_M 27-01-2000 */
ISAM    *is;
{
    short   i;
    short   j;
    ISFIELD *isf = is->is_flds;
    char    c;
    short   s;
    long    l;
    float   f;
    double  d;
    long    pos;
    static  char *buf1 = 0, *buf2 = 0;
    static  int  len_buf = 0;

    if(!is->is_hist) return(0);
    if(flag_trans == 0) return(0);

    if(SCR_CHECK_RREC)
	if(memcmp(is->is_rec, is->is_rrec, is->is_rec_len) == 0) return(0);

    if(IS_open_trans_()) return(-1);

    ISC_fprintf(fd_trans, "&%s@R@", SCR_find_ptr_name(is));
    j = is->is_rec_len;
    ISC_fwrite(&j, sizeof(short), 1, fd_trans);
    ISC_fwrite(is->is_rrec, is->is_rec_len, 1, fd_trans);

    if(buf1 == 0) {
	buf1 = SCR_malloc(5000);        /* BP_M 28-03-2000 */
	buf2 = SCR_malloc(5000);        /* BP_M 28-03-2000 */
	len_buf = 5000;
    }

    for(i = 0 ; i < is->is_nb_flds ; i++, isf++) {
	if(isf->isf_length > len_buf) {
	    buf1 = (char *)SCR_realloc(buf1, sizeof(char), len_buf, isf->isf_length + 1000);
	    buf2 = (char *)SCR_realloc(buf2, sizeof(char), len_buf, isf->isf_length + 1000);
	    len_buf = isf->isf_length + 1000;
	}
	IS_get_field(is, i, buf1);
	IS_get_read_field(is, i, buf2);
	if(SCR_HSIS_ALLREC == 0 && memcmp(buf1, buf2, isf->isf_length) == 0) continue;
	ISC_fwrite(&i, sizeof(short), 1, fd_trans);

	switch(isf->isf_type) {
	    case SCR_STRING  :
	    case SCR_ZSTRING :
	    case SCR_DATE    :
	    case SCR_TIME    :
	    case SCR_MENU    :
	    case SCR_CHECK_BOX:
	    case SCR_TYPE_RADIO:
		ISC_fwrite(is->is_rec + isf->isf_start, isf->isf_length, 1, fd_trans);
		break;

	    case SCR_CHAR    :
	    case SCR_SHORT1  :
		if(isf->isf_hsi_fix)
		    c = *(char *)(is->is_rec + isf->isf_start);
		else
		    c = *(char *)(is->is_rec + isf->isf_start) -
			*(char *)(is->is_rrec + isf->isf_start);

		ISC_fwrite(&c, 1, 1, fd_trans);
		break;
	    case SCR_SHORT   :
		if(isf->isf_hsi_fix)
		    s = *(short *)(is->is_rec + isf->isf_start);
		else
		    s = *(short *)(is->is_rec + isf->isf_start) -
			*(short *)(is->is_rrec + isf->isf_start);
		ISC_fwrite(&s, sizeof(short), 1, fd_trans);
		break;
	    case SCR_LONG    :
	    case SCR_AUTO    :
		if(isf->isf_hsi_fix)
		    l = *(long *)(is->is_rec + isf->isf_start);
		else
		    l = *(long *)(is->is_rec + isf->isf_start) -
			*(long *)(is->is_rrec + isf->isf_start);
		ISC_fwrite(&l, sizeof(long), 1, fd_trans);
		break;
	    case SCR_FLOAT   :
		if(isf->isf_hsi_fix)
		    f = *(float *)(is->is_rec + isf->isf_start);
		else
		    f = *(float *)(is->is_rec + isf->isf_start) -
			*(float *)(is->is_rrec + isf->isf_start);
		ISC_fwrite(&f, sizeof(float), 1, fd_trans);
		break;
	    case SCR_DOUBLE  :
		if(isf->isf_hsi_fix)
		    d = *(double *)(is->is_rec + isf->isf_start);
		else
		    d = *(double *)(is->is_rec + isf->isf_start) -
			*(double *)(is->is_rrec + isf->isf_start);
		ISC_fwrite(&d, sizeof(double), 1, fd_trans);
		break;
	}
    }
    i = -1;
    ISC_fwrite(&i, sizeof(short), 1, fd_trans);
    ISC_fprintf(fd_trans, "#");

    IS_close_trans_();

    return(0);
}

/* ================================================================
	Destruction d'un record dans le fichier des historiques
	IN : is : pointeur sur le fichier ISAM
   ================================================================ */

IS_trans_delete(is)
ISAM    *is;
{
    short   i;

    if(!is->is_hist) return(0);
    if(flag_trans == 0) return(0);

    if(IS_open_trans_()) return(-1);

    ISC_fprintf(fd_trans, "&%s@D@", SCR_find_ptr_name(is));
    i = is->is_rec_len;
    ISC_fwrite(&i, sizeof(short), 1, fd_trans);
    ISC_fwrite(is->is_rrec, i, 1, fd_trans);
    ISC_fprintf(fd_trans, "#");

    IS_close_trans_();

    return(0);
}

/* ================================================================
	Exécution d'une requête sql sans isam
	IN : sql : requête sql
   ================================================================ */

IS_trans_sql_nois(char *sql)
{
    short   i;      /* BP_M 01-03-2003 10:14 */

    if(flag_trans == 0) return(0);

    if(IS_open_trans_()) return(-1);

    ISC_fprintf(fd_trans, "&_sqlnois_+@");
    i = strlen(sql);
    ISC_fwrite(&i, sizeof(short), 1, fd_trans);
    ISC_fwrite(sql, i, 1, fd_trans);
    ISC_fprintf(fd_trans, "#");

    IS_close_trans_();

    return(0);
}

/* ================================================================
	Creation d'une table
	IN : is : structure ISAM
   ================================================================ */

IS_trans_create_db(ISAM *is)
{
    if(!is->is_hist) return(0);
    if(flag_trans == 0) return(0);

    if(IS_open_trans_()) return(-1);

    ISC_fprintf(fd_trans, "&%s@C@", SCR_find_ptr_name(is));
    ISC_fprintf(fd_trans, "#");

    IS_close_trans_();

    return(0);
}


/* ================================================================
	Destruction d'une table
	IN : is : structure ISAM
   ================================================================ */

IS_trans_delete_db(ISAM *is)
{
    if(!is->is_hist) return(0);
    if(flag_trans == 0) return(0);

    if(IS_open_trans_()) return(-1);

    ISC_fprintf(fd_trans, "&%s@E@", SCR_find_ptr_name(is));
    ISC_fprintf(fd_trans, "#");

    IS_close_trans_();

    return(0);
}

/* ================================================================
	Set un nouveau nom à une table
	IN : is : structure ISAM
	IN : name : char * nouveau nom
   ================================================================ */

IS_trans_set_name_db(ISAM *is, char *name)
{
    int     i;

    if(!is->is_hist) return(0);
    if(flag_trans == 0) return(0);

    if(IS_open_trans_()) return(-1);

    ISC_fprintf(fd_trans, "&%s@N@%s@", SCR_find_ptr_name(is), name);
    ISC_fprintf(fd_trans, "#");

    IS_close_trans_();

    return(0);
}


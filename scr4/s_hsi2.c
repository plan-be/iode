#include <scr4.h>

char    *HSI_read_name();
int     HSI_NO_SET_NAME = 0;    /* BP_M 02-02-2004 15:22 */

SCR_load_history(file_hsi, flag)    /* BP_M 27-01-2000 */
char    *file_hsi;
int     flag;
{
    FILE    *fd_trans;
    int     c, i, type;
    short   len, fld, len_sql = 0;
    char    *name, *buf, msg[1048], *sql = 0, bufn[256];
    ISAM    *is;
    FILE_TYPE   *flt, **flt_ptr;
    COMPOSITE   *cmp;
    ISFIELD *isf;
    int     nb_w = 0, nb_rw = 0, nb_d = 0, nb_nois = 0, nb_c = 0, nb_e = 0, nb_sn = 0, nb_op = 0, flagrw = 0, nb_err = 0;
    /* BP_M 18-03-1999 */
    extern int SCR_NO_REREAD;
    int     tmp_noreread;
    char    *bufrw;

#ifdef DOS
    fd_trans = fopen(file_hsi, "rb");
#else
    fd_trans = fopen(file_hsi, "r");
#endif

    if(fd_trans == 0) {
	sprintf(msg, "SCR4_hsi: cannot open file %s", file_hsi);
	Debug("%s\n", msg);
	if(flag == 1)
	    fprintf(stderr, "%s\n", msg);
	if(flag == 2)
	    PG_display_error(msg);
	return(-2);
    }

    sprintf(msg, "SCR4_HSI: start of information extract for file %s\n", file_hsi);
    Debug(msg);
    if(flag == 1) fprintf(stderr, msg);

    /* BP_M 18-03-1999 */
    tmp_noreread = SCR_NO_REREAD;
    SCR_NO_REREAD = 1;

    bufrw = SCR_malloc(10000);

    flt_ptr = SCR_READ_FILES.tbl_fl;

    for(i = 0 ; i < SCR_READ_FILES.tbl_nb_fl ; i++)
	if(flt_ptr[i]->fl_name != 0)
	    SCR_lower(flt_ptr[i]->fl_name);

    while(!feof(fd_trans)) {
	c = fgetc(fd_trans);
	if(c == EOF) break;
	if(c != '&') {
//          Debug("Erreur de structure dans fichier historique\n");
	    nb_err++;
	    continue;
	}

	name = HSI_read_name(fd_trans);
	if(name == (char *)0) break;

	/* SQL NOIS */
	if(strcmp(name, "_sqlnois_+") == 0) { /* sql nois */
	    fread(&len, sizeof(short), 1, fd_trans);

	    if(len > len_sql) {
		if(sql == 0) sql = SCR_malloc(len + 100);
		else
		    sql = SCR_realloc(sql, sizeof(char), len_sql, len + 100);
		len_sql = len + 100;
	    }

	    fread(sql, sizeof(char), len, fd_trans);
	    IS_exec_sql_nois(sql);
	    nb_nois++; nb_op++;
	    goto endop;
	}


	type = fgetc(fd_trans);
	if(fgetc(fd_trans) != '@') continue;

	SCR_lower(name);
	flt = SCR_find_flt(name);
	if(flt == 0) {
	    HSI_read_to_next(fd_trans, type);
	    nb_err++;
	    continue;
	}
	if(flt->fl_type != SCR_TYPE_ISAM - SCR_TYPE_PAGE) {
	    sprintf(msg, "SCR4_hsi: bad object type\n");
	    Debug(msg);
	    if(flag == 1)
		fprintf(stderr, msg);
	    HSI_read_to_next(fd_trans, type);
	    nb_err++;
	}
	is = (ISAM *)flt->fl_ptr;
	if(is == 0) {
	    sprintf(msg, "SCR4_hsi: isam %s:'%s' not defined\n", name, flt->fl_name);
	    Debug(msg);
	    if(flag == 1)
		fprintf(stderr, msg);
	    HSI_read_to_next(fd_trans, type);
	    continue;
	    nb_err++;
	}

	if(type != 'C' && type != 'E' && type != 'N') {
	    for(i = 0 ; i < is->is_nb_cmps ; i++)
		if(is->is_cmps[i].cmp_dup == 1) break;
	    if(i == is->is_nb_cmps)
		i = 0;

	    fread(&len, sizeof(short), 1, fd_trans);
	    if(len != is->is_rec_len) {
		sprintf(msg, "SCR4_hsi: isam %s bad len '%d != %d'\n", name, len, is->is_rec_len);
		Debug(msg);
		if(flag == 1)
		    fprintf(stderr, msg);
		buf = SCR_malloc(len);
		fread(buf, len, 1, fd_trans);
		if(type == 'R')
		    fread(buf, len, 1, fd_trans);
		fgetc(fd_trans);
		nb_err++;
		continue;
	    }
	}

	switch(type) {
	    case 'W':
		fread(is->is_rec, len, 1, fd_trans);
		if(HSI_open(is) != 0 || IS_write(is) != 0) {
		    sprintf(msg, "SCR4_hsi: bad write in isam %s: msg %s\n", name, IS_error_text());
		    Debug(msg);
		    if(flag == 1)
			fprintf(stderr, msg);
		}
		else
writeok:
		    nb_w++;
		break;

	    case 'R':
		isf = is->is_flds;
		fread(is->is_rec, len, 1, fd_trans);
		if(HSI_open(is) != 0) {
		    sprintf(msg, "SCR4_hsi: Cannot open ISAM %s\n", name);
		    Debug(msg);
		    if(flag == 1)
			fprintf(stderr, msg);
		    fread(is->is_rec, len, 1, fd_trans);
		    fgetc(fd_trans);
		    continue;
		}
		flagrw = 0;
		if(IS_search(is, i, SCR_EQ, (char *)0) != 0) {
		    sprintf(msg, "SCR4_hsi: record not found in file %s\n", name);
		    Debug(msg);
		}
		else
		    flagrw = 1;
		while(1) {
		    fread(&fld, sizeof(short), 1, fd_trans);
		    if(fld == -1 || fld > is->is_nb_flds - 1) break;
		    isf = is->is_flds + fld;
		    fread(bufrw, isf->isf_length, 1, fd_trans);
		    switch(isf->isf_type) {
			case SCR_STRING  :
			case SCR_ZSTRING :
			case SCR_DATE    :
			case SCR_TIME    :
			case SCR_MENU    :
			case SCR_CHECK_BOX:
			case SCR_TYPE_RADIO:

			    memcpy(is->is_rec + isf->isf_start,
				   bufrw,
				   isf->isf_length);
			    break;

			case SCR_CHAR    :
			case SCR_SHORT1  :
			    if(isf->isf_hsi_fix)
				*(char *)(is->is_rec + isf->isf_start) =
				*(char *)(bufrw);
			    else
				*(char *)(is->is_rec + isf->isf_start) +=
				*(char *)(bufrw);
			    break;
			case SCR_SHORT   :
			    if(isf->isf_hsi_fix)
				*(short *)(is->is_rec + isf->isf_start) =
				*(short *)(bufrw);
			    else
				*(short *)(is->is_rec + isf->isf_start) +=
				*(short *)(bufrw);
			    break;
			case SCR_LONG    :
			case SCR_AUTO    :
			    if(isf->isf_hsi_fix)
				*(long *)(is->is_rec + isf->isf_start) =
				*(long *)(bufrw);
			    else
				*(long *)(is->is_rec + isf->isf_start) +=
				*(long *)(bufrw);
			    break;
			case SCR_FLOAT   :
			    if(isf->isf_hsi_fix)
				*(float *)(is->is_rec + isf->isf_start) =
				*(float *)(bufrw);
			    else
				*(float *)(is->is_rec + isf->isf_start) +=
				*(float *)(bufrw);
			    break;
			case SCR_DOUBLE  :
			    if(isf->isf_hsi_fix)
				*(double *)(is->is_rec + isf->isf_start) =
				*(double *)(bufrw);
			    else
				*(double *)(is->is_rec + isf->isf_start) +=
				*(double *)(bufrw);
			    break;
		    }
		}
		if(flagrw) {
		    if(IS_rewrite(is) != 0) {
			sprintf(msg, "SCR4_hsi: bad rewrite in isam %s: msg %s\n", name, IS_error_text());
			Debug(msg);
			if(flag == 1)
			    fprintf(stderr, msg);
		    }
		    else
			nb_rw++;
		}
		else {
		    if(IS_write(is) != 0) {
			sprintf(msg, "SCR4_hsi: bad write in isam %s: msg %s\n", name, IS_error_text());
			Debug(msg);
			if(flag == 1)
			    fprintf(stderr, msg);
		    }
		    else
			nb_w++;
		}
		break;

	    case 'D':
		fread(is->is_rec, len, 1, fd_trans);
		if(HSI_open(is) != 0 || IS_search(is, i, SCR_EQ, (char *)0) != 0) {
		    sprintf(msg, "SCR4_hsi: record not found in file %s\n", name);
		    Debug(msg);
		    if(flag == 1)
			fprintf(stderr, msg);
		    fgetc(fd_trans);
		    continue;
		}
		if(IS_delete(is) != 0) {
		    sprintf(msg, "SCR4_hsi: bad delete in isam %s: msg %s\n", name, IS_error_text());
		    Debug(msg);
		    if(flag == 1)
			fprintf(stderr, msg);
		}
		else
		    nb_d++;
		break;

	    case 'C':  /* Creation de table */
		if(IS_create_db(is) == 0)
		    nb_c++;
		break;

	    case 'E':  /* Erase table */
		if(IS_delete_db(is) == 0)
		    nb_e++;
		break;

	    case 'N':  /* set name db */
		i = 0;
		while(!feof(fd_trans)) {
		    c = fgetc(fd_trans);
		    if(c == EOF) break;
		    if(c == '@') {
			bufn[i] = 0;
			break;
		    }
		    bufn[i++] = c;
		}
		if(bufn[i] == 0 && HSI_NO_SET_NAME == 0) {  /* BP_M 02-02-2004 15:23 */
		    IS_set_name_db(is, bufn);
		    nb_sn++;
		}
		break;

	    default:
		sprintf(msg, "SCR4_hsi: bad modifie in file %s\n", name);
		Debug(msg);
		if(flag == 1)
		    fprintf(stderr, msg);
		continue;
	}
	IS_close(is);
	nb_op++;
endop:
	while(1) {
	    c = fgetc(fd_trans);
	    if(c == EOF) goto ended;
	    if(c != '#')
		continue;
	    else
		break;
	}
    }
ended:

    fclose(fd_trans);

    sprintf(msg, "SCR4_HSI: end of extract informations of file %s\n", file_hsi);
    Debug(msg);
    if(flag == 1)
	fprintf(stderr, msg);

    sprintf(msg, "SCR4_HSI: NB_OP=%d, NB_W=%d, NB_RW=%d, NB_D=%d, NB_NOIS=%d, NB_CTBL=%d, NB_DTBL=%d, NB_SN=%d\n",
			nb_op, nb_w, nb_rw, nb_d, nb_nois, nb_c, nb_e, nb_sn);
    Debug(msg);
    if(flag == 1)
	fprintf(stderr, msg);

    SCR_NO_REREAD = tmp_noreread;   /* BP_M 18-03-1999 */
    SCR_free(bufrw);

    if(nb_op != nb_w + nb_rw + nb_d + nb_nois + nb_c + nb_e + nb_sn)
	return(-1);

    return(0);
}

char *HSI_read_name(fd)
FILE    *fd;
{
    static char    name[SCR_MAX_FILE_LG + 1];
    int     c, i = 0;

    while(!feof(fd)) {
	c = fgetc(fd);
	if(c == '@') {
	    name[i] = 0;
	    return(name);
	}
	name[i] = c;
	i++;
	if(i == SCR_MAX_FILE_LG) return((char *)0); /* BP_M 18-03-1999 */
    }
    return((char *)0);   /* JMP 06-02-97 */
}

HSI_open(is)
ISAM    *is;
{
    return(IS_open(is, SCR_UPDATE, SCR_NO_LOCK));
}


HSI_read_to_next(fd, type)
FILE    *fd;
int     type;
{
    short   len;
    char    *buf;

    fread(&len, sizeof(short), 1, fd);
    buf = SCR_malloc(len);
    fread(buf, len, 1, fd);
    if(type == 'R')
	fread(buf, len, 1, fd);
    fgetc(fd);
    SCR_free(buf);
    return(0);
}

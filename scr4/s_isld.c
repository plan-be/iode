#include "scr.h"

extern int SCR_ISLD_QUIET;      /* BP_M 06-01-2000 */


/* ======================================================================
Importe un fichier ASCII et place les records lus dans l'ISAM is.
L'ISAM doit exister. Le fichier ascii est en principe construit à l'aide de
la fonction IS_dump_rec(). Chaque ligne contient un record. Chaque champ est
séparé du suivant par un pipe (|).

Cette fonction envoie des messages dans le standard output. Elle est
utilisée par scr4 avec l'option -load.

&RT -1 si un des fichiers ne peut être ouvert. 0 sinon.
&SA IS_dump_rec()
------------------------------------------------------------------------- */
long IS_load_rec(is, filename)       /* BP_M 06-01-2000 */
ISAM    *is;
char    *filename;
{
    int     i, reclen, c;
    ISFIELD *isf;
    long    bufl, nb_read = 0;
#ifndef WATCOM
    long    atol();
    double  SCR_atof();
#endif
    float   buff;
    short   bufs;
    double  bufd;
    unsigned char    *buf; /* JMP 10-04-00 */
    FILE    *fd;
    extern  int     CT_SYNC;
    int     ctsync_tmp;
    int     quit = 1;
    char    *ptr;
    short   len;

    ctsync_tmp = CT_SYNC;
    CT_SYNC = 0;
    SCR_REPOS_AFTER_WRITE = 0;

    if(IS_open(is, SCR_UPDATE, SCR_NO_LOCK) != 0) {
	if(!SCR_ISLD_QUIET)
	    fprintf(stderr, "scr4 load: %s\n", IS_error_text());
	return(-1);
    }
    fd = fopen(filename, "r");
    if(fd == 0) {
	if(!SCR_ISLD_QUIET)
	    fprintf(stderr, "scr4 load: cannot access file %s\n", filename);
	return(-1);
    }

    reclen = is->is_rec_len + 50; /* JMP 10-04-00 */
    if(reclen < 1024) reclen = 1024; /* JMP 10-04-00 */
    buf = SCR_malloc(reclen + 1); /* JMP 10-04-00 */
    while(quit) {
	IS_empty_rec(is);
	for(i = 0, isf = is->is_flds ; i < is->is_nb_flds ; i++, isf++) {
	    if(isf->isf_var) {
		switch(isf->isf_type) {
		    case SCR_LONG:
		    case SCR_TIME:
		    case SCR_DATE:
		    case SCR_AUTO:
		    case SCR_REF:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			bufl = atol(buf);
			IS_set_vfield(is, i, (char *)&bufl, (long)sizeof(long));
			break;
		    case SCR_STRING:
		    case SCR_ZSTRING:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			IS_set_vfield(is, i, buf, (long)strlen(buf));
			break;
		    case SCR_SHORT:
		    case SCR_MENU:
		    case SCR_TYPE_RADIO: /* BP_M 21-01-97 */
		    case SCR_CHECK_BOX:  /* BP_M 21-01-97 */
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			bufs = atol(buf);
			IS_set_vfield(is, i, (char *)&bufs, (long)sizeof(short));
			break;
		    case SCR_FLOAT:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			buff = SCR_atof(buf);
			IS_set_vfield(is, i, (char *)&buff, (long)sizeof(float));
			break;
		    case SCR_DOUBLE:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			bufd = SCR_atof(buf);
			IS_set_vfield(is, i, (char *)&bufd, (long)sizeof(double));
			break;
		    case SCR_SHORT1:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			buf[0] = atoi(buf);
			IS_set_vfield(is, i, buf, 1L);
			break;
		    case SCR_CHAR:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			IS_set_vfield(is, i, buf, 1L);
			break;
		    case SCR_EDITOR:
			len = LA_len_next(fd) + 1;
			if(len > 0) {
			    ptr = SCR_malloc(len);
			    quit = LA_read(fd, (int)len, ptr);
			    LA_cnv_txt(ptr);
			    IS_set_vfield(is, i, ptr, (long)len);
			    SCR_free(ptr);
			}
			break;

		    default:
			if(!SCR_ISLD_QUIET)
			    fprintf(stderr, "l_asc: incompatible type %d\n", isf->isf_type);
			break;
		}
	    }
	    else {
		switch(isf->isf_type) {
		    case SCR_LONG:
		    case SCR_TIME:
		    case SCR_DATE:
		    case SCR_AUTO:
		    case SCR_REF:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			bufl = atol(buf);
			IS_set_field(is, i, (char *)&bufl);
			break;
		    case SCR_STRING:
		    case SCR_ZSTRING:
			quit = LA_read(fd, isf->isf_length, buf);
			if(quit == 0) goto ended;
			SCR_replace(buf, "\\", "");
			IS_set_field(is, i, buf);
			break;
		    case SCR_SHORT:
		    case SCR_MENU:
		    case SCR_TYPE_RADIO: /* BP_M 21-01-97 */
		    case SCR_CHECK_BOX:  /* BP_M 21-01-97 */
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			bufs = atol(buf);
			IS_set_field(is, i, (char *)&bufs);
			break;
		    case SCR_FLOAT:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			buff = SCR_atof(buf);
			IS_set_field(is, i, (char *)&buff);
			break;
		    case SCR_DOUBLE:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			bufd = SCR_atof(buf);
			IS_set_field(is, i, (char *)&bufd);
			break;
		    case SCR_SHORT1:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			buf[0] = atoi(buf);
			IS_set_field(is, i, buf);
			break;
		    case SCR_CHAR:
			quit = LA_read(fd, reclen, buf);
			if(quit == 0) goto ended;
			IS_set_field(is, i, buf);
			break;
		    default:
			if(!SCR_ISLD_QUIET)
			    fprintf(stderr, "l_asc: incompatible type %d\n", isf->isf_type);
			break;
		}
	    }
	    if(quit == 2) break;
	}
	if(quit != 2) {
	    while(!feof(fd)) {
		c = fgetc(fd);
		if(c == EOF) break;
		if(c == '\n') break;
	    }
	}

	if(IS_write(is) != 0) {
	    if(!SCR_ISLD_QUIET)
		fprintf(stderr, "\nscr4 load: %s\n", IS_error_text());
	}
	else {
	    if(!SCR_ISLD_QUIET)
		fprintf(stderr, "scr4 load nb: = %ld\r", ++nb_read);
	}

	if(feof(fd)) break;
	quit = 1;
    }
ended:
    IS_close(is);
    fclose(fd); /* BP_M 06-01-2000 */
    fprintf(stderr, "\n\nEnded\n");
    CT_SYNC = ctsync_tmp;

    SCR_free(buf); /* JMP 10-04-00 */
    return(nb_read);
}

/*NH*/
LA_read(fd, len, buf)       /* BP_M 20-08-1997 */
FILE    *fd;
int     len;
char    *buf;
{
    int     c;
    U_int     i;
    int     ret = 0, flag = 0;

    for(i = 0 ;  ; i++) {
	c = fgetc(fd);
	switch(c) {
	    case '\"':
		if(flag) flag = 0;
		else     flag = 1;
		break;
	    case EOF:
		ret = 3;
		break;
	    case '|':
		ret = 1;
		break;
	    case '\n':
	    case '\r':
		if(flag) break;
		ret = 2;
		break;
	    default:
		if(c == '\\')   /* BP_M 09-04-2007 */
		    c = fgetc(fd);
		if(i < len) buf[i] = c; /* JMP 10-04-00 */
		break;
	}
	if(ret) break;
    }
    if(ret == 3) ret = 0;
    buf[i] = 0;
    return(ret);
}
/*NH*/
IS_load_rec_interne(is, filename)       /* BP_M 17-06-1993 11:23 */
ISAM    *is;
char    *filename;
{
    int     i;
    ISFIELD *isf;
    long    bufl, nb_read = 0;
#ifndef WATCOM
    long    atol();
    double  SCR_atof();
#endif
    float   buff;
    short   bufs;
    double  bufd;
    unsigned char    buf[500];
    FILE    *fd;
    extern  int     CT_SYNC;
    int     ctsync_tmp;
    int     quit = 1;

    ctsync_tmp = CT_SYNC;
    CT_SYNC = 0;

    if(IS_open(is, SCR_UPDATE, SCR_NO_LOCK) != 0) {
	if(!SCR_ISLD_QUIET)
	    fprintf(stderr, "scr4 load: %s\n", IS_error_text());
	return(-1);
    }
    fd = fopen(filename, "rb");
    if(fd == 0) {
	if(!SCR_ISLD_QUIET)
	    fprintf(stderr, "scr4 load: cannot access file %s\n", filename);
	return(-1);
    }

    while(!feof(fd)) {
	if(fread(is->is_rec, is->is_rec_len, 1, fd) != 1) break;    /* BP_M 05-07-2007 */

	if(IS_write(is) != 0) {
	    if(!SCR_ISLD_QUIET)
		fprintf(stderr, "\n%s\n", IS_error_text());
	}
	else {
	    nb_read++;
	    if(!SCR_ISLD_QUIET)
		fprintf(stderr, "%ld\r", nb_read);
	}

	if(feof(fd)) break;
	quit = 1;
    }

    IS_close(is);

    CT_SYNC = ctsync_tmp;
    fclose(fd);
    if(!SCR_ISLD_QUIET)
	fprintf(stderr, "'%ld' record loaded\n", nb_read);

    return(0);
}

/*NH*/
LA_len_next(fd)     /* BP_M 26-10-95 */
FILE    *fd;
{
    long        pos, ftell();
    int         i = 0, c;

    pos = ftell(fd);
    while(1) {
	c = fgetc(fd);
	if(c == '|' || c == '\n' || c == '\r' || c == EOF) break;
	i++;
    }
#ifdef INTAU
    fseek(fd, pos, 0);
#else
/*    fseek(fd, pos, SEEK_SET); /* JMP 17-01-96 oiur XENIX */
    fseek(fd, pos, 0 /*SEEK_SET */); /* JMP 17-01-96 */
#endif
    return(i);
}

/*NH*/
LA_cnv_txt(txt)       /* BP_M 26-10-95 */
char    *txt;
{
    while(*txt) {
	if(*txt == '~') *txt = '\n';
	txt++;
    }
    return(0);
}


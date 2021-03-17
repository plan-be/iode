#include "scr4.h"

/********* Gestion des arguments (argv et POST) *************/

long    SCR_WEB_SESSION = -1;
char    *SCR_WEB_IP;
U_ch    **SCR_WEB_PARMS = 0;
U_ch    **SCR_WEB_ARGS = 0;
U_ch    *SCR_WEB_CGI = 0;
int     SCR_WEB_NB_ARGS = 0;
int     SCR_WEB_DEBUG_LEVEL = 3;
char    *SCR_WEB_MSG;
int     SCR_WEB_NOHEADER = 0;
char    SCR_WEB_PATH[255];
char    *SCR_WEB_LANGUAGE = 0; /* BP_M 14-02-2000 */
char    SCR_WEB_BCMD[] = "<!-- START:%s -->";
char    SCR_WEB_ECMD[] = "<!-- END:%s -->";
char    SCR_WEB_TRDFILE[255];                   /* BP_M 09-11-2002 16:16 */
int     SCR_WEB_TRDLANG;                        /* BP_M 09-11-2002 16:17 */
char    SCR_WEB_OUTVAL[4096];  /* BP_M 18-11-2002 10:38 */

char    SCR_WEB_BSCR[] = "<!-- ";
char    SCR_WEB_ESCR[] = " -->";


char *ScrWebTxt2Html();                         /* BP_M 09-11-2002 17:02 */
FILE    *fd_trd;            /* BP_M 13-12-2002 12:34 */


U_ch *ScrWebGetCgiArg()
{
    char    *getenv();
    U_ch   *ptr;
    int     lg;

    ptr = (U_ch *) getenv("SCR_WEB_PARAM"); /* JMP 08-10-01 */
    if(ptr == 0) {
	ptr = (U_ch *) getenv("CONTENT_LENGTH");
	if(ptr != 0)
	    lg = atol(ptr);
	else {
	    Debug("CONTENT_LENGTH is NULL !! \n");
	    return((U_ch *)0);
	}
	ptr = (U_ch *)SCR_malloc(lg + 2);
	read(0, ptr, lg);
	}
    ptr[lg] = 0;
    Debug("CONTENT_LENGTH=%ld - txt = '%s'\n",  lg, ptr);
//  ScrWebDecodeArg(ptr);   /* BP_M 11-10-2011 10:38 */
    return(ptr);
}

/*NH*/
ScrWebDecodeArg(ptr)
U_ch   *ptr;
{
    int     i, j, ch, lg;

    lg = strlen(ptr);
    for(i = j = 0 ; i < lg ; i++) {
	ch = ptr[i];
	if(ch == '+') ch = ' ';
	else if(ch == '%') {
	    ch = 16 * ScrWebHexToDec(ptr[i + 1]) + ScrWebHexToDec(ptr[i + 2]);
	    i += 2;
	    }
	ptr[j++] = ch;
	}

    ptr[j] = 0;
//    SCR_AnsiToOem(ptr, ptr);
}

/*NH*/
ScrWebHexToDec(ch)
int     ch;
{
    ch = SCR_upper_char(ch);
    return(U_pos(ch, "0123456789ABCDEF"));
}

U_ch *ScrWebCgiFind(arg)
U_ch *arg;
{
    int     i, len;

    if(SCR_WEB_PARMS == 0) return((U_ch *)"");
    len = strlen(arg);
    for(i = 0 ; SCR_WEB_PARMS[i] ; i++)
	if(SCR_cstrncmp(arg, SCR_WEB_PARMS[i], len) == 0 && SCR_WEB_PARMS[i][len] == '=') {
	    return(SCR_WEB_PARMS[i] + len + 1);
	}

    return((U_ch *)"");
}

U_ch *ScrWebGetTextVParm(char *prefx, int n)
{
    char    buf[128], *ptr;

    sprintf(buf, "%s%d", prefx, n);
    return(ScrWebCgiFind(buf));
}

ScrWebGetNumVParm(char *prefx, int n)
{
    char    *ptr;

    ptr = ScrWebGetTextVParm(prefx, n);
    if(ptr) return(atol(ptr));
    return(-1);
}


ScrWebInit(char **argv)
{
    U_ch    *parms, *buf;
    int     pos, i;

    SCR_WEB_CGI = argv[0];
    parms = ScrWebGetCgiArg();
    Debug("ALL PARMS='%s'\n", parms);
    SCR_WEB_IP = getenv("REMOTE_ADDR");
    Debug("SCR_WEB_IP='%s'\n", SCR_WEB_IP);
    if(parms) {
	SCR_WEB_PARMS = SCR_vtom(parms, '&');
	SCR_free(parms);
    }
    SCR_WEB_ARGS  = SCR_vtoms(argv[1], "?");
    SCR_WEB_NB_ARGS = SCR_tbl_size(SCR_WEB_ARGS);
    buf = ScrWebCgiFind("SESSION");
    SCR_WEB_SESSION = -1;
    if(buf) SCR_WEB_SESSION = atol(buf);
    if(SCR_WEB_SESSION <= 0) {
	for(i = 0 ; argv[i] ; i++) {
	    pos = U_index(argv[i], "SESSION:");
	    if(pos >= 0) {
		SCR_WEB_SESSION = atol(argv[i] + pos + 8);
		// Debug("Sess:%s = %d\n",argv[i] + pos + 8, SCR_WEB_SESSION);
		}
	    }
	}

    if(SCR_WEB_PARMS)
	for(i = 0 ; SCR_WEB_PARMS[i] ; i++)
	    ScrWebDecodeArg(SCR_WEB_PARMS[i]);

    return(0);
}

ScrWebEnd()
{
    SCR_WEB_CGI = 0;
    SCR_free_tbl(SCR_WEB_PARMS);
    SCR_free_tbl(SCR_WEB_ARGS);
    SCR_WEB_PARMS = 0;
    SCR_WEB_ARGS = 0;
    SCR_WEB_NB_ARGS = 0;
    return(0);
}


ScrWebDumpHtml(file, fn)
char    *file;
int     (*fn)();
{
    return(ScrWebPgToHtmlEx(file, 0L, 0L, 0L, fn));
}

ScrWebDumpHtmlPart(file, part, fn)
char    *file, *part;
int     (*fn)();
{
    return(ScrWebPgToHtmlEx(file, part, 0L, 0L, fn));
}

ScrWebPgToHtmlEx(file, part, pg, fd_out, fn)
char    *file, *part;
PAGE    *pg;
FILE    *fd_out;
int     (*fn)();
{
    FILE    *fd_in;
    int     c, i, j, strip, rex;
    char    topic[5000], *val_trd;
    char    filename[255];
    char    stag[4096];



    if(fd_out == 0) fd_out = stdout;

    if(SCR_WEB_PATH[0] > ' ')
	sprintf(filename, "%s%c%s", SCR_WEB_PATH, DIR_CHAR, file);
    else
	sprintf(filename, file);

    if(SCR_WEB_LANGUAGE) {
	strcat(filename, ".");
	strcat(filename, SCR_WEB_LANGUAGE);
    }

    if(strlen(filename) > 4 && strcmp(filename + strlen(filename) - 4, ".htm") != 0)
	strcat(filename, ".htm");

#ifdef LINUX
    SCR_lower(filename);
#endif
    Debug("ScrWebPgToHtmlEx(%s, %s)\n", filename, part);

    fd_in = fopen(filename, "r"); /* JMP 15-11-01 */
    if(fd_in == 0) {
	Debug("ScrWebPgToHtmlEx: Error: fichier '%s' non trouv‚\n", filename);
	return(-1);
    }

    if(!SCR_WEB_NOHEADER) fprintf(fd_out, "Content-type: text/html\n\n<HTML>\n"); /* JMP 05-11-01 */

    if(part) {
	sprintf(stag, SCR_WEB_BCMD, part);
	Debug("Recherche de : '%s'\n", stag);
	j = 0;
	while(!feof(fd_in)) {
	    c = fgetc(fd_in);
	    if(c == EOF) goto ended;
	    if(c == stag[0]) {
		j = 1;
		while(1) {
		    c = fgetc(fd_in);
		    if(c == EOF) goto ended;
		    if(stag[j] != c)
			break;
		    j++;
		    if(j >= strlen(stag)) break;
		}
		if(stag[j] == 0)
		    break;
	    }
	}
	if(stag[j] != 0) {
	    Debug("File '%s' section '%s' non trouv‚e\n", filename, part);
	    return(-1);
	}
	Debug("'%s' dans '%s' trouvé\n", part, filename);
    }
    if(SCR_WEB_TRDFILE[0] > ' ') {
	fd_trd = fopen(SCR_WEB_TRDFILE, "r");
	if(fd_trd == 0)
	    Debug("Fichier '%s' non trouv‚", SCR_WEB_TRDFILE);
    }
    else
	fd_trd = 0;

    if(part) sprintf(stag, SCR_WEB_ECMD, part);

    j = 0;
    while(!feof(fd_in)) {
	c = fgetc(fd_in);
	if(c == EOF) break;

	if(part && c == '<') {
	    j = 1;
	    while(1) {
		c = fgetc(fd_in);
		if(c == EOF) goto ended;
		if(stag[j] != c) break;
		j++;
	    }
	    if(stag[j] == 0) break;
	    else
		fwrite(stag, j, 1, fd_out);
	}

	if(c != '$') {
	    fputc(c, fd_out);
	    continue;
	}
	strip = c;

	i = 0;
	rex = 0;
	SCR_WEB_OUTVAL[0] = 0;
	while(!feof(fd_in)) {
	    c = fgetc(fd_in);
	    if(c == EOF) break;
	    if(U_is_in(c, " #$\r\n\t,;:/\\&\"'()!-?=+[]<>{}.") == 1 || c > 126) {
		if(c == '.' && i < 2) {
		    topic[i++] = c;
		    continue;
		}
		else {
		    if(c == '$' && rex == 0) {
			rex = 1;
			continue;
		    }
		    ungetc(c, fd_in);
		    break;
		}
	    }
	    topic[i++] = c;
	}
	topic[i] = 0;
	if(i == 0) {
	    fputc(strip, fd_out);
	    continue;
	}
	if(topic[0] == '.') {
	    if(strlen(topic) > 3 && strncmp(topic, ".IF.", 4) == 0)
		ScrWebFldIsToHtml(fd_out, pg, topic + 4);
	    else
		ScrWebFldPgToHtml(fd_out, pg, topic + 1);
	    continue;
	}
ag:
	i = 1;
	if(pg) {
//          Debug("sts32: pass6 topic = '%s'\n", topic);
	    i = ScrWebPgFldToHtml(pg, topic, strip, fd_out, !rex);  /* BP_M 18-11-2002 11:29 */
//          Debug("sts32: pass6.1\n");
	    if(i == 0) {
		if(rex) {
		    rex = 0;
		    strcpy(topic, SCR_WEB_OUTVAL);
		    SCR_WEB_OUTVAL[0] = 0;
		    goto ag;
		}
		continue;
	    }
	}
	if(i != 0 && fn) {
	    i = (*fn)(fd_in, topic, strip, fd_out);
	    if(i == 0) {
		if(rex) {
		    rex = 0;
		    strcpy(topic, SCR_WEB_OUTVAL);
		    SCR_WEB_OUTVAL[0] = 0;
		    goto ag;
		}
		continue;
	    }
	}

	if(i != 0 && fd_trd) {
	    val_trd = SCR_get_err_txt_t_lang(fd_trd, topic, SCR_WEB_TRDLANG);
	    if(val_trd) {
		if(val_trd[0] == '=') val_trd = topic;
		if(strncmp(val_trd, "Msg#", 4) == 0) val_trd += 4;
		strcpy(SCR_WEB_OUTVAL, ScrWebTxt2Html(val_trd));
		SCR_strip(SCR_WEB_OUTVAL);
		fprintf(fd_out, SCR_WEB_OUTVAL);
		if(rex) {
		    rex = 0;
		    strcpy(topic, SCR_WEB_OUTVAL);
		    SCR_WEB_OUTVAL[0] = 0;
		    goto ag;
		}
	    }
	    continue;
	}

	if(i == 1)
	    while(!feof(fd_in)) {
		c = fgetc(fd_in);
		if(c == EOF || c != ' ') break;
	    }
	else {
	    fprintf(fd_out, "%c%s", strip, topic);
	}
    }

ended:
    if(fd_trd) fclose(fd_trd);
    fd_trd = 0;

    fclose(fd_in);
    fflush(fd_out);

    return(0);
}


/*NH*/
ScrWebPgFldToHtml(PAGE *pg, char *topic, int strip, FILE *fd, int disp) /* BP_M 18-11-2002 10:40 */
{
    char    buf[5001], *name, *res;
    U_ch    *ptr;
    int     i, j, k;
    FIELD   *fld = pg->pg_flds;

    strcpy(buf, topic);
    SCR_upper(topic);
    ACT_exec_nu(pg->pg_disp_act);
    for(i = 0 ; i < pg->pg_nb_flds; i++, fld++) {
	ACT_exec_nu(fld->fld_disp_act);
	name = PG_fld_name(pg, i);
	if(name) {
	    if((fld->fld_type == SCR_TYPE_RADIO ||
	       (fld->fld_type == SCR_TYPE_MENU && fld->fld_io == 0)) &&    /* JMP 07-11-01 */
		 strlen(topic) > strlen(name) && topic[strlen(name)] == '_') { /* JMP 10-09-02 */
		if(SCR_cstrncmp(name, topic, strlen(name)) == 0) {
		    j = atoi(topic + strlen(name) + 1);
		    if(j == SCR_get_field_short(pg, i)) {
			if(fld->fld_type == SCR_TYPE_RADIO) {
			    strcpy(SCR_WEB_OUTVAL, "checked");
			    if(disp) fprintf(fd, "checked");
			}
			else {
			    strcpy(SCR_WEB_OUTVAL, "selected");
			    if(disp) fprintf(fd, "selected");
			}
			return(0);
		    }
		}
	    }
	    else {
		if(SCR_cstrcmp(name, topic) == 0) {
		    ptr = PG_aget_rec_txt(pg, i);
		    Debug("Txt pg = '%s'\n", ptr);
		    res = ScrWebTxt2Html(ptr);
		    if(ptr) {
			if(strip == '$') SCR_strip(res);
			if(fld->fld_type == SCR_CHECK_BOX) {
			    if(SCR_CHECK_BOX_CHAR && res[0] == SCR_CHECK_BOX_CHAR[0]) {
				strcpy(SCR_WEB_OUTVAL, "checked");
				if(disp) fprintf(fd, "checked");
			    }
			}
			else {
			    for(j = 0, k = 0 ; res[j] ; j++) {     /* BP_M 18-11-2002 10:40 */
				if(res[j] != '\r' && res[j] != '\n') {
				    if(res[j] == '%') {
					SCR_WEB_OUTVAL[k++] = '%';
					SCR_WEB_OUTVAL[k++] = '%';
				    }
				    else
					SCR_WEB_OUTVAL[k++] = res[j];
				}
				else {
				    SCR_WEB_OUTVAL[k] = 0;
				    strcat(SCR_WEB_OUTVAL, "<br>");
				    k += 4;
				    if(res[j+1] == '\n' || res[j+1] == '\r') j++;
				}
			    }
			    SCR_WEB_OUTVAL[k] = 0;
//                          if(disp) fprintf(fd, ScrWebTxt2Html(SCR_WEB_OUTVAL));
			    if(disp) fprintf(fd, SCR_WEB_OUTVAL);
			}
			SCR_free(ptr);
			return(0);
		    }
		}
	    }
	}
    }
    return(-1);
}

ScrFind1Isi(ISAM *is, char *name)
{
    int     i;
    ISFINFO *isi;
    char    buf1[255], buf2[255];

    isi = is->is_info;
    strcpy(buf2, name);          SCR_upper(buf2);
    for(i = 0 ; i < is->is_nb_flds ; i++, isi++)
	if(isi->isi_name) {
	    strcpy(buf1, isi->isi_name); SCR_upper(buf1);
	    if(strcmp(buf1, buf2) == 0)
		return(i);
	}

    return(-1);
}

ScrFind1FlPg(PAGE *pg, char *name)
{
    int     i;
    char    buf1[255], buf2[255], *nfld;

    strcpy(buf2, name);          SCR_upper(buf2);
    for(i = 0 ; i < pg->pg_nb_flds ; i++) {
	nfld = PG_fld_name(pg, i);
	if(nfld) {
	    strcpy(buf1, nfld); SCR_upper(buf1);
	    if(strcmp(buf1, buf2) == 0)
		return(i);
	}
    }
    return(-1);
}

ScrWebFldIsToHtml(FILE *fd_out, PAGE *pg, char *topic)
{
    U_ch     **tbl, **tbl2;
    int     i, j, ifld, nfld[255];
    ISAM    *is;
    ISFINFO *isi;
    char    sqlstm[255], buf[255], valpg[255], *res;
    FILE_TYPE *flt;

    tbl = SCR_vtom(topic, '.');
    if(tbl == 0) return(-1);
    if(tbl[0] == 0 || tbl[1] == 0 || tbl[2] == 0 || tbl[3] == 0) return(-1);

    /* recherche de l'isam */
    flt = SCR_find_flt(tbl[0]);
    if(!flt) {
	Debug("Isam not found '%s'\n", tbl[0]);
	return(-1);
    }
    is = (ISAM *)flt->fl_ptr;

    if(is->is_info == 0) {
	Debug("Isam info not found '%s'\n", tbl[0]);
	return(-1);
    }
    /* Ouverture de la table */
    if(IS_open_files(is, 0L) != 0) {
	Debug("Error opening %s\n", tbl[0]);
	return(-1);
    }
    /* recherche du champs de recherche */
    ifld = ScrFind1Isi(is, tbl[1]);
    if(ifld < 0) return(-1);

    tbl2 = SCR_vtom(tbl[2], ',');
    for(i = 0 ; tbl2[i] ; i++) {
	j = ScrFind1Isi(is, tbl2[i]);
	if(j < 0) nfld[i] = -1000;
	else nfld[i] = j;
    }
    SCR_free_tbl(tbl2);
    nfld[i] = -1;

    /* Recherche */
    IS_empty_rec(is);
    sprintf(sqlstm, "ORDER BY %s", tbl[1]);
    if(IS_search_sql(is, sqlstm) == 0) {
	fprintf(fd_out, "\n\t<select size=\"1\" name=\"%s\" style=\"font-size:8pt;\">\n", tbl[3]);
	i = ScrFind1FlPg(pg, tbl[3]);
	if(i >= 0)
	    SCR_get_field_text(pg, i, valpg); SCR_strip(valpg);
	while(1) {
	    IS_get_field(is, ifld, buf);
	    SCR_strip(buf);
	    fprintf(fd_out, "\t\t<option value=\"%s\"", buf);
	    if(strcmp(buf, valpg) == 0)
		fprintf(fd_out, " selected");
	    fprintf(fd_out, ">");
	    for(i = 0 ; nfld[i] != -1 ; i++) {
		if(nfld[i] == -1000) continue;
		IS_get_field(is, nfld[i], buf);
		if(buf[0] > ' ')
		    fprintf(fd_out, "%s ", ScrWebTxt2Html(buf));
	    }
	    fprintf(fd_out, "\n");
	    if(IS_next(is)) break;
	}
	fprintf(fd_out, "\t</select>\n");
    }
    IS_close_files(is, 0L);
    SCR_free_tbl(tbl);

    return(0);
}


/***************************************************************
Copie les arguments d'un CGI d‚cod‚s par
DwGetCgiArg() dans les champs de la page pg.

Le nom des champs de la FORM doivent avoir la structure pgname@fieldname o—
&EN pgname est le nom de la page pg
&EN fieldname est le nom du champ

La page doit avoir l'attribut NAME_MEM.

&SA ScrWebFormToIsam()
****************************************************************/

ScrWebFormToPage(pg)
PAGE    *pg;
{
    int     ret;

    ret = ScrWebFormToPTR((char *)pg, SCR_TYPE_PAGE);

    return(ret);
}

/***************************************************************
Copie les arguments d'un CGI d‚cod‚s par
DwGetCgiArg() dans les champs de l'ISAM is.

Le nom des champs de la FORM doivent avoir la structure isname@fieldname o—
&EN isname est le nom de l'ISAM is
&EN fieldname est le nom du champ

L'ISAM doit avoir l'attribut ISI_MEM.

&SA ScrWebFormToIsam()
****************************************************************/
ScrWebFormToIsam(is)
ISAM    *is;
{
    int     ret;

    ret = ScrWebFormToPTR((char *)is, SCR_TYPE_ISAM);

    return(ret);
}

/*NH*/
ScrWebFormToPTR(ele, type)
char    *ele;
int     type;
{
    int     i, j, k, nbele;
    char    *ptr, nameele[255], *val, *namefld, buf[255], name[255], *dts;
    PAGE    *pg;
    ISAM    *is;
    long    dt[3], dtval, tm[2], tmval;

    ptr = SCR_find_ptr_name(ele);
    if(ptr == 0) nameele[0] = 0;
    else         strcpy(nameele, SCR_upper(ptr));

    if(type == SCR_TYPE_PAGE) {
	pg = (PAGE *)ele;
	nbele = pg->pg_nb_flds;
    }
    else {
	is = (ISAM *)ele;
	if(is->is_info == 0) return(-3);
	nbele = is->is_nb_flds;
    }

    dt[0] = dt[1] = dt[2] = tm[0] = tm[1] = 0L;

    for(i = 0 ; SCR_WEB_PARMS[i] ; i++) {

	k = U_pos('=', SCR_WEB_PARMS[i]);
	if(k < 0) continue;

	strcpy(name, SCR_WEB_PARMS[i]);
	name[k] = 0;
	SCR_upper(name);

	j = U_pos('@', name);
	if(j > 0) {
	    if(strncmp(name, nameele, j) != 0) continue;
	    strcpy(buf, name + j + 1);
	    strcpy(name, buf);
	}

	val  = SCR_WEB_PARMS[i] + k + 1;

	tmval = dtval = 0;
	if(strlen(name) > 3) {
	    dts = name + strlen(name) - 3;
	    if(strcmp(dts, "_DD") == 0)
		dt[0] = atoi(val);
	    if(strcmp(dts, "_MM") == 0)
		dt[1] = atoi(val);
	    if(strcmp(dts, "_YY") == 0) {
		dt[2] = atoi(val);
		if(dt[0] != 0 || dt[1] != 0 || dt[2] != 0) {    /* BP_M 05-12-2003 17:07 */
		    if(dt[0] > 31) dt[0] = 31;
		    if(dt[0] < 1)  dt[0] = 1;
		    if(dt[1] > 12) dt[1] = 12;
		    if(dt[1] < 1)  dt[1] = 1;
		    if(dt[2] > 2999) dt[2] = 2099;
		    if(dt[2] < 1)  dt[2] = 2002;
		}
	    }
	    if(strcmp(dts, "_HH") == 0)
		tm[0] = atoi(val);

	    if(strcmp(dts, "_MM") == 0) {
		tm[1] = atoi(val);
		if(tm[0] != 0 || tm[1] != 0) {  /* BP_M 05-12-2003 17:08 */
		    if(tm[0] > 23) tm[0] = 23;
		    if(tm[0] < 0)  tm[0] = 0;
		    if(tm[1] > 59) tm[1] = 59;
		    if(tm[1] < 0)  tm[1] = 0;
		}
	    }
	    if(dt[0] && dt[1] && dt[2]) {
		name[strlen(name) - 3] = 0;
		dtval = DT_dmy_date2000(dt);
		dt[0] = dt[1] = dt[2] = 0;
	    }
	    else if(tm[0] && tm[1]) {
		name[strlen(name) - 3] = 0;
		tmval = tm[0] * 10000L + tm[1] * 100L;
		tm[0] = tm[1] = 0;
	    }
	}

	for(j = 0 ; j < nbele ; j++) {
	    if(type == SCR_TYPE_PAGE)
		namefld = PG_fld_name(pg, j);
	    else
		namefld = is->is_info[j].isi_name;

	    if(namefld == 0) continue;
	    SCR_upper(namefld);
	    if(strcmp(name, namefld) == 0) {
		if(type == SCR_TYPE_PAGE) {
		    switch(pg->pg_flds[j].fld_type) {
			case SCR_TYPE_MENU:
			    SCR_set_field_short(pg, j, atoi(val));
			    break;
			case SCR_DATE:
			    if(dtval)
				SCR_set_field_long(pg, j, dtval);
			    else
				PG_set_rec_txt(pg, j, val);
			    break;

			case SCR_TIME:
			    if(tmval)
				SCR_set_field_long(pg, j, tmval);
			    else
				PG_set_rec_txt(pg, j, val);
			    break;

			default:
			    PG_set_rec_txt(pg, j, val);
			    break;
		    }
		}
		else
		    IS_set_field_txt(is, j, val);
	    }
	}
	SCR_WEB_PARMS[i][k] = '=';
    }
    return(0);
}

/* BP_M 12-07-2011 15:25 */
ScrWebFldPgToHtml(FILE *fd, PAGE *pg, char *topic)
{
    int     i, sel;
    MENU    *mn;
    char    buf[255], *nfld, *val_trd, *valfld;
    FIELD   *fld = pg->pg_flds;
    long    l, dt[3];

    SCR_upper(topic);
    for(i = 0 ; i < pg->pg_nb_flds ; i++, fld++) {
	nfld = PG_fld_name(pg, i);
	if(!nfld) continue;
	strcpy(buf, nfld);
	SCR_upper(nfld);
	if(strcmp(nfld, topic) == 0) {
	    switch(fld->fld_type) {
		case SCR_TYPE_MENU:
		    if(pg->pg_flds[i].fld_menu == 0) {
			fprintf(fd, "Error menu %s", topic);
			return;
		    }
		    mn = (MENU *)SCR_PTR(pg->pg_flds[i].fld_menu);
		    MN_read_opt(mn);
		    if(mn) {
			sel = SCR_get_field_short(pg, i);
			fprintf(fd, "\n\t<select size=\"1\" name=\"%s\" style=\"font-size:8pt;\">\n", topic);
			for(i = 0 ; i < mn->mn_nb_opts ; i++) {
			    fprintf(fd, "\t\t<option value=\"%d\"", i);
			    if(sel == i)
				fprintf(fd, " selected");
			    fprintf(fd, ">");
			    nfld = MN_text(mn, i);
			    if(nfld) {
				if(nfld[0] == '$') {
				    SCR_strip(nfld);
				    val_trd = SCR_get_err_txt_t_lang(fd_trd, nfld + 1, SCR_WEB_TRDLANG);
				    if(val_trd) {
					if(val_trd[0] == '=') val_trd = nfld;
					if(strncmp(val_trd, "Msg#", 4) == 0) val_trd += 4;
					fprintf(fd, SCR_WEB_OUTVAL);
				    }
				    else
					fprintf(fd, "%s", ScrWebTxt2Html(nfld));
				}
				else
				    fprintf(fd, "%s", ScrWebTxt2Html(nfld));
			    }
			    else
				fprintf(fd, "%s", ScrWebTxt2Html(nfld));
			    fprintf(fd, "\n");
			}
			fprintf(fd, "\t</select>\n");
		    }
		    return;
		case SCR_PASSWD    :
		case SCR_NATURAL   : case SCR_INTEGER   :
		case SCR_REAL      :
		case SCR_EDITOR    : case SCR_STRING    :
		case SCR_DIR       :
		    valfld = PG_aget_rec_txt(pg, i);
		    Debug("Type txt: '%s'\n", valfld);
		    SCR_strip(valfld);
		    if(fld->fld_nb_line == 1) {
			fprintf(fd, "<input type=\"text\" name=\"%s\" size=\"%d\" VALUE=\"%s\" style=\"font-size:8pt;\">\n",
			    nfld, fld->fld_nb_column, ScrWebTxt2Html(valfld));
		    }
		    else {
			fprintf(fd, "<textarea rows=\"%d\" name=\"%s\" cols=\"%d\" style=\"font-size:8pt;\">%s</textarea>\n",
			    fld->fld_nlv, nfld, fld->fld_ncv, ScrWebTxt2Html(valfld));
		    }
		    SCR_free(valfld);
		    return;
		case SCR_DATE      :
		    DT_dmy2000(SCR_get_field_long(pg, i), dt);
		    fprintf(fd, "<input type=\"text\" name=\"%s_DD\" size=\"2\" VALUE=\"%ld\" style=\"font-size:8pt;\">\n",
			nfld, dt[0]);
		    fprintf(fd, "<input type=\"text\" name=\"%s_MM\" size=\"2\" VALUE=\"%ld\" style=\"font-size:8pt;\">\n",
			nfld, dt[1]);
		    fprintf(fd, "<input type=\"text\" name=\"%s_YY\" size=\"4\" VALUE=\"%ld\" style=\"font-size:8pt;\">\n",
			nfld, dt[2]);
		    return;
		case SCR_TIME      :
		    l = SCR_get_field_long(pg, i);
		    fprintf(fd, "<input type=\"text\" name=\"%s_HH\" size=\"2\" VALUE=\"%ld\" style=\"font-size:8pt;\">\n",
			nfld, l / 10000L);
		    fprintf(fd, "<input type=\"text\" name=\"%s_MM\" size=\"2\" VALUE=\"%ld\" style=\"font-size:8pt;\">\n",
			nfld, (l % 10000L) / 100L);
		    break;
	    }
	}
    }
}

char *ScrWebTxt2Html(U_ch *str)    /* BP_M 15-11-2002 12:06 */
{
    static U_ch *buf = 0;
    static int  len_buf;
    U_ch    *txt, buf2[10], *buf_str;
    int     i = 0, len, j;

    if(buf == 0) {
	buf = SCR_malloc(5000);
	len_buf = 5000;
    }
    memset(buf, 0, len_buf);

    buf_str = SCR_stracpy(str);
//  SCR_AnsiToOem(buf_str, buf_str);
    Debug("In: '%s'\n", str);
    while(*buf_str) {
	txt = 0;
	switch(*buf_str) {
//          case '<'    : txt = "&lt;";     break;
//          case '>'    : txt = "&gt;";     break;
	    case '&'    :
		for(j = 0 ; j < 10 && buf_str[j] ; j++)
		    if(buf_str[j] == ';' && j > 2)
			break;
		if((buf_str[j] == ';' && j > 2) || buf_str[j] == 0) {
		    for(j = 0 ; j < 10 && *buf_str ; j++) {
			buf[i++] = *buf_str;
			if(*buf_str == ';') {
			    buf_str++;
			    break;
			}
			buf_str++;
		    }
		    continue;
		}
		else
		    txt = "&amp;";
		break;
	    // A
	    case 128    : txt = "&euro;"; break; /* euro  */
	    case 225    : txt = "&aacute;"; break; /* a aigu  */
	    case 224    : txt = "&agrave;"; break; /* a grave */
	    case 226    : txt = "&acirc;";  break; /* a circomflexe */
	    case 228    : txt = "&auml;";   break; /* a trema */
	    case 193    : txt = "&Aacute;"; break; /* A aigu */
	    case 192    : txt = "&Agrave;"; break; /* A grave */
	    case 194    : txt = "&Acirc;";  break; /* A circ */
	    case 196    : txt = "&Auml;";   break; /* A trema*/
	    // E
	    case 233    : txt = "&eacute;"; break; /* e aigu */
	    case 232    : txt = "&egrave;"; break; /* e grave */
	    case 234    : txt = "&ecirc;";  break; /* e circomflexe */
	    case 235    : txt = "&euml;";   break; /* e trema */
	    case 201    : txt = "&Eacute;"; break; /* E aigu */
	    case 200    : txt = "&Egrave;"; break; /* E grave */
	    case 202    : txt = "&Ecirc;";  break; /* E circomflexe */
	    case 203    : txt = "&Euml;";   break; /* E trema */

	    // I
	    case 237    : txt = "&iacute;"; break; /* i aigu  */
	    case 236    : txt = "&igrave;"; break; /* i grave */
	    case 238    : txt = "&icirc;";  break; /* i circomflexe */
	    case 239    : txt = "&iuml;";   break; /* i trema */
	    case 205    : txt = "&Iacute;"; break; /* I aigu  */
	    case 204    : txt = "&Igrave;"; break; /* I gave */
	    case 206    : txt = "&Icirc;";  break; /* I circomflexe */
	    case 207    : txt = "&Iuml;";   break; /* I trema */

	    // O
	    case 243    : txt = "&oacute;"; break; /* o aigu  */
	    case 242    : txt = "&ogave;";  break; /* o grave */
	    case 244    : txt = "&ocirc;";  break; /* o circomfl */
	    case 246    : txt = "&ouml;";   break; /* o trema */
	    case 211    : txt = "&Oacute;"; break; /* O aigu  */
	    case 210    : txt = "&Ograve;"; break; /* O grave  */
	    case 212    : txt = "&Ocirc;";  break; /* O circomfl */
	    case 214    : txt = "&Ouml;";   break; /* O trema */

	    // U
	    case 250    : txt = "&uacute;"; break; /* u aigu  */
	    case 249    : txt = "&ugave;";  break; /* u grave */
	    case 251    : txt = "&ucirc;";  break; /* u circomfl */
	    case 252    : txt = "&uuml;";   break; /* u trema */
	    case 218    : txt = "&Uacute;"; break; /* U aigu  */
	    case 217    : txt = "&Ugrave;"; break; /* U grave  */
	    case 219    : txt = "&Ucirc;";  break; /* U circomfl */
	    case 220    : txt = "&Uuml;";   break; /* U trema */
	    // C
	    case 231    : txt = "&ccedil;"; break;
	    // Divers
	    case 167    : txt = "&sect;";   break; /* degree */
	    case 178    : txt = "&sup2;";   break; /* degree */
	    case 179    : txt = "&sup3;";   break; /* degree */
	    case 181    : txt = "&micro;";  break; /* degree */
	    case 163    : txt = "&pound;";  break; /* degree */

	    // Y
	    case 253    : txt = "&yacute;"; break; /* y aigu  */
	    case 221    : txt = "&Yacute;"; break; /* Y aigu  */
	    case 255    : txt = "&yuml;";   break; /* y trema */
	    case 159    : txt = "&Yuml;";   break; /* Y trema */

	}
	if(txt == 0 && *buf_str > 127) {
	    sprintf(buf2, "&#%o;", *buf_str);
	    txt = buf2;
	}
	if(txt == 0)
	    len = 1;
	else
	    len = strlen(txt);

	if(len + strlen(buf) >= len_buf) {
	    buf = SCR_realloc(buf, sizeof(char), len_buf, len_buf + 100);
	    len_buf += 100;
	}
	if(txt == 0) {
	    buf[i++] = *buf_str;
	    buf[i] = 0;
	}
	else {
	    strcat(buf, txt);
	    i += len;
	}
	buf_str++;
    }
    SCR_free(buf_str);
    Debug("Out: '%s'\n", buf);

    return(buf);
}

/*
	    case 129    : txt = "&uuml;";   break; /* u trema
	    case 130    : txt = "&eacute;"; break; /* e aigu
	    case 131    : txt = "&acirc;";  break; /* a circomflexe
	    case 132    : txt = "&auml;";   break; /* a trema
	    case 133    : txt = "&agrave;"; break; /* a grave
	    case 135    : txt = "&#231;";   break; /* c cedille
	    case 136    : txt = "&ecirc;";  break; /* e circomflexe
	    case 137    : txt = "&euml;";   break; /* e trema
	    case 138    : txt = "&egrave;"; break; /* e grave
	    case 139    : txt = "&iuml;";   break; /* i trema
	    case 140    : txt = "&icirc;";  break; /* i circomflexe
	    case 147    : txt = "&ocirc;";  break; /* o circomfl
	    case 148    : txt = "&ouml;";   break; /* o trema
	    case 150    : txt = "&ucirc;";  break; /* u circomflexe
	    case 151    : txt = "&ugrave;"; break; /* u grave
	    case 167    : txt = "&#176;";   break; /* degree
	    case 238    : txt = "'";        break; /* single quote
	    case 239    :
	    case 240    : txt = "\"";       break; /* double quote
	    case 248    : txt = "&#176;";   break; /* degree
	    case 'ø'    : txt = "&#176;";   break; /* degree
*/



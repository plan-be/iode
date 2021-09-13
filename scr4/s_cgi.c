#include "scr4.h"

extern  FILE *DW_FD_OUT;

U_ch *DwGetCgiArg()
{
    char    *getenv();
    U_ch   *ptr;
    int     lg;

    ptr = (U_ch *) getenv("DW_PARAM"); /* JMP 08-10-01 */
    if(ptr == 0) {
	ptr = (U_ch *) getenv("CONTENT_LENGTH");
	if(ptr != 0)
	    lg = atol(ptr);
	else
	 lg = 20;
	if(lg == 0) return((U_ch *)0);
	ptr = (U_ch *)SCR_malloc(lg + 2);
	read(0, ptr, lg);
	}
    DwDecodeArg(ptr);
    return(ptr);
}

DwDecodeArg(ptr)
U_ch   *ptr;
{
    int     i, j, ch, lg;

    lg = strlen(ptr);
    for(i = j = 0 ; i < lg ; i++) {
	ch = ptr[i];
	if(ch == '+') ch = ' ';
	else if(ch == '%') {
	    ch = 16 * DwHexToDec(ptr[i + 1]) + DwHexToDec(ptr[i + 2]);
	    i += 2;
	    }
	ptr[j++] = ch;
	}

    ptr[j] = 0;
}

DwHexToDec(ch)
int     ch;
{
    ch = SCR_upper_char(ch);
    return(U_pos(ch, "0123456789ABCDEF"));
}

U_ch *DwCgiFind(tbl, arg)
U_ch   **tbl, *arg;
{
    int     i, len;

    if(tbl == 0) return((U_ch *)0);

    len = strlen(arg);

    for(i = 0 ; tbl[i] ; i++)
	if(memcmp(arg, tbl[i], len) == 0 && tbl[i][len] == '=')
	    return(tbl[i] + len + 1);

    return((U_ch *)0);
}

DwImpStrip(txt, len)
char    *txt;
int     len;
{
    char    *buf;
    int     i = 0;

    if(len == -1) len = strlen(txt);
    if(len == 0) return;
    buf = SCR_malloc(len + 10);
    strncpy(buf, txt, len);
    buf[len] = 0;
    SCR_strip(buf);
    while(buf[i]) {
	if(buf[i] == ' ' && buf[i + 1] == ' ') {
	    i++;
	    continue;
	}
	fputc(buf[i], DW_FD_OUT);
	i++;
    }
    if(strlen(buf) == 0) fprintf(DW_FD_OUT, "&nbsp");
    SCR_free(buf);
}

DwImpStripBr(txt, len, step)
U_ch    *txt;
int     len, step;
{
    U_ch    *buf;
    int     i, j = 0;

    buf = SCR_stracpy(txt);
    SCR_strip(buf);
    if(SCR_ISAM_ANSI)                   /* BP_M 24-10-2001 19:16 */
	SCR_OemToAnsi(buf, buf);
    for(i = 0 ; i < len ; i++) {
	if(buf[i] == 0) break;
	if(buf[i] == '\n' || j >= step) {
	    fprintf(DW_FD_OUT, "<br>");
	    j = 0;
	}
	fprintf(DW_FD_OUT, "%c", buf[i]);
	j++;
    }
    if(strlen(buf) == 0) fprintf(DW_FD_OUT, "&nbsp");
}


DwImpDate(l)
long    l;
{
    char    buf[20];

    SCR_long_to_date(l, buf);
    fprintf(DW_FD_OUT, buf);

    return(0);
}

DwImpTime(l)
long    l;
{
    char    buf[20];

    SCR_long_to_ftime(l, buf, "HH:MM");
    fprintf(DW_FD_OUT, buf);

    return(0);
}

DwOutputFile(file, fn)
char    *file;
int     (*fn)();
{
    return(DwOutputFileEx(file, fn, (char *)0));
}

int     DW_NOHEADER = 0;
char    DW_PATH[255];
char    *DW_LANGUAGE = 0; /* BP_M 14-02-2000 */
char    HTML_BCMD[] = "<!-- START:%s -->";
char    HTML_ECMD[] = "<!-- END:%s -->";

DwOutputFileEx(file, fn, part)
char    *file;
int     (*fn)();
char    *part;
{
    FILE    *fd_in;
    int     c, i, j;
    char    topic[255];
    char    filename[255];
    char    stag[255];
    FILE    *fd_out;

    DebugB("DwOutputFileEx[%s,,%s", file, part);
    if(DW_FD_OUT) fd_out = DW_FD_OUT;
    else          fd_out = DW_FD_OUT = stdout;



    if(DW_PATH[0] > ' ')
	sprintf(filename, "%s\\%s", DW_PATH, file);
    else
	sprintf(filename, file);

    if(DW_LANGUAGE) {
	strcat(filename, ".");
	strcat(filename, DW_LANGUAGE);
    }

    if(strlen(filename) > 4 && strcmp(filename + strlen(filename) - 4, ".htm") != 0)
	strcat(filename, ".htm");

    fd_in = fopen(filename, "rb");
    if(fd_in == 0) return(-1);

    if(!DW_NOHEADER) fprintf(stdout, "Content-type: text/html\n\n<HTML>\n");

    if(part) {
	sprintf(stag, HTML_BCMD, part);
	while(!feof(fd_in)) {
	    c = fgetc(fd_in);
	    if(c == EOF) {
		Debug("Rubrique '%s' non trouv‚e\n", part);
		goto ended;
	    }
	    if(c == '<') {
		j = 1;
		while(1) {
		    c = fgetc(fd_in);
		    if(c == EOF) goto ended;
		    if(stag[j] != c)
			break;
		    j++;
		}
		if(stag[j] == 0)
		    break;
	    }
	}
    }
    if(part) sprintf(stag, HTML_ECMD, part);
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
	i = 0;
	while(!feof(fd_in)) {
	    c = fgetc(fd_in);
	    if(c == EOF) break;
	    if(U_is_in(c, " \r\n\t,;.:/\\&\"'()!-?=+[]<>{}") == 1 || c > 126) {
		ungetc(c, fd_in);
		break;
	    }
	    topic[i++] = c;
	}
	topic[i] = 0;
	if(i == 0) {
	    fputc('$', fd_out);
	    continue;
	}

	if(fn != 0L)
	    i = (*fn)(fd_in, fd_out, topic);
	else
	    i = -1;

	if(i == 0) continue;

	if(i == 1)
	    while(!feof(fd_in)) {
		c = fgetc(fd_in);
		if(c == EOF || c != ' ') break;
	    }
	else {
	    if(i != 0)
		fprintf(fd_out, "$%s", topic);
	}
    }
ended:
    fclose(fd_in);
    fflush(fd_out);

    DebugE("]");
    return(0);
}

/***************************************************************
Copie les arguments d'un CGI d‚cod‚s par
DwGetCgiArg() dans les champs de la page pg.

Le nom des champs de la FORM doivent avoir la structure pgname@fieldname o—
&EN pgname est le nom de la page pg
&EN fieldname est le nom du champ

La page doit avoir l'attribut NAME_MEM.

&SA DwFormToIsam()
****************************************************************/

DwFormToPage(tbl, pg)
char    **tbl;
PAGE    *pg;
{
    int     ret;

    ret = DwFormToPTR(tbl, (char *)pg, SCR_TYPE_PAGE);
//    Debug("DwFormToPTR: ret = %d\n", ret);

    return(ret);
}

/***************************************************************
Copie les arguments d'un CGI d‚cod‚s par
DwGetCgiArg() dans les champs de l'ISAM is.

Le nom des champs de la FORM doivent avoir la structure isname@fieldname o—
&EN isname est le nom de l'ISAM is
&EN fieldname est le nom du champ

L'ISAM doit avoir l'attribut ISI_MEM.

&SA DwFormToIsam()
****************************************************************/
DwFormToIsam(tbl, is)
char    **tbl;
ISAM    *is;
{
    int     ret;

    ret = DwFormToPTR(tbl, (char *)is, SCR_TYPE_ISAM);
//    Debug("DwFormToIsam: ret = %d\n", ret);

    return(ret);
}

/*NH*/
DwFormToPTR(tbl, ele, type)
char    **tbl;
char    *ele;
int     type;
{
    int     i, j, k, nbele;
    char    *ptr, nameele[255], *val, *namefld, buf[255], name[255];
    PAGE    *pg;
    ISAM    *is;

    ptr = SCR_find_ptr_name(ele);
    if(ptr == 0) nameele[0] = 0;
    else         strcpy(nameele, SCR_upper(ptr));
//    Debug("Nameele = '%s'\n", nameele);

    if(type == SCR_TYPE_PAGE) {
	pg = (PAGE *)ele;
	nbele = pg->pg_nb_flds;
    }
    else {
	is = (ISAM *)ele;
	if(is->is_info == 0) return(-3);
	nbele = is->is_nb_flds;
    }

    for(i = 0 ; tbl[i] ; i++) {

	k = U_pos('=', tbl[i]);
	if(k < 0) continue;

	strcpy(name, tbl[i]);
	name[k] = 0;
	SCR_upper(name);

	j = U_pos('@', name);
	if(j > 0) {
	    if(strncmp(name, nameele, j) != 0) continue;
	    strcpy(buf, name + j + 1);
	    strcpy(name, buf);
	}

	val  = tbl[i] + k + 1;

//        Debug("Name = '%s' Val = '%s'\n", name, val);

	for(j = 0 ; j < nbele ; j++) {
	    if(type == SCR_TYPE_PAGE)
		namefld = PG_fld_name(pg, j);
	    else
		namefld = is->is_info[j].isi_name;

//          Debug("Nom du champs = '%s'\n", namefld);
	    if(namefld == 0) continue;
	    SCR_upper(namefld);
	    if(strcmp(name, namefld) == 0) {
//                Debug("namefld '%s' -> '%s' modifi‚\n", namefld, val);
		if(type == SCR_TYPE_PAGE)
		    PG_set_rec_txt(pg, j, val);
		else
		    IS_set_field_txt(is, j, val);
	    }
	}
	tbl[i][k] = '=';
    }
    return(0);
}






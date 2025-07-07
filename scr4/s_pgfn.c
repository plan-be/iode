#include "scr.h"

/* ======================================================================
Retourne sous forme d'un long la valeur du champ fld_nb de la page pg. Ce
champ doit être du type integer ou natural.
&NO
    Il n'y a pas de vérification de type !
&RT la valeur du champ
------------------------------------------------------------------------- */
long SCR_get_field_long(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    long    l;

    memcpy(&l, pg->pg_rec + pg->pg_fld_pos[fld_nb], sizeof(long));  /* JMP38 31-08-92 */
    return(l);
}
/* ======================================================================
Retourne sous forme d'un int la valeur du champ fld_nb de la page pg. Ce
champ doit être du type MENU.
&NO
    Il n'y a pas de vérification de type !
&RT la valeur du champ
------------------------------------------------------------------------- */
SCR_get_field_short(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    short   s;

    memcpy(&s, pg->pg_rec + pg->pg_fld_pos[fld_nb], sizeof(short)); /* JMP38 31-08-92 */
    return((int)s);
}

/* ======================================================================
Retourne sous forme d'un double la valeur du champ fld_nb de la page pg. Ce
champ doit être du type real.
&NO
    Il n'y a pas de vérification de type !
&RT la valeur du champ
------------------------------------------------------------------------- */
double SCR_get_field_double(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    double  d;

    memcpy(&d, pg->pg_rec + pg->pg_fld_pos[fld_nb], sizeof(double)); /* JMP38 31-08-92 */
    return(d);
}

/* ======================================================================
Copie dans text la valeur du champ fld_nb de la page pg. Ce champ doit
être du type string.
&NO
    Il n'y a pas de vérification de type !
&RT un pointeur vers text
------------------------------------------------------------------------- */
char *SCR_get_field_text(pg, fld_nb, text)
PAGE    *pg;
int     fld_nb;
char    *text;
{
    PG_read_flds(pg);
    strcpy(text, PG_get_text(pg, fld_nb));
    return(text);
}

/* ======================================================================
Copie dans le champ fld_nb de la page pg le texte text. Ce champ doit
être du type string. Des blancs sont ajoutés si text est trop court. Si text
est trop long, les caractères excédentaires sont supprimés.
&NO
    Il n'y a pas de vérification de type !
&RT un pointeur vers text
------------------------------------------------------------------------- */
SCR_set_field_text(pg, fld_nb, text)
PAGE    *pg;
int     fld_nb;
unsigned char    *text;
{
    FIELD   *fld;
    char    *rec;
    int     lg, i;

    PG_read_flds(pg);

    fld = pg->pg_flds + fld_nb;
    rec = PG_get_text(pg, fld_nb);

    lg = FLD_len_in_rec(fld) - 1;
    memset(rec, ' ', lg);
    for(i = 0 ; *text && i < lg ; i++, text++)
	rec[i] = *text;
    rec[lg] = 0;
    return(0);
}

/* ======================================================================
Fixe la valeur du champ fld_nb de la page pg. Ce
champ doit être du type real.
&NO
    Il n'y a pas de vérification de type !
------------------------------------------------------------------------- */
SCR_set_field_double(pg, fld_nb, val)
PAGE    *pg;
int     fld_nb;
double  val;
{
    memcpy(pg->pg_rec + pg->pg_fld_pos[fld_nb], &val, sizeof(double)); /* JMP38 31-08-92 */
    return(0);
}

/* ======================================================================
Fixe la valeur du champ fld_nb de la page pg. Ce
champ doit être du type integer ou natural.
&NO
    Il n'y a pas de vérification de type !
------------------------------------------------------------------------- */
SCR_set_field_long(pg, fld_nb, val)
PAGE    *pg;
int     fld_nb;
long    val;
{
    memcpy(pg->pg_rec + pg->pg_fld_pos[fld_nb], &val, sizeof(long));
    return(0);
}

/* ======================================================================
Fixe la valeur du champ fld_nb de la page pg. Ce
champ doit être du type menu.
&NO
    Il n'y a pas de vérification de type !
------------------------------------------------------------------------- */
SCR_set_field_short(pg, fld_nb, val)
PAGE    *pg;
int     fld_nb;
int     val;
{
    memcpy(pg->pg_rec + pg->pg_fld_pos[fld_nb], &val, sizeof(short));
    return(0);
}

/*NH*/
/* JMP 17-04-92
SCR_fmt_double(out, fmt, d)
char    *out, *fmt;
double  d;
{
    int     i, j;
    char    in[30];

    sprintf(in, "%lg", d);
    i = strlen(fmt) - 1;
    out[i + 1] = 0;
    j = strlen(in) - 1;
    for( ; i >= 0 ; i--) {
	switch(fmt[i]) {
	    case '9' :
	    case 'x' :
	    case 'X' :
		if(j >= 0) out[i] = in[j--];
		else out[i] = ' ';
		break;
	    case '0' :
		if(j >= 0) out[i] = in[j--];
		else out[i] = '0';
		if(out[i] == ' ') out[i] = '0';
		break;
	    default :
		out[i] = fmt[i]; break;
	    }
    }
}
*/
/*NH*/
FLD_align_in_rec(fld)
FIELD   *fld;
{
    switch(fld->fld_type) { /* 4.03 */
	case SCR_NATURAL   :
	case SCR_INTEGER   :
	case SCR_TIME      :
	case SCR_DATE      : return(sizeof(long));
	case SCR_REAL      : return(sizeof(double));
	case SCR_CHECK_BOX :    /* BP_M 19-01-97 */
	case SCR_TYPE_RADIO:    /* BP_M 21-01-97 */
	case SCR_TYPE_MENU : return(sizeof(short));
	case SCR_STRING    :
	case SCR_PASSWD    :
	case SCR_DIR       : return(1);
	case SCR_EDITOR    : return(sizeof(char *));
	default            : return(0);               /* JMP 4.23 07-11-95 */
	/*
	    SCR_yyerror(ERR_FATAL, ERR_FIELD_UNKNOWN, "type unknown");
	    return(-1);
	/* JMP 4.23 07-11-95 */
    }
}

/*NH*/
FLD_len_in_rec(fld)
FIELD   *fld;
{
    int     len; /* 4.03 */

    if(fld->fld_type == SCR_EDITOR) return(41 + sizeof(char *) * 2);
    len = FLD_align_in_rec(fld);
    if(len > 1) return(len);
    return(fld->fld_nb_line * fld->fld_nb_column + 1);
}

/*NH*/
PG_set_rec(pg, fld_nb, txt)
PAGE    *pg;
int     fld_nb;
char    *txt;
{
    char    *ptr;

    PG_read_flds(pg);
    ptr = pg->pg_rec + pg->pg_fld_pos[fld_nb];

    pg->pg_flds[fld_nb].fld_modified = 1;
    switch(pg->pg_flds[fld_nb].fld_type) {
	case SCR_CHECK_BOX :    /* BP_M 10-10-2011 10:14 */
	case SCR_TYPE_RADIO:
	    memcpy(ptr, txt, 1);
	    break;
	case SCR_NATURAL   :
	case SCR_INTEGER   :
	case SCR_TIME      :
	case SCR_DATE      :
	case SCR_REAL      :
	case SCR_TYPE_MENU :
	    memcpy(ptr, txt, FLD_len_in_rec(pg->pg_flds + fld_nb));
	    break;
	case SCR_EDITOR    :
	    memcpy(ptr, &txt, sizeof(char *));
	    break;
	case SCR_STRING    :
	case SCR_PASSWD    :
	case SCR_DIR       :
	case SCR_TYPE_MENUI:
	    SCR_set_field_text(pg, fld_nb, txt);
	    break;
    }
    return(0);
}

/*NH*/
PG_set_rec_txt(pg, fld_nb, txt)
PAGE    *pg;
int     fld_nb;
char    *txt;
{
#ifndef __cplusplus /* JMP 06-01-98 */
    extern long PG_fdate_to_long(), PG_ftime_to_long();
    extern unsigned long PG_ufmt_long();
    extern unsigned char *PG_ufmt_text();
    double  SCR_atof();
#endif
    FIELD   *fld;
    char    *fmt, buf[80], buf1[80];
    long    l;
/*    long    atol(); /* JMP 06-01-98 */
    double  d, d1;
    short   s;      /* BP_M 19-01-97 */
    int     len;

    PG_read_flds(pg);

    fld = pg->pg_flds + fld_nb;
    fld->fld_modified = 1;
    fmt = (!fld->fld_fmt) ? 0:PG_fld_fmt(pg, fld_nb);

    switch(fld->fld_type) {
	case SCR_NATURAL   :
	case SCR_INTEGER   :
	    if(fld->fld_fmt)
		l = PG_ufmt_long(fmt, txt);
	    else
		l = atol(txt);
	    SCR_set_field_long(pg, fld_nb, l);
	    break;

	case SCR_CHECK_BOX :    /* BP_M 19-01-97 */
	    if(txt[0] == ' ') s = 0;
	    else              s = 1;
	    SCR_set_field_short(pg, fld_nb, (int)s);
	    break;

	case SCR_TYPE_RADIO:    /* BP_M 21-01-97 */
	    if(txt[0] == ' ') s = 0;
	    else
		s = atoi(txt);
	    SCR_set_field_short(pg, fld_nb, (int)s);
	    break;

	case SCR_TIME      :
	    SCR_set_field_long(pg, fld_nb,
			       PG_ftime_to_long(txt, fmt));
	    break;
	case SCR_DATE      :
	    SCR_set_field_long(pg, fld_nb,
			       PG_fdate_to_long(txt, fmt));
	    break;

	case SCR_REAL      :
/*          if(fld->fld_fmt != 0)
		d = SCR_ufmt_double(fmt, txt);
	    else
		d = SCR_atof(txt);   */

	    /* Lit la nouvelle valeur */
	    if(strncmp(txt, "--", 2) == 0)
		d = SCR_NOT_AVAIL;
	    else
		d = SCR_atof(txt);

	    /* Cherche l'ancienne valeur sous forme de texte */
	    d1 = SCR_get_field_double(pg, fld_nb);              /* JMP 29-12-01 */
	    PG_get_rec_txt(pg, fld_nb, buf1, 0);                /* JMP 29-12-01 */

	    /* Assigne la nouvelle et reformatte*/
	    SCR_set_field_double(pg, fld_nb, d);                /* JMP 29-12-01 */
	    PG_get_rec_txt(pg, fld_nb, buf, 0);                 /* JMP 29-12-01 */

	    /* Compare les deux versions et remet l'ancienne si pas de changement */
	    if(strcmp(buf, buf1) == 0)                          /* JMP 29-12-01 */
		SCR_set_field_double(pg, fld_nb, d1);           /* JMP 29-12-01 */

	    break;

	case SCR_TYPE_MENU :
	    MN_read_opt(MENU_PTR(fld->fld_menu));
	    SCR_set_field_short(pg, fld_nb, (int)SCR_search_menu(MENU_PTR(fld->fld_menu), txt));
	    MN_free_opt(MENU_PTR(fld->fld_menu));
	    break;
	case SCR_STRING    :
	case SCR_PASSWD    :
	case SCR_DIR       :
	case SCR_TYPE_MENUI:
	    if(fmt) {
		if(PG_check_fmt(txt, fmt) == 0) {       /* BP_M 14-07-1997 */
		    PG_ufmt_text(buf, fmt, txt);
		    SCR_set_field_text(pg, fld_nb, buf);
		}
		else
		    SCR_set_field_text(pg, fld_nb, txt);
	    }
	    else
		SCR_set_field_text(pg, fld_nb, txt);
	    break;
	case SCR_EDITOR    :
	    SCR_set_field_editor(pg, fld_nb, txt);
	    break;
    }
    return(0);
}

/*NH
char *PG_get_text(pg, fld_nb)
PAGE    *pg;
char    fld_nb;
{
    return(pg->pg_rec + pg->pg_fld_pos[fld_nb]);
}
/* JMP 4.23 13-12-95 */

/*NH*/
PG_get_rec(pg, fld_nb, txt)
PAGE    *pg;
int     fld_nb;
char    *txt;
{
    char    *ptr;
    int     len;

    PG_read_flds(pg);
    ptr = PG_get_text(pg, fld_nb);
    len = FLD_len_in_rec(pg->pg_flds + fld_nb);
    if(pg->pg_flds[fld_nb].fld_type == SCR_EDITOR) len = sizeof(char *);
    memcpy(txt, ptr, len);
    return(0);
}

/* ======================================================================
Alloue un string correspondant à la valeur formatée d'un champ. Cette
fonction retourne un pointeur vers le champ alloué.

&EX
    PAGE pg {
	...
	FIELD string NAME ccp FORMAT "000-0000000-00"
	...
	}

    PrintCcp()
    {
	char    *ptr;

	ptr = PG_aget_rec_txt(pg, n);
	printf("1: CCP : %s\n", ptr);
	SCR_free(ptr);
	printf("2: CCP : %s\n", pg_CCP);
    }

    Résultat de PrintCcp() :

	1: CCP : 123-1234567-12
	2: CCP : 123123456712

&TX
&RT
&SA
------------------------------------------------------------------------- */
char *PG_aget_rec_txt(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    char    *txt, **ptr, c, *txt2;
    FIELD   *fld;

    PG_read_flds(pg);
    fld = pg->pg_flds + fld_nb;

    if(fld->fld_type == SCR_EDITOR) {
	ptr = (char **)PG_get_text(pg, fld_nb);
	txt = SCR_stracpy(ptr[0]);
    }
    else {  /* BP_M 02-12-2009 17:26 */
	txt = SCR_malloc(fld->fld_nb_column * fld->fld_nb_line + 1);
	txt2 = PG_get_rec_txt(pg, fld_nb, txt, 1);
	if(txt2 != txt)
	    txt = txt2;
    }
    return(txt);
}

/*NH*/
char *PG_get_rec_txt(pg, fld_nb, txt, flag)
PAGE    *pg;
int     fld_nb;
char    *txt;
int     flag;
{
    FIELD   *fld;
    int     size, lenptr;
    int     nb_dec;
    char    buf[80];
    char    buf1[80];
    char    *buf2, *ptr;
    char    *fmt;
    long    l;
    double  d;
    short   s;
    MENU    *mn;

    PG_read_flds(pg);
    fld  = pg->pg_flds + fld_nb;
    size = fld->fld_nb_line * fld->fld_nb_column;
    fmt = (fld->fld_fmt == 0) ? 0:PG_fld_fmt(pg, fld_nb);

    switch(fld->fld_type) {
	case SCR_NATURAL   :
	case SCR_INTEGER   :
	    memset(txt, 0, size);
	    l = SCR_get_field_long(pg, fld_nb);
	    if(fmt != 0)
		PG_fmt_long(txt, fmt, l);
	    else {
		sprintf(buf, "%%-%dld", size);
#ifdef SCRW32
		if(l == 0)  /* BP_M 21-02-2012 22:01 */
		    buf[0] = 0;
		else
#endif
		    sprintf(txt, buf, l);
	    }
	    break;
	case SCR_CHECK_BOX :        /* BP_M 19-01-97 */
	    s = SCR_get_field_short(pg, fld_nb);
	    if(s == 0) strcpy(txt, " ");
	    else       strcpy(txt, SCR_CHECK_BOX_CHAR);
	    break;

	case SCR_TYPE_RADIO:        /* BP_M 21-01-97 */
	    s = SCR_get_field_short(pg, fld_nb);
	    sprintf(txt, "%d", s);
	    break;

	case SCR_TIME      :
	    SCR_long_to_ftime(SCR_get_field_long(pg, fld_nb), txt, fmt);
	    break;
	case SCR_DATE      :
	     SCR_long_to_fdate(SCR_get_field_long(pg, fld_nb), txt, fmt);
	    break;
	case SCR_REAL      :
	    d = SCR_get_field_double(pg, fld_nb);
/*          if(fmt != 0)
		SCR_fmt_double(txt, fmt, d);
	    else
*/
		{
		nb_dec = pg->pg_flds[fld_nb].fld_nb_dec;
		if(nb_dec == 15) nb_dec = SCR_NB_DEC; /* JMP 17-04-92 */
		SCR_fmt_dbl(d, txt, size, nb_dec);
		}
	    break;

	case SCR_TYPE_MENU :
	    memset(txt, ' ', size);
	    s = SCR_get_field_short(pg, fld_nb);
	    mn = (MENU *)SCR_PTR(fld->fld_menu);
	    MN_read_opt(mn);
	    if(s < 0 || s > mn->mn_nb_opts - 1) s = 0;
	    ptr = mn->mn_opts[s].opt_text;
	    if(ptr)
		lenptr = strlen(ptr);
	    else
		lenptr = 0;

//          Debug("Ptr='%s' - lenptr=%d - size=%d - opt=%d\n", ptr, lenptr, size, s);

	    if(ptr) {
		if(lenptr > size) {
		    SCR_free(txt);
		    size = strlen(ptr);
		    txt = SCR_malloc(size * 2);
		}
		memcpy(txt, ptr, strlen(ptr));
	    }
	    else
		txt[0] = 0;
	    txt[size] = 0;
	    SCR_pad(txt, size);  /* JMP 20-10-2003 */
	    MN_free_opt(mn);
	    break;

	case SCR_DIR       :
	case SCR_TYPE_MENUI:
	case SCR_STRING    :
	case SCR_PASSWD    :
	    PG_get_rec(pg, fld_nb, txt);
	    if(fmt != 0) {
		PG_fmt_text(buf, fmt, txt);
		strcpy(txt, buf);
		return(txt);
	    }
	    else {
		sprintf(buf, "%%-%ds", size);
		if(size > 79) {
		    buf2 = SCR_malloc(size + 1);
		    sprintf(buf2, buf, txt);
		    memcpy(txt, buf2, size + 1);
		    SCR_free(buf2);
		}
		else {
		    sprintf(buf1, buf, txt);
		    memcpy(txt, buf1, size + 1);
		}
	    }
	    break;
	case SCR_EDITOR:
	    return(txt);
    }

    if(flag)
	PG_modify_text(pg, txt, fld_nb);
    return(txt);
}

/*NH*/
PG_cmp_val(pg, fld_nb, txt) /* BP_M 30-03-1995 */
PAGE    *pg;
int     fld_nb;
char    *txt;
{
    int     i, ret = 0;
    char    *buf1, *buf2;
    FIELD   *fld;

    PG_read_flds(pg);
    fld = pg->pg_flds + fld_nb;
    i = FLD_len_in_rec(pg->pg_flds + fld_nb);
    buf1 = SCR_malloc(i + 1);
    buf2 = SCR_malloc(i + 1);
    memcpy(buf1, txt, i); buf1[i] = 0;
    memcpy(buf2, pg->pg_rec + pg->pg_fld_pos[fld_nb], i); buf2[i] = 0;
    switch(fld->fld_type) {
	case SCR_TYPE_RADIO:    /* BP_M 21-01-97 */
	case SCR_CHECK_BOX :    /* BP_M 19-01-97 */
	case SCR_NATURAL   :
	case SCR_INTEGER   :
	case SCR_TIME      :
	case SCR_DATE      :
	case SCR_REAL      :
	case SCR_TYPE_MENU :
	    ret = memcmp(buf1, buf2, i);
	    break;
	case SCR_STRING    :
	case SCR_PASSWD    :
	case SCR_DIR       :
	case SCR_TYPE_MENUI:
	    SCR_strip(buf1);
	    SCR_strip(buf2);
	    ret = strcmp(buf1, buf2);
	    break;
	case SCR_EDITOR    :
	    break;
    }
    SCR_free(buf1);
    SCR_free(buf2);
    return(ret);
}

/*NH OLD VERSION JMP
PG_cmp_val(pg, fld_nb, txt)
PAGE    *pg;
int     fld_nb;
char    *txt;
{
    PG_read_flds(pg);
    return(memcmp(pg->pg_rec + pg->pg_fld_pos[fld_nb],
		  txt,
		  FLD_len_in_rec(pg->pg_flds + fld_nb)));
}
*/


/*NH*/
PG_empty_field(pg, fld_nb)/* BP_M 17-07-1998 */
PAGE    *pg;
int     fld_nb;
{
    long    l = 0L;
    double  d = 0.0;
    short   s = 0;
    FIELD   *fld;
    extern int SCR_DFT_SET_TODAY;

    PG_read_flds(pg);
    fld = pg->pg_flds + fld_nb;
    fld->fld_modified = 1;

    switch(fld->fld_type) {
	case SCR_NATURAL   :
	case SCR_INTEGER   :
	    SCR_set_field_long(pg, fld_nb, l);
	    break;
	case SCR_TIME      :    /* BP_M 17-07-1998 */
	    if(fld->fld_date == 1 && SCR_DFT_SET_TODAY) /* JMP 01-10-98 */
		l = SCR_current_time();/* BP_M 17-07-1998 */
	    SCR_set_field_long(pg, fld_nb, l);
	    break;
	case SCR_DATE      :        /* BP_M 17-07-1998 */
	    if(fld->fld_date == 1 && SCR_DFT_SET_TODAY) /* JMP 01-10-98 */
		l = SCR_current_date();/* BP_M 17-07-1998 */
	    SCR_set_field_long(pg, fld_nb, l);
	    break;
	case SCR_REAL      :
	    SCR_set_field_double(pg, fld_nb, d);
	    break;
	case SCR_TYPE_RADIO:/* BP_M 21-01-97 */
	case SCR_CHECK_BOX :/* BP_M 19-01-97 */
	    SCR_set_field_short(pg, fld_nb, (int)s);
	    break;
	case SCR_STRING    :
	case SCR_PASSWD    :
	case SCR_DIR       :
	case SCR_TYPE_MENUI:
	    memset(PG_get_text(pg, fld_nb),
		   ' ',
		   fld->fld_nb_line * fld->fld_nb_column);
	    break;
	case SCR_TYPE_MENU :
	    SCR_set_field_short(pg, fld_nb, 0);
	    break;
	case SCR_EDITOR    :
	    SCR_set_field_editor(pg, fld_nb, (char *)0);
	    break;
    }
    return(0);
}

/*NH*/
PG_fld_blanc(txt, len)
unsigned char    *txt;
int     len;
{
    for(len-- ; len >= 0 ; len--) {
	if(txt[len] > ' ') return(0);
	if(txt[len] < ' ') txt[len] = ' ';
    }
    return(0);
}

/*NH*/
char *_SCR_find_buf(buf, fld_nb)
unsigned char *buf;
int           fld_nb;
{
    int     i;
    short   s;

    s = *(short *)buf;
    if(fld_nb >= s) return((char *)0);
    buf++;
    buf++;

    for(i = 0 ; i < fld_nb ; i++, buf++)
	while(*buf) buf++;

    return((*buf == 0) ? (char *)0:(char *)buf);
}

/*NH*/
int _SCR_find_name(buf, name)
unsigned char *buf, *name;
{
    int     i;
    short   s;

    s = *(short *)buf;
    buf++;
    buf++;

    for(i = 0 ; i < s ; i++, buf++) {
	if(strcmp(buf, name) == 0) return(i);
	while(*buf) buf++;
    }
    return(-1);
}


/*NH*/
PG_check_fmt(txt, fmt)
char    *txt, *fmt;
{
    int     i, j;
    char    buf[255];


    strcpy(buf, txt);

    SCR_strip(buf);
    if(strlen(buf) < strlen(fmt)) return(-1);

    return(0);
}


PG_find_abkey(pg)       /* BP_M 22-02-2001 12:00 */
PAGE    *pg;
{
    FNKEYS      *fnk = (FNKEYS *)SCR_PTR(pg->pg_fnks);
    int         i;
    ACTION      *act;
    A_ACTION    *aa;
    int         flag = -1;

    if(fnk == 0) return(SCR_F3);

    for(i = 0 ; i < fnk->fk_nb_fnks ; i++) {
	/* BP_M 05-03-2010 12:32 */
	if(SCR_check_prot_vt(fnk->fk_fnks[i].fnk_prot)) continue;
	act = (ACTION *)SCR_PTR(fnk->fk_fnks[i].fnk_act);
	if(act == 0) continue;
	aa = act->act_aa;
	if(aa->aa_fn == ACT_PG_ABORT) flag = i;
	if(flag == i && fnk->fk_fnks[i].fnk_key == SCR_F3) break;
    }
    /* BP_M 05-03-2010 12:32 */
    if(flag == -1) flag = 0;

    return(fnk->fk_fnks[flag].fnk_key);
}

/* ===========================================================
    Set l'io de tous les champs d'une page
    IN : pg : Pointeur d'une page
	 io : io à attribuer à tous les champs
   =========================================================== */
PG_set_all_io(PAGE *pg, int io)
{
    int     i;
    FIELD   *fld = pg->pg_flds;

    for(i = 0 ; i < pg->pg_nb_flds ; i++, fld++)
	fld->fld_io = io - SCR_INPUT;
}

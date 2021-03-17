#include "iode.h"

char     KT_sep = '&';
char     **KT_names = NULL;
int      KT_nbnames = 0;
int      KT_mode[MAX_MODE];
int      KT_CUR_TOPIC = 0;
int      KT_CUR_LEVEL = 0;

T_prep_cls(tbl, smpl, cls)
TBL     *tbl;
char    *smpl;
COLS    **cls;
{
    int     dim;

    *cls = COL_cc(smpl);
    if(*cls == NULL) {
	B_seterror("Sample %.80s : syntax error", smpl);
	return(-1);
    }

    dim = COL_resize(tbl, *cls);
    return(dim);
}

int T_print_tbl(tbl, smpl)
TBL     *tbl;
char    *smpl;
{
    int     i, j, dim, rc = 0, first = 1;
    COLS    *cls;
    TLINE   *line;
    unsigned char *T_get_title();

    dim = T_prep_cls(tbl, smpl, &cls);
    if(dim < 0) return(-1);

    B_PrintRtfTopic(T_get_title(tbl));
/*    W_printf( ".topic %d %d %s\n", KT_CUR_TOPIC++, KT_CUR_LEVEL,
				    T_get_title(tbl));
*/
    if(T_begin_tbl(dim, cls)) return(-1);
    W_printf( ".ttitle %s\n", T_get_title(tbl)); /* JMP 27-02-98 */
    for(i = 0; rc == 0 && i < T_NL(tbl); i++) {
	line = T_L(tbl) + i;

	switch(line->tl_type) {
	case KT_LINE  :
	    W_printf(".tl\n"); break;
	case KT_TITLE :
	    if(first) {
		first = 0;
		break;
		}
	    T_print_cell((TCELL *) line->tl_val, NULL, dim);
	    W_printf( "\n");
	    break;
	case KT_DATE  :
	    T_print_date(dim); break;
	case KT_MODE  :
	    T_print_mode(cls, dim); break;
	case KT_FILES :
	    T_print_files(cls, dim); break;
	case KT_CELL  :
	    if(T_print_line(tbl, i, cls) < 0) {
		rc = -1;
		B_seterror("Unable to print line %d ", i);
		}
	    break;
	}
    }
    T_end_tbl();

    COL_free(cls);
    return(rc);
}

T_print_line(tbl, i, cls)
TBL     *tbl;
int     i;
COLS    *cls;
{
    int     j, d;
    TLINE   *line = T_L(tbl) + i;
    TCELL   *cell = (TCELL *) line->tl_val;

    COL_clear(cls);
    if(COL_exec(tbl, i, cls) < 0)   return(-1);

    for(j = 0; j < cls->cl_nb; j++) {
	d = j % T_NC(tbl);
	if(tbl->t_free == 0 && d == 0 && j != 0) continue;
	T_print_cell(cell + d, cls->cl_cols + j, 1);
    }
    W_printf("\n");

    return(0);
}

T_print_cell(cell, cl, straddle)
COL     *cl;
TCELL   *cell;
int     straddle;
{
    if(cell == 0 || cell->tc_val == 0) {                /* JMP 24-06-98 */
	W_printf("%c1R", KT_sep);  /* JMP 24-06-98 */
	return(0);                 /* JMP 24-06-98 */
	}
    if(cell->tc_type == KT_STRING && U_is_in('#', cell->tc_val))
      //cell->tc_attr = KT_ALIGN(cell->tc_attr, KT_CENTER); /* JMP 05-01-02 */
	cell->tc_attr = KT_ALIGN(cell->tc_attr, KT_RIGHT); /* JMP 05-01-02 */
    if(cell->tc_type == KT_LEC)
	cell->tc_attr = KT_ALIGN(cell->tc_attr, KT_DECIMAL);

    T_align(cell->tc_attr, straddle, cell->tc_type);  /* JMP 17-12-93 */
    T_open_attr(cell->tc_attr);

    if(cell->tc_type != 0) {
	if (cl == NULL || cell->tc_type == KT_STRING) {
	    T_print_string(cl, cell->tc_val);
	    }
	else T_print_val(cl->cl_res);
    }

    T_close_attr(cell->tc_attr);
    return(0);
}

T_fmt_val(buf, val, lg, nd)             /* JMP 12-11-93 */
char    *buf; IODE_REAL    val;
int     lg, nd;
{
    if(L_ISAN(val)) SCR_fmt_dbl(val, buf, lg, nd);
    else strcpy(buf, "-.-");
    SCR_sqz(buf);
    return(0);
}

T_print_val(val) IODE_REAL    val;
{
    char    buf[64];

    //T_fmt_val(buf, val, 12, B_NBDEC);        /* JMP 07-05-97 */
    T_fmt_val(buf, val, 30, B_NBDEC);        /* JMP 07-05-97 */
    W_printf(buf);
    return(0);
}

T_print_string(cl, string)
COL     *cl;
char    *string;
{
    char   *ptr = NULL;
    char    *COL_text();

    ptr = (char *) COL_text(cl, string, KT_nbnames);
    if(ptr != NULL) W_printf( "%s", ptr);
    SW_nfree(ptr);
    return(0);
}

T_align(attr, straddle, type) /* JMP 17-12-93 */
int     attr, straddle, type;
{
    char    align = 'L';

    if(attr & KT_CENTER)    align = 'C';
    if(attr & KT_RIGHT)     align = 'R';
    if(type != KT_STRING && (attr & KT_DECIMAL))   align = 'D'; /* JMP 17-12-93 */

    W_printf("%c%d%c", KT_sep, straddle, align);
    return(0);
}

T_open_attr(attr)
int attr;
{

    if(attr & KT_BOLD)      W_printf("\\b");
    if(attr & KT_ITALIC)    W_printf("\\i");
    if(attr & KT_UNDERLINE) W_printf("\\u");
    return(0);
}

T_close_attr(attr)
int attr;
{
    if(attr & KT_BOLD)      W_printf("\\B");
    if(attr & KT_ITALIC)    W_printf("\\I");
    if(attr & KT_UNDERLINE) W_printf("\\U");
    return(0);
}

char **T_find_files(cls)
COLS *cls;
{
    int     i, nf = 0;
    COL     *cl;
    int     files[K_MAX_FREF + 1];
    char    **names = 0, buf[K_MAX_FILE + 10];
    KDB     *kdb;

    memset(files, 0, (K_MAX_FREF + 1) * sizeof(int));
    for(i = 0; i < cls->cl_nb; i++) {
	cl = cls->cl_cols + i;
	files[cl->cl_fnb[0]] = 1;
	files[cl->cl_fnb[1]] = 1;
    }

    for(i = 1; i < K_MAX_FREF + 1; i++) {
	if(files[i] == 0) continue;
	kdb = K_RWS[K_VAR][i - 1];
	if(kdb == NULL) {
	    B_seterror("File %d not present", i);
	    SCR_add_ptr(&names, &nf, 0L);
	    SCR_free_tbl(names);
	    return(NULL);
	}
	sprintf(buf, "[%d] %s", i, KNAMEPTR(kdb));
	B_path_change(buf);
	SCR_add_ptr(&names, &nf, buf);
    }
    SCR_add_ptr(&names, &nf, 0L);
    return(names);
}

T_print_files(cls, dim)
COLS    *cls;
int     dim;
{
    int     i;

/*    if(KT_nbnames < 2) return(0);    GB 23-03-98 */
    if(KT_nbnames <= 0) return(0);   /* JMP 11-06-99 */

    for(i = 0; KT_names[i]; i++) {
	T_align(KT_LEFT, dim, KT_STRING); /* JMP 17-12-93 */
	W_printf("%s\n", KT_names[i]);
    }

    return(0);
}

T_print_mode(cls, dim)
COLS    *cls;
int     dim;
{
    COL   *cl;
    int    i;
    extern char *KLG_OPERS_TEXTS[][3];

    for(i = 0; i < MAX_MODE; i++) {
	if(KT_mode[i] == 0) continue;
	T_align(KT_LEFT, dim, KT_STRING);  /* JMP 17-12-93 */
	W_printf("(%s) %s\n", COL_OPERS[i + 1], KLG_OPERS_TEXTS[i + 1][B_LANG]); /* JMP38 01-10-92 */
    }
    return(0);
}

T_print_date(dim)
int     dim;
{
    long    SCR_current_date();
    char    date[11];

    SCR_long_to_fdate(SCR_current_date(), date, "dd/mm/yy");
    T_align(KT_LEFT, dim, KT_STRING); /* JMP 17-12-93 */
    W_printf( "%s\n", date);
    return(0);
}

T_begin_tbl(dim, cls)
int     dim;
COLS    *cls;
{
    KT_names = T_find_files(cls);
    KT_nbnames = SCR_tbl_size(KT_names);
    if(KT_nbnames == 0) return(-1); /* JMP 11-06-99 */
    COL_find_mode(cls, KT_mode, 2);

    W_printf( ".tb %d\n", dim);
    if(KT_sep == '\t') W_printf( ".sep TAB\n");
    else W_printf( ".sep %c\n", KT_sep);
    return(0);
}

T_end_tbl()
{
    W_printf( ".te \n");
    SCR_free_tbl(KT_names);
    KT_names = NULL;
    KT_nbnames = 0;
    return(0);
}





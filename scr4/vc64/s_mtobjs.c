#include <scr.h>

#include "s_mt.h"

#if !defined(DOSWIN) && !defined(SCRW32) && !defined(X11)

YYKEYS _SPECIAL_KEYS[105] = {
	"\000<", 3002,	"\000=", 3003,	"\000>", 3004,	"\000?", 3005,	"\000@", 3006,	
	"\000A", 3007,	"\000B", 3008,	"\000C", 3009,	"\000D", 3010,	"\000^", 3201,	
	"\000_", 3202,	"\000`", 3203,	"\000a", 3204,	"\000b", 3205,	"\000c", 3206,	
	"\000d", 3207,	"\000e", 3208,	"\000f", 3209,	"\000g", 3210,	"\000T", 3101,	
	"\000U", 3102,	"\000V", 3103,	"\000W", 3104,	"\000X", 3105,	"\000Y", 3106,	
	"\000Z", 3107,	"\000[", 3108,	"\000\134", 3109,	"\000]", 3110,	"\000h", 3301,	
	"\000i", 3302,	"\000j", 3303,	"\000k", 3304,	"\000l", 3305,	"\000m", 3306,	
	"\000n", 3307,	"\000o", 3308,	"\000p", 3309,	"\000q", 3310,	"\000I", 2004,	
	"\000Q", 2005,	"\000H", 2000,	"\000P", 2001,	"\000K", 2002,	"\000M", 2003,	
	"\000G", 2006,	"\000O", 2007,	"\000R", 1000,	"\000S", 1001,	"\016", 1213,	
	"\003\003\003", 1010,	"\000.", 1302,	"\000 ", 1303,	"\001", 1200,	"\000,", 1325,	
	"\011", 9019,	"\012", 9024,	"\000;", 9025,	"\015", 1002,	"\020", 1215,	
	"\000\020", 1316,	"\023", 1218,	"\003", 1202,	"\026", 1008,	"\000\022", 1304,	
	"\025", 1220,	"\000\021", 1322,	"\030", 1223,	"\032", 1225,	"\033", 1006,	
	"\000\017", 1103,	"\000\204", 2204,	"\000v", 2205,	"\000s", 2202,	"\000t", 2203,	
	"\000w", 2206,	"\000u", 2207,	"\000\036", 1300,	"\0000", 1301,	"\000.", 1302,	
	"\000 ", 1303,	"\000\022", 1304,	"\000!", 1305,	"\000\042", 1306,	"\000;", 1307,	
	"\000\027", 1308,	"\000$", 1309,	"\000%", 1310,	"\000&", 1311,	"\0002", 1312,	
	"\0001", 1313,	"\000\030", 1314,	"\000\031", 1315,	"\000\020", 1316,	"\000\023", 1317,	
	"\000\037", 1318,	"\000\024", 1319,	"\000\026", 1320,	"\000/", 1321,	"\000\021", 1322,	
	"\000-", 1323,	"\000\025", 1324,	"\000,", 1325,	"\031", 9202,	"\031", 2208,	
	
};

YYKEYS _CAPABILITIES[7] = {
	"\033[2J", 6056,	"\033[%d;%dH", 6057,	"\033[0m", 1,	"\033[1m", 4,	"\033[4m", 3,	
	"\033[5m", 5,	"\033[7m", 2,	
};
#endif
#if !defined(DOSWIN) && !defined(SCRW32) && !defined(X11)

YYKEYS _MAP_OUT[256] = {
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	0, 0,	0, 0,	0, 0,	0, 0,	
	0, 0,	
};
#endif

SCR_read_driver()
{
#if !defined(DOSWIN) && !defined(SCRW32) && !defined(X11)
	SCR_PAGE_SIZE[0] = 25;
	SCR_PAGE_SIZE[1] = 80;
#endif
	SCR_FILL_SCREEN  = ' ';
#if defined(DOSWIN) || defined(SCRW32) || defined(X11)
	SCR_NB_KEYS      = 0;
	SCR_NB_CAPS      = 0;
	SCR_SPECIAL_KEYS = 0;
	SCR_CAPABILITIES = 0;
	SCR_MAP_OUT      = 0;
#else
	SCR_NB_KEYS      = 105;
	SCR_NB_CAPS      = 7;
	SCR_SPECIAL_KEYS = (YYKEYS *)_SPECIAL_KEYS;
	SCR_CAPABILITIES = (YYKEYS *)_CAPABILITIES;
	SCR_MAP_OUT      = (YYKEYS *)_MAP_OUT;
#endif
}
#ifdef SCRW32
#include <scr4w.h>
#else
#include <scr4.h>
#endif
FNKEYS *scr_f3f10;
PAGE *scr_prt;
PAGE *scr_conf;
PAGE *scr_err;
MENU *scr_bcm1;
MENU *scr_bcm2;
PAGE *scr_incr;
	OBJ scr_incr_nbl = {(char *)0, 0, 9500};
	OBJ scr_incr_nbs = {(char *)0, 1, 9500};
A_BAR *scr_abedis;
MENU *scr_bcie;
PAGE *pg_ap_isam;
APPL *scr_ap_isam;
FNKEYS *fk_esc;
FNKEYS *fk_f1;
FNKEYS *fk_f5;
FNKEYS *fk_f6;
FNKEYS *fk_f7;
FNKEYS *fk_f8;
FNKEYS *fk_f10;
FNKEYS *tt_enter;
FNKEYS *tt_kgene;
MENU *tt_yes;
MENU *tt_no;
PAGE *tt_add;
PAGE *tt_chang;
PAGE *tt_fill;
PAGE *tt_name;
PAGE *tt_pglin;
PAGE *tt_searc;
PAGE *tt_ttab;
MENU *ttm_just;
MENU *ttm_del;
PAGE *tt_margin;
PAGE *tt_findn;
PAGE *tt_chn;
MENU *ttm_edimp;
ACTION *tta_imp;
PAGE *ttp_printer;
PAGE *tt_configure;
PAGE *ttp_print;
PAGE *ttp_nbexep;
PAGE *ttp_key;
MENU *ttm_key;
MENU *ttm_insert;
MENU *ttm_decorate;
MENU *ttm_margins;
MENU *ttm_vars;
MENU *ttm_imps;
PAGE *tt_grep;
PAGE *tt_titre;
	OBJ tt_titre_sn = {(char *)0, 0, 9500};
	OBJ tt_titre_user = {(char *)0, 1, 9500};
PAGE *tt_pcomm;
PAGE *tt_vline;
APPL *tt_amain;
PAGE *tt_dir;
MENU *ttm_box;
#include "s_mtobjs.h"

#include <scr4_rd.h>
extern char *OBJ_read();
PAGE *PG_read(name)
char	*name;
{
	return((PAGE *)OBJ_read(name, SCR_TYPE_PAGE));
}
A_BAR *AB_read(name)
char	*name;
{
	return((A_BAR *)OBJ_read(name, SCR_TYPE_ABAR));
}
APPL *AP_read(name)
char	*name;
{
	return((APPL *)OBJ_read(name, SCR_TYPE_APPL));
}
REPORT *RP_read(name)
char	*name;
{
	return((REPORT *)OBJ_read(name, SCR_TYPE_REPORT));
}
MENU *MN_read(name)
char	*name;
{
	return((MENU *)OBJ_read(name, SCR_TYPE_MENU));
}
ISAM *IS_read(name)
char	*name;
{
	return((ISAM *)OBJ_read(name, SCR_TYPE_ISAM));
}
MPAGE *MP_read(name)
char	*name;
{
	return((MPAGE *)OBJ_read(name, SCR_TYPE_MPAGE));
}
BC *BC_read(name)
char	*name;
{
	return((BC *)OBJ_read(name, SCR_TYPE_BC));
}
ACTION *ACT_read(name)
char	*name;
{
	return((ACTION *)OBJ_read(name, SCR_TYPE_ACT));
}
PR_DEF *PR_read(name)
char	*name;
{
	return((PR_DEF *)OBJ_read(name, SCR_TYPE_PRINTER));
}
FNKEYS *FNK_read(name)
char	*name;
{
	return((FNKEYS *)OBJ_read(name, SCR_TYPE_FNKEYS));
}
TPAGES *TP_read(name)
char	*name;
{
	return((TPAGES *)OBJ_read(name, SCR_TYPE_TPAGES));
}
WTOOLBAR *TB_read(name)
char	*name;
{
	return((WTOOLBAR *)OBJ_read(name, SCR_TYPE_WTOOLB));
}
SCR_read_apps(char *filename){return(0);}

/*NO ISAM FUNCTIONS*/
/*-----------------*/
SCR_apply_cmp(PAGE *pg, int i)    {return(0);}
SCR_apply_all_cmp(PAGE *pg)       {return(0);}
SCR_display_cmp(PAGE *pg, int i)  {return(0);}
ACT_exec_aa_isam(A_ACTION *aa)    {return(0);}
BC_display(BC *bc)                {return(0);}
BC_undisplay(BC *bc)              {return(0);}
BC_empty(BC *bc)                  {return(0);}
IS_close_all()                    {return(0);}
PG_check_nodup(PAGE *p, FIELD *f) {return(0);}
char *IS_get_field(ISAM *is, int i, char *ptr) {return(0);}
IS_get_read_field(ISAM *is, int i, char *ptr) {return(0);}
IS_set_field(ISAM *is, int i, char *ptr) {return(0);}
OBJ_mpage_adjust(MPAGE *mp) {return(0);}
OBJ_read_bc(BC *bc)         {return(0);}
OBJ_read_mpage(MPAGE *mp)   {return(0);}
OBJ_read_isam(ISAM *is)     {return(0);}
#if defined(SCRW32)
WmpSetScroll(HWND hwnd, MPAGE *mp) {return(0);}

WbcSetScroll(HWND hwnd, BC *bc)    {return(0);}

WscrMpNbFlds(HWND hwnd, MPAGE *mp) {return(0);}

WmpEdit(HWND hwnd, MPAGE *mp)      {return(0);}

#endif

/*NO MMT FUNCTIONS*/
/*----------------*/
PG_create_fld_mmt(PAGE *pg, int i, int j) {return(0);}
PG_editor(PAGE *pg, int i) {return(0);}
SCR_set_field_editor(PAGE *pg, int i, unsigned char *txt) {return(0);}
SCR_set_field_editor_title(PAGE *pg, int i, unsigned char *txt) {return(0);}
PG_free_editor(PAGE *pg) {return(0);}

/*NO FMT FUNCTIONS*/
/*----------------*/
PG_edit_fmt(PAGE *pg, int i) {return(0);}
PG_set_dft_fmt(PAGE *pg, char *txt, int i)  {return(0);}
PG_is_fmt_empty(char *a, char *b) {return(0);}
unsigned char *PG_fmt_text(char *a, char *b, char *c)  {return(0);}
unsigned char *PG_ufmt_text(char *a, char *b, char *c) {return(0);}
unsigned char *PG_fmt_long(unsigned char *a, unsigned char *b, unsigned long c) {return(0);}
unsigned long PG_ufmt_long(char *a, char *b)  {return(0);}

/*NO ZOOM FUNCTIONS*/
/*-----------------*/
IMAGE   *PG_edfl_zoom(PAGE *pg, int i) {return((IMAGE *)0);}
PG_edfl_unzoom(FIELD *a,int b,int c,int d,int e,IMAGE *f)  {return(0);}

/*NO DATE IN PAGES*/
/*-----------------*/
PG_check_fmt_date(char *a, char *b) {return(0);}
long PG_fdate_to_long(char *a, char *b) {return(0L);}

/*NO TIME IN PAGES*/
/*-----------------*/
PG_check_fmt_time(char *a, char *b) {return(0);}
long PG_ftime_to_long(char *a, char *b) {return(0L);}

/*NO DRAWS IN PAGES*/
/*-----------------*/
PG_read_drws(PAGE *pg) {return(0);}
PG_drw_free(PAGE *pg)  {return(0);}

/*NO COMMENTS*/
/*-----------*/
SCR_get_comment(long l, int i, int j) {return(0);}
SCR_comment(unsigned char *txt) {return(0);}
SCR_display_comment(long l, int i, int j) {return(0);}
SCR_display_cmt(int i) {return(0);}
SCR_scmt_free() {return(0);}
SCR_scmt_add_key(int i, char *txt) {return(0);}
SCR_scmt_add_fnk(int i) {return(0);}
SCR_scmt_set_keys(YYKEYS *yk) {return(0);}
SCR_scmt_set_txt() {return(0);}
SCR_scmt_txt(char *a) {return(0);}
SCR_read_cmt_txt(long l) {return(0);}
SCR_scmt_mouse() {return(0);}
SCR_scmt_menu(int i, int j) {return(0);}
SCR_scmt_display() {return(0);}

/*NO PRINTER FUNCTIONS*/
/*--------------------*/
PR_action(A_ACTION *aa) {return(-1);}
OBJ_read_prt(PR_DEF *pr){return(-1);}
extern	PR_DEF	*PR_CURRENT;

extern	FILE_TABLE	 SCR_READ_FILES;

void C_init_ptrs()
{
	FILE_TYPE  **flt = SCR_READ_FILES.tbl_fl;
	scr_f3f10 = (FNKEYS *)flt[1]->fl_ptr;
	scr_prt = (PAGE *)flt[3]->fl_ptr;
	scr_conf = (PAGE *)flt[4]->fl_ptr;
	scr_err = (PAGE *)flt[5]->fl_ptr;
	scr_bcm1 = (MENU *)flt[7]->fl_ptr;
	scr_bcm2 = (MENU *)flt[8]->fl_ptr;
	scr_incr = (PAGE *)flt[9]->fl_ptr;
	scr_incr_nbl.obj_ptr = (char *) scr_incr;
	scr_incr_nbs.obj_ptr = (char *) scr_incr;
	scr_abedis = (A_BAR *)flt[10]->fl_ptr;
	scr_bcie = (MENU *)flt[11]->fl_ptr;
	pg_ap_isam = (PAGE *)flt[12]->fl_ptr;
	scr_ap_isam = (APPL *)flt[13]->fl_ptr;
	fk_esc = (FNKEYS *)flt[23]->fl_ptr;
	fk_f1 = (FNKEYS *)flt[25]->fl_ptr;
	fk_f5 = (FNKEYS *)flt[27]->fl_ptr;
	fk_f6 = (FNKEYS *)flt[29]->fl_ptr;
	fk_f7 = (FNKEYS *)flt[31]->fl_ptr;
	fk_f8 = (FNKEYS *)flt[33]->fl_ptr;
	fk_f10 = (FNKEYS *)flt[35]->fl_ptr;
	tt_enter = (FNKEYS *)flt[44]->fl_ptr;
	tt_kgene = (FNKEYS *)flt[45]->fl_ptr;
	tt_yes = (MENU *)flt[46]->fl_ptr;
	tt_no = (MENU *)flt[47]->fl_ptr;
	tt_add = (PAGE *)flt[48]->fl_ptr;
	tt_chang = (PAGE *)flt[52]->fl_ptr;
	tt_fill = (PAGE *)flt[53]->fl_ptr;
	tt_name = (PAGE *)flt[54]->fl_ptr;
	tt_pglin = (PAGE *)flt[55]->fl_ptr;
	tt_searc = (PAGE *)flt[56]->fl_ptr;
	tt_ttab = (PAGE *)flt[57]->fl_ptr;
	ttm_just = (MENU *)flt[60]->fl_ptr;
	ttm_del = (MENU *)flt[61]->fl_ptr;
	tt_margin = (PAGE *)flt[62]->fl_ptr;
	tt_findn = (PAGE *)flt[65]->fl_ptr;
	tt_chn = (PAGE *)flt[66]->fl_ptr;
	ttm_edimp = (MENU *)flt[67]->fl_ptr;
	tta_imp = (ACTION *)flt[72]->fl_ptr;
	ttp_printer = (PAGE *)flt[73]->fl_ptr;
	tt_configure = (PAGE *)flt[74]->fl_ptr;
	ttp_print = (PAGE *)flt[79]->fl_ptr;
	ttp_nbexep = (PAGE *)flt[80]->fl_ptr;
	ttp_key = (PAGE *)flt[83]->fl_ptr;
	ttm_key = (MENU *)flt[84]->fl_ptr;
	ttm_insert = (MENU *)flt[85]->fl_ptr;
	ttm_decorate = (MENU *)flt[86]->fl_ptr;
	ttm_margins = (MENU *)flt[87]->fl_ptr;
	ttm_vars = (MENU *)flt[88]->fl_ptr;
	ttm_imps = (MENU *)flt[95]->fl_ptr;
	tt_grep = (PAGE *)flt[96]->fl_ptr;
	tt_titre = (PAGE *)flt[97]->fl_ptr;
	tt_titre_sn.obj_ptr = (char *) tt_titre;
	tt_titre_user.obj_ptr = (char *) tt_titre;
	tt_pcomm = (PAGE *)flt[98]->fl_ptr;
	tt_vline = (PAGE *)flt[99]->fl_ptr;
	tt_amain = (APPL *)flt[100]->fl_ptr;
	tt_dir = (PAGE *)flt[179]->fl_ptr;
	ttm_box = (MENU *)flt[180]->fl_ptr;
}
#include <scr.h>

#include "s_mt.h"

#include "s_mtobjs.h"


ACT_functions(fn)
int    fn;
{
	switch(fn) {
	case   0:
TT_list_old_file()
	;break;
	case   1:
TT_prev_file()
	;break;
	case   2:
TT_standart_tab()
	;break;
	case   3:
TT_standard_marge()
	;break;
	case   4:
TT_create_imp()
	;break;
	case   5:
TT_modify_imp()
	;break;
	case   6:
TT_delete_imp()
	;break;
	case   7:
TT_select_printer()
	;break;
	case   8:
TT_current_imp = MN_get_choice(ttm_chimp)
	;break;
	case   9:
TT_imp_modify_line()
	;break;
	case  10:
TT_help_reset()
	;break;
	case  11:
TT_set_clock()
	;break;
	case  12:
ttp_nbexep_FLINE = 1;
            ttp_nbexep_FPAGE = 1
	;break;
	case  13:
TT_print_txt()
	;break;
	case  14:
TT_set_dft_mode(TT_HPD_MODE)
	;break;
	case  15:
TT_set_dft_mode(TT_IBM_MODE)
	;break;
	case  16:
TT_set_dft_mode(TT_TOSH_MODE)
	;break;
	case  17:
TT_imp_set_val()
	;break;
	case  18:
TT_imp_get_val()
	;break;
	case  19:
TT_edit_txt()
	;break;
	case  20:
return(TT_add_new_file())
	;
	case  21:
TT_vieuw_file()
	;break;
	case  22:
TT_edit_next_file()
	;break;
	case  23:
TT_add_file_error()
	;break;
	case  24:
TT_view_back()
	;break;
	case  25:
TT_save_tt(1);TT_display_comm()
	;break;
	case  26:
return(TT_quit_file())
	;
	case  27:
return(TT_save_all())
	;
	case  28:
TT_change_name()
	;break;
	case  29:
TT_chdir()
	;break;
	case  30:
TT_system(tt_configure_SYSTEM)
	;break;
	case  31:
TT_system(tt_configure_vALTF[0])
	;break;
	case  32:
TT_system(tt_configure_vALTF[1])
	;break;
	case  33:
TT_system(tt_configure_vALTF[2])
	;break;
	case  34:
TT_mark_block()
	;break;
	case  35:
TT_mark_line()
	;break;
	case  36:
TT_unmark_block()
	;break;
	case  37:
TT_delete_block()
	;break;
	case  38:
TT_copy_block()
	;break;
	case  39:
TT_move_block()
	;break;
	case  40:
TT_copy_block_overlay_1()
	;break;
	case  41:
TT_move_block_left()
	;break;
	case  42:
TT_move_block_right()
	;break;
	case  43:
TT_block_lower()
	;break;
	case  44:
TT_block_upper()
	;break;
	case  45:
TT_fill_block()
	;break;
	case  46:
TT_block_del_bl()
	;break;
	case  47:
TT_del_ds()
	;break;
	case  48:
TT_execute_mark()
	;break;
	case  49:
TT_print_text_block()
	;break;
	case  50:
TT_reflow_par()
	;break;
	case  51:
TT_switch()
	;break;
	case  52:
TT_move_begin_file()
	;break;
	case  53:
TT_move_end_file()
	;break;
	case  54:
TT_move_to_line(0, 0)
	;break;
	case  55:
TT_next_word()
	;break;
	case  56:
TT_prev_word()
	;break;
	case  57:
TT_c_pg_up()
	;break;
	case  58:
TT_c_pg_down()
	;break;
	case  59:
TT_search_string()
	;break;
	case  60:
TT_change_string()
	;break;
	case  61:
TT_grep_string2()
	;break;
	case  62:
TT_load_filel()
	;break;
	case  63:
TT_execute_key()
	;break;
	case  64:
TT_record_key()
	;break;
	case  65:
TT_change_type_write()
	;break;
	case  66:
TT_edit_marge()
	;break;
	case  67:
TT_activate_marge()
	;break;
	case  68:
TT_variables()
	;break;
	case  69:
TT_edit_tab()
	;break;
	case  70:
TT_insert_menu()
	;break;
	case  71:
TT_del_begin_line()
	;break;
	case  72:
TT_del_end_line()
	;break;
	case  73:
TT_insert_l()
	;break;
	case  74:
TT_joint_line()
	;break;
	case  75:
TT_kill_line()
	;break;
	case  76:
TT_tronc_line()
	;break;
	case  77:
TT_underline()
	;break;
	case  78:
TT_help()
	;break;
	case  79:
SCR_config_screen()
	;break;
	case  80:
SCR_print_screen()
	;break;
	case  81:
SCR_refresh()
	;break;
	case  82:
SCR_lock_screen("MT 4.0 - Press any key")
	;break;
	default: break;
	}
	return(0);
}

char SCR_NAME_SCR[SCR_MAX_FILE_LG_MAX + 1];

SCR_init()
{
	SCR_CLOCK_POS[0] = 1;
	SCR_CLOCK_POS[1] = 60;
	SCR_CLOCK_SECS   = 1;
	SCR_DATE_POS[0] = -1;
	SCR_DATE_POS[1] = 72;
	SCR_MEM_POS[0] = -1;
	SCR_MEM_POS[1] = 72;
	SCR_KEY_POS[0] = -1;
	SCR_KEY_POS[1] = 72;

	SCR_CMT.c_line   = -1;
	SCR_CMT.c_col    = 0;
	SCR_CMT.c_nl     = 1;
	SCR_CMT.c_nc     = 80;
	SCR_CMT.c_attr   = 14;
	SCR_CMT.c_center = 0;
	SCR_NB_DEC       = -1;

	SCR_openmenu     = 0;
	SCR_limdate      = 0;
	SCR_username     = 0;
	SCR_applname     = (unsigned char *)"Multi-Text Editor v4.0 - SCR/AL1 Tools";
	SCR_serialnb     = 0;
	SCR_crdate       = 20170710;
	SCR_crtime       = 123236;
	SCR_actkey       = 0;

	SCR_LOCK_TXT      = "SCR/AL1 Version 4 (c) Press any key";
	SCR_LOCK_TIME     = 300;
	strcpy(SCR_LANGUAGE, (unsigned char *)"f");
	SCR_mini_init();
	SCR_sn();
	if(SCR_NAME_SCR[0] != 0)
		SCR_read_obj(SCR_NAME_SCR);
	else
		SCR_read_obj((unsigned char *)"s_mtobjs.scr");

	if(SCR_CMT_FILE[0] == 0)
		strcpy(SCR_CMT_FILE, (unsigned char *)"s_mtobjs.cmt");

	C_init_ptrs();
	SCR_base_init();
	WSCR_ATTRS[0] = 0;
	WSCR_ATTRS[1] = 0;
	WSCR_ATTRS[2] = 0;
	WSCR_ATTRS[3] = 0;
	WSCR_ATTRS[4] = 0;
	WSCR_ATTRS[5] = 0;
	WSCR_ATTRS[6] = 0;
	WSCR_ATTRS[7] = 0;
	WSCR_ATTRS[8] = 0;
	WSCR_ATTRS[9] = 0;
	WSCR_ATTRS[10] = 0;
	WSCR_ATTRS[11] = 0;
	WSCR_ATTRS[12] = 0;
	WSCR_ATTRS[13] = 0;
	WSCR_ATTRS[14] = 0;
	WSCR_ATTRS[15] = 0;
	WSCR_ATTRS[16] = 0;
	WSCR_ATTRS[17] = 0;
	WSCR_ATTRS[18] = 0;
	WSCR_ATTRS[19] = 0;
	WSCR_NOCOLOR = 0;
}


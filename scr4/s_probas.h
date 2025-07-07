#ifdef __cplusplus
extern "C" {
#endif


/* s_act.c */
extern int ACT_exec_nu(int nu);
extern int ACT_exec(ACTION *act);
extern int ACT_exec_aa(A_ACTION *aa,int flag);

/* s_actsys.c */
extern int ACT_exec_system(char *cmd);

/* s_actcr.c */
extern long ACT_find_lg(A_ACTION *aa,int i);
extern double ACT_find_db(A_ACTION *aa,int i);
extern char *ACT_find_str(A_ACTION *aa,int i);

/* s_uact.c */
extern int ACT_ufn_exec(unsigned int handle);
extern int ACT_ufn_get_old(unsigned int handle);
extern int ACT_ufn_install_old(int old_handle,int (*fn)(),int u_Int,char *u_ptr);
extern int ACT_ufn_install(int (*fn)(),int u_Int,char *u_ptr);
extern int ACT_ufn_uninstall(int handle);

/* s_menu.c */
extern int MN_init(MENU *mn);
extern int MN_init_option(OPTION *opt);
extern MENU *MN_create(void);
extern int MN_free_option(OPTION *opt);
extern int MN_free(MENU *mn);
extern OPTION *MN_add_option(MENU *mn);
extern int MN_end(MENU *mn);
extern int MN_option_end(void);
extern MENU *MN_create_default(int line,int col,int back,int nb_line);
extern int MN_search_option(MENU *mn,unsigned char *txt);
extern int SCR_search_menu(MENU *mn,unsigned char *txt);
extern MENU *MN_auto_create(char **text,int line,int col,int nlv,int box,int back);

/* s_derr.c */
extern int ERR_display(char *fmt,char *parm1,char *parm2,char *parm3);

/* s_mnch.c */
extern int MN_set_toggle(MENU *mn,int opt_nb,int toggle);
extern int MN_get_toggle(MENU *mn,int opt_nb);
extern int MN_set_choice(MENU *mn,int choice);
extern int MN_get_choice(MENU *mn);
extern int MN_del_option(MENU *mn,int opt_nb);

/* s_cnf.c */
extern char *CNF_filename(void);
extern int CNF_load(void);
extern int CNF_save(void);
extern int CNF_map_attr(int attr);

/* s_cnfed.c */
extern int SCR_config_screen(void);
extern int CNF_write_buf(int line,int col,unsigned char *buf);
extern int CNF_display_cols(int col, int bf);
extern int CNF_display(int co);

/* s_mnauto.c */
extern int MN_edit_auto(YYKEYS *ytbl,int l,int c,int attr);

/* s_cmtmn.c */
extern int MN_read_opt(MENU *mn);
extern int MN_free_opt(MENU *mn);

/* s_wri.c */
extern int SCR_write_center(int line,int col,unsigned char *text,int nc);
extern int SCR_write_scr_mem(char mem[],int line,int col,char text[],int lg,int attr);

/* s_base.c */
extern int SCR_background(void);
extern int SCR_base_init(void);
extern int SCR_end(void);
extern int SCR_refresh(void);
extern int SCR_beep(void);
extern int SCR_cls(void);
extern int SCR_write_text(int line,int col,unsigned char *text,int len);

/* s_bastat.c */
extern STATUS *SCR_save_status(void);
extern int SCR_reset_status(STATUS *sta);

/* s_lock.c */
extern int SCR_lock_screen(unsigned char *txt);

/* s_bamini.c */
extern int SCR_mini_init(void);

/* s_caps.c */
extern char *SCR_find_cap(int cap);
extern int SCR_print_cap(int cap);

/* s_dispwd.c */
extern int PG_display_cadre(PAGE *pg);
extern int PG_display_title(PAGE *pg);

/* s_baend.c */
extern int SCR_mini_end(void);

/* s_prtscr.c */
extern int SCR_print_screen(void);

/* s_box.c */
extern int SCR_hline(int type,int line,int col,int nc);
extern int SCR_vline(int type,int line,int col,int nl);
extern int SCR_corner(int type,int line,int col,int corner);
extern int SCR_box(int type,int line,int col,int nl,int nc);
extern int SCR_fill(int ch,int line,int col,int nl,int nc);
extern int SCR_nb_pins(int ch,int dir);
extern int SCR_auto_table(int line,int col,int nl,int nc);
extern int SCR_auto_char(int ch,int l,int r,int u,int d);
extern int SCR_shadow(int type,int orient,int line,int col,int nl,int nc,int attr);
extern int SCR_shadow_0(int orient,int line,int col,int nl,int nc,int attr);
extern int SCR_shadow_1(int orient,int line,int col,int nl,int nc,int attr);
extern int SCR_chg_im_attr(int line,int col,int nl,int nc,int attr);
extern int SCR_fix_im_attr(IMAGE *im,unsigned int attr);
extern int SCR_shadow_2(int orient,int line,int col,int nl,int nc,int attr);
extern int SCR_shadow_3(int orient,int line,int col,int nl,int nc,int attr);
extern int SCR_shadow_4(int orient,int line,int col,int nl,int nc,int attr);

/* s_disp.c */
extern int PG_write_field(PAGE *pg,int fld_nb,char *edtxt);
extern int PG_write_field_cmp(PAGE *pg,int fld_nb,char *edtxt,int flag_cmp);
extern int PG_display_field(PAGE *pg,int fld_nb);
extern int PG_write(PAGE *pg);
extern int PG_write_char(PAGE *pg,int c);
extern int PG_display_im(PAGE *pg);
extern int PG_display_im_pg(PAGE *pg);
extern int PG_undisplay(PAGE *pg);
extern int PG_undisplay_pg(PAGE *pg);
extern int PG_undisplay_1(PAGE *pg);
extern int PG_invert_page(PAGE *pg);
extern int PG_display_nr(PAGE *pg,int rev);
extern int PG_display(PAGE *pg);
extern int PG_display_flds(PAGE *pg);
extern int PG_display_flds_no_cmp(PAGE *pg);
extern int PG_closebox_pos(PAGE *pg,int *line,int *col);

/* s_dispfn.c */
extern int PGd_write_field_cmp(PAGE *pg,int fld_nb,char *edtxt,int flag_cmp);
extern int PGd_display_field(PAGE *pg,int fld_nb);
extern int PGd_display_flds(PAGE *pg);
extern int PGd_display_flds_no_cmp(PAGE *pg);
extern int PG_number_group_radio(PAGE *pg,int fld_nb,int flag);
extern int PG_find_val_radio(PAGE *pg,int nfld);
extern int PG_find_first_radio(PAGE *pg,int nfld);
extern int PG_calc_nummer_radio(PAGE *pg,int fld_nb);
extern int PG_display_all_grpbox(PAGE *pg,int rev);
extern int PG_edfl_isrbr(FIELD *fld,unsigned char *edtxt);
extern int PG_edfl_calc_eof(FIELD *fld,char *edtxt);

/* s_disped.c */
extern int PG_create_fld_mmt(PAGE *pg,int fld_nb,int disp);

/* s_grpbox.c */
extern int PG_set_lc_grp(PAGE *pg,int nb);

/* s_edfl.c */
extern int PG_edit_input_field(PAGE *pg,int fld_nb);
extern int PG_edit_accessible_field(PAGE *pg,int fld_nb);
extern int PG_check_input_field(PAGE *pg,char *edtxt,int fld_nb);
extern int PG_check_natural(char text[]);
extern int PG_check_integer(char text[]);
extern int PG_check_real(char *buf);
extern int PG_edit_menu(PAGE *pg,int fld_nb);
extern int PG_display_mn_fld(PAGE *pg,int fld_nb,MENU *mn);
extern int PG_edit_dir(PAGE *pg,int fld_nb);
extern int PG_key(PAGE *pg,int fld_nb);
extern int PG_get_key(PAGE *pg,int fld_nb);
extern int PG_help_stack(PAGE *pg,int fld_nb);
extern char *PG_search_fld_name(PAGE *pg,int fld_nb);

/* s_edfld.c */
extern int PG_check_fmt_date(char *str,char *fmt);
extern long PG_fdate_to_long(char *txt,char *fmt);

/* s_edflt.c */
extern int PG_check_fmt_time(char *str,char *fmt);
extern long PG_ftime_to_long(char *txt,char *fmt);

/* s_edit.c */
extern int PG_edit_field(PAGE *pg,int fld_nb);
extern int PG_edfl_vsize(FIELD *fld,int pv,char *edtxt);
extern int PG_edfl_calc_pv(FIELD *fld,char *edtxt,int cursor);
extern int PG_edfl_cursor_move(FIELD *fld,int pos[],int cursor);
extern int PG_edfl_hmove(FIELD *fld,int incr,int *cursor);
extern int PG_edfl_vmove(FIELD *fld,int incr,int *cursor);
extern int PG_edfl_mod_char(FIELD *fld,unsigned char *edtxt,int key,int *cursor,unsigned char *allowed);
extern int PG_edfl_del(FIELD *fld,unsigned char *edtxt,int *cursor);
extern int PG_edfl_erase_eof(FIELD *fld,unsigned char *edtxt,int cursor);
extern int PG_edfl_end(FIELD *fld,char *edtxt,int *cursor);
extern int PG_edfl_home(FIELD *fld,char *edtxt,int *cursor);
extern int PG_display_cmt(PAGE *pg,int fld_nb);

/* s_edtp.c */
extern int TP_edit_window(TPAGES *tp);
extern int TP_display(TPAGES *tp);
extern int TP_undisplay(TPAGES *tp);
extern int TP_edit(TPAGES *tp);
extern int TP_calc_size(TPAGES *tp,int *line,int *col,int *nl,int *nc);
extern int TP_display_1(TPAGES *tp,int pgnb);

/* s_edflz.c */
extern IMAGE *PG_edfl_zoom(PAGE *pg,int fld_nb);
extern int PG_edfl_unzoom(FIELD *fld,int nlv,int ncv,int zline,int zcol,IMAGE *im);

/* s_edfmt.c */
extern int PG_edit_fmt(PAGE *pg,int fld_nb);
extern int PG_fmt_next(char *fmt,int cursor);
extern int PG_fmt_prev(char *fmt,int cursor);
extern int PG_set_dft_fmt(PAGE *pg,char *edtxt,int fld_nb);

/* s_getkey.c */
extern int SCR_disp_cdkm(char *msg,int pos[]);
extern int SCR_set_clock(void);
extern int SCR_set_date(void);
extern int SCR_set_mem(void);
extern int SCR_check_key(void);
extern int SCR_check_end_time(void);
extern long TI_nb_sec(long time);
extern int SCR_find_other_keys(int i,unsigned char *buf,int n,int eq);
extern int SCR_get_key(void);
extern int SCR_clear_key(void);

/* s_dumppg.c */
extern int SCR_dump_pg(void);

/* s_pgcol.c */
extern int PG_set_color(PAGE *pg,int attr);

/* s_edmn.c */
extern int MN_display_body(MENU *mn);
extern int MN_display_box(MENU *mn);
extern int MN_display_opt(MENU *mn,int nb,int invert);
extern int MN_adjust(MENU *mn);
extern int MN_calc_first_opt(MENU *mn);
extern int MN_display(MENU *mn);
extern int MN_redisplay(MENU *mn);
extern int MN_display2(MENU *mn);
extern int MN_undisplay(MENU *mn);
extern IMAGE *MN_save(MENU *mn);
extern int MN_edit(MENU *mn);
extern int MN_edit_ab(MENU *mn,int ab_key);
extern int MN_main_loop(MENU *mn,int ab_key);
extern int MN_cmp_letter(int key1,int key2);
extern int MN_find_letter(MENU *mn,int key);
extern int MN_up(MENU *mn);
extern int MN_down(MENU *mn);
extern int MN_pgup(MENU *mn);
extern int MN_pgdn(MENU *mn);
extern int MN_calc_next_choice(MENU *mn);
extern int MN_calc_previous_choice(MENU *mn);
extern int SCR_pos_menu_act(int line,int col,unsigned int u_act);
extern int MN_if_select(MENU *mn,int nb);
extern int MN_get_key(MENU *mn);
extern int MN_mouse_in_menu(MENU *mn);

/* s_mnhlp.c */
extern int MN_help_stack(MENU *mn);
extern int MN_help_stack_opt(MENU *mn);

/* s_edpg.c */
extern int PG_reedit(PAGE *pg);
extern int PG_goto_field(PAGE *pg,int fld_nb);
extern int PG_edit(PAGE *pg);
extern int PG_edit_pg(PAGE *pg);
extern IMAGE *PG_save(PAGE *pg);
extern int PG_pos(PAGE *pg,int *l,int *c,int *nl,int *nc);
extern int PG_updn_field(PAGE *pg,int c_line,int c_col,int direct);
extern int PG_leftright_field(PAGE *pg,int c_line,int c_col,int direct);
extern int PG_move(PAGE *pg);
extern int PG_mouse_in_page(PAGE *pg);
extern int PG_mouse_event(PAGE *pg,int fld_nb,int *cursor);
extern int PG_next_fld_vis(PAGE *pg,int fld_nb,int type,int dir);
extern int PG_next_fld_dft(PAGE *pg,int fld_nb,int dir);
extern int PG_next_edit(PAGE *pg,int fld_nb,int dir);

/* s_env.c */
extern int SCR_read_env(void);

/* s_eded.c */
extern int PG_editor_help(void);
extern int PG_editor_fnk(int key);
extern int PG_editor(PAGE *pg,int fld_nb);

/* s_misc.c */
extern int SCR_invert_attr(int attr);
extern int SCR_dim_attr(int attr);

/* s_pginfo.c */
extern int PG_display_info(char *infos[]);

/* s_pgfn.c */
extern long SCR_get_field_long(PAGE *pg,int fld_nb);
extern int SCR_get_field_short(PAGE *pg,int fld_nb);
extern double SCR_get_field_double(PAGE *pg,int fld_nb);
extern char *SCR_get_field_text(PAGE *pg,int fld_nb,char *text);
extern int SCR_set_field_text(PAGE *pg,int fld_nb,unsigned char *text);
extern int SCR_set_field_double(PAGE *pg,int fld_nb,double val);
extern int SCR_set_field_long(PAGE *pg,int fld_nb,long val);
extern int SCR_set_field_short(PAGE *pg,int fld_nb,int val);
extern int FLD_align_in_rec(FIELD *fld);
extern int FLD_len_in_rec(FIELD *fld);
extern int PG_set_rec(PAGE *pg,int fld_nb,char *txt);
extern int PG_set_rec_txt(PAGE *pg,int fld_nb,char *txt);
extern int PG_get_rec(PAGE *pg,int fld_nb,char *txt);
extern char *PG_aget_rec_txt(PAGE *pg,int fld_nb);
extern char *PG_get_rec_txt(PAGE *pg,int fld_nb,char *txt,int flag);
extern int PG_cmp_val(PAGE *pg,int fld_nb,char *txt);
extern int PG_empty_field(PAGE *pg,int fld_nb);
extern int PG_fld_blanc(unsigned char *txt,int len);
extern char *_SCR_find_buf(unsigned char *buf,int fld_nb);
extern int PG_check_fmt(char *txt,char *fmt);

/* s_obj.c */
extern int SCR_get_vchar(OBJ obj,int i);
extern double SCR_get_vdouble(OBJ obj,int i);
extern float SCR_get_vfloat(OBJ obj,int i);
extern long SCR_get_vlong(OBJ obj,int i);
extern short SCR_get_vshort(OBJ obj,int i);
extern int SCR_set_vchar(OBJ obj,int i,int val);
extern int SCR_set_vdouble(OBJ obj,int i,double d);
extern int SCR_set_vfloat(OBJ obj,int i,double f);
extern int SCR_set_vlong(OBJ obj,int i,long l);
extern int SCR_set_vshort(OBJ obj,int i,int s);
extern int SCR_get_menu(OBJ obj);
extern int SCR_set_menu(OBJ obj,int i);
extern int PG_set_fld_menu(OBJ obj,MENU *mn);

/* s_pgadj.c */
extern int PG_adjust(PAGE *pg);

/* s_objis.c */
extern int SCR_rget_vchar(OBJ obj,int i);
extern double SCR_rget_vdouble(OBJ obj,int i);
extern float SCR_rget_vfloat(OBJ obj,int i);
extern long SCR_rget_vlong(OBJ obj,int i);
extern short SCR_rget_vshort(OBJ obj,int i);
extern char *SCR_rget_vtext(OBJ obj,int i,char *txt);
extern int OBJ_search(OBJ obj,int cond);
extern char *IS_text(OBJ obj);
extern int FLD_set_isam(OBJ obj,int nb,OBJ isf);
extern int FLD_set_check(OBJ obj,int nb,int chk);

/* s_objtxt.c */
extern char *SCR_get_vtext(OBJ obj,int i,void *val);
extern int SCR_set_vtext(OBJ obj,int i,void *txt);

/* s_objpg.c */
extern unsigned char *PG_vtext(OBJ obj,int i);
extern int PG_set_vio(OBJ obj,int i,int type);
extern int PG_set_vfield(OBJ obj,int i,char *text);
extern int PG_get_vfield(OBJ obj,int i,char *text);
extern int PG_set_vattr(OBJ obj,int i,int attr);
extern int PG_display_vobj(OBJ obj,int i);
extern int PG_edit_vobj(OBJ obj,int i);
extern int PG_set_vdfld(OBJ obj,int i,unsigned char *text);
extern int PG_empty_vobj(OBJ obj,int i);
extern int PG_set_grpio(PAGE *pg,int grpnb,int type);

/* s_drvget.c */
extern int SCR_get_term(char *tty,char *term);
extern int SCR_auto_term(void);

/* s_flt.c */
extern FILE_TYPE *SCR_realloc_filetable(char *filename,char *ptr,int type);
extern FILE_TYPE *SCR_add_flt_ptr(char *filename,char *ptr,int type);

/* s_fltnam.c */
extern char *SCR_find_ptr_name(void *ptr);

/* s_fltnu.c */
extern int SCR_find_ptr_nu(void *ptr);

/* s_fltfnd.c */
extern FILE_TYPE *SCR_find_flt(char *filename);
extern char *SCR_find_load_obj(int nu);
extern int SCR_unload_obj(int nu);

/* s_fltfix.c */
extern int SCR_fix_filetable(int nb);

/* s_fltexi.c */
extern FILE_TYPE *SCR_flt_exist(void *ptr,int type);

/* s_fltset.c */
extern int SCR_flt_set_def(int flt_nb,char *name,char *ptr,int type);

/* s_edbox.c */
extern int SCR_in_box(int cl,int cc,int l,int c,int nl,int nc);
extern int SCR_on_box(int cl,int cc,int l,int c,int nl,int nc);
extern int SCR_on_corner(int cl,int cc,int l,int c,int nl,int nc);
extern int SCR_box_move(int *l,int *c,int nl,int nc,int bl,int bc,int bnl,int bnc);
extern int SCR_box_resize(int l,int c,int *nl,int *nc,int minl,int minc,int maxl,int maxc);
extern int SCR_box_mvsz(int *l,int *c,int nl,int nc,int bl,int bc,int bnl,int bnc,int type);
extern int SCR_box_save(IMAGE *ims[],int l,int c,int nl,int nc);
extern int SCR_box_reset(IMAGE *ims[]);
extern int SCR_box_attrs(unsigned long attrs,unsigned int l,unsigned int c,unsigned int nl,unsigned int nc,unsigned int attr);
extern int SCR_box_edit(unsigned long attrs,unsigned int l,unsigned int c,unsigned int nl,unsigned int nc,unsigned int attr);
extern int SCR_edit_button(int line,int col,int nl,int nc,int attr);
extern int SCR_check_button(int line,int col,int nl,int nc,int attr);

/* s_msgbox.c */
extern int SCR_MessageBox(unsigned char *title,unsigned char *msg,unsigned char *buts[]);
extern int SCR_MessageBox1(unsigned char *title,unsigned char *msg,unsigned char *buts[],int ncurb,int required);
extern int SCR_button_letter(unsigned char *txt);
extern int SCR_display_button(unsigned char *txt,int line,int col,int stat);

/* s_keytxt.c */
extern char *SCR_fnk_txt(int key);

/* s_pgmmt.c */
extern int SCR_set_field_editor_title(PAGE *pg,int fld_nb,unsigned char *title);
extern unsigned char *SCR_get_field_editor(PAGE *pg,int fld_nb);
extern int SCR_set_editor(OBJ obj,unsigned char *text);
extern unsigned char *SCR_get_editor(OBJ obj);
extern int PG_free_editor(PAGE *pg);

/* s_cmtbp.c */
extern int SCR_read_cmt_len(long pos,char *txt);
extern int SCR_read_cmt(long pos,char *txt,int len);

/* s_prof.c */
extern int SCR_prof_filename(char *filename,char *res);
extern int SCR_prof_save_pwd(char *filename,char *passwd);
extern int SCR_prof_save(char *filename);
extern int SCR_prof_wpage(FILE *fd,FILE_TYPE *flt);
extern int SCR_prof_wmenu(FILE *fd,FILE_TYPE *flt);
extern int SCR_prof_wpwd(FILE *fd,char *passwd);
extern FILE *SCR_prof_open(char *filename);
extern int SCR_prof_load_pwd(char *filename,char *passwd);
extern int SCR_prof_load(char *filename);
extern int SCR_prof_rpage(FILE *fd,PAGE *pg);
extern int SCR_prof_rmenu(FILE *fd,MENU *mn);
extern int SCR_prof_rpwd(FILE *fd,char *passwd);
extern int SCR_prof_reset(void);

/* s_atof.c */
extern double SCR_atof(char *txt);

/* s_pgrset.c */
extern int PG_reset_01(PAGE *pg,int flag);
extern int PG_reset(PAGE *pg);
extern int PG_reset_all(PAGE *pg);
extern int PG_reset_ch(PAGE *pg,int flag);

/* s_vars.c */

/* s_cfn.c */
extern int SCR_add_cfn(char *fn_name);
extern int SCR_add_new_cfn(char *fn_name);
extern int SCR_find_cfn(char *fn_name);

/* s_ab.c */
extern int AB_display(A_BAR *ab);
extern int AB_display_option(A_BAR *ab,int nu_opts,int attr);
extern int AB_opt_pos(A_BAR *ab,int nu_opt);
extern int AB_edit(A_BAR *ab);
extern int AB_get_key(A_BAR *ab,int choice);
extern int AB_mouse_in_ab(A_BAR *ab);

/* s_abhlp.c */
extern int AB_help_stack(A_BAR *ab,int choice);

/* s_drw.c */
extern int DRW_display_nr(DRAWS *dw,int line,int col,int nl,int nc,int rev);
extern int DRW_display(DRAWS *dw,int line,int col,int nl,int nc);
extern int DRW_display_reverse(DRAWS *dw,int line,int col,int nl,int nc);
extern int DRW_display_drw(DRAW *drw,int line,int col,int nl,int nc);

/* s_dispf.c */
extern unsigned char **SCR_read_file(char *filename);
extern int SCR_save_file(char *filename,char **ptr);
extern int PG_display_file(char *filename);

/* s_anum.c */
extern long SCR_read_long(FILE *fd);
extern long SCR_current_auto_num(ISAM *is);
extern long SCR_auto_num(char *filename);

/* s_ap.c */
extern int AP_display(APPL *ap);
extern int AP_undisplay(APPL *ap);
extern int AP_init_disp(APPL *ap);
extern int AP_edit(APPL *ap);
extern int SCR_app_key(void);
extern int APP_key(int key);
extern int AP_run(APPL *ptr);
extern int SCR_app_mouse(void);

/* s_hlp.c */
extern int HLP_close(void);
extern int HLP_find_nbr(char *name);
extern int HLP_cmp(unsigned char *read,unsigned char *txt,unsigned char *utxt);
extern int HLP_read_struct(int nb,HELP *hlp);
extern int HLP_read_name(int nb,char *name);
extern int HLP_get_help(int nb,HELP *hlp);
extern int HLP_free(HELP *hlp);
extern int HLP_find_eq(char *name);
extern int HLP_find(char *name);
extern int HLP_add_topic(int nbr,int *l_nb_help,int *l_help,int *l_cline,int *l_ckeyw);
extern int HLP_save_current(int l_cur_help,int *l_cline,int *l_ckeyw);
extern int HLP_edit(char *name);
extern int HLP_display(HELP *hlp);
extern int HLP_display_box(HELP *hlp);
extern int HLP_display_text(HELP *hlp);
extern int HLP_display_keyw(HELP *hlp);
extern int HLP_display_1keyw(HELP *hlp,int keyw_nb);
extern int HLP_check_keyw(HELP *hlp,int i);
extern int HLP_first_keyw(HELP *hlp);
extern int HLP_last_keyw(HELP *hlp);
extern int HLP_change_keyw(HELP *hlp,int oldkw,int newkw);
extern int HLP_next_keyw(HELP *hlp);
extern int HLP_prev_keyw(HELP *hlp);
extern int HLP_sqz(unsigned char *text);
extern int HLP_open_msg(int msg);
extern int HLP_alloc_im(void);
extern int HLP_history(int *l_help,int nb);
extern int HLP_maximize(void);
extern int HLP_resize(void);
extern int HLP_move(void);
extern int HLP_mouse_keyw(HELP *hlp);
extern int HLP_mouse_event(HELP *hlp);
extern int HLP_scmts(YYKEYS *keys);
extern int HLP_stack_edit(void);

/* s_hlpstk.c */
extern int HLP_stack_add(char *help);
extern int HLP_stack_del(int n);

/* s_pgcmt.c */

/* s_vers.c */
extern int SCR_version(void);

/* s_magic.c */
extern int SCR_write_magic(FILE *fd,unsigned char *txt);
extern int SCR_check_magic(FILE *fd,unsigned char *txt);

/* s_sys.c */
extern int SCR_ctrlbrk(void);
extern int SCR_term_vkey(int val);
extern int SCR_init_term(void);
extern int SCR_end_term(void);
extern int SCR_clear_screen(void);
extern int SCR_force_attr(int attr);
extern int SCR_set_font(int font);
extern int SCR_set_color(int color);
extern int SCR_cursor_on(void);
extern int SCR_cursor_off(void);
extern int SCR_cursor_move(int line,int col);
extern int SCR_cursor_size(int size);
extern int SCR_force_cursor_size(int size);
extern int SCR_map_box_2(void);
extern int SCR_print_text(char *text,int lg);
extern int SCR_get_no_echo(void);
extern int SCR_hit_key(void);
extern int SCR_getch(void);
extern int SCR_set_alarm(int sec, void    (*fn)());
extern int SCR_stop_alarm(void);
extern int SCR_restart_alarm(void);
extern int MS_init(void);
extern int MS_end(void);
extern int MS_getevent(void);
extern int MS_show_attr(void);
extern int MS_hide_attr(void);
extern int MS_hide(void);
extern int MS_show(void);

/* s_cmt.c */
extern int SCR_comment(unsigned char *txt);
extern int SCR_display_cmt(int flag);
extern int SCR_display_comment(long pos1,int pos2,int flag);

/* s_cmtdrw.c */
extern int PG_read_drws(PAGE *pg);
extern int PG_drw_free(PAGE *pg);

/* s_cmtget.c */
extern int SCR_get_cmt(int flag,long pos);
extern int SCR_get_cmt_ele(long pos1,int pos2);
extern int SCR_read_cmt_txt(long pos);
extern int SCR_get_comment(long pos1,int pos2,int flag);

/* s_cmtrd.c */
extern int SCR_open_cmt(int mode);
extern int SCR_end_cmt(void);
extern int CMT_read_text(char **txt);
extern int CMT_read_len(char **txt);

/* s_cmts.c */
extern int SCR_scmt_txt(char *txt);
extern int SCR_scmt_free(void);
extern int SCR_scmt_add_key(int key,char *txt);
extern int SCR_scmt_set_keys(YYKEYS *keys);
extern int SCR_scmt_add_fnk(int fnknu);
extern int SCR_scmt_set_txt(void);
extern int SCR_scmt_txt_1(char *buf,int key,char *txt);
extern int SCR_scmt_lgs_pos(int *lgs,int *pos);

/* s_cmtsed.c */
extern int SCR_scmt_display(void);
extern int SCR_scmt_display_1(int key_nb,int pos,int lg,int attr);
extern int SCR_scmt_menu(int line,int col);
extern int SCR_scmt_find_button(int *lgs,int *pos);
extern int SCR_scmt_mouse(void);

/* s_cmtpg.c */
extern int PG_read_flds(PAGE *pg);
extern int PG_fld_free(PAGE *pg);

/* s_wuser.c */
extern int WscrDOSUserInit(int argc,char *argv[]);

/* s_wgetms.c */
extern long WscrGetMS(); 

/* s_cmtscr.c */
extern int PG_display_scr(PAGE *pg);

/* s_pgdft.c */
extern int PG_set_dft_field(PAGE *pg,int fld_nb);
extern int PG_is_fld_empty(PAGE *pg,int fld_nb);
extern int SCR_is_empty(char *text);
extern int PG_check_required(PAGE *pg);

/* s_pgmod.c */
extern int PG_modify_text(PAGE *pg,char *edtxt,int fld_nb);

/* s_pgfmt.c */
extern unsigned char *PG_fmt_text(char *buf,char *fmt,char *txt);
extern unsigned char *PG_ufmt_text(char *buf,char *fmt,char *txt);
extern unsigned char *PG_fmt_long(unsigned char *buf,unsigned char *fmt,unsigned long l);
extern unsigned long PG_ufmt_long(char *fmt,char *txt);
extern int PG_is_fmt_empty(char *text,char *fmt);

/* s_pgedit.c */
extern int SCR_set_field_editor(PAGE *pg,int fld_nb,unsigned char *text);

/* s_aberr.c */
extern int ERR_abort(void);

/* s_sbar.c */
extern int SB_init(S_BAR *sb,int line,int col,int nl,int c1,int c2,int attr);
extern int SB_ginit(S_BAR *sb,int orient,int line,int col,int nl,int c1,int c2,int attr,int nbi);
extern int SB_set(S_BAR *sb,int l1t,int nlt);
extern int SB_get(S_BAR *sb,int l1t,int nlt,int linecol);

/* s_edpwd.c */
extern int PG_edit_pwd(PAGE *pg,unsigned char *pwd,int key,int nbtry);
extern int PG_change_pwd(PAGE *pg,unsigned char *pwd,int key,int nbtry);

/* s_scincr.c */
extern int SC_init_display(void);
extern int SC_init_display_d(int disp);
extern int SC_end_display(void);
extern int SC_incr_display(long nb_read,long nb_found);
extern int SC_hit_key(char *text);
extern int SC_wait(void);

/* s_pgsld.c */
extern int PG_display_slide(PAGE *pg);
extern int PG_display_slide_cmt(PAGE *pg,unsigned char *cmt);

/* s_prt.c */
extern int PR_open(void);
extern int PR_clear(void);
extern int PR_clear_line(int line,int col);
extern int PR_send(void);
extern int PR_select(PR_DEF *pr);
extern int PR_print_attr_std(PR_DEF *pr,int attr);
extern int PR_print_attr(PR_DEF *pr,int attr);
extern int PR_flush(PR_DEF *pr);
extern int PR_flush_char(PR_DEF *pr,int ch);
extern int PR_flush_lf_std(PR_DEF *pr,int n);
extern int PR_flush_lf(PR_DEF *pr,int n);
extern int PR_flush_ff_std(PR_DEF *pr);
extern int PR_flush_ff(PR_DEF *pr);
extern int PR_cr(void);
extern int PR_goto(int l,int c);
extern int PR_char(int ch);
extern int PR_formfeed(void);
extern int PR_linefeed(void);
extern int PR_lf(int n);
extern int PR_block(int nl);
extern int PR_utitle(char *c1,char *c2,char *c3,char *c4,char *c5,char *c6,char *c7,char *c8);
extern int PR_print_str(PR_DEF *pr,unsigned char *str);

/* s_prmail.c */
extern FILE *PR_file_open(char *filename);
extern int PR_is_eq(FILE *fd,int ch);
extern int PR_read_keyw(FILE *fd,char *keyw);
extern int PR_file_ungetc(int ch);
extern int PR_file_getc(FILE *fd);
extern int PR_file_eol(FILE *fd);
extern int PR_print_var(char *fld_name);
extern int PR_mail_col(unsigned char *filename,unsigned char *(*fn)(),int col,int nl,int first_ch);
extern int PR_mail(unsigned char *filename,unsigned char *(*fn)());
extern int PR_is_empty(unsigned char *ptr);

/* s_prfile.c */
extern unsigned char *PR_file_pg(unsigned char *fld_name);
extern int PG_find_nb(PAGE *pg,char *fld_name);
extern int PR_file(char *filename,PAGE *pg);
extern int PR_file_col(char *filename,PAGE *pg,int col,int nl,int first_ch);

/* s_prlab.c */
extern int PR_label_select(PR_DEF *pr,int nv,int nh,int nl,int nc);
extern int PR_label_pos(void);
extern int PR_label_pg(PAGE *pg);
extern int PR_label_file(char *filename,PAGE *pg,int ch);
extern int PR_label_send(void);

/* s_prlist.c */
extern int PR_file_nl(char *filename,int first_ch);
extern int PR_listing_0(PAGE *pg_ref,char *file);
extern int PR_listing_eop(void);
extern int PR_listing_1(ISAM *is,long nb_read,long nb_found,int success);
extern int PR_listing(PR_DEF *pr,PAGE *pg_ref,PAGE *pg_scan,int cmp,char *file);
extern int PR_listing_fn(PR_DEF *pr,PAGE *pg_ref,PAGE *pg_scan,int cmp,char *file,int (*fn)());

/* s_prtdev.c */
extern int PR_set_device(PR_DEF *pr,int type,char *name);

/* s_prtnam.c */
extern int PR_set_name(PR_DEF *pr,char *filename);

/* s_prtsiz.c */
extern int PR_set_size(PR_DEF *pr,int nl,int nc);

/* s_prtpg.c */
extern int PR_page(PAGE *pg);
extern int PR_page_clf(PAGE *pg,int c);
extern int PR_write_im(IMAGE *im);

/* s_prtmv.c */
extern int PR_down(int val);
extern int PR_up(int val);
extern int PR_left(int val);
extern int PR_right(int val);

/* s_prtatr.c */
extern int PR_bold(void);
extern int PR_enlarged(void);
extern int PR_condensed(void);
extern int PR_underline(void);
extern int PR_normal(void);

/* s_prttab.c */
extern int PR_pg_tab(void);
extern int PR_tab(void);
extern int PR_vtab(void);

/* s_prtact.c */
extern int PR_action(A_ACTION *aa);

/* s_prterr.c */
extern int PR_error(PR_DEF *pr);

/* s_drv.c */
extern int SCR_read_driver(void);
extern int SCR_read_ykword(FILE *fd,YYKEYS **Yk,int *nb);

/* s_pgtp.c */

/* s_pgip.c */

/* s_pgcd.c */

/* s_pgusr.c */
extern int PG_user_fn(PAGE *pg,int fld_nb,char *txt);

/* s_reckey.c */
extern int SCR_record_key(int key);
extern int SCR_record_text(unsigned char *text);

/* s_pgkey.c */
extern int PG_is_fnk(PAGE *pg,int fld_nb);
extern int PG_fnkeys(PAGE *pg,int fld_nb);

/* s_isfnk.c */
extern int FNK_is_fnk(FNKEYS *fks,int key);

/* s_iskey.c */
extern int SCR_is_key(int key);

/* s_pgedw.c */
extern int PG_edit_window(PAGE *pg);

/* s_ccmtpg.c */
extern int PG_calc_cmt(PAGE *pg,int fld_nb);

/* s_ccmtmn.c */
extern int MN_calc_cmt(MENU *mn);

/* s_ccmtab.c */
extern int AB_calc_cmt(A_BAR *ab,int choice);

/* s_cmtall.c */
extern int SCR_cmt_alloc(void);

/* s_key.c */
extern long KEY_calc(unsigned char *key);

/* s_sn.c */
extern int SCR_sn(void);
extern int SCR_pad_center_print(char *fmt,char *a,char *b);

/* s_ak.c */
extern long SCR_ak(void);

/* s_cak.c */
extern int C_check_ak(void);

/* s_uname.c */
extern int SCR_get_uname(char *name);
extern int SCR_get_tty(char *ttyname);

/* s_cvttab.c */
extern int SCR_cvt_tab(int key);

/* s_fnk.c */
extern int FNK_read_def(FNKEYS *fk);

/* s_ract.c */
extern int ACT_len_ract(RACT *ract);
extern A_ACTION *ACT_ract_to_aa(RACT *ract);
extern ACTION *ACT_create(void);
extern int ACT_free(ACTION *act);
extern int ACT_add(ACTION *act,RACT *ract);
extern int ACT_read_def(ACTION *act);
extern int ACT_read_rec_key(int *info);
extern int ACT_read_cfn(void);
extern int ACT_if_exist(ACTION *act);

/* s_rapp.c */
extern char *SCR_construct_filename(char *filename,int type);
extern int SCR_read_apps(char *filename);
extern char *SCR_read_object(char *name_file,int type);
extern int SCR_read_def(int type,char *ptr);
extern int SCR_read_flt_nu(int type);
extern int SCR_read_comment(void);
extern int SCR_read_global(void);
extern int SCR_get_glob_text(unsigned char **ptr);
extern int SCR_get_glob_yn_1(void);
extern int SCR_get_glob_yn(int *val);
extern int SCR_get_glob_long(long *val);
extern int SCR_get_glob_pos(int *pos);
extern int SCR_get_glob_color(int *val);
extern int SCR_read_winattr(void);
extern int SCR_read_winncpfld(void);

/* s_rdbc.c */
extern int BC_read_def(BC *bc);
extern int BC_check(BC *bc);

/* s_rdis.c */
extern ISAM *IS_create(void);
extern int IS_free(ISAM *is);
extern ISFIELD *IS_add_fld(ISAM *is);
extern COMPOSITE *IS_add_cmp(ISAM *is);
extern LINK *IS_add_lnk(ISAM *is);
extern int IS_read_def(ISAM *is);
extern int IS_read_fld(ISAM *is,int vfld);
extern int IS_read_cmp(ISAM *is);
extern int IS_read_cmp_fld(ISAM *is,COMPOSITE *cmp);
extern int IS_read_fld_name(ISAM *is);
extern int IS_read_cmp_name(ISAM *is);
extern int IS_check_cmp(ISAM *is,int cmp_nb);
extern int IS_check_lnk(ISAM *is,int lnk_nb);
extern int IS_end(ISAM *is, int flag);
extern int SCR_anal_vector(char *text);
extern int IS_find_fld_nb(ISAM *is,char *isf_name);
extern int IS_find_cmp_nb(ISAM *is,char *cmp_name);
extern int SCR_read_alias_isam(void);

/* s_tbl.c */
extern YYFILE *SCR_yyopen_tbl(char *filename);
extern int SCR_yyopen(char *filename);
extern int SCR_yyopen_app(char *filename);
extern int SCR_yyclose(void);
extern int SCR_read(void);
extern int SCR_getc(void);
extern int SCR_ungetc(int c);
extern int SCR_unread(void);
extern int SCR_yylex(void);
extern int SCR_read_str(void);
extern char *SCR_yylex_str(char *ptr);
extern unsigned char *SCR_yylex_alloc_str(void);
extern unsigned char *SCR_yylex_alloc_name(void);
extern int SCR_yylex_int(int *ptr);
extern int SCR_yylex_opt_int(void);
extern int SCR_yylex_val(void);
extern int SCR_yylex_color(void);
extern int SCR_yylex_yn(void);
extern int WR_define(FILE *fd,int def);
extern int SCR_search_kw(int key);
extern int SCR_include(char *filename);
extern int SCR_yyrecord(unsigned char *txt);

/* s_rdmn.c */
extern int MN_read_def(MENU *mn);
extern int MN_read_option(MENU *mn);
extern int MN_save_opt(MENU *mn);

/* s_rdmp.c */
extern int MP_read_def(MPAGE *mp);
extern int MPBC_read_tcol(MPTCOL **tc2,unsigned short *nbtc,ISAM *is);
extern int MP_check(MPAGE *mp);
extern int SCR_set_lpg_pos(PAGE *apg,PAGE *lpg,int *line);
extern int SCR_pos_lpgs(void);

/* s_rdpg.c */
extern int PG_center(PAGE *pg);
extern int PG_read_def(PAGE *pg);
extern int PG_read_field(PAGE *pg,char **appto[], U_ch *next[]);
extern GRPBOX *PG_add_group(PAGE *pg);
extern int PG_read_group(PAGE *pg);
extern int PG_end_group(PAGE *pg);
extern int PG_find_group(PAGE *pg,unsigned char *name);
extern int PG_adjust_buf_fld(PAGE *pg,char *name,char *fmt,char *help,char *allowed,char *pg_cmt);
extern int PG_add_buf(unsigned char *buf,char *txt);
extern int PG_insert_buf(unsigned char *buf,char *txt,int nu);
extern int PG_delete_buf(unsigned char *buf,int nu);
extern int PG_set_buf_last(unsigned char *buf,char *txt);
extern int PG_len_buf(unsigned char *buf);
extern int PG_read_dft(PAGE *pg);
extern int PG_read_line(short *op,short *val);
extern int PG_read_pos(PAGE *pg,int keyword);
extern int PG_read_fill(void);
extern int PG_read_fld_comment(PAGE *pg,int fld_nb);
extern int PG_read_screen(PAGE *pg,char **pg_scr,NFLD **pg_nfld);
extern int PG_field_end_scr(PAGE *pg,int nu_fld,NFLD *nfld);
extern int PG_end_scr(PAGE *pg,NFLD *nfld);
extern int PG_end_scr_afld(PAGE *pg,NFLD *nfld,int nu_isam,int nu_isf);
extern int SCR_add_cmt(char *txt,long *pos);
extern int SCR_add_cmt_len(char *txt,int len,long *pos);
extern int SCR_add_cmt_ele(char *txt,long *pos,short *cmt);
extern int SCR_add_cmt_free_txt(char *txt,int len,long *pos);
extern int PG_save_scr(PAGE *pg,char *scr);
extern int PG_save_drw(PAGE *pg);
extern int PG_dump_draws(PAGE *pg);
extern int PG_save_fld(PAGE *pg);
extern int PG_dump_flds(PAGE *pg);
extern int PG_write_text(char *txt);
extern int PG_write_len(char *txt,int len);

/* s_rdpr.c */
extern int PR_init(PR_DEF *pr);
extern int PR_free(PR_DEF *pr);
extern PR_DEF *PR_create(void);
extern int PR_read_def(PR_DEF *pr);

/* s_rdrw.c */
extern int DRW_read(DRAWS *dw, int keyw);
extern int DRW_read_drw_attr(DRAW *drw);
extern DRAW *DRW_add(DRAWS *dw);
extern int DRW_init_drw(DRAWS *dw,int drw_nb);
extern int DRW_free(DRAWS *dw);
extern int DRW_check(DRAWS *dw,int *nc,int *nl);

/* s_rdv.c */
extern int VIEW_read_def(VIEW **views,short *nb_views);
extern int VIEW_fill(VIEW *views,int nb_views,int lpg,int bpg,int isam);
extern int VIEW_check(VIEW *views,int nb_views,int isam);

/* s_rdtp.c */
extern TPAGES *TP_create(void);
extern int TP_read_def(TPAGES *tp);

/* s_runa.c */
extern PAGE *PG_read(char *name);
extern TPAGES *TP_read(char *name);
extern REPORT *RP_read(char *name);
extern A_BAR *AB_read(char *name);
extern APPL *AP_read(char *name);
extern MENU *MN_read(char *name);
extern ISAM *IS_read(char *name);
extern MPAGE *MP_read(char *name);
extern BC *BC_read(char *name);
extern ACTION *ACT_read(char *name);
extern PR_DEF *PR_read(char *name);
extern FNKEYS *FNK_read(char *name);
extern WTOOLBAR *TB_read(char *name);

/* s_crobj.c */
extern FILE_TYPE *SCR_add_flt(char *filename,int type);
extern char *SCR_create(int type);

/* s_mp.c */
extern MPAGE *MP_create(void);
extern int MP_init(MPAGE *mp);

/* s_bc.c */
extern BC *BC_create(void);
extern int BC_init(BC *bc);
extern int BC_free(BC *bc);

/* s_page.c */
extern int PG_init(PAGE *pg);
extern PAGE *PG_create(void);
extern FIELD *PG_add_field(PAGE *pg);
extern int PG_end(PAGE *pg);
extern int PG_adjust_lc(PAGE *pg);
extern int PG_anal_apply_cmp(PAGE *pg,char **apply_to[]);
extern int PG_anal_lag(char *orig,char *dest);
extern int PG_order_flds(PAGE *pg);

/* s_rdtb.c */
extern WTOOLBAR *TB_create(void);
extern int TB_read_def(WTOOLBAR *tb);
extern int TB_read_button(WTOOLBAR *tb,int type);

/* s_fld.c */
extern int PG_init_dft_field(PAGE *pg);
extern int PG_dft_field(FIELD *fld);
extern int PG_init_field(PAGE *pg,int nb_fld);
extern int PG_realloc_field(PAGE *pg,int fld_nb,char *txt);
extern int PG_add_fld_rec(PAGE *pg,int fld_nb);
extern int PG_field_end(PAGE *pg,int fld_nb,char *prompt,unsigned char *txt,FIELD *fld_chk,unsigned char *title);
extern int PG_add_draw_prompt(PAGE *pg,char *prompt,unsigned short fld_nb);
extern int PG_field_end_isam(PAGE *pg,int fld_nb);
extern int SCR_lg_fmt(char *fmt);
extern int FLD_add_isam(FIELD *fld);

/* s_rab.c */
extern A_BAR *AB_create(void);
extern int AB_free(A_BAR *ab);
extern A_OPT *AB_add_option(A_BAR *ab);
extern int AB_end(A_BAR *ab);
extern int AB_opt_end(A_OPT *ao);
extern int AB_read_def(A_BAR *ab);
extern int AB_read_opts(A_BAR *ab);

/* s_rap.c */
extern int AP_read_def(APPL *ap);
extern int AP_init(APPL *ap);
extern APPL *AP_create(void);
extern int AP_free(APPL *ap);
extern int AP_end(APPL *ap);

/* s_rdre.c */
extern REPORT *RP_create(void);
extern int RP_read_def(REPORT *rp);
extern int RP_end(REPORT *rp);
extern int RP_read_defaults(REPORT *rp);
extern int RP_read_field(REPORT *rp);
extern int RP_add_val(char **buf,int nb,char *txt);
extern RIFLD *RP_add_fld_ip(RFLD *rfld,int type,int ptr);
extern int PG_read_fld_name(PAGE *pg);
extern int PG_find_fld_nb(PAGE *pg,char *name);

/* s_rdsmp.c */
extern int SMP_free(SMPAGE *smp);
extern int SMP_read_def(void);
extern int SMP_read_flds(SMPAGE *smp,int type);
extern int SMP_read_1fld(SFLD **sflds,int *nflds,ISAM *is,int ext);
extern int SMP_calc_pos(SMPAGE *smp);
extern int SMP_dft_fld_len(SFLD *sfld,ISAM *is);
extern int SMP_fld_len(SFLD *sfld,ISAM *is);
extern int SMP_calc_tit(SFLD *fld,ISAM *is,char *buf,int strip);
extern int SMP_name(char *ptr,char *name);
extern int SMP_dump_1(SMPAGE *smp,FILE *fd);
extern int SMP_print_mpage(SMPAGE *smp,FILE *fd);
extern int SMP_print_apage(SMPAGE *smp,FILE *fd);
extern int SMP_print_lpage(SMPAGE *smp,FILE *fd);
extern int SMP_print_1fld(SMPAGE *smp,int i,int type,FILE *fd,int pos);
extern int SMP_print_1drw(SMPAGE *smp,int i,FILE *fd,int line,int col);
extern int SMP_dump(SMPAGE *smp);
extern int EXT_name(FILE_TYPE *ft,char *name);

/* s_rdwpos.c */
extern int SCR_WposReadAdd(int iObj,int iFld,int v);
extern int SCR_WposAddObj(int iObj,int iFld);
extern int SCR_WposFindObj(int iObj,int iFld);
extern int SCR_WposAddV(int iObj,int iFld,int v,int x,int r);
extern int SCR_WposSetAll(int iObj,int iFld,int *x,int *y,int *w,int *h);
extern int SCR_WposSet1(int n,int *x,int v);

/* s_robj.c */
extern int SCR_read_obj(char *filename);
extern int O_check_ak(void);
extern int O_read(void *text,int len);
extern void *O_read_text(void **text);
extern int O_read_str(unsigned char *text);
extern int OBJ_read_filetable(void);
extern int OBJ_read_structures(void);
extern int OBJ_read_page(PAGE *pg);
extern int OBJ_read_wtoolb(WTOOLBAR *tb);
extern int O_read_lena(unsigned char **ptr,int a);
extern int OBJ_read_fields(PAGE *pg,int nb_fld);
extern int OBJ_read_fnkeys(FNKEYS *fk);
extern int OBJ_read_action(ACTION *act);
extern int OBJ_read_drws(DRAWS *dw);
extern int OBJ_read_menu(MENU *mn);
extern int OBJ_read_options(int nb_opt,OPTION *opts);
extern int OBJ_adjust_reference(void);
extern int OBJ_read_abar(A_BAR *ab);
extern int OBJ_read_appl(APPL *ap);
extern int OBJ_read_cmt(void);
extern int OBJ_read_tpages(TPAGES *tp);

/* s_robjpr.c */
extern int OBJ_read_prt(PR_DEF *pr);

/* s_objrd.c */
extern char *SCR_add_ext(char *filename);
extern char *OBJ_read(char *name,int type);

/* s_cint.c */
extern int C_init_objects(void);

/* s_image.c */
extern IMAGE *SCR_extract_image(IMAGE *in_im,int line,int column,int nb_line,int nb_column);
extern IMAGE *SCR_save_image(int line,int column,int nb_line,int nb_column);
extern int SCR_display_image(IMAGE *im);
extern IMAGE *SCR_copy_image(IMAGE *in);

/* s_imwrt.c */
extern int SCR_write_im(int line,int col,unsigned char *text,int len);

/* s_immv.c */
extern int SCR_move_image(IMAGE **im1,IMAGE *im2);
extern int SCR_mv_im(IMAGE *s_im,IMAGE **im1,IMAGE *im2);

/* s_im2im.c */
extern int SCR_im_to_im(IMAGE *ou_im,IMAGE *in_im,int line,int col);

/* s_imfd.c */
extern int SCR_display_im_fd(FILE *fd,IMAGE *im);

/* s_imclr.c */
extern int SCR_clear_image(IMAGE *im,int attr);

/* s_imset.c */
extern int SCR_set_image(IMAGE *im,unsigned char *text,int line,int col,int nc);

/* s_imattr.c */
extern int SCR_set_image_attr(IMAGE *im,int line,int col,int nl,int nc,int attr);

/* s_imallc.c */
extern IMAGE *SCR_alloc_image(int nl,int nc);
extern int SCR_free_image(IMAGE *im);

/* s_mmted.c */
extern int ED_text(int unzoom);
extern int ED_recalc_pos(int *line,int *col,int *nl,int *nc);
extern int ED_init_im(void);
extern int ED_change_pos(int line,int col,int nl,int nc);
extern int ED_pos_in(int pos[],int line,int col,int nl,int nc);
extern int ED_end_im(void);
extern int ED_reset_pos(void);
extern int ED_display(void);
extern int ED_show_staff(void);
extern int ED_mark_image(void);
extern int ED_set_cursor(void);
extern int ED_fns(int key);
extern int ED_exec_fns(MMTFN *mmtfns,int key);
extern int ED_set_pos(void);
extern int ED_set_char(int line,int col,int ch);
extern int ED_del_char(int line,int col,int n);
extern int ED_order_mark(int *bl,int *bc,int *el,int *ec);
extern int ED_mouse(void);
extern int ED_mouse_spec(void);

/* s_mmtfns.c */
extern int ED_left(void);
extern int ED_right(void);
extern int ED_up(void);
extern int ED_down(void);
extern int ED_home(void);
extern int ED_end(void);
extern int ED_pgup(void);
extern int ED_pgdn(void);
extern int ED_c_home(void);
extern int ED_c_end(void);
extern int ED_mode_insert(void);
extern int ED_delete_ch(void);
extern int ED_backspace(void);
extern int ED_enter(void);
extern int ED_mark_line(void);
extern int ED_mark_block(void);
extern int ED_mark_text(void);
extern int ED_unmark(void);
extern int ED_mark(int type);
extern int ED_copy_paste(void);
extern int ED_cut(void);
extern int ED_move(void);
extern int ED_ask_int(unsigned char *title,int lg,int in);
extern int ED_goto(void);
extern int ED_repl(void);
extern int ED_find(void);
extern int ED_underl(void);
extern int ED_box_1(int type);
extern int ED_box_border(void);
extern int ED_box_redraw(void);
extern int ED_box(void);
extern int ED_shift_bloc(int type);
extern int ED_shift_left(void);
extern int ED_shift_right(void);
extern int ED_paste(void);
extern int ED_copy(void);
extern int ED_show(void);
extern int ED_delete(void);
extern int ED_insert(void);
extern int ED_first_nb(int i);
extern int ED_join_par(int line);
extern int ED_join(void);
extern int ED_kill(void);
extern int ED_del_eol_par(int cl,int cc);
extern int ED_del_eol(void);
extern int ED_del_bol(void);
extern int ED_char_type(void);
extern int ED_prev_word(void);
extern int ED_next_word(void);
extern int ED_left_del(void);
extern int ED_new_par(int cl,int cc);
extern int ED_new(void);
extern int ED_overlay(void);
extern int ED_fill(void);
extern int ED_tab(void);
extern int ED_zoom(void);
extern int ED_move_box(void);
extern int ED_resize_box(void);
extern int ED_quit(void);
extern int ED_show_dos(void);
extern int ED_shell(void);
extern int ED_help(void);
extern int ED_scr_help(void);
extern int ED_hypertext(void);
extern int ED_write_file(void);
extern int ED_buf_create(void);
extern int ED_buf_free(void);
extern int ED_about(void);
extern int ED_get_rmarg(void);
extern int ED_act_marg(void);
extern int ED_is_par(int i);
extern int ED_reflow_base(void);
extern int ED_reflow(void);
extern int ED_reflow_auto(void);
extern int ED_upper_lower(int type);
extern int ED_upper(void);
extern int ED_lower(void);
extern int ED_savequit(void);
extern int ED_saveall(void);
extern int ED_chngfname(void);
extern int ED_chngdir(void);
extern int ED_printmmt(void);
extern int ED_grep(void);
extern int ED_switch2(void);
extern int SCR_is_valid_fname(unsigned char *name);
extern int ED_menu_right(void);
extern int ED_set_staff(void);
extern int ED_go_staff(void);

/* s_mmtcr.c */
extern MMT *MMT_create(int l,int c,int nl,int nc,int at,int rat,int stat,int box,unsigned char *tit,int sbar,MMTFN *mfns,int (*fnks)(),int chng);
extern int MMT_set_title(MMT *mmt,unsigned char *title);
extern int MMT_free(MMT *mmt);
extern int MMT_set_lst(MMT *mmt,LST *lst);
extern LST *MMT_get_lst(MMT *mmt);
extern int MMT_set_vec(MMT *mmt,unsigned char *vec);
extern int MMT_display_vec(MMT *mmt,char *txt);
extern unsigned char *MMT_get_vec(MMT *mmt);
extern int MMT_display(MMT *mmt);
extern int MMT_edit(MMT *mmt);
extern int MMT_edit_window(MMT *mmt);
extern int MMT_save_im(MMT *mmt);
extern int MMT_restore_im(MMT *mmt);
extern unsigned char *MMT_edit_line(unsigned char *txt,unsigned char *title,int line,int col,int nl,int nc);
extern int MMT_init_edit_file(void);
extern int MMT_end_edit_file(void);
extern int MMT_quit_file(int j);
extern int MMT_body_edit_file(int line,int col,int nl,int nc);
extern int MMT_edit_file(char *file,int line,int col,int nl,int nc);
extern int MMT_show_file(char *file,int line,int col,int nl,int nc);
extern int MMT_editshow_file(char *file,int line,int col,int nl,int nc,int output);
extern int MMT_add_file(char *file,int line,int col,int nl,int nc);
extern int MMT_set_pos_attr(MMT *mmt,int line,int col,int attr);
extern int MMT_set_cursor(MMT *mmt);
extern int MMT_unmark(MMT *mmt);
extern int MMT_ask_save(MMT *mmt);
extern int MMT_backup(MMT *mmt);
extern int MMT_list_files(void);
extern int MMT_set_pos(MMT *mmt,int line,int col,int nl,int nc);
extern int MMT_litlebox(void);
extern int MMT_cascade(void);
extern int MMT_fullsize(void);
extern int MMT_mouse_select(void);

/* s_mmtint.c */
extern int ED_nb(void);
extern int ED_free(int n);
extern unsigned char *ED_realloc(int n,int lg);
extern unsigned char *ED_get_line(int n);
extern int ED_set_line(int n,int line);
extern int ED_set_txt(int i,int j,int ch);
extern int ED_get_txt(int i,int j);
extern int ED_line_lg(int i);
extern int ED_add_lines(int line,int n);
extern int ED_del_lines(int line,int n);
extern int ED_move_lines(int from,int to,int n);

/* s_mmtmn.c */
extern int ED_menu_gnrl(YYKEYS *ytbl);
extern int ED_menu_win(void);
extern int ED_menu_txt(void);
extern int ED_menu_curs(void);
extern int ED_menu_bloc(void);
extern int ED_menu_misc(void);
extern int ED_menu_box(void);
extern char *ED_quick_help_prep1(int key,MMTFN *mmtfns);
extern int ED_quick_help_prep(YYKEYS *ytbl);
extern int ED_quick_help(void);
extern int ED_quick_help_set_image(void);
extern int ED_quick_help_disp_image(void);
extern int ED_quick_help_display(YYKEYS *ytbl);

/* s_rprdev.c */
extern PRTDEV **SCR_ReadPrts(char *filename);   /* JMP 30-08-98 */

#ifdef __cplusplus
}
#endif

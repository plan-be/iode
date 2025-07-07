#ifdef __cplusplus
extern "C" {
#endif


/* s_isam.c */
extern long win_app32_segment(void);
extern int IS_search_db(ISAM *is);
extern int IS_open(ISAM *is,int mode,int lock);
extern int IS_close(ISAM *is);
extern int IS_close_isam(ISAM *is);
extern int IS_close_all(void);
extern int IS_is_open(ISAM *is);
extern int IS_search(ISAM *is,int nu_index,int cond,char *order);
extern int IS_next(ISAM *is);
extern int IS_prev(ISAM *is);
extern int IS_check_required(ISAM *is);
extern int IS_is_fld_empty(char *ptr,int type,int lg);
extern int IS_write(ISAM *is);
extern int IS_rewrite(ISAM *is);
extern int IS_delete(ISAM *is);
extern char *IS_get_field(ISAM *is,int fld_nb,char *ptr);
extern int IS_get_max_field(ISAM *is,int fld_nb,char *ptr);
extern int IS_get_read_field(ISAM *is,int fld_nb,char *ptr);
extern int IS_set_field(ISAM *is,int fld_nb,char *ptr);
extern int IS_set_max_field(ISAM *is,int fld_nb,char *ptr);
extern int IS_check_all_zstring(ISAM *is);
extern int IS_check_zstring(ISAM *is,int isf_nb);
extern int IS_check_max_zstring(ISAM *is,int isf_nb);
extern int IS_set_ref_to_0(ISAM *is);
extern int IS_ref_is_0(ISAM *is);
extern int IS_check_ref(ISAM *is,int cmp_nb);
extern int IS_is_mod_link(ISAM *is,int lnk_nb);
extern int IS_incr_link(ISAM *is,int lnk_nb,int add);
extern int IS_auto_num(ISAM *is);
extern int IS_save_rec(ISAM *is);
extern int IS_empty_rec(ISAM *is);
extern int IS_empty_max_rec(ISAM *is);
extern int IS_empty_field(ISAM *is,int isf_nb);
extern int IS_is_mod_field(ISAM *is,int fld_nb);
extern int IS_copy_rec(ISAM *is,char *rec1,char *rec2);
extern int IS_copy_field(ISAM *is_out,int nb_out,int b_out,ISAM *is_in,int nb_in,int b_in);
extern ISAM *IS_dup(ISAM *is,char *filename);
extern int IS_free_dup(ISAM *is);
extern int IS_set_name(ISAM *is,char *name);
extern char *IS_error(void);
extern int IS_set_err(int n);
extern int IS_set_field_txt(ISAM *is,int fld,char *txt);
extern int SCR_edis_create_F10(PAGE *pg,ISAM *is);

/* s_robjis.c */
extern int OBJ_read_isam(ISAM *is);
extern int OBJ_read_isi(ISAM *is);
extern int OBJ_read_mpage(MPAGE *mp);
extern int OBJ_read_bc(BC *bc);
extern int OBJ_mpage_adjust(MPAGE *mp);

/* s_pgis.c */
extern int SCR_isf_to_fld(PAGE *pg,int fld_nb,ISAM *is,int isf_nb);
extern int SCR_fld_to_isf(PAGE *pg,int fld_nb,ISAM *is,int isf_nb);
extern int SCR_rec_to_page(PAGE *pg,ISAM *is);
extern int SCR_rec_to_page_1(PAGE *pg,ISAM *is, int flag_join);
extern int SCR_isam_to_page(PAGE *pg,ISAM *is);
extern int SCR_page_to_isam(PAGE *pg,ISAM *is);
extern int SCR_page_to_isam_1(PAGE *pg,ISAM *is);
extern int FLD_change_isf(PAGE *pg,int nb_fld,int nb_isf,int new_val);
extern int PG_set_isam(PAGE *pg,ISAM *is1,ISAM *is2);
extern int SCR_page_eq_isam(PAGE *pg,ISAM *is);

/* s_actis.c */
extern int SCR_pg_create_rec(PAGE *pg,ISAM *is);
extern int SCR_next_rec(PAGE *pg,ISAM *is);
extern int SCR_prev_rec(PAGE *pg,ISAM *is);
extern int SCR_delete_rec(PAGE *pg,ISAM *is);
extern int PG_check_nodup(PAGE *pg,FIELD *fld);
extern int ACT_exec_aa_isam(A_ACTION *aa);
extern int MP_act_delete(MPAGE *mp);
extern int MP_act_modify(MPAGE *mp);
extern int IS_act_pg_create_rec(PAGE *pg);
extern int IS_act_pg_search_rec(PAGE *pg);
extern int IS_act_pg_modify_rec(PAGE *pg);
extern int IS_act_pg_next_rec(PAGE *pg);
extern int IS_act_pg_prev_rec(PAGE *pg);
extern int IS_act_pg_delete_rec(PAGE *pg);
extern int IS_act_mp_import(MPAGE *mp,PAGE *pg);
extern int IS_act_pg_import(PAGE *pg1,PAGE *pg2);
extern int IS_act_ap_import(APPL *ap,PAGE *pg);
extern int IS_act_is_search(ISAM *is,int cmp_nb);
extern int SCR_search_rec(PAGE *pg,ISAM *is);

/* s_pgfis.c */
extern ISAM *PG_find_isam(PAGE *pg);

/* s_cmp.c */
extern int SCR_apply_cmp(PAGE *pg,int fld_nb);
extern int SCR_apply_cmp_1(PAGE *pg,int fld_nb);
extern int SCR_apply_all_cmp(PAGE *pg);
extern int SCR_apply_all_cmp_1(PAGE *pg);
extern int SCR_display_all_cmp(PAGE *pg);
extern int SCR_display_cmp(PAGE *pg,int fld_nb);
extern int PG_display_isam_flds(PAGE *pg);
extern int SCR_search_page_by_cmp(PAGE *pg,ISAM *is,int cmp_nb);
extern int SCR_next_page_by_cmp(PAGE *pg,ISAM *is);
extern int SCR_prev_page_by_cmp(PAGE *pg,ISAM *is);

/* s_scan.c */
extern long SC_scan_isam_gn(ISAM *is,char *from,char *to,int cmp_nb,int (*fn)(),int opent,int disp,int stop,int rev);
extern long SC_scan_isam_gn_chk(ISAM *is,char *from,char *to,int cmp_nb,int (*fn)(),int opent,int disp,int stop,int rev,char *fromd,char *tod);
extern long SC_scan_isam(ISAM *is,char *from,char *to,int cmp_nb,int (*fn)(),int open_type,int disp);
extern int SC_save_empty(ISAM *is,char *from,char *to,char **from_def,char **to_def);

/* s_actbc.c */
extern int ACT_exec_aa_bc(A_ACTION *aa);
extern int BC_act_create(BC *bc);
extern int BC_act_reset(BC *bc);

/* s_edis.c */
extern int PG_edit_isam(PAGE *pg);
extern int SCR_edis_query(PAGE *pg,ISAM *is, int type);
extern int SCR_get_cmp_flds(PAGE *pg,ISAM *is,int cmp_nb,int *cmp_flds);
extern int SCR_next_page_composite(PAGE *pg,ISAM *is,int cmp_nb);
extern int SCR_prev_page_composite(PAGE *pg,ISAM *is,int cmp_nb);
extern int PG_display_reverse_field(PAGE *pg,int fld_nb);
extern int SCR_select_composite(PAGE *pg,ISAM *is);
extern int SCR_edit_cmp_page(PAGE *pg,ISAM *is,int cmp_nb);

/* s_scchk.c */
extern int SC_check_index(ISAM *is,char *to,char *to_def,int cmp_nb,int direct);
extern int SC_check_rec(ISAM *is,char *from,char *to,char *from_def,char *to_def);
extern int SC_cmp_flds(unsigned char *rec1,unsigned char *rec2,int type,int lg);
extern int SC_chk_1(int chk);

/* s_mpis.c */
extern int MP_start1(MPAGE *mp);
extern int MP_start2(MPAGE *mp,int cmp_nb);
extern int MP_open(MPAGE *mp);
extern int MP_close(MPAGE *mp);
extern int MP_set_cbuf(MPAGE *mp);
extern int MP_alloc_bufs(MPAGE *mp);
extern int MP_free_bufs(MPAGE *mp);
extern int MP_free_scan(MPAGE *mp);
extern int MP_reinit(MPAGE *mp);
extern int MP_fnkeys(MPAGE *mp,int key);
extern int MP_up(MPAGE *mp);
extern int MP_down(MPAGE *mp);
extern int MP_pg_up(MPAGE *mp);
extern int MP_pg_down(MPAGE *mp);
extern int MP_home(MPAGE *mp);
extern int MP_end(MPAGE *mp);
extern int MP_up_window(MPAGE *mp,int nl);
extern int MP_down_window(MPAGE *mp,int nl);
extern int MP_cmp_to_buf(MPAGE *mp,int rec_nb);
extern int MP_isam_to_buf(MPAGE *mp);
extern int MP_retrieve_rec(MPAGE *mp);
extern int MP_rebuild_buf(MPAGE *mp);
extern int MP_mouse_in_lpg(MPAGE *mp);
extern int MP_mouse_event(MPAGE *mp);
extern int SC_set_field_FF(ISAM *is,int fld_nb);
extern int SC_set_rec_FF(ISAM *is);
extern int SC_set_cmp_FF(ISAM *is,int cmp_nb,char *to_def);
extern int MP_set_isam(MPAGE *mp,ISAM *is1,ISAM *is2);
extern int MP_set_index(MPAGE *mp,int cmp_nb);
extern int MP_set_cond(MPAGE *mp,int cond);

/* s_mpfn.c */
extern int MP_view_pg(MPAGE *mp,PAGE *bpg,ISAM *is);
extern int MP_modify_pg(MPAGE *mp,PAGE *bpg,ISAM *is);
extern int MP_create_pg(MPAGE *mp,PAGE *bpg,ISAM *is);
extern int MP_view_add_modify_pg(MPAGE *mp,PAGE *bpg,ISAM *is,int vam);
extern int MP_view_modify(MPAGE *mp,int view);
extern int MP_add(MPAGE *mp);
extern int MP_adjust_bufs(MPAGE *mp);
extern int MP_cmp_index(MPAGE *mp);
extern int MP_delete(MPAGE *mp);
extern int MP_re_search(MPAGE *mp,int rec_nb);
extern int MP_reposition(MPAGE *mp,int rec_nb);
extern int MP_del_buf(MPAGE *mp);
extern int MP_shift_buf_up(MPAGE *mp);
extern int MP_shift_buf_down(MPAGE *mp);
extern int MP_is_rec_ok(MPAGE *mp);
extern int MP_is_spg_ok(MPAGE *mp);
extern int MP_search(MPAGE *mp);
extern int MP_next(MPAGE *mp, int recur);
extern int MP_prev(MPAGE *mp, int recur);
extern int MP_check_key(void);
extern int MP_redisplay(MPAGE *mp);
extern int MP_display_screen(MPAGE *mp,int from,int to);
extern int MP_read_screen(MPAGE *mp,char *rec);
extern int MP_fill_screen(MPAGE *mp, int flag);
extern int MP_display_lpg(MPAGE *mp,int line,int invert);
extern int MP_error(void);
extern int MP_count_add(MPAGE *mp,int nbread,int nbsel);
extern int MP_count(MPAGE *mp,int nbread,int nbsel);
extern int MP_check_repos(MPAGE *mp);
extern int MP_select_view(MPAGE *mp,int line,PAGE **lpg,ISAM **is);
extern int MP_get_view(MPAGE *mp,PAGE **bpg,ISAM **is,int *type,int view_create);
extern int MP_retr_apg(MPAGE *mp);
extern int MP_edit_1key(MPAGE *mp,int key, int diff);
extern int MP_reset_apg(MPAGE *mp);

/* s_pgsrch.c */
extern int PG_search(PAGE *pg,ISAM *is);
extern int PG_search_1(PAGE *pg,ISAM *is);

/* s_isdb.c */
extern int IS_create_dbase(ISAM *is);
extern int IS_delete_dbase(ISAM *is);
extern int IS_rebuild_dbase(ISAM *is);

/* s_bcis.c */
extern int BC_edis_create1(BC *bc,int retry);
extern int BC_edis_create2(BC *bc);
extern int BC_next_bc(BC *bc);
extern int BC_prev_bc(BC *bc);
extern int BC_modify_bc(BC *bc);
extern int BC_delete_bc(BC *bc);
extern int BC_nb_lbcs(BC *bc);
extern char *BC_lbc_rec(BC *bc,int i);
extern char *BC_lpg_rec(BC *bc,int i);
extern int BC_get_lbc_field(BC *bc,int line_nb,int fld_nb,char *ptr);

/* s_bcl.c */
extern int BC_free_lbcs(BC *bc);
extern int BCL_free(BCLIST *bcl);
extern BCLIST *BCL_locate(BCLIST *bcl,int n);
extern BCLIST *BC_add_bcl(BC *bc,int n);
extern int BC_delete_bcl(BC *bc,int n);

/* s_scset.c */
extern int SC_set_isam_gn_1(ISAM *is,long nb_read,long nb_found,int success);
extern long SC_set_isam_gn(ISAM *is,char *rec,int disp,int stop);
extern long SC_set_page_gn(PAGE *pg,ISAM *is,int disp,int stop);

/* s_cpis.c */
extern int IS_translate(ISAM *is1,ISAM *is2);

/* s_scact.c */
extern int PG_scan_isam(PAGE *pg);
extern int SC_scan_index(PAGE *pg,ISAM *is);

/* s_isdmp.c */
extern int IS_dump_rec(ISAM *is,char *filename);
extern int IS_dump_rec_interne(ISAM *is,char *filename);
extern int IS_dump_long_txt(FILE *fd,char *txt);

/* s_bcdw.c */
extern int BC_start1(BC *bc);
extern int BC_edit_end(BC *bc);

/* s_edbc.c */
extern int BC_edit_1key(BC *bc,int key, int diff);
extern int BC_fnkeys(BC *bc,int key);
extern int BC_if_fnkeys(BC *bc,int key);
extern int BC_up(BC *bc);
extern int BC_down(BC *bc);
extern int BC_pg_up(BC *bc);
extern int BC_pg_down(BC *bc);
extern int BC_home(BC *bc);
extern int BC_end(BC *bc,int disp);
extern long BC_calc_count(BC *bc,int line,int pos,int nl);
extern int BC_renum_lbcs(BC *bc);
extern int BC_renum_lbc_1(BC *bc,int i,long count);
extern int BC_ask_insert_pos(BC *bc);
extern int BC_create_lbc(BC *bc);
extern int BC_view_lbc(BC *bc);
extern int BC_modify_lbc(BC *bc);
extern int BC_view_modify_lbc(BC *bc,int view_mod);
extern int BC_search_lbc(BC *bc,int line);
extern int BC_delete_lbc(BC *bc);
extern int BC_open(BC *bc);
extern int BC_close(BC *bc);
extern int BC_search(BC *bc,int cmp_nb,int mode);
extern int BC_next(BC *bc);
extern int BC_prev(BC *bc);
extern int BC_search_lbcs(BC *bc);
extern int BC_check_eq(BC *bc,char *val,int flag);
extern int BC_set_lbc_count(BC *bc,long count);
extern int BC_bc_to_lbc(BC *bc,int flag,long count);
extern int BC_isam_to_bcl(BC *bc,int bcl_nb);
extern int BC_write_bc(BC *bc);
extern int BC_rewrite_bc(BC *bc);
extern int BC_delete(BC *bc);
extern int BC_delete_lbcs(BC *bc);
extern int BC_display_lbcs(BC *bc);
extern int BC_display_screen(BC *bc,int from,int to);
extern int BC_display_flds(BC *bc);
extern int BC_empty(BC *bc);
extern long BC_get_count(BC *bc,int line);
extern int BC_set_count(BC *bc,int line,long count);
extern int BC_set_lpg_count(BC *bc);
extern int BC_in_mark(BC *bc,int line);
extern int BC_mark(BC *bc);
extern int BC_unmark(BC *bc);
extern int BC_pgrec_len(BC *bc);
extern int BC_cp_mark(BC *bc);
extern int BC_del_mark(BC *bc);
extern int BC_adjust(BC *bc);
extern int BC_exec_link(BC *bc);
extern int BC_count_fld(BC *bc,int *type);
extern int BC_select_view(BC *bc,BCLIST *bcl,PAGE **lpg,ISAM **is);
extern int BC_get_view(BC *bc,PAGE **bpg,ISAM **is,int view_create);
extern int BC_empty_views(BC *bc);
extern int BC_search_bc_lbc(BC *bc);
extern int BC_edit(BC *bc);
extern int BC_edit_lbcs(BC *bc);

/* s_mpbc.c */
extern int MP_modify_bc(MPAGE *mp,BC *bc);
extern int MP_view_bc(MPAGE *mp,BC *bc);
extern int MP_view_modify_bc(MPAGE *mp,BC *bc,int vm);
extern int MP_delete_bc(MPAGE *mp,BC *bc);
extern int MP_create_bc(MPAGE *mp,BC *bc);

/* s_bcd.c */
extern int BC_edit_isam(BC *bc);
extern int BC_edis_create(BC *bc);
extern int BC_edis_query(BC *bc);
extern int BC_display_sbar(BC *bc);
extern int BC_display_lpg(BC *bc,int line,int invert);
extern int BC_display_cmt(BC *bc);
extern int BC_fnk_menu(BC *bc);
extern int BC_edit_lbcs_loop(BC *bc);
extern int BC_undisplay(BC *bc);
extern int BC_display(BC *bc);

/* s_mp.c */
extern MPAGE *MP_create(void);
extern int MP_init(MPAGE *mp);

/* s_mpd.c */
extern int MP_edit_isam(MPAGE *mp);
extern int MP_edit(MPAGE *mp);
extern int MP_display_lpg_dw(MPAGE *mp,PAGE *lpg,int line,int invert);
extern int MP_empty_lpg_dw(MPAGE *mp,PAGE *lpg,int line,int ch);
extern int MP_display_cmt(MPAGE *mp);
extern int MP_fnk_menu(MPAGE *mp);

/* s_abis.c */
extern int AB_edit_isam(PAGE *pg);
extern int AB_edis_query(A_BAR *ab,PAGE *pg,ISAM *is);

/* s_bcab.c */
extern int BCAB_edit_isam(BC *bc);
extern int BCAB_edis_query(A_BAR *ab,BC *bc);

/* s_hsis.c */
extern int IS_init_history(void);
extern int IS_open_trans_();
extern int IS_end_history(void);
extern int IS_close_trans_(void);
extern int IS_trans_write(ISAM *is);
extern int IS_trans_rewrite(ISAM *is);
extern int IS_trans_delete(ISAM *is);

/* s_hsi2.c */
extern int SCR_load_history(char *file_hsi,int flag);
extern char *HSI_read_name(FILE *fd);
extern int HSI_open(ISAM *is);
extern int HSI_read_to_next(FILE *fd,int type);

/* s_scpage.c */
extern long SC_scan_page_gn(PAGE *pg,ISAM *is,int cmp_nb,int (*fn)(),int disp,int stop,int rev);
extern long SC_scan_page(PAGE *pg,ISAM *is,int cmp_nb,int (*fn)(),int disp);
extern int SC_save_crit(PAGE *pg,ISAM *is,char **from,char **to);
extern int SC_check_pg_crec(PAGE *pg,ISAM *is);
extern int SC_page_to_isam2(PAGE *pg,ISAM *is);
extern int SC_pg_setcheck2(PAGE *pg,ISAM *is,char *def);
extern int SC_pg_setcheck(PAGE *pg,ISAM *is,char *def);
extern int SC_fld_setcheck(PAGE *pg,ISAM *is,char *def,int fld_nb);

/* s_scisck.c */
extern int SC_check_crec(ISAM *is,char *from,char *to);

/* s_isauto.c */
extern int PG_set_auto_num(PAGE *pg,ISAM *is1);

/* s_ccmtmp.c */
extern int MP_calc_cmt(MPAGE *mp);
extern int MP_construct_tbl_cmt(MPAGE *mp);

/* s_ccmtbc.c */
extern int BC_calc_cmt(BC *bc);
extern int BC_construct_tbl_cmt(BC *bc);

/* s_ediscr.c */
extern int SCR_edis_create(PAGE *pg,ISAM *is);
extern int SCR_modify_rec(PAGE *pg,ISAM *is);
extern int PG_create_rec(PAGE *pg);

/* s_vfld.c */
extern int IS_set_vfield(ISAM *is,int fld_nb,char *txt,long len);
extern long IS_get_len_vfield(ISAM *is,int fld_nb);
extern int IS_get_nuvfld(ISAM *is,int fld);
extern ISFIELD *IS_get_vfld_isf(ISAM *is,int vfld);
extern int IS_get_vfield(ISAM *is,int fld_nb,char **ptr);
extern int IS_compose_vrec(ISAM *is,long *lenrec,char **vrec);
extern int IS_decompose_vrec(ISAM *is,char *vrec,long rec_len);
extern int IS_free_vptr(ISAM *is);
extern int IS_alloc_vfld(ISAM *is);

/* s_isld.c */
extern long IS_load_rec(ISAM *is,char *filename);
extern int LA_read(FILE *fd,int len,char *buf);
extern int IS_load_rec_interne(ISAM *is,char *filename);
extern int LA_len_next(FILE *fd);
extern int LA_cnv_txt(char *txt);
/* ctree4\s_ctree.c */
extern int IS_create_struct(ISAM *is);
extern int IS_set_name_db(ISAM *is,char *name);
extern int IS_init_struct(ISAM *is);
extern int IS_free_struct(ISAM *is);
extern int IS_init_isam(void);
extern int IS_open_isam(ISAM *is,int mode,int lock);
extern int IS_close_db(ISAM *is);
extern int IS_search_rec(ISAM *is,int nu_index,int cond,char *order);
extern int IS_lock_rec(ISAM *is);
extern int IS_unlock_rec(ISAM *is);
extern int IS_reread_rec(ISAM *is);
extern int IS_write_rec(ISAM *is);
extern int IS_next_ge_le(ISAM *is);
extern int IS_prev_ge_le(ISAM *is);
extern int IS_next_rec(ISAM *is);
extern int IS_prev_rec(ISAM *is);
extern int IS_check_eq(ISAM *is,char *rec);
extern int IS_rewrite_rec(ISAM *is);
extern int IS_delete_rec(ISAM *is);
extern char *IS_error_text(void);
extern int IS_print_stat(ISAM *is);
extern int IS_search_nb_2val(void);
extern int IS_search_rec_idx(void);
extern int IS_next_rec_idx(void);
extern int IS_prev_rec_idx(void);
extern int IS_set_firstv(void);
extern int IS_set_lastv(void);
extern int IS_start_trans(void);
extern int IS_start_point_trans(void);
extern int IS_restart_point_trans(void);
extern int IS_stop_trans(void);
extern int IS_abort_trans(void);
extern int IS_end_database(void);
extern int IS_init_database(void);
extern int IS_delete_idx(ISAM *is);
extern int IS_delete_index(ISAM *is,int i);
extern int IS_sql_menu_order(ISAM *is,PAGE *pg,char **order);
extern int IS_sql_simple_order(ISAM *is,PAGE *pg,char **order);
extern int IS_exec_sql_file(char *file);
/* ctree4\s_ctdb.c */
extern int IS_create_db(ISAM *is);
extern int IS_add_resource(ISAM *is);
extern int IS_delete_db(ISAM *is);
extern int IS_rebuild(ISAM *is);
extern int IS_compact_isam(ISAM *is);
extern CTINFO *IS_get_doda(char *filename,int mode);

extern char *IS_get_tblname(ISAM *is);

#ifdef __cplusplus
}
#endif

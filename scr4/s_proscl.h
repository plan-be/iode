#ifdef __cplusplus
extern "C" {
#endif

/* s_scroll.c */
extern int SCRL_edit(SCROLL *scrl);
extern int SCRL_fedit(void);
extern int SCRL_fdisp(void);
extern int SCRL_fins(void);
extern int SCRL_fdel(void);
extern int SCRL_ffn(int key);
extern int SCRL_init(SCROLL *scrl);
extern int SCRL_display(int disp);
extern int SCRL_h(int bl,int nl);
extern int SCRL_w(int bc,int nc);
extern int SCRL_calc_pos(void);
extern int SCRL_adapt_pos(int im);
extern int SCRL_image(int nl,int nc);
extern int SCRL_display_scroll(int disp);
extern int SCRL_disp_frm(void);
extern int SCRL_disp_titles(void);
extern int SCRL_disp_ltitles(void);
extern int SCRL_disp_ctitles(void);
extern int SCRL_disp_title(void);
extern int SCRL_disp_ltitle(int i);
extern int SCRL_disp_ctitle(int i);
extern int SCRL_disp_flds(void);
extern int SCRL_disp_fld(int l,int c,int attr);
extern int SCRL_write_fld(char *ptr,int line,int col,int attr,int lg,int align);
extern int SCRL_adjust(int *h0,int *h,int hm,int nl,int *nw,int rc);
extern int SCRL_rotate(int *nl,int *ol0,int *nc,int *oc0);
extern int SCRL_maximize(void);
extern int SCRL_resize(void);
extern int SCRL_move(void);
extern int SCRL_reset_im(void);
extern int SCRL_goto(int key);
extern int SCRL_select(void);
extern int SCRL_selected(int line);
extern int SCRL_des(int line);
extern int SCRL_deselect_all(void);
extern int SCRL_sel(int line);
extern int SCRL_ccoord(int *l,int *c);
extern int SCRL_change_lc0(int l,int c);
extern int SCRL_mouse_event(SCROLL *scrl,int *nline,int *ncol,int *ol0,int *oc0);
extern int SCRL_scmts(void);

#ifdef __cplusplus
}
#endif

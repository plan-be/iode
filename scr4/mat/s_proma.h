#ifdef __cplusplus 
extern "C" { 
#endif 


/* s_maallc.c */ 
extern MAT *M_alloc(int nl,int nc);
extern int M_free(MAT *m1);
extern double *M_vswp_to_cnv(double *row2,VECT row1,int dim);
extern VECT M_vcnv_to_swp(VECT row2,double *row1,int dim);
extern double *M_alloc_vec(int nc);
extern int M_free_vec(double *vec);

/* s_madup.c */ 
extern MAT *M_copy(MAT *m2,MAT *m1);
extern MAT *M_dup(MAT *m1);

/* s_maerr.c */ 
extern char *M_error(void);

/* s_maprod.c */ 
extern MAT *M_prod(MAT *m3,MAT *m1,MAT *m2);

/* s_matran.c */ 
extern MAT *M_trans(MAT *m2,MAT *m1);

/* s_mainv.c */ 
extern MAT *M_inv_1(MAT *m2,MAT *m1);
extern int M_inv_init(int dim,MAT *m2,MAT *m1);
extern MAT *M_inv_2(MAT *m2,MAT *m);
extern int M_inv_pivot(int r_row,MAT *m1,MAT *m2);

/* s_madet.c */ 
extern double M_det_1(MAT *m1);
extern int M_det_init(int dim,MAT *m1,double *det);
extern double M_det_2(MAT *m1);
extern int M_det_pivot(int dim,int row,MAT *m1,double *p_det,int *sing);

/* s_masol.c */ 
extern MAT *M_solve(MAT *m3,MAT *m1,MAT *m2);
extern int M_gel_init(int dim,MAT *m1,MAT *m2,MAT *m3);
extern int M_gel_ut(int dim,MAT *m1,MAT *m2);
extern int M_gel_pivot(int dim,int r_row,MAT *m1,MAT *m2);
extern int M_bw_sub(int dim,MAT *m1,MAT *m2,MAT *m3);

/* s_mamisc.c */ 

/* s_maro.c */ 
extern int M_ero_sw(int dim,double *row1,double *row2);
extern int M_ero_ma(int dim,double mult,double *row1,double *row2);
extern int M_ero_div(int dim,double div,double *row);

/* s_matrac.c */ 
extern MAT *M_trace(MAT *m2,MAT *m1);

/* s_macalc.c */ 
extern MAT *M_calc(MAT *m3,MAT *m1,MAT *m2,int oper);

/* s_makron.c */ 
extern MAT *M_diag(int mdim,double dscale);

/* s_mascal.c */ 
extern MAT *M_scale(MAT *m2,MAT *m1,double scale);

/* s_manull.c */ 
extern int M_clear(MAT *m1);

/* s_maextr.c */ 
extern MAT *M_extr(MAT *m2,MAT *m1,int l,int c,int nl,int nc);

/* s_maxpx.c */ 
extern MAT *M_xprimx(MAT *m2,MAT *m1);

/* s_maxxp.c */ 
extern MAT *M_xxprim(MAT *m2,MAT *m1);

/* s_maxaxp.c */ 
extern MAT *M_xaxprim(MAT *m3,MAT *m1,MAT *m2);

/* s_maxpax.c */ 
extern MAT *M_xpri_max_(MAT *m3,MAT *m1,MAT *m2);

/* s_maprt.c */ 
extern int M_tprintf(FILE *fd,MAT *m1,char *title,char **t_line,char **t_col);
extern int M_printf(FILE *fd,MAT *m1,char *title);

/* s_madump.c */ 
extern int M_dump(FILE *fd,MAT *m1,char *tit);

/* s_mascrl.c */ 
/* JMP 23/11/2011
extern char *M_sc_title(SCROLL *scrl);
extern char *M_sc_ctitle(SCROLL *scrl,int j);
extern char *M_sc_ltitle(SCROLL *scrl,int i);
extern int M_sc_nl(SCROLL *scrl);
extern int M_sc_nc(SCROLL *scrl);
extern char *M_sc_text(SCROLL *scrl,int i,int j);
extern int M_sc_len(SCROLL *scrl,int i);
extern int M_scroll_init(SCROLL *scrol,int (*fn)());
extern int M_scroll(MAT *m1,char *title,char **ltitle,char **ctitle,int (*fn)(),int lc0,int lc);
*/

/* s_maset.c */ 
extern MAT *M_set(MAT *m2,MAT *m1,int lig,int col);

/* s_mastat.c */ 
extern double M_cstat(MAT *m1,int c,int op);
extern double M_lstat(MAT *m1,int l,int op);

/* s_masol2.c */ 
extern MAT *M_solve_2(MAT *m3,MAT *m,MAT *m4);

#ifdef __cplusplus 
} 
#endif 

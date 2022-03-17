#ifndef _IODEBASE_
#define _IODEBASE_

#ifdef SCRCPP
extern "C" {
#endif

/* k_ws.c */
extern void K_init_ws(int );
extern void K_end_ws(int );


/* k_pack.c */
extern int K_vpack(char **,double *,int *);
extern int KV_alloc_var(int );
extern int K_ipack(char **,char *);
//extern char *K_tcell_pack(char *,TCELL *); 
extern int K_tpack(char **,char *);
extern int K_epack(char **,char *,char *);
extern int K_spack(char **,char *);
extern int KS_alloc_scl(void);
extern int K_cpack(char **,char *);
extern int K_lpack(char **,char *);
extern int K_opack(char **,char *,int *);
//extern void K_vunpack(void);
//extern void K_iunpack(void);
extern TBL *K_tunpack(char *);
extern EQ *K_eunpack(char *);
//extern void K_sunpack(void);
//extern void K_cunpack(void);
//extern void K_lunpack(void);
//extern void K_onpack(void);

/* k_type.c */
extern int isfile(int ,char **);
extern int isname(int ,char **);
extern int isper(int ,char **);
extern int isreal(int ,char **);
extern int isint(int ,char **);
extern int ismethod(int ,char **);
extern int isyon(int ,char **);

/* k_error.c */
extern int kclmsg(int ,int );
extern void kwrmsg(char *);

/* k_errorv.c */

/* k_super.c */
extern void    (*kerror_super)(int level, char*fmt);
extern void    (*kwarning_super)(char* msg);
extern void    (*kmsg_super)(char*fmt);
extern int     (*kwprintf_super)(char* msg);
extern void    (*kpanic_super)();
extern int     (*kconfirm_super)(char* msg);
extern int     (*kmsgbox_super)(unsigned char *str, unsigned char *v, unsigned char **buts);
extern int     kmsgbox_continue;
extern void    (*krecordkey_super)(int ch);
extern void    (*ksettitle_super)(void);
extern int     (*ktermvkey_super)(int vkey);
extern int     (*khitkey_super)();
extern int     (*kgetkey_super)();
extern void    (*kbeep_super)(void);
extern SAMPLE* (*kasksmpl_super)(void);
extern int     (*kexecsystem_super)(char*);
extern int     (*kshellexec_super )(char*);

extern void    kerror(int level, char* fmt, ...);
extern void    kwarning(char* fmt, ...);
extern void    kmsg(char* fmt, ...);
extern int     kwprintf(char* msg, ...);
extern void    kpanic(void);
extern int     kconfirm(char* fmt, ...);
extern int     kmsgbox(unsigned char *str, unsigned char *v, unsigned char **buts);
extern void    krecordkey(int ch);
extern void    ksettitle(void);
extern int     ktermvkey(int vkey);
extern int     khitkey();
extern int     kgetkey();
extern void    kbeep();
extern SAMPLE* kasksmpl();
extern int     kexecsystem(char*);
extern int     kshellexec(char*);
extern char    *A_expand_super_API(char* name);

extern void    IODE_assign_super_API(void);

/* k_cmp.c */
extern int K_cmp(char *,KDB *,KDB *);

/* k_lec.c */
extern char *(*L_expand_super)(char* list_name);

extern IODE_REAL *L_getvar(KDB *,int );
extern IODE_REAL L_getscl(KDB *,int );
extern SAMPLE *L_getsmpl(KDB *);
extern int L_findscl(KDB *,char *);
extern int L_findvar(KDB *,char *);
extern char* L_expand(char* list_name);

/* pack.c */
extern int P_free(char *);
extern void *P_create(); 
extern void *P_add(void *,void *,int );
extern void *P_get_ptr(void *,int );
extern OSIZE P_get_len(void *,int );
extern void *P_alloc_get_ptr(void *, int );

/* k_descr.c */
extern int K_filetype(char *,char *,int *,SAMPLE *);

/* k_kdb.c */
extern KDB *K_init_kdb(int ,char *);
extern void K_set_kdb_name(KDB *kdb, U_ch *filename);
extern KDB *K_create(int ,int );
extern int K_free(KDB *);
extern int K_clear(KDB *);
extern int K_free_kdb(KDB *);
extern int K_merge(KDB* kdb1, KDB* kdb2, int replace);
extern KDB* K_refer(KDB* kdb, int nb, char** names);
extern KDB *K_quick_refer(KDB *, char **names);

/* k_objs.c */
extern int K_key(char *,int );
extern int K_dup(KDB *,char *,KDB *,char *);
extern int K_ren(KDB *,char *,char *);
extern int K_add_entry(KDB *,char *);
extern int K_find(KDB *,char *);
extern int K_del(KDB *,int );
extern int K_del_entry(KDB *,int );
extern int K_del_by_name(KDB* kdb, char* name);

/* k_objvers.c */
extern int K_calcvers(char *);
extern void K_setvers(KDB* kdb, int i, int vers);

/* k_objfile.c */
extern char *K_set_ext(char *,char *,int );
extern void K_strip(char *);
extern int K_merge(KDB *,KDB *,int );
extern int K_merge_del(KDB *,KDB *,int );
extern KDB *K_load(int ,char *,int ,char **);
extern KDB *K_interpret(int ,char *);
extern int K_copy(KDB *,int ,char **,int ,char **,SAMPLE *);
extern int K_cat(KDB *,char *);
extern int K_backup(char *);
extern int K_save(KDB *,char *);
extern int K_save_ws(KDB *);
extern int K_setname(char *,char *);

/* buf.c */
extern char *BUF_alloc(int );
extern void BUF_free(void);
extern void BUF_lock(void);
extern void BUF_unlock(void);
extern char *BUF_memcpy(char *,int );
extern char *BUF_strcpy(char *);

/* per.c */
extern int L_pos(char *,int );
extern int PER_nb(int );
extern int PER_diff_per(PERIOD *,PERIOD *);
extern PERIOD *PER_addper(PERIOD *,int );
extern char *PER_pertoa(PERIOD *,char *);
extern PERIOD *PER_atoper(char *);
extern int PER_common_smpl(SAMPLE *,SAMPLE *,SAMPLE *);
extern SAMPLE *PER_atosmpl(char *,char *);
extern SAMPLE *PER_pertosmpl(PERIOD *,PERIOD *);
extern char *PER_smpltoa(SAMPLE *,char *);
extern int PER_nbper(PERIOD *);
extern IODE_REAL PER_per2real(PERIOD *,int );
extern char *PER_real2a(IODE_REAL ,int );

/* yy.c */
extern IODE_REAL K_read_real(YYFILE *);
extern long K_read_long(YYFILE *);
extern char *K_read_str(YYFILE *);
extern PERIOD *K_read_per(YYFILE *);
extern SAMPLE *K_read_smpl(YYFILE *);
extern int K_read_align(YYFILE* yy);
extern void K_stracpy(char **,char *);
extern int K_wrdef(FILE *,YYKEYS *,int );
//extern int K_compare(YYKEYS *,YYKEYS *);
extern char *K_wrap(char *,int );

/* k_xdr.c */
extern void K_xdrPINT(unsigned char *);
extern void K_xdrPLONG(unsigned char *);
extern void K_xdrKDB(KDB *,KDB **);

/* l_token.c */
extern int L_open_all(char *,int );
extern void L_close(void);
//extern int L_lex(void);
//extern int L_read(void);
//extern int L_unread(void);
//extern int L_macro(void);
//extern int L_string(void);
//extern int L_read_string(void);
//extern int L_getc(void);
//extern void L_ungetc(int );
//extern void L_skip(void);
extern int L_get_token(void);
//extern int L_include(char *,char *);
//extern int L_get_int(void);
//extern int L_get_period(YYFILE *,PERIOD *);

/* l_cc1.c */
extern void L_alloc_expr(int );
extern int L_cc1(int );
// extern int L_add_new_series(char *);
extern void L_free_anames(void);
//extern int L_save_var(void);
//extern int L_save_op(void);
//extern int L_priority_sup(int );
//extern int L_add_stack(int );
//extern int L_clear_stack(void);
//extern int L_anal_lag(void);
extern int L_sub_expr(ALEC *,int );
//extern int L_time_expr(void);
//extern int L_lag_expr(int );

/* ode.c */
//extern  char    *L_expand(char *);

/* l_alloc.c */
extern char *L_malloc(int );
//extern int L_free(char *);
extern void L_free(void *);

/* l_err.c */
extern char *L_error(void);

/* l_cc2.c */
extern CLEC *L_cc2(ALEC *);
extern void L_move_arg(char *,char *,int );
//extern int L_calc_len(ALEC *,int ,int );
extern CLEC *L_cc_stream(void);
extern CLEC *L_cc(char *);

/* l_link.c */
extern int L_link(KDB *,KDB *,CLEC *);
// extern int L_link1(KDB *,KDB *,CLEC *);
//extern int L_link2(KDB *,CLEC *);
//extern int L_link_sub(KDB *,char *,int );
extern void L_link_endos(KDB* kde, CLEC* cl);

/* l_exec.c */
#ifdef _MSC_VER
        extern int matherr(struct _exception *);
#else
        extern int matherr(struct exception *);
#endif
extern void L_fperror(void);
extern double L_exec(KDB *,KDB *,CLEC *,int );
extern double L_exec_sub(unsigned char *,int ,int ,double *);
extern int L_intlag(double );
extern double L_uminus(double *);
extern double L_uplus(double *);
extern double L_logn(double );
extern double L_log(double *,int );
extern double L_ln(double *);
extern double L_not(double *);
extern double L_expn(double *, int);
extern double L_max(double *,int );
extern double L_min(double *,int );
extern double L_sin(double *);
extern double L_asin(double *);
extern double L_cos(double *);
extern double L_acos(double *);
extern double L_tan(double *);
extern double L_atan(double *);
extern double L_sinh(double *);
extern double L_cosh(double *);
extern double L_tanh(double *);
extern double L_sqrt(double *);
extern double L_int(double *);
extern double L_abs(double *);
extern double L_sign(double *);
extern double L_rad(double *);
extern double L_if(double *,int );
extern double L_fnisan(double *,int );
extern double L_lcount(double *,int );
extern double L_lprod(double *,int );
extern double L_lsum(double *,int );
extern double L_lstderr(double *,int );
extern double L_lmean(double *,int );
extern double L_random(double *);
extern double L_or(double ,double );
extern double L_and(double ,double );
extern double L_ge(double ,double );
extern double L_gt(double ,double );
extern double L_eq(double ,double );
extern double L_ne(double ,double );
extern double L_le(double ,double );
extern double L_lt(double ,double );
extern double L_plus(double ,double );
extern double L_minus(double ,double );
extern double L_divide(double ,double );
extern double L_times(double ,double );
extern double L_exp(double ,double );
extern double L_pi(void);
extern double L_euro(void);
extern double L_e(void);
extern double L_time(int );
extern double L_i(int );
extern double L_lag(unsigned char *,short ,int ,double *,int );
extern double L_diff(unsigned char *,short,int ,double *,int );
extern double L_rapp(unsigned char *,short,int ,double *,int );
extern double L_dln(unsigned char *,short,int ,double *,int );
extern double L_grt(unsigned char *,short,int ,double *,int );
extern double L_mavg(unsigned char *,short,int ,double *,int );
extern void L_tfn_args(int ,double *,int ,int *,int *);
extern double L_vmax(unsigned char *,short,int ,double *,int );
extern double L_vmin(unsigned char *,short,int ,double *,int );
extern double L_sum(unsigned char *,short,int ,double *,int );
extern double L_prod(unsigned char *,short,int ,double *,int );
extern double L_mean(unsigned char *,short ,int ,double *,int );
extern double L_stderr(unsigned char *,short,int ,double *,int );
extern double L_lastobs(unsigned char *,short,int ,double *,int );
extern double L_calccorr(unsigned char *,short ,unsigned char *,short ,int ,double *,int );
extern double L_corr(unsigned char *,short ,int ,double *,int );
extern double L_calccovar(unsigned char *,short ,unsigned char *,short ,int ,double *,int ,int );
extern double L_covar(unsigned char *,short ,int ,double *,int );
extern double L_covar0(unsigned char *,short ,int ,double *,int );
extern double L_var(unsigned char *,short ,int ,double *,int );
extern double L_stddev(unsigned char *,short ,int ,double *,int );
extern double L_index(unsigned char *,short ,int ,double *,int );
extern double L_acf(unsigned char *,short ,int ,double *,int );
extern int L_stackna(double **,int );
extern int L_calcvals(unsigned char *,short ,int ,double *,int *,double *, int);
extern double L_interpol(unsigned char *,short ,int ,double *,int );
extern double L_app(unsigned char *,short ,int ,double *,int );

/* l_hodrick.c */
extern int HP_calc(IODE_REAL *f_vec, IODE_REAL *t_vec, int nb, IODE_REAL lambda, int std);
extern void HP_test(IODE_REAL *f_vec, IODE_REAL *t_vec, int nb, int *beg, int *dim);

/* l_eqs.c */
extern CLEC *L_solve(char *,char *);
extern int L_split_eq(char *);

/* l_newton.c */
extern double L_zero(KDB *,KDB *,CLEC *,int ,int ,int );
extern double L_newton(KDB *,KDB *,CLEC *,int ,int ,int );
//extern double L_newton_1(int ,KDB *,KDB *,CLEC *,int ,int ,int );

/* l_secant.c */
extern double L_fx(double ,int );
extern int L_bracket(double *,double *,int );
extern double L_secant(KDB *,KDB *,CLEC *,int ,int ,int );

/* e_est.c */
extern int E_est(char **,char **,KDB *,KDB *,SAMPLE *,int ,char **,int ,double );
extern int E_gls(void);
extern int E_residuals(void);
extern double E_rhs_ij(int ,int );
extern int E_c_rhs(void);
extern int E_jacobian(void);
extern int E_scl_in_eq(int ,int );
extern int E_mod_residuals(int ,int ,double );
extern int E_c_gmg(void);
extern int E_c_gmu(void);
extern int E_deltac(void);
extern int E_testcv(void);
extern int E_adaptcoef(void);
extern int E_c_vcu(void);
extern int E_c_ivcu(void);
extern int E_c_mcu(void);
extern int E_c_vcc(void);
extern int E_c_ivcc(void);

/* e_step.c */
extern IODE_REAL C_evallec(char *,int);

/* e_tests.c */
extern double M_c_line(MAT *,int ,int );
extern double E_div_0(double ,double );
extern int E_output(void);
extern void E_deg_freed(void);
extern double E_c_umu(void);
extern int E_c_loglik(void);
extern int E_c_mcorr(void);
extern int E_c_mcorru(void);
extern int E_c_ttests(void);

/* e_prep.c */
extern int E_prep(char **,char **);
extern int E_prep_lecs(char **);
extern int E_prep_instrs(char **);
extern int E_prep_coefs(void);
extern int E_get_C(void);
extern int E_put_C(void);
extern int E_get_SMO(void);
extern int E_prep_alloc(void);
extern int E_free_work(void);
extern int E_prep_reset(void);

/* e_error.c */
extern void E_msg_n(int );
extern void E_error_n(int );

/* e_print.c */
extern void E_print_enum(void);
extern void E_BeginTbl(char *,int );
extern void E_print_parms(void);
extern void E_print_eqs(void);
extern void E_print_instrs(void);
extern void E_print_coefs(void);
extern void E_print_mcorr(void);
extern void E_print_mcorru(void);
extern void E_print_eqres_1(int );
extern void E_print_eqres_2(int );
extern void E_print_eqres(int );
extern int E_print_results(int ,int ,int ,int ,int );

/* e_stat.c */
extern double *E_UnitRoot(char *,int ,int ,int );
extern int E_UnitRoot_1(SAMPLE *,char *);
extern void E_SclToReal(char *,double *);
extern void E_PrintDF(char *,double *,int ,int ,int );
extern int E_GetLecName(char *,char *);

/* k_wsvar.c */
extern int KV_sample(KDB *,SAMPLE *);
extern int KV_merge(KDB *,KDB *,int );
extern void KV_merge_del(KDB *,KDB *,int );
extern double KV_get(KDB *,int ,int ,int );
extern void KV_set(KDB *,int ,int ,int ,double );
extern int KV_extrapolate(KDB *,int ,SAMPLE *,char **);
extern KDB *KV_aggregate(KDB *,int ,char *,char *);
void KV_init_values_1(IODE_REAL* val, int t, int method);
//extern int KV_GetSmpl(SAMPLE *,char *);

/* k_val.c */
extern char *K_oval(KDB *,int ,int );
extern char *K_oval0(KDB *,int );
extern char *K_oval1(KDB *,int );
extern double *K_vval(KDB *,int ,int );

extern char *K_optr(KDB *,char* ,int );
extern char *K_optr0(KDB *,char* );
extern char *K_optr1(KDB *,char* );
extern double *K_vptr(KDB *,char* ,int );

extern EQ* K_eptr(KDB* kdb, char* name);

/* k_lst.c */
extern int K_scan(KDB *,char *,char *);
//extern void K_clecscan(KDB *,CLEC *,KDB *,KDB *);
extern void KE_scan(KDB *,int ,KDB *,KDB *);
extern void KI_scan(KDB *,int ,KDB *,KDB *);
extern void KT_scan(KDB *,int ,KDB *,KDB *);
extern int KL_lst(char *,char **,int );
extern unsigned char **KL_expand(char *);

/* k_eqs.c */
extern void E_free(EQ *);
extern int E_split_eq(char *,char **,char **);
extern int E_dynadj(int ,char *,char *,char *,char **);
extern int E_DynamicAdjustment(int ,char **,char *,char *);

/* k_tbl.c */
extern TBL *T_create(int );
extern void T_free(TBL *);
extern void T_free_line(TLINE *,int );
extern void T_free_cell(TCELL *);
extern int T_add_line(TBL *);
extern TCELL *T_create_cell(TBL *,TLINE *);
extern TCELL *T_create_title(TBL *,TLINE *);
extern char *T_cell_cont(TCELL *,int );
extern int T_insert_line(TBL *,int ,int ,int );
extern int T_set_lec_cell(TCELL *,unsigned char *);
extern void T_set_string_cell(TCELL *,unsigned char *);
extern void T_set_cell_attr(TBL *,int ,int ,int );
extern int T_default(TBL *,char *,char **,char **,int ,int ,int );
extern void T_auto(TBL *,char *,char **,int ,int ,int );

/* k_est.c */
extern int KE_est_s(KDB *,KDB *,KDB *,SAMPLE *,int ,char **);
extern void E_tests2scl(EQ *,int ,int ,int );
extern void E_savescl(double ,int ,char *);
extern void E_savevar(char *,int ,MAT *);

/* k_ccvar.c */
extern KDB *KV_load_str(char *);
extern KDB *KV_load_asc(char *);
extern int KV_save_asc(KDB *,char *);
extern int KV_save_csv(KDB *, char *, SAMPLE *, char **);

/* k_cccmt.c */
extern KDB *KC_load_asc(char *);
extern int KC_save_asc(KDB *,char *);
extern int KC_save_csv(KDB *, char *);

/* k_ccscl.c */
extern KDB *KS_load_asc(char *);
extern int KS_save_asc(KDB *,char *);
extern int KS_save_csv(KDB *, char *);

/* k_ccidt.c */
extern KDB *KI_load_asc(char *);
extern int KI_save_asc(KDB *,char *);
extern int KI_save_csv(KDB *, char *);

/* k_cclst.c */
extern KDB *KL_load_asc(char *);
extern int KL_save_asc(KDB *,char *);
extern int KL_save_csv(KDB *, char *);

/* k_cceqs.c */
extern KDB *KE_load_asc(char *);
extern int KE_save_asc(KDB *,char *);
extern int KE_save_csv(KDB *, char *);

/* k_cctbl.c */
extern KDB *KT_load_asc(char *);
extern int KT_save_asc(KDB *,char *);
extern int KT_save_csv(KDB *, char *);

/* sb_xode.c */
extern int SB_XodeRuleImport(void);
extern int C_XodeRuleImport(void);
extern int X_RuleImport(int ,char *,char *,char *,char *,char *,char *,int ,int );
extern int X_RuleImportCmt(char *,char *,char *,char *,int ,int );
extern int X_RuleImportVar(char *,char *,char *,char *,char *,char *,int );
extern int SB_XodeRuleExport(void);
extern int C_XodeRuleExport(void);
extern int X_RuleExport(char *,char *,char *,char *,char *,char *,char *,char *,char *,int );

/* k_iprn.c */
extern double PRN_readreal(YYFILE *);
extern int IMP_vec_prn(YYFILE *,char *,int ,double *);
extern int IMP_hd_cprn(IMPDEF *,char *,int );
extern int IMP_vec_cprn(char *,char **);

/* k_imain.c */
extern int IMP_readrule(char *);
extern int IMP_grep(char **,char *);
extern int IMP_change(char **,char **,char *,char *);
extern KDB *IMP_InterpretVar(IMPDEF *,char *,char *,SAMPLE *);
extern KDB *IMP_InterpretCmt(IMPDEF *,char *,char *,int );

/* k_iasc.c */
extern int IMP_hd_asc(YYFILE *,SAMPLE *);
extern int IMP_vec_asc(YYFILE *,char *,int ,double *);
extern int IMP_hd_casc(IMPDEF *,char *,int );
extern int IMP_vec_casc(char *,char **);

/* k_irasc.c */
extern int IMP_hd_rasc(YYFILE *,SAMPLE *);
extern int IMP_elem_rasc(YYFILE *,char *,int *,double *);
extern int IMP_end_rasc(void);

/* k_idif.c */
extern int DIF_skip_to(YYFILE *,int );
extern int DIF_cell(YYFILE *,char **,double *);
extern int IMP_hd_dif(YYFILE *,SAMPLE *);
extern int IMP_vec_dif(YYFILE *,char *,int ,double *);
extern int IMP_end_dif(void);

/* k_ibst.c */
extern int IMP_hd_bst(YYFILE *,SAMPLE *);
extern int IMP_elem_bst(YYFILE *,char *,int *,double *);
extern YYFILE *IMP_open_bst(YYKEYS *,int ,char *,char *);
extern char *IMP_addftr(char *,long ,int );
extern int IMP_hd_rbst(int );
extern int IMP_hd_cbst(IMPDEF *,char *,int );
extern int DIF_long(YYFILE *,long *);
extern int IMP_vec_cbst(char *,char **);
extern int IMP_niv(char *);
extern int IMP_end_cbst(void);

/* k_inis.c */
extern int IMP_hd_nis(YYFILE *,SAMPLE *);
extern int IMP_vec_nis(YYFILE *,char *,int ,double *);

/* k_igem.c */
extern int IMP_hd_gem(YYFILE *,SAMPLE *);
extern int IMP_vec_gem(YYFILE *,char *,int ,double *);
extern int IMP_end_gem(void);
extern int GEM_readrubr(YYFILE *);
extern int GEM_readbegin(YYFILE *);
extern int GEM_readsrubr(YYFILE *);
extern int GEM_name(char *);

/* k_emain.c */
extern void EXP_set(char *,char *);
extern void EXP_val(char *,double );
extern char *EXP_addprepost(char *,char *,char *,char **);
extern char *EXP_addsep(char *,char **);
extern int EXP_Ws(EXPDEF *,KDB *,KDB *,char *,char *,char *,char *);
extern int EXP_Rev_Ws(EXPDEF *,KDB *,KDB *,char *,char *,char *,char *);

/* k_wks.c */
extern int wks_init(char *,int ,int );
extern int wks_cwidth(int ,int );
extern int wks_end(void);
extern void wks_string(char *,int ,int );
extern void wks_value(double ,int ,int );
extern void wks_name(char *,int ,int ,int ,int );

/* k_ecsv.c */
extern int EXP_hd_csv(EXPDEF *,KDB *,KDB *,char *);
extern int EXP_end_csv(EXPDEF *,KDB *,KDB *);
extern char *EXP_code_csv(char *,char **);
extern char *EXP_cmt_csv(KDB *,char *,char **);
extern char *EXP_elem_csv(KDB *,int ,int ,char **);
extern int EXP_vec_csv(EXPDEF *,char *,char *,char *);
extern int EXP_hd_rcsv(EXPDEF *,KDB *,KDB *,char *);
extern char *EXP_elem_rcsv(KDB *,int ,int ,char **);
extern int EXP_vec_rcsv(EXPDEF *,char *,char *,char *);

/* k_edif.c */
extern int EXP_hd_dif(EXPDEF *,KDB *,KDB *,char *);
extern int EXP_end_dif(EXPDEF *,KDB *,KDB *);
extern char *EXP_code_dif(char *,char **);
extern char *EXP_cmt_dif(KDB *,char *,char **);
extern char *EXP_elem_dif(KDB *,int ,int ,char **);
extern int EXP_vec_dif(EXPDEF *,char *,char *,char *);

/* k_etsp.c */
extern int EXP_hd_tsp(EXPDEF *,KDB *,KDB *,char *);
extern int EXP_end_tsp(EXPDEF *,KDB *,KDB *);
extern char *EXP_code_tsp(char *,char **);
extern char *EXP_cmt_tsp(KDB *,char *,char **);
extern char *EXP_elem_tsp(KDB *,int ,int ,char **);
extern int EXP_vec_tsp(EXPDEF *,char *,char *,char *);

/* k_ewks.c */
extern int EXP_hd_wks(EXPDEF *,KDB *,KDB *,char *);
extern int EXP_end_wks(EXPDEF *,KDB *,KDB *);
extern char *EXP_code_wks(char *,char **);
extern char *EXP_cmt_wks(KDB *,char *,char **);
extern char *EXP_elem_wks(KDB *,int ,int ,char **);
extern int EXP_vec_wks(EXPDEF *,char *,char *,char *);

/* k_vers.c */
extern char *K_CurrentVersion();
extern char *K_LastVersion();

/* c_cc.c */
extern COLS *COL_cc(char *);
extern int COL_free(COLS *);
extern int COL_stripy(char *);
extern char *COL_ctoa(COL *,int ,int ,int );
extern char *COL_text(COL *,char *,int );
extern COLS *COL_add(COLS *,int ,int );
extern COLS *COL_construct(COLS *,COLS *,FILS *,REP *, int, int);
extern int COL_apply_fil(COL *,FIL *);
extern int COL_read_per(YYFILE *,PERIOD *);
extern COLS *COL_read_y(YYFILE *);
extern int COL_read_rep(YYFILE *,REP *);
extern int COL_read_1f(YYFILE *,FIL *);
extern FILS *COL_read_f(YYFILE *);
extern COLS *COL_read_cols(YYFILE *);
extern int COL_find_mode(COLS *,int *,int );

/* c_calc.c */
extern int COL_resize(TBL *,COLS *);
extern void COL_clear(COLS *);
extern CLEC *COL_cp_clec(CLEC *);
extern int COL_exec(TBL *,int ,COLS *);
extern int COL_calc(COL *,CLEC *,CLEC *);
extern int COL_link(int ,CLEC *);

/* b_args.c */
extern char **B_ainit_chk(char *,ADEF *,int );
extern char **B_vtom_chk(char *,int );
extern int B_loop(char **,int (*)(),char *);
extern int B_ainit_loop(char *,int (*)(),char *);
extern int B_get_arg0(char *,char *,int );
extern int B_argpos(char *,int );

/* b_errors */
extern void B_seterror(char *,...);
extern void B_seterrn(int , ...);

extern void B_display_last_error(void);
extern void B_print_last_error(void);
extern void B_clear_last_error(void);
//extern void B_reset_error(void);
extern char *B_msg(int );
extern int B_get1int(char *);

/* b_dir.c */
extern int C_dir_edit_fn(char *,char *,SCRSTAT *);
extern int B_FileList(char *);
extern int B_FileList2(char *);
extern void B_Read_Dir(char *);
extern int B_FileListGnl(char *,int (*)());
extern void B_path_change(char *);
extern void B_analyse_file(SCRSTAT *,char **,int *,int *,int *,int *,int *,char *,long *,int *);
extern int B_dir_ed_prt(char *);

/* b_file.c */
extern int B_FilePrint(char *,int );

/* b_fdel.c */
extern int B_unlink_1(char *,int *);
extern int B_FileDelete(char *,int );

/* b_fcopy.c */
extern int B_FileCopy(char *,int );

/* b_fren.c */
extern int B_FileRename(char *,int );
extern int B_SysRename(char *);
extern int B_SysCopy(char *);
extern int B_SysDelete(char *);

/* b_fedit.c */
extern int B_FileEdit(char *);

/* b_rep.c */
extern int B_ReportLine(char *);
extern int B_ReportExec(char *);
extern int B_ReportExec_1(char *);
extern int RP_chk_ignore(char *);
extern int RP_splitline(char *,char *,char **,int );
extern int RP_find_fn(char *,int *,int );
extern int RP_exec_fn(char *,char *,int );
extern int RP_err_dump(char *,char *);
extern int RP_onerror_1(char *);
extern int RP_onerror(char *);
extern int RP_label(char *);
extern int RP_abort(char *);
extern int RP_quitode(char *);
extern int RP_return(char *);
extern int RP_goto(char *);
extern int RP_goto_label(char *, char *);
extern int RP_ask(char *);
extern int RP_message(char *);
extern int RP_warning(char *);
extern int RP_beep(void);
extern int RP_setdebug(char *);
extern int RP_system(char *);
extern int RP_define(char *);
extern int RP_shift(char *);
//extern int RP_readline(REPFILE *,char **,int );
extern int RP_add(char **,int *,int *,char *);
extern int RP_expand(char **,char *);
extern char *RP_extract(char *,int *,int );
extern char *RP_gmacro(char *);
extern char *RP_gcmd(char *);
extern double RP_evallec(char *);
extern int RP_eval(char **,char *);
extern int RP_settime(char *);
extern int RP_incrtime(char *);
extern int RP_evaltime(void);
extern int RP_fmt(char *,char *,double );
extern unsigned char **SCR_vtomsq(char *,char *,int );
extern int B_ReportPrompt(char *);
extern int B_Sleep(char *);

/* b_rep2.c */
extern unsigned char *RPF_replace(unsigned char **);
extern unsigned char *RPF_date(unsigned char **);
extern unsigned char *RPF_time(unsigned char **);
extern unsigned char *RPF_upper(unsigned char **);
extern unsigned char *RPF_lower(unsigned char **);
extern unsigned char *RPF_fappend(unsigned char **);
extern unsigned char *RPF_fdelete(unsigned char **);
extern unsigned char *RPF_ttitle(unsigned char **);
extern int RPF_vsliste1(CLEC *,unsigned char ***,int *,int );
extern unsigned char *RPF_vsliste(unsigned char **,int );
extern unsigned char **RPF_unique(unsigned char **);
extern unsigned char *RPF_vliste(unsigned char **);
extern unsigned char *RPF_sliste(unsigned char **);
extern unsigned char *RPF_expand(unsigned char **,int );
extern unsigned char *RPF_cexpand(unsigned char **);
extern unsigned char *RPF_eexpand(unsigned char **);
extern unsigned char *RPF_iexpand(unsigned char **);
extern unsigned char *RPF_lexpand(unsigned char **);
extern unsigned char *RPF_sexpand(unsigned char **);
extern unsigned char *RPF_texpand(unsigned char **);
extern unsigned char *RPF_vexpand(unsigned char **);
extern char *RP_extractpar(char *,int *,char *);
extern unsigned char *RP_gfn(unsigned char *);
extern int RP_fneval(char **,char *);

/* b_data.c */
extern int B_DataCalcVar(char *);
extern int B_DataCreate_1(char *,int *);
extern int B_DataCreate(char *,int );
extern int B_DataDelete_1(char *,int *);
extern int B_DataDelete(char *,int );
extern int B_DataRename(char *,int );
extern int B_DataDuplicate(char *,int );
extern int B_DataUpdateEqs(char *,char *,char *,int ,SAMPLE *,char *,char *,float *,int );
extern int B_DataUpdateTbl(char *,char *);
extern int B_DataUpdate(char *,int );
extern int B_DataSearch(char *,int );
extern int B_DataEditCnf(char *);
extern int my_strcmp(const void *,const void *);
extern int B_DataListSort(char *);
extern int B_DataDisplayGraph(char *);
extern int B_DataPrintGraph(char *);
extern int B_DataEditGraph(int ,char *);
extern int B_DataScan(char *,int );
extern int B_DataExist(char *,int );
extern int B_DataAppend(char *,int );
extern int B_DataList(char *,int );
extern int B_DataCalcLst(char *);
extern int B_DataListCount(char *);
extern int B_DataCompare(char *,int );

/* b_ws.c */
extern int B_WsLoad(char *,int );
extern int X_findtype(char *);
extern int B_WsDump(KDB *,char *);
extern int B_WsSave(char *,int );
extern int B_WsExport(char *,int );
extern int B_WsImport(char *,int );
extern int B_WsSample(char *);
extern int B_WsClear(char *,int );
extern int B_WsDescr(char *,int );
extern int B_WsName(char *,int );
extern int B_WsCopy(char *,int );
extern int B_WsMerge(char *,int );
extern int B_WsExtrapolate(char *);
extern int B_WsAggrChar(char *);
extern int B_WsAggrSum(char *);
extern int B_WsAggrProd(char *);
extern int B_WsAggrMean(char *);
extern int B_WsAggr(int ,char *);
extern double *B_StatUnitRoot_1(char *,int );
extern int B_StatUnitRoot(char *);

/* b_prof.c */
extern int B_ProfileLoad(char *);
extern int B_ProfileSave(char *);
extern int B_ProfileReset(void);

/* b_readme.c */
extern int B_ReadMe(void);

/* b_pdest.c */
extern int B_PrintDestFile(char *,int );
extern int B_PrintDest(char *);
extern int B_PrintDestNew(char *);
extern int B_PrintDestExt(char *,int ,int );
extern int B_PrintNbDec(char *);
extern int B_PrintLang(char *);
extern int B_PrintA2mAppend(char *);
extern int B_PrintTBreak(char *);
extern int B_PrintTPage(char *);
extern int B_PrintGPage(char *);
extern int B_PrintGTheme(char *);
extern int B_PrintParaNum(char *);
extern int B_PrintPageHeader(char *);
extern int B_PrintPageFooter(char *);
extern int B_PrintFont(char *);
extern int B_PrintTFont(char *);
extern int B_PrintTBox(char *);
extern int B_PrintTColor(char *);
extern int B_PrintTWidth(char *);
extern int B_PrintGSize(char *);
extern int B_PrintGBox(char *);
extern int B_PrintGBrush(char *);
extern int B_GetColor(char *);
extern int B_PrintGColor(char *);
extern int B_PrintRtfHelp(char *);
extern int B_PrintRtfTitle(char *);
extern int B_PrintRtfCopy(char *);
extern int B_PrintRtfLevel(char *);
extern int B_PrintRtfTopic(char *);
extern int B_PrintGdiOrient(char *);
extern int B_PrintGdiDuplex(char *);
extern int B_PrintGdiPrinter(char *);
extern int B_PrintGIFBackColor(char *);
extern int B_PrintGIFTransColor(char *);
extern int B_PrintGIFInterlaced(char *);
extern int B_PrintGIFTransparent(char *);
extern int B_PrintGIFFilled(char *);
extern int B_PrintGIFFont(char *);
extern int B_PrintHtmlStrip(char *);
extern int B_A2mToAll(char *,int );
extern int B_A2mToHtml(char *);
extern int B_A2mToRtf(char *);
extern int B_A2mToMif(char *);
extern int B_A2mToCsv(char *);

/* b_model.c */
extern int B_ModelSimulate(char *);
extern int B_ModelSimulateEqs(SAMPLE *,char **);
extern int B_ModelSimulateParms(char *);
extern int B_ModelExchange(char *);
extern int B_ModelCompile(char *);
extern int KE_compile(KDB *);

/* b_print.c */
extern int B_PrintVal(double );
extern double B_calc_ttest(SCL *);
extern int B_replesc(unsigned char *,unsigned char *);
extern int B_PrintDefGnl(char *,char *);
extern int B_isdef(char *);
extern int B_dump_str(unsigned char *,unsigned char *);
extern int B_PrintObjTblTitle(char *);
extern int B_PrintObjLec(char *);
extern int B_PrintObjEqsInfos(char *);
extern int B_PrintObjDef_1(char *,int *);
extern int B_PrintObjDef(char *,int );
extern int B_PrintObjDefArgs(char *,int );
extern int B_PrintDefTbl(KDB *,int );
extern int B_DumpTblDef(TBL *);
extern int B_CellDef(TCELL *);
extern int B_PrintTblCell(TCELL *,int );
extern int B_PrintDefCmt(KDB *,int );
extern int B_PrintDefLst(KDB *,int );
extern int B_PrintDefIdt(KDB *,int );
extern int B_PrintDefEqs(KDB *,int );
extern int B_PrintLec(char *,char *,CLEC *,int );
extern int B_PrintEqs(char *,EQ *);
extern int B_PrintDefSclPtr(SCL *,char *,int );
extern int B_PrintDefScl(KDB *,int );
extern int B_PrintDefVar(KDB *,int );

/* b_view.c */
extern int B_ViewVar(char *);
extern int B_PrintVar(char *);
extern int B_ViewPrintVar(char *,int );
extern int B_ViewByTbl(char *);
extern int B_ViewTbl(char *);
extern int B_PrintTbl(char *);
extern int B_ViewGr(char *);
extern int B_PrintGr(char *);
extern int B_ViewPrintTbl_1(char *,char *);
extern int B_ViewPrintGr_1(char *,char *);
extern int B_ViewPrintTbl(char *,int ,int );
extern int B_ViewTblFile(char *);
extern int B_ViewTblEnd(void);

/* b_global.c */
extern int B_ScrollSet(char *,long *,int ,int );
extern int B_ScrollVarW(char *);
extern int B_ScrollVarN(char *);
extern int B_ScrollVarM(char *);
extern int B_ScrollVarS(char *);
extern int B_ScrollSclW(char *);
extern int B_ScrollSclN(char *);
extern int B_ScrollTblW(char *);
extern int B_ScrollVTW(char *);
extern int B_ScrollVTW0(char *);
extern int B_ScrollVTN(char *);

/* b_idt.c */
extern int B_IdtExecute(char *);
extern int B_IdtExecuteIdts(SAMPLE *,char **);
extern int B_IdtExecuteVarFiles(char *);
extern int B_IdtExecuteSclFiles(char *);
extern int B_IdtExecuteTrace(char *);

/* b_est.c */
extern char **B_EqsSplitSmplName(char *,SAMPLE **);
extern int B_EqsEstimate(char *);
extern int B_EqsEstimateEqs(SAMPLE *,char **);
extern int B_EqsSetSample(char *);
extern int B_EqsSetMethod(char *);
extern int B_EqsSetBloc(char *);
extern int B_EqsSetCmt(char *);
extern int B_EqsSetInstrs(char *);

/* b_htol.c */
extern int B_WsHtoLLast(char *);
extern int B_WsHtoLMean(char *);
extern int B_WsHtoLSum(char *);
extern int HTOL_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *,int *);
extern int B_htol(int ,char *);

/* b_ltoh.c */
extern int B_WsLtoHStock(char *);
extern int B_WsLtoHFlow(char *);
extern int LTOH_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *,int *);
extern int B_ltoh(int ,char *);
extern double LTOH_ylin(double *,double );
extern int LTOH_lin(int ,double *,int ,double *,int ,int );
extern int LTOH_y2cs(double *,int ,double *);
extern double LTOH_ycs(double *,double *,double );
extern int LTOH_cs(int ,double *,int ,double *,int ,int );

/* b_season.c */
extern int B_WsSeasonAdj(char *);
extern int B_season(char *);
extern int DS_test(double *,int ,int *,int *,int ,double *);
extern int DS_vec(double *,double *,double *,double *,int ,int ,double );
extern int DS_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *);
extern int DS_extr(double *,int ,int ,double *,double );

/* b_trend.c */
extern int B_WsTrend(char *);
extern int B_WsTrendStd(char *);
//extern int HP_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *);
//extern int HP_calc(double *,double *,int ,int );	
//extern int HP_calc(double *,double *,int , IODE_REAL);     // JMP 7-3-2019
//extern int HP_calc(double *,double *,int , IODE_REAL, int);  // JMP 12-4-2019
//extern void HP_test(double *,double *,int ,int *,int *);

/* w_wrt1.c */
extern void W_print_enum(int );
extern void W_print_cmd(int );
extern void W_print_par(int );
extern void W_print_tit(int );

/* sb_file.c */
extern void SB_FileOpen(void);
extern void C_FileOpen(void);
extern void SB_FileNew(void);
extern void C_FileNew(void);
extern void SB_FileSave(void);
extern void C_FileSave(void);
extern int SB_FilePrintSetup(void);
extern void C_FilePrintSetup(void);
extern void C_FilePrintFile(void);
extern void SB_FilePreferences(void);
extern void SB_PrintPreferences(void);
extern void C_FilePreferences(void);

/* sb_rep.c */
extern int SB_ReportEdit(void);
extern int C_ReportEdit(void);
extern int SB_ReportPrint(void);
extern int C_ReportPrint(void);
extern int SB_ReportExec(void);
extern int C_ReportExec(void);
extern int SB_ReportEditOutput(void);
extern int SB_ReportLine(void);
extern int C_ReportLine(void);
extern int SB_ReportPrompt(char *,int );
extern int C_ReportPrompt(void);

/* sb_pdest.c */
extern int C_PrintDestDecLang(int ,int ,int );
extern void B_A2mGetGnlParms(void);
extern void B_A2mGetRtfParms(void);
extern int B_A2mSetRtfTitle(unsigned char *);
extern int B_A2mSetRtfCopy(unsigned char *);
extern void B_A2mGetCsvParms(void);
extern void B_A2mGetHtmlParms(void);
extern void B_A2mGetGdiParms(void);
extern void B_A2mGetMifParms(void);
extern void B_A2mSetCol(int *,int );
extern void B_A2mGetGIFParms(void);

/* sb_print.c */
extern int SB_PrintObjDef(void);
extern int C_PrintObjDef(void);
extern int SB_A2MPrint(void);
extern int C_A2MPrint(void);

/* sb_view.c */
extern int SB_ViewByTbl(void);
extern int C_ViewByTbl(void);
extern int SB_ViewPrintTbl(void);
extern int SB_ViewPrintGr(void);
extern int SB_ViewPrintVar(void);
extern char *C_ViewTblArgs(char *,char *);
extern int C_ViewTblFiles(char *,char *,char *,char *);
extern int C_ViewPrintTbl(int );
extern int C_ViewPrintVar(int );
extern int C_ViewPrintGr(int );
extern int C_ViewPrintGrTblVar(int ,int ,int ,char *,char *,char *,char *,char *,char *,int ,int (*)(),int (*)());

/* sb_model.c */
extern int SB_ModelSimulate(void);
extern int C_ModelSimulate(void);
extern int C_ModelSimulateCnf(void);
extern int SB_ModelCompile(void);
extern int C_ModelCompile(void);

/* sb_idt.c */
extern int SB_IdtExecute(void);
extern int C_IdtExecute(void);

/* sb_est.c */
extern int SB_EqsEstimate(void);

/* sb_data.c */
extern int SB_DataSearch(void);
extern void C_DataSearch(void);
extern int SB_DataDuplicate(void);
extern int C_DataDuplicate(void);
extern int SB_DataEdit(char *,int );
extern int SB_DataEditScroll(char *,int );
extern int SB_DataEditObj1(char *,int );
extern int SB_DataEditCil1(char *,int );
extern int C_DataEditCil1(void);
extern int SB_DataEditEqs1(char *);
extern int C_DataEditEqs1(void);
extern int SB_DataListSort(void);
extern int C_DataListSort(void);
extern int SB_DataEditGraph(void);
extern int C_AutoEditGraph(char *,int );
extern int C_DataEditGraph(int );
extern int SB_DataScan(void);
extern int C_DataScan(void);
extern int SB_DataList(void);
extern void C_DataList(void);
extern int SB_DataCalcLst(void);
extern void C_DataCalcLst(void);
extern int SB_DataCompare(void);
extern int C_DataCompare(void);

/* sb_ws.c */
extern int SB_WsLoad(void);
extern int C_WsLoad(int );
extern int SB_WsCopy(void);
extern int C_WsCopy(void);
extern int SB_WsMerge(void);
extern int C_WsMerge(void);
extern int SB_WsSave(void);
extern int C_InitWsSave(void);
extern int C_WsSave(void);
extern int SB_WsDescr(void);
extern int C_InitWsDescr(void);
extern int C_WsDescr(void);
extern int SB_WsClear(void);
extern int C_InitWsClear(void);
extern int C_WsClear(void);
extern int SB_WsSample(void);
extern int C_InitWsSample(void);
extern int C_WsSample(void);
extern int SB_WsExtrapolate(void);
extern int C_WsExtrapolate(void);
extern int SB_WsHtoL(void);
extern int C_WsHtoL(void);
extern int SB_WsLtoH(void);
extern int C_WsLtoH(void);
extern int SB_WsSeasonAdj(void);
extern int C_WsSeasonAdj(void);
extern int SB_ResetSample(void);
extern int SB_ResetFromTo(char *,char *);
extern int SB_WsTrend(void);
extern int C_WsTrend(void);
extern int SB_WsAggregate(void);
extern int SB_StatUnitRoot(void);
extern int C_StatUnitRoot(void);
//extern SAMPLE *K_ask_smpl(void); => kasksmpl()

/* sb_prof.c */
extern int SB_ProfileLoad(void);
extern int C_ProfileLoad(void);
extern int SB_ProfileSave(void);
extern int C_ProfileSave(void);
extern int SB_ProfileReset(void);

/* sb_wrap.c */

/* k_sim_main.c */
extern int K_simul(KDB *,KDB *,KDB *,SAMPLE *,char **, char **);
extern int K_simul_1(int );
extern void K_simul_free();
//extern void K_init_values(int );
//extern void K_restore_XK(int );
extern double K_calc_clec(int ,int ,int ,int);
//extern void K_prolog(int );
//extern int K_interdep(int );
//extern int K_interdep(int );
//extern void K_epilog(int );
extern void K_lstorder(char *,char *,char *);
// extern int K_diverge(int ,char *,double );

/* k_sim_order.c */
extern void KE_order(KDB *, char **);
//extern int KE_preorder(KDB *,int **,int **);
extern int KE_poseq(int );
//extern int KE_add_post(int **,int ,int );
//extern int KE_postorder(KDB *,int **,int **);
//extern int KE_pre(KDB *,int **,int );
//extern int KE_interdep(KDB *,int **);
extern void KE_tri(KDB *,int **,int );
//extern int KE_tri_perm1(KDB *,int ,int *);
//extern int KE_tri_begin(KDB *);
//extern int KE_tri_end(KDB *);

/* k_sim_exo2endo.c */
extern int KE_exo2endo(int ,int );
//extern int KE_findpath(int ,int ,int *);

/* k_sim_scc.c */
extern int KE_ModelCalcSCC(KDB* dbe, int tris, char* pre, char* inter, char* post);
//extern int K_simul_SCC_init(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl);
extern int K_simul_SCC(KDB* dbe, KDB* dbv, KDB* dbs, SAMPLE* smpl, char** pre, char** inter, char** post);

/* k_exec.c */
extern KDB *KI_series_list(KDB *);
extern KDB *KI_scalar_list(KDB *);
extern KDB *KI_exec(KDB *,KDB *,int ,char **,KDB *,int ,char **,SAMPLE *);
extern int *KI_reorder(KDB *);
extern int KI_read_vars_db(KDB *,KDB *);
extern int KI_read_vars_file(KDB *,char *);
extern int KI_read_vars(KDB *,KDB *,KDB *,int ,char **);
extern int KI_read_scls_db(KDB *,KDB *);
extern int KI_read_scls_file(KDB *,char *);
extern int KI_read_scls(KDB *,KDB *,int ,char **);
extern int KI_execute(KDB *,KDB *,KDB *,int *,SAMPLE *);
extern int KI_extract(KDB *,KDB *);

/* k_print.c */
extern int T_prep_cls(TBL *,char *,COLS **);
extern int T_print_tbl(TBL *,char *);
extern int T_print_line(TBL *,int ,COLS *);
extern int T_print_cell(TCELL *,COL *,int );
extern int T_fmt_val(char *,double ,int ,int );
extern int T_print_val(double );
extern int T_print_string(COL *,char *);
extern int T_align(int ,int ,int );
extern int T_open_attr(int );
extern int T_close_attr(int );
extern char **T_find_files(COLS *);
extern int T_print_files(COLS *,int );
extern int T_print_mode(COLS *,int );
extern int T_print_date(int );
extern int T_begin_tbl(int ,COLS *);
extern int T_end_tbl(void);

/* k_graph.c */
/*
extern int T_GraphTest(TBL *);
extern int T_GraphInit(double ,double ,int ,int ,double ,double ,double ,double ,int ,int ,int );
extern int T_GraphEnd(void);
extern int T_graph_tbl_1(TBL *,char *);
extern int T_GraphTitle(char *);
extern int T_GraphLineTitle(TLINE *,COLS *,int );
extern int T_GraphLegend(int ,int ,char *,char *);
extern int T_GraphLineData(int ,double *,double *);
extern int T_GraphTimeData(SAMPLE *,double *);
extern int T_GraphLine(TBL *,int ,COLS *,SAMPLE *,double *,double *,COLS *);
extern int T_prep_smpl(COLS *,COLS **,SAMPLE *);
extern int T_find_opf(COLS *,COL *);
*/

/* New auto extraction 1/10/2021 */
extern int APIChartAlloc(int nl);
extern int APIChartAxis(int hdl, int i);
extern double *APIChartData(int hdl, int i);
extern int APIChartFree(int hdl);
extern int APIChartNc(int hdl);
extern int APIChartNl(int hdl);
extern char *APIChartTitle(int hdl, int i);
extern int APIChartType(int hdl, int i);
extern int APIGraphLegendTitle(int hdl, int axis, int type, char *txt, char *fileop);
extern int APIGraphLine(int hdl, TBL *tbl, int i, COLS *cls, SAMPLE *smpl, IODE_REAL *x, IODE_REAL *y, COLS *fcls);
extern int APIGraphLineTitle(int hdl, TLINE *line, COLS *fcls, int i);
extern int APIGraphTimeData(int hdl, SAMPLE *smpl, IODE_REAL *y);
extern int APIGraphTitle(int hdl, char *txt, IODE_REAL *x, int nb);
extern int APIPrepareChart(TBL *tbl, char *gsmpl);
extern int T_GraphEnd();
extern int T_GraphInit(double w, double h, int xgrid, int ygrid, double ymin, double ymax, double zmin, double zmax, int align, int box, int brush);
extern int T_GraphLegend(int axis, int type, char *txt, char *fileop);
extern int T_GraphLine(TBL *tbl, int i, COLS *cls, SAMPLE *smpl, IODE_REAL *x, IODE_REAL *y, COLS *fcls);
extern int T_GraphLineData(int nb, IODE_REAL *x, IODE_REAL *y);
extern int T_GraphLineTitle(TLINE *line, COLS *fcls, int i);
extern int T_GraphTest(TBL *tbl);
extern int T_GraphTimeData(SAMPLE *smpl, IODE_REAL *y);
extern int T_GraphTitle(char *txt);
extern int T_GraphXYData(int nb, IODE_REAL *x, IODE_REAL *y);
extern int T_GraphXYLegend(int axis, int type, char *txt, char *fileop);
extern int T_find_opf(COLS *fcls, COL *cl);
extern int T_graph_tbl_1(TBL *tbl, char *gsmpl);
extern int T_prep_smpl(COLS *cls, COLS **fcls, SAMPLE *smpl);


/* k_grep.c */
extern char **K_grep(KDB *,char *,int ,int ,int ,int ,int );
extern char *K_expand(int ,char *,char *,int );
extern int K_aggr(char *,char *,char *);

/* o_edit.c */
extern int ODE_edit_obj1(KDB *,int );
extern int ODE_edit_scl(KDB *,int ,int );
extern int ODE_edit_tbl(KDB *,int ,int );
extern int ODE_edit_rec_scl_var(void);
extern int ODE_set_pos(PAGE *);
extern int ODE_edit_var_cell(KDB *,int ,int );
extern int ODE_edit_scl_cell(KDB *,int ,int );
extern int ODE_edit_val(double *,int );

/* o_misc.c */
extern int ODE_ask(int );
extern int ODE_edit_window(PAGE *);

/* o_scroll.c */
extern int VAR_deselect(void);
extern int VAR_select(int ,int );
extern int VAR_sel_order(int *,int *,int *,int *);
extern int VAR_is_sel(int ,int );
extern char *ODE_t(SCROLL *);
extern char *ODE_ct(SCROLL *,int );
extern char *ODE_lt(SCROLL *,int );
extern unsigned char *T_get_title(TBL *);
extern char *ODE_text(SCROLL *,int ,int );
extern int ODE_nl(SCROLL *);
extern int ODE_nc(SCROLL *);
extern int ODE_len(SCROLL *,int );
extern int ODE_del(SCROLL *,int ,int );
extern int ODE_ins(SCROLL *,int ,int );
extern int ODE_ed(SCROLL *,int ,int );
extern int ODE_adj_swidth(int );
extern int ODE_adj_sndec(int );
extern int ODE_adj_vndec(int );
extern int ODE_adj_vwidth(SCROLL *,int );
extern int ODE_adj_title(SCROLL *,int );
extern int ODE_attr(SCROLL *,int ,int );
extern int ODE_ffn(SCROLL *,int ,int ,int );
extern int ODE_bfn(SCROLL *,int *,int *);
extern int ODE_efn(SCROLL *,int ,int );
extern int OSCRL_init(void);
extern int ODE_scroll(KDB *,char **);
extern int ODE_settext(KDB *,int ,int );
extern int ODE_importtext(KDB *,int ,int );
extern int ODE_ClipAddText(KDB *,int ,int ,unsigned char ***,int *);
extern unsigned char *ODE_ClipText(KDB *);
extern int ODE_ClipCopy(KDB *);

/* o_scr.c */
extern int ODE_doc(KDB *,int ,int );
extern int ODE_tins(int *,int *);
extern int ODE_status(void);
extern int SB_status(void);
extern char **A_clec(char *,CLEC *,int );

/* o_tab.c */
extern int TAB_deselect(void);
extern int TAB_select(int );
extern int TAB_sel_order(int *,int *);
extern int TAB_mark_ins(int ,int );
extern int TAB_mark_del(int );
extern int TAB_is_sel(int );
extern int TAB_copy_lines(TBL *,int ,int ,TBL *,int );
extern int TAB_buffer(TBL *);
extern int TAB_paste(TBL *,int );
extern int TAB_delete(TBL *);
extern int TAB_copy(TBL *,int );
extern char *T_get_div(TBL *,int );
extern int T_mmt_left(void);
extern int T_mmt_rec(void);
extern char *TAB_t(SCROLL *);
extern char *TAB_ct(SCROLL *,int );
extern char *TAB_lt(SCROLL *,int );
extern int TAB_align_cell(unsigned char *,TCELL *,int ,int );
extern int TAB_cattr(SCROLL *,int ,int );
extern int TAB_align(SCROLL *,int ,int );
extern int T_getcellalign(TBL *,int ,int );
extern int T_getcellfont(TBL *,int ,int );
extern int T_getfont(TBL *,int ,int ,int );
extern char *TAB_text(SCROLL *,int ,int );
extern int TAB_nl(SCROLL *);
extern int TAB_nc(SCROLL *);
extern int TAB_len(SCROLL *,int );
extern int TAB_del(SCROLL *,int ,int );
extern int TAB_del_qu(TBL *,int ,int );
extern int TAB_ins(SCROLL *,int ,int );
extern int TAB_ed(SCROLL *,int ,int );
extern int TAB_disp(SCROLL *,int ,int );
extern int T_adj_width(SCROLL *,int ,int );
extern int TAB_ffn(SCROLL *,int ,int ,int );
extern int TAB_end(void);
extern int TSCRL_init(void);
extern int TAB_scroll(TBL *,char *);
extern int T_ins_line(TBL *,int );
extern int T_del_line(TBL *,int );
extern int T_ed_getcell(SCROLL *,int ,int ,TCELL **,int *);
extern char *T_ed_getcelltxt(SCROLL *,int ,int );
extern int T_ed_savecell(SCROLL *,int ,int ,char *);
extern int T_ed_line(SCROLL *,int ,int );
extern int T_attr_tab(TBL *,int );
extern int T_ed_attr(TBL *,int ,int );
extern int T_attr_cell(TBL *,int ,int );
extern int T_attr_msg(TBL *,int ,int );

/* o_vt.c */
extern char *VT_t(SCROLL *);
extern char *VT_ct(SCROLL *,int );
extern int VT_cattr(SCROLL *,int ,int );
extern int VT_ltattr(SCROLL *,int );
extern char *VT_text(SCROLL *,int ,int );
extern char *VT_lt(SCROLL *,int );
extern int VT_nl(SCROLL *);
extern int VT_nc(SCROLL *);
extern int VT_len(SCROLL *,int );
extern int VT_adj_width(int ,int );
extern int VT_adj_ndec(int );
extern int VT_ffn(SCROLL *,int ,int ,int );
extern int VT_calc(char *,char *,char *,double *,int ,int );
extern int VT_edit(SCROLL *,int ,int );
extern int VT_init(void);
extern int VT_scroll(TBL *,char *);
extern int T_tbl_test(TBL *,char *);
extern int T_alloc_val(TBL *,char *);
extern int T_free_val(TBL *);
extern int T_name_inline(TCELL *,int ,char *);
extern int T_calc_val(TBL *,char *);
extern int T_view_tbl(TBL *,char *,char *);
extern int T_calc_line(TBL *,int ,COLS *,int *);
extern int VT_files(void);

/* o_est.c */
extern int ODE_blk_begin(void);
extern int ODE_blk_fn(void);
extern int ODE_blk_name_fn(void);
extern int ODE_blk_end_fn(void);
extern int ODE_blk_next_fn(void);
extern int ODE_blk_coef_fn(void);
extern int ODE_blk_est_fn(void);
extern int ODE_blk_save_fn(void);
extern int ODE_blk_res_fn(void);
extern int ODE_blk_init(void);
extern int ODE_blk_check(void);
extern int ODE_blk_cur(void);
extern int ODE_blk_cur_tests(void);
extern int ODE_blk_cur_atests(void);
extern int ODE_blk_save_1(char *,char *,char *,float *);
extern int ODE_blk_save_cur(void);
extern int ODE_blk_coef(void);
extern int ODE_blk_save(void);
extern int ODE_blk_instr(void);
extern int ODE_blk_est(void);
extern int ODE_blk_unitroot(void);
extern int ODE_blk_gr_print(void);
extern int ODE_blk_print_res(void);
extern int ODE_blk_dadj_fn(char *);

/* o_gr.c */
extern int o_calc_lhs(char *,char *,char *);
extern int o_estgr(char **,SAMPLE *,MAT *,MAT *,int ,int );
extern int ODE_graph(int ,int ,int ,int ,int ,int ,double ,double ,SAMPLE *,char **);
extern void T_create_wtcfl(void);
extern int o_var_gr_1(int ,int ,int ,int ,int ,double ,double ,SAMPLE *,int ,int ,KDB *,char *);
extern int o_var_gr(int ,int ,int ,int ,int ,double ,double ,SAMPLE *,KDB *,char **);

/* o_abort.c */
// extern int SW_abort(int ,char *);   	// JMP 10/5/2021 double de avec SCR
// extern int SCR_panic(void);			// JMP 10/5/2021 double de avec SCR

/* b_dde.c (MSC) */
extern int __cdecl IodeDdeLocale(char *buf);
extern int __cdecl IodeDdeUnLocale(char *buf);
extern int __cdecl IodeDdeType(char *szTopic);
extern char *__cdecl IodeDdeGetWS(char *szItem);
extern char *__cdecl IodeDdeCreateSeries(int objnb,int bt);
extern char *__cdecl IodeDdeCreatePer(int bt);
extern char *__cdecl IodeDdeXlsCell(char *offset,int i,int j,int lg,int hg);
extern char *__cdecl IodeTblCell(struct _tcell_ *cell,struct _col_ *cl, int nbdec);
extern char *__cdecl IodeDdeCreateTbl(int objnb,char *ismpl,int *nc,int *nl,int nbdec);
extern char *__cdecl IodeDdeCreateObj(int objnb,int type,int *nc,int *nl);
extern char *__cdecl IodeDdeGetXObj(char *szItem,int type);
extern char *__cdecl IodeDdeGetItem(char *szTopic,char *szItem);
extern int __cdecl IodeDdeSetWS(char *szItem,char *szBuffer);
extern int __cdecl IodeDdePlay(char *szItem,char *szBuffer);
extern int __cdecl DdeTsfKey(char *key);
extern int __cdecl IodeDdeSetItem(char *szTopic,char *szItem,char *szBuffer);
extern char *__cdecl B_ExcelGetItem(char *arg);
extern int __cdecl B_ExcelSetItem(char *ddeitem,char *ptr,int nc,int nl);
extern int __cdecl B_ExcelGet(char *arg,int type);
extern int __cdecl B_ExcelSet(char *arg,int type);
extern int __cdecl B_ExcelExecute(char *arg);
extern int __cdecl B_ExcelCmd(char *cmd,char *arg);
extern int __cdecl B_ExcelOpen(char *arg);
extern int __cdecl B_ExcelClose(char *arg);
extern int __cdecl B_ExcelPrint(char *arg);
extern int __cdecl B_ExcelSave(char *arg);
extern int __cdecl B_ExcelSaveAs(char *arg);
extern int __cdecl B_ExcelNew(char *arg);
extern int __cdecl B_DDEGet(char *arg);
extern int __cdecl IodeFmtVal(char *buf,double val);

/* b_ds.c (MSC) */
extern char *__cdecl B_DSPeriod2Date(struct _period *per,char *date,char *freq);
extern int __cdecl B_DSLog(char *log);
extern int __cdecl B_DSTimedOut(char *name);
extern int __cdecl B_DSDate2Period(struct _period *per,char *date,char freq);
extern int __cdecl B_DSCode(char *name);
extern int __cdecl B_DSUpdateCmt(char *name,char *val);
extern int __cdecl B_DSUpdateVar(char *name,struct _period *per,char *val);
extern int __cdecl B_DSInterpret(char *code,char freq,char *contents);
extern int __cdecl B_DSImportDb_1(char *arg,struct _sample *smpl);
extern int __cdecl B_DSImportDb(char *arg);

/* b_sql.c */
extern KDB  *K_load_odbc(int , char *, int , char **);

/* b_win.c (MSC) */
extern int __cdecl B_WindowMinimize();
extern int __cdecl B_WindowMaximize();
extern int __cdecl B_exec_system(char *arg);
extern int __cdecl B_shellexec(char *arg);

/* w_wrt.c (MSC) */
extern int __cdecl W_dest(char *filename,int type);
extern int __cdecl W_InitParms();
extern int __cdecl W_open();
extern int __cdecl W_close();
extern int __cdecl W_flush();
extern int __cdecl W_putc(int ch);
extern int __cdecl W_isempty(char *buf);
extern int __cdecl W_record(char *str);
extern int __cdecl W_InitDisplay();
extern int __cdecl W_EndDisplay(char *title,int x,int y,int w,int h);
extern int __cdecl W_SavePrinterSettings();
extern int __cdecl W_ResetPrinterSettings();
extern int __cdecl W_SetPrinterSettings();

// interface
extern U_ch  *T_mmt_edit(U_ch *, int , int , int , int );

/* New files 2021 */




#ifdef SCRCPP
}
#endif
#endif /* _IODEBASE_ */

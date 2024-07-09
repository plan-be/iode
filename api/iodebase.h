/**
 *  @header4iode
 *  
 *  IODEBASE.H
 *  ----------
 *  Contains 
 *      - declarations of IODE functions source file by source file
 *      - declarations of IODE pointer to "super" functions
 *  
 *  This file is included in iode.h.
 */
 
#ifndef _IODEBASE_
#define _IODEBASE_

#include <stdarg.h> // JMP 06/12/2022 for va_list

#ifdef SCRCPP
extern "C" {
#endif

/* k_ws.c */
extern void K_init_ws(int );
extern void K_end_ws(int );
extern int K_load_RWS(int ref, char *filename);


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
extern TBL *K_tunpack(char *);
extern EQ *K_eunpack(char *, char *);
extern IDT* K_iunpack(char *);
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
extern void    (*kerror_super)(const int level, const char* fmt);
extern void    (*kwarning_super)(const char* msg);
extern void    (*kpause_super)();
extern void    (*kmsg_super)(const char* fmt);
extern int     (*kwprintf_super)(const char* msg);
extern void    (*kpanic_super)();
extern int     (*kconfirm_super)(const char* msg);
extern int     (*kmsgbox_super)(const unsigned char* str, const unsigned char* v, const unsigned char** buts);
extern int     kmsgbox_continue;
extern int     kpause_continue;
extern void    (*krecordkey_super)(const int ch);
extern void    (*krecordtext_super)(const unsigned char* text);
extern void    (*ksettitle_super)(void);
extern int     (*ktermvkey_super)(const int vkey);
extern int     (*khitkey_super)();
extern int     (*kgetkey_super)();
extern void    (*kbeep_super)(void);
extern SAMPLE* (*kasksmpl_super)(void);
extern int     (*kexecsystem_super)(const char*);
extern int     (*kshellexec_super )(const char*);
extern int     (*ODE_end_super)(const int);

extern void    kerror(const int level, const char* fmt, ...);
extern void    kwarning(const char* fmt, ...);
extern void    kpause();
extern void    kmsg(const char* fmt, ...);
extern void    kmsg_toggle(const int IsOn);
extern int     kwprintf(const char* msg, ...);
extern void    kpanic(void);
extern int     kconfirm(const char* fmt, ...);
extern int     kmsgbox(const unsigned char *str, const unsigned char *v, const unsigned char **buts);
extern void    krecordkey(const int ch);
extern void    krecordtext(const unsigned char* text);
extern void    ksettitle(void);
extern int     ktermvkey(const int vkey);
extern int     khitkey();
extern int     kgetkey();
extern void    kbeep();
extern SAMPLE* kasksmpl();
extern int     kexecsystem(const char*);
extern int     kshellexec(const char*);
extern char    *A_expand_super_API(const char* name);
extern int     ODE_end(const int st);
extern void    IODE_assign_super_API(void);

/* k_cmp.c */
extern int K_cmp(char *,KDB *,KDB *);

/* k_lec.c */
extern char *(*L_expand_super)(char* list_name);

extern double *L_getvar(KDB *,int );
extern double L_getscl(KDB *,int );
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
extern void K_sort(KDB* kdb);
extern KDB *K_init_kdb(int ,char *);
extern char *K_get_kdb_nameptr(KDB *kdb);
extern void K_set_kdb_name(KDB *kdb, U_ch *filename);
extern void K_set_kdb_fullpath(KDB *kdb, U_ch *filename);
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
extern int K_upd_eqs(char* name, char* lec, char* cmt, int method, SAMPLE* smpl, char* instr, char* blk, float* tests, int date);
extern int K_upd_tbl(char* name, char* arg);

/* k_objsv.c */
extern int K_add(KDB *kdb, char* name,...);

/* k_objvers.c */
extern int K_calcvers(char *);
extern void K_setvers(KDB* kdb, int i, int vers);

/* k_objfile.c */
extern char *K_add_ext(char* filename, char* ext);
extern int K_get_ext(char* filename, char* ext, int max_ext_len);
extern int K_has_ext(char* filename);
extern char *K_set_ext(char *,char *,int );
extern char *K_set_ext_asc(char *,char *,int );
extern void K_strip(char *);
extern int K_merge(KDB *,KDB *,int );
extern int K_merge_del(KDB *,KDB *,int );
extern KDB *K_load(int ,char *,int ,char **);
extern KDB *K_interpret(int ,char *);
extern int K_copy(KDB *,int ,char **,int ,char **,SAMPLE *);
extern int K_cat(KDB *,char *);
extern int K_set_backup_on_save(int take_backup);
extern int K_get_backup_on_save();
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
extern double PER_per2real(PERIOD *,int );
extern char *PER_real2a(double ,int );

/* yy.c */
extern double K_read_real(YYFILE *);
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
extern int L_nb_tokens(void);
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
extern L_REAL* L_cc_link_exec(char* lec, KDB* dbv, KDB* dbs);
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
extern int HP_calc(double *f_vec, double *t_vec, int nb, double lambda, int std);
extern void HP_test(double *f_vec, double *t_vec, int nb, int *beg, int *dim);

/* l_eqs.c */
extern CLEC *L_solve(char *,char *);
extern int L_split_eq(char *);

/* l_newton.c */
extern double L_zero(KDB *,KDB *,CLEC *,int ,int ,int );
extern double L_newton(KDB *,KDB *,CLEC *,int ,int ,int );
//extern double L_newton_1(int ,KDB *,KDB *,CLEC *,int ,int ,int );

/* l_secant.c */
//extern double L_fx(double ,int );
//extern int L_bracket(double *,double *,int );
extern double L_secant(KDB *,KDB *,CLEC *,int ,int ,int );

/* l_debug.c */
extern void L_debug(char *,...);


/* e_est.c */
extern int E_est(char **,char **,KDB *,KDB *,SAMPLE *,int ,char **,int ,double );
//extern int E_gls(void);
//extern int E_residuals(void);
//extern double E_rhs_ij(int ,int );
//extern int E_c_rhs(void);
//extern int E_jacobian(void);
extern int E_scl_in_eq(int ,int );
//extern int E_mod_residuals(int ,int ,double );
//extern void E_c_gmg(void);
//extern int E_c_gmu(void);
//extern int E_deltac(void);
//extern int E_testcv(void);
//extern int E_adaptcoef(void);
//extern int E_c_vcu(void);
//extern int E_c_ivcu(void);
//extern int E_c_mcu(void);
//extern int E_c_vcc(void);
//extern int E_c_ivcc(void);

/* e_step.c */
extern double C_evallec(char *,int);
extern double E_StepWise(SAMPLE* smpl, char* eqname, char* cond, char* test);

/* e_tests.c */
extern double M_c_line(MAT *,int ,int );
extern double E_div_0(double ,double );
extern double E_sqrt(double val);
extern int E_output(void);
//extern void E_deg_freed(void);
//extern double E_c_umu(void);
//extern int E_c_loglik(void);
//extern int E_c_mcorr(void);
//extern int E_c_mcorru(void);
//extern int E_c_ttests(void);

/* e_prep.c */
extern int E_prep(char **,char **);
//extern int E_prep_lecs(char **);
//extern int E_prep_instrs(char **);
//extern int E_prep_coefs(void);
extern void E_get_C(void);
extern void E_put_C(void);
extern void E_get_SMO(void);
//extern int E_prep_alloc(void);
extern void E_free_work(void);
//extern int E_prep_reset(void);

/* e_error.c */
//extern void E_msg_n(int );
extern void E_error_n(int );
extern void E_msg(char* fmt,...);
extern void E_error(char* fmt,...);

/* e_print.c */
//extern void E_print_enum(void);
//extern void E_BeginTbl(char *,int );
//extern void E_print_parms(void);
//extern void E_print_eqs(void);
//extern void E_print_instrs(void);
//extern void E_print_coefs(void);
//extern void E_print_mcorr(void);
//extern void E_print_mcorru(void);
//extern void E_print_eqres_1(int );
//extern void E_print_eqres_2(int );
//extern void E_print_eqres(int );
extern int E_graph(char** titles, SAMPLE* smpl, MAT* mlhs, MAT* mrhs, int view, int res);
extern int E_print_results(int ,int ,int ,int ,int );

/* e_dftest.c */
extern double *E_UnitRoot(char *,int ,int ,int );
//extern int E_UnitRoot_1(SAMPLE *,char *);
extern void E_SclToReal(char *,double *);
extern void E_PrintDF(char *,double *,int ,int ,int );
extern int E_GetLecName(char *,char *);

/* k_wsvar.c */
extern int KV_sample(KDB *,SAMPLE *);
extern int KV_merge(KDB *,KDB *,int );
extern void KV_merge_del(KDB *,KDB *,int );
extern int KV_add(char* varname);
extern double KV_get(KDB *,int ,int ,int );
extern void KV_set(KDB *,int ,int ,int ,double );
extern int KV_extrapolate(KDB *,int ,SAMPLE *,char **);
extern KDB *KV_aggregate(KDB *,int ,char *,char *);
void KV_init_values_1(double* val, int t, int method);
//extern int KV_GetSmpl(SAMPLE *,char *);
extern double KV_get_at_t(char*varname, int t);
extern double KV_get_at_per(char*varname, PERIOD* per);
extern double KV_get_at_aper(char*varname, char* aper);
extern int KV_set_at_t(char*varname, int t, double val);
extern int KV_set_at_per(char*varname, PERIOD* per, double val);
extern int KV_set_at_aper(char*varname, char* aper, double val);


/* k_val.c */
extern char *K_oval(KDB *,int ,int );
extern char *K_oval0(KDB *,int );
extern char *K_oval1(KDB *,int );
extern double *K_vval(KDB *,int ,int );

extern char *K_optr(KDB *,char* ,int );
extern char *K_optr0(KDB *,char* );
extern char *K_optr1(KDB *,char* );
extern double *K_vptr(KDB *,char* ,int );

extern EQ*  K_eptr(KDB* kdb, char* name);
extern TBL* K_tptr(KDB* kdb, char* name);

// Estimation tests by equation name
extern double K_etest(KDB* kdb, char*name, int test_nb);
extern double K_e_stdev (KDB* kdb, char*name);
extern double K_e_meany (KDB* kdb, char*name);
extern double K_e_ssres (KDB* kdb, char*name);
extern double K_e_stderr(KDB* kdb, char*name);
extern double K_e_fstat (KDB* kdb, char*name);
extern double K_e_r2    (KDB* kdb, char*name);
extern double K_e_r2adj (KDB* kdb, char*name);
extern double K_e_dw    (KDB* kdb, char*name);
extern double K_e_loglik(KDB* kdb, char*name);

// Values of scalars by name
extern double K_s_get_info(KDB* kdb, char*name, int info_nb);
extern double K_s_get_value (KDB* kdb, char*name);
extern double K_s_get_relax (KDB* kdb, char*name);
extern double K_s_get_stderr(KDB* kdb, char*name);
extern double K_s_get_ttest (KDB* kdb, char*name);
extern int K_s_set_info(KDB* kdb, char*name, int info_nb, double val);
extern int K_s_set_value (KDB* kdb, char*name, double val);
extern int K_s_set_relax (KDB* kdb, char*name, double val);
extern int K_s_set_stderr(KDB* kdb, char*name, double val);


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
extern char *T_cell_cont_tbl(TBL *,int, int, int );
extern char *T_div_cont_tbl(TBL *, int, int );
extern int T_insert_line(TBL *,int ,int ,int );
extern int T_set_lec_cell(TCELL *,unsigned char *);
extern int T_set_lec_cell_tbl(TBL *, int, int, unsigned char *);
extern void T_set_string_cell(TCELL *,unsigned char *);
extern void T_set_string_cell_tbl(TBL *, int, int, unsigned char *);
extern void T_set_cell_attr(TBL *,int ,int ,int );
extern int T_default(TBL *,char *,char **,char **,int ,int ,int );
extern void T_auto(TBL *,char *,char **,int ,int ,int );

/* k_est.c */
extern int KE_est_s(KDB *,KDB *,KDB *,SAMPLE *,char **, int);
extern void E_tests2scl(EQ *,int ,int ,int );
extern int KE_estim(char* veqs, char* afrom, char* ato);
//extern void E_savescl(double ,int ,char *);
//extern void E_savevar(char *,int ,MAT *);

/* k_ccvar.c */
extern KDB *KV_load_str(char *);
extern KDB *KV_load_asc(char *);
extern int KV_save_asc(KDB *,char *);
extern int KV_save_csv(KDB *, char *, SAMPLE *, char **);

extern char *KV_CSV_SEP;
extern char *KV_CSV_DEC;
extern char *KV_CSV_NAN;
extern char *KV_CSV_AXES;
extern int  KV_CSV_NBDEC;

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
extern int SB_XodeRuleExport(void);
extern int C_XodeRuleExport(void);


/* k_iprn.c */
extern double PRN_readreal(YYFILE *);
extern int IMP_vec_prn(YYFILE *,char *,int ,double *);
extern int IMP_hd_cprn(IMPDEF *,char *,int );
extern int IMP_vec_cprn(char *,char **);

/* k_rules.c */
extern int IMP_readrule(char *);
extern int IMP_change(char **,char **,char *,char *);

/* k_imain.c */
extern KDB *IMP_InterpretVar(IMPDEF *,char *,char *,SAMPLE *);
extern KDB *IMP_InterpretCmt(IMPDEF *,char *,char *,int );
extern int IMP_RuleImport(int ,char *,char *,char *,char *,char *,char *,int ,int );

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
//extern void EXP_set(char *,char *);
extern void EXP_val(char *,double );
extern char *EXP_addprepost(char *,char *,char *,char **);
extern char *EXP_addsep(char *,char **);
extern int EXP_Ws(EXPDEF *,KDB *,KDB *,char *,char *,char *,char *);
extern int EXP_Rev_Ws(EXPDEF *,KDB *,KDB *,char *,char *,char *,char *);
extern int EXP_RuleExport(char *,char *,char *,char *,char *,char *,char *,char *,char *,int );

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
extern int COL_free_cols(COLS *);
//extern int COL_stripy(char *);
extern char *COL_ctoa(COL *,int ,int ,int );
extern char *COL_text(COL *,char *,int );
extern COLS *COL_add_col(COLS *);
//extern COLS *COL_construct(COLS *,COLS *,FILS *,REP *, int, int);
//extern int COL_apply_fil(COL *,FIL *);
//extern int COL_read_per(YYFILE *,PERIOD *);
//extern COLS *COL_read_y(YYFILE *);
//extern int COL_read_rep(YYFILE *,REP *);
//extern int COL_read_1f(YYFILE *,FIL *);
//extern FILS *COL_read_f(YYFILE *);
//extern COLS *COL_read_cols(YYFILE *);
extern int COL_find_mode(COLS *,int *,int );

/* c_calc.c */
extern int COL_resize(TBL *,COLS *);
extern void COL_clear(COLS *);
//extern CLEC *COL_cp_clec(CLEC *);
extern int COL_exec(TBL *,int ,COLS *);
//extern int COL_calc(COL *,CLEC *,CLEC *);
// extern int COL_link(int ,CLEC *);

/* b_api.c */
#include "iodeapi.h"

/* b_iodeini.c */
extern char* ODE_INIFILE;
extern void B_IodeIniFile();
extern int B_IniReadText(char* section, char* parm, char* res, int maxlen, char* dft);
extern int B_IniWriteText(char* section, char* parm, char* val);
extern int B_IniReadChar(char* section, char* parm, char dft);
extern int B_IniWriteChar(char* section, char* parm, char val);
extern int B_IniReadNum(char* section, char* parm, int dft);
extern int B_IniWriteNum(char* section, char* parm, int val);
extern int B_IniReadYN(char* section, char* parm, int dft);
extern int B_IniWriteYN(char* section, char* parm, int val);


/* b_a2mini.c */
extern void B_A2mGetGnlParms();
extern void B_A2mSaveGnlParms();
extern void B_A2mSetRtfTitle(U_ch* title);
extern void B_A2mSetRtfCopy(U_ch* title);
extern void B_A2mGetRtfParms();
extern void B_A2mSaveRtfParms();
extern void B_A2mGetHtmlParms();
extern void B_A2mSaveHtmlParms();
extern void B_A2mGetGIFParms();
extern void B_A2mSaveGIFParms();
extern void B_A2mGetCsvParms();
extern void B_A2mSaveCsvParms();
extern void B_A2mGetMifParms();
extern void B_A2mSaveMifParms();
extern void B_A2mGetGdiParms();
extern void B_A2mSaveGdiParms();
extern void B_A2mGetAllParms();
extern void B_A2mSaveAllParms();
//extern void K_load_iode_ini();
//extern void K_save_iode_ini();


/* b_args.c */
extern char **B_ainit_chk(char *,ADEF *,int );
extern char **B_vtom_chk(char *,int );
extern int B_loop(char **,int (*)(),char *);
extern int B_ainit_loop(char *,int (*)(),char *);
extern int B_get_arg0(char *,char *,int );
extern int B_argpos(char *,int );

/* b_errors */
extern void B_IodeMsgPath();
extern void B_seterror(char *,...);
extern void B_seterrn(int , ...);

extern void B_add_error(char* msg);
extern char* B_get_last_error(void);
extern void B_display_last_error(void);
extern void B_print_last_error(void);
extern void B_clear_last_error(void);
//extern void B_reset_error(void);
extern char *B_msg(int );
extern int B_get1int(char *);

/* b_dir.c */
//extern int C_dir_edit_fn(char *,char *,SCRSTAT *);
//extern int B_FileList(char *);
//extern int B_FileList2(char *);
//extern void B_Read_Dir(char *);
//extern int B_FileListGnl(char *,int (*)());
//extern void B_path_change(char *);
//extern void B_analyse_file(SCRSTAT *,char **,int *,int *,int *,int *,int *,char *,long *,int *);
//extern int B_dir_ed_prt(char *);

/* b_file.c */
extern int B_FilePrint(char *,int );

/* b_fdel.c */
//extern int B_unlink_1(char *,int *);
extern int B_FileDelete(char *,int );

/* b_fcopy.c */
extern int B_FileCopy(char *,int );

/* b_fren.c */
extern int B_FileRename(char *,int );
extern int B_SysRename(char *);
extern int B_SysCopy(char *);
extern int B_SysDelete(char *);

/* b_fsys.c */
extern int B_SysRename(char* arg);
extern int B_SysCopy(char* arg);
extern int B_SysAppend(char* arg);
extern int B_SysDelete(char* arg);
extern int B_SysOemToUTF8(char *arg);
extern int B_SysAnsiToUTF8(char *arg);
extern int B_SysAnsiToOem(char *arg);
extern int B_SysOemToAnsi(char *arg);

/* b_xode.c */
extern int B_FileImportCmt(char* arg);
extern int B_FileImportVar(char* arg);


/* b_fedit.c */
extern int B_FileEdit(char *);

/* b_eviews.c */
int B_WsImportEviews(char *arg);

/* b_rep_utils.c */
extern int RP_alloc_ptrs();
extern char *RP_alloc(int size);
extern int RP_find_ptr(char *ptr);
extern int RP_free(char *ptr);
extern void RP_free_bufs();
extern char *RP_stracpy(char *ptr);
extern unsigned char **RP_vtoms(unsigned char* str, unsigned char *seps);
extern unsigned char **RP_vtom(unsigned char* str, int sep);
extern int RP_free_tbl(unsigned char **tbl);
extern int RP_tbl_size(unsigned char **tbl);
extern int RP_is_cmd(char *line);
extern U_ch **SCR_vtomsq(char* str, char* seps, int quote);

/* b_rep_debug.c */
extern void RP_debug(char* txt);

/* b_rep_engine.c */
extern REPFILE *RP_create_repfile(char *filename, unsigned char **tbl);
extern int RP_free_repfile(REPFILE *rf);
extern unsigned char **RP_read_file(char* filename);
extern char* RP_read_multiline(REPFILE* rf);
extern int RP_readline(REPFILE* rf, char** line, int mode);
extern int RP_chk_ignore(char* line);
extern int RP_splitline(char* text, char* cmd, char** arg, int lg);
extern int RP_find_fn(char* name, int* type, int fs);
extern int RP_exec_fn(char* name, char* arg, int fs);
extern int RP_err_dump(char* name, char* arg);
extern char *RP_extract(char* buf, int* i, int ch);
extern char *RP_gmacro(char* str);
extern char *RP_gcmd(char* str);
extern int RP_evaltime();
extern double RP_evallec(char* lec);
extern int RP_fmt(char* buf, char* format, double value);
extern int RP_eval(char** res, char* farg);
extern int RP_add(char** line, int* lg, int* j, char* res);
extern int RP_expand(char** line, char* buf);
extern int RP_ReportExec_tbl(REPFILE *rf);
extern int RP_ReportExec_1(char* file);
extern int B_ReportExec(char* arg);
extern int B_ReportLine(char* line);

/* b_rep_defs.c */
extern int RP_macro_createdb();
extern int RP_macro_deletedb();
extern int RP_define_1(char *name, char *macro);
extern int RP_define(char* arg);
extern char* RP_get_macro_ptr(char* macro_name);
extern int RP_undef_1(char *name);
extern int RP_undef(char *arg);
extern int RP_define_calcdepth(char *name);
extern int RP_define_save(char *name);
extern int RP_define_restore(char *name);
extern int RP_define_save_list(char **list);
extern int RP_define_restore_list(char **list);

/* b_rep_cmds.c */
extern int RP_vseps(char* seps);
extern int RP_repeatstring(char* buf);
extern int RP_repeat(char* buf);
extern int RP_onerror_1(char* arg);
extern int RP_onerror(char* arg);
extern int RP_abort(char* arg);
extern int RP_quitode(char* arg);
extern int RP_return(char* arg);
extern int RP_label(char* arg);
extern int RP_goto_label(char *command, char *parm);
extern int RP_goto(char* arg);
extern int RP_message(char* arg);
extern int RP_warning(char* arg);
extern int RP_silent(char* arg);
extern int RP_beep(char* arg);
extern int RP_ask(char* arg);
extern int B_ReportPrompt(char* arg);
extern int RP_setdebug(char* arg);
extern int RP_setindent(char* arg);
extern int RP_setmultiline(char* arg);
extern int RP_noparsing(char* arg);
extern int RP_shift_args(char* arg);
extern int RP_chdir(char* arg);
extern int RP_rmdir(char* arg);
extern int RP_mkdir(char* arg);
extern int RP_settime(char* arg);
extern int RP_incrtime(char* arg);
extern int RP_system(char* arg);
extern int B_shellexec(char *arg);
extern int B_Sleep(char* arg);
extern int B_GraphDefault(char* type);

/* b_rep_fns.c */ 
extern U_ch *RPF_IodeVersion();
extern U_ch *RPF_take(U_ch** args);
extern U_ch *RPF_drop(U_ch** args);
extern U_ch *RPF_replace(U_ch** args);
extern U_ch *RPF_equal(U_ch** args);
extern U_ch *RPF_upper(U_ch** args);
extern U_ch *RPF_lower(U_ch** args);
extern U_ch *RPF_sqz(U_ch** args);
extern U_ch *RPF_strip(U_ch** args);
extern U_ch *RPF_fmtint(U_ch** args);
extern U_ch *RPF_ansi(U_ch** args);
extern U_ch *RPF_count(U_ch** args);
extern U_ch *RPF_index(U_ch** args);
extern U_ch *RPF_void(U_ch **args);
extern U_ch *RPF_date(U_ch** args);
extern U_ch *RPF_time(U_ch** args);
extern U_ch *RPF_month(U_ch** args);
extern U_ch *RPF_sstderr(U_ch** args);
extern U_ch *RPF_srelax(U_ch** args);
extern U_ch *RPF_ttitle(U_ch** args);
extern U_ch *RPF_cvalue(U_ch** args);
extern U_ch *RPF_vvalue(U_ch** args);
extern U_ch *RPF_lvalue(U_ch** args);
extern U_ch *RPF_ivalue(U_ch** args);
extern U_ch *RPF_evalue(U_ch** args);
extern U_ch *RPF_eqsample(U_ch** args);
extern U_ch *RPF_eqsamplefromto(U_ch** args, int fromto);
extern U_ch *RPF_eqsamplefrom(U_ch **args);
extern U_ch *RPF_eqsampleto(U_ch **args);
extern U_ch *RPF_eqlhsrhs(U_ch** args, int lhsrhs);
extern U_ch *RPF_eqlhs(U_ch **args);
extern U_ch *RPF_eqrhs(U_ch **args);
extern U_ch *RPF_sample(U_ch** args);
extern int RPF_vsliste1(CLEC* cl, U_ch*** tbl, int* nb, int type);
extern U_ch *RPF_vsliste(U_ch** args, int type);
extern U_ch **RPF_unique(U_ch** tbl);
extern U_ch *RPF_vliste(U_ch** args);
extern U_ch *RPF_sliste(U_ch** args);
extern U_ch *RPF_expand(U_ch** args, int type);
extern U_ch *RPF_cexpand(U_ch **args);
extern U_ch *RPF_eexpand(U_ch **args);
extern U_ch *RPF_iexpand(U_ch **args);
extern U_ch *RPF_lexpand(U_ch **args);
extern U_ch *RPF_sexpand(U_ch **args);
extern U_ch *RPF_texpand(U_ch **args);
extern U_ch *RPF_vexpand(U_ch **args);

extern int RPF_CalcPeriod(U_ch** args);
extern U_ch *RPF_SimMaxit();
extern U_ch *RPF_SimEps();
extern U_ch *RPF_SimRelax();
extern U_ch* RPF_SimSortNbPasses();
extern U_ch* RPF_SimSortAlgo();
extern U_ch* RPF_SimInitValues();

extern double RPF_SimNormReal(U_ch** args);
extern U_ch *RPF_SimNorm(U_ch** args);

extern int RPF_SimNIterInt(U_ch** args);
extern U_ch *RPF_SimNIter(U_ch** args);

extern int RPF_SimCpuInt(U_ch** args);
extern U_ch* RPF_SimCpu(U_ch** args);

extern U_ch* RPF_SimCpuSCC();
extern U_ch* RPF_SimCpuSort();

extern U_ch *RPF_vtake(U_ch** args);
extern U_ch *RPF_vdrop(U_ch** args);
extern U_ch *RPF_vcount(U_ch** args);
extern U_ch *RPF_memory(U_ch** args);
extern U_ch *RPF_ChronoReset();
extern U_ch *RPF_ChronoGet();
extern U_ch *RPF_fappend(U_ch** args);
extern U_ch *RPF_fdelete(U_ch** args);
extern U_ch *RPF_getdir();
extern U_ch *RPF_chdir(U_ch **args);
extern U_ch *RPF_mkdir(U_ch **args);
extern U_ch *RPF_rmdir(U_ch **args);

/* b_rep_foreach.C */ 
int RP_foreach(char* arg);
int RP_foreach_break(char *name);
int RP_foreach_next(char* arg);

/* b_rep_proc.c */
extern void RP_proc_free_all();
extern int RP_procdef(char* arg);
extern int RP_procexec(char* arg);

/* b_rep_super.c - function pointer that can be superseeded */
extern int (*SB_FileDelete_super    )();
extern int (*SB_FileRename_super    )();
extern int (*SB_FileCopy_super      )();
extern int (*SB_FileEdit_super      )();
extern int (*SB_FileList_super      )();
extern int (*SB_FileImport_super    )();
extern int (*SB_ReportExec_super    )();
extern int (*SB_ReportEdit_super    )();
extern int (*SB_ReportPrompt_super  )();
extern int (*SB_PrintObjDef_super   )();
extern int (*SB_PrintCnf_super      )();
extern int (*SB_ViewPrintGr_super   )();
extern int (*SB_ViewPrintTbl_super  )();
extern int (*SB_ViewByTbl_super     )();
extern int (*SB_DataEdit_super      )();
extern int (*SB_DataEditScroll_super)();
extern int (*SB_DataSearch_super    )();
extern int (*SB_DataDuplicate_super )();
extern int (*SB_DataList_super      )();
extern int (*SB_DataCompare_super   )();
extern int (*SB_DataCalcLst_super   )();
extern int (*SB_DataListSort_super  )();
extern int (*SB_DataEditGraph_super )();
extern int (*SB_DataEditCnf_super   )();
extern int (*SB_DataScan_super      )();
extern int (*SB_StatUnitRoot_super  )();
extern int (*SB_WsLoad_super        )();
extern int (*SB_WsSave_super        )();
extern int (*SB_WsMerge_super       )();
extern int (*SB_WsDescr_super       )();
extern int (*SB_WsCopy_super        )();
extern int (*SB_WsClear_super       )();
extern int (*SB_WsSample_super      )();
extern int (*SB_WsExtrapolate_super )();
extern int (*SB_WsHtoL_super        )();
extern int (*SB_WsLtoH_super        )();
extern int (*SB_WsSeasonAdj_super   )();
extern int (*SB_WsTrend_super       )();
extern int (*SB_WsAggregate_super   )();
extern int (*SB_ModelSimulate_super )();
extern int (*SB_ModelCompile_super  )();
extern int (*SB_IdtExecute_super    )();
extern int (*SB_EqsEstimate_super   )();
extern int (*SB_Dir_super           )();
extern int (*SB_XodeRuleImport_super)();

//extern int (*B_DataDisplayGraph_super)();
//extern int (*B_DataPrintGraph_super  )();   
extern int (*B_WindowMinimize_super  )();   
extern int (*B_WindowMaximize_super  )(); 

extern int (*B_ScrollVarW_super)(char *arg);
extern int (*B_ScrollVarN_super)(char *arg);
extern int (*B_ScrollVarM_super)(char *arg);
extern int (*B_ScrollVarS_super)(char *arg);
extern int (*B_ScrollSclW_super)(char *arg);
extern int (*B_ScrollSclN_super)(char *arg);
extern int (*B_ScrollTblW_super)(char *arg);
extern int (*B_ScrollVTW_super )(char *arg);
extern int (*B_ScrollVTW0_super)(char *arg);
extern int (*B_ScrollVTN_super )(char *arg);

extern int (*ODE_scroll_super) (KDB *kdb, char **lst);
extern int (*T_view_tbl_super) (TBL *tbl, char *smpl, char* name);
// extern int (*T_view_tbl_super) (char* name, char *smpl, char** vars_names); // TEMP version for IODE-QT

/* b_rep_super.c - function declarations */
extern int SB_FileDelete       ();
extern int SB_FileRename       ();
extern int SB_FileCopy         ();
extern int SB_FileEdit         ();
extern int SB_FileList         ();
extern int SB_FileImport       ();
extern int SB_ReportExec       ();
extern int SB_ReportEdit       ();
extern int SB_ReportPrompt     ();
extern int SB_PrintObjDef      ();
extern int SB_PrintCnf         ();
extern int SB_ViewPrintGr      ();
extern int SB_ViewPrintTbl     ();
extern int SB_ViewByTbl        ();
extern int SB_DataEdit         ();
extern int SB_DataEditScroll   ();
extern int SB_DataSearch       ();
extern int SB_DataDuplicate    ();
extern int SB_DataList         ();
extern int SB_DataCompare      ();
extern int SB_DataCalcLst      ();
extern int SB_DataListSort     ();
extern int SB_DataEditGraph    ();
extern int SB_DataEditCnf      ();
extern int SB_DataScan         ();
extern int SB_StatUnitRoot     ();
extern int SB_WsLoad           ();
extern int SB_WsSave           ();
extern int SB_WsMerge          ();
extern int SB_WsDescr          ();
extern int SB_WsCopy           ();
extern int SB_WsClear          ();
extern int SB_WsSample         ();
extern int SB_WsExtrapolate    ();
extern int SB_WsHtoL           ();
extern int SB_WsLtoH           ();
extern int SB_WsSeasonAdj      ();
extern int SB_WsTrend          ();
extern int SB_WsAggregate      ();
extern int SB_ModelSimulate    ();
extern int SB_ModelCompile     ();
extern int SB_IdtExecute       ();
extern int SB_EqsEstimate      ();
extern int SB_Dir              ();
extern int SB_XodeRuleImport   ();
//extern int B_DataDisplayGraph  ();
//extern int B_DataPrintGraph    ();  
extern int B_WindowMinimize    ();  
extern int B_WindowMaximize    ();

extern int B_ScrollVarW(char* arg);
extern int B_ScrollVarN(char* arg);
extern int B_ScrollVarM(char* arg);
extern int B_ScrollVarS(char* arg);
extern int B_ScrollSclW(char* arg);
extern int B_ScrollSclN(char* arg);
extern int B_ScrollTblW(char* arg);
extern int B_ScrollVTW (char* arg);
extern int B_ScrollVTW0(char* arg);
extern int B_ScrollVTN (char* arg);

extern int ODE_scroll(KDB *kdb, char **lst);
extern int T_view_tbl(TBL *tbl, char *smpl, char* name);
// extern int T_view_tbl(char* name, char *smpl, char** vars_names); // Temp version for IODE-QT


/* b_data.c */
extern int B_DataPattern(char* arg,int type);
extern int B_DataRasVar(char* arg);
extern int B_DataCalcVar(char *);
extern int B_DataCreate_1(char *,int *);
extern int B_DataCreate(char *,int );
extern int B_DataDelete_1(char *,int *);
extern int B_DataDelete(char *,int );
extern int B_DataRename(char *,int );
extern int B_DataDuplicate(char *,int );
//extern int B_DataUpdateEqs(char *,char *,char *,int ,SAMPLE *,char *,char *,float *,int );
//extern int B_DataUpdateTbl(char *,char *);
extern int B_DataUpdate(char *,int );
extern char** B_DataSearchParms(char* name, int word, int ecase, int names, int forms, int texts, int type);
extern int B_DataSearch(char *,int );
extern int B_DataEditCnf(char *);
// extern int my_strcmp(const void *,const void *);
extern int B_DataListSort(char *);
extern int B_DataScan(char *,int );
extern int B_DataExist(char *,int );
extern int B_DataAppend(char *,int );
extern int B_DataList(char *,int );
extern int B_DataCalcLst(char *);
extern int B_DataListCount(char *);
extern int B_DataCompareEps(char* arg);
extern int B_DataCompare(char *,int );
//extern int B_DataEditGraph(int ,char *);
extern int B_DataDisplayGraph(char *);
extern int B_DataPrintGraph(char *);

/* b_ras.c */
extern int RasExecute(char *pattern, char *xdim, char *ydim, PERIOD *rper, PERIOD *cper, int maxit, double eps);

/* b_ws.c */
extern int B_WsLoad(char *,int );
extern int X_findtype(char *);
extern int B_WsDump(KDB *,char *);
extern int B_WsSave(char *,int );
extern int B_WsSaveCmp(char *,int );
extern int B_WsExport(char *,int );
extern int B_WsImport(char *,int );
extern int B_WsSample(char *);
extern int B_WsClear(char *,int );
extern int B_WsClearAll(char* arg);
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
extern int B_CsvSave(char* arg, int type);
extern int B_CsvNbDec(char *nbdec);
extern int B_CsvSep(char *sep);
extern int B_CsvNaN(char *nan);
extern int B_CsvAxes(char *var);
extern int B_CsvDec(char *dec);


/* b_prof.c */
extern int B_ProfileLoad(char *);
extern int B_ProfileSave(char *);
extern int B_ProfileReset(void);

/* b_readme.c */
extern int B_ReadMe(void);

/* b_pdest.c */
extern int B_PrintDestFile(char *arg, int newf);
extern int B_PrintDest(char *file);
extern int B_PrintDestNew(char* file);
extern int B_PrintDestExt(char* file, int newf, int type);
extern int B_PrintNbDec(char* nbdec);
extern int B_PrintLang(char* lang);
extern int B_PrintMulti(char* multi);
extern int B_PrintA2mAppend(char* arg);
extern int B_PrintTBreak(char* arg);
extern int B_PrintTPage(char* arg);
extern int B_PrintGPage(char* arg);
extern int B_PrintParaNum(char* arg);
extern int B_PrintPageHeader(char* arg);
extern int B_PrintPageFooter(char* arg);
extern int B_PrintFont(char* arg);
extern int B_PrintTFont(char* arg);
extern int B_PrintTBox(char* arg);
extern int B_PrintTColor(char* arg);
extern int B_PrintTWidth(char* arg);
extern int B_PrintGSize(char* arg);
extern int B_PrintGTheme(char* arg);
extern int B_PrintGBand(char* arg);
extern int B_PrintGBox(char* arg);
extern int B_PrintGBrush(char* arg);
extern int B_GetColor(char* arg);
extern int B_PrintGColor(char* arg);
extern int B_PrintRtfHelp(char* arg);
extern int B_PrintHtmlHelp(char* arg);
extern int B_PrintRtfTitle(char* arg);
extern int B_PrintRtfCopy(char* arg);
extern int B_PrintRtfLevel(char* arg);
extern int B_PrintRtfTopic(char* arg);
extern int B_PrintGdiOrient(char* arg);
extern int B_PrintGdiDuplex(char* arg);
extern int B_PrintGdiPrinter(char* arg);
extern int B_PrintGIFBackColor(char* arg);
extern int B_PrintGIFTransColor(char* arg);
extern int B_PrintGIFInterlaced(char* arg);
extern int B_PrintGIFTransparent(char* arg);
extern int B_PrintGIFFilled(char* arg);
extern int B_PrintGIFFont(char* arg);
extern int B_PrintHtmlStrip(char* arg);
extern int B_PrintHtmlStyle(char* arg);
extern int B_A2mToAll(char* arg, int type);
extern int B_A2mToPrinter(char* arg);
extern int B_A2mToHtml(char* arg);
extern int B_A2mToRtf(char* arg);
extern int B_A2mToMif(char* arg);
extern int B_A2mToCsv(char* arg);
extern int B_A2mSetCol(int *dest, int col);
extern int B_PrintHtmlTableClass(char *table_class);
extern int B_PrintHtmlTRClass(char *tr_class);
extern int B_PrintHtmlTHClass(char *th_class);
extern int B_PrintHtmlTDClass(char *td_class);

/* b_model.c */
extern int B_ModelSimulate(char *);
//extern int B_ModelSimulateEqs(SAMPLE *,char **);
extern int B_ModelSimulateParms(char *);
extern int B_ModelExchange(char *);
extern int B_ModelCompile(char *);
extern int KE_compile(KDB *);
extern int B_ModelCalcSCC(char *arg);
extern int B_ModelSimulateSCC(char *arg);
extern int B_ModelSimulateSaveNIters(char *arg);
extern int B_ModelSimulateSaveNorms(char *arg);

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
//extern char **B_EqsSplitSmplName(char *,SAMPLE **);
extern int B_EqsEstimate(char *);
extern int B_EqsEstimateEqs(SAMPLE *,char **);
extern int B_EqsSetSample(char *);
extern int B_EqsSetMethod(char *);
extern int B_EqsSetBloc(char *);
extern int B_EqsSetCmt(char *);
extern int B_EqsSetInstrs(char *);

/* b_step.c */
extern int B_EqsStepWise(char* arg);

/* b_htol.c */
extern int B_WsHtoLLast(char *);
extern int B_WsHtoLMean(char *);
extern int B_WsHtoLSum(char *);
//extern int HTOL_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *,int *);
//extern int B_htol(int ,char *);

/* b_ltoh.c */
extern int B_WsLtoHStock(char *);
extern int B_WsLtoHFlow(char *);
//extern int LTOH_smpl(SAMPLE *,SAMPLE *,SAMPLE **,int *,int *);
// extern int B_ltoh(int ,char *);
//extern double LTOH_ylin(double *,double );
//extern int LTOH_lin(int ,double *,int ,double *,int ,int );
//extern int LTOH_y2cs(double *,int ,double *);
//extern double LTOH_ycs(double *,double *,double );
//extern int LTOH_cs(int ,double *,int ,double *,int ,int );

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
//extern int HP_calc(double *,double *,int , double);     // JMP 7-3-2019
//extern int HP_calc(double *,double *,int , double, int);  // JMP 12-4-2019
//extern void HP_test(double *,double *,int ,int *,int *);

/* w_wrt1.c */
extern void W_print_enum(int );
extern void W_print_cmd(int );
extern void W_print_par(int );
extern void W_print_tit(int );
extern void W_print_pg_header(char* arg);
extern void W_print_pg_footer(char* arg);
extern void W_print_rtf_topic(char* arg);
extern void W_print_tb(char* title, int nc);

/* sb_file.c */
//extern void SB_FileOpen(void);
//extern void C_FileOpen(void);
//extern void SB_FileNew(void);
//extern void C_FileNew(void);
//extern void SB_FileSave(void);
//extern void C_FileSave(void);
extern int SB_FilePrintSetup(void);
extern void C_FilePrintSetup(void);
extern void C_FilePrintFile(void);
//extern void SB_FilePreferences(void);
extern void SB_PrintPreferences(void);
//extern void C_FilePreferences(void);

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
//extern int SB_ReportPrompt(char *,int );
extern int C_ReportPrompt(void);

/* sb_pdest.c */
extern int C_PrintDestDecLang(int ,int ,int );

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
//extern int SB_DataEdit(char *,int );
//extern int SB_DataEditScroll(char *,int );
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
//extern KDB *KI_series_list(KDB *);
//extern KDB *KI_scalar_list(KDB *);
extern KDB *KI_exec(KDB *,KDB *,int ,char **,KDB *,int ,char **,SAMPLE *);
//extern int *KI_reorder(KDB *);
//extern int KI_read_vars_db(KDB *,KDB *);
//extern int KI_read_vars_file(KDB *,char *);
//extern int KI_read_vars(KDB *,KDB *,KDB *,int ,char **);
//extern int KI_read_scls_db(KDB *,KDB *);
//extern int KI_read_scls_file(KDB *,char *);
//extern int KI_read_scls(KDB *,KDB *,int ,char **);
//extern int KI_execute(KDB *,KDB *,KDB *,int *,SAMPLE *);
//extern int KI_extract(KDB *,KDB *);
//extern int KI_quick_extract(KDB* dbv, KDB* dbi);

/* k_lang.c */
extern int K_LANG;      // Current language

/* k_print.c */
extern  int K_NBDEC;
extern int T_prep_cls(TBL *,char *,COLS **);
extern int T_print_tbl(TBL *,char *);
//extern int T_print_line(TBL *,int ,COLS *);
//extern int T_print_cell(TCELL *,COL *,int );
extern void T_fmt_val(char *,double ,int ,int );
extern void T_print_val(double );
//extern void T_print_string(COL *,char *);
extern void T_open_cell(int ,int ,int );
extern void T_open_attr(int );
extern void T_close_attr(int );
extern char **T_find_files(COLS *);
//extern void T_print_files(COLS *,int );
//extern void T_print_mode(COLS *,int );
//extern void T_print_date(int );
//extern int T_begin_tbl(int ,COLS *);
//extern void T_end_tbl(void);

/* k_graph.c */
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
extern int APIGraphLine(int hdl, TBL *tbl, int i, COLS *cls, SAMPLE *smpl, double *x, double *y, COLS *fcls);
extern int APIGraphLineTitle(int hdl, TLINE *line, COLS *fcls, int i);
extern int APIGraphTimeData(int hdl, SAMPLE *smpl, double *y);
extern int APIGraphTitle(int hdl, char *txt, double *x, int nb);
extern int APIPrepareChart(TBL *tbl, char *gsmpl);
extern int T_GraphEnd();
extern int T_GraphInit(double w, double h, int xgrid, int ygrid, double ymin, double ymax, double zmin, double zmax, int align, int box, int brush);
extern int T_GraphLegend(int axis, int type, char *txt, char *fileop);
extern int T_GraphLine(TBL *tbl, int i, COLS *cls, SAMPLE *smpl, double *x, double *y, COLS *fcls);
//extern int T_GraphLineData(int nb, double *x, double *y);
//extern int T_GraphLineTitle(TLINE *line, COLS *fcls, int i);
extern int T_GraphTest(TBL *tbl);
extern int T_GraphTimeData(SAMPLE *smpl, double *y);
extern int T_GraphTitle(char *txt);
extern int T_GraphXYData(int nb, double *x, double *y);
extern int T_GraphXYLegend(int axis, int type, char *txt, char *fileop);
extern int T_find_opf(COLS *fcls, COL *cl);
extern int T_graph_tbl_1(TBL *tbl, char *gsmpl, int mode);
extern int T_prep_smpl(COLS *cls, COLS **fcls, SAMPLE *smpl);
extern int V_graph(int ,int ,int ,int ,int ,int ,double ,double ,SAMPLE *,char **);

/* k_grep.c */
extern char **K_grep(KDB *,char *,int ,int ,int ,int ,int );
extern char *K_expand(int ,char *,char *,int );
extern char *K_expand_kdb(KDB *,int ,char *,int );
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
//extern int o_calc_lhs(char *,char *,char *);
//extern int o_estgr(char **,SAMPLE *,MAT *,MAT *,int ,int );
//extern int ODE_graph(int ,int ,int ,int ,int ,int ,double ,double ,SAMPLE *,char **);
//extern void T_create_wtcfl(void);
//extern int o_var_gr_1(int ,int ,int ,int ,int ,double ,double ,SAMPLE *,int ,int ,KDB *,char *);
//extern int o_var_gr(int ,int ,int ,int ,int ,double ,double ,SAMPLE *,KDB *,char **);

/* o_abort.c */
// extern int SW_abort(int ,char *);   	// JMP 10/5/2021 double de avec SCR
// extern int SCR_panic(void);			// JMP 10/5/2021 double de avec SCR

/* b_dde.c */
extern int IodeDdeLocale(char *buf);
extern int IodeDdeUnLocale(char *buf);
extern int IodeDdeType(char *szTopic);
extern char *IodeDdeGetWS(char *szItem);
extern char *IodeDdeCreateSeries(int objnb, int bt);
extern char *IodeDdeCreatePer(int bt);
extern char *ToBase26(int num);
extern char *IodeDdeXlsCell(char *offset, int i, int j, int lg, int hg);
extern char *IodeTblCell(TCELL *cell, COL *cl, int nbdec);
extern char *IodeDdeCreateTbl(int objnb, char *ismpl, int *nc, int *nl, int nbdec);
extern char *IodeDdeCreateObj(int objnb, int type, int *nc, int *nl);
extern char *IodeDdeGetReportRC(char *szItem);
extern char *IodeDdeGetXObj(char *szItem, int type);
extern char *IodeDdeGetItem(char *szTopic, char *szItem);
extern int IodeDdeSetWS(char *szItem, char *szBuffer);
extern int IodeDdePlay(char *szItem, char *szBuffer);
extern int DdeTsfKey(char *key);
extern int IodeDdeSetItem(char *szTopic, char *szItem, char *szBuffer);
extern char *B_ExcelGetItem(char *arg);
extern int B_ExcelSetItem(char *ddeitem, char *ptr, int nc, int nl);
extern int B_ExcelDecimal(char *arg);
extern int B_ExcelThousand(char *arg);
extern int B_ExcelCurrency(char *arg);
extern int B_ExcelLang(char *arg);
extern int B_ExcelGet(char *arg, int type);
extern int B_ExcelSet(char *arg, int type);
extern int B_ExcelExecute(char *arg);
extern int B_ExcelCmd(char *cmd, char *arg);
extern int B_DDEGet(char *arg);
extern int B_ExcelWrite(char *ptr);
extern int B_DDEGet(char *arg);
extern char *B_ExcelGetItem(char *arg);
extern int B_ExcelGet(char *arg, int type);
extern int B_ExcelSet(char *arg, int type);
extern int B_ExcelExecute(char *arg);
extern int B_ExcelCmd(char *cmd, char *arg);
extern int B_ExcelWrite(char *ptr);
extern int B_ExcelOpen(char *arg);
extern int B_ExcelClose(char *arg);
extern int B_ExcelPrint(char *arg);
extern int B_ExcelSave(char *arg);
extern int B_ExcelSaveAs(char *arg);
extern int B_ExcelNew(char *arg);
extern int IodeFmtVal(char *buf, double val);

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
//extern KDB  *K_load_odbc(int , char *, int , char **);
extern U_ch *RPS_Open(U_ch** args);
extern U_ch *RPS_Field(U_ch** args);
extern U_ch *RPS_NbFlds(U_ch** args);
extern U_ch *RPS_Record(U_ch** args);
extern U_ch *RPS_Query(U_ch** args);
extern U_ch *RPS_Sql(U_ch** args);
extern U_ch *RPS_Next(U_ch** args);
extern U_ch *RPS_Close(U_ch** args);

/* b_win.c (MSC) */
extern int __cdecl B_WindowMinimize();
extern int __cdecl B_WindowMaximize();
extern int __cdecl B_exec_system(char *arg);
extern int __cdecl B_shellexec(char *arg);

/* w_wrt.c (MSC) */
extern int W_dest(char *filename,int type);
// extern int W_InitParms();
// extern int W_open();
extern int W_close();
extern int W_flush();
extern int W_putc(int ch);
//extern int W_isempty(char *buf);
extern int W_record(char *str);
extern int W_InitDisplay();
extern int W_EndDisplay(char *title,int x,int y,int w,int h);
// extern int W_SavePrinterSettings();
// extern int W_ResetPrinterSettings();
// extern int W_SetPrinterSettings();
extern int W_printf(char *fmt,...);
extern int W_printfDbl(char* fmt, ...);
extern int W_printfRepl(char* fmt, ...);
extern int W_printfReplEsc(char* fmt, ...);
extern int W_printfEx(int dup, int ch1, int ch2, char *fmt, va_list args);

// interface
extern U_ch  *T_mmt_edit(U_ch *, int , int , int , int );


#ifdef SCRCPP
}
#endif
#endif /* _IODEBASE_ */

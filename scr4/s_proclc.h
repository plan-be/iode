#ifdef __cplusplus 
extern "C" { 
#endif 


/* s_calc.c */ 
extern double CALC_start(char *form);

/* s_caltok.c */ 
extern int CALC_open_all(char *filename,int type);
extern int CALC_close(void);
extern int CALC_lex(void);
extern int CALC_read(void);
extern int CALC_unread(void);
extern int CALC_getc(void);
extern int CALC_ungetc(int ch);
extern int CALC_skip(void);
extern int CALC_get_token(void);

/* s_calcc.c */ 
extern int CALC_cc(void);
extern int CALC_save_var(void);
extern int CALC_save_op(void);
extern int CALC_priority_sup(TOKEN token);
extern int CALC_add_stack(TOKEN token);
extern int CALC_clear_stack(void);
extern PAS *CALC_cc2(void);
extern PAS *CALC_cc_stream(void);

/* s_calerr.c */ 
extern char *CALC_error(void);

/* s_calexc.c */ 
extern double CALC_exec_pas(PAS *pas);
extern double CALC_start_exec_pas(PAS *pas);
extern double CALC_assign(double s1,double s2);
extern double CALC_plus(double s1,double s2);
extern double CALC_minus(double s1,double s2);
extern double CALC_times(double s1,double s2);
extern double CALC_div(double s1,double s2);
extern double CALC_exp(double s1,double s2);
extern double CALC_add_as(double s1,double s2);
extern double CALC_dim_as(double s1,double s2);
extern double CALC_times_as(double s1,double s2);
extern double CALC_div_as(double s1,double s2);
extern double CALC_log(double *s);
extern double CALC_sqrt(double *s);
extern double CALC_expn(double *s);
extern double CALC_inv(double *s);
extern double CALC_abs(double *s);
extern double CALC_acos(double *s);
extern double CALC_asin(double *s);
extern double CALC_atan(double *s);
extern double CALC_cos(double *s);
extern double CALC_cosh(double *s);
extern double CALC_int(double *s);
extern double CALC_ln(double *s);
extern double CALC_rad(double *s);
extern double CALC_sin(double *s);
extern double CALC_sinh(double *s);
extern double CALC_tan(double *s);
extern double CALC_tanh(double *s);
extern double CALC_uplus(double *s);
extern double CALC_uminus(double *s);
extern double CALC_deg(double *s);
extern double CALC_incr(double *s);
extern double CALC_decr(double *s);
extern double CALC_e(void);
extern double CALC_pi(void);
extern double CALC_valreg(double *s);
extern double CALC_reg(double *s);

#ifdef __cplusplus 
} 
#endif 

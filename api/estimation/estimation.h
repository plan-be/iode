#pragma once

#include "scr4/s_mat.h"

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/kdb.h"
#include "api/objs/equations.h"
#include "api/lec/lec.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---------------------- ENUMS ---------------------- */

enum IodeEstimationError
{
    EST_NO_EQ_ERR = 1,
    EST_MEM_ERR,
    EST_SYNTAX_ERR,
    EST_LINK_ERR,
    EST_DREG_ERR,
    EST_NAN_ERR,
    EST_VCC_SING_ERR,
    EST_VCU_SING_ERR,
    EST_GMG_SING_ERR,
    EST_NO_SCALARS
};

enum IodeAdjustmentMethod
{
    AM_PARTIAL_ADJUSTMENT,
    AM_ERROR_CORRECTION_METHOD
};

/* ---------------------- GLOBALS ---------------------- */

extern int      E_errno, 
                E_IT, 
                E_MAXIT, 
                E_CONV, 
                E_NEQ, 
                E_NCE, 
                E_NC, 
                E_NINSTR, 
                E_T, 
                E_FROM, 
                *E_C_NBS;
extern KDB      *E_DBV, 
                *E_DBS;
extern SAMPLE   *E_SMPL;
extern char     E_MET;
extern double   E_CONV_TEST, 
                E_EPS;
extern CLEC     **E_CRHS;
extern char     **E_LECS, 
                **E_INSTRS, 
                **E_ENDOS;
extern char     *E_ERRORS[];
extern MAT      *E_RHS,
                *E_LHS,
                *E_U,
                *E_G,
	            *E_VCC,
                *E_VCCTMP,
                *E_M,
                *E_G,
                *E_MTMP,
                *E_MTMPP,
                *E_D,
                *E_C,
                *E_SMO,
                *E_NBCE,
                *E_VCU,
                *E_IVCU,
                *E_MCU,
                *E_GMU,
                *E_dC,
                *E_UM,
                *E_UMT,
                *E_UMTMP,
                *E_UVCCTMP,
                *E_GMUTMP,
                *E_DF,
                *E_SSRES,
                *E_RSQUARE_ADJ,
                *E_DW,
                *E_LOGLIK,
                *E_STDERR,
                *E_MEAN_Y,
                *E_STDEV,
                *E_RSQUARE,
                *E_FSTAT,
                *E_STD_PCT,
                *E_MCORR,
                *E_MCORRU,
                *E_DEV;

/* ---------------------- FUNCS ---------------------- */

/* e_dftest.c */
extern double *E_UnitRoot(char *,int ,int ,int );
//extern int E_UnitRoot_1(SAMPLE *,char *);
extern void E_SclToReal(char *,double *);
extern void E_PrintDF(char *,double *,int ,int ,int );
extern int E_GetLecName(char *,char *);

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
extern void E_msg_n(int, ...);
extern void E_error_n(int, ...);
extern void E_msg(char* fmt, ...);
extern void E_error(char* fmt, ...);

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

/* k_est.c */
extern int KE_est_s(KDB *,KDB *,KDB *,SAMPLE *,char **, int);
extern void E_tests2scl(EQ *,int ,int ,int );
extern int KE_estim(char* veqs, char* afrom, char* ato);
extern void E_savescl(double, int, char*);
extern void E_savevar(char*, int, MAT*);

#ifdef __cplusplus
}
#endif

#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/kdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------- DEFINE -------------------------*/

#define ASC_SMPL    1

/*---------------- ENUMS -------------------------*/

enum IodeImportFormat
{
    IMPORT_ASCII,
    IMPORT_ROT_ASCII,
    IMPORT_DIF,
    IMPORT_BISTEL,
    IMPORT_NIS,
    IMPORT_GEM,
    IMPORT_PRN,
    IMPORT_TXT
};

const static int IODE_NB_IMPORT_FORMATS = 8;

/*---------------- STRUCTS ------------------ */

// forward declaration
struct _impdef_;

// struct defining input file and fn pointers for one type of data format to be imported
typedef struct _impdef_ {
    YYKEYS  *imp_keys;                                          // Table of keywords (see YY group of functions in scr4 libs)
    int     imp_dim;                                            // Nb of keys in imp_keys
    int     (*imp_hd_fn)(struct _impdef_*, char*, int);         // Pointer to the fn to open the input file and to read its header
    int     (*imp_hd_sample_fn)(YYFILE*, SAMPLE*);              // Pointer to the fn to open the input file and to read its header
    int     (*imp_vec_var_fn)(YYFILE*, char*, int, double*);    // Pointer to the fn to read full variable (i.e. a name + a series of values)
    int     (*imp_vec_cmt_fn)(char*, char**);                   // Pointer to the fn to read full comment
    int     (*imp_elem_fn)(YYFILE*, char*, int*, double*);      // Pointer to the fn to read a single numerical value (a double)
    int     (*imp_end_fn)();                                    // Pointer to the fn to close the input file
} IMPDEF;

/*---------------- GLOBALS -------------------------*/

extern char     **IMP_rule;
extern char     **IMP_pat;
extern int      IMP_trace;

extern  IMPDEF  IMP_ASC_CMT;
extern  IMPDEF  IMP_BST_CMT;
extern  IMPDEF  IMP_PRN_CMT;
extern  IMPDEF  IMP_TXT_CMT;

extern  IMPDEF  IMP_ASC;
extern  IMPDEF  IMP_RASC;
extern  IMPDEF  IMP_DIF;
extern  IMPDEF  IMP_BST;
extern  IMPDEF  IMP_NIS;
extern  IMPDEF  IMP_GEM;
extern  IMPDEF  IMP_PRN;
extern  IMPDEF  IMP_TXT;

/*---------------- FUNCS -------------------------*/

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

/* k_iprn.c */
extern double PRN_readreal(YYFILE *);
extern int IMP_vec_prn(YYFILE *,char *,int ,double *);
extern int IMP_hd_cprn(IMPDEF *,char *,int );
extern int IMP_vec_cprn(char *,char **);

/* k_rules.c */
extern int IMP_readrule(char *);
extern int IMP_change(char **,char **,char *,char *);

#ifdef __cplusplus
}
#endif

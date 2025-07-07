#pragma once

#include <stdio.h>
#include "api/constants.h"
#include "api/objs/kdb.h"

#ifdef __cplusplus
extern "C" {
#endif

/*---------------- ENUMS -------------------------*/

enum IodeExportFormat
{
    EXPORT_CSV,
    EXPORT_DIF,
    EXPORT_WKS,
    EXPORT_TSP,
    EXPORT_RCSV
};

const static int IODE_NB_EXPORT_FORMATS = 5;

/*---------------- STRUCTS ------------------ */

// forward declaration
struct _expdef_;

// struct defining output File descriptor and fn pointers for one type of data format to export
typedef struct _expdef_ {
    int     (*exp_hd_fn)(struct _expdef_*, KDB*, KDB*, char*);          // Pointer to the function that creates the output file and writes the header
    char    *(*exp_code_fn)(char*, char**);                             // Pointer to the function to create the output object name + the separator
    char    *(*exp_cmt_fn)(KDB*, char*, char**);                        // Pointer to the function to create the output object comment (if it exists in KC_WS) + the separator for the output file
    char    *(*exp_elem_fn)(KDB*, int, int, char**);                    // Pointer to the function constructing an allocated string of one value + sep
    int     (*exp_vec_fn)(struct _expdef_*, char*, int, int);           // Pointer to the function saving the VAR and CMT in the output file
    int     (*exp_vec_var_fn)(struct _expdef_*, char*, char*, char*);   // Pointer to the function saving the VAR and CMT in the output file
    int     (*exp_end_fn)(struct _expdef_*, KDB*, KDB*, char*);         // Pointer to the function that closes the output file after having written its footer
    FILE    *exp_fd;                                                    // Output file descriptor (output of fopen)
} EXPDEF;

/*---------------- GLOBALS ------------------ */

extern  EXPDEF  EXPCSV;
extern  EXPDEF  EXPDIF;
extern  EXPDEF  EXPWKS;
extern  EXPDEF  EXPTSP;
extern  EXPDEF  EXPRCSV;  /* JMP 28-08-98 */

extern  char    EXP_SEP[11];
extern  char    EXP_NA[11];

/*---------------- FUNCS ------------------ */

/* k_emain.c */
//extern void EXP_set(char *,char *);
extern void EXP_val(char *,double );
extern char *EXP_addprepost(char *,char *,char *,char **);
extern char *EXP_addsep(char *,char **);
extern int EXP_Ws(EXPDEF *,KDB *,KDB *,char *,char *,char *,char *);
extern int EXP_Rev_Ws(EXPDEF *,KDB *,KDB *,char *,char *,char *,char *);
extern int EXP_RuleExport(char *,char *,char *,char *,char *,char *,char *,char *,char *,int );

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

/* k_wks.c */
extern int wks_init(char *,int ,int );
extern int wks_cwidth(int ,int );
extern int wks_end(void);
extern void wks_string(char *,int ,int );
extern void wks_value(double ,int ,int );
extern void wks_name(char *,int ,int ,int ,int );

#ifdef __cplusplus
}
#endif

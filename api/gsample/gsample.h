#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/tables.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------- ENUMS ----------------------------*/

/* COLS : LANGUAGE ELEMENTS */
enum IodeColLangElement
{
    COL_OBRACK = 1,
    COL_CBRACK,  
    COL_OPAR,    
    COL_CPAR,    
    COL_COLON,   
    COL_COMMA,   
    COL_DOT,     
    COL_BOS,     
    COL_EOS,     
    COL_NOW,     
    COL_BOS1,    
    COL_EOS1,    
    COL_NOW1,    
    COL_PER,     
    COL_SUBPER
};

/* COLS : OPERATIONS */
enum IodeColOperation
{
    COL_NOP = 20,
    COL_DIFF,
    COL_MDIFF,
    COL_MEAN,
    COL_GRT,
    COL_MGRT,
    COL_ADD,
    COL_BASE,
    COL_LAST_OP,
    COL_SHIFTL,
    COL_SHIFTR
};

#define MAX_MODE    (COL_BASE - COL_DIFF + 1)

/*----------------------- STRUCTS ---------------------------------*/

// COL struct: contains all infos needed to compute one table CELL on one specific GSAMPLE column
//             + cl_val = the result of the LEC formulas on each FILE / PERIOD needed (i.e. max 4 values)
//             + cl_res = final result

typedef struct _col_ {
    short   cl_opy;             // operator on periods => cl_per[0] cl_opy cl_per[1])
    PERIOD  cl_per[2];          // period 1 , period 2
    short   cl_opf;             // operator on files => cl_fnb[0] cl_opf cl_fnb[1]
    short   cl_fnb[2];          // position in K_RWS of file1 and file2 (starts at 1)
    double    cl_val[2][2];  // computed values of the LEC formulas on periods / files => max 4 values see table below

    /*   {{v00, v01},{v10,v11}}

            |             |
            |   file1     |   file2
    --------|-------------|------------
    period1 |    v00      |    v01
            | cl_val[0,0] | cl_val[0,1]    v.. = valeur
    --------|-------------|------------
    period2 |    v10      |    v11
            | cl_val[1,0] | cl_val[1,1]
            |             |
    */
    double    cl_res;        // computed value (v00 opp v10) opf (v01 opp v11)
} COL;

// COLS: group of COL's = result of a GSAMPLE compilation
typedef struct _cols_ {
    int     cl_nb;          // Number of columns
    COL     *cl_cols;       // Pointer to the first COL struct
} COLS;


// REP: definition of the repetition of a group of periods / file
// GSAMPLE example.: (2000/1999):5*4
typedef struct _rep_ {
    short   r_nb;           // Nb of repetitions  (in example => 5)
    short   r_incr;         // Increment          (in example => 4)
} REP;

// FIL: files and operation used in a COL
// GSAMPLE example: (2000:10)[2%3]
typedef struct _fil_ {
    short   fl_op;      // Operation on files (in example => %)
	short   fl_1;       // file nb 1          (in example => 2)
    short   fl_2;       // file nb 2          (in example => 3)
} FIL;

// FILS: group of FIL's
typedef struct _fils_ {
    int     fl_nb;
    FIL     *fl_fils;
} FILS;

// FREF Reference tables for the execution of tables */
//typedef struct _fref_ {
//    CLEC    *fr_clec;
//} FREF;

/*----------------------- GLOBALS ---------------------------------*/

#define K_MAX_FREF          5           // Max number of file references in GSAMPLE's
//extern  FREF    fref[K_MAX_FREF + 1];

extern char     *COL_OPERS[];
// extern char     *COL_OPERS_TEXTS[][3]; // unused - replaced by KLG_OPERS_TEXTS

/*----------------------- FUNCS ---------------------------------*/

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

#ifdef __cplusplus
}
#endif

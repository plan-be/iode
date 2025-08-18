#pragma once

#include "scr4/s_yy.h"

#include "api/constants.h"
#include "api/utils/time.h"
#include "api/objs/tables.h"

/*----------------------- ENUMS ----------------------------*/

/* COLS : LANGUAGE ELEMENTS */
enum IodeColLangElement
{
    COL_OBRACK = 1,     // open bracket [
    COL_CBRACK,         // close bracket ]
    COL_OPAR,           // open parenthesis (
    COL_CPAR,           // close parenthesis )
    COL_COLON,          // colon :
    COL_COMMA,          // comma ;
    COL_DOT,            // dot *
    COL_BOS,            // beginning of sample
    COL_EOS,            // end of sample
    COL_NOW,            // now (current period)
    COL_BOS1,           // beginning of sample (first period of the year)
    COL_EOS1,           // end of sample (first period of the year)
    COL_NOW1,           // now (current period) (first period of the year)
    COL_PER,            // period
    COL_SUBPER          // sub-period
};

/* COLS : OPERATIONS */
enum IodeColOperation
{
    COL_NOP = 20,   // no operation ""
    COL_DIFF,       // difference "-"
    COL_MDIFF,      // mean difference "--"
    COL_MEAN,       // mean "^"
    COL_GRT,        // growth rate "/"
    COL_MGRT,       // mean growth rate "//"
    COL_ADD,        // add "+"
    COL_BASE,       // equal "="
    COL_LAST_OP,
    COL_SHIFTL,     // shift left "<"
    COL_SHIFTR      // shift right ">"
};

#define MAX_MODE    (COL_BASE - COL_DIFF + 1)

/*----------------------- STRUCTS ---------------------------------*/

// COL struct: contains all infos needed to compute one table CELL on one specific GSAMPLE column
//             + cl_val = the result of the LEC formulas on each FILE / PERIOD needed (i.e. max 4 values)
//             + cl_res = final result
struct COL 
{
    short   cl_opy;             // operator on periods => cl_per[0] cl_opy cl_per[1])
    PERIOD  cl_per[2];          // period 1 , period 2
    short   cl_opf;             // operator on files => cl_fnb[0] cl_opf cl_fnb[1]
    short   cl_fnb[2];          // position in K_RWS of file1 and file2 (starts at 1)
    double  cl_val[2][2];       // computed values of the LEC formulas on periods / files => max 4 values see table below
    double  cl_res;             // computed value (v00 op_per v10) op_file (v01 op_per v11)

    /*   {{v00, v01}, {v10,v11}}

            |             |
            |   file1     |   file2
    --------|-------------|------------
    period1 |    v00      |    v01
            | cl_val[0,0] | cl_val[0,1]
    --------|-------------|------------
    period2 |    v10      |    v11
            | cl_val[1,0] | cl_val[1,1]
            |             |
    */
};

// COLS: group of COL's = result of a GSAMPLE compilation
struct COLS 
{
    int     cl_nb;          // Number of columns
    COL     *cl_cols;       // Pointer to the first COL struct
};


// REP: definition of the repetition of a group of periods / file
// GSAMPLE example.: (2000/1999):5*4
struct REP 
{
    short   r_nb;           // Nb of repetitions  (in example => 5)
    short   r_incr;         // Increment          (in example => 4)
};

// FIL: files and operation used in a COL
// GSAMPLE example: (2000:10)[2%3]
struct FIL 
{
    short   fl_op;      // Operation on files (in example => %)
	short   fl_1;       // file nb 1          (in example => 2)
    short   fl_2;       // file nb 2          (in example => 3)
};

// FILS: group of FIL's
struct FILS 
{
    int     fl_nb;
    FIL     *fl_fils;
};

// FREF Reference tables for the execution of tables */
//struct FREF 
//{
//    CLEC    *fr_clec;
//};

/*----------------------- GLOBALS ---------------------------------*/

#define K_MAX_FREF          5           // Max number of file references in GSAMPLE's

// Operator representations used when printing (only valid for period operations)
// TODO: distinguish bw operations between periods and between files ?
inline char* COL_OPERS[] = 
{
    "",     // COL_NOP
    "-",    // COL_DIFF,
    "--",   // COL_MDIFF, 
    "^",    // COL_MEAN, 
    "/",    // COL_GRT,
    "//",   // COL_MGRT,
    "+",    // COL_ADD,
    "="     // COL_BASE,
//    "<",    // Wrong !! Cannot happen
//    ">"     // Wrong !! Idem
};

/*----------------------- FUNCS ---------------------------------*/

/*---------------- FUNCS ------------------ */

inline int col_compare(const void* a, const void* b)
{
    return YY_strcmp((const char*) a, (const char*) b);
}

/* c_cc.c */
COLS *COL_cc(char *);
int COL_free_cols(COLS *);
//int COL_stripy(char *);
char *COL_ctoa(COL *,int ,int ,int );
char *COL_text(COL *,char *,int );
COLS *COL_add_col(COLS *);
//COLS *COL_construct(COLS *,COLS *,FILS *,REP *, int, int);
//int COL_apply_fil(COL *,FIL *);
//int COL_read_per(YYFILE *,PERIOD *);
//COLS *COL_read_y(YYFILE *);
//int COL_read_rep(YYFILE *,REP *);
//int COL_read_1f(YYFILE *,FIL *);
//FILS *COL_read_f(YYFILE *);
//COLS *COL_read_cols(YYFILE *);
int COL_find_mode(COLS *,int *,int );

/* c_calc.c */
int COL_resize(TBL *,COLS *);
void COL_clear(COLS *);
//CLEC *COL_cp_clec(CLEC *);
int COL_exec(TBL *,int ,COLS *);
//int COL_calc(COL *,CLEC *,CLEC *);
// int COL_link(int ,CLEC *);


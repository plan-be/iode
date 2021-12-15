#ifndef _IODE_H_
#define _IODE_H_

/* tests */
#define __DBG__ DebugForce("%s[%d]\n", __FILE__, __LINE__);

/***** ALLOCS DOC 28/8/2012 *****/
// A PLACER DEVANT LES INCLUDES DE SCR (ou dans cc -c -DALLOCDOCON ...)
#define ALLOCDOCON

#include <scr4w.h>
#include <s_scroll.h>
#include <s_strs.h>
#include <s_args.h>
#include <s_mat.h>
#include <s_xdr.h>
#include <s_a2m.h>
#include <math.h>

// values.h n'existe pas en  VCC, remplacer les defines
#ifndef VC
#include <values.h>
#else
#include <float.h>
#define DMAXEXP   DBL_MAX_EXP
#define DMINEXP   DBL_MIN_EXP
#define MAXDOUBLE DBL_MAX
#define MINDOUBLE DBL_MIN
#define MAXFLOAT  FLT_MAX
#define MINFLOAT  FLT_MIN
#endif


//#include "o_objs.h" // JMP 8/12/2011

/******************************* DEFINES **********************************/
#define IODE_VERSION "IODE Modeling Software 6.63 - (c) 1990-2020 Planning Office - Brussels"
#define IODE_VERSION_MAJOR 6
#define IODE_VERSION_MINOR 63
#define K_VERSION  "1.0"
#define OK_MAX_NAME  10
#define K_MAX_NAME   20  /* IODE64K */

#ifndef ARCH
#define ARCH    "INTEL86 DOS"
#endif

// JMP 23/5/2019 (from b_rep.c for b_proc.c)
#define LINELENGTH 102400

#define OK_MAX_FILE  64
#define K_MAX_FILE  512
#define K_MAX_DESC  64
//#define K_CHUNCK    1024 /* JMP 11-03-07 */
#define K_CHUNCK    (64 * 1024)
#define LMAGIC      32

#define K_UPPER     0       /* default for mode */
#define K_LOWER     1
#define K_ASIS      2

#ifndef IODE_REAL
#ifdef REALD
#define IODE_REAL    double
#else
#define IODE_REAL    float
#endif
#endif

#define L_NAN       (IODE_REAL)(-2.0e37)
#define L_ISAN(x)   ((x) >= (IODE_REAL)(-1.0e37))
#define L_IS0(x)    (((x) > 0 && (x) < 1.0e-36) || ((x) <= 0 && (-x) < 1.0e-36))

#if defined(DOS) || defined(SCRW32)
#define P_ALIGN     4
#endif

#ifdef UNIX
#define P_ALIGN     4
#endif

#ifdef SUN3
#define P_ALIGN     4
#endif

#ifdef SUN4
#define P_ALIGN     4
#endif

#ifdef HP
#define P_ALIGN     4
#endif

#ifndef U_sh
#define U_sh    unsigned short
#endif

#define P_len(ptr)              P_get_len(ptr, -1)

#define KARCH(kdb)   ((kdb)->k_arch)
#define KMAGIC(kdb)  ((kdb)->k_magic)
#define KTYPE(kdb)   ((kdb)->k_type)
#define KMODE(kdb)   ((kdb)->k_mode)
//#define KNAME(kdb)   ((kdb)->k_name) Supprimé pour éviter les oublis
#define KNAMEPTR(kdb)((kdb)->k_nameptr) // 6.44
#define KDESC(kdb)   ((kdb)->k_desc)
#define KDATA(kdb)   ((kdb)->k_data)
#define KFREE(kdb)   ((kdb)->k_free)
#define KNB(kdb)     ((kdb)->k_nb)
#define KOBJS(kdb)   ((kdb)->k_objs)

#define KONAME(kdb, pos)    ((kdb)->k_objs[pos].o_name)
#define KSOVAL(kdb, pos)    ((kdb)->k_objs[pos].o_val)
#define KGOVAL(kdb, pos)    (SW_getptr((kdb)->k_objs[pos].o_val))


#define K_NBR_OBJ   7

#define K_CMT       0
#define K_EQS       1
#define K_IDT       2
#define K_LST       3
#define K_SCL       4
#define K_TBL       5
#define K_VAR       6

#define K_OBJ       7

#define K_AC        8
#define K_AE        9
#define K_AI        10
#define K_AL        11
#define K_AS        12
#define K_AT        13
#define K_AV        14

#define K_REP       16
#define K_A2M       17
#define K_AGL       18
#define K_PRF       19
#define K_DIF       20
#define K_MIF       21
#define K_RTF       22
#define K_PS        23
#define K_AAS       24
#define K_TXT       25
#define K_CSV       26

#define K_ASC      -1

#define T_NC(tbl)           (tbl->t_nc)
#define T_NL(tbl)           (tbl->t_nl)
#define T_LANG(tbl)         (tbl->t_lang)
#define T_L(tbl)            (tbl->t_line)
#define T_C(tbl, i, j)      ((tbl->t_line[i]).tl_val[j])

#define KSMPL(kdb)          ((SAMPLE *) (kdb)->k_data)

#define KVVAL(kdb, pos, t)            K_vval(kdb, pos, t)
#define KILEC(kdb, pos)               K_oval0(kdb, pos)
#define KICLEC(kdb, pos)    ((CLEC *) K_oval1(kdb, pos))
#define KSVAL(kdb, pos)     ((SCL *)  K_oval0(kdb, pos))
//#define KCVAL(kdb, pos)     ((CMT *)  K_oval0(kdb, pos))    // TODO: Check (CMT) or (CMT *) => ok:CMT
//#define KLVAL(kdb, pos)     ((LIS *)  K_oval0(kdb, pos))    // TODO: Check (LIS) or (LIS *) = >ok:LIS 
#define KCVAL(kdb, pos)     ((CMT )   K_oval0(kdb, pos))    
#define KLVAL(kdb, pos)     ((LIS )   K_oval0(kdb, pos))    

#define KELEC(kdb, pos)                   K_oval0(kdb, pos)
#define KECLEC(kdb, pos)    ((CLEC *)     K_oval1(kdb, pos))
#define KESOLV(kdb, pos)    (* (char *)   K_oval(kdb, pos, 2))
#define KEMETH(kdb, pos)    (* (char *)   K_oval(kdb, pos, 3))
#define KESMPL(kdb, pos)    (* (SAMPLE *) K_oval(kdb, pos, 4))
#define KECMT(kdb, pos)                   K_oval(kdb, pos, 5)
#define KEBLK(kdb, pos)                   K_oval(kdb, pos, 6)
#define KEINSTR(kdb, pos)                 K_oval(kdb, pos, 7)
#define KEDATE(kdb, pos)    (* (long *)   K_oval(kdb, pos, 8))

#define KTVAL(kdb, pos)     (K_tunpack(SW_getptr(kdb->k_objs[pos].o_val)) )
#define KEVAL(kdb, pos)     (K_eunpack(SW_getptr(kdb->k_objs[pos].o_val)) )

#define KOVAL(kdb, pos)     K_oval0(kdb, pos)

#define KCPTR(name)         K_optr0(KC_WS, name)
#define KLPTR(name)         K_optr0(KL_WS, name)
#define KVPTR(name)         K_vptr(KV_WS, name, 0)
#define KEPTR(name)         K_eptr(KE_WS, name)


/*----------------------- TABLES ----------------------------*/
#define  KT_NORMAL          0
#define  KT_BOLD            1
#define  KT_ITALIC          2
#define  KT_UNDERLINE       4

#define  KT_CENTER          8
#define  KT_LEFT            16
#define  KT_RIGHT           32
#define  KT_DECIMAL         64

#define KT_ALIGN(attr, align)   (((attr) & 7) | (align))
#define KT_FONT(attr, font)     ((attr) | (font))
#define KT_SETFONT(attr, font)  (((attr) & 120) | (font))

#define  KT_ENGLISH         100
#define  KT_DUTCH           (KT_ENGLISH + 1)
#define  KT_FRENCH          (KT_ENGLISH + 2)
#define  KT_DIM             (KT_ENGLISH + 3)
#define  KT_DIV             (KT_ENGLISH + 4)
#define  KT_FILES           (KT_ENGLISH + 5)
#define  KT_MODE            (KT_ENGLISH + 6)
#define  KT_OPEN            (KT_ENGLISH + 7)
#define  KT_CLOSE           (KT_ENGLISH + 8)
#define  KT_BREAK           (KT_ENGLISH + 9)
#define  KT_CELL            (KT_ENGLISH + 10)
#define  KT_LINE            (KT_ENGLISH + 11)
#define  KT_DLINE           (KT_ENGLISH + 12)
#define  KT_TITLE           (KT_ENGLISH + 13)
#define  KT_LEC             (KT_ENGLISH + 14)
#define  KT_STRING          (KT_ENGLISH + 15)
#define  KT_DATE            (KT_ENGLISH + 16)
#define  KT_YMIN            (KT_ENGLISH + 17)
#define  KT_YMAX            (KT_ENGLISH + 18)
#define  KT_ZMIN            (KT_ENGLISH + 19)
#define  KT_ZMAX            (KT_ENGLISH + 20)
#define  KT_XGRID           (KT_ENGLISH + 21)
#define  KT_YGRID           (KT_ENGLISH + 22)
#define  KT_BOX             (KT_ENGLISH + 23)
#define  KT_AXIS            (KT_ENGLISH + 24)
#define  KT_ALGN            (KT_ENGLISH + 25)
#define  KT_LAXIS           (KT_ENGLISH + 26)
#define  KT_RAXIS           (KT_ENGLISH + 27)
#define  KT_GRLINE          (KT_ENGLISH + 28)
#define  KT_GRBAR           (KT_ENGLISH + 29)
#define  KT_GRSCATTER       (KT_ENGLISH + 30)

#define  KT_CHUNCK   5

/*----------------------- EQUATIONS ----------------------------*/
#define  KE_BLK             0
#define  KE_CMT             1
#define  KE_INSTR           2
#define  KE_METH            3
#define  KE_SMPL            4
#define  KE_LSQ             5
#define  KE_ZEL             6
#define  KE_INF             7
#define  KE_GLS             8
#define  KE_DATE            9
#define  KE_OPEN           10
#define  KE_CLOSE          11
#define  KE_STDEV          12
#define  KE_MEANY          13
#define  KE_SSRES          14
#define  KE_STDERR         15
#define  KE_FSTAT          16
#define  KE_R2             17
#define  KE_R2ADJ          18
#define  KE_DW             19
#define  KE_LOGLIK         20
#define  KE_MAXLIK         21

/*----------------------- WS ----------------------------*/
#define KC_WS   K_WS[K_CMT]
#define KE_WS   K_WS[K_EQS]
#define KI_WS   K_WS[K_IDT]
#define KL_WS   K_WS[K_LST]
#define KS_WS   K_WS[K_SCL]
#define KT_WS   K_WS[K_TBL]
#define KV_WS   K_WS[K_VAR]

#define KC_RWS   K_RWS[K_CMT][K_PWS[K_CMT]]
#define KE_RWS   K_RWS[K_EQS][K_PWS[K_EQS]]
#define KI_RWS   K_RWS[K_IDT][K_PWS[K_IDT]]
#define KL_RWS   K_RWS[K_LST][K_PWS[K_LST]]
#define KS_RWS   K_RWS[K_SCL][K_PWS[K_SCL]]
#define KT_RWS   K_RWS[K_TBL][K_PWS[K_TBL]]
#define KV_RWS   K_RWS[K_VAR][K_PWS[K_VAR]]


/*----------------------- COLS ----------------------------*/
/* COLS : LANGUAGE ELEMENTS */

#define COL_OBRACK  1
#define COL_CBRACK  2
#define COL_OPAR    3
#define COL_CPAR    4
#define COL_COLON   5
#define COL_COMMA   6
#define COL_DOT     7
#define COL_BOS     8
#define COL_EOS     9
#define COL_NOW     10
#define COL_BOS1    11
#define COL_EOS1    12
#define COL_NOW1    13
#define COL_PER     14
#define COL_SUBPER  15

/* COLS : OPERATIONS */

#define COL_NOP     20
#define COL_DIFF    21
#define COL_MDIFF   22
#define COL_MEAN    23
#define COL_GRT     24
#define COL_MGRT    25
#define COL_ADD     26
#define COL_BASE    27
#define COL_LAST_OP 39
#define COL_SHIFTL  40
#define COL_SHIFTR  41




#define MAX_MODE    (COL_BASE - COL_DIFF + 1)

/*----------------------- PRINTERS ----------------------------*/
#define W_GDI   A2M_DESTGDIPRT
#define W_A2M   A2M_DESTA2M
#define W_MIF   A2M_DESTFRM
#define W_HTML  A2M_DESTHTML
#define W_RTF   A2M_DESTRTF
#define W_CSV   A2M_DESTCSV
#define W_DUMMY A2M_DESTDUMMY
//#define W_DISP  A2M_DESTGDIEMF
//#define W_DISP  A2M_DESTGDIWMF
#define W_DISP  A2M_DESTTCHRT

/*--------------- KVAR_MODE parameters ------------------------*/
#define K_LEVEL 0
#define K_DIFF  1
#define K_GRT   2
#define K_DIFFY 3
#define K_GRTY  4

/*----------------------- MISC --------------------------------*/
#define K_ISFILE(filename)  (filename != 0 && filename[0] != '-' && filename[0] != 0)

/*---------------- IMPORT/EXPORT: ASC -------------------------*/
#define ASC_SMPL 1

/*---------------- IMPORT/EXPORT: DIF -------------------------*/
#define DIF_EOD     1
#define DIF_TABLE   2
#define DIF_VECTORS 3
#define DIF_DATA    4
#define DIF_BOT     5
#define DIF_COLON   6
#define DIF_V       7
#define DIF_TUPLES  8

/*---------------- MESSAGES DE ODE.MSG ------------------------*/

/* FULL SCREEN OUTPUT FIELDS */
#define OM_WS_STATUS            1
#define OM_SAVE_WS              2
#define OM_WSID                 3
#define OM_INVALID_SMPL         4

/* ODE_ask */
#define OM_SURE                 5
#define OM_LEAVE                6
#define OM_REPLACE              7
#define OM_STOP_SIM             8
#define OM_DELETE               9
#define OM_SAVE_FIRST          10
#define OM_STOP                11

/* ODE_error */
#define OM_PROF_LOADED         12
#define OM_PROF_SAVED          13
#define OM_PRG_NOT_FOUND_1     14
#define OM_NO_OUTPUT           15
#define OM_SMPL_MOD            16
#define OM_EST_ACH             17
#define OM_EXEC_ACH            18
#define OM_SIM_ACH             19
#define OM_OBJS_PRINTED        20
#define OM_TBLS_PRINTED        21
#define OM_TBLS_VIEWED         22
#define OM_FILE_DEL            23
#define OM_FILE_ERR_2          24
#define OM_FILE_COPIED         25
#define OM_FILE_RENAMED        26
#define OM_FILE_PRINTED        27

/* B_error */
#define OM_ILL_ARGS            28
#define OM_DEL_FAILED_1        29
#define OM_DREN_FAILED_2       30
#define OM_DDUP_NO_SENSE       31
#define OM_DDUP_FAILED_2       32
#define OM_DUPD_SCL_ERR        33
#define OM_UPD_TBLS            34
#define OM_DUPD_VAR_ERR_1      35
#define OM_PATTERN_1           36
#define OM_LIST_SET_ERR_1      37
#define OM_NO_FILE_1           38
#define OM_EEST_SMPL_2         39
#define OM_EEST_EMPTY          40
#define OM_FCOPY_OPEN_1        41
#define OM_FCOPY_CREATE_1      42
#define OM_FCOPY_WRITE_1       43
#define OM_CANNOT_DEL_1        44
#define OM_FPRINT_OPEN_1       45
#define OM_FPRINT_WRITE_1      46
#define OM_CANNOT_RENAME_1     47
#define OM_EXEC_SMPL_2         48
#define OM_SIM_SMPL_2          49
#define OM_PRINT_ERR_1         50
#define OM_OBJ_NOT_FOUND_1     51
#define OM_CANNOT_OPEN         52
#define OM_NBDEC_ERR_1         53
#define OM_TBL_NOT_FOUND_1     54
#define OM_TBL_NOT_PRINTED_1   55
#define OM_SMPL_ERR            56
#define OM_PROF_CANNOT_LOAD_1  57
#define OM_PROF_CANNOT_SAVE_1  58
#define OM_TBL_NOT_VIEWED_1    59
#define OM_INV_ODE_FILE_1      60
#define OM_NO_SCL              61
#define OM_SMPL_SYNTAX_1       62
#define OM_CANNOT_PRINT_LINE_1 63

/*---------------- LEC ------------------------*/
#ifndef M_E
#define M_E         2.71828182845904523536
#define M_PI        3.14159265358979323846
#endif

#define L_MAX_NAME  K_MAX_NAME
#define L_MAX_EXPR  512
#define L_MAX_STACK 512
#define L_MAX_NAMES 100
#define L_MAX_COEFS 999

#define L_ISCOEF(x) ((x)[0] >= 'a')

#define L_REAL  double
#define LECREAL float
#define s_dbl   sizeof(LECREAL)
#define s_ptr   sizeof(IODE_REAL *)
#define s_short sizeof(short)
#define s_long  sizeof(long)

/* LEC ERRORS */
#define L_SYNTAX_ERR    1
#define L_PAR_ERR       2
#define L_ARGS_ERR      3
#define L_STACK_ERR     4
#define L_MEMORY_ERR    5
#define L_PERIOD_ERR    6
#define L_LAG_ERR       7
#define L_PERIODY_ERR   8
#define L_BOUNDS_ERR    9
#define L_LINK_ERR     10
#define L_DIVIDE_ERR   11
#define L_LOG_ERR      12
#define L_COEF_ERR     13
#define L_ASSIGN_ERR   14
#define L_DUP_ERR      15
#define L_INVERT_ERR   16
#define L_NOT_FOUND_ERR   17
#define L_MACRO_ERR    18

#define L_open_file(filename)   L_open_all(filename, YY_FILE)
#define L_open_string(str)      L_open_all(str, YY_MEM)
#define L_open_stdin()          L_open_all((char *)0, YY_STDIN)

#define is_fn(op)       ((op) >= L_FN  && (op) < L_OP)
#define is_op(op)       ((op) >= L_OP  && (op) < L_TFN)
#define is_tfn(op)      ((op) >= L_TFN && (op) < L_VAL)
#define is_val(op)      ((op) >= L_VAL && (op) < L_MTFN)  /* JMP 20-04-98 */
#define is_mtfn(op)     ((op) >= L_MTFN)                  /* JMP 20-04-98 */

/* LEC:SPECIAL */
#define L_EOE       10
#define L_OPENP     11
#define L_CLOSEP    12
#define L_VAR       13
#define L_COEF      14
#define L_OPENB     15
#define L_CLOSEB    16
#define L_COMMA     17
#define L_OCPAR     18  /* () */
#define L_PERIOD    19
#define L_VART      20  /* VARIABLE [time] */
#define L_COLON     21
#define L_LCONST    22
#define L_DCONST    23
#define L_SQUARE    24

/* LEC:FUNCTIONS */
#define L_FN        30
#define L_UMINUS    L_FN + 0
#define L_UPLUS     L_FN + 1
#define L_LOG       L_FN + 2
#define L_LN        L_FN + 3
#define L_NOT       L_FN + 4
#define L_EXPN      L_FN + 5
#define L_MAX       L_FN + 6
#define L_MIN       L_FN + 7
#define L_SIN       L_FN + 8
#define L_COS       L_FN + 9
#define L_ACOS      L_FN + 10
#define L_ASIN      L_FN + 11
#define L_TAN       L_FN + 12
#define L_ATAN      L_FN + 13
#define L_TANH      L_FN + 14
#define L_SINH      L_FN + 15
#define L_COSH      L_FN + 16
#define L_ABS       L_FN + 17
#define L_SQRT      L_FN + 18
#define L_INT       L_FN + 19
#define L_RAD       L_FN + 20
#define L_IF        L_FN + 21
#define L_LSUM      L_FN + 22
#define L_LMEAN     L_FN + 23
#define L_FNISAN    L_FN + 24
#define L_LCOUNT    L_FN + 25
#define L_LPROD     L_FN + 26
#define L_SIGN      L_FN + 27
#define L_LSTDERR   L_FN + 28
#define L_RANDOM    L_FN + 29
#define L_FLOOR     L_FN + 30
#define L_CEIL      L_FN + 31
#define L_ROUND     L_FN + 32
#define L_URANDOM   L_FN + 33
#define L_GRANDOM   L_FN + 34
#define L_GAMMA     L_FN + 35
#define L_DIV0      L_FN + 36

/* LEC:OPERATORS */
#define L_OP        70
#define L_OR        L_OP + 0
#define L_AND       L_OP + 1
#define L_GE        L_OP + 2
#define L_GT        L_OP + 3
#define L_LE        L_OP + 4
#define L_LT        L_OP + 5
#define L_EQ        L_OP + 6
#define L_NE        L_OP + 7
#define L_PLUS      L_OP + 8
#define L_MINUS     L_OP + 9
#define L_DIVIDE    L_OP + 10
#define L_TIMES     L_OP + 11
#define L_EXP       L_OP + 12

/* LEC:TFN (time related functions) */
#define L_TFN       100
#define L_LAG       L_TFN + 0   /* l    ([n,] expr) */
#define L_DIFF      L_TFN + 1   /* d    ([n,] expr) */
#define L_RAPP      L_TFN + 2   /* r    ([n,] expr) */
#define L_DLN       L_TFN + 3   /* dln  ([n,] expr) */
#define L_GRT       L_TFN + 4   /* grt  ([n,] expr) */
#define L_MAVG      L_TFN + 5   /* ma   ([n,] expr) */
#define L_VMAX      L_TFN + 6   /* vmax ([n,[m,]] expr) */
#define L_VMIN      L_TFN + 7   /* vmin ([n,[m,]] expr) */
#define L_SUM       L_TFN + 8   /* sum  ([n,[m,]] expr) */
#define L_PROD      L_TFN + 9   /* prod ([n,[m,]] expr) */
#define L_MEAN      L_TFN + 10  /* mean ([n,[m,]] expr) */
#define L_STDERR    L_TFN + 11  /* stderr([n,[m,]] expr) */
#define L_DLAG      L_TFN + 12  /* dlag (n, coef, expr) */ /* !! not implemented */
#define L_LASTOBS   L_TFN + 13  /* lastobs([from, [to,]] expr) */

/* LEC: CONST VALUES */
#define L_VAL       120
#define L_PI        L_VAL + 0
#define L_E         L_VAL + 1
#define L_TIME      L_VAL + 2
#define L_I         L_VAL + 3
#define L_EURO      L_VAL + 4

/* LEC:MTFN (multiple LEC sub-expr possible in argument list) */
#define L_MTFN      140
#define L_CORR      L_MTFN + 0  /* corr([n, [m,]] expr, expr) */ /* JMP 17-04-98 */
#define L_COVAR     L_MTFN + 1  /* covar([n, [m,]] expr, expr) */ /* JMP 17-04-98 */
#define L_COVAR0    L_MTFN + 2  /* covar0([n, [m,]] expr, expr) */ /* JMP 17-04-98 */
#define L_VARIANCE  L_MTFN + 3  /* var([n, [m,]] expr) */ /* JMP 24-04-98 */
#define L_STDDEV    L_MTFN + 4  /* stddev([n, [m,]] expr) *//* JMP 24-04-98 */
#define L_INDEX     L_MTFN + 5  /* index([from, [to,]] expr, expr) /* JMP 31-07-98 */
#define L_ACF       L_MTFN + 6  /* acf([from, [to,]] expr, expr) /* JMP 31-07-98 */
#define L_INTERPOL  L_MTFN + 7  /* mkval(expr) /* JMP 09-03-99 */
#define L_APP       L_MTFN + 8  /* mkval(expr, app) /* JMP 09-03-99 */
#define L_HP        L_MTFN + 9  /* hp(([from, [to,]] lambda, expr) /* GB 10-10-00*/
#define L_DAPP      L_MTFN + 10 /* hp(([from, [to,]] lambda, expr) /* GB 10-10-00*/
#define L_HPSTD     L_MTFN + 11 /* hpstd(([from, [to,]] lambda, expr) /* JMP  12-04-2019 */

#define LYYTEXT   (L_YY->yy_text)
#define LYYLONG   (L_YY->yy_long)
#define LYYREAD   (L_YY->yy_type)
#define LYYDOUBLE (L_YY->yy_double)

/*----------------------------- GRAPHICS -----------------------------*/
/*  COLOR LINE WIDTH FILL : warning: keep the defines values equal the
    enum values from graphics.h */

#define   G_BLACK           0
#define   G_BLUE            1
#define   G_GREEN           2
#define   G_CYAN            3
#define   G_RED             4
#define   G_MAGENTA         5
#define   G_BROWN           6
#define   G_LIGHTGRAY       7
#define   G_DARKGRAY        8
#define   G_LIGHTBLUE       9
#define   G_LIGHTGREEN      10
#define   G_LIGHTCYAN       11
#define   G_LIGHTRED        12
#define   G_LIGHTMAGENTA    13
#define   G_YELLOW          14
#define   G_WHITE           15

#define   G_SOLID_LINE      0
#define   G_DOTTED_LINE     1
#define   G_CENTER_LINE     2
#define   G_DASHED_LINE     3

#define   G_NORM_WIDTH      1
#define   G_THICK_WIDTH     3

#define   G_EMPTY_FILL          0
#define   G_SOLID_FILL          1
#define   G_LINE_FILL           2
#define   G_LTSLASH_FILL        3
#define   G_SLASH_FILL          4
#define   G_BKSLASH_FILL        5
#define   G_LTBKSLASH_FILL      6
#define   G_HATCH_FILL          7
#define   G_XHATCH_FILL         8
#define   G_INTERLEAVE_FILL     9
#define   G_WIDE_DOT_FILL       10
#define   G_CLOSE_DOT_FILL      11

/*        */

#define G_FSCALEMAX 2.0
#define G_WINDOW    0
#define G_TITLE     1
#define G_CHART     2
#define G_XYDATA    3   /* JMP 05-07-92 */
#define G_TIMEDATA  4   /* JMP 05-07-92 */
#define G_LABELDATA 5   /* JMP 05-07-92 */
#define G_XYCHART   6   /* JMP 05-07-92 */
#define G_TIMECHART 7   /* JMP 05-07-92 */
#define G_LABELCHART 8  /* JMP 05-07-92 */
#define G_DATA      9   /* HC 13/7/92*/

#define G_XY        10
#define G_TIME      11
#define G_PIE       12
#define G_LBAR      13

#define G_NO        20
#define G_YES       21

#define G_MAJOR     29
#define G_NONE      30
#define G_MINOR     31

#define G_VALUES    40
#define G_LOG       41
#define G_SEMILOG   42

#define G_PERCENT   50

#define G_LINE      100
#define G_SCATTER   101
#define G_BAR       102
#define G_SCLINE    103
#define G_SBAR      104
#define G_PBAR      105
#define G_FILL      106

#define G_ABS       150

#define G_U         151
#define G_D         152

#define G_L         160
#define G_R         161
#define G_C         162

#define G_HORIZ     200
#define G_VERT      201


#define G_SCATTER_SHAPE 250
#define G_DOT       (G_SCATTER_SHAPE + 0)
#define G_BOX       (G_SCATTER_SHAPE + 1)
#define G_CROSS     (G_SCATTER_SHAPE + 2)
#define G_X         (G_SCATTER_SHAPE + 3)


extern  int     G_CUR_CHART;
extern  int     G_CUR_DATA;
extern  int     G_CUR_WINDOW;
extern  int     G_CUR_TITLE;

/*error msg*/
#define G_SYNTAX_ERR        -1
#define G_UNREC_ERR         -2
#define G_DUP_ERR           -3
#define G_MEM_ERR           -4
#define G_BGI_ERR           -5
#define G_OUT_ERR           -6
#define G_DAT_ERR           -7

#define G_F                 1001
#define G_MAXOBS            1024

/*------ ESTIMATION ------------ */
#define E_NO_EQ_ERR     1
#define E_MEM_ERR       2
#define E_SYNTAX_ERR    3
#define E_LINK_ERR      4
#define E_DREG_ERR      5
#define E_NAN_ERR       6
#define E_VCC_SING_ERR  7
#define E_VCU_SING_ERR  8
#define E_GMG_SING_ERR  9
#define E_NO_SCALARS    10

/******************************* TYPEDEFS **********************************/
typedef char    OFNAME[OK_MAX_FILE];
typedef char    FNAME[K_MAX_FILE];
typedef char    OONAME[OK_MAX_NAME + 1];
typedef char    ONAME[K_MAX_NAME + 1];
typedef long    OSIZE;   /* JMP 23-05-00 */

typedef struct  _okobj_ {
    short       o_val;
	OONAME      o_name;
    char        o_pad[3];
} OKOBJ;

typedef struct  _kobj_ {
    SWHDL       o_val;      /* SWHDL=long */ /* IODE64K */
    ONAME       o_name;
    char        o_pad[3];
} KOBJ;

typedef struct _okdb_ {
    KOBJ        *k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    OFNAME       k_name;
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
} OKDB;

typedef struct _okdb643_ {
    KOBJ        *k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    OFNAME       k_name;
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
    char        k_compressed;           /* IODE64K */
    char        k_reserved[63];         /* IODE64K */
} OKDB643;

typedef struct _kdb_ {
    KOBJ        *k_objs;
	long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    //OFNAME       k_name; // 6.44
    OFNAME       k_oname;  // 6.44 (compat size but not used)
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
    char        k_compressed;           /* IODE64K */
    char        k_reserved[59];         /* 6.44 : decreased by 4 bytes for k_nameptr */
    char        *k_nameptr;             /* 6.44 */
} KDB;

typedef struct _period {
    long    p_y; /* PERIOD LONG */
    long    p_s; /* PERIOD LONG */
    char    p_p;
    char    p_pad[3];
} PERIOD;

typedef struct _sample {
    PERIOD  s_p1,
	    s_p2;
    short   s_nb;
    char    s_pad[2];
} SAMPLE;

typedef struct _olname_ {
    OONAME   name;
    char    pad[3];
    short   pos;
} OLNAME;

typedef struct _lname_ {
    ONAME   name;
	char    pad[3];
    long    pos; /* SWHDL */ /* IODE64K */
} LNAME;

typedef struct _oclec_ {
    short   tot_lg,         /* SWHDL */
	    exec_lg,
	    nb_names;
    char    dupendo;
    char    pad;
    OLNAME  lnames[1];
} OCLEC;

typedef struct _clec_ {
    long    tot_lg,        /* JMP 20-05-00 */ /* IODE64K */
	        exec_lg;       /* JMP 20-05-00 */ /* IODE64K */
    short   nb_names;
    char    dupendo;
    char    pad;
    LNAME   lnames[1];
} CLEC;

/**
 *  IODE OBJECTS STRUCT
 */

// VAR = Variable (pointer to a vector of IODE_REAL)
typedef IODE_REAL     *VAR;  

// SCL = Scalar (struct containing a scalar value, and if estimated, its relaxation parameter and its standard error).
typedef struct _scl_ {
    IODE_REAL    val;       // Value of the scalar
    IODE_REAL    relax;     // Relaxation parameter. Used only for estimation. 
                            // If null, the parameter will not be estimated. 
                            // If 0 < relax < 1, the convergence of the estimation will be slowed down. To use when the estimation process diverges.
    IODE_REAL    std;       // Standard error. Result of the estimation. 
} SCL;

// CMT = Comment (string)
typedef char    *CMT;  

// LIS = List (string)
typedef char    *LIS;

// IDT = Identity (struct with the identity LEC formula and the compiled CLEC structure)
typedef struct _idt_ {
    char     *lec;      // LEC expression (may ot be an equation)
    CLEC     *clec;     // Compiled version of LEC
} IDT;

#define EQS_NBTESTS     20

// EQ = Equation (struct continaing a LEC equation, its compled form (CLEC), the estimation parameter and tests...)
typedef struct _eq_ {
    char    *lec;       // LEC form of the equation (LHS := RHS)
    CLEC    *clec;      // Compiled equation for the simulation
    char    solved;     // Indicates if in clec, the equation is solved with respect to its endogenous (e.g.: "ln X := RHS" => "X := exp(RHS)")
    char    method;     // Estimation method
    SAMPLE  smpl;       // Estimation sample
    char    *cmt;       // Free comment
    char    *blk;       // List of equations estimated simultaneously
    char    *instr;     // List of instruments used to modify metric in the estimation process (INSTR method)
    long    date;       // Estimation date
    float   tests[EQS_NBTESTS]; // Estimation tests
} EQ;

// Table elements definition 

// TCELL = Table Cell 
typedef struct _tcell_ {
    char    *tc_val;    // Content of the CELL (LEC or text)
    char    tc_type;    // TEXT, LEC 
    char    tc_attr;    // KT_LEFT, KT_CENTER, KT_RIGHT, KT_BOLD, KT_ITALIC, KT_UNDERLINE, KT_NORMAL 
    char    tc_pad[2];
} TCELL;

// TLINE = Table Line
typedef struct _tline_ {
    char    *tl_val;        /* if tl_type = title then val = ptr unsigned char else ptr to TCELL */
    char    tl_type;        /* FILES, MODE, TITLE, LINE, CELL */
    char    tl_graph;       /* G_LINE = 0, .... */
    U_ch    tl_axis:1;      /* was tl_attr, Unused before? */
    U_ch    tl_pbyte:7;     /* was tl_attr, Unused before? */
    char    tl_pad[1];
} TLINE;

// TBL = Table (struct containing a table definition)
typedef struct _tbl_ {
    short   t_lang;     // Output language : KT_ENGLISH, KT_FRENCH, KT_DUTCH
    short   t_free;     // = 0, first column is frozen (?) TODO: update doc for t_free
    short   t_nc;       // Number of columns
    short   t_nl;       // Number of lines
	TLINE   t_div;      // t_nc CELLS, each CELL contains a divider, attr 
    TLINE   *t_line;    // t_nl TLINE of t_nc CELLS */
	float   t_zmin;     // Min on 2d axis (value axis on graph) */
    float   t_zmax;     // Max on 2d axis (value axis on graph) */
    float   t_ymin;     // Min on 1st axis (value axis on graph) */
    float   t_ymax;     // Max on 1st axis (value axis on graph) */
    char    t_attr;     // 0, ... 9 */ // TODO: complete doc
    char    t_box;      // 0, ... 9 */ // TODO: complete doc
    char    t_shadow;   // 0, ... 9 */ // TODO: complete doc
    char    t_gridx;    // G_NONE, G_MINOR, G_MAJOR */
    char    t_gridy;    // G_NONE, G_MINOR, G_MAJOR */
    char    t_axis;     // G_VALUES, G_LOG, G_SEMILOG, G_PERCENT */
    char    t_align;    // G_LEFT, G_MIDDLE, G_RIGHT */
    char    t_pad[13];
} TBL;

/******************************* X64  : TYPEDEFS  particuliers 32 bits pour import/export  **********************************/
#define X64 (sizeof(char*) == 8)
typedef long PTR32; // pour garantir char  *

typedef struct _okdb32_ {
    PTR32       k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    OFNAME       k_name;
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
} OKDB32;

typedef struct _okdb643_32_ {
    PTR32       k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
	char        k_magic[LMAGIC];
    OFNAME       k_name;
	char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
    char        k_compressed;           /* IODE64K */
    char        k_reserved[63];         /* IODE64K */
} OKDB643_32;

typedef struct _kdb32_ {
    PTR32       k_objs;
    long        k_nb;
    short       k_type;
    short       k_mode;
    char        k_arch[LMAGIC];
    char        k_magic[LMAGIC];
    //OFNAME       k_name; // 6.44
    OFNAME       k_oname;  // 6.44 (compat size but not used)
    char        k_desc[K_MAX_DESC];
    char        k_data[K_MAX_DESC];     /* Client Data */
    char        k_compressed;           /* IODE64K */
    char        k_reserved[59];         /* 6.44 : decreased by 4 bytes for k_nameptr */
    PTR32       k_nameptr;             /* 6.44 */
} KDB32;

typedef struct _idt32_ {
    PTR32     lec;
    PTR32     clec;
} IDT32;

typedef struct _eq32_ {
    PTR32   lec;
    PTR32   clec;
    char    solved;
    char    method;
    SAMPLE  smpl;
    PTR32   cmt;
    PTR32   blk;
    PTR32   instr;
	long    date;
    float   tests[EQS_NBTESTS]; /* FLOAT 12-04-98 */
} EQ32;

/* Table definition */
typedef struct _tcell32_ {
    PTR32   tc_val;
    char    tc_type;    /* TEXT, LEC */
    char    tc_attr;    /* LEFT, CENTER, RIGHT, BOLD, ITALIC, UNDERLINE, NORMAL */
    char    tc_pad[2];
} TCELL32;

typedef struct _tline32_ {
    PTR32   tl_val;        /* if tl_type = title then val = ptr unsigned char else ptr to TCELL */
    char    tl_type;        /* FILES, MODE, TITLE, LINE, CELL */
    char    tl_graph;       /* G_LINE = 0, .... */
    U_ch    tl_axis:1;      /* was tl_attr, Unused before? */
    U_ch    tl_pbyte:7;     /* was tl_attr, Unused before? */
    char    tl_pad[1];
} TLINE32;

typedef struct _tbl32_ {
    short   t_lang;
    short   t_free;     /* = 0, first column is frozen */
    short   t_nc;
    short   t_nl;
    TLINE32 t_div;     /* t_nc CELLS, each CELL contains a divider, attr */
    PTR32   t_line;    /* t_nl TLINE of t_nc CELLS */
    float   t_zmin;    /* FLOAT 12-04-98 */
    float   t_zmax;    /* FLOAT 12-04-98 */
    float   t_ymin;    /* FLOAT 12-04-98 */
    float   t_ymax;    /* FLOAT 12-04-98 */
    char    t_attr;    /* 0, ... 9 */
    char    t_box;     /* 0, ... 9 */
    char    t_shadow;  /* 0, ... 9 */
    char    t_gridx;   /* G_NONE, G_MINOR, G_MAJOR */
    char    t_gridy;   /* G_NONE, G_MINOR, G_MAJOR */
    char    t_axis;    /* G_VALUES, G_LOG, G_SEMILOG, G_PERCENT */
	char    t_align;   /* G_LEFT, G_MIDDLE, G_RIGHT */
    char    t_pad[13];
} TBL32;

/*----------------------- COLS ---------------------------------*/
typedef struct _col_ {
    short   cl_opy;     /*op‚rateur sur p‚riode per1 opp per2*/
    PERIOD  cl_per[2];  /*p‚riode 1 , p‚riode 2*/
    short   cl_opf;     /*op‚rateur fichier  fich1 opf fich2 */
    short   cl_fnb[2];  /*fichier1 , fichier 2 */
    IODE_REAL    cl_val[2][2];
    /*   {{v00, v01},{v10,v11}} avec

	   ³            ³
	   ³   fich1    ³   fich2
    ÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄ
    per1   ³    v00     ³    v01
	   ³ cl_val[0,0]³ cl_val[0,1]    v.. = valeur
    ÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄ
    per2   ³    v10     ³    v11
	   ³ cl_val[1,0]³ cl_val[1,1]
	   ³            ³
      */
    IODE_REAL    cl_res;   /* r‚sultat de (v00 opp v10) opf (v01 opp v11)*/
} COL;

typedef struct _cols_ {
    int     cl_nb;
    COL     *cl_cols;
} COLS;

typedef struct _rep_ {
    short   r_nb;
    short   r_incr;
} REP;

typedef struct _fil_ {
    short   fl_op;
	short   fl_1;
    short   fl_2;
} FIL;

typedef struct _fils_ {
    int     fl_nb;
    FIL     *fl_fils;
} FILS;

/* Reference tables for the execution of tables */
typedef struct _fref_ {
    CLEC    *fr_clec;
} FREF;

/*---------------- IMPORT / EXPORT ------------------ */
typedef struct _impdef_ {
    YYKEYS  *imp_keys;
    int     imp_dim;
    int     (*imp_hd_fn)();
    int     (*imp_vec_fn)();
    int     (*imp_elem_fn)();
    int     (*imp_end_fn)();
} IMPDEF;

typedef struct _expdef_ {
    int     (*exp_hd_fn)();

    char    *(*exp_code_fn)();
    char    *(*exp_cmt_fn)();
    char    *(*exp_elem_fn)();
    int     (*exp_vec_fn)();

    int     (*exp_end_fn)();
    FILE    *exp_fd;
} EXPDEF;

/*------------------------ LEC ----------------------- */
typedef struct _alec {          /* LEC atomic element */
	int     al_type;            /* type : L_VAR, L_COEF, L_CONST ... */
    union {
        LECREAL v_real;         /* constant values IODE_REAL */
        long    v_long;         /* constant values long int */
        int     v_nb_args;      /* nb of args for fn */
        struct {
            short   pos;        /* coef or series pos in table ?? */
            PERIOD  per;        /* PERIOD if any */
            short   lag;        /* lag if any */
        } v_var;                /* variable  */
	short   v_coef;             /* coef number */
	PERIOD  v_per;              /* period */
    } al_val;
} ALEC;

typedef struct _cvar_ {
    short   pos,  /* SWHDL ?? NON */
	    lag,
	    ref,
	    pad;
    PERIOD  per;
} CVAR;


typedef struct _token {
    LECREAL tk_real;
    long    tk_long;
    PERIOD  tk_period;
	int     tk_def;
	char    tk_name[L_MAX_NAME + 1];
} TOKEN;

typedef struct _lstack {        /* stack of operators used by L_analyse */
    unsigned ls_op      : 8;    /* operator */
    //unsigned ls_nb_args : 8;    /* nb of arguments */
    unsigned ls_nb_args;        /* nb of arguments */ // 16 bits pour permettre de vérifier si plus de 255 arguments
} LSTACK;



/*------------------------ REPORTS ----------------------- */
typedef struct _repfile_ {
    char                *filename;
    unsigned char       **tbl;
    int                 curline,
			nblines;
} REPFILE;


#ifdef SCRCPP
extern "C" {
#endif


extern REPFILE *CUR_REPFILE;

/******************************* VARS *************************************/

extern  char    *BUF_DATA;
extern char     L_PERIOD_CH[];
extern int      L_PERIOD_NB[];
extern  char    k_magic[][LMAGIC];
extern  char    k_ext[][4];

#define K_MAX_FREF          5
extern  FREF    fref[K_MAX_FREF + 1];

extern char     *COL_OPERS[];
extern char     *COL_OPERS_TEXTS[][3]; /* JMP38 01-10-92 */

extern char     W_filename[];
extern FILE     *W_fd;
extern int      W_gdiask;
extern int      W_a2mapp;
extern int      W_rtfhelp;
extern int      W_htmlhelp;
extern int      W_gdiorient;
extern int      W_gdiduplex;
extern char     W_gdiprinter[80];

extern  KDB     *K_WS[7];
extern  KDB     *K_RWS[7][5];
extern  int     K_PWS[7];
extern  int     K_WARN_DUP;
extern  int     K_SECRETSEP;

extern  int     K_SCR;

extern  char    **IMP_rule;
extern  char    **IMP_pat;
extern  int     IMP_trace;

extern  IODE_REAL    KSIM_EPS, KSIM_RELAX;
extern  int     KSIM_MAXIT, KSIM_DEBUG, KSIM_PASSES, KSIM_DEBUGNEWTON,
	KSIM_SORT, KSIM_START;
extern  char    **KSIM_EXO;

/*-------------- MESSAGES -----------------------*/
extern void (*B_MessageBox_impl)(unsigned char* title, unsigned char* message, unsigned char* buts[]);

/*------------- IMPORT/EXPORT: Cmt --------------*/
extern  IMPDEF  IMP_ASC_CMT;
extern  IMPDEF  IMP_BST_CMT;
extern  IMPDEF  IMP_PRN_CMT;
extern  IMPDEF  IMP_TXT_CMT;

/* ------------- IMPORT/EXPORT:Var --------------*/
extern  IMPDEF  IMP_ASC;
extern  IMPDEF  IMP_RASC;
extern  IMPDEF  IMP_DIF;
extern  IMPDEF  IMP_BST;
extern  IMPDEF  IMP_NIS;
extern  IMPDEF  IMP_GEM;
extern  IMPDEF  IMP_PRN;
extern  IMPDEF  IMP_TXT;

extern  EXPDEF  EXPCSV;
extern  EXPDEF  EXPDIF;
extern  EXPDEF  EXPWKS;
extern  EXPDEF  EXPTSP;
extern  EXPDEF  EXPRCSV;  /* JMP 28-08-98 */

extern  char    EXP_SEP[11];
extern  char    EXP_NA[11];

/*------------ MISC -------------------*/
extern  char    K_AggrChar;
extern  ONAME   tbl_name;

extern char     **EXEC_ARGS;

extern  int     KT_ndec;
extern  char    **EXEC_ARGS;
extern  char    *KT_smpl;
extern  char    KT_sep;
extern  char    **B_ERROR_MSG;
extern  char    B_SEPS[];
extern  int     B_NBDEC;
extern  int     B_MULTIBAR; /* GB 30/10/2007 */
extern  int     B_LANG;     /* JMP38 26-09-92 */
extern  int     KVAR_MODE, KVAR_NDEC, KVAR_START;
extern  char    STATIC_BUF[];
extern  int     STATIC_BUF_LG;

extern  int     B_dest;
extern  char    *B_outfile;
extern double   A2M_GWIDTH;
extern double   A2M_GHEIGHT;
extern int      A2M_BOXWIDTH;
extern int      A2M_BACKBRUSH;
extern int      A2M_BACKCOLOR;

/*----------------- LEC ----------------------*/
extern  int     L_errno;
extern  int     L_LEVEL;
extern  SAMPLE  L_SAMPLE;
extern  ALEC    *L_EXPR;
extern  char    **L_NAMES;
extern  int     L_NB_EXPR, L_NB_AEXPR, L_NB_ANAMES;
extern  int     L_NB_NAMES;
extern  int L_PRIOR[],
	L_MAX_FARGS[],
	L_MIN_FARGS[],
	L_MAX_TARGS[],
	L_MIN_TARGS[],
	L_MAX_MTARGS[],
	L_MIN_MTARGS[];

extern  TOKEN   L_TOKEN;
extern  YYFILE  *L_YY;
extern  char    L_PERIOD_CH[];
extern  int     L_PERIOD_NB[];

extern int  G_errno;

extern  char    **KEXEC_VFILES, **KEXEC_SFILES;
extern  int     KEXEC_TRACE;

/*------------------ ESTIMATION -----------------------*/
extern int  E_errno, E_IT, E_MAXIT, E_CONV,
       E_NEQ, E_NCE, E_NC, E_NINSTR, E_T, E_FROM,
       *E_C_NBS;
extern KDB  *E_DBV, *E_DBS;
extern SAMPLE  *E_SMPL;
extern char E_MET;
extern IODE_REAL E_CONV_TEST, E_EPS;
extern CLEC **E_CRHS;
extern char **E_LECS, **E_INSTRS, **E_ENDOS;
extern char *E_ERRORS[];
extern MAT  *E_RHS,
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

/************************* SIMULATION *************************************/
extern int      *KSIM_NITERS;                           // JMP 21/3/2012
extern int  KSIM_MAXIT;
extern IODE_REAL     *KSIM_NORMS;
//extern IODE_REAL KSIM_RELAX, KSIM_EPS;


/*********************** FUNCTIONS DECLARATIONS ***************************/
// NEW FOR IODECOM (JMP 1/23/2018)

// Table of function ptrs
extern  KDB   *(*K_load_asc[])();
extern  int   (*K_save_asc[])();
extern  int   (*K_save_csv[])();
extern  int   (*K_pack[])();
//extern  int   (*K_cmpobj[])();
extern  int   (*K_xdrobj[])();

#ifdef SCRCPP
}
#endif

/* -- Function declarations --
1/3/2018 --> Function declaration are moved into iodebase.h and iodevarg.h.
*/

#include "iodebase.h"
#include "iodevarg.h"


#endif /* _IODE_H_ */

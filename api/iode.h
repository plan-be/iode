/**
 *  @header4iode
 *
 *  IODE.H
 *  ------
 *  Contains
 *      - #includes
 *      - #defines
 *      - structs and typedefs
 *      - declarations of functions
 *      - declarations of variables
 *
 *  This file should be included in all IODE api based C/C++ developement.
 *
 */

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
#ifndef _MSC_VER
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
// ALD 23/11/2023
#include "version.h"

#define K_VERSION  "1.0"
#define OK_MAX_NAME  10
#define K_MAX_NAME   20  /* IODE64K */

#ifndef ARCH
#define ARCH    "INTEL86 DOS"
#endif

// ALD 18/11/2022
#define I_DEFAULT_FILENAME  "ws"

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

// REALD defines if not yet defined (to avoid compilation warning)
#ifndef REALD
    #define REALD
#endif
// IODE_REAL forced to double (-DREALD not needed anymore) => JMP 19/02/2022
#define IODE_REAL    double

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
#define KETESTS(kdb, pos)   ((float *)    K_oval(kdb, pos, 9))

//#define KE_STDEV (kdb, pos)  KETESTS(kdb, pos)[1]
//#define KE_MEANY (kdb, pos)  KETESTS(kdb, pos)[2]
//#define KE_SSRES (kdb, pos)  KETESTS(kdb, pos)[3]
//#define KE_STDERR(kdb, pos)  KETESTS(kdb, pos)[4]
//#define KE_FSTAT (kdb, pos)  KETESTS(kdb, pos)[6]
//#define KE_R2    (kdb, pos)  KETESTS(kdb, pos)[7]
//#define KE_R2ADJ (kdb, pos)  KETESTS(kdb, pos)[8]
//#define KE_DW    (kdb, pos)  KETESTS(kdb, pos)[9]
//#define KE_LOGLIK(kdb, pos)  KETESTS(kdb, pos)[10]


#define KTVAL(kdb, pos)     (K_tunpack(SW_getptr(kdb->k_objs[pos].o_val)) )
#define KEVAL(kdb, pos)     (K_eunpack(SW_getptr(kdb->k_objs[pos].o_val)) )

#define KOVAL(kdb, pos)     K_oval0(kdb, pos)

#define KCPTR(name)         K_optr0(KC_WS, name)     // Returns a pointer to the CMT in swap (not allocated -> do not free)
#define KIPTR(name)         K_optr0(KI_WS, name)     // id.
#define KLPTR(name)         K_optr0(KL_WS, name)     // id.
#define KVPTR(name)         K_vptr(KV_WS, name, 0)   // id.

#define KEPTR(name)         K_eptr(KE_WS, name)      // returns an allocated EQ
#define KTPTR(name)         K_tptr(KT_WS, name)      // returns an allocated TBL


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

/*--------------- VAR_INIT defines ------------------------*/
#define KV_INIT_TM1       0
#define KV_INIT_TM1_A     1
#define KV_INIT_EXTRA     2
#define KV_INIT_EXTRA_A   3
#define KV_INIT_ASIS      4
#define KV_INIT_TM1_NA    5
#define KV_INIT_EXTRA_NA  6

/*--------------- WS HTOL defines ------------------------*/
#define WS_HTOL_LAST   0
#define WS_HTOL_MEAN   1
#define WS_HTOL_SUM    2

/*--------------- WS LTOH defines ------------------------*/
#define WS_LTOH_STOCK  0
#define WS_LTOH_FLOW   1

#define WS_LTOH_LIN    'L'
#define WS_LTOH_CS     'C'
#define WS_LTOH_STEP   'S'

/*----------------------- MISC --------------------------------*/
#define K_ISFILE(filename)  (filename != 0 && filename[0] != '-' && filename[0] != 0)


/*---------------- IMPORT/EXPORT: FORMAT DEFINES -------------------------*/
#define EXP_CSV  0
#define EXP_DIF  1
#define EXP_WKS  2
#define EXP_TSP  3
#define EXP_RCSV 4

#define IMP_FMT_ASCII     0
#define IMP_FMT_ROT_ASCII 1
#define IMP_FMT_DIF       2
#define IMP_FMT_Bistel    3
#define IMP_FMT_NIS       4
#define IMP_FMT_GEM       5
#define IMP_FMT_PRN       6
#define IMP_FMT_TXT       7

/*---------------- IMPORT/EXPORT: ASC -------------------------*/
#define ASC_SMPL 1

/*---------------- IMPORT/EXPORT: DIF -------------------------*/
#define DIF_EOD     1
#define DIF_TABLE   2
#define DIF_VECTORS 3
#define DIF_DATA    4
#define DIF_BOT     5
#define DIF_COMMA   6
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


/* SIMULATION */
#define SORT_CONNEX    0
#define SORT_BOTH      1
#define SORT_NONE      2

/******************************* TYPEDEFS **********************************/
typedef char    OFNAME[OK_MAX_FILE];
typedef char    FNAME[K_MAX_FILE];
typedef char    ONAME[K_MAX_NAME + 1];
typedef long    OSIZE;   /* JMP 23-05-00 */

typedef struct  _kobj_ {
    SWHDL       o_val;          // position in the "SWAP" memory
    ONAME       o_name;         // name of the object
    char        o_pad[3];
} KOBJ;

typedef struct _kdb_ {
    KOBJ        *k_objs;                // map <position in the "SWAP" memory, object name>
	long        k_nb;                   // number of objects in the database
    short       k_type;                 // type of the object: K_CMT, K_EQS, ..., K_VAR
    short       k_mode;                 // case of the object name: K_UPPER, K_LOWER or K_ASIS 
    char        k_arch[LMAGIC];         // ???
    char        k_magic[LMAGIC];        // ???
    OFNAME       k_oname;               // ??? (compat size but not used)
    char        k_desc[K_MAX_DESC];     // ???
    char        k_data[K_MAX_DESC];     // Sample if Variables database
    char        k_compressed;           // ???
    char        k_reserved[59];         // not used (NOTE: decreased by 4 bytes in version 6.44 for k_nameptr)
    char        *k_nameptr;             // filepath to the database file
} KDB;

typedef struct _period {
    long    p_y;        // year
    long    p_s;        // position in the year (according to the periodicity)
    char    p_p;        // periodicity (Y S Q M W D)
    char    p_pad[3];
} PERIOD;

typedef struct _sample {
    PERIOD  s_p1,
	    s_p2;
    short   s_nb;
    char    s_pad[2];
} SAMPLE;

typedef struct _lname_ {
    ONAME   name;   // scalar or variable name
	char    pad[3];
    long    pos;    // SWHDL 
} LNAME;

typedef struct _clec_ {
    long    tot_lg,      
		exec_lg;       
    short   nb_names;   // number of scalar and variables names
    char    dupendo;
    char    pad;
    LNAME   lnames[1];  // list of of scalar and variable names
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

// TBL (table) definition
// ----------------------

// TCELL = Table Cell
typedef struct _tcell_ {
    char    *tc_val;    // NULL or
			// char* if type == KT_STRING or
			// packed IDT (i.e. char*) if type == KT_LEC
    char    tc_type;    // KT_STRING or KT_LEC
    char    tc_attr;    // KT_LEFT, KT_CENTER, KT_RIGHT, KT_BOLD, KT_ITALIC, KT_UNDERLINE, KT_NORMAL
    char    tc_pad[2];  // Padding for struct alignment
} TCELL;

// TLINE = Table Line
typedef struct _tline_ {
    char    *tl_val;    // if tl_type == KT_CELL  : tl_val is TCELL*
			// if tl_type == KT_TITLE : tl_val is TCELL*
			// if tl_type == KT_LINE  : tl_val is NULL
			// if tl_type == KT_MODE  : tl_val is NULL
			// if tl_type == KT_DATE  : tl_val is NULL
			// if tl_type == KT_FILES : tl_val is NULL
    char    tl_type;    // KT_FILES, KT_MODE, KT_TITLE, KT_LINE or KT_CELL
    char    tl_graph;   // 0=Line, 1=scatter, 2=bar (non implemented in all IODE flavours)
    U_ch    tl_axis:1;  // 0 if values are relative to the left axis, 1 to the right axis
    U_ch    tl_pbyte:7; // available free space
    char    tl_pad[1];  // Padding for struct alignment
} TLINE;

// TBL = Table (struct containing a table definition)
typedef struct _tbl_ {
    short   t_lang;     // Output language : KT_ENGLISH, KT_FRENCH, KT_DUTCH
    short   t_free;     // if 0, first column is frozen, otherwise, col 1 is repeated as other columns
    short   t_nc;       // Number of columns (of text and lec, not calculated values)
    short   t_nl;       // Number of lines
	TLINE   t_div;      // t_nc TCELL's, each TCELL contains a divider
    TLINE   *t_line;    // t_nl TLINE's of t_nc TCELL's
    float   t_zmin;     // Min on the right axis
    float   t_zmax;     // Max on the right axis
    float   t_ymin;     // Min on left axis
    float   t_ymax;     // Max on left axis
    char    t_attr;     // Combination (logical &) of attributes: KT_BOLD, KT_ITALIC, KT_UNDERLINE, KT_CENTER,
			// KT_DECIMAL, KT_LEFT and KT_RIGHT
    char    t_box;      // 1 to surround the chart by a box
    char    t_shadow;   // 1 to place a shadow behind the chart
    char    t_gridx;    // 0 = major grids, 1 = no grids, 2 = minor + major grids
    char    t_gridy;    // idem
    char    t_axis;     // 0=normal axis, 1=log, 2=semi-log, 3=percents (TODO: to be tested)
    char    t_align;    // Text alignement: 0=left, 1=centered, 2 = right
    char    t_pad[13];  // Padding for struct alignment
} TBL;

/******************************* X64  : TYPEDEFS  particuliers 32 bits pour import/export  **********************************/
#define X64 (sizeof(char*) == 8)



/*----------------------- Table calculations on GSAMPLE's ---------------------------------*/

// COL struct: contains all infos needed to compute one table CELL on one specific GSAMPLE column
//             + cl_val = the result of the LEC formulas on each FILE / PERIOD needed (i.e. max 4 values)
//             + cl_res = final result

typedef struct _col_ {
    short   cl_opy;             // operator on periods => cl_per[0] cl_opy cl_per[1])
    PERIOD  cl_per[2];          // period 1 , period 2
    short   cl_opf;             // operator on files => cl_fnb[0] cl_opf cl_fnb[1]
    short   cl_fnb[2];          // position in K_RWS of file1 and file2 (starts at 1)
    IODE_REAL    cl_val[2][2];  // computed values of the LEC formulas on periods / files => max 4 values see table below

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
    IODE_REAL    cl_res;        // computed value (v00 opp v10) opf (v01 opp v11)
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
//

/*---------------- IMPORT / EXPORT ------------------ */

// struct defining input file and fn pointers for one type of data format to be imported
typedef struct _impdef_ {
    YYKEYS  *imp_keys;          // Table of keywords (see YY group of functions in scr4 libs)
    int     imp_dim;            // Nb of keys in imp_keys
    int     (*imp_hd_fn)();     // Pointer to the fn to open the input file and to read its header
    int     (*imp_vec_fn)();    // Pointer to the fn to read full variable (i.e. a name + a series of values)
    int     (*imp_elem_fn)();   // Pointer to the fn to read a single numerical value (a double)
    int     (*imp_end_fn)();    // Pointer to the fn to close the input file
} IMPDEF;

// struct defining output File descriptor and fn pointers for one type of data format to export
typedef struct _expdef_ {
    int     (*exp_hd_fn)();     // Pointer to the function that creates the output file and writes the header
    char    *(*exp_code_fn)();  // Pointer to the function to create the output object name + the separator
    char    *(*exp_cmt_fn)();   // Pointer to the function to create the output object comment (if it exists in KC_WS) + the separator for the output file
    char    *(*exp_elem_fn)();  // Pointer to the function constructing an allocated string of one value + sep
    int     (*exp_vec_fn)();    // Pointer to the function saving the VAR and CMT in the output file
    int     (*exp_end_fn)();    // Pointer to the function that closes the output file after having written its footer
    FILE    *exp_fd;            // Output file descriptor (output of fopen)
} EXPDEF;

/*------------------------ LEC ----------------------- */
typedef struct _alec {      /* LEC atomic element */
	int     al_type;        /* type : L_VAR, L_COEF, L_CONST ... */
    union {
	LECREAL v_real;         /* constant values IODE_REAL */
	long    v_long;         /* constant values long int */
	int     v_nb_args;      /* nb of args for fn */
	struct {
	    short   pos;        /* coef or series pos in table ?? */
	    PERIOD  per;        /* PERIOD if any */
	    short   lag;        /* lag if any */
	} v_var;                /* variable  */
	short   v_coef;         /* coef number */
	PERIOD  v_per;          /* period */
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

typedef struct _lstack {        // stack of operators used by L_analyse 
    unsigned ls_op      : 8;    // operator 
    //unsigned ls_nb_args : 8;  // nb of arguments 
    unsigned ls_nb_args;        // nb of arguments : 16 bits instead of 8 to allow checking max 255 args
} LSTACK;



/*------------------------ REPORTS ----------------------- */
#define RP_LINELENGTH 102400

// REPFILE contains a report to be interpreted and the current line during interpretation
typedef struct _repfile_ {
    char                *filename;      // Source file for report file (*.rep) or
                                        // Proc name and position for procedures (PROCDEF)
    unsigned char       **tbl;          // Lines of the report
    int                 curline,        // Current line (during execution)
			nblines;        // Total number of lines in the report
} REPFILE;

typedef struct _repfns_ {
    char    *keyw;          // function name to use in reports
    char    *key_camel;     // function name in CamelCase
    int     (*fn)();        // function pointer in *non GUI* mode
    int     (*sfn)();       // function pointer in GUI mode
    int     type;           // suffix required after keyw (var, idt...) or not:
			    //   0=not required
						//   1=ws type required: cmt, idt, ... see k_ext => not used ?
						//   2=suffix required: file extensions defined in k_ext
						//   3=ws type required in non GUI, no extension in GUI
						//   4=suffix required: file extensions defined in k_ext if non GUI, not required in GUI
    int     group;          // for color highlighting
} BFNS;

const static int BFNS_NB_GROUPS = 2;

typedef struct _rpfn_ {
    U_ch    *name;          // name of the @function (ex "upper")
    U_ch    *(*fn)();       // Pointer to the corresponding function
} RPFN;


#ifdef SCRCPP
extern "C" {
#endif

extern REPFILE *CUR_REPFILE;        // Pointer to the current REPFILE during report execution
extern BFNS    B_fns[];             // Table of report command names and function pointers
extern RPFN    RP_FNS[];            // Table of report @functions names and function pointers
extern char*   RP_RPTSTR;           // Repeat string used by the command $repeat. Default = '_'.
extern char*   RP_VSEPS;            // Separators allowed in report @functions
extern int     RP_RT;               // Indicates what to do on error (0-ignore, -2=return, -3=ABORT, -4=QUITODE)
extern int     RP_ARG0;             // Position of the first argument in RP_ARGV (can be changed by a call to $shift)
extern int     RP_DEBUG;            // Debug level: 0=no debug, 1=std debug, 2=full debug
extern int     RP_ALLOW_INDENT;     // If set, indentation is allowed in reports
extern int     RP_ALLOW_MULTILINE;  // If set, accepts instruction on multiple lines (non breaking char = '\')
extern int     RP_ALLOW_NOPARSING;  // If set, text between '`' is left asis
extern int     RP_NOPARSING_CHAR;   // If set, strings between RP_NOPARSING_CHAR are not parsed
extern int     RP_PRINT;            // 2 = print and display messages, 1 = print only, 0 = ignore
extern int     RP_T;                // current value of t for LEC evaluations in reports
extern PERIOD  RP_PER;              // current PERIOD for LEC evaluations in reports


/******************************* VARS *************************************/

extern char    *BUF_DATA;
extern char    L_PERIOD_CH[];
extern int     L_PERIOD_NB[];
extern char    k_magic[][LMAGIC];
extern char    k_ext[][4];

#define K_MAX_FREF          5           // Max number of file references in GSAMPLE's
//extern  FREF    fref[K_MAX_FREF + 1];

extern char     *COL_OPERS[];
// extern char     *COL_OPERS_TEXTS[][3]; // unused - replaced by KLG_OPERS_TEXTS

extern char     W_filename[];
extern FILE     *W_fd;
extern int      W_gdiask;
extern int      W_a2mapp;
extern int      W_rtfhelp;
extern int      W_htmlhelp;
extern int      W_gdiorient;
extern int      W_gdiduplex;
extern char     W_gdiprinter[80];

extern KDB      *K_WS[7];
extern KDB      *K_RWS[7][5];
extern int      K_PWS[7];
extern int      K_WARN_DUP;
extern int      K_SECRETSEP;

extern int      K_SCR;

extern char     **IMP_rule;
extern char     **IMP_pat;
extern int      IMP_trace;

extern IODE_REAL    KSIM_EPS,               // Model simulation convergence threshold
		    KSIM_RELAX,             // Model relaxation parameter ("damping")
		    KSIM_NEWTON_EPS,        // Newton-Raphson convergence threshold
		    KSIM_NEWTON_STEP;       // Newton-Raphson step to calculate the local derivative (f(x+h) - f(x)) / h

extern int          KSIM_MAXIT,             // Simulation: max number of iterations
		    KSIM_DEBUG,             // Simulation: if not null : save 3 list _PRE, _INTER and _POST
		    KSIM_PASSES,            // Simulation: number of passes for the heuristic triangulation algorithm
		    KSIM_NEWTON_DEBUG,      // Newton-Raphson: save a trace of the sub-iterations
		    KSIM_NEWTON_MAXIT,      // Newton-Raphson: max number of iterations of the Newton-Raphson sub algorithm.
		    KSIM_SORT,              // Simulation: reordering option : SORT_NONE, SORT_CONNEX or SORT_BOTH
		    KSIM_START;             // Simulation: endogenous initial values



extern char         **KSIM_EXO;

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
// extern  char    KT_sep; replaced by A2M_SEPCH JMP 20/03/2023
extern  char    **B_ERROR_MSG;
extern  char    B_SEPS[];
// extern  int     B_NBDEC;  // JMP 18-04-2022
extern  int     B_MULTIBAR; /* GB 30/10/2007 */
// extern  int     B_LANG;    // JMP 18/04/2022
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
extern YYKEYS L_TABLE[];

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
extern int          KSIM_MAXIT;     // Maximum number of iteration to reach a solution
extern int          *KSIM_NITERS;   // Numbers of iterations needed for each simulation period
extern long         *KSIM_CPUS;     // Elapsed time for each simulation period
extern IODE_REAL    *KSIM_NORMS;    // Convergence threshold reached at the end of each simulation period

extern int          KSIM_CPU_SCC;   // Elapsed time to compute SCC
extern int          KSIM_CPU_SORT;  // Elapsed time to sort interdep block

// EQUATION ORDERING
extern int  KSIM_PRE;               // number of equations in the "prolog" block
extern int  KSIM_INTER;             // number of equations in the "interdep" block
extern int  KSIM_POST;              // number of equations in the "epilog"
//extern int  *KSIM_ORDER;
//extern char *KSIM_ORDERED;
extern int  KSIM_MAXDEPTH;          // Number of equations in the model
extern int  *KSIM_POSXK;            // Position in KSIM_DBV of the endo variable of equation "KSIM_DBE[i]"
extern int  *KSIM_POSXK_REV;        // Position in KSIM_DBE of the equation whose endo is "KSIM_DBV[i]" (reverse of KSIM_POSXK)

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
//#include "iodevarg.h"


#endif /* _IODE_H_ */


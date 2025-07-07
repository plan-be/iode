#ifndef _YY_
#define _YY_

#include <stdio.h>
#include <string.h>
#include "s_alldoc.h" // JMP 28/9/2012

//#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============= SCR_YY.H ===================================== */

#define YY_MAX_STRING_LG 40000
#define YY_MAX_RECORD    40000
#define YY_STRING_DELIMITER '"'
#define YY_CHAR_DELIMITER '\''
#define YY_MAX_DEF_LG    15

#define YY_EOF          -1
#define YY_LONG         -2
#define YY_DOUBLE       -3
#define YY_STRING       -4
#define YY_SPECIAL      -5
#define YY_COMMENT      -100
#define YY_LETTER       -110
#define YY_WORD         -8
#define YY_UNKNOWN      -9
#define YY_MEMORY_FULL  -10
#define YY_ERROR        -11

// #define YYFILE          struct _yyfile /* JMP 28-12-97 */
#define YY_FILE         0
#define YY_MEM          1
#define YY_STDIN        2

typedef struct _yykeys {
    unsigned char   *yk_word;       /* keyword */
    short           yk_def;         /* define  */           /*4.04*/
} YYKEYS;

typedef struct _yydef {
    unsigned char    *yd_def;        /* pointer to the definition */
    int              yd_nb;          /* Parameter number */
    unsigned char    *yd_parms;      /* Parameter names */
    unsigned char    yd_name[YY_MAX_DEF_LG + 1]; /* Macro Name */
} YYDEF;

typedef struct _yyfile {
    char    *yy_name;         /* file name */
    FILE    *yy_fd;           /* file descriptor */
    int     yy_c_line;        /* current line in the file */
    int     yy_c_col;         /* current col in the line */
    int     yy_line;          /* current line of the object */
    int     yy_col;           /* current col of the object */
    int     yy_type;          /* last type read */
    long    yy_long;          /* last long read */
    double  yy_double;        /* last double read */
    int     yy_text_lg;       /* allocated bytes for yy->yy_text */
    unsigned char *yy_text;   /* last text read */
    int     yy_lg;            /* length of the last string read */
    int     yy_blks;          /* number of blanks gener. by tabs */
    struct _yyfile *yy_prev;   /* ptr to file wh.includes current */ /* JMP 28-12-97 */
    struct _yyfile *yy_include;/* ptr to file wh.includes current */ /* JMP 28-12-97 */
    int     yy_reread;        /* 1 after unread, 0 else */
    int     yy_nb_keys;       /* number of element in YYKEYS table */
    YYKEYS  *yy_keys;         /* YYKEYS keywords table */
    int     yy_input_type;    /* Type of text (YY_FILE, YY_MEM, YY_STDIN) */
    int     yy_cpos;          /* current position in the string */
    unsigned char *yy_ptr;    /* pointer to the string (if YY_MEM) */
    unsigned char *yy_record; /* Recorded characters */
    int     yy_rec_alg;       /* Size of allocated buffer to record */
    int     yy_rec_lg;        /* Number of recorded characters */
    int     yy_nb_stdefs,     /* Number of static macros definitions */
            yy_nb_dyndefs;    /* Number of dynamic macros definitions */
    YYDEF   **yy_stdefs,      /* Static macros definition table */
            **yy_dyndefs;     /* Dynamic macros definition table */
    int     yy_nb_if;         /* Level of ##if */
    int     yy_msg;           /* private */
} YYFILE; /* JMP 28-12-97 */

extern int      YY_errno;
extern int      YY_OLD_DEF;             /* JMP_M 4.20 26-06-95 */
extern char     YY_B_COMMENT[5],
                YY_E_COMMENT[5];
extern int      YY_CASE_SENSITIVE;

#ifdef __cplusplus
}
#endif

#ifndef SCRPROTO

#ifdef __cplusplus
extern "C" {
#endif

extern YYFILE   *YY_open();
extern char     *YY_error();
extern int      YY_strcmp();
extern YYDEF    *YY_search_def();

#ifdef __cplusplus
}
#endif

#else

#ifdef __cplusplus
extern "C" {
#endif
/*extern int      YY_strcmp(char *, char *); */
extern double   SCR_atof(char *); /* JMP 29-12-97 */

#ifdef __cplusplus
}
#endif

#include <s_proy.h> /* JMP 28-12-97 */
#endif /* SCRPROTO */

#endif /* _YY_ */






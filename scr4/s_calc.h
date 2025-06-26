#ifndef _S_CALC_H_
#define _S_CALC_H_

#include <scr4.h>
#include <errno.h>
#include <math.h>
#include <setjmp.h>

#define MIN(a, b)   (((a) < (b)) ? (a) : (b))
#define MAX(a, b)   (((a) > (b)) ? (a) : (b))
#ifndef M_E
#define M_E         2.71828182845904523536
#define M_PI        3.14159265358979323846
#endif

/* mots clés */

#define     C_TOKEN_SUP 10
#define     C_OP        C_TOKEN_SUP+1
#define     C_OPENP     C_TOKEN_SUP+2
#define     C_CLOSEP    C_TOKEN_SUP+3
#define     C_FN        C_TOKEN_SUP+4
#define     C_REDO      C_TOKEN_SUP+5
#define     C_DO        C_TOKEN_SUP+6
#define     C_EOE       C_TOKEN_SUP+7

/* operators */

#define     C_OPERATOR  50
#define     C_ASSIGN    C_OPERATOR+1
#define     C_PLUS      C_OPERATOR+2
#define     C_MINUS     C_OPERATOR+3
#define     C_TIMES     C_OPERATOR+4
#define     C_DIV       C_OPERATOR+5
#define     C_EXP       C_OPERATOR+6

/* opérateurs sur registres */

#define     C_ADD_AS    C_OPERATOR+7
#define     C_DIM_AS    C_OPERATOR+8
#define     C_TIMES_AS  C_OPERATOR+9
#define     C_DIV_AS    C_OPERATOR+10

/* function */

#define     C_FUNCTION  100
#define     C_LOG       C_FUNCTION+1
#define     C_SQRT      C_FUNCTION+2
#define     C_EXPN      C_FUNCTION+3
#define     C_INV       C_FUNCTION+4
#define     C_ABS       C_FUNCTION+5
#define     C_ACOS      C_FUNCTION+6
#define     C_ASIN      C_FUNCTION+7
#define     C_ATAN      C_FUNCTION+8
#define     C_COS       C_FUNCTION+9
#define     C_COSH      C_FUNCTION+10
#define     C_INT       C_FUNCTION+11
#define     C_LN        C_FUNCTION+12
#define     C_RAD       C_FUNCTION+13
#define     C_SIN       C_FUNCTION+14
#define     C_SINH      C_FUNCTION+15
#define     C_TAN       C_FUNCTION+16
#define     C_TANH      C_FUNCTION+17
#define     C_UPLUS     C_FUNCTION+18
#define     C_UMINUS    C_FUNCTION+19
#define     C_DEG       C_FUNCTION+20
#define     C_INCR      C_FUNCTION+21
#define     C_DECR      C_FUNCTION+22

/* C_errno */

#define     C_ERR           0
#define     C_PAR_ERR       C_ERR+1
#define     C_SYNTAX_ERR    C_ERR+2
#define     C_STACK_ERR     C_ERR+3
#define     C_ARGS_ERR      C_ERR+4
#define     C_MEM_ERR       C_ERR+5
#define     C_LOG_ERR       C_ERR+6
#define     C_REG_NAME_ERR  C_ERR+7
#define     C_INTERNAL_ERR  C_ERR+8
#define     C_DIVIDE_ERR    C_ERR+9
#define     C_POW_ERR       C_ERR+10
#define     C_SQRT_ERR      C_ERR+11
#define     C_TRIGO_ERR     C_ERR+12

/*  valeurs */

#define     C_VAL       150
#define     C_E         C_VAL+1
#define     C_PI        C_VAL+2

/*  variable ou registre */
#define     C_VAR       200
#define     C_REG       C_VAR+1


/* special */
#define     C_LCONST    250
#define     C_DCONST    251


#ifndef REAL
#define REAL    double
#define C_NAN       (REAL)(-2.0e36)
#define C_ISAN(x)   ((x) >= (REAL)(-1.0e36))
#endif

#define     C_TEXT      (C_YY->yy_text)
#define     C_PTR       (C_YY->yy_ptr)
#define     C_LONG      (C_YY->yy_long)
#define     C_READ      (C_YY->yy_type)
#define     C_DOUBLE    (C_YY->yy_double)
#define     C_NAN       (REAL)(-2.0e36)

#define     NB_REGISTER 1       /*version en ligne*/
#define     MAX_EXPR    10

#define last_op         (C_OPS[C_NB_OPS - 1])
#define is_fn(op)       ((op) >= C_FUNCTION && (op) < C_VAL)
#define is_op(op)       ((op) >= C_OPERATOR && (op) < C_FUNCTION)
#define is_val(op)      ((op) >= C_VAL && (op) < C_VAR)
#define is_var(op)      ((op) >= C_VAR)

#define C_MAX_EXPR  64      /* JMP 4.23 03-01-96 */
#define C_MAX_STACK 32      /* JMP 4.23 03-01-96 */

/* variables */
#define C_NAN       (REAL)(-2.0e36)

extern  REAL    REGISTER[NB_REGISTER];
extern  int     C_errno;
extern  YYFILE  *C_YY;

typedef struct  _token_
	{
	    int type;    /* C_VAR C_VAL C_FUNCTION C_OPERATOR */
	    union
	    {
		REAL    v_real;
		long    v_long;
		int     v_val;
		int     v_reg;
		int     fn;
		int     op;
	    } val;
	    int prior;
	    unsigned char string[10];
	}   TOKEN;

typedef struct _pas_
	{
	    int         nb;
	    TOKEN       *first;
	}   PAS;

extern  int         C_PRIOR[12];
extern  TOKEN       C_EXPR[C_MAX_EXPR];
extern  TOKEN       C_OPS[C_MAX_STACK];
extern  TOKEN       C_TOKEN;
extern  TOKEN       LAST_TOKEN;
extern  REAL        *stack0;



/*functions */

#define CALC_open_file(filename)   CALC_open_all(filename, YY_FILE)
#define CALC_open_string(str)      CALC_open_all(str, YY_MEM)
#define CALC_open_stdin()          CALC_open_all((char *)0, YY_STDIN)

#ifndef SCRPROTO
extern  PAS     *CALC_cc_stream();
extern  PAS     *CALC_cc2();
extern  REAL    CALC_exec_pas();
extern  REAL    CALC_start_exec_pas();
extern  char    *CALC_error();
extern  REAL    CALC_start();

#else /* SCRPROTO */
#include <s_proclc.h>
#endif /* SCRPROTO */

#endif



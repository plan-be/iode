#ifndef _S_SWAP_H_
#define _S_SWAP_H_

#include <stdio.h>
#include <math.h>
#include <s_swap.h>
//#include <s_scroll.h>

#define NOSWAP

#define MREAL           double
#define ZERO(x)         (fabs(x) < 1e-25)

// WARNING: using min and max as macro names makes conflicts with the GNU implementation 
//          of the C++ standard library
#define _max_(a,b)        (((a) > (b)) ? (a) : (b))
#define _min_(a,b)        (((a) < (b)) ? (a) : (b))

#define M_NC(m)           ((m)->m_nc)
#define M_NL(m)           ((m)->m_nl)

#define NC1             M_NC(m1)
#define NC2             M_NC(m2)
#define NC3             M_NC(m3)
#define NL1             M_NL(m1)
#define NL2             M_NL(m2)
#define NL3             M_NL(m3)

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************/
/*  to swap or not to swap in EMS ? */
/*  Les matrices sont stockées en EMS par défaut, la définition de la
constante symbolique NOSWAP impose le stockage des matrices en mémoire
conventionnelle */

#ifdef NOSWAP

#define VAL(m)          ((m)->m_lines)
#define VAL1            VAL(m1)
#define VAL2            VAL(m2)
#define MATL(m,i)       ((m)->m_lines[i])
#define MATE(m,i,j)     (MATL(m,i))[j]
#define MATE1(i,j)      (MATE(m1,i,j))
#define MATE2(i,j)      (MATE(m2,i,j))
#define MATE3(i,j)      (MATE(m3,i,j))
#define ROW(m,i)        ((m)->m_lines[i])

typedef MREAL    *VECT;

typedef struct _mat_
	    {
		int         m_nl,
			    m_nc;

		VECT        *m_lines;
	    } MAT;

#else

#define VAL(m)          ((m)->m_lines)
#define VAL1            VAL(m1)
#define VAL2            VAL(m2)
#define MATL(m,i)       ((MREAL*)SW_getptr((m)->m_lines[i]))
#define MATE(m,i,j)     (MATL(m,i))[j]
#define MATE1(i,j)      (MATE(m1,i,j))
#define MATE2(i,j)      (MATE(m2,i,j))
#define MATE3(i,j)      (MATE(m3,i,j))
#define ROW(m,i)        ((m)->m_lines[i])

typedef int     VECT;

typedef struct _mat_
	    {
		int         m_nl,
			    m_nc;

		VECT        *m_lines;
	    } MAT;

#endif

/* messages d'erreur */

#define     M_ERR_MSG       1
#define     M_PROD_ERR      M_ERR_MSG + 1
#define     M_MEM_ERR       M_ERR_MSG + 2
#define     M_TRAN_ERR      M_ERR_MSG + 3
#define     M_INVDIM_ERR    M_ERR_MSG + 4
#define     M_INVDET_ERR    M_ERR_MSG + 5
#define     M_DUP_ERR       M_ERR_MSG + 6
#define     M_DETDIM_ERR    M_ERR_MSG + 7
#define     M_DETSING_ERR   M_ERR_MSG + 8
#define     M_SOLDIM_ERR    M_ERR_MSG + 9
#define     M_SOLNOSOL_ERR  M_ERR_MSG + 10
#define     M_TRDIM_ERR     M_ERR_MSG + 11
#define     M_ALLOCDIM_ERR  M_ERR_MSG + 12
#define     M_SCALEDIM_ERR  M_ERR_MSG + 13
#define     M_CLNULL_ERR    M_ERR_MSG + 14
#define     M_OPERDIM_ERR   M_ERR_MSG + 15
#define     M_EXTRDIM_ERR   M_ERR_MSG + 16

/****** VARIABLES */

extern  int M_errno;

#ifdef __cplusplus
}
#endif

/********** FNS PROTOTYPES ***********/

#ifndef SCRPROTO

#ifdef __cplusplus
extern "C" {
#endif

extern  char    *M_error();
extern  MAT*    M_alloc();
extern  MAT*    M_prod();
extern  MAT*    M_trans();
extern  MAT*    M_copy();
extern  MAT*    M_dup();
extern  MAT*    M_inv_1();
extern  MAT*    M_inv_2();
extern  MREAL*   M_alloc_vec();
extern  MREAL*   M_vswp_to_cnv();
extern  VECT    M_vcnv_to_swp();
extern  MREAL    M_det_1();
extern  MREAL    M_det_2();
extern  MAT     *M_solve();
extern  MAT     *M_solve_2();
extern  MAT     *M_trace();
extern  MAT     *M_oper();
extern  MAT     *M_diag();
extern  MAT     *M_scale();
extern  MAT     *M_extr();
extern  MAT     *M_xprimx();
extern  MAT     *M_xxprim();
extern  MAT     *M_xaxprim();
extern  MAT     *M_xpri_max_();

#ifdef __cplusplus
}
#endif

#else

#include <s_proma.h>

#endif

#endif /* _S_SWAP_H_ */

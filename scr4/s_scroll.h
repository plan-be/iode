#ifndef _SCROLL_
#define _SCROLL_

#include <scr.h>

typedef struct _scroll {
    /* El�ments modifiables par l'utilisateur */
    int     sc_ncl;     /* Entier (client) */
    char    *sc_pcl;    /* Pointeur vers une donn�e client */
    U_ch    *sc_cmt;    /* Comment */
    YYKEYS  *sc_scmts;  /* Commentaires structur�s (null term. list)*/
    U_ch    *sc_help;   /* Topic du help */
    int     sc_ipl,     /* Ligne sup�rieure de la fen�tre dans l'�cran */
	    sc_ipc;     /* Colonne gauche de la fen�tre dans l'�cran */
    int     sc_inlc,    /* Nombre de lignes total de la fen�tre */
	    sc_incc;    /* Nombre de colonnes total de la fen�tre */
    int     sc_iattr,   /* Attribut d'affichage */
	    sc_irattr,  /* Attribut invers� (cellule courante) */
	    sc_ilc0,    /* Longueur de la colonne de titres */
	    sc_irlen;   /* Longueur des colonnes en rotate */
    int     sc_norot;   /* Rotate possible ? !!! 0 = oui, 1 = non !!! */
    int     sc_nohsb;   /* Horiz. scroll bar ? !!! 0 = oui, 1 = non !!! */
    int     sc_ir;      /* Indicateur de rotation courante de la fen�tre */
    int     sc_noltit;  /* Pas de titre de lignes */
    int     sc_noctit;  /* Pas de titre de colonnes (2 lignes gagn�es) */


    /* El�ments modifi�s par le programme */
    IMAGE   *sc_im;     /* Image de l'�cran */
    int     sc_icl,     /* ligne courante dans l'�cran */
	    sc_icc;     /* colonne courante dans l'�cran */
    int     sc_icl0,    /* ligne de la premi�re cellule dans l'�cran */
	    sc_icc0;    /* colonne de la premi�re cellule dans l'�cran */
    int     sc_inl,     /* nbre de lignes visibles */
	    sc_inc;     /* nbre de colonnes visibles */
    int     sc_iml,     /* ligne maximum */
	    sc_imc;     /* colonne maximum */
    int     sc_ins;     /* nombre de lignes s�lectionn�e */
    int     *sc_ias;    /* tableau des lignes s�lectionn�es */
    S_BAR   sc_vbar;    /* Scrollbar horizontal */
    S_BAR   sc_hbar;    /* Scrollbar vertical */
    int     sc_max;     /* Scroll Maximized ? */
    int     sc_ocl,     /* Pr�c�dente line courante (old current line) */
	    sc_occ,     /* Pr�c�dente col  courante (old current col ) */
	    sc_onl,     /* Pr�c�dent nb lines (old nl) */
	    sc_onc;     /* Pr�c�dent nb cols (old nc) */

    /* Fonctions � d�finir par l'utilisateur */
#ifdef SCRPROTO
    char    *(*sc_ftitle)(struct _scroll *),           /* Fn retournant le titre g�n�ral */
	    *(*sc_fctitle)(struct _scroll *, int),     /* Fn retournant le titre d'un colonne */
	    *(*sc_fltitle)(struct _scroll *, int),     /* Fn retournant le titre d'une ligne */
	    *(*sc_ftext)(struct _scroll *, int, int);  /* Fn retournant le texte d'une cellule */
    int     (*sc_fnl)(struct _scroll *),               /* Fn retournant le nombre de lignes */
	    (*sc_fnc)(struct _scroll *),               /* Fn retournant le nombre de colonnes */
	    (*sc_flen)(struct _scroll *, int);         /* Fn retournant la largeur d'une col */
    int     (*sc_fedit)(struct _scroll *, int, int),   /* Fn d'�dition d'une cellule */
	    (*sc_fdel)(struct _scroll *, int, int),    /* Fn de destruction d'une cellule */
	    (*sc_fins)(struct _scroll *, int, int);    /* Fn de cr�ation d'une cellule */
    int     (*sc_ffn)(struct _scroll *, int, int, int),/* Fn ex�cut�e pour une touche fonction */
	    (*sc_fbeg)(struct _scroll *, int *, int *),/* Fn de d�but d'�dition */
	    (*sc_fend)(struct _scroll *, int, int),    /* Fn de fin d'�dition */
	    (*sc_fattr)(struct _scroll *, int, int),   /* Attribut (SCR_BLUE, ..) de la cellule *//* JMP 19-11-93 */
	    (*sc_flattr)(struct _scroll *, int),       /* Attribut (SCR_BLUE, ..) du titre ligne*//* JMP 19-11-93 */
	    (*sc_fcattr)(struct _scroll *, int),       /* Attribut (SCR_BLUE, ..) du titre col  *//* JMP 19-11-93 */
	    (*sc_falign)(struct _scroll *, int, int),  /* Alignement (0=left, 1=Center, 2=Right) de la cellule */ /* JMP 02-06-97 */
	    (*sc_flalign)(struct _scroll *, int),      /* Alignement (0=left, 1=Center, 2=Right)(SCR_BLUE, ..) du titre ligne*/ /* JMP 02-06-97 */
	    (*sc_fcalign)(struct _scroll *, int),      /* Alignement (0=left, 1=Center, 2=Right)(SCR_BLUE, ..) du titre col  */ /* JMP 02-06-97 */
	    (*sc_fdisp)(struct _scroll *, int, int);      /* Fn avant display cellule  *//* JMP 19-11-93 */
#else
    char    *(*sc_ftitle)(),    /* Fn retournant le titre g�n�ral */
	    *(*sc_fctitle)(),   /* Fn retournant le titre d'un colonne */
	    *(*sc_fltitle)(),   /* Fn retournant le titre d'une ligne */
	    *(*sc_ftext)();     /* Fn retournant le texte d'une cellule */
    int     (*sc_fnl)(),        /* Fn retournant le nombre de lignes */
	    (*sc_fnc)(),        /* Fn retournant le nombre de colonnes */
	    (*sc_flen)();       /* Fn retournant la largeur d'une col */
    int     (*sc_fedit)(),      /* Fn d'�dition d'une cellule */
	    (*sc_fdel)(),       /* Fn de destruction d'une cellule */
	    (*sc_fins)();       /* Fn de cr�ation d'une cellule */
    int     (*sc_ffn)(),        /* Fn ex�cut�e pour une touche fonction */
	    (*sc_fbeg)(),       /* Fn de d�but d'�dition */
	    (*sc_fend)(),       /* Fn de fin d'�dition */
	    (*sc_fattr)(),      /* Attribut (SCR_BLUE, ..) de la cellule *//* JMP 19-11-93 */
	    (*sc_flattr)(),     /* Attribut (SCR_BLUE, ..) du titre ligne*//* JMP 19-11-93 */
	    (*sc_fcattr)(),     /* Attribut (SCR_BLUE, ..) du titre col  *//* JMP 19-11-93 */
	    (*sc_falign)(),     /* Alignement (0=left, 1=Center, 2=Right) de la cellule */ /* JMP 02-06-97 */
	    (*sc_flalign)(),    /* Alignement (0=left, 1=Center, 2=Right)(SCR_BLUE, ..) du titre ligne*/ /* JMP 02-06-97 */
	    (*sc_fcalign)(),    /* Alignement (0=left, 1=Center, 2=Right)(SCR_BLUE, ..) du titre col  */ /* JMP 02-06-97 */
	    (*sc_fdisp)();      /* Fn avant display cellule  *//* JMP 19-11-93 */
#endif /* SCRPROTO */

    char    reserved[16];       /* reserv� */
} SCROLL;

/*** D�placement de s_scroll.h pour les probl�mes de compilation le 8/12/2011 
#define DATA        (SCRL->sc_pcl)

#define PL          (SCRL->sc_ipl)          // position on the screen 
#define PC          (SCRL->sc_ipc)

#define NLC         (SCRL->sc_inlc)        // nbr of lines & cols 
#define NCC         (SCRL->sc_incc)
#define ATTR        (SCRL->sc_iattr)
#define RATTR       (SCRL->sc_irattr)
#define LC0         (SCRL->sc_ilc0)
#define RLEN        (SCRL->sc_irlen)
#define IR          (SCRL->sc_ir)
#define IM          (SCRL->sc_im)
#define INS         (SCRL->sc_ins)
#define IAS         (SCRL->sc_ias)
#define NOROT       (SCRL->sc_norot)
#define NOHSBAR     (SCRL->sc_nohsb)
#define NOTITL      (SCRL->sc_noltit)
#define NOTITC      (SCRL->sc_noctit)

#define L           (SCRL->sc_icl)
#define C           (SCRL->sc_icc)
#define L0          (SCRL->sc_icl0)
#define C0          (SCRL->sc_icc0)
#define NL          (SCRL->sc_inl)
#define NC          (SCRL->sc_inc)
#define ML          (SCRL->sc_iml)
#define MC          (SCRL->sc_imc)
#define VBAR        (SCRL->sc_vbar)
#define HBAR        (SCRL->sc_hbar)

#define FTITLE       ((*(SCRL->sc_ftitle))(SCRL))
#define FCTITLE(i)   ((*(SCRL->sc_fctitle))(SCRL, i))
#define FLTITLE(i)   ((*(SCRL->sc_fltitle))(SCRL, i))
#define FML          ((*(SCRL->sc_fnl))(SCRL))
#define FMC          ((*(SCRL->sc_fnc))(SCRL))
#define FLTEXT(i,j)  ((*(SCRL->sc_ftext))(SCRL, i, j))
#define FLEN(i)      ((*(SCRL->sc_flen))(SCRL, i))
#define FEDIT(i, j)  ((*(SCRL->sc_fedit))(SCRL, i, j))
#define FDISP(i, j)  ((*(SCRL->sc_fdisp))(SCRL, i, j))
#define FDEL(i, j)   ((*(SCRL->sc_fdel))(SCRL, i, j))
#define FINS(i, j)   ((*(SCRL->sc_fins))(SCRL, i, j))
#define FFN(key, i, j)    ((*(SCRL->sc_ffn))(SCRL, key, i, j))
#define FBEG              (*(SCRL->sc_fbeg))
#define FEND              (*(SCRL->sc_fend))

 FIN D�placement de s_scroll.h pour les probl�mes de compilation le 8/12/2011 ***/

extern SCROLL  *SCRL;
extern int      SCRL_end;

#ifdef SCRPROTO
#include <s_proscl.h>
#endif

#endif

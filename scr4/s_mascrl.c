#include "s_mat.h"
#include "s_strs.h"
#include "s_scroll.h"

typedef struct _scrl_data_
	{
	    MAT *m;
	    char *title;
	    char **tl;
	    char **tc;
	} SCRL_DATA;
SCRL_DATA   m_data;
SCROLL      *old_scroll;
int         M_LC;

/*** Moved from s_scroll.h due to compilation problems 8/12/2011 ***/
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

/*** FIN Déplacement de s_scroll.h pour les problçmes de compilation le 8/12/2011 ***/


/*NH*/
char   *M_sc_title(scrl)
SCROLL  *scrl;
{
    char *ptr;
    ptr = ((SCRL_DATA *) scrl->sc_pcl)->title ;
    return(ptr);
}

/*NH*/
char   *M_sc_ctitle(scrl, j)
SCROLL  *scrl;
int     j;
{
    static char buf[20];
    char **ptr = ((SCRL_DATA *)scrl->sc_pcl)->tc;

    if(ptr != NULL)  return(ptr[j]);
    else {
	sprintf(buf, "%d", j + 1);
	return(buf);
    }
}

/*NH*/
char   *M_sc_ltitle(scrl, i)
SCROLL  *scrl;
int     i;
{
    static char buf[20];
    char **ptr = ((SCRL_DATA *)scrl->sc_pcl)->tl;

    if(ptr != NULL)  return(ptr[i]);
    else {
	sprintf(buf, "%d", i + 1);
	return(buf);
    }
}


/*NH*/
int M_sc_nl(scrl)
SCROLL  *scrl;
{
    int nl;
    nl =  M_NL(((SCRL_DATA *)scrl->sc_pcl)->m) ;
    return( nl);
}

/*NH*/
int M_sc_nc(scrl)
SCROLL  *scrl;
{
    int nc;
    nc = M_NC(((SCRL_DATA *)scrl->sc_pcl)->m) ;
    return( nc);
}


/*NH*/
char   *M_sc_text(scrl, i, j)
SCROLL  *scrl;
int     i, j;
{
    static char buf[100];
    MAT *ptr = ((SCRL_DATA *)scrl->sc_pcl)->m;

    SCR_fmt_dbl((double)MATE(ptr, i, j), buf, 10, -1);
    return(buf);
}


/*NH*/
int M_sc_len(scrl, i)
SCROLL  *scrl;
int     i;
{
    return(M_LC);         /* JMP38 21-09-92 */
}


/*NH*/
M_scroll_init(scrol, fn)
SCROLL *scrol;
int  (*fn)();  /* Fn d'édition d'une cellule */
{
    old_scroll = SCRL;
    SCRL = scrol;
    memset(scrol, 0, sizeof(SCROLL));
    DATA = (char *) &m_data;
    PL   = 3;
    PC   = 5;
    NLC  = 15;
    NCC  = 60;
    ATTR = SCR_DEFAULT;      /*63*/
    RATTR= SCR_REVERSE;      /*12*/
/*    LC0  = 25;             JMP38 21-09-92 */
    RLEN = 25;
    SCRL->sc_ftitle     = M_sc_title;
    SCRL->sc_fctitle    = M_sc_ctitle;
    SCRL->sc_fltitle    = M_sc_ltitle;
    SCRL->sc_fnl        = M_sc_nl;
    SCRL->sc_fnc        = M_sc_nc;
    SCRL->sc_ftext      = M_sc_text;
    SCRL->sc_flen       = M_sc_len;

    SCRL->sc_fedit      = fn;

    SCRL->sc_fdel       = NULL;
    SCRL->sc_fins       = NULL;
    SCRL->sc_ffn        = NULL;
    SCRL->sc_fbeg       = NULL;
    SCRL->sc_fend       = NULL;
    SCRL->sc_norot      = 1;
    NOHSBAR = 1;

    return(0);
}



/* ====================================================================
Visualisation d'une matrice sous forme d'un tableau déroulant.

Le contenu de la matrice est éditable, l'utilisateur doit pour ce faire
définir sa fonction d'édition (voir manuel de la librairie s_scroll).

On passe en paramètre: le pointeur sur la matrice à éditer, un pointeur sur
une structure SCROLL allouée, un pointeur sur le titre (ou NULL si pas de
titre), un pointeur sur un tableau contenant les titres des lignes et
colonnes (ou NULL pour les titres par défaut), et enfin un pointeur sur la
fonction d'édition d'une cellule (ou NULL si non éditable).

Les titres des lignes et colonnes sont passés dans deux tableaux de chaînes
dans lesquels on trouve tous les titres des lignes et tous les titres de
colonnes dans l'ordre, ces tableaux de chaînes doivent être terminés par
NULL.

Les titres par défaut sont les numéros de lignes et de colonnes.

Les valeurs lc0 et lc représentent les largeurs des première et autres
colonnes de la matrice en caractères.

Les touches d'édition suivantes sont définies:

   - ESCAPE : quitte l'édition
   - ENTER  : édition de la cellule courante
   - UP, DOWN : déplace le curseur vers la cellule supérieure ou
     inférieure
   - PGUP, PGDN : déplace le curseur d'un écran vers le haut ou le bas
   - LEFT, RIGHT : déplace le curseur vers la cellule de gauche ou de
     droite
   - TAB et Shift-TAB : déplace le curseur d'un écran vers la droite ou
     la gauche
   - HOME : place le curseur en première colonne de la ligne courante
   - CTRL + HOME : place le curseur en première ligne de la colonne courante
   - END : place le curseur en derniere colonne de la ligne courante
   - CTRL + END : place le curseur en derniere ligne de la colonne courante
   - Alt-R : inverse le tableau
   - Alt-M : Déplace le tableau dans les limites de l'écran suivant
     les touches curseur utilisées ensuite. L'édition reprend après que
     ENTER ait été pressé.
   - Alt-Z : Change la taille du tableau dans les limites de l'écran
     suivant les touches curseur utilisées ensuite. L'édition reprend
     après que ENTER ait été pressé.
   - Lettre ou chiffre : positionne le curseur sur la prochaine ligne
     dont le titre commence par le caractère pressé.
   - touche globale de l'application (Alt-X par exemple) : quitte
     l'édition et exécute la fonction associée.


ATTENTION: si la fonction M_scroll est utilisée en dehors d'un environnement
SCR, il faut :
    -lier le programme avec scr4m.lib et s_strs.lib
    -initialiser le programme avec SCR_init() et le terminer avec SCR_end().
Il faut de plus lier le programme avec s_scroll.lib et inclure s_scroll.h.


&RT
néant

&EX

    #include "s_mat.h"
    #include <s_scroll.h>

    main(argc, argv)
    int     argc;
    char    *argv[];
    {

	int          i, j, dim;
	int nl1, nc1, nl2, nc2, nl3, nc3,NL5, NC5;
	MAT       *m1,  *m4=NULL;
	FILE *fd;

	char *ligne[] = {"ligne1", "ligne2",NULL};
	char *col[] = {"col1", "col2", NULL};
	nl1= 2, nc1 = 2;
	fd = fopen("test.a2m", "w+b");

	SCR_init();
	SW_init(1);

	m1 = M_alloc(nl1, nc1);
	MATE1(0,0) = 3.0;
	MATE1(0,1) = 2.0;
	MATE1(1,0) = -5.0;
	MATE1(1,1) = -8.0;
	m4 = M_inv_1(m4, m1);

	M_scroll(m4, "Matrice 4", ligne, col, NULL);
	M_tprintf(fd, m4, "Matrice 4", ligne, col);
	M_dump(stdout, m4, "Inverse de Matrice 1");

	M_free(m1);
	M_free(m4);

	SW_end();
	SCR_end();
	return (0);
    }


    SCR_app_key()
    {return(0);}

&TX
&SA M_dump(), M_printf(), M_tprinf(), M_scroll()

======================================================================= */


M_scroll(m1, title, ltitle, ctitle, fn, lc0, lc)
MAT *m1;             /* matrice à éditer */
char *title;         /* titre général */
char **ltitle;       /* tableau des titres de lignes ou NULL */
char **ctitle;       /* tableau des titres colonnes ou NULL */
int  (*fn)();        /* Fn d'édition d'une cellule */
int  lc0, lc;        /* Largeur de la colonne 1 et des autres  JMP38 21-09-92 */
{
    SCROLL *scrol;        /* pointeur vers une structure scrol allouée */

    if(m1 == NULL) return(-1);
    scrol = (SCROLL *) SCR_malloc(sizeof(SCROLL));
    m_data.m = m1;
    m_data.title = title;
    if (ltitle != NULL)
    {
	if(SCR_tbl_size((U_ch **)ltitle) != NL1) m_data.tl = NULL;
	else m_data.tl = ltitle;
    }
    else m_data.tl = NULL;

    if (ctitle != NULL)
    {
	if(SCR_tbl_size((U_ch **)ctitle) != NC1) m_data.tc = NULL;
	else m_data.tc = ctitle;
    }
    else m_data.tc = NULL;

    M_scroll_init(scrol, fn);
    LC0 = lc0;
    M_LC = lc;
    NCC = (72 - lc0) /  (1 + lc);     /* JMP38 21-09-92 */
    NCC = _min_(NC1, NCC);              /* JMP38 21-09-92 */
    NCC = lc0 + 4 + NCC * (lc + 1);   /* JMP38 21-09-92 */
    SCRL_edit(scrol);
    SCRL = old_scroll;
    SCR_free(scrol);
    return(0);
}













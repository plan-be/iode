#include "scr.h"

#include "scr4_box.h"

#define SCR_graphic_char(u, r, d, l) (SCR_GRAPHIC_CHARS[u][r][d][l])
/* BP_M 11-06-1999 */
/*
#define SCR_is_graph(ch)             ((ch) > 178 && (ch) < 219)
*/
#define LEFT    0
#define RIGHT   1
#define UPPER   2
#define DOWN    3

unsigned char SCR_PINS[4][40] = {

    0, 1, 2, 1, 1, 2, 2, 0, 2, 2, 1,
       2, 1, 0, 1, 1, 0, 1, 1, 0, 0,
       0, 0, 2, 2, 0, 2, 2, 2, 1, 2,
       1, 0, 0, 0, 0, 1, 2, 1, 0,

    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 1, 1, 1, 1, 1, 1, 2, 1,
       2, 2, 2, 2, 2, 2, 2, 2, 1, 2,
       1, 1, 2, 2, 1, 1, 2, 0, 1,

    1, 1, 1, 2, 0, 0, 2, 2, 0, 2, 2,
       1, 0, 1, 1, 0, 1, 0, 1, 1, 2,
       2, 0, 2, 0, 2, 0, 2, 1, 2, 0,
       0, 2, 1, 0, 0, 2, 1, 1, 0,

    1, 1, 1, 2, 2, 1, 2, 2, 2, 0, 0,
       0, 1, 0, 0, 1, 1, 0, 1, 1, 2,
       0, 2, 0, 2, 2, 0, 2, 0, 0, 1,
       2, 0, 0, 1, 2, 2, 1, 0, 1
};

unsigned char SCR_GRAPHIC_CHARS[3][3][3][3] = {
					     /* [upper][right][lower][left] */
    0  ,        0         , 0         ,      /* [0]    [0]    [0] */
    0  ,        SCR_T1_R1 , SCR_T2_R1 ,      /* [0]    [0]    [1] */
    0  ,        SCR_T1_R2 , SCR_T2_R2 ,      /* [0]    [0]    [2] */

    0  ,        SCR_H_BAR1, SCR_H_BAR1,      /* [0]    [1]    [0] */
    SCR_T1_L1,  SCR_T1_C1 , SCR_T1_C1 ,      /* [0]    [1]    [1] */
    SCR_T1_L2,  SCR_T1_C2 , SCR_T1_C2 ,      /* [0]    [1]    [2] */

    0  ,        SCR_H_BAR1, SCR_H_BAR2,      /* [0]    [2]    [0] */
    SCR_T2_L1,  SCR_T2_C1 , SCR_T2_C1 ,      /* [0]    [2]    [1] */
    SCR_T2_L2,  SCR_T2_C2 , SCR_T2_C2 ,      /* [0]    [2]    [2] */



    0  ,        SCR_B1_R1 , SCR_B2_R1 ,      /* [1]    [0]    [0] */
    SCR_V_BAR1, SCR_M1_R1 , SCR_M2_R1 ,      /* [1]    [0]    [1] */
    SCR_V_BAR1, SCR_M1_R2 , SCR_M2_R2 ,      /* [1]    [0]    [2] */

    SCR_B1_L1,  SCR_B1_C1 , SCR_B1_C1 ,      /* [1]    [1]    [0] */
    SCR_M1_L1,  SCR_M1_C1 , SCR_M1_C1 ,      /* [1]    [1]    [1] */
    SCR_M1_L1,  SCR_M1_C1 , SCR_M1_C1 ,      /* [1]    [1]    [2] */

    SCR_B2_L1,  SCR_B2_C1 , SCR_B2_C1 ,      /* [1]    [2]    [0] */
    SCR_M2_L1,  SCR_M2_C1 , SCR_M2_C1 ,      /* [1]    [2]    [1] */
    SCR_M2_L2,  SCR_M2_C2 , SCR_M2_C2 ,      /* [1]    [2]    [2] */


    0  ,        SCR_B1_R2 , SCR_B2_R2 ,      /* [2]    [0]    [0] */
    SCR_V_BAR2, SCR_M1_R2 , SCR_M2_R2 ,      /* [2]    [0]    [1] */
    SCR_V_BAR2, SCR_M1_R2 , SCR_M2_R2 ,      /* [2]    [0]    [2] */

    SCR_B1_L2,  SCR_B1_C2 , SCR_B1_C2 ,      /* [2]    [1]    [0] */
    SCR_M1_L2,  SCR_M1_C1 , SCR_M1_C1 ,      /* [2]    [1]    [1] */
    SCR_M1_L2,  SCR_M1_C2 , SCR_M1_C2 ,      /* [2]    [1]    [2] */

    SCR_B2_L2,  SCR_B2_C2 , SCR_B2_C2 ,      /* [2]    [2]    [0] */
    SCR_M2_L2,  SCR_M2_C2 , SCR_M2_C2 ,      /* [2]    [2]    [1] */
    SCR_M2_L2,  SCR_M2_C2 , SCR_M2_C2        /* [2]    [2]    [2] */

};

unsigned char SCR_HLINE_CHS[] = {' ', SCR_H_BAR1, SCR_H_BAR2};
unsigned char SCR_VLINE_CHS[] = {' ', SCR_V_BAR1, SCR_V_BAR2};

/* ====================================================================
Dessine une ligne horizontale de poids 1, 2 ou 3 (blanc) à partir de la
position (line, col) pendant nc colonnes.
=======================================================================*/

SCR_hline(type, line, col, nc)
int     type;
int     line;
int     col;
int     nc;
{
    unsigned char   ch;

    if((type < 1) || (type > 3)) return(0);
    if(type == 3) type = 0;
    return(SCR_fill(SCR_HLINE_CHS[type], line, col, 1, nc));
}
/* ====================================================================
Dessine une ligne verticale de poids 1, 2 ou 3 (blanc) à partir de la
position (line, col) pendant nl lignes.
=======================================================================*/

SCR_vline(type, line, col, nl)
int     type;
int     line;
int     col;
int     nl;
{
    unsigned char   ch;

    if((type < 1) || (type > 3)) return(0);
    if(type == 3) type = 0;
    return(SCR_fill(SCR_VLINE_CHS[type], line, col, nl, 1));
}

unsigned char SCR_BOX_CHARS[3][11] = {
	       /*  width 0 (blanc in reverse video)  */
			' ', ' ', ' ',          /* top left, center, right */
			' ', ' ', ' ',          /* mid left, center, right */
			' ', ' ', ' ',          /* bot.left, center, right */
			' ', ' ',               /* vert. bar, horiz. bar   */
	       /*  width 1  */
			SCR_T1_L1, SCR_T1_C1, SCR_T1_R1, /* top left, center, right */
			SCR_M1_L1, SCR_M1_C1, SCR_M1_R1, /* mid left, center, right */
			SCR_B1_L1, SCR_B1_C1, SCR_B1_R1, /* bot.left, center, right */
			SCR_V_BAR1, SCR_H_BAR1, /* vert. bar, horiz. bar   */
	       /*  width 2  */
			SCR_T2_L2, SCR_T2_C2, SCR_T2_R2, /* top left, center, right */
			SCR_M2_L2, SCR_M2_C2, SCR_M2_R2, /* mid left, center, right */
			SCR_B2_L2, SCR_B2_C2, SCR_B2_R2, /* bot.left, center, right */
			SCR_V_BAR2, SCR_H_BAR2  /* vert. bar, horiz. bar   */
		} ;

/*NH*/
/* =+====== SCR_corner ====================================
     Draw a corner
     type   : IN : type de box (0, 1, 2)
     line   : IN : line (index 0)
     column : IN : column(index 0)
     corner : IN : corner position :
		    0 = top left    1 = top center      2 = top right
		    3 = left center 4 = center          5 = right center
		    6 = bottom left 7 = bottom center   8 = left bottom
		    9 = vertical bar    10 = horizontal bar
     return : error code
   ============================================================== */

SCR_corner(type, line, col, corner)
int     type;
int     line;
int     col;
int     corner;
{
    unsigned char   ch;

    if((type < 1) || (type > 3)) return(0);
    if(type == 3) type = 0;

    SCR_write_xattrs(line, col, 0, 1);
    return(SCR_write(line, col, &SCR_BOX_CHARS[type][corner], 1));
}

/* ====================================================================
Dessine un rectangle de poids 1, 2 ou 3 (blanc) à partir de la position
(line, col) et de dimension (nl, nc).
=======================================================================*/

SCR_box(type, line, col, nl, nc)
int     type;
int     line;
int     col;
int     nl;
int     nc;
{
    int     attr;

    if(type == 3) {
	attr = SCR_CURRENT_ATTR;
	SCR_set_attr(SCR_invert_attr(attr));
	}

    SCR_hline(type, line,      col+1,    nc-2);
    SCR_hline(type, line+nl-1, col+1,    nc-2);
    SCR_vline(type, line+1,    col,      nl-2);
    SCR_vline(type, line+1,    col+nc-1, nl-2);

    SCR_corner(type, line,      col,      0);
    SCR_corner(type, line,      col+nc-1, 2);
    SCR_corner(type, line+nl-1, col,      6);
    SCR_corner(type, line+nl-1, col+nc-1, 8);

    if(type == 0) SCR_set_attr(attr);
    return(0);
}
/* ====================================================================
Remplit un rectangle avec le caractère ch à partir de la position (line,
col) et de dimension (nl, nc).
=======================================================================*/

SCR_fill(ch, line, col, nl, nc)
int     ch;
int     line;
int     col;
int     nl;
int     nc;
{
    int         i;
    char        buf[1024];      /* JMP 08-01-11 */

    if(nc < 0) return(0);
    if(nc > 1024) nc = 1024;    /* JMP 08-01-11 */
    memset(buf, ch, nc);
    for(i = 0 ; i < nl ; i++) {
	SCR_write_xattrs(line+i, col, 0, nc);
	SCR_write(line+i, col, buf, nc);
	    }
    return(0);
}

/*NH*/
SCR_nb_pins(ch, dir)
int     ch, dir;
{
    if(!SCR_is_graph(ch)) return(0);
    return(SCR_PINS[dir][ch - 179]);
}

/* ====================================================================
Balaie une partie d'écran en tenant de remplacer les caractères
semi-graphiques de définition de lignes par des coins correspondant aux
croisements.

La partie d'écran traitée est un rectangle qui commence en position
(line, col) et possède une taille de (nl x nc) caractères.
=======================================================================*/

SCR_auto_table(line, col, nl, nc)
int line;
int col;
int nl;
int nc;
{
    int     attr;
    int     pos;
    unsigned char    ch, nch;
    int     i, j;
    int     lline, lcol;
    int     upper, lower, right, left;
    int     max_col;
    unsigned char    *screen_mem;

    lline = nl + line;
    lcol  = nc + col;

    line   = max(0, min(line,  SCR_PAGE_SIZE[0] - 1));
    lline  = max(0, min(lline, SCR_PAGE_SIZE[0] ));
    col    = max(0, min(col,   SCR_PAGE_SIZE[1] - 1));
    lcol   = max(0, min(lcol,  SCR_PAGE_SIZE[1]));

    if(SCR_TMP_IMAGE != 0) {
	max_col    = SCR_TMP_IMAGE->im_nb_column;
	screen_mem = SCR_TMP_IMAGE->im_cells;
    }
    else {
	if(SCR_SCREEN == 0) return(0); /* JMP 13-02-2004 */
	max_col    = SCR_PAGE_SIZE[1];
	screen_mem = SCR_SCREEN->im_cells;
    }

    for(i = line ; i < lline ; i++)
	for(j = col ; j < lcol ; j++) {
	    if(SCR_TMP_IMAGE != 0)
		pos = 2 * ((i - SCR_TMP_IMAGE->im_line) * max_col + j - SCR_TMP_IMAGE->im_column);
	    else
		pos = 2 * (j + i * max_col);
	    ch  = screen_mem[pos];
	    if(SCR_is_graph(ch)) {
		upper = lower = left = right = 0;
		if(i != line)
		    upper = SCR_nb_pins(screen_mem[pos - 2 * max_col], DOWN);
		if(i != lline - 1)
		    lower = SCR_nb_pins(screen_mem[pos + 2 * max_col], UPPER);
		if(j != col)
		    left = SCR_nb_pins(screen_mem[pos - 2], RIGHT);
		if(j != lcol - 1)
		    right = SCR_nb_pins(screen_mem[pos + 2], LEFT);
		nch = SCR_graphic_char(upper, right, lower, left);
		if(nch != 0 && nch != ch) {
		    attr = screen_mem[1 + pos];
		    SCR_set_attr(attr);
		    SCR_write_xattrs(i, j, 0, 1);
		    SCR_write(i, j, &nch, 1);
		    }
		}
	    }

    return(0);
}

/*NH*/
/* ===================================================================
    Retourne le caractère semi-graphique ch modifié en fonction de ses
    voisins. Si ch n'est pas un caractère semi-graphique, il n'est pas
    modifié. S'il est semi-graphique, il est calculé en fonction des 4
    caractères l, r, u et d qui l'entourent.

====================================================================== */

SCR_auto_char(ch, l, r, u, d)
int     ch, l, r, u, d;
{
    unsigned char    nch;

    if(!SCR_is_graph(ch)) return(ch);
    u = SCR_nb_pins(u, DOWN);
    d = SCR_nb_pins(d, UPPER);
    l = SCR_nb_pins(l, RIGHT);
    r = SCR_nb_pins(r, LEFT);

    return(SCR_graphic_char(u, r, d, l));
}

/* SHADOWING OF WINDOWS */

/* unsigned char SCR_SHADOWS[3] = {'\xDB', '\xDF', '\xDC'}; */
unsigned char SCR_SHADOWS[3] = {219, 223, 220};

/* ====================================================================
Réalise des ombrages autour de rectangles dans l'écran.

Les arguments sont :

&EN type vaut entre 0 et 4.
&EN orient : orientation de l'ombrage (inactif)
&EN line et col : coin supérieur gauche du rectangle
&EN nl et nc : taille du rectangle
&EN attr : attribut de l'ombrage
=======================================================================*/

SCR_shadow(type, orient, line, col, nl, nc, attr)
int     type, orient, line, col, nc, nl, attr;
{
    int     oattr = SCR_CURRENT_ATTR;

    switch(type) {
	case 0 : SCR_shadow_0(orient, line, col, nl, nc, attr); break;
	case 1 : SCR_shadow_1(orient, line, col, nl, nc, attr); break;
	case 2 : SCR_shadow_2(orient, line, col, nl, nc, attr); break;
	case 3 : SCR_shadow_3(orient, line, col, nl, nc, attr); break;
	default: SCR_shadow_4(orient, line, col, nl, nc, attr); break;
    }
    SCR_set_attr(oattr);
    return(0);
}

/*NH*/
SCR_shadow_0(orient, line, col, nl, nc, attr)
int     orient, line, col, nc, nl, attr;
{
    SCR_set_attr(attr);
    SCR_fill(SCR_SHADOWS[1], line + nl, col + 1, 1, nc);
    SCR_fill(SCR_SHADOWS[2], line, col + nc, 1, 1);
    SCR_fill(SCR_SHADOWS[0], line + 1, col + nc, nl - 1, 1);
/*  SCR_fill(SCR_SHADOWS[2], line + nl, col + nc, 1, 1); */
    return(0);
}

/*NH*/
SCR_shadow_1(orient, line, col, nl, nc, attr)
int     orient, line, col, nc, nl, attr;
{
    SCR_chg_im_attr(line + nl, col + 1, 1, nc, attr);
    SCR_chg_im_attr(line + 1, col + nc, nl, 2, attr);
    return(0);
}

/*NH*/
SCR_chg_im_attr(line, col, nl, nc, attr)
int     line, col, nl, nc, attr;
{
    IMAGE   *im;

    im = SCR_save_image(line, col, nl, nc);
    if(im == 0) return(0);
    SCR_fix_im_attr(im, attr);
    SCR_display_image(im);
    SCR_free_image(im);
    return(0);
}

/*NH*/
SCR_fix_im_attr(im, attr)
IMAGE           *im;
unsigned int    attr;
{
    int     i, lg;

    lg = 2 * im->im_nb_line * im->im_nb_column;

    for(i = 1  ; i < lg ; i += 2)
	im->im_cells[i] = attr;

    return(0);
}

/*NH*/
SCR_shadow_2(orient, line, col, nl, nc, attr)
int     orient, line, col, nc, nl, attr;
{
    SCR_set_attr(attr);
    SCR_fill(' ', line + 1, col + nc, nl - 1, 1);
    SCR_fill(' ', line + nl, col + 1, 1, nc);
    return(0);
}

/*NH*/
SCR_shadow_3(orient, line, col, nl, nc, attr)
int     orient, line, col, nc, nl, attr;
{
    SCR_set_attr(attr);
    SCR_fill(' ', line + 1, col + nc, nl - 1, 1);
    SCR_fill(' ', line + nl, col + 1, 1, nc + 1);
    return(0);
}

/*NH*/
SCR_shadow_4(orient, line, col, nl, nc, attr)
int     orient, line, col, nc, nl, attr;
{
    SCR_set_attr(SCR_DEFAULT);
    SCR_fill(SCR_SPEC_1, line + 1, col + nc, nl - 1, 2);
    SCR_fill(SCR_SPEC_1, line + nl, col + 1, 1, nc + 1);
    return(0);
}


/*
SCR_nb_upper_pins(ch)
unsigned char    ch;
{
    if(!SCR_is_graph(ch)) return(0);
    return((int)(0377 & SCR_UPPER_PINS[ch - 179]));
}
SCR_nb_lower_pins(ch)
unsigned char    ch;
{
    if(!SCR_is_graph(ch)) return(0);
    return((int)(0377 & SCR_LOWER_PINS[ch - 179]));
}

SCR_nb_left_pins(ch)
unsigned char    ch;
{
    if(!SCR_is_graph(ch)) return(0);
    return((int)(0377 & SCR_LEFT_PINS[ch - 179]));
}
SCR_nb_right_pins(ch)
unsigned char    ch;
{
    if(!SCR_is_graph(ch)) return(0);
    return((int)(0377 & SCR_RIGHT_PINS[ch - 179]));
}
*/

/*
unsigned char SCR_LEFT_PINS[40] = {
    0, 1, 2, 1, 1, 2, 2, 0, 2, 2, 1,
       2, 1, 0, 1, 1, 0, 1, 1, 0, 0,
       0, 0, 2, 2, 0, 2, 2, 2, 1, 2,
       1, 0, 0, 0, 0, 1, 2, 1, 0 };

unsigned char SCR_RIGHT_PINS[40] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 1, 1, 1, 1, 1, 1, 2, 1,
       2, 2, 2, 2, 2, 2, 2, 2, 1, 2,
       1, 1, 2, 2, 1, 1, 2, 0, 1 };

unsigned char SCR_UPPER_PINS[40] = {
    1, 1, 1, 2, 0, 0, 2, 2, 0, 2, 2,
       1, 0, 1, 1, 0, 1, 0, 1, 1, 2,
       2, 0, 2, 0, 2, 0, 2, 1, 2, 0,
       0, 2, 1, 0, 0, 2, 1, 1, 0 };

unsigned char SCR_LOWER_PINS[40] = {
    1, 1, 1, 2, 2, 1, 2, 2, 2, 0, 0,
       0, 1, 0, 0, 1, 1, 0, 1, 1, 2,
       0, 2, 0, 2, 2, 0, 2, 0, 0, 1,
       2, 0, 0, 1, 2, 2, 1, 0, 1 };
*/

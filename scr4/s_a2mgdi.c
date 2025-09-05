#include "scr4.h"
#ifndef PRWINDOWS

/*NH*/
A2mToGdiPrinter()
{
    return(-1);
}

#else

#include <scr4w.h>
#include "s_a2m.h"

/* Variables générales */
int     A2M_GDI_CURX, A2M_GDI_CURY, A2M_GDI_H, A2M_GDI_W;
int     A2M_GDI_TWIDTH_FIXED = 0; /* JMP 21-03-05 */
int     A2M_GDI_CURPAGE, A2M_GDI_MARGX, A2M_GDI_MARGY, A2M_GDI_MARGX2, A2M_GDI_MARGY2;
int     A2M_GDI_GCOLOR;
A2MOBJ  **A2M_OBJS;
int     A2M_NBOBJS = 0;
extern int A2M_CUROBJ;

/******** Paramètres *************/
int A2M_GDI_COLOR = 1, A2M_GDI_TBORDER = 2;
int A2M_GDI_LMARG = 12;
int A2M_GDI_RMARG = 12;
int A2M_GDI_TMARG = 12;
int A2M_GDI_BMARG = 12;


/*NH*/
int A2mGdiPrinterInit(HWND hWndOwner, int Dlg, U_ch* JobTitle)
{
    if(JobTitle == 0) JobTitle = "A2M";
    if(WprPrinterInit(hWndOwner, Dlg, JobTitle)) return(-1);

    A2mGdiAllInit();
    return(0);
}

/*NH*/
A2mGdiAllInit()
{
    A2M_GDI_MARGX = WprLOGX / 2;             /* 0.5" marges gauche et droite */
    A2M_GDI_MARGY = WprLOGY / 4;             /* 0.25" marges haut et bas */
    A2M_GDI_W = WprHORZRES - WprLOGX;        /* 0.5" marges gauche et droite */
    A2M_GDI_H = WprVERTRES - 3 * A2M_GDI_MARGY; /* 0.25" marges haut, 0.5" bas */

    A2M_GDI_CURX = 0;
    A2M_GDI_CURY = 0;
    A2M_GDI_CURPAGE = 1;
    A2M_CUROBJ = 1;
    return(0);
}

/*NH*/
A2mGdiPrinterEnd()
{
    WprPrinterEnd();
    A2mGdiAllEnd();
    return(0);
}

/*NH*/
A2mGdiAllEnd()
{
    SCR_free(A2M_OBJS);
    A2M_OBJS = 0;
    A2M_NBOBJS = 0;
    return(0);
}

/* ================================================================
Interprète le contenu d'un fichier a2m et l'imprime sur une imprimante
définie dans le système Win32.

&EN hWndOwner = fenêtre propriétaire de la tâche
&EN Dlg = 0 pour ne pas afficher la boîte de dialogue de configuration
    de l'imprimante, 1 pour l'afficher. Dans ce dernier cas, l'impression
    peut se faire sur une imprimante spécifique.
&EN JobTitle = nom du job
&EN a2mfile = nom du fichier a2m à imprimer

&RT La fonction retourne 0 si le processus s'est bien déroulé, -1 sinon.
    Si le job est annulé par l'utilisateur (Dlg == 1 et Cancel), la
    fonction retourne aussi -1.

La syntaxe des fichiers a2m est décrite dans un chapitre spécifique.

&TI Fichier a2m.ini
-------------------
Le fichier a2m.ini (ou un autre) contient des paramètres pour
l'impression et la lecture du fichier a2m. Pour que ces paramètres
soient pris en compte par le programme A2mToGdiPrinter(), il faut
appeler la fonction A2mGdiReadIni(filename) avant de lancer la fonction
d'interprétation et d'impression.

&CO
    #include <s_a2m.h>

    A2mGdiReadIni("myfile.ini");
    A2mToGdiPrinter(0L, 1, "Job Title", "myfile.a2m");
&TX

&TI Variables globales
----------------------
Les variables globales décrites dans le fichier .ini peuvent également
être modifiées dans le programme.

&IT Variables influençant la lecture du fichier
-----------------------------------------------
Ces variables sont définies dans la section [A2M] du fichier ini.

&EN int A2M_ESCCH = caractère d'escape (enrichissements et caractères
    spéciaux) : '\'par défaut
&EN int A2M_CMDCH = caractère de commande ('.' par défaut)
    spéciaux)
&EN int A2M_DEFCH = caractère préfixant les macros ('&' par défaut)
&EN int A2M_SEPCH = caractère de séparation des cellules ('&' par défaut)
&EN int A2M_LFON = conserve les linefeed (1) ou non (0)
&EN int A2M_BLON = conserve les blancs (1) ou non (0)
&EN char A2M_CURTAG[41] = paragraphe par défaut ("par_0")

&IT Variables influençant l'impression
--------------------------------------
Ces variables sont définies dans la section [GDI] du fichier .ini.

&EN int A2M_FONTSIZE = taille par défaut des caractères dans les
    paragraphes (10 pts par défaut)
&EN int A2M_FONTFAMILY = police de caractère par défaut ('H', 'T' ou 'C')
&EN int A2M_FONTINCR = incrément de taille de caractères (2 par défaut)
&EN int A2M_TFONTSIZE = taille par défaut des caractères dans les
    tableaux (8 pts par défaut)
&EN U_ch *A2M_PGHEAD = titre de page ("" par défaut)
&EN U_ch *A2M_PGFOOT = pied de page ("" par défaut)
&EN int A2M_TSHADING_COL[2] = couleurs de la brosse de hachurage des
    titres([0]) et corps([1]) des tableaux
&EN2 0 = noir
&EN2 1 = blanc
&EN2 2 = rouge
&EN2 3 = vert
&EN2 4 = bleu
&EN2 5 = cyan
&EN2 6 = magenta
&EN2 7 = jaune

&EN int A2M_TSHADING_BRUSH[2] = brosse de hachurage des titres([0]) et
    corps([1]) des tableaux
&EN2 7 = 50%
&EN2 6 = 25%
&EN2 5 = 12%
&EN2 4 = 6%
&EN2 3 = 3%
&EN2 0-2 = 0%

&EN int A2M_GDI_COLOR = 1 (dft) pour impression en couleur, 0 en B/W
&EN int A2M_GDI_TBORDER = épaisseur des cadres des tableaux (2 par dft)
&EN int A2M_GDI_LMARG = marge de gauche de la page (12 pts par dft)
&EN int A2M_GDI_RMARG = marge de droite de la page (12 pts par dft)
&EN int A2M_GDI_TMARG = marge de haut de la page (12 pts par dft)
&EN int A2M_GDI_BMARG = marge de bas de la page (12 pts par dft)

&RT
&EN 0 en cas de succès
&EN -1 si le fichier ne peut être ouvert
&EN -2 si l'imprimante n'a pû être initialisée ou si l'utilisateur
    a pressé sur Cancel dans le panneau d'impression

&SA A2mGdiReadIni(), A2mToGdiEMF, A2mToRtf(), A2mToMif(),
    A2mToHtml(), A2mPrintError()
==================================================================== */

int A2mToGdiPrinter(HWND hWndOwner, int Dlg, U_ch* JobTitle, U_ch* a2mfile)
{
    A2MFILE *af;
    A2MOBJ  *ao;

    af = A2mOpen(a2mfile, 0);
    if(af == 0) return(-1);

    if(JobTitle == 0) JobTitle = a2mfile;
    if(A2mGdiPrinterInit(hWndOwner, Dlg, JobTitle)) {
	A2mClose(af);
	return(-2);
	}

    while(1) {
	ao = A2mRead(af);
	if(ao == 0) break;
	A2mGdiPrintObj(ao);
	}

    A2mGdiPrintObj((A2MOBJ *)0);
    A2mGdiPrinterEnd();
    A2mClose(af);
    return(0);
}

/*NH*/
A2mGdiPrintObj(A2MOBJ *ao)
{
    int     i, h, toth;
    char    msg[80];

    for(i = h = 0 ; i < A2M_NBOBJS ; i++)
	h += A2M_OBJS[i]->ao_h + A2M_OBJS[i]->ao_spacea +
				 A2M_OBJS[i]->ao_spaceb;
    if(ao) {
	A2mCalcObjSize(ao, A2M_GDI_W);
	toth = h + ao->ao_h + ao->ao_spaceb;
	if(ao->ao_kwn && (!ao->ao_newpage) &&
			    toth + ao->ao_spacea < A2M_GDI_H) {
	    A2M_OBJS = (A2MOBJ **)
		    SCR_realloc(A2M_OBJS, sizeof(A2MOBJ *),
				A2M_NBOBJS, A2M_NBOBJS + 1);
	    A2M_OBJS[A2M_NBOBJS] = ao;
	    A2M_NBOBJS++;
	    return(0);
	    }
	if(toth + A2M_GDI_CURY > A2M_GDI_H) A2mFormFeed();
	}

    for(i = 0 ; i < A2M_NBOBJS ; i++) {
	sprintf(msg, "Printing Page %d - Object %d",
			    A2M_GDI_CURPAGE, A2M_CUROBJ++);
	A2mMessage(msg);
	A2mGdiWriteObj(A2M_OBJS[i]);
	A2mFreeObj(A2M_OBJS[i]);
	A2M_OBJS[i] = 0;
	}
    A2M_NBOBJS = 0;

    if(ao) {
	if(ao->ao_newpage) A2mFormFeed(); /* JMP 08-09-97 */
	A2mGdiWriteObj(ao);
	A2mFreeObj(ao);
	}
    else { /* Fin de l'impression */
	A2mPageHead();
	A2mPageFoot();
	}
    return(0);
}

/*NH*/
A2mCalcObjSize(A2MOBJ *ao, int  w)
{
    A2MPAR  *ap;
    A2MTBL  *at;
    A2MGRF  *ag;

    switch(ao->ao_type) {
	case A2M_PAGE :
	    ao->ao_newpage = 1; /* JMP 08-09-97 */
	    break;

	case A2M_PAR :
	    ap = (A2MPAR *)ao->ao_ptr;
	    A2mGdiPrepPar1(ap);
	    A2mGdiPrepPar2(ap, w);
	    ao->ao_spaceb = ap->ap_spaceb;
	    ao->ao_spacea = ap->ap_spacea;
	    ao->ao_w = ap->ap_w;
	    ao->ao_h = ap->ap_h;
	    ao->ao_h1 = ap->ap_h1;
	    ao->ao_kwn = ap->ap_ppr.pp_keepwn;
	    ao->ao_newpage = ap->ap_ppr.pp_newpage;  /* JMP 08-09-97 */
	    break;

	case A2M_TBL :
	    at = (A2MTBL *)ao->ao_ptr;
	    A2mGdiPrepTbl1(at);
	    A2mGdiPrepTblWs(at, w);
	    A2mGdiPrepTbl2(at);
	    A2mGdiCalcTblH(at);

	    if(at->at_break)
		ao->ao_h = A2mGdiSubTblH(at, 1, at->at_nc - 1, 0) +
			   A2mGdiSubTblH(at, 1, at->at_nc - 1, 1);
	    else
		ao->ao_h = at->at_h;

	    ao->ao_h1 = 0;
	    ao->ao_spaceb = at->at_spaceb;
	    ao->ao_spacea = at->at_spacea;
	    ao->ao_newpage = at->at_newpage;  /* JMP 08-09-97 */
	    break;

	case A2M_GRF :
	    ag = (A2MGRF *)ao->ao_ptr;
	    A2mGdiPrepGrf(ag);
	    ao->ao_w = ag->ag_w;
	    ao->ao_h = ag->ag_h;
	    ao->ao_spaceb = ag->ag_spaceb;
	    ao->ao_spacea = ag->ag_spacea;
	    if(ag->ag_title) {
		ao->ao_h += ag->ag_title->ap_h + ag->ag_title->ap_spacea;
		ao->ao_spaceb = ag->ag_title->ap_spaceb;
		}
	    if(ag->ag_footnote) {
		ao->ao_h += ag->ag_footnote->ap_h + ag->ag_footnote->ap_spaceb;
		ao->ao_spacea = ag->ag_footnote->ap_spacea;
		}

	    ao->ao_newpage = ag->ag_newpage;  /* JMP 08-09-97 */
	    break;
	default : break;
	}
    return(0);
}

/*NH*/
A2mGdiWriteObj(A2MOBJ *ao)
{
    A2MPAR  *ap;
    A2MTBL  *at;
    A2MGRF  *ag;
    A2MLINE *al;
    A2MGOTO *ago;
    A2MMARGINS am;

    if(ao->ao_h + ao->ao_spaceb + A2M_GDI_CURY > A2M_GDI_H)
	A2mFormFeed();
    if(A2M_GDI_CURY) A2M_GDI_CURY += ao->ao_spaceb;

    switch(ao->ao_type) {
	case A2M_PAR :
	    ap = (A2MPAR *)ao->ao_ptr;
	    A2mGdiWritePar(ap, 0, A2M_GDI_CURY, A2M_GDI_W, ap->ap_ppr.pp_just);
	    A2M_GDI_CURY += ao->ao_h + ao->ao_spacea;
	    break;
	case A2M_TBL :
	    at = (A2MTBL *)ao->ao_ptr;
	    A2mGdiWriteTbl(at);
	    A2M_GDI_CURY += ao->ao_spacea;
	    break;
	case A2M_GRF :
	    ag = (A2MGRF *)ao->ao_ptr;
	    A2mGdiWriteGrf(ag);
	    A2M_GDI_CURY += ao->ao_h + ao->ao_spacea;
	    break;
	case A2M_LINE :
	    al = (A2MLINE *)ao->ao_ptr;
	    A2mGdiWriteLine(al);
	    A2M_GDI_CURY += ao->ao_h + ao->ao_spacea;
	    break;
	case A2M_GOTO :
	    ago = (A2MGOTO *)ao->ao_ptr;
	    am.am_left = ago->ag_x;
	    am.am_right = -1;
	    am.am_top = -1;
	    am.am_bottom = -1;
	    A2mGdiSetMargins(&am);
	    if(ago->ag_y >= 0) A2M_GDI_CURY  = (ago->ag_y / 2.54) * WprLOGY - WprOFFSETY - A2M_GDI_MARGY;
	    break;
	case A2M_MARGINS :
	    A2mGdiSetMargins((A2MMARGINS *)ao->ao_ptr);
	    break;
	default : break;
	}
    return(0);
}

/*NH*/
int A2mGdiSetMargins(A2MMARGINS* am)
{
    if(am->am_left >= 0) {
	A2M_GDI_MARGX = (am->am_left / 2.54) * WprLOGX - WprOFFSETX;
	if(A2M_GDI_MARGX < 0) A2M_GDI_MARGX = 0;
	A2M_GDI_W = WprHORZRES - A2M_GDI_MARGX - A2M_GDI_MARGX2;
	if(A2M_GDI_W <= 50) {
	    A2M_GDI_MARGX = WprHORZRES - A2M_GDI_MARGX2 - WprLOGX / 2;
	    A2M_GDI_W = WprHORZRES - A2M_GDI_MARGX - A2M_GDI_MARGX2;
	    }
	}

    if(am->am_right >= 0) {
	A2M_GDI_MARGX2 = (am->am_right / 2.54) * WprLOGX - WprOFFSETX;
	if(A2M_GDI_MARGX2 < 0) A2M_GDI_MARGX2 = 0;
	A2M_GDI_W = WprHORZRES - A2M_GDI_MARGX - A2M_GDI_MARGX2;
	if(A2M_GDI_W <= 50) {
	    A2M_GDI_MARGX2 = WprHORZRES - A2M_GDI_MARGX - WprLOGX / 2;
	    A2M_GDI_W = WprHORZRES - A2M_GDI_MARGX - A2M_GDI_MARGX2;
	    }
	}
    return(0);
}

/*NH*/
int A2mGdiPrepPar1(A2MPAR* ap)
{
    A2MPPR  *pp;

    if(ap == 0 || ap->ap_strs == 0) return(0);
    A2mSplitStrs(ap);
    A2mGdiCalcSizes(ap);
    pp = &(ap->ap_ppr);
    ap->ap_spacea = (pp->pp_spacea * WprLOGY) / 72;
    ap->ap_spaceb = (pp->pp_spaceb * WprLOGY) / 72;
    ap->ap_lmarg = (pp->pp_lmarg * WprLOGX) / 72;
    return(0);
}

/*NH*/
int A2mGdiPrepPar2(A2MPAR* ap, int w)
{
    if(ap == 0 || ap->ap_as2 == 0) return(0);

    A2mGdiCalcRect(ap, w - ap->ap_lmarg);
    return(0);
}
/*NH*/
A2mPtsToPix(int pts)
{
    return((WprLOGX * pts) / 72);
}

/*NH*/
int A2mGdiWritePar(A2MPAR* ap, int x, int y, int w, int just)
{
    int     i, bulw, bulh, bulm, lw, lh, pos;

    if(ap == 0 || ap->ap_as2 == 0) return(0);

    if(ap->ap_ppr.pp_bullet) {
	bulm = (8 * WprLOGX) / 72;
	bulw = (2 * WprLOGX) / 72;
	bulh = (2 * WprLOGY) / 72;
	WprSetPen(1, A2mPtsToPix(2), 0);
	WprSetBrush(2);
	WprRectangle(A2M_GDI_MARGX + x + ap->ap_lmarg - bulm,
		     A2M_GDI_MARGY + y + 3 * bulh, bulw, bulh);
	}

    i = 0;
    while(1) {
	if(A2mGdiCalcLineWH(ap->ap_as2, i, &lw, &lh)) return(0);
	switch(just) {
	    case 1 : pos = x + ap->ap_lmarg + (w - ap->ap_lmarg - lw) / 2; break;
	    case 2 :
	    case 3 : pos = x + w - lw; break;
	    default: pos = x + ap->ap_lmarg; break;
	    }

	i = A2mGdiPrintLine(ap->ap_as2, i, pos, y + lh);
	y += lh;
	i++;
	}
    
    return(0);
}

/*NH*/
int A2mGdiWriteLine(A2MLINE* al)
{
    int     type = 1;

    if(al == 0) return(0);
    if(al->al_type) type = al->al_type;

    WprSetPen(type, A2mPtsToPix(1), 0);
//    WprLine(A2M_GDI_MARGX, A2M_GDI_CURY, A2M_GDI_MARGX + A2M_GDI_W, A2M_GDI_CURY);
    WprLine(0, A2M_GDI_CURY, WprHORZRES, A2M_GDI_CURY);
}

/*NH*/
A2mGdiCalcLineWH(A2MSTR **as, int from, int *w, int *h)
{
    int     i;

    *w = *h = 0;
    if(as[from] == 0) return(-1);
    for(i = from ; as[i] ; i++) {
	if(as[i]->as_type == A2M_NEWLINE) {
	    *h = as[i]->as_h;
	    return(0);
	    }
	*w += as[i]->as_w;
	}
    return(-1);
}

/*NH*/
/* Returns next NEWLINE index */

/*NH*/
A2mGdiPrintLine(A2MSTR **as, int from, int x, int y)
{
    int     i, cy;

    for(i = from ; as[i] ; i++) {
	if(as[i]->as_type == A2M_NEWLINE) return(i);
	if(as[i]->as_type == A2M_TEXT) {
	    if(as[i]->as_txt == 0) continue;
	    A2mGdiSetFont(&(as[i]->as_fnt));
	    cy = y;
	    if(as[i]->as_fnt.af_pos) cy += as[i]->as_fnt.af_pos * as[i]->as_h / 2;
	    WprPrintString(A2M_GDI_MARGX + x,
			   A2M_GDI_MARGY + cy, as[i]->as_txt);
	    x += as[i]->as_w;
	    }
	}
    return(i - 1);
}

/*NH*/
int A2mGdiCalcRect(A2MPAR *ap, int marg)
{
    A2MSTR  **as = ap->ap_as2;
    int     i, prevsep = -1, type;
    long    lw = 0, lh = 0, cw = 0, ch = 0;

    ap->ap_h = ap->ap_h1 = ap->ap_w = 0;

    for(i = 0 ; as[i] ; i++) {
	type = as[i]->as_type;
	if(type != A2M_NEWLINE && type != A2M_SEP) continue;
	A2mGdiCalcSubRect(as, prevsep + 1, i, &cw, &ch);
	if(type == A2M_NEWLINE) {
	    lw += cw;
	    if(lh < ch) lh = ch;
	    as[i]->as_h = lh;
	    if(ap->ap_w < lw) ap->ap_w = lw;
	    if(ap->ap_h == 0) ap->ap_h1 = lh;
	    ap->ap_h += lh;
	    lw = 0;
	    lh = 0;
	    }
	else if(cw + lw > marg) {
	    if(prevsep < 0) {
		lw = cw;
		lh = ch;
		prevsep = i;
		}
	    as[prevsep]->as_type = A2M_NEWLINE;
	    as[prevsep]->as_h = lh;
	    if(ap->ap_w < lw) ap->ap_w = lw;
	    ap->ap_h += lh;
	    lw = cw;
	    lh = ch;
	    }
	else {
	    lw += cw;
	    if(lh < ch) lh = ch;
	    }
	prevsep = i;
	}

    as[i - 1]->as_type = A2M_NEWLINE;
    as[i - 1]->as_h = lh;
    if(ap->ap_w < lw) ap->ap_w = lw;
    ap->ap_h += lh;
    return(0);
}

/*NH*/
int A2mGdiCalcSubRect(A2MSTR **as, int from, int to, long *w, long *h)
{
    int     i;

    *w = *h = 0;

    for(i = from ; i < to ; i++) {
	if(as[i]->as_type != A2M_TEXT) continue;
	*w += as[i]->as_w;
	if(as[i]->as_h > *h) *h = as[i]->as_h;
	}
    return(0);
}


/*NH*/
int A2mGdiSetFont(A2MFNT *fnt)
{
    int     size = fnt->af_size;

/*    if(fnt->af_sub || fnt->af_sup) size = (3 * size) / 5; */
    if(A2M_GDI_COLOR) WprSetColor(fnt->af_color);
    else              WprSetColor(0);                 /* JMP 24-02-98 */
    WprSetFont(fnt->af_family, size,
	       fnt->af_bold, fnt->af_italic, fnt->af_underline + 2 * fnt->af_strike); /* JMP 11-04-00 */
    return(0);
}

/*NH*/
int A2mGdiCalcSizes(A2MPAR* ap)
{
    int     i;
    long    wmin = ap->ap_lmarg, wmax = ap->ap_lmarg;
    A2MSTR  **as = ap->ap_as2;

    ap->ap_w = ap->ap_h = 0;
    ap->ap_wmin = ap->ap_wmax = 0;
    for(i = 0 ; as[i] ; i++) {
	as[i]->as_w = as[i]->as_h = 0;
	switch(as[i]->as_type) {
	    case A2M_TEXT :
		if(as[i]->as_txt == 0) continue;
		A2mGdiSetFont(&(as[i]->as_fnt));
		WprGetStringWidth(as[i]->as_txt, &(as[i]->as_w), &(as[i]->as_h));
		ap->ap_w += as[i]->as_w;
		if(ap->ap_h < as[i]->as_h) ap->ap_h = as[i]->as_h;
		wmin += as[i]->as_w;
		wmax += as[i]->as_w;
		break;
	    case A2M_NEWLINE :
		if(ap->ap_wmax < wmax) ap->ap_wmax = wmax;
		wmax = ap->ap_lmarg;
		/* NO BREAK */
	    case A2M_SEP :
		if(ap->ap_wmin < wmin) ap->ap_wmin = wmin;
		wmin = ap->ap_lmarg;
		break;
	    default : break;
	    }
	}
    if(ap->ap_wmax < wmax) ap->ap_wmax = wmax;
    if(ap->ap_wmin < wmin) ap->ap_wmin = wmin;
    if(ap->ap_h == 0) {
	A2mGdiSetFont(&(ap->ap_ppr.pp_fnt));
	WprGetStringWidth("I", &wmin, &(ap->ap_h));
	}
    return(0);
}

/*NH*/
/*NH*/
/****************** TABLES ************************/

/*NH*/
int A2mGdiPrepTbl1(A2MTBL* at)
{
    A2MTC   *tc;
    int     i, j;

    if(at == 0) return(0);
    A2mGdiPrepPar1(at->at_title);
    A2mGdiPrepPar1(at->at_footnote);

    if(at->at_tspacea > -10000)
	at->at_spacea = (at->at_tspacea * WprLOGY) / 72;
    else
	at->at_spacea = (4 * WprLOGY) / 72;

    if(at->at_tspaceb > -10000)
	at->at_spaceb = (at->at_tspaceb * WprLOGY) / 72;
    else
	at->at_spaceb = (2 * WprLOGY) / 72;

    for(i = 0 ; i < at->at_nl ; i++) {
	if(at->at_tls[i].atl_type) continue;
	for(j = 0 ; j < at->at_nc ; j++) {
	    tc = at->at_tls[i].atl_tcs + j;
	    if(tc->atc_par) {
		A2mGdiPrepPar1(tc->atc_par);
		tc->atc_par->ap_spacea = 0;
		tc->atc_par->ap_spaceb = 0;
		tc->atc_w = tc->atc_par->ap_w;
		}
	    }
	}
    return(0);
}


/*NH*/
int A2mGdiPrepTblWs(A2MTBL* at, int w)
{
    A2MTC   *tc;
    A2MPAR  *ap;
    int     i, j, maxw;
    int     totw = 0, marg = WprLOGX / 20;

    if(at == 0) return(0);
    A2mGdiPrepPar2(at->at_title, w);
    A2mGdiPrepPar2(at->at_footnote, w);

    /* Calcul des largeurs en pix de chaque colonne */
    at->at_ws = (short *) SCR_malloc(sizeof(short) * at->at_nc);
    if(at->at_widths) {
	for(i = 0 ; i < at->at_nc ; i++)
	    at->at_ws[i] = (at->at_widths[i] / 2.54) * WprLOGX;
	}
    else {
	for(i = 0 ; i < at->at_nl ; i++) {
	    if(at->at_tls[i].atl_type) continue;
	    for(j = 0 ; j < at->at_nc ; j++) {
		tc = at->at_tls[i].atl_tcs + j;
		ap = tc->atc_par;
		if(ap && tc->atc_ncells == 1) {
		    at->at_ws[j] = _max_(at->at_ws[j], (ap->ap_wmax + ap->ap_lmarg + 2 * marg));
		    totw += ap->ap_wmax + 2 * marg;
		    }
		}
	    }

	if(at->at_nc > 1) maxw = (w * 45) / 100;
	else              maxw = (w * 95) / 100;
	for(j = 0 ; j < at->at_nc ; j++) {
	    if(at->at_ws[j] > maxw) at->at_ws[j] = maxw;
	    }
	}

    /* Ajuster les colonnes au min et max des cells */
    for(i = 0 ; i < at->at_nl ; i++) {
	if(at->at_tls[i].atl_type) continue;
	for(j = 0 ; j < at->at_nc ; j++) {
	    tc = at->at_tls[i].atl_tcs + j;
	    ap = tc->atc_par;
	    if(ap && tc->atc_ncells == 1) {
		if(A2M_GDI_TWIDTH_FIXED == 0)
		    at->at_ws[j] = _max_(at->at_ws[j], (ap->ap_wmin + ap->ap_lmarg + 2 * marg));
		// at->at_ws[j] = _min_(at->at_ws[j], ap->ap_wmax);
		}
	    }
	}
    return(0);
}

/*NH*/
int A2mGdiPrepTbl2(A2MTBL *at)
{
    A2MTC   *tc;
    int     i, j, k, w;

    if(at == 0) return(0);

    for(i = 0 ; i < at->at_nl ; i++) {
	if(at->at_tls[i].atl_type) continue;
	for(j = 0 ; j < at->at_nc ; j++) {
	    tc = at->at_tls[i].atl_tcs + j;
	    for(k = w = 0 ; k < tc->atc_ncells ; k++)
		w += at->at_ws[j + k];
	    A2mGdiPrepPar2(tc->atc_par, w);
	    }
	}
    return(0);
}

/*NH*/
int A2mGdiCalcTblH(A2MTBL* at)
{
    A2MTC   *tc;
    A2MPAR  *ap;
    int     i, j, h = 0, lh;

    if(at == 0) return(0);
    h += A2mGdiTblTitleH(at);

    at->at_hhead = at->at_hfoot = at->at_hbody = at->at_hbody1 = 0;
    for(i = 0 ; i < at->at_nl ; i++) {
	if(at->at_tls[i].atl_type) {
	    lh = WprLOGY / 10;
	    }
	else {
	    lh = 0;
	    for(j = 0 ; j < at->at_nc ; j++) {
		ap = at->at_tls[i].atl_tcs[j].atc_par;
		if(ap) lh = _max_(lh, ap->ap_h);
		}
	    }
	h += lh;
	switch(at->at_tls[i].atl_hbf) {
	    case 0 : at->at_hhead += lh; break;
	    case 2 : at->at_hfoot += lh; break;
	    default: at->at_hbody += lh;
		     at->at_hbody1 = _max_(at->at_hbody1, lh);
		     break;
	    }
	}

    h += A2mGdiTblFootnoteH(at);
    at->at_h = at->at_h1 = h;
    return(0);
}

/*NH*/
int A2mGdiTblTitleH(A2MTBL* at)
{
    A2MPAR  *ap = at->at_title;

    if(ap == 0) return(0);
    return(ap->ap_h + ap->ap_spaceb + ap->ap_spacea);
}

/*NH*/
int A2mGdiTblFootnoteH(A2MTBL* at)
{
    A2MPAR  *ap = at->at_footnote;

    if(ap == 0) return(0);
    return(ap->ap_h + ap->ap_spaceb);
}

/*NH*/
int A2mGdiSubTblH(A2MTBL* at, int from, int nc, int hbf)
{
    A2MPAR  *ap;
    int     i, j, h = 0, ch;

    for(i = 0 ; i < at->at_nl ; i++) {
	if(at->at_tls[i].atl_hbf != hbf) continue;
	if(at->at_tls[i].atl_type) {
	    h += WprLOGY / 10;
	    continue;
	    }
	ch = 0;
	ap = at->at_tls[i].atl_tcs[0].atc_par;
	if(ap) ch = ap->ap_h;
	for(j = 0 ; j < nc ; j++) {
	    ap = at->at_tls[i].atl_tcs[from + j].atc_par;
	    if(ap) ch = _max_(ch, ap->ap_h);
	    }
	h += ch;
	if(hbf == 1 && at->at_break) break; /* Body1 */
	}

    return(h + WprLOGY / 20);
}

/*NH*/
int A2mGdiCalcTblW(A2MTBL* at, int from, int nc)
{
    int     i, w = 0;

    w = at->at_ws[0];
    for(i = 0 ; i < nc ; i++) w += at->at_ws[from + i];
    return(w);
}

/*NH*/
int A2mWriteTblHBFwr(A2MTBL* at, int from, int nc, int hbf, int i, int wr)
{
    int     j, cx, ch, w, hmin = 0, cy = A2M_GDI_CURY;
    int     b = at->at_border;

    if(b < 0) b = A2M_GDI_TBORDER;

    w = A2mGdiCalcTblW(at, from, nc);

    if(from + nc >= at->at_nc) hmin += A2mGdiTblFootnoteH(at);
    hmin += A2mGdiSubTblH(at, from, nc, 2);

    for( ; i < at->at_nl ; i++) {
	if(at->at_tls[i].atl_hbf != hbf) continue;
	cx = 0;
	ch = 0;
	if(at->at_tls[i].atl_type) {
	    if(wr) {
		WprSetPen(1, A2mPtsToPix(1), 0);
		WprSetBrush(2);
		WprRectangle(A2M_GDI_MARGX,
			     A2M_GDI_MARGY + cy + WprLOGY / 20, w, 1);
		}
	    cy += WprLOGY / 10;
	    continue;
	    }
	A2mWriteTblCellwr(at, i, from, -from, nc, &cx, cy, &ch, wr);
	for(j = 0 ; j < nc ; j++)
	    A2mWriteTblCellwr(at, i, from, j, nc, &cx, cy, &ch, wr);

	cy += ch;
	if(hbf == 1 && hmin + cy > A2M_GDI_H) {
	    i++;
	    break;
	    }
	}

    if(cy == A2M_GDI_CURY) return(i);
    if(!wr) {
	WprSetPen(1, A2mPtsToPix(b), 0);
	if(A2M_GDI_COLOR) WprSetColor(at->at_shadcol[hbf % 2]);
	else              WprSetColor(0); /* JMP 24-02-98 */
	WprSetBrush(at->at_shadbrush[hbf % 2]);
	WprRectangle(A2M_GDI_MARGX, A2M_GDI_MARGY + A2M_GDI_CURY,
				w, cy - A2M_GDI_CURY + WprLOGY / 20);
	WprSetColor(0);
	}
    else A2M_GDI_CURY = cy + WprLOGY / 20;
    return(i);
}

/*NH*/
int A2mWriteTblHBF(A2MTBL* at, int from, int nc, int hbf)
{
    int     line1 = 0;

    while(1) {
	A2mWriteTblHBFwr(at, from, nc, hbf, line1, 0);
	line1 = A2mWriteTblHBFwr(at, from, nc, hbf, line1, 1);
	if(line1 >= at->at_nl) break;
	A2mFormFeed();
	A2mWriteTblHBFwr(at, from, nc, 0, 0, 0);
	A2mWriteTblHBFwr(at, from, nc, 0, 0, 1);
	}
    return(0);
}

/*NH*/
int A2mWriteTblCellwr(A2MTBL* at, int line, int from, int j, int nc, int* cx, int cy, int *ch, int wr)
{
    A2MTC   *tc = at->at_tls[line].atl_tcs + from + j;
    A2MPAR  *ap = tc->atc_par;
    int     cw = 0, k, ncells, marg = WprLOGX / 20, just;

    if(ap == 0) return(0);
    ncells = tc->atc_ncells;
    if(from == -j && from > 1) {
	ncells -= from;
	cw = at->at_ws[0];
	j = 0;
	}
    for(k = 0 ; k < ncells && k + j < nc; k++)
	    cw += at->at_ws[from + j + k];

    just = tc->atc_center;                             /* JMP 28-01-99 */
    if(tc->atc_center == 5) just = ap->ap_ppr.pp_just; /* JMP 28-01-99 */
    if(tc->atc_center >= 6) just -= 6; /* JMP 17-03-00 */
    if(wr) A2mGdiWritePar(ap, *cx + marg, cy, cw - 2 * marg, just);

    if(*ch < ap->ap_h) *ch = ap->ap_h;
    *cx += cw;
    return(0);
}

/*NH*/
int A2mGdiWriteSubTbl(A2MTBL* at, int from, int nc, int w)
{
    A2MPAR  *ap;
    int     hmin = 0;

    if(from == 1 || A2M_GDI_CURY == 0) hmin += A2mGdiTblTitleH(at);
    hmin += A2mGdiSubTblH(at, from, nc, 0);
    hmin += A2mGdiSubTblH(at, from, nc, 1);
    if(at->at_break == 0) hmin += A2mGdiSubTblH(at, from, nc, 2);
    if(hmin + A2M_GDI_CURY > A2M_GDI_H) A2mFormFeed();

    /* Print Title */
    if(from == 1 || A2M_GDI_CURY == 0) {
	ap = at->at_title;
	if(ap) {
	    if(A2M_GDI_CURY) A2M_GDI_CURY += ap->ap_spaceb;
	    A2mGdiWritePar(ap, 0, A2M_GDI_CURY, w, ap->ap_ppr.pp_just);
	    A2M_GDI_CURY += ap->ap_h + ap->ap_spacea;
	    }
	}

    A2mWriteTblHBF(at, from, nc, 0);
    A2mWriteTblHBF(at, from, nc, 1);
    A2mWriteTblHBF(at, from, nc, 2);

    /* Print Footnote */
    if(from + nc >= at->at_nc) {
	ap = at->at_footnote;
	if(ap) {
	    if(A2M_GDI_CURY) A2M_GDI_CURY += ap->ap_spaceb;
	    A2mGdiWritePar(ap, 0, A2M_GDI_CURY, w, ap->ap_ppr.pp_just);
	    A2M_GDI_CURY += ap->ap_h + ap->ap_spacea;
	    }
	}
    return(0);
}

/*NH*/
int A2mGdiWriteTbl(A2MTBL* at)
{
    int     from = 1, nc, w;

    if(at->at_nc == 1) {
	A2mGdiWriteSubTbl(at, 1, 0, A2M_GDI_W);
	A2M_GDI_CURY += WprLOGY / 5;
	return(0);
	}

    while(from < at->at_nc) {
	w = at->at_ws[0];
	for(nc = 0 ; nc + from < at->at_nc ; nc++) {
	    w += at->at_ws[from + nc];
	    if(w > A2M_GDI_W) break;
	    }
	if(nc == 0) nc = 1;
	A2mGdiWriteSubTbl(at, from, nc, A2M_GDI_W);
	A2M_GDI_CURY += WprLOGY / 5;
	from += nc;
	}
    return(0);
}

/*NH*/
int A2mFormFeed()
{
    if(A2M_GDI_CURX == 0 && A2M_GDI_CURY == 0) return(0);
    A2mPageHead();
    A2mPageFoot();
    WprFormFeed();
    A2M_GDI_CURX = A2M_GDI_CURY = 0;
    A2M_GDI_CURPAGE++;
    return(0);
}

/*NH*/
int A2mPageTitle(U_ch* txt, int yt, int yr, int just)
{
    A2MPAR  *ap;
    U_ch    buf[256];

    if(txt == 0 || txt[0] == 0) return(0);

    sprintf(buf, txt, A2M_GDI_CURPAGE, A2M_GDI_CURPAGE, A2M_GDI_CURPAGE);
    ap = A2mNewPar("PageNum");
    A2mSupdbl(buf, 1);
    ap->ap_strs = A2mAddParLine(ap->ap_strs, buf);
    A2mGdiPrepPar1(ap);
    A2mGdiPrepPar2(ap, A2M_GDI_W);

    WprSetPen(1, 1, 0);
    WprSetBrush(2);
    WprRectangle(A2M_GDI_MARGX, yr + A2M_GDI_MARGY, A2M_GDI_W, 1);

    A2mGdiWritePar(ap, 0, yt - ap->ap_h, A2M_GDI_W, just);

    A2mFreePar(ap);
    return(0);
}

/*NH*/
A2mPageHead()
{
    A2mPageTitle(A2M_PGHEAD, -2 * A2M_GDI_MARGY/3, -A2M_GDI_MARGY/3, 1);
    return(0);
}

/*NH*/
A2mPageFoot()
{
    A2mPageTitle(A2M_PGFOOT, A2M_GDI_H + (5 * A2M_GDI_MARGY) / 3,
			     A2M_GDI_H + (7 * A2M_GDI_MARGY) / 6,   2);
    return(0);
}


/* ================================================================
Fixe la valeur des variables globales avant l'impression d'un fichier
a2m en se basant sur les définitions du fichier .ini associé.

Les sections [A2M] et [GDI] du fichier .ini sont interprétées.

&EN filename = nom du fichier .ini

La syntaxe des fichiers a2m est décrite dans un chapitre spécifique.

&TI Fichier a2m.ini
───────────────
Le fichier a2m.ini (ou un autre) contient des paramètres pour
l'impression et la lecture du fichier a2m. Pour que ces paramètres
soient pris en compte par le programme A2mToGdi(), il faut appeler la
fonction A2mGdiReadIni(filename) avant de lancer la fonction
d'interprétation et d'impression.

&CO
    #include <s_a2m.h>

    A2mGdiReadIni("myfile.ini");
    A2mToGdi(0L, 1, "Job Title", "myfile.a2m");
&TX

&SA A2mToGdi(), A2mToRtf(), A2mToMif(), A2mToHtml(), A2mPrintError()
==================================================================== */

A2mGdiReadIni(filename)
char    *filename;
{
    U_ch    buf[255];


    A2mReadA2mIni(filename);
    if(IniReadTxtParm(filename, "GDI", "PGHEAD", buf, 255) == 0)
	A2M_PGHEAD = SCR_stracpy(buf);
    if(IniReadTxtParm(filename, "GDI", "PGFOOT", buf, 255) == 0)
	A2M_PGFOOT = SCR_stracpy(buf);
    if(IniReadTxtParm(filename, "GDI", "FONTFAMILY", buf, 255) == 0)
	A2M_FONTFAMILY = buf[0];

    IniReadNumParm(filename, "GDI", "FONTSIZE",       &A2M_FONTSIZE);
    IniReadNumParm(filename, "GDI", "FONTINCR",       &A2M_FONTINCR);
    IniReadNumParm(filename, "GDI", "TABLEFONTSIZE",  &A2M_TFONTSIZE);
    IniReadNumParm(filename, "GDI", "NUMBERING",      &A2M_NUMBERS);
    IniReadNumParm(filename, "GDI", "LMARGIN",        &A2M_GDI_LMARG);
    IniReadNumParm(filename, "GDI", "RMARGIN",        &A2M_GDI_RMARG);
    IniReadNumParm(filename, "GDI", "TMARGIN",        &A2M_GDI_TMARG);
    IniReadNumParm(filename, "GDI", "BMARGIN",        &A2M_GDI_BMARG);
    IniReadNumParm(filename, "GDI", "COLOR",          &A2M_GDI_COLOR);
    IniReadNumParm(filename, "GDI", "TABLEBORDER",    &A2M_GDI_TBORDER);

    return(0);
}

/*NH*/
/*************** GRAPHICS **********************/

#define A2M_STD_COLOR 4

extern int (*A2M_GDI_GRF_FNS[])();

/*NH*/
A2mGdiPrepGrf(ag)
A2MGRF  *ag;
{
    if(ag == 0) return(0);
    ag->ag_spacea = (8 * WprLOGY) / 72;
    ag->ag_spaceb = (8 * WprLOGY) / 72;
    ag->ag_w      = (ag->ag_size[0] * WprLOGX) / 2.54;
    ag->ag_h      = (ag->ag_size[1] * WprLOGY) / 2.54;

    if(ag->ag_title) {
	A2mGdiPrepPar1(ag->ag_title);
	A2mGdiPrepPar2(ag->ag_title, ag->ag_w);
	}
    if(ag->ag_footnote) {
	A2mGdiPrepPar1(ag->ag_footnote);
	A2mGdiPrepPar2(ag->ag_footnote, ag->ag_w);
	}
    return(0);
}

/*NH*/
A2mGdiWriteGrf(ag)
A2MGRF  *ag;
{
    if(ag == 0) return(0);
    A2M_GRF_FNS = A2M_GDI_GRF_FNS;
    A2mGrfPrint(ag);
    return(0);
}

int     A2M_GDI_GRW, A2M_GDI_GRH;

/*NH*/
A2mGdiTsf(double x1, double y1, int *gx1, int *gy1)
{
    *gx1 = A2M_GDI_MARGX + (x1 * WprLOGX) / 72;
    *gy1 = A2M_GDI_MARGY + A2M_GDI_CURY + A2M_GDI_GRH - (y1 * WprLOGX) / 72;
    return(0);
}

/*NH*/
A2mGdiGrfInit(ag)
A2MGRF  *ag;
{
    A2mGdiGrfPrepare();
    if(ag->ag_title) {          /* JMP 19-02-98 */
	A2mGdiWritePar(ag->ag_title, 0, A2M_GDI_CURY, ag->ag_w, ag->ag_title->ap_ppr.pp_just);  /* JMP 24-02-98 */
	A2M_GDI_CURY += ag->ag_title->ap_h + ag->ag_title->ap_spacea;
	}
    A2M_GDI_GRW = ag->ag_w;
    A2M_GDI_GRH = ag->ag_h;
    WprSetPen(1, A2mPtsToPix(ag->ag_box), 0); /* JMP 20-02-98 */
    WprSetBrush(ag->ag_fillbrush); /* JMP 20-02-98 */
    if(A2M_GDI_COLOR) WprSetColor(ag->ag_fillcolor);  /* JMP 20-02-98 */
    else              WprSetColor(0); /* JMP 24-02-98 */
    if(ag->ag_fillbrush || ag->ag_box) /* JMP 20-02-98 */
       WprRectangle(A2M_GDI_MARGX, A2M_GDI_MARGY + A2M_GDI_CURY,
		 A2M_GDI_GRW, A2M_GDI_GRH);
    return(0);
}

/*NH*/
A2mGdiGrfEnd(ag)
A2MGRF  *ag;
{
    if(ag->ag_footnote == 0) return(0);
    A2M_GDI_CURY += ag->ag_h + ag->ag_footnote->ap_spaceb;
    A2mGdiWritePar(ag->ag_footnote, 0, A2M_GDI_CURY, ag->ag_w, -1);
    A2M_GDI_CURY += ag->ag_footnote->ap_h;
    return(0);
}

/*NH*/
A2mGdiGrfLine(axis, x1, y1, x2, y2, props)
double  x1, y1, x2, y2;
int     axis, props;
{
    int     gx1, gy1, gx2, gy2;


    switch(axis) {
	case -1 : break;
	case  0 : x1 += GR_X_0; y1 += GR_Y_0;
		  x2 += GR_X_0; y2 += GR_Y_0;
		  break;
	default : x1 += GR_X_0; y1 += GR_Z_0;
		  x2 += GR_X_0; y2 += GR_Z_0;
		  break;
    }

    A2mGdiSetPen(props);
    A2mGdiTsf(x1, y1, &gx1, &gy1);
    A2mGdiTsf(x2, y2, &gx2, &gy2);
    WprLine(gx1, gy1, gx2, gy2);
    return(0);
}

/*NH
A2mGdiGrfText(axis, x, y, string, align, color)
int     axis;
double  x, y;
char    *string, *align, *color;
{
    double  y_0;
    int     gx1, gy1;


    A2mGdiTsf(x, y, &gx1, &gy1);
    WprPrintString(gx1, gy1, string);
    return(0);
}
*/

/*NH*/
A2mGdiGrfPar(axis, x, y, par, align, color)
double  x, y;
char    *align, *color;
A2MPAR  *par;
{
    int     gx1, gy1;

    switch(axis) {
	case -1 : break;
	case  0 : x += GR_X_0; y += GR_Y_0; break;
	default : x += GR_X_0; y += GR_Z_0;
    }

    y += A2M_FONTSIZE / 3;
    A2mGdiTsf(x, y, &gx1, &gy1);
    gx1 -= A2M_GDI_MARGX;
    gy1 -= A2M_GDI_MARGY;

    A2mGdiPrepPar1(par);
    A2mGdiPrepPar2(par, A2M_GDI_GRW);
    par->ap_spaceb = 0;
    par->ap_lmarg = 0;
    switch(align[0]) {
	case 'L' : break;
	case 'C' : gx1 -= par->ap_w / 2; break;
	case 'R' : gx1 -= par->ap_w; break;
	}

    gy1 -= par->ap_h / 2;
    A2mGdiWritePar(par, gx1, gy1, par->ap_w, -1);
    return(0);
}

/*NH*/
A2mGdiGrfGroupObj()
{
    return(0);
}

/*NH*/
A2mGdiGrfGroup()
{
    return(0);
}

/*NH*/
A2mGdiGrfBox(axis, x, y, w, h, props)
double  x, y, w, h;
int     axis, props;
{
    double      y_0 = 0;
    int         gx1, gy1, gx2, gy2;


    switch(axis) {
	case -1 : break;
	case  0 : x += GR_X_0; y_0 = GR_Y_0; break;
	default : x += GR_X_0; y_0 = GR_Z_0; break;
    }

    if(h < 0) {h = -h; y -= h;}
    if(w < 0) {w = -w; x -= w;}

    A2mGdiTsf(x,     y_0 + y,     &gx1, &gy1);
    A2mGdiTsf(x + w, y_0 + y + h, &gx2, &gy2);
    A2mGdiSetPen(0);
    A2mGdiSetBrush(props);
/*    WprSetBrush(props + 2);
    WprSetColor(props + 2);
/* JMP 19-02-98 */
    WprRectangle(gx1, gy1, gx2 - gx1, gy2 - gy1);
    return(0);
}

/*NH*/
A2mGdiGrfPolyLine(axis, nobs, vals, props)
int     axis, nobs, props;
double  *vals;
{
    int     i;
    double  y_0;
    long    *lvals = (long *)SCR_malloc(nobs * 2 * sizeof(long));

    if(axis == 0) y_0 = GR_Y_0;
    else y_0 = GR_Z_0;

    A2mGdiSetPen(props);

    for(i = 0; i < nobs; i++)
	A2mGdiTsf(GR_X_0 + vals[i * 2], y_0 + vals[i * 2 + 1],
		  lvals + i * 2, lvals + i * 2 + 1);
    WprPolyline(lvals, nobs);
    SCR_free(lvals);
    return(0);
}

/*NH*/
A2mGdiGrfPolyBar(axis, nobs, vals, width, props)
int     axis, nobs, props;
double  *vals, width;
{
    int     i;

    for(i = 0; i < nobs; i++) {
	A2mGdiGrfBox(axis, vals[i*2] - width/2.0, 0.0, width, vals[i*2+1], props);
	}
    return(0);
}


/*NH*/
/*********** TEMPORARY OBJECTS ******************/

/*NH*/
A2mGdiSetBrush(props)
int     props;
{
    int     col, brush;

    if(A2M_GDI_COLOR) {
	switch(props) {
	    case 1 : col = 2; brush = 8; break;
	    case 2 : col = 3; brush = 8; break;
	    case 3 : col = 4; brush = 8; break;
	    case 4 : col = 5; brush = 8; break;
	    case 5 : col = 6; brush = 8; break;
	    case 6 : col = 7; brush = 8; break;
	    case 7 : col = 2; brush = 5; break;
	    case 8 : col = 3; brush = 5; break;
	    case 9 : col = 4; brush = 5; break;
	    case 10: col = 5; brush = 5; break;
	    case 11: col = 6; brush = 5; break;
	    case 12: col = 7; brush = 5; break;
	    default: col = 0; brush = 0; break; /* JMP 22-04-2004 */
	    }
	 }
    else {
	col = 0;
	switch(props) {
	    case 1 :  brush = 8; break;
	    case 2 :  brush = 5; break;
	    case 3 :  brush = 2; break;
	    case 4 :  brush = 7; break;
	    case 5 :  brush = 4; break;
	    case 6 :  brush = 1; break;
	    case 7 :  brush = 6; break;
	    case 8 :  brush = 3; break;
	    case 9 :  brush = 0; break;
	    case 10:  brush = 8; break;
	    case 11:  brush = 5; break;
	    case 12:  brush = 2; break;
	    default:  brush = 0; break; /* JMP 22-04-2004 */
	    }
	 }

    WprSetBrush(brush);
    WprSetColor(col);
    return(0);
}

/*NH*/
A2mGdiSetPen(props)
int     props;
{
    if(A2M_GDI_COLOR) {
	switch(props) {
	    case 1 : WprSetPen(1, A2mPtsToPix(1), 2); break;
	    case 2 : WprSetPen(1, A2mPtsToPix(1), 3); break;
	    case 3 : WprSetPen(1, A2mPtsToPix(1), 4); break;
	    case 4 : WprSetPen(1, A2mPtsToPix(1), 5); break;
	    case 5 : WprSetPen(1, A2mPtsToPix(1), 6); break;
	    case 6 : WprSetPen(1, A2mPtsToPix(1), 7); break;
	    case 7 : WprSetPen(2, A2mPtsToPix(1), 2); break;
	    case 8 : WprSetPen(2, A2mPtsToPix(1), 3); break;
	    case 9 : WprSetPen(2, A2mPtsToPix(1), 4); break;
	    case 10: WprSetPen(2, A2mPtsToPix(1), 5); break;
	    case 11: WprSetPen(2, A2mPtsToPix(1), 6); break;
	    case 12: WprSetPen(2, A2mPtsToPix(1), 7); break;
	    default: WprSetPen(1, 1, 0); break;
	    }
	}
    else {
	switch(props) {
	    case 1 : WprSetPen(1, A2mPtsToPix(1), 0); break;
	    case 2 : WprSetPen(1, A2mPtsToPix(2), 0); break;
	    case 3 : WprSetPen(2, A2mPtsToPix(1), 0); break;
	    case 4 : WprSetPen(3, A2mPtsToPix(1), 0); break;
	    case 5 : WprSetPen(4, A2mPtsToPix(1), 0); break;
	    case 6 : WprSetPen(5, A2mPtsToPix(1), 0); break;
	    case 7 : WprSetPen(1, A2mPtsToPix(3), 0); break;
	    case 8 : WprSetPen(1, A2mPtsToPix(4), 0); break;
	    case 9 : WprSetPen(2, A2mPtsToPix(1), 0); break;
	    case 10: WprSetPen(3, A2mPtsToPix(1), 0); break;
	    case 11: WprSetPen(4, A2mPtsToPix(1), 0); break;
	    case 12: WprSetPen(5, A2mPtsToPix(1), 0); break;
	    default: WprSetPen(1, 1, 0); break;
	    }
	}
    return(0);
}

/*NH*/
A2mGdiGrfPrepare()
{
    return(0);
}

/******* FUNCTION TABLE FOR GDI **********/

int (*A2M_GDI_GRF_FNS[])() = {
    A2mGdiGrfInit,
    A2mGdiGrfEnd,
    0,          /*A2mGdiGrfPen,*/
    0,          /*A2mGdiGrfBrush,*/
    A2mGdiGrfLine,
    0,          /*A2mGdiGrfText,*/
    0,
    A2mGdiGrfPar,
    A2mGdiGrfGroupObj,
    A2mGdiGrfGroup,
    A2mGdiGrfBox,
    A2mGdiGrfPolyLine,
    A2mGdiGrfPolyBar,
    A2mGdiGrfPrepare,
    0,          /*A2mGdiGrfGetPen, */
    0           /*A2mGdiGrfGetBrush */
};

#endif



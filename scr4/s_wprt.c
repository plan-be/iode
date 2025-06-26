#include "scr4.h"
#ifdef PRWINDOWS
#include "scr4w.h"

typedef struct _wprfont_ {
    short   wf_size;
    U_ch    wf_family;      /* 0='T'=Times, 'A'rial, 'C'ourier */
    U_ch    wf_italic   :2;
    U_ch    wf_bold     :2;
    U_ch    wf_underline:2;
    HANDLE  wf_hFont;
    HDC     wf_hDC;
    short   wf_h;
    short   wf_w;
} WPRFONT;

typedef struct _wprpen_ {
    short   wp_width;
    short   wp_color;
    short   wp_type;
    HPEN    wp_hPen;
} WPRPEN;

WPRFONT     *WprFONTS = 0;
int         WprNBFONTS = 0;
HANDLE      WprFONT = 0;
int         WprCOLOR = 0;
WPRPEN      *WprPENS;
int         WprNBPENS = 0;
HBRUSH      WprBRUSHES[8];
int         WprCURPEN = -1, WprCURBRUSH = -1, WprCURFONT = -1;
HDC         WprhDC = 0;

int         WprFONTH, WprFONTW, WprHORZRES, WprVERTRES, WprLOGX, WprLOGY, WprOFFSETX, WprOFFSETY;
PRINTDLG    *WprPD;
PRINTDLGEX  *WprPDEX;
int         WprOutType;
int         WprCurLine, WprCurCol;

/* colors : "Bwrgbcmy" */

COLORREF WprCOLORS[] = {
    RGB(  0,   0,   0),  /* BLACK */
    RGB(255, 255, 255),  /* WHITE */
    RGB(255,   0,   0),  /* RED */
    RGB(  0, 255,   0),  /* GREEN */
    RGB(  0,   0, 255),  /* BLUE */
    RGB(  0, 255, 255),  /* CYAN */
    RGB(255,   0, 255),  /* MAGENTA */
    RGB(255, 255,   0)   /* YELLOW */
};


/* ======================================================================
Initialise un processus d'impression.

&EN hWndOwner = 0 ou HWND de la fenêtre père
&EN dlg = 1 si la fenêtre de sélection de paramètres de l'imprimante
    doit être ouverte, 0 sinon. Dans ce cas, l'imprimante par défaut
    de Windows est utilisée dans ses paramètres actuels.
&EN docname = nom du document (affiché dans le print spooler)

La fonction crée les différentes ressources nécessaires à l'impression
et initialise toutes les variables nécessaires.

&EN Crée les BRUSHES
&EN Crée les PENS
&EN fixe le font par défaut à Times 10 Pts

Les variables globales suivantes sont définies par la fonction :

&EN int WprLOGX    : nombre de pixels par pouce en X
&EN int WprLOGY    : nombre de pixels par pouce en Y
&EN int WprHORZRES : nombre de pixels par page en X
&EN int WprVERTRES : nombre de pixels par page en Y

&RT 0 en cas de succès, -1 en cas d'erreur ou si l'utilisateur utilise
    CANCEL pour quitter la boîte de dialogue de sélection de
    l'impression.

========================================================================= */

int WprPrinterInit(HWND hWndOwner, int dlg, char *docname)
{
    return(WprPrinterInitStd(hWndOwner, dlg, docname));
}

int WprPrinterInitStd(HWND hWndOwner, int dlg, char *docname)
{    
/*    DOCINFO     diInfo; */

    if(WprPD) return(0);
    WprPD = (PRINTDLG *) SCR_malloc(4 * sizeof(PRINTDLG));
    if(WprPD == 0) return(-1);
    memset(WprPD, 0, sizeof(PRINTDLG));

    /* Initialize the necessary PRINTDLG structure members. */
    WprPD->lStructSize  = sizeof(PRINTDLG);
    WprPD->hwndOwner    = hWndOwner;
    WprPD->Flags        = PD_RETURNDC | PD_NOSELECTION | PD_NOPAGENUMS;
    if(dlg == 0) WprPD->Flags |= PD_RETURNDEFAULT;

    if(PrintDlg(WprPD) == 0) {
	SCR_free(WprPD);
	WprPD = 0;
	return(-1);
	}

    WprhDC = WprPD->hDC;
    if(docname == 0) docname = "Wscr4";
    SCR_OemToAnsi(docname, docname);  /* JMP 13-09-98 */

    {
     // Escape(WprhDC, STARTDOC, strlen(docname), docname, NULL);
	DOCINFO     diInfo;

	memset(&diInfo, 0, sizeof(diInfo));
	diInfo.cbSize = sizeof(DOCINFO);
	diInfo.lpszDocName = "Wscr4";
	diInfo.lpszOutput = NULL;
	StartDoc(WprhDC, &diInfo);
	StartPage(WprhDC);
    }

    SCR_AnsiToOem(docname, docname);  /* JMP 13-09-98 */

    WprAllInit();
    WprOutType = WPR_TYPE_PRT;
    return(0);
}

WprPrinterInitEx(HWND hWndOwner, int dlg, char *docname)
{
/*    DOCINFO     diInfo; */

    if(WprPDEX) return(0);
    WprPDEX = (PRINTDLGEX *) SCR_malloc(4 * sizeof(PRINTDLGEX));
    if(WprPDEX == 0) return(-1);
    memset(WprPDEX, 0, sizeof(PRINTDLGEX));

    /* Initialize the necessary PRINTDLGEX structure members. */
    WprPDEX->lStructSize  = sizeof(PRINTDLGEX);
    WprPDEX->hwndOwner    = hWndOwner;
    WprPDEX->Flags        = PD_RETURNDC | PD_NOSELECTION | PD_NOPAGENUMS;
    if(dlg == 0) WprPDEX->Flags |= PD_RETURNDEFAULT;

    if(PrintDlgEx(WprPDEX) == 0) {
        SCR_free(WprPDEX);
        WprPDEX = 0;
        return(-1);
	}

    WprhDC = WprPDEX->hDC;
    if(docname == 0) docname = "Wscr4";
    SCR_OemToAnsi(docname, docname);  /* JMP 13-09-98 */

    {
     // Escape(WprhDC, STARTDOC, strlen(docname), docname, NULL);
	DOCINFO     diInfo;

	memset(&diInfo, 0, sizeof(diInfo));
	diInfo.cbSize = sizeof(DOCINFO);
	diInfo.lpszDocName = "Wscr4";
	diInfo.lpszOutput = NULL;
	StartDoc(WprhDC, &diInfo);
	StartPage(WprhDC);
    }

    SCR_AnsiToOem(docname, docname);  /* JMP 13-09-98 */

    WprAllInit();
    WprOutType = WPR_TYPE_PRT;
    return(0);
}

/*NH*/
WprAllInit()
{
    WprSetFont('T', 10, 0, 0, 0);
    WprHORZRES = GetDeviceCaps(WprhDC, HORZRES);
    WprVERTRES = GetDeviceCaps(WprhDC, VERTRES);
    WprLOGX = GetDeviceCaps(WprhDC, LOGPIXELSX);
    WprLOGY = GetDeviceCaps(WprhDC, LOGPIXELSY);
    WprOFFSETX = GetDeviceCaps(WprhDC, PHYSICALOFFSETX);
    WprOFFSETY = GetDeviceCaps(WprhDC, PHYSICALOFFSETY);
    WprCreateBrushes();
    WprCURPEN = -1;
    WprCURBRUSH = -1;
    WprCurLine = WprCurCol = 0;
    return(0);
}

/* ======================================================================
Termine le processus d'impression et envoie le job vers l'imprimante.

La fonction libère l'espace alloué par les différentes ressources
nécessaires à l'impression.

&SA WprPrinterInit()
========================================================================= */

WprPrinterEnd()
{
    WprPrinterEndFF(1);
    return(0);
}

/*NH*/
WprEndAll()
{
    WprDeleteFonts();
    WprDeleteBrushes();
    WprDeletePens();
    WprhDC = 0;
    return(0);
}

/*NH*/
int WprPrinterEndFF(int ff)
{
    return(WprPrinterEndFFStd(ff));
}   

int WprPrinterEndFFStd(int ff)
{
    if(WprPD == 0) return(0);

    EndPage(WprhDC);
    EndDoc(WprhDC);
//    if(ff) Escape(WprhDC, NEWFRAME, 0, NULL, NULL);
//    Escape(WprhDC, ENDDOC, 0, NULL, NULL);
    DeleteDC(WprhDC);
    if(WprPD->hDevMode)  GlobalFree(WprPD->hDevMode);
    if(WprPD->hDevNames) GlobalFree(WprPD->hDevNames);
    SCR_free(WprPD);
    WprPD = 0;
    WprEndAll();
    return(0);
}

WprPrinterEndFFEx(int ff)
{
    if(WprPDEX == 0) return(0);

    EndPage(WprhDC);
    EndDoc(WprhDC);
//    if(ff) Escape(WprhDC, NEWFRAME, 0, NULL, NULL);
//    Escape(WprhDC, ENDDOC, 0, NULL, NULL);
    DeleteDC(WprhDC);
    if(WprPDEX->hDevMode)  GlobalFree(WprPDEX->hDevMode);
    if(WprPDEX->hDevNames) GlobalFree(WprPDEX->hDevNames);
    SCR_free(WprPDEX);
    WprPDEX = 0;
    WprEndAll();
    return(0);
}

/*NH*/
/********************* DESSINS ***********************************/
/*NH*/
/********************** FONTS ************************************/

WprRegisterFont(int family, int size, int bold, int italic,
		int underl, HANDLE hFont, int w, int h)
{
    WprFONTS = (WPRFONT *) SCR_realloc(WprFONTS, sizeof(WPRFONT),
					WprNBFONTS, WprNBFONTS + 1);
    WprFONTS[WprNBFONTS].wf_size      = size;
    WprFONTS[WprNBFONTS].wf_family    = family;
    WprFONTS[WprNBFONTS].wf_italic    = italic;
    WprFONTS[WprNBFONTS].wf_bold      = bold;
    WprFONTS[WprNBFONTS].wf_underline = underl;
    WprFONTS[WprNBFONTS].wf_hDC       = WprhDC;
    WprFONTS[WprNBFONTS].wf_hFont     = hFont;
    WprFONTS[WprNBFONTS].wf_h         = h;
    WprFONTS[WprNBFONTS].wf_w         = w;
    WprCURFONT = WprNBFONTS;
    WprNBFONTS++;
    return(0);
}

/*NH*/
WprSelectFont(int family, int size, int bold, int italic, int underl)
{
    int     i;

    for(i = 0 ; i < WprNBFONTS ; i++) {
	if(WprFONTS[i].wf_size      == size &&
	   WprFONTS[i].wf_family    == family  &&
	   WprFONTS[i].wf_italic    == italic  &&
	   WprFONTS[i].wf_bold      == bold    &&
	   WprFONTS[i].wf_underline == underl &&
	   WprFONTS[i].wf_hDC       == WprhDC) {
		if(WprFONT == WprFONTS[i].wf_hFont) return(0);
		WprSetCurFont(i);
		return(0);
		}
	}
    return(-1);
}

/*NH*/
WprSetCurFont(int i)
{
    WprFONT  = WprFONTS[i].wf_hFont;
    WprFONTH = WprFONTS[i].wf_h;
    WprFONTW = WprFONTS[i].wf_w;
    WprCURFONT = i;
    SelectObject(WprhDC, WprFONT);
    return(0);
}

/*NH*/
WprDeleteFonts()
{
    int     i;

    for(i = 0 ; i < WprNBFONTS ; i++)
	DeleteObject(WprFONTS[i].wf_hFont);

    SCR_free(WprFONTS);
    WprFONTS = 0;
    WprNBFONTS = 0;
    WprFONT = 0;
    return(0);
}

/* ======================================================================
Cette fonction définit la police de caractères qui sera utilisée lors
des impressions suivantes.

&EN family  = 'C', 'T' ou 'A' (défaut 'A') pour Courier, Times ou Arial
&EN pty     = taille en points de la police (defaut 10)
&EN bold    = 1 si bold, 0 sinon
&EN italic  = 1 si italic, 0 sinon
&EN underl  = 1 si souligné, 0 sinon, 2 si strikethrough, 3 si souligné et
    strikethrough

Le choix de la police s'opère en fonction de ce qui est disponible
sur l'unité de destination).

Les polices utilisées sont conservées pendant toute la durée de
l'impression de façon à optimiser la vitesse de basculement d'une police
à l'autre.

========================================================================= */

WprSetFont(family, pty, bold, italic, underl)
int     family, pty, bold, italic, underl;
{
    HDC             hDC = WprhDC;
    LOGFONT         LogFont;
    TEXTMETRIC      tm;
    int             nby;
    char            *ptr;   /* BP_M 15-05-2007 */

    if(pty <= 0) pty = 10;
    if(WprSelectFont(family, pty, bold, italic, underl) == 0) return(0);
    nby = GetDeviceCaps(hDC, LOGPIXELSY);
    memset(&LogFont, 0, sizeof(LOGFONT));

    LogFont.lfHeight = -(pty * nby) / 72;
    LogFont.lfCharSet = ANSI_CHARSET;
    LogFont.lfOutPrecision = OUT_DEVICE_PRECIS; /* JMP 02-04-98 */

    if(bold)   LogFont.lfWeight = 700;
    if(italic) LogFont.lfItalic = TRUE;
    if(underl & 1) LogFont.lfUnderline = TRUE; /* JMP 11-04-00 */
    if(underl & 2) LogFont.lfStrikeOut = TRUE; /* JMP 11-04-00 */
    switch(family) {
	case 'C' :
	    //LogFont.lfQuality = PROOF_QUALITY;
	    LogFont.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	    strcpy(LogFont.lfFaceName, "Courier");
	    break;
	case 'T' :
	    strcpy(LogFont.lfFaceName, "Times");
	    break;
	case 'E' :
	    ptr = getenv("SCR_FONT_BC");
	    if(ptr)
		strcpy(LogFont.lfFaceName, ptr);
	    else
		strcpy(LogFont.lfFaceName, "Free 3 of 9");
	    break;
	default :
	    strcpy(LogFont.lfFaceName, "Arial");
	    break;
	}

//    if(WprFONT) DeleteObject(WprFONT);
    WprFONT = CreateFontIndirect(&LogFont);
    SelectObject(hDC, WprFONT);
    GetTextMetrics(hDC, &tm);
    WprFONTH = tm.tmHeight + tm.tmExternalLeading;
    WprFONTW = (100 * tm.tmAveCharWidth) / 100 ;
    WprRegisterFont(family, pty, bold, italic, underl,
		    WprFONT, WprFONTW, WprFONTH);
    return(0);
}


/* ======================================================================
Effectue un saut de page.
========================================================================= */

int WprFormFeed()
{
    return(WprFormFeedStd());
}

int WprFormFeedStd()
{
    int     i;

    if(WprPD == 0) return(0);
    // Escape(WprhDC, NEWFRAME, 0, NULL, NULL);
    EndPage(WprhDC);
    StartPage(WprhDC);  /* JMP 09-10-00 */
    WprCurLine = WprCurCol = 0;

    i = WprCOLOR;
    WprCOLOR = -1;
    WprSetColor(i);

    i = WprCURFONT;
    WprCURFONT = -1;
    WprFONT = 0;
    WprSetCurFont(i);

    i = WprCURPEN;
    WprCURPEN = -1;
    WprSetCurPen(i);

    i = WprCURBRUSH;
    WprCURBRUSH = -1;
    WprSetBrush(i);
    return(0);
}

int WprFormFeedEx()
{
    int     i;

    if(WprPDEX == 0) return(0);
    // Escape(WprhDC, NEWFRAME, 0, NULL, NULL);
    EndPage(WprhDC);
    StartPage(WprhDC);  /* JMP 09-10-00 */
    WprCurLine = WprCurCol = 0;

    i = WprCOLOR;
    WprCOLOR = -1;
    WprSetColor(i);

    i = WprCURFONT;
    WprCURFONT = -1;
    WprFONT = 0;
    WprSetCurFont(i);

    i = WprCURPEN;
    WprCURPEN = -1;
    WprSetCurPen(i);

    i = WprCURBRUSH;
    WprCURBRUSH = -1;
    WprSetBrush(i);
    return(0);
}

/* ======================================================================
Imprime le texte txt en position x, y sans changer l'alignement courant.

&EN x est l'abscisse du début (gauche) du texte
&EN y est l'ordonnée de la ligne de base du texte
&EN txt est un string terminé par un 0 (code ASCII).

Les coordonnées sont exprimées en pixels. Les variables globales
suivantes indiquent la taille du papier et la définition de l'impression:

&EN int WprLOGX    : nombre de pixels par pouce en X
&EN int WprLOGY    : nombre de pixels par pouce en Y
&EN int WprHORZRES : nombre de pixels par page en X
&EN int WprVERTRES : nombre de pixels par page en Y
========================================================================= */

WprPrintStringNoAlign(int x, int y, U_ch *txt)
{
    if(txt == 0) return(0);
    SCR_OemToAnsi(txt, txt);
    TextOut(WprhDC, x, y, txt, (int)strlen(txt));
    SCR_AnsiToOem(txt, txt);
    return(0);
}

/* ======================================================================
Imprime le texte txt en position x, y. Cette fonction change l'alignement
en le fixant à la ligne de base, cadré à gauche.

&EN x est l'abscisse du coin inférieur gauche du texte
&EN y est l'ordonnée du coin inférieur gauche du texte
&EN txt est un string terminé par un 0 (code ASCII).

Les coordonnées sont exprimées en pixels. Les variables globales
suivantes indiquent la taille du papier et la définition de l'impression:

&EN int WprLOGX    : nombre de pixels par pouce en X
&EN int WprLOGY    : nombre de pixels par pouce en Y
&EN int WprHORZRES : nombre de pixels par page en X
&EN int WprVERTRES : nombre de pixels par page en Y
========================================================================= */

WprPrintString(int x, int y, U_ch *txt)
{
    SetBkMode(WprhDC, TRANSPARENT);
    SetTextAlign(WprhDC, TA_BASELINE | TA_LEFT);
    WprPrintStringNoAlign(x, y, txt);
    return(0);
}

/* ======================================================================
Change l'alignement horizontal du texte pour la prochaine impression.
Le texte sera cadré à gauche.

&SA WprPrintString(), WprPrintStringNoAlign(), WprHCenter*(), WprVCenter*()
========================================================================= */

WprHCenterLeft()
{
    SetTextAlign(WprhDC, TA_LEFT);
    return(0);
}

/* ======================================================================
Change l'alignement horizontal du texte pour la prochaine impression.
Le texte sera cadré à droite

&SA WprPrintString(), WprPrintStringNoAlign(), WprHCenter*(), WprVCenter*()
========================================================================= */

WprHCenterRight()
{
    SetTextAlign(WprhDC, TA_RIGHT);
    return(0);
}

/* ======================================================================
Change l'alignement horizontal du texte pour la prochaine impression.
Le texte sera centré autour du point de référence.

&SA WprPrintString(), WprPrintStringNoAlign(), WprHCenter*(), WprVCenter*()
========================================================================= */

WprHCenterCenter()
{
    SetTextAlign(WprhDC, TA_CENTER);
    return(0);
}

/* ======================================================================
Change l'alignement vertical du texte pour la prochaine impression.
Le point de référence indiquera le bord supérieur.

&SA WprPrintString(), WprPrintStringNoAlign(), WprHCenter*(), WprVCenter*()
========================================================================= */

WprVCenterTop()
{
    SetTextAlign(WprhDC, TA_TOP);
    return(0);
}

/* ======================================================================
Change l'alignement vertical du texte pour la prochaine impression.
Le point de référence indiquera le bord inférieur.

&SA WprPrintString(), WprPrintStringNoAlign(), WprHCenter*(), WprVCenter*()
========================================================================= */

WprVCenterBottom()
{
    SetTextAlign(WprhDC, TA_BOTTOM);
    return(0);
}

/* ======================================================================
Change l'alignement vertical du texte pour la prochaine impression.
Le point de référence indiquera la ligne de base.

&SA WprPrintString(), WprPrintStringNoAlign(), WprHCenter*(), WprVCenter*()
========================================================================= */

WprVCenterBaseline()
{
    SetTextAlign(WprhDC, TA_BASELINE);
    return(0);
}

/* ======================================================================
Calcule la largeur et la hauteur du texte txt dans la police courante.

&EN txt est un string terminé par un 0 (code ASCII).
&EN w est un pointeur vers la largeur du texte
&EN h est un pointeur vers la hauteur du texte

Les valeurs retournées sont exprimées en pixels. Les variables globales
suivantes indiquent la taille du papier et la définition de l'impression:

&EN int WprLOGX    : nombre de pixels par pouce en X
&EN int WprLOGY    : nombre de pixels par pouce en Y
&EN int WprHORZRES : nombre de pixels par page en X
&EN int WprVERTRES : nombre de pixels par page en Y

========================================================================= */

WprGetStringWidth(U_ch *txt, long *w, long *h)
{
    SIZE    size;

    if(txt == 0) return(0);
    SCR_OemToAnsi(txt, txt);
    GetTextExtentPoint32(WprhDC, txt, (int)strlen(txt), &size);
    SCR_AnsiToOem(txt, txt);
    if(w) *w = size.cx;
    if(h) *h = size.cy;
    return(size.cx);
}

/* ======================================================================
Dessine un rectangle avec la plume (PEN), la couleur et la brosse
(BRUSH) courantes.

&EN x = abscisse du coin supérieur gauche
&EN y = ordonnée du coin supérieur gauche
&EN w = largeur
&EN h = hauteur

========================================================================= */

WprRectangle(int x, int y, int w, int h)
{
    Rectangle(WprhDC, x, y, x + w, y + h);
    return(0);
}

/* ======================================================================
Dessine une série de segments de lignes reliés entre eux en connectant
les points sucessifs.
La plume et la couleur courantes sont utilisées.

&EN pts = tableaux de coordonnées (x, y) exprimés en pixels
&EN npts = nombre de points

&SA WprSetPen(), WprSetColor(), WprLine()
========================================================================= */

WprPolyline(long *pts, int npts)
{
    Polyline(WprhDC, (POINT *)pts, npts);
    return(0);
}

/* ======================================================================
Dessine le segment de l'arc inclus dans un rectangle (ellipse) compris
entre deux points.
La plume et la couleur courantes sont utilisées.

&EN (left, top) = coordonnées du coin supérieur gauche du rectangle
&EN (right, bottom) = coordonnées du coin inférieur droit du rectangle
&EN (x0, y0) = coordonnées du point de départ
&EN (x1, y1) = coordonnées du point d'arrivée

&SA WprSetPen(), WprSetColor(), WprLine(), WprRectangle()
========================================================================= */

WprArc(int left, int top, int right, int bot, int x0, int y0, int x1, int y1)
{
    Arc(WprhDC, left, top, right, bot, x0, y0, x1, y1);
    return(0);
}

/* ======================================================================
Dessine une ligne entre deux points.
La plume et la couleur courantes sont utilisées.

&EN x0, y0 = coordonnées du point de départ
&EN x1, y1 = coordonnées du point de fin

========================================================================= */

WprLine(int x0, int y0, int x1, int y1)
{
    MoveToEx(WprhDC, x0, y0, 0L);
    LineTo(WprhDC, x1, y1);
    return(0);
}

/* ======================================================================
Sélectionne la couleur utilisée pour les textes et les brosses (BRUSH)
suivantes. Les couleurs sont définies comme suit :

&EN i = 0 : RGB(  0,   0,   0) = BLACK
&EN i = 1 : RGB(255, 255, 255) = WHITE
&EN i = 2 : RGB(255,   0,   0) = RED
&EN i = 3 : RGB(  0, 255,   0) = GREEN
&EN i = 4 : RGB(  0,   0, 255) = BLUE
&EN i = 5 : RGB(  0, 255, 255) = CYAN
&EN i = 6 : RGB(255,   0, 255) = MAGENTA
&EN i = 7 : RGB(255, 255,   0) = YELLOW

La couleur des seules brosses 3 à 8 sont modifiables.
========================================================================= */

WprSetColor(int i)
{
    if(i < 0) i = 0;
    if(WprCOLOR == i) return(0);
    if(i >= sizeof(WprCOLORS) / sizeof(WprCOLORS[0])) i = 0;
    WprCOLOR = i;
    SetTextColor(WprhDC, WprCOLORS[i]);
    return(0);
}

/*
WORD WprBR50[] = {0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55};
WORD WprBR25[] = {0xFF, 0xAA, 0xFF, 0xAA, 0xFF, 0xAA, 0xFF, 0xAA};
WORD WprBR15[] = {0x6D, 0xFF, 0xFF, 0xB6, 0xFF, 0xFF, 0x5B, 0xFF};
WORD WprBR10[] = {0xEE, 0xFF, 0xFF, 0xFF, 0xBB, 0xFF, 0xFF, 0xFF};
*/

/* ======================================================================
Sélectionne la brosse (BRUSH) de dessin pour les rectangles et ellipses
suivants. Les brosses disponibles sont :

&EN brush = 0 pour une brosse transparente,
&EN brush = 1 pour une brosse pleine blanche,
&EN brush = 2 pour une brosse pleine noire,
&EN brush = 3 pour une brosse hachurée 1 pixel/64,
&EN brush = 4 pour une brosse hachurée 2 pixels/64,
&EN brush = 5 pour une brosse hachurée 4 pixels/64,
&EN brush = 6 pour une brosse hachurée 8 pixels/64,
&EN brush = 7 pour une brosse hachurée 16 pixels/64,
&EN brush = 8 pour une brosse hachurée 32 pixels/64,

La couleur des brosses 3 à 8 sont définies par la fonction WprSetColor().

&REM Si le numéro de la brosse n'est pas défini, une brosse nulle est
sélectionnée.
========================================================================= */

WprSetBrush(int brush)
{
    int     nbr;

    if(brush == WprCURBRUSH) return(0);
    nbr = sizeof(WprBRUSHES) / sizeof(WprBRUSHES[0]);
    if(brush < 1 || brush > nbr) {
	brush = 0;
	SelectObject(WprhDC, GetStockObject(NULL_BRUSH));
	}
    else
	SelectObject(WprhDC, WprBRUSHES[brush - 1]);

    WprCURBRUSH = brush;
    return(0);
}

WORD WprBR0[]  = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
WORD WprBR1[]  = {0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
WORD WprBR2[]  = {0x7F, 0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF};
WORD WprBR4[]  = {0x77, 0xFF, 0xFF, 0xFF, 0xDD, 0xFF, 0xFF, 0xFF};
WORD WprBR8[]  = {0x77, 0xFF, 0xDD, 0xFF, 0x77, 0xFF, 0xDD, 0xFF};
WORD WprBR16[] = {0x55, 0xFF, 0xAA, 0xFF, 0x55, 0xFF, 0xAA, 0xFF};
WORD WprBR32[] = {0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA};
WORD WprBR64[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/*NH*/
HBRUSH WprCreateBrush(WORD *brdef)
{
    HBITMAP hBitmap;
    HBRUSH  hBrush;

    hBitmap = CreateBitmap(8, 8, 1, 1, (LPSTR) brdef);
    hBrush = CreatePatternBrush(hBitmap);
    DeleteObject(hBitmap);
    return(hBrush);
}

/*NH*/
WprCreateBrushes()
{
    WprBRUSHES[0] = CreateSolidBrush(RGB(255, 255, 255)); /* White */
    WprBRUSHES[1] = WprCreateBrush(WprBR0);
    WprBRUSHES[2] = WprCreateBrush(WprBR1);
    WprBRUSHES[3] = WprCreateBrush(WprBR2);
    WprBRUSHES[4] = WprCreateBrush(WprBR4);
    WprBRUSHES[5] = WprCreateBrush(WprBR8);
    WprBRUSHES[6] = WprCreateBrush(WprBR16);
    WprBRUSHES[7] = WprCreateBrush(WprBR32);
    return(0);
}

/*NH*/
WprDeleteBrushes()
{
    int     i;

    for(i = 0 ; i < sizeof(WprBRUSHES) / sizeof(WprBRUSHES[0]); i++) {
	DeleteObject(WprBRUSHES[i]);
	WprBRUSHES[i] = 0;
	}
    return(0);
}

/*NH*/
/*************************** PENS ********************************/

/*NH*/
WprRegisterPen(int type, int width, int color, HPEN hPen)
{
    WprPENS = (WPRPEN *) SCR_realloc(WprPENS, sizeof(WPRPEN),
					WprNBPENS, WprNBPENS + 1);
    WprPENS[WprNBPENS].wp_type  = type;
    WprPENS[WprNBPENS].wp_width = width;
    WprPENS[WprNBPENS].wp_color = color;
    WprPENS[WprNBPENS].wp_hPen  = hPen;
    WprNBPENS++;
    return(0);
}

/*NH*/
WprDeletePens()
{
    int     i;

    for(i = 0 ; i < WprNBPENS ; i++)
	DeleteObject(WprPENS[i].wp_hPen);

    SCR_free(WprPENS);
    WprPENS = 0;
    WprNBPENS = 0;
    WprCURPEN = -1;
    return(0);
}

/*NH*/
WprSetCurPen(int i)
{
    if(WprCURPEN == i ) return(0);
    SelectObject(WprhDC, WprPENS[i].wp_hPen);
    WprCURPEN = i;
    return(0);
}

/*NH*/
WprFindPen(int type, int width, int color)
{
    int     i;

    for(i = 0 ; i < WprNBPENS ; i++) {
	if(WprPENS[i].wp_type  == type &&
	   WprPENS[i].wp_width == width  &&
	   WprPENS[i].wp_color == color) {
		WprSetCurPen(i);
		return(0);
		}
	}
    return(-1);
}

/* ======================================================================
Sélectionne une plume de pour les tracés suivants.
&EN type vaut :
&EN2 0 : plume nulle
&EN2 1 : plume continue
&EN2 2 : plume dash
&EN2 3 : plume dot
&EN2 4 : plume dash-dot
&EN2 5 : plume dash-dot-dot
&EN width = largeur de la plume en pixels
&EN color vaut:
&EN2 0 : RGB(  0,   0,   0) = BLACK
&EN2 1 : RGB(255, 255, 255) = WHITE
&EN2 2 : RGB(255,   0,   0) = RED
&EN2 3 : RGB(  0, 255,   0) = GREEN
&EN2 4 : RGB(  0,   0, 255) = BLUE
&EN2 5 : RGB(  0, 255, 255) = CYAN
&EN2 6 : RGB(255,   0, 255) = MAGENTA
&EN2 7 : RGB(255, 255,   0) = YELLOW

&SA WprSetBrush(), WprSetFont()
========================================================================= */

WprSetPen(int type, int width, int color)
{
    HPEN    hPen;
    int     style;

    if(WprFindPen(type, width, color) == 0) return(0);
    if(type == 0 || width == 0) {
	WprCURPEN = -1;
	SelectObject(WprhDC, GetStockObject(NULL_PEN));
	return(0);
	}

    switch(type) {
	case 1 : style = PS_SOLID; break;
	case 2 : style = PS_DASH;       width = 0; break;
	case 3 : style = PS_DOT;        width = 0;break;
	case 4 : style = PS_DASHDOT;    width = 0;break;
	case 5 : style = PS_DASHDOTDOT; width = 0;break;
	}

    if(color < 0 || color > 7) color = 0;
    hPen = CreatePen(style, width, WprCOLORS[color]);
    WprRegisterPen(type, width, color, hPen);
    SelectObject(WprhDC, hPen);
    WprCURPEN = WprNBPENS - 1;
    return(0);
}

/*NH
HPEN WprCreateDashPen(int width, long *segs)
{
    HPEN        hPen;
    LOGBRUSH    lb;
    HBITMAP     hBitmap;
    int         nbsegs = 0, i;
    DWORD       dwsegs[20], *pdwsegs = dwsegs;

    if(segs) for(nbsegs = 0 ; segs[nbsegs] ; nbsegs++)
		pdwsegs[nbsegs] = (WprLOGX * segs[nbsegs]) / 72;
    else pdwsegs = 0;

    lb.lbStyle = BS_PATTERN;
    lb.lbColor = 0;
    hBitmap    = CreateBitmap(8, 8, 1, 1, (LPSTR) WprBR64);
    lb.lbHatch = (LONG)hBitmap;

    hPen = ExtCreatePen(PS_COSMETIC | PS_USERSTYLE,
			(DWORD)((WprLOGX * width) / 72) ,
			&lb,
			(DWORD)nbsegs, pdwsegs);

    DeleteObject(hBitmap);
    return(hPen);
}

DWORD WprSEGS1[]  = {2, 3, 0};
DWORD WprSEGS2[]  = {1, 1, 0};
DWORD WprSEGS3[]  = {3, 1, 1, 1, 0};
DWORD WprSEGS4[]  = {3, 1, 1, 1, 1, 1, 0};

WprCreatePens()
{
    WprPENS[0] = WprCreateDashPen(1, (long *)0L);
    WprPENS[1] = WprCreateDashPen(1, (long *)WprSEGS1);
    WprPENS[2] = WprCreateDashPen(1, (long *)WprSEGS2);
    WprPENS[3] = WprCreateDashPen(1, (long *)WprSEGS3);
    WprPENS[4] = WprCreateDashPen(1, (long *)WprSEGS4);
    return(0);
}
*/

#endif /* PRWINDOWS */








#include "scr4.h"
#ifdef PRWINDOWS
#include "scr4w.h"

/****** Fonctions level 2 ************/
U_ch    *WprBUF;
int     WprBUFSIZE, WprMAXBUFSIZE;
int     WprCOUR10H, WprCOUR10W, WprMARGL, WprMARGT, WprNL, WprNC;

extern int  WprCurLine, WprCurCol;
extern HDC  WprhDC;

/* ======================================================================
Initialise un processus d'impression en mode caractère.

&EN hWndOwner = 0 ou HWND de la fenêtre père
&EN dlg = 1 si la fenêtre de sélection de paramètres de l'imprimante
    doit être ouverte, 0 sinon. Dans ce cas, l'imprimante par défaut
    de Windows est utilisée dans ses paramètres actuels.
&EN docname = nom du document (affiché dans le print spooler)

La fonction crée les différentes ressources nécessaires à l'impression
et initialise toutes les variables nécessaires.

&EN Crée les BRUSHES
&EN Crée les PENS
&EN fixe le font par défaut à Courier 10 Pts

Les variables globales suivantes sont définies par la fonction :

&EN int WprLOGX    : nombre de pixels par pouce en X
&EN int WprLOGY    : nombre de pixels par pouce en Y
&EN int WprHORZRES : nombre de pixels par page en X
&EN int WprVERTRES : nombre de pixels par page en Y
&EN int WprMARGL   : nombre de pixels marge de gauche (4 caractères
		     Courier 10)
&EN int WprMARGT   : nombre de pixels marge du haut (0)
&EN int WprNL      : nombre de lignes de texte en Courier 10
&EN int WprNC      : nombre de colonnes de texte en Courier 10

Les valeurs de WprMARGT et WprMARGL peuvent être modifiées par
l'utilisateur après l'appel de cette fonction.

&RT 0 en cas de succès, -1 en cas d'erreur ou si l'utilisateur utilise
    CANCEL pour quitter la boîte de dialogue de sélection de
    l'impression.

========================================================================= */

WprChInit(HWND hWndOwner, int dlg, char *docname)
{
    if(WprPrinterInit(hWndOwner, dlg, docname)) return(-1);
    WprSetFont('C', 10, 0, 0, 0);
    WprCOUR10H = WprFONTH;
    WprCOUR10W = WprFONTW;
    WprMARGL = 4 * WprCOUR10W;
    WprMARGT = 0 * WprCOUR10H;
    WprNL = (WprVERTRES - WprMARGT) / WprCOUR10H;
    WprNC = (WprHORZRES - WprMARGL) / WprCOUR10W;
    WprBUF = 0;
    WprBUFSIZE = WprMAXBUFSIZE = 0;
    return(0);
}

/* ======================================================================
Termine le processus d'impression en mode caractère et envoie le job
vers l'imprimante.

Cette fonction prend l'hypothèse que la dernière page a été préalablement
envoyée par un formfeed antérieur (c'est le cas lors de l'utilisation des
fonctions du groupe PR_*).

La fonction libère l'espace alloué par les différentes ressources
nécessaires à l'impression.
========================================================================= */

WprChEnd()
{
    WprPrinterEndFF(0);
    SCR_free(WprBUF);
    WprBUF = 0;
    WprBUFSIZE = WprMAXBUFSIZE = 0;
    return(0);
}

/* ======================================================================
Effectue un saut à la ligne (fonction d'impression Windows en mode
caractère).

&EN n = nombre de lignes à sauter.
========================================================================= */

WprChNewLine(int n)
{
    if(WprhDC == 0) return(0);
    WprChFlushBuf();
    WprCurLine += n;
    WprCurCol = 0;
    return(0);
}

/* ======================================================================
Imprime la page courante et prépare une nouvelle page (fonction
d'impression Windows en mode caractère).
========================================================================= */

WprChFormFeed()
{
    if(WprhDC == 0) return(0);
    WprChFlushBuf();
    WprFormFeed();
    return(0);
}

/*NH*/
WprChFlushBuf()
{
    if(WprBUFSIZE == 0) return(0);
    SCR_OemToAnsi(WprBUF, WprBUF);
    TextOut(WprhDC, WprMARGL + WprCOUR10W * WprCurCol,
			WprMARGT + WprCOUR10H * WprCurLine,
			WprBUF, WprBUFSIZE);
    WprCurCol += WprBUFSIZE;
    WprBUFSIZE = 0;
    return(0);
}

/* ======================================================================
Imprime un string codé Ascii (fonction d'impression Windows en mode
caractère). L'impression a lieu à la position courante dans la page.

&EN txt = pointeur vers un string terminé par un 0.

Les caractères semi-graphiques de la table ASCII-437 sont imprimés en
utilisant les primitives graphiques de l'imprimante.

Les caractères suivants sont interprétés:

&EN TAB : aligne sur 8 caractères
&EN NEWLINE : saut à la ligne
&EN FORMFEED : saut à la page
&EN SPACE : si deux blancs successifs sont détectés, un réalignement
    a lieu en fonction de la taille moyenne d'un caractère.
========================================================================= */

WprChPrintString(U_ch *txt)
{
    int     i;

    if(txt == 0) return(0);
    for(i = 0 ; txt[i] ; i++) WprChPrintChar(txt[i]);
    return(0);
}

/*NH*/
WprChPrintChar(int ch)
{
    switch(ch) {
	case '\n' : WprChNewLine(1); return(0);
	case '\f' : WprChFormFeed(); return(0);
	case ' '  : if(WprBUFSIZE > 0 && WprBUF[WprBUFSIZE - 1] == ' ') {
			WprChFlushBuf();
			WprCurCol++;
			return(0);
			}
		    break;
	case '\t' : WprChFlushBuf();
		    WprCurCol += 8 - WprCurCol % 8;
		    return(0);
	default   : break;
	}

    if(ch > 178 && ch < 219) {
	WprChFlushBuf();
	WprChPrintGr(ch);
	WprCurCol++;
	return(0);
	}

    if(WprMAXBUFSIZE <= WprBUFSIZE + 1) {
	WprBUF = (U_ch *) SCR_realloc(WprBUF, 1, WprMAXBUFSIZE,
				      WprMAXBUFSIZE + 80);
	if(WprBUF == 0) SCR_panic();
	WprMAXBUFSIZE += 80;
	}

    WprBUF[WprBUFSIZE] = ch;
    WprBUF[WprBUFSIZE + 1] = 0;
    WprBUFSIZE++;
    return(0);
}



/* GRAPHIC CHARS */

unsigned char WprLEFTPINS[40] = {
    0, 1, 2, 1, 1, 2, 2, 0, 2, 2, 1,
       2, 1, 0, 1, 1, 0, 1, 1, 0, 0,
       0, 0, 2, 2, 0, 2, 2, 2, 1, 2,
       1, 0, 0, 0, 0, 1, 2, 1, 0 };

unsigned char WprRIGHTPINS[40] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 1, 1, 1, 1, 1, 1, 2, 1,
       2, 2, 2, 2, 2, 2, 2, 2, 1, 2,
       1, 1, 2, 2, 1, 1, 2, 0, 1 };

unsigned char WprUPPERPINS[40] = {
    1, 1, 1, 2, 0, 0, 2, 2, 0, 2, 2,
       1, 0, 1, 1, 0, 1, 0, 1, 1, 2,
       2, 0, 2, 0, 2, 0, 2, 1, 2, 0,
       0, 2, 1, 0, 0, 2, 1, 1, 0 };

unsigned char WprLOWERPINS[40] = {
    1, 1, 1, 2, 2, 1, 2, 2, 2, 0, 0,
       0, 1, 0, 0, 1, 1, 0, 1, 1, 2,
       0, 2, 0, 2, 2, 0, 2, 0, 0, 1,
       2, 0, 0, 1, 2, 2, 1, 0, 1 };

/*NH*/
WprChPrintGr(ch)
int     ch;
{
    ch -= 179;
    if(ch < 0 || ch >= 40) return(0);

    if(WprUPPERPINS[ch]) {
	WprSetPen(1, WprUPPERPINS[ch], 0);
	MoveToEx(WprhDC,
		    WprMARGL + (WprCurCol * WprCOUR10W) + WprCOUR10W/2,
		    WprMARGT + WprCurLine * WprCOUR10H, NULL);
	LineTo  (WprhDC,
		    WprMARGL + (WprCurCol * WprCOUR10W) + WprCOUR10W/2,
		    WprMARGT + WprCurLine * WprCOUR10H + WprCOUR10H / 2);
	}
    if(WprLOWERPINS[ch]) {
	WprSetPen(1, WprLOWERPINS[ch], 0);
	MoveToEx(WprhDC,
		    WprMARGL + (WprCurCol * WprCOUR10W) + WprCOUR10W/2,
		    WprMARGT + WprCurLine * WprCOUR10H + WprCOUR10H/2, NULL);
	LineTo  (WprhDC,
		    WprMARGL + (WprCurCol * WprCOUR10W) + WprCOUR10W/2,
		    WprMARGT + (WprCurLine + 1) * WprCOUR10H);
	}

    if(WprLEFTPINS[ch])  {
	WprSetPen(1, WprLEFTPINS[ch], 0);
	MoveToEx(WprhDC,
		    WprMARGL + (WprCurCol * WprCOUR10W),
		    WprMARGT + WprCurLine * WprCOUR10H + WprCOUR10H/2, NULL);
	LineTo  (WprhDC,
		    WprMARGL + (WprCurCol * WprCOUR10W) + WprCOUR10W/2,
		    WprMARGT + WprCurLine * WprCOUR10H + WprCOUR10H/2);
	}
    if(WprRIGHTPINS[ch]) {
	WprSetPen(1, WprRIGHTPINS[ch], 0);
	MoveToEx(WprhDC,
		    WprMARGL + (WprCurCol * WprCOUR10W) + WprCOUR10W/2,
		    WprMARGT + WprCurLine * WprCOUR10H + WprCOUR10H/2, NULL);
	LineTo  (WprhDC,
		    WprMARGL + (WprCurCol + 1 ) * WprCOUR10W,
		    WprMARGT + WprCurLine * WprCOUR10H + WprCOUR10H/2);
	}
    return(0);
}

int WprChENLARGED = 14, WprChCONDENSED = 8;

/*NH*/
WprSetFontType(int attr)
{
    switch(attr) {
	case SCR_DEFAULT    : WprSetFont('C', 10, 0, 0, 0); break;
	case SCR_BOLD       : WprSetFont('C', 10, 1, 0, 0); break;
	case SCR_UNDERLINE  : WprSetFont('C', 10, 0, 0, 1); break;
	case SCR_REVERSE    : WprSetFont('C', 10, 0, 1, 0); break;
	case SCR_ENLARGED   : WprSetFont('C', WprChENLARGED,   1, 0, 0); break;
	case SCR_CONDENSED  : WprSetFont('C', WprChCONDENSED,  0, 0, 0); break;
	}
    return(0);
}


#endif /* PRWINDOWS */

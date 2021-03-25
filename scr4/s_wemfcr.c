#if defined(SCRW32) || defined(DOSW32)
#include "scr4w.h"

extern int      WprOutType;
extern HDC      WprhDC;

char            *WprEMFFile = 0;
HENHMETAFILE    WprhMeta;

/* ANNULE JMP
HWND WprGethWnd()
{
#ifdef SCRW32
    return(hMainWnd);
#else
    extern HWND hWndDOS;

    return(hWndDOS);
#endif
}
*/

/*NH
HDC WprEMFCreatehDC(U_ch *filename, int w, int h)
{
    HDC     hDCMeta, hdcRef;
    RECT    rect;
    HWND    hWnd = WscrGetMainWnd();
    int     iWidthMM, iHeightMM,
	    iWidthPels, iHeightPels;

    hdcRef      = GetDC(hWnd);
    iWidthMM    = GetDeviceCaps(hdcRef, HORZSIZE);
    iHeightMM   = GetDeviceCaps(hdcRef, VERTSIZE);
    iWidthPels  = GetDeviceCaps(hdcRef, HORZRES);
    iHeightPels = GetDeviceCaps(hdcRef, VERTRES);
    GetClientRect(hWnd, &rect);

//    SetMapMode(hdcRef, MM_ANISOTROPIC);
//    SetViewportExtEx(hdcRef, rect.right, -rect.bottom, NULL);
//    SetViewportOrgEx(hdcRef, 0, rect.bottom, NULL);

    rect.left   = (rect.left   * iWidthMM  * 100)/iWidthPels;
    rect.top    = (rect.top    * iHeightMM * 100)/iHeightPels;
    rect.right  = (rect.right  * iWidthMM  * 100)/iWidthPels;
    rect.bottom = (rect.bottom * iHeightMM * 100)/iHeightPels;

    hDCMeta = CreateEnhMetaFile(hdcRef, filename, &rect, "SCR4/AL1\0IODE\0\0");
    ReleaseDC(hWnd, hdcRef);
    return(hDCMeta);
}
*/

/*NH*/
HDC WprEMFCreatehDC(U_ch *filename, int w, int h, U_ch *pictname)
{
    HDC     hDCMeta;
    RECT    rect;
    HWND    hWnd = WscrGetMainWnd();
    HDC     hdcRef = GetDC(hWnd);
    U_ch    buf[26];

    rect.left   = 0;
    rect.top    = 0;
    rect.right  = w * 100;
    rect.bottom = h * 100;

    memset(buf, 0, sizeof(buf));
    if(filename) strcat(buf, filename);
    if(pictname) strcpy(buf + strlen(buf) + 1, pictname);
    hDCMeta = CreateEnhMetaFile(hdcRef, filename, &rect, buf);
    ReleaseDC(hWnd, hdcRef);
    return(hDCMeta);
}

/* ======================================================================
Initialise un processus d'impression dans un fichier ou un handle en
m�moire de type Windows Enhanced MetaFile.

&EN filename = nom du fichier r�sultat (l'extension EMF n'est pas ajout�e
    automatiquement) ou 0 pour cr�er un EMF en m�moire. Le handle sur le
    EMF m�moire sera retourn� dans ce cas par le fonction WprEMFEnd().
    Cette valeur devra �tre lib�r�e par le programmeur
    (DeleteEnhMetaFile(hEmf)).

&EN w = largeur en mm de la fen�tre de dessin. Les marges sont comprises
    dans cette largeur.

&EN h = hauteur en mm de la fen�tre de dessin. Les marges sont comprises
    dans cette valeur.

&EN pictname = nom du document (affich� dans le titre de la fen�tre). Si
    nul, il n'y a pas de nom associ�.

La fonction cr�e les diff�rentes ressources n�cessaires � l'impression
et initialise toutes les variables n�cessaires.

&EN Cr�e les BRUSHES
&EN Cr�e les PENS
&EN fixe le font par d�faut � Times 10 Pts

Les variables globales suivantes sont d�finies par la fonction :

&EN int WprLOGX    : nombre de pixels par pouce en X
&EN int WprLOGY    : nombre de pixels par pouce en Y
&EN int WprHORZRES : nombre de pixels en X
&EN int WprVERTRES : nombre de pixels en Y

&RT 0 en cas de succ�s, -1 en cas d'erreur

&SA WprPrinterInit(), WprWMFInit(), WprEMFEnd(), WscrEMFCreate()

&NO Les sauts de pages �ventuels surchargent simplement le dessin,
    une seule "page" �tant disponible.
========================================================================= */

WprEMFInit(char *filename, int w, int h, U_ch *pictname)
{
    char    buf[132], *fn;

    if(filename) {
	SCR_change_ext(buf, filename, "emf");
	WprhDC = WprEMFCreatehDC(buf, w, h, pictname);
	fn = SCR_stracpy(buf);
	}
    else
	WprhDC = WprEMFCreatehDC(filename, w, h, pictname);

    if(WprhDC == 0) return(-1);
    WprAllInit();
    WprEMFFile = fn;
    WprOutType = WPR_TYPE_EMF;
    WprHORZRES = (10 * w * WprLOGX) / 254;
    WprVERTRES = (10 * h * WprLOGY) / 254;
    return(0);
}

/* ======================================================================
Initialise un processus d'impression dans un fichier ou un handle en
m�moire de type Windows MetaFile.

&EN filename = nom du fichier r�sultat (l'extension WMF n'est pas ajout�e
    automatiquement) ou 0 pour cr�er un WMF en m�moire. Le handle sur le
    WMF m�moire sera retourn� dans ce cas par le fonction WprWMFEnd().
    Cette valeur devra �tre lib�r�e par le programmeur
    (DeleteMetaFile(hWmf)).

&EN w = largeur en mm de la fen�tre de dessin. Les marges sont comprises
    dans cette largeur.

&EN h = hauteur en mm de la fen�tre de dessin. Les marges sont comprises
    dans cette valeur.

La fonction cr�e les diff�rentes ressources n�cessaires � l'impression
et initialise toutes les variables n�cessaires.

&EN Cr�e les BRUSHES
&EN Cr�e les PENS
&EN fixe le font par d�faut � Times 10 Pts

Les variables globales suivantes sont d�finies par la fonction :

&EN int WprLOGX    : nombre de pixels par pouce en X
&EN int WprLOGY    : nombre de pixels par pouce en Y
&EN int WprHORZRES : nombre de pixels en X
&EN int WprVERTRES : nombre de pixels en Y

&RT 0 en cas de succ�s, -1 en cas d'erreur

&SA WprPrinterInit(), WprEMFInit(), WprWMFEnd()

&NO Les sauts de pages �ventuels surchargent simplement le dessin,
    une seule "page" �tant disponible.
========================================================================= */

WprWMFInit(char *filename, int w, int h)
{
    if(WprEMFInit(filename, w, h, 0L)) return(-1);
    WprOutType = WPR_TYPE_WMF;
    return(0);
}

/* ======================================================================
Termine le processus d'impression dans un Enhanced Windows MetaFile.
Selon qu'un nom de fichier ait �t� fourni � la fonction
d'initialisation, cr�e un fichier ou retourne un handle vers un EMF en
m�moire.

On peut utiliser le handle retourn� pour un affichage dans une fen�tre.

&EX
    #include <scr4w.h>

    HENHMETAFILE    hEmf;
    ...
    WprEMFInit(0L, 100, 100, "Mon dessin");
    ..
    hEmf = WprEMFEnd();
    WscrEMFCreate(HMainWnd, 0L, hEmf);
&TX

La fen�tre cr��e par la fonction WscrEMFCreate() d�truit le Handle hEmf
lorsqu'on la quitte. L'utilisateur n'a donc pas � s'en pr�occuper dans
le cas de l'exemple. Dans d'autres cas, il faut utiliser la fonction de
l'API Win32

&CO
    DeleteEnhMetaFile(hEmf);
&TX

pour lib�rer le HANDLE hEmf.

&RT
&EN Si un fichier est cr��, la valeur de retour est nulle
&EN Si un handle est cr��, la valeur de retour est ce handle

La fonction lib�re l'espace allou� par les diff�rentes ressources
allou�es.

&SA WprEMFInit(), WscrEMFCreate()
========================================================================= */

HENHMETAFILE WprEMFEnd()
{
    HDC             hDC;
    HENHMETAFILE    hEnhMeta;

    if(WprhDC == 0) return(0);

    hEnhMeta = CloseEnhMetaFile(WprhDC);
    if(WprEMFFile) {
	DeleteEnhMetaFile(hEnhMeta);
	hEnhMeta = 0;
	SCR_free(WprEMFFile);
	WprEMFFile = 0;
	}
    WprEndAll();
    WprEMFFile = 0;
    WprhMeta = 0;
    return(hEnhMeta);
}

/* ======================================================================
Termine le processus d'impression dans un Windows MetaFile. Selon qu'un
nom de fichier ait �t� fourni � la fonction d'initialisation, cr�e un
fichier ou retourne un handle vers un WMF en m�moire.

L'extension .wmf est fix�e pour le fichier r�sultat (si un nom de
fichier a �t� pass� � la fonction WprWMFInit().

&EX
    #include <scr4w.h>

    HMETAFILE    hWmf;
    ...
    WprWMFInit(0L, 100, 100);
    ..
    hWmf = WprWMFEnd();
&TX

Il faut utiliser la fonction de l'API Win32

&CO
    DeleteMetaFile(hWmf);
&TX
pour lib�rer le HANDLE hWmf.

On peut utiliser le handle retourn� pour un affichage dans une fen�tre.

&EX
    #include <scr4w.h>

    HMETAFILE    hWmf;
    ...
    WprWMFInit(0L, 100, 100);
    ..
    hEmf = WprWMFEnd();
    WscrWMFCreate(HMainWnd, 0L, hWmf);
&TX

La fen�tre cr��e par la fonction WscrWMFCreate() d�truit automatiquement
le Handle hWmf � la fin de son existence. L'utilisateur n'a donc pas �
s'en pr�occuper dans le cas de l'exemple.

&RT
&EN Si un fichier est cr��, la valeur de retour est nulle
&EN Si un handle est cr��, la valeur de retour est ce handle

La fonction lib�re l'espace allou� pour les diff�rentes ressources
n�cessaires.

&SA WprWMFInit()
========================================================================= */

HMETAFILE WprWMFEnd()
{
    HDC             hDC;
    HENHMETAFILE    hEnhMeta;
    HMETAFILE       hMeta, hMeta2;
    char            buf[128], *wmf;
    int             bufsize;

    if(WprhDC == 0) return(0);

    hDC = GetDC(WscrGetMainWnd());
    WprhMeta = CloseEnhMetaFile(WprhDC);
    bufsize = GetWinMetaFileBits(WprhMeta, 0, NULL, MM_ANISOTROPIC, hDC);
    wmf = SCR_malloc(bufsize);
    bufsize = GetWinMetaFileBits(WprhMeta, bufsize, wmf, MM_ANISOTROPIC, hDC);
    DeleteEnhMetaFile(WprhMeta);
    WprhMeta = 0;
    hMeta = SetMetaFileBitsEx(bufsize, wmf);
    SCR_free(wmf);
    ReleaseDC(WscrGetMainWnd(), hDC);

    if(WprEMFFile) {
	unlink(WprEMFFile);  /* JMP 29-04-98 */
	SCR_change_ext(buf, WprEMFFile, "wmf");
	hMeta2 = CopyMetaFile(hMeta, buf);
	DeleteMetaFile(hMeta2);
	DeleteMetaFile(hMeta);
	hMeta = 0;
	SCR_free(WprEMFFile);
	WprEMFFile = 0;
	}

    WprEndAll();
    WprEMFFile = 0;
    WprhMeta = 0;
    return(hMeta);
}

#endif

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
mémoire de type Windows Enhanced MetaFile.

&EN filename = nom du fichier résultat (l'extension EMF n'est pas ajoutée
    automatiquement) ou 0 pour créer un EMF en mémoire. Le handle sur le
    EMF mémoire sera retourné dans ce cas par le fonction WprEMFEnd().
    Cette valeur devra être libérée par le programmeur
    (DeleteEnhMetaFile(hEmf)).

&EN w = largeur en mm de la fenêtre de dessin. Les marges sont comprises
    dans cette largeur.

&EN h = hauteur en mm de la fenêtre de dessin. Les marges sont comprises
    dans cette valeur.

&EN pictname = nom du document (affiché dans le titre de la fenêtre). Si
    nul, il n'y a pas de nom associé.

La fonction crée les différentes ressources nécessaires à l'impression
et initialise toutes les variables nécessaires.

&EN Crée les BRUSHES
&EN Crée les PENS
&EN fixe le font par défaut à Times 10 Pts

Les variables globales suivantes sont définies par la fonction :

&EN int WprLOGX    : nombre de pixels par pouce en X
&EN int WprLOGY    : nombre de pixels par pouce en Y
&EN int WprHORZRES : nombre de pixels en X
&EN int WprVERTRES : nombre de pixels en Y

&RT 0 en cas de succès, -1 en cas d'erreur

&SA WprPrinterInit(), WprWMFInit(), WprEMFEnd(), WscrEMFCreate()

&NO Les sauts de pages éventuels surchargent simplement le dessin,
    une seule "page" étant disponible.
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
mémoire de type Windows MetaFile.

&EN filename = nom du fichier résultat (l'extension WMF n'est pas ajoutée
    automatiquement) ou 0 pour créer un WMF en mémoire. Le handle sur le
    WMF mémoire sera retourné dans ce cas par le fonction WprWMFEnd().
    Cette valeur devra être libérée par le programmeur
    (DeleteMetaFile(hWmf)).

&EN w = largeur en mm de la fenêtre de dessin. Les marges sont comprises
    dans cette largeur.

&EN h = hauteur en mm de la fenêtre de dessin. Les marges sont comprises
    dans cette valeur.

La fonction crée les différentes ressources nécessaires à l'impression
et initialise toutes les variables nécessaires.

&EN Crée les BRUSHES
&EN Crée les PENS
&EN fixe le font par défaut à Times 10 Pts

Les variables globales suivantes sont définies par la fonction :

&EN int WprLOGX    : nombre de pixels par pouce en X
&EN int WprLOGY    : nombre de pixels par pouce en Y
&EN int WprHORZRES : nombre de pixels en X
&EN int WprVERTRES : nombre de pixels en Y

&RT 0 en cas de succès, -1 en cas d'erreur

&SA WprPrinterInit(), WprEMFInit(), WprWMFEnd()

&NO Les sauts de pages éventuels surchargent simplement le dessin,
    une seule "page" étant disponible.
========================================================================= */

WprWMFInit(char *filename, int w, int h)
{
    if(WprEMFInit(filename, w, h, 0L)) return(-1);
    WprOutType = WPR_TYPE_WMF;
    return(0);
}

/* ======================================================================
Termine le processus d'impression dans un Enhanced Windows MetaFile.
Selon qu'un nom de fichier ait été fourni à la fonction
d'initialisation, crée un fichier ou retourne un handle vers un EMF en
mémoire.

On peut utiliser le handle retourné pour un affichage dans une fenêtre.

&EX
    #include <scr4w.h>

    HENHMETAFILE    hEmf;
    ...
    WprEMFInit(0L, 100, 100, "Mon dessin");
    ..
    hEmf = WprEMFEnd();
    WscrEMFCreate(HMainWnd, 0L, hEmf);
&TX

La fenêtre créée par la fonction WscrEMFCreate() détruit le Handle hEmf
lorsqu'on la quitte. L'utilisateur n'a donc pas à s'en préoccuper dans
le cas de l'exemple. Dans d'autres cas, il faut utiliser la fonction de
l'API Win32

&CO
    DeleteEnhMetaFile(hEmf);
&TX

pour libérer le HANDLE hEmf.

&RT
&EN Si un fichier est créé, la valeur de retour est nulle
&EN Si un handle est créé, la valeur de retour est ce handle

La fonction libère l'espace alloué par les différentes ressources
allouées.

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
nom de fichier ait été fourni à la fonction d'initialisation, crée un
fichier ou retourne un handle vers un WMF en mémoire.

L'extension .wmf est fixée pour le fichier résultat (si un nom de
fichier a été passé à la fonction WprWMFInit().

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
pour libérer le HANDLE hWmf.

On peut utiliser le handle retourné pour un affichage dans une fenêtre.

&EX
    #include <scr4w.h>

    HMETAFILE    hWmf;
    ...
    WprWMFInit(0L, 100, 100);
    ..
    hEmf = WprWMFEnd();
    WscrWMFCreate(HMainWnd, 0L, hWmf);
&TX

La fenêtre créée par la fonction WscrWMFCreate() détruit automatiquement
le Handle hWmf à la fin de son existence. L'utilisateur n'a donc pas à
s'en préoccuper dans le cas de l'exemple.

&RT
&EN Si un fichier est créé, la valeur de retour est nulle
&EN Si un handle est créé, la valeur de retour est ce handle

La fonction libère l'espace alloué pour les différentes ressources
nécessaires.

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

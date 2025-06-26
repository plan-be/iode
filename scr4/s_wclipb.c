#if defined(DOSW32) || defined(SCRW32)

#include "scr4w.h"

/* =======================================================================
Place sur dans le clipboard un buffer de texte. Ce buffer ne doit pas être
statique: il est réalloué par la fonction. Le précédent texte dans le
clipboard est perdu.

Si le texte contient plusieurs lignes, elles doivent être séparées par
des \n. S'il y a plusieurs colonnes, elles seront séparées par des \t.

&RT
&EN 0 si l'opération est réussie.
&EN -1 en cas d'erreur (clipboard inaccessible, memoire indisponible.

&SA WscrClipGet(), WscrClipIsData()
========================================================================== */

WscrClipSet(U_ch *txt)
{
    HGLOBAL hglbCopy;
    U_ch    *ptr;

    /* Open the clipboard, and empty it. */
    if(txt == 0) return(0);
    if(!OpenClipboard(WscrGetMainWnd())) return(-1);
    EmptyClipboard();

    /* Allocate a global memory object for the text. */
    hglbCopy = GlobalAlloc(GMEM_DDESHARE, 1 + strlen(txt));
    if(hglbCopy == NULL) {
	CloseClipboard();
	return(-1);
	}

    /* Lock the handle and copy the text to the buffer. */
    ptr = GlobalLock(hglbCopy);
    strcpy(ptr, txt);
    GlobalUnlock(hglbCopy);

    /* Place the handle on the clipboard. */
    SetClipboardData(CF_TEXT, hglbCopy);

    /* Close the clipboard. */
    CloseClipboard();

    return(0);
}

/* =======================================================================
Indique si du texte est disponible dans le clipboard.

&RT
&EN 0 si non
&EN 1 si oui

&SA WscrClipSet(), WscrClipGet()
========================================================================== */

WscrClipIsData()
{
    return(IsClipboardFormatAvailable(CF_TEXT));
}

/* =======================================================================
Retourne dans un buffer alloué le contenu du clipboard.
Ce buffer doit être libéré par le programme appelant: il ne s'agit pas
d'un pointeur direct vers le clipboard.

&RT
&EN pointeur vers un buffer alloué de texte. S'il y a plusieurs lignes,
    elles sont séparées par des \n. S'il y a plusieurs colonnes, elles
    sont séparées par des \t.
&EN NULL si le clipboard est vide ou s'il contient autre chose que du texte.

&SA WscrClipSet(), WscrClipIsData()
========================================================================== */

U_ch *WscrClipGet()
{
    HGLOBAL hglb;
    LPTSTR  lptstr;
    U_ch    *txt, *atxt = 0;

    if(WscrClipIsData() == 0)     return(atxt);
    if(!OpenClipboard(WscrGetMainWnd()))  return(atxt);
    hglb = GetClipboardData(CF_TEXT);
    if(hglb == NULL) {
	CloseClipboard();
	return(atxt);
	}
    txt = GlobalLock(hglb);
    if(txt) atxt = SCR_stracpy(txt);
    GlobalUnlock(hglb);
    CloseClipboard();
    return(atxt);
}
#else
#include "scr4.h"   /* JMP 12-10-98 */

/*NH*/
WscrClipSet(U_ch *txt)
{
    return(0);
}

/*NH*/
WscrClipIsData()
{
    return(0);
}

/*NH*/
U_ch *WscrClipGet()
{
    return((U_ch *)0);
}

#endif





#if defined(SCRW32) || defined(DOSW32)
#include <scr4w.h>

/* =======================================================================
Ajoute une icône dans la barre de tâches de Windows, à côté de l'horloge.
Plusieurs icônes peuvent être associées à la même fenêtre.

&EN hWnd : handle de la fenêtre mère
&EN IconNb : numéro de l'icône (défini dans le fichier .rc)
&EN lpszTip : tooltip

&SA WscrDeleteTaskBarIcon(), WscrChangeTaskBarTooltip()
======================================================================= */

WscrAddTaskBarIcon(HWND hWnd, int IconNb, LPSTR lpszTip)
{
    HICON           hIcon;
    BOOL            res;
    NOTIFYICONDATA  tnid;

    memset(&tnid, 0, sizeof(tnid));
    if(IconNb) hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IconNb));
    tnid.cbSize = sizeof(NOTIFYICONDATA);
    tnid.hWnd = hWnd;
    tnid.uID = IconNb;
    tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    tnid.uCallbackMessage = WSCR_NOTIFYICON;
    tnid.hIcon = hIcon;
    if(lpszTip)
	SCR_strlcpy(tnid.szTip, lpszTip, sizeof(tnid.szTip));
    else
	tnid.szTip[0] = '\0';
    res = Shell_NotifyIcon(NIM_ADD, &tnid);
    if(hIcon) DestroyIcon(hIcon);
    if(res == TRUE) return(0);
    else            return(-1);
}

/* =======================================================================
Détruit l'icône associée à une fenêtre dans la barre de tâches de Windows.

&EN hWnd : handle de la fenêtre mère
&EN IconNb : numéro de l'icône (défini dans le fichier .rc)

&SA WscrDeleteTaskBarIcon(), WscrChangeTaskBarTooltip()
======================================================================= */

WscrDeleteTaskBarIcon(HWND hwnd, UINT IconNb)
{
    BOOL            res;
    NOTIFYICONDATA  tnid;

    memset(&tnid, 0, sizeof(tnid));
    tnid.cbSize = sizeof(NOTIFYICONDATA);
    tnid.hWnd = hwnd;
    tnid.uID = IconNb;
    res = Shell_NotifyIcon(NIM_DELETE, &tnid);
    if(res == TRUE) return(0);
    else            return(-1);
}

/* =======================================================================
Change le tooltip d'une icône associée à une fenêtre dans la barre de tâches
de Windows.

&EN hWnd : handle de la fenêtre mère
&EN IconNb : numéro de l'icône (défini dans le fichier .rc)
&EN lpszTip : nouveau texte du tooltip (peut être une adresse volatile)

&SA WscrDeleteTaskBarIcon(), WscrAddTaskBarIcon()
======================================================================= */

WscrChangeTaskBarTooltip(HWND hWnd, int IconNb, LPSTR lpszTip)
{
    BOOL            res;
    NOTIFYICONDATA  tnid;

    memset(&tnid, 0, sizeof(tnid));
    tnid.cbSize = sizeof(NOTIFYICONDATA);
    tnid.hWnd = hWnd;
    tnid.uID = IconNb;
    tnid.uFlags = NIF_TIP;
    if(lpszTip)
	SCR_strlcpy(tnid.szTip, lpszTip, sizeof(tnid.szTip));
    else
	tnid.szTip[0] = '\0';
    res = Shell_NotifyIcon(NIM_MODIFY, &tnid);
    if(res == TRUE) return(0);
    else            return(-1);
}

#endif






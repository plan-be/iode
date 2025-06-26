#if defined(SCRW32) || defined(DOSW32)
#include <scr4w.h>

/*NH*/
WINAPI WscrHiddenProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL        lRes;
    WSCRPROCFN  fn = (WSCRPROCFN) GetWindowLongPtr(hWnd, 0);

    if(fn) {
	lRes = (*fn)(hWnd, wMsg, wParam, lParam);
	if(lRes) return(0);
	}

    lRes = DefWindowProc(hWnd, wMsg, wParam, lParam);
    return(lRes);
}

/* =======================================================================
Crée une fenêtre cachée permettant d'échanger des messages entre processus
sur une même machine.

Il faut créer une boucle de messages pour que cette fenêtre puisse recevoir
les messages de l'application. Le plus simple est en général de créer un
thread séparé, dans lequel la boucle de messages pourra se dérouler.

&RT handle vers la fenêtre crée ou 0 en cas de problème.

&EN fn : fonction utilisateur qui reçoit les messages de la fenêtre
    et agit en fonction

Cette fonction a la syntaxe suivante :

&CO
    int fn(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
&TX

Les arguments reçus sont :
&EN hWnd : handle de la fenêtre
&EN wMsg : message (entier)
&EN wParam : paramètre (entier)
&EN lParam : paramètre (long)

La fonction fn retourne 0 si le message n'est pas traité (il s'agit d'un
message de Windows) et 1 si le message est traité. Dans le cas où le message
n'est pas traité par la fonction, il est passé à une fonction standard de
Windows.

&EX
    int ClientEvents(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
    {
	switch(wMsg) {
	    case WSCR_GETCLIENTPID :
		PostMessage(hWndServer, WSCR_SETCLIENTPID,
			    (WPARAM)wParam, (LPARAM) GetCurrentProcessId());
		return(1);

	    case WSCR_KILLPROCESS :
		PostMessage(hWndServer, WSCR_ENDCLIENT, lParam, 0);
		ISS_STOP = 1;
		return(1);

	    default : break;
	    }
	return(0);
    }

    MyMessageWindow()
    {
	HWND    hWndClient;

	ISS_STOP = 0;
	hWndClient = WscrCreateHiddenWindow(ClientEvents);
	WscrMessageLoop(&ISS_STOP);
	WscrDeleteHiddenWindow(hWndClient);
    }
&TX

&SA WscrDeleteHiddenWindow(), WscrMessageLoop(), WscrAddTaskBarIcon()
======================================================================= */

HWND WscrCreateHiddenWindow(WSCRPROCFN fn)
{
    HWND        hWnd;
    WNDCLASS    wc;
    static int  wcdefined = 0;

    /* Register Wscr Classes */
    if(wcdefined == 0) {
	memset(&wc, 0, sizeof(wc));
	wc.lpszClassName    = "WscrHidden";
	wc.hbrBackground    = (HBRUSH) (COLOR_WINDOW + 1);
	wc.hInstance        = hInst;
	wc.lpfnWndProc      = WscrHiddenProc;
	wc.cbWndExtra       = 12;  //
	if(!RegisterClass(&wc)) return(0);
	}

    hWnd = CreateWindow("WscrHidden", "",
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, hInst, NULL);

    if(hWnd) SetWindowLongPtr(hWnd, 0, (LONG)fn);
    return(hWnd);
}

/* =======================================================================
Crée une boucle de message. Cette boucle se termine quand la valeur
pointée par stop n'est plus nulle. Cette fonction est blocante pour le reste
du thread courant.

&EN int *stop : pointeur vers la valeur qui indique si la boucle doit
    se poursuivre (0) ou s'arrêter (autre valeur).

&RT valeur de *stop
&SA WscrCreateHiddenWindow(), WscrDeleteHiddenWindow(), WscrAddTaskBarIcon()
======================================================================= */

WscrMessageLoop(int *stop)
{
    MSG     Msg;

    while(*stop == 0 && GetMessage(&Msg, NULL, 0, 0)) {
	TranslateMessage(&Msg);
	DispatchMessage(&Msg);
	}
    return(*stop);
}

/* =======================================================================
Détruit la fenêtre cachée créée par WscrCreateHiddenWindow().

&SA WscrCreateHiddenWindow(), WscrMessageLoop(), WscrAddTaskBarIcon()
======================================================================= */

WscrDeleteHiddenWindow(HWND hWnd)
{
    PostMessage(hWnd, WM_CLOSE, 1, 0L);
}

#endif

#include "scr4w.h"
#include <commctrl.h>

/**************************  A ADAPTER ******************************/
extern U_ch    **WSCRArgv;  /* BP_M 12-12-2009 14:08 */
extern int     WSCRArgc;    /* BP_M 12-12-2009 14:08 */
extern long FAR PASCAL WscrMainProc  (HWND, UINT, WPARAM, LPARAM);
extern long FAR PASCAL WscrDosProc   (HWND, UINT, WPARAM, LPARAM);

extern long    WSCR_TIMER_DELAY;


/* + adapter WscrMainProc */
/********************************************************************/


/* =================== FONCTIONS ANNEXES D'INITIALISATION =========== */

U_ch **WscrCreateArgv(U_ch *CmdLine)
{
    U_ch    *buffer, **argv;
/*    U_ch    **SCR_vtoms(); /* JMP 04-01-98 */

    buffer = SCR_malloc(strlen(CmdLine) + 100);
    GetModuleFileName(hInst, buffer, 99);
    strcat(buffer, " ");
    strcat(buffer, CmdLine);

    argv = SCR_vtoms(buffer, " \t");
/*    argv = A_init(buffer); */
    SCR_free(buffer);
    return(argv);
}

#if !defined(__cplusplus) && !defined(GENPROTO)

extern DWORD   WSCR_LASTKEY_TIME;   /* BP_M 25-03-2000 */

int CALLBACK /*PASCAL*/ WinMain(hInstance, hPrevInstance, lpCmdLine, nCmdShow)
HINSTANCE hInstance, hPrevInstance;
LPSTR   lpCmdLine;
int     nCmdShow;
{
    MSG     Msg;
/*    U_ch    **WscrCreateArgv(); /* JMP 07-01-98 */
    int     rc = -1;

    hInst = hInstance;
    hPrevInst = hPrevInstance;
    WSCRArgv = WscrCreateArgv(lpCmdLine);
    WSCRArgc = SCR_tbl_size(WSCRArgv);
    if(WscrStart1(WSCRArgc, WSCRArgv)) goto fin2;
    WscrSetKeyboardHook();
    WscrSetMouseHook();
    WscrSetKeyboardTimer();
    if(!MainInit(WSCRArgc, WSCRArgv)) goto fin;
//    hAccel = LoadAccelerators(hInst, "TestAccel");

    while(GetMessage(&Msg, NULL, 0, 0)) {
	if(hAccel) {
	    rc = TranslateAccelerator(Msg.hwnd, hAccel, &Msg);
	    if(rc) continue;
	    }

	switch(Msg.message) {
	    case WM_KEYDOWN:
	    case WM_SYSKEYDOWN:
		WSCR_LASTKEY_TIME = GetTickCount(); /* BP_M 25-03-2000 */
		if(WscrProcKeydown(Msg.hwnd, Msg.wParam)) {
		    if(WSCRArgv == 0) return(-1); /* JMP 18-06-99 */
		    continue;
		}
		break;
	    default:
		break;
	}

	TranslateMessage(&Msg);
	DispatchMessage(&Msg);
	}

    rc = Msg.wParam;

fin:
    MainEnd();
    WscrClearKeyboardHook();
    WscrClearMouseHook();
    WscrKillKeyboardTimer();
fin2:
    WscrEnd(WSCRArgc, WSCRArgv);
    SCR_free_tbl(WSCRArgv);
    WSCRArgv = 0;
    return(rc);
}

#endif /* __cplusplus ... */

int MainInit(int argc, U_ch **argv)
{
    WNDCLASS    wc;
    HMENU       hMenu;

    /* Init Common Controls */
    InitCommonControls();

    /* Register Wscr Classes */
    if(!hPrevInst) {
	/* Wscr Class */
	wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon            = LoadIcon(hInst, MAKEINTRESOURCE(WSCR_ICON_RC));
/*        wc.lpszMenuName     = MAKEINTRESOURCE(WSCR_MAIN_ABAR); */
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = "Wscr";
	wc.hbrBackground    = (HBRUSH) (COLOR_WINDOW + 1);
	wc.hInstance        = hInst;
	wc.style            = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc      = WscrMainProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = 0;
	if(!RegisterClass(&wc)) return(FALSE);

/* WscrDos Class
	wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName    = "WscrDos";
	wc.hbrBackground    = COLOR_WINDOW + 1;
	wc.style            = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc      = WscrDosProc;
	if(!RegisterClass(&wc)) return(FALSE);
*/

	/* CLASS SCROLL */
	if(WscrScrollRegister() == 0) return(FALSE); /* JMP 13-01-97 */
	}


    /* Create GDI Objetcs */
    WscrCreateGDIObjects();

    /* Load .scr, alloc CMT */
    WscrInit();
    SCR_cmt_alloc();
    ERR_TYPE = 1; /* JMP 13-10-96 */
    if(SCR_applname) WSCRMainTitle = SCR_applname;
    if(WSCRMainTitle) SCR_OemToAnsi(WSCRMainTitle, WSCRMainTitle);

    hMainWnd = WscrMain(argc, argv);
    if(hMainWnd == 0) return(FALSE);
    WscrSetTimer(WSCR_TIMER_DELAY); /* BP_M 10-12-2010 10:03 */


    return(TRUE);
}


HWND WscrCreateWscr(HMENU hMenu)
{
    HWND    hWnd;

    hWnd = CreateWindow("Wscr", WSCRMainTitle,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			NULL, NULL, hInst, NULL);
    if(!hWnd) return(hWnd);
    if(hMenu) SetMenu(hWnd, hMenu);
    WscrStatusCreate(hWnd);
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    return(hWnd);
}

extern FARPROC  W_DLGEDITFN;    /* BP_M 12-12-2009 14:09 */

int MainEnd()
{
    extern FARPROC  W_DLGEDITFN, WDDLG_STATICFN;
    extern FARPROC  W_DLGEDITFN_DFT, W_DLGBUTTONFN_DFT,
		    W_DLGCOMBOFN_DFT, W_DLGLISTBOXFN_DFT;
    extern int      WHLP_ACTIVE;

    WscrSetTimer(0);

    if(WHLP_ACTIVE)
	WinHelp(hMainWnd, HLP_NAME, HELP_QUIT, 0L); /* JMP 14-04-97 */
    WHLP_ACTIVE = 0;

    if(W_DLGEDITFN)     FreeProcInstance(W_DLGEDITFN);
    if(WDDLG_STATICFN)  FreeProcInstance(WDDLG_STATICFN);

    WscrDeleteGDIObjects(); /* JMP 08-11-97 */
    SCR_mini_end();         /* JMP 13-09-97 */
    return(0);
}

WscrSetApplName(txt)
U_ch    *txt;
{
    WSCRMainTitle = SCR_stracpy(txt);
    SCR_OemToAnsi(WSCRMainTitle, WSCRMainTitle);
    WscrSetWindowTextOem(hMainWnd, txt);
}



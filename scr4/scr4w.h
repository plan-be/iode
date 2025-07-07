#ifndef DOS
#include <scr4.h>
#include <s_mat.h>
#include <s_xdr.h>

#else

#ifndef _WSCR_
#define IDC_GRAPH1      30010
#define _WSCR_

#define NOCOMM
#ifndef _WINDOWS
    #define _WINDOWS
#endif     
#define __SIMPLE_LOCALES__
#include <windows.h>
#include <scr4.h> 
#include <s_swap.h>
#include <s_mat.h>
#include <s_xdr.h>
#include <s_scroll.h>
#include <ddeml.h>



/* ====== COLORS ========= */

#define WCOL_BLACK       RGB(  0,   0,   0)
#define WCOL_BLUE        RGB(  0,   0, 255)
#define WCOL_GREEN       RGB(  0, 255,   0)
#define WCOL_RED         RGB(255,   0,   0)
#define WCOL_CYAN        RGB(  0, 255, 255)
#define WCOL_MAGENTA     RGB(255,   0, 255)
#define WCOL_YELLOW      RGB(255, 255,   0)
#define WCOL_WHITE       RGB(255, 255, 255)
#define WCOL_DKGRAY      RGB( 64,  64,  64)
#define WCOL_GRAY        RGB(128, 128, 128)
#define WCOL_LTGRAY      RGB(192, 192, 192)
#define WCOL_LTBLUE      RGB(  0,  64, 255)
#define WCOL_DKBLUE      RGB(  0, 128, 255)

/* ============ MISC ======= */

typedef int (*WSCRPROCFN)(HWND , UINT , WPARAM , LPARAM ); /* JMP 16-04-00 */

#define WACT_SHIFT      100
#define WIS_SHIFT       500
#define WMP_SHIFT1      510
#define WMP_SHIFT2      520
#define WIDX_SHIFT      550
#define WSCR_NOTIFYICON 5000 /* JMP 16-04-00 */
#define WGRP_FIRST      22000
#define WSCR_SCROLLID   23000
#define WSCR_HEADERID   24000
#define WSCR_TOOLBARID  25000
#define WSCR_STATUSID   26000
#define WBC_SCROLLID    27000
#define WMP_SCROLLID    28000
#define WFLD_FIRST      30000
#define WFLD_DIRBUTTON  31000

/* =============== SCROLL =================== */

/* WSCROLL MESSAGES */
#define WM_WSCRL_SET          WM_USER + 1000
#define WM_WSCRL_GET          WM_USER + 1001
#define WM_WSCRL_CHGCURRENT   WM_USER + 1002
#define WM_WSCRL_CHGSELECTION WM_USER + 1003
#define WM_WSCRL_DBLCLK       WM_USER + 1004
#define WM_WSCRL_KILLFOCUS    WM_USER + 1005
#define WM_WSCRL_KEYDOWN      WM_USER + 1006

#define WSCR_TOOLBAR_NB     9
#define WSCR_TOOLBAR_SIZE   16
#define WSCR_BITMAP_RC      26001
#define WSCR_ICON_RC        26002
#define WSCR_NOTIFYICON     5000 /* JMP 16-04-00 */

/* WSCROLL DEFINES */
#define WSL_LINESEL   1
#define WSL_COLSEL    2
#define WSL_ONESEL    4

typedef struct _WSCRLCELL_ {
    int     x, y, w, h, col, align, font, fg;
    U_ch    *txt;
} WSCRLCELL;

typedef struct _WSCROLL_ {
    char     *data;
    int      cline, ccol, line, col;
    int      seltype, sellinefrom, selcolfrom, sellineto, selcolto;
#ifdef __cplusplus
    int      (*getnl)(_WSCROLL_ *);
    int      (*getnc)(_WSCROLL_ *);
    int      (*getwidth)(_WSCROLL_ *, int);
    int      (*getwidthp)(_WSCROLL_ *, int);
    int      (*setwidthp)(_WSCROLL_ *, int);
    int      (*getheight)(_WSCROLL_ *, int);
    int      (*getheightp)(_WSCROLL_ *, int);
    int      (*setheightp)(_WSCROLL_ *, int);
    int      (*getalign)(_WSCROLL_ *, int, int);      /* JMP 27-05-97 */
    U_ch     *(*gettext)(_WSCROLL_ *, int, int);
    U_ch     *(*getedtext)(_WSCROLL_ *, int, int);
    int      (*settext)(_WSCROLL_ *, int, int, U_ch *);
    int      (*getcolor)(_WSCROLL_ *, int, int);
    int      (*getfont)(_WSCROLL_ *, int, int);
    int      (*fnkey)(_WSCROLL_ *, int, int, int);
    int      (*fnbeg)(_WSCROLL_ *, int *, int *);
    int      (*fnend)(_WSCROLL_ *, int, int);
    int      (*fncmd)(_WSCROLL_ *, int, int , WPARAM);
    int      (*fnrmouse)(_WSCROLL_ *, int, int);
#else
    int      (*getnl)();
    int      (*getnc)();
    int      (*getwidth)();
    int      (*getwidthp)();
    int      (*setwidthp)();
    int      (*getheight)();
    int      (*getheightp)();
    int      (*setheightp)();
    int      (*getalign)();      /* JMP 27-05-97 */
    U_ch     *(*gettext)();
    U_ch     *(*getedtext)();
    int      (*settext)();
    int      (*getcolor)();
    int      (*getfont)();
    int      (*fnkey)();
    int      (*fnbeg)();
    int      (*fnend)();
    int      (*fncmd)();
    int      (*fnrmouse)();
#endif

    HWND     hMenu;
    int      *colws;

    /* Private */
    int      mousedown, editon;
    int      nlv, ncv;
    int      oline, ocol, ocline, occol;
    int      height, width;
    HWND     hWnd, hEditWnd, hHeaderWnd;
    int      enabled, end;
    int      mnl, mnc;
    char     ***mlns;

} WSCROLL;


typedef struct _scrwin_ {
    char    *sh_ptr;        /* Obj ptr       */
    PAGE    *sh_pg;         /* Page (if any) */
    int     sh_act;         /* Action        */
    int     sh_status;      /* Status MPAGE  */
    int     sh_pgedit;      /* Internal loop PG_EDIT  */
    HMENU   sh_hmn;         /* Menu Handle   */
    HWND    sh_hwnd;        /* Window Handle */
#ifdef __cplusplus
    int     (*sh_fn)(HWND, void *, int);     /* Fn pointer    */
#else
    int     (*sh_fn)();     /* Fn pointer    */
#endif
    char    *sh_data;       /* User Data */
} WSCRHWND;

#define DDE_TIMEOUT      3000

/* ========= Wpr DEFINES ============= */
#define WPR_TYPE_PRT     1
#define WPR_TYPE_EMF     2
#define WPR_TYPE_WMF     3

/************* WDDLG *****************/
#define WDDLG_BUTTON    0x0080
#define WDDLG_EDIT      0x0081
#define WDDLG_STATIC    0x0082
#define WDDLG_LISTBOX   0x0083
#define WDDLG_SCROLLBAR 0x0084
#define WDDLG_COMBOBOX  0x0085
#define WDDLG_ICON      0x0086
#define WDDLG_BMP       0x0087
#define WDDLG_SLIDE     0x0088
#define WDDLG_LISTVIEW  0x0089
#define WDDLG_PROGRESS  0x008A
#define WDDLG_GROUPBOX  0x008B

#define WDDLG_MULTISEL  0x0001
#define WDDLG_DEFBUTTON 0x0002
#define WDDLG_HIDDEN    0x0004

#define WDDLG_SETFOCUS      1
#define WDDLG_KILLFOCUS     2
#define WDDLG_CLICKED       3
#define WDDLG_DBLCLK        4
#define WDDLG_CHANGED       5
#define WDDLG_CHAR          (WM_USER + 6)

typedef struct _wddlg_ {
    char    *wdlg;
    WORD    *p;
    int     nctrls;
    int     x, y, rc;
    int     *types;
    int     *fonts;
    int     *ios;
    int     *attrs;
    int     **tabs;
    char    **ptrs;
    char    **exts;
#ifdef __cplusplus
    int     (**fns)(_wddlg_ *, int, char *, UINT);
    int     (*escfn)(_wddlg_ *);
    int     (*okfn)(_wddlg_ *);
    int     (*beginfn)(_wddlg_ *);
    int     (*endfn)(_wddlg_ *);
#else
    int     (**fns)();
    int     (*escfn)();
    int     (*okfn)();
    int     (*beginfn)();
    int     (*endfn)();
#endif
    HWND    hWnd;
    int     changing;
} WDDLG;

/* ======================= VARIABLES =============================== */

extern U_ch     *WSCRQueryEnd, *WSCRMainTitle; /* JMP 04-02-97 */
extern HANDLE   hInst, hPrevInst, hAccel;
extern HWND     hMainWnd, hWndDOS;
extern HWND     WSCR_CUR_HWND;
extern int      WSCR_ABORT;         /* PRIVATE - DO NOT CHANGE */

extern WSCRHWND  *WSCR_HWNDS;
extern int       WSCR_NB_HWNDS, WSCR_MAX_HWNDS;

//extern FARPROC W_SCRL_EDITFN, W_SCRL_DFT_EDITFN;
extern WNDPROC W_SCRL_EDITFN, W_SCRL_DFT_EDITFN;
extern HANDLE  W_SCRL_FONT, W_SCRL_FONTI, W_SCRL_FONTB, W_SCRL_FONTBI,
	       W_SCRL_FONT_TERM;
extern HBRUSH  W_BRUSH_BLACK, W_BRUSH_BLUE, W_BRUSH_GREEN, W_BRUSH_RED,
	       W_BRUSH_CYAN, W_BRUSH_MAGENTA, W_BRUSH_YELLOW, W_BRUSH_WHITE,
	       W_BRUSH_GRAY, W_BRUSH_LTGRAY, W_BRUSH_DKGRAY,
	       W_BRUSH_LTBLUE, W_BRUSH_DKBLUE, WSCR_BRUSHES[];
extern HPEN    WSCR_PENS[];
extern int     WSCR_TERM_H, WSCR_TERM_W;
extern int     WSCR_NOCOLOR; /* JMP 29-04-00 */

extern int         WprFONTH, WprFONTW, WprHORZRES, WprVERTRES, WprOFFSETX, WprOFFSETY; /* JMP 31-03-2004 */
extern int         WprLOGX, WprLOGY, WprCOUR10H, WprCOUR10W;
extern int         WprMARGL, WprMARGH, WprNL, WprNC;
extern int         WprCurLine, WprCurCol;
extern int         WprChENLARGED, WprChCONDENSED;

extern char *WSCR_FONTNAME;
extern int  WSCR_FONTSIZE;


/* ====== FNS PROTOTYPES =========================== */

#ifndef SCRPROTO

extern int      WscrUnRegisterObjPg(char *, PAGE *);
extern PAGE     *WscrFindPage(HWND );
extern MENU     *WscrFindMenu(HMENU );
extern APPL     *WscrFindAppl(HMENU );
extern char     *WscrFindObjPtr(HWND );
extern WSCROLL  *WscrSCROLLToWSCROLL(SCROLL *);
extern U_ch     *WscrOemVsAnsi(U_ch  *, int);
extern U_ch     *WscrOemToAnsi(U_ch  *);
extern U_ch     *WscrAnsiToOem(U_ch  *);
extern U_ch     *WscrGetWindowTextOem(HWND);
extern int      WscrKeyScrToWin(int);
extern int      WscrKeyWinToScr(int);
extern int      WscrSetWindowTextOem(HWND, U_ch  *);
extern U_ch     *WscrGetWindowTextOem(HWND);
extern int      WscrTextOutOem(HDC, int, int, U_ch  *);
extern HWND     WscrCreateWscr(HMENU );
extern HWND     WapEditAppl(APPL *);
extern HWND     WscrMain(int, U_ch **);
extern HWND     WscrPgDisplay(PAGE *, int);
extern HWND     WscrObjDisplay(PAGE *, int , int );
extern HMENU    WscrCreateMenu();
extern HMENU    WscrCreateMenu(U_ch  *, U_ch , int);
extern int      WscrSetSubMenu(HMENU, HMENU, int);
extern HWND     WscrScrollCreate(HWND , int , int , int , int , int );
extern ATOM     WscrScrollRegister();
extern HANDLE   WscrGetFont(int );
extern HWND     WscrFindHwndPg(PAGE *);
extern HWND     WscrFindHwnd(char *);
extern HWND     WscrGetMainWnd();  /* JMP 25-01-00 */

extern U_ch     *WscrClipGet();

extern HWND     WscrCreateHeader(HWND, int, int, int, int );
extern HWND     WscrCreateToolbar(HWND , WTOOLBAR *);
extern HWND     WscrStatusCreate(HWND );

extern COLORREF WscrChooseColor(HWND, COLORREF, COLORREF *);
extern int WscrGetFilename(HWND , int, U_ch *, U_ch *, U_ch *, U_ch *);
extern int WscrChooseFont(HWND , LOGFONT *);
extern char **WscrListPrinters();

/* DDE */
HCONV               WscrDdeConnect();
HDDEDATA CALLBACK   DdeCallback();
HDDEDATA CALLBACK   DdeSrvCallback ();
unsigned char       *WscrDdeGetItem();
unsigned char       *WscrDdeGet();

extern WDDLG *WDDlgCreate();
extern WDDLG *WDDlgCreateInfoBox();

extern HPEN        WprCreateDashPen();
extern HDC         WscrEMFCreatehDC();
extern HMETAFILE   WprWMFEnd();
#ifdef WIN32
extern HENHMETAFILE WprEMFEnd();
#endif

HWND WscrCreateHiddenWindow(WSCRPROCFN);

#else /* SCRPROTO */

#include <s_prowin.h>
#include <s_prowis.h>

extern HWND WscrMain(int, U_ch **);

#endif /* SCRPROTO */

#endif // _WSCR_
#endif // DOS






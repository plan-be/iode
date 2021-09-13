#include "scr4w.h"
#include <commctrl.h>

WBUTTON WSCR_STD_BUTTONS[] = {
    {0, 1,         IDOK - WACT_SHIFT,       0,  "Ok"},
    {1, 1,         IDCANCEL - WACT_SHIFT,   0,  "Cancel"},
    {0, SCR_SPACE, 0,                       0,  0},
    {0, SCR_SPACE, 0,                       0,  0},
    {2, 1,         1,                       0,  "Test"},
    {3, 1,         2,                       0,  "Test"}
};

WTOOLBAR WSCR_STD_TOOLBAR = {
    0,                                     /* Nom du groupe        */
    sizeof(WSCR_STD_BUTTONS) / sizeof(WBUTTON), /* Nb de boutons   */
    WSCR_STD_BUTTONS,                      /* Attr                 */
    WSCR_TOOLBAR_SIZE, WSCR_TOOLBAR_SIZE,  /* buttons size         */
    WSCR_TOOLBAR_SIZE, WSCR_TOOLBAR_SIZE,  /* icons size           */
    WSCR_BITMAP_RC,                        /* Bitmap resource Nb   */
    9,                                     /* Nb icons in bitmap   */
    0,                                     /* Espace pour Windows  */
    0,                                     /* Nom du fichier bitmap*/
    1
};

WscrInitToolbar(WTOOLBAR *wtb)
{
    int         i;
    TBBUTTON    *tbb;

    if(wtb->wt_bitmaprc == 0) {
	wtb->wt_bitmaprc = WSCR_BITMAP_RC;
	wtb->wt_iconsnb = 9;
	wtb->wt_butsize[0] = wtb->wt_butsize[1] = WSCR_TOOLBAR_SIZE;
	wtb->wt_iconsize[0] = wtb->wt_iconsize[1] = WSCR_TOOLBAR_SIZE;
	}

    tbb = (TBBUTTON *)SCR_malloc(sizeof(TBBUTTON) * wtb->wt_nb);
    if(tbb == 0) return(-1);
    if(wtb->wt_tbb) SCR_free(wtb->wt_tbb);
    wtb->wt_tbb = (char *)tbb;
    for(i = 0 ; i < wtb->wt_nb ; i++) {
	tbb[i].iBitmap   = wtb->wt_buttons[i].wb_iconnb;
	tbb[i].idCommand = wtb->wt_buttons[i].wb_act + WACT_SHIFT;
	tbb[i].fsState   = TBSTATE_ENABLED;
	switch(wtb->wt_buttons[i].wb_attr) {
	    case SCR_SPACE        : tbb[i].fsStyle = TBSTYLE_SEP; break;
	    case SCR_CHECK_BUTTON : tbb[i].fsStyle = TBSTYLE_CHECK; break;
	    default               : tbb[i].fsStyle = TBSTYLE_BUTTON; break;
	    }
	tbb[i].dwData    = (DWORD)i;
	tbb[i].iString   = 0;
	}

    return(0);
}

HWND WscrCreateToolbar(HWND hWndParent, WTOOLBAR *wtb)
{
    HWND        hWnd;
    TOOLINFO    ToolInfo;

    if(wtb == 0) wtb = &WSCR_STD_TOOLBAR;

    WscrInitToolbar(wtb);
    hWnd = CreateToolbarEx(
	       hWndParent,             // parent
	       WS_CHILD | WS_BORDER | WS_VISIBLE | WS_GROUP | TBSTYLE_TOOLTIPS|TTS_ALWAYSTIP /*| CCS_ADJUSTABLE*/,   // window style
	       WSCR_TOOLBARID,         // toolbar id
	       wtb->wt_iconsnb,        // number of bitmaps
	       hInst,                  // mod instance
	       wtb->wt_bitmaprc,       // resource ID for bitmap
	       (TBBUTTON *)wtb->wt_tbb,// address of buttons
	       wtb->wt_nb,             // number of buttons
	       wtb->wt_butsize[0], wtb->wt_butsize[1],// width & height of buttons
	       wtb->wt_iconsize[0], wtb->wt_iconsize[1], // width & height of bitmaps
	       sizeof(TBBUTTON));      // structure size

    return(hWnd);
}

U_ch *WscrToolbarFindHelp(WTOOLBAR *wt, int act_nu)
{
    int         i;

    if(wt == 0) return((U_ch *)0);
    for(i = 0 ; i < wt->wt_nb ; i++)
	if(wt->wt_buttons[i].wb_act + WACT_SHIFT == act_nu)
	    return(wt->wt_buttons[i].wb_help);

    return((U_ch *)0);
}

/***** Return 0 if non traite, 1 si traite ********/

WscrToolbarNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    NMHDR       *nmh = (NMHDR *)lParam;
    TOOLTIPTEXT *ttt;
    int         wscrnb = WscrFindObjNb(hWnd);
    PAGE        *pg;
    APPL        *ap;
    U_ch        *txt;
    WTOOLBAR    *wt;

//    if(wParam != WSCR_TOOLBARID) return(0); /* NOT TOOLBAR */
    if(wscrnb < 0) return(0);

    switch(WSCR_HWNDS[wscrnb].sh_act) {
	case ACT_APPL_RUN :
	    ap = (APPL *)WSCR_HWNDS[wscrnb].sh_ptr;
	    if(ap->ap_nb_tb == 0) return(0);
	    wt = (WTOOLBAR *)SCR_PTR(ap->ap_toolb[0]);
	    break;
	default :
	    pg = (PAGE *)WSCR_HWNDS[wscrnb].sh_pg;
	    if(pg->pg_nb_tb == 0) return(0);
	    wt = (WTOOLBAR *)SCR_PTR(pg->pg_toolb[0]);
	    break;
	 }

    if(wt == 0) return(0);
    switch(nmh->code) {
	case TTN_NEEDTEXT:
	    ttt = (TOOLTIPTEXT *)lParam;
	    txt = WscrToolbarFindHelp(wt, ttt->hdr.idFrom);
	    SCR_strlcpy(ttt->szText, txt, 70);
	    SCR_OemToAnsi(ttt->szText, ttt->szText);
	 // WscrStatusSetText(hWnd, txt);
	    return(1);

	case TBN_QUERYDELETE:
	    // Toolbar customization -- can we delete this button?
	    return(0);

	case TBN_GETBUTTONINFO:
	    // The toolbar needs information about a button.
	    return(0);

	case TBN_QUERYINSERT:
	    // Can this button be inserted? Just say yo.
	    return(0);

	case TBN_CUSTHELP:
	    // Need to display custom help.
	    //MessageBox(hWnd, "This help is custom.",NULL, MB_OK);
	    break;

	case TBN_TOOLBARCHANGE:
	    // Done dragging a bitmap to the toolbar.
	    //SendMessage(hWndToolbar, TB_AUTOSIZE, 0L, 0L);
	    break;

	default:
	    return(0);
    }
    return(0);
}








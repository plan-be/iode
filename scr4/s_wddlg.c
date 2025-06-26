#include "scr4w.h"
#include <commctrl.h>

/*NH*/
static LPWORD lpwAlign (WORD *lpIn)
{
    ULONG ul;

    ul = (ULONG) lpIn;
    ul +=3;
    ul >>=2;
    ul <<=2;
    return (LPWORD) ul;
}

/*NH*/
static int nCopyAnsiToWideChar (LPWORD lpWCStr, LPSTR lpAnsiIn)
{

    int nChar = 0;

    do {
	*lpWCStr++ = (WORD) *lpAnsiIn;
	nChar++;
    } while (*lpAnsiIn++);

    return nChar;
}


/*NH*/
HANDLE WDDlgGetFont(int attr)
{
    if(W_SCRL_FONTB == 0) WscrCreateFonts(); /* JMP 16-04-06 */

    switch(attr) {
    case SCR_BOLD      :
	return(W_SCRL_FONTB);
    case SCR_ITALIC    :
	return(W_SCRL_FONTI);
    case SCR_BOLDITALIC:
	return(W_SCRL_FONTBI);
    case SCR_WIN_FIXED :
	return(W_SCRL_FONT_TERM);
    default            :
	return(W_SCRL_FONT);
    }
}

/* ========================================================================
Change la police de caractère du contrôle fld_nb de la boîte wddlg. Cette
fonction peut être appelée avant l'édition ou en cours d'édition (dans
une fonction utilisateur).

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles

&EN fld_nb = numéro du contrôle concerné ou -1 pour changer le font
    de tous les contrôles
&EN font = font normal par défaut ou choisi parmi :
&EN2 SCR_BOLD
&EN2 SCR_ITALIC
&EN2 SCR_BOLDITALIC
&EN2 SCR_WIN_FIXED

&EX
    WDDlgSetFont(wddlg, 1, SCR_BOLD);
&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAdd*(), WDDlgSet*()
================================================================== */

WDDlgSetFont(WDDLG *wddlg, int fld_nb, int font)
{
    int     i;

    if(fld_nb < 0)
	for(i = 0 ; i < wddlg->nctrls ; i++)
	    WDDlgSetFont1(wddlg, i, font);
    else
	WDDlgSetFont1(wddlg, fld_nb, font);

    return(0);
}

/*NH*/
WDDlgSetFont1(WDDLG *wddlg, int fld_nb, int font)
{
    HANDLE  hFont;
    HWND    hWndEdit;

    wddlg->fonts[fld_nb] = font;
    if(wddlg->hWnd == 0) return(0);

    hFont = WDDlgGetFont(font);
    hWndEdit = GetDlgItem(wddlg->hWnd, 100 + fld_nb);
    SendMessage(hWndEdit, WM_SETFONT, (WPARAM) hFont, TRUE);
    return(0);
}

/* ========================================================================
Change l'accessibilité du contrôle fld_nb de la boîte wddlg. Cette
fonction peut être appelée avant l'édition ou en cours d'édition (dans
une fonction utilisateur).

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles

&EN fld_nb = numéro du contrôle concerné ou -1 pour changer l'accès de tous
    les contrôles
&EN io = accès choisi parmi :
&EN2 SCR_INPUT
&EN2 SCR_OUTPUT

&EX
    WDDlgSetIo(wddlg, -1, SCR_INPUT);
&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAdd*(), WDDlgSet*()
================================================================== */

WDDlgSetIo(WDDLG *wddlg, int fld_nb, int io)
{
    int     i;

    if(fld_nb < 0)
	for(i = 0 ; i < wddlg->nctrls ; i++)
	    WDDlgSetIo1(wddlg, i, io);
    else
	WDDlgSetIo1(wddlg, fld_nb, io);

    return(0);
}

/*NH*/
WDDlgSetIo1(WDDLG *wddlg, int fld_nb, int io)
{
    HWND    hWndEdit;

    wddlg->ios[fld_nb] = io;
    if(wddlg->hWnd == 0) return(0);
    hWndEdit = GetDlgItem(wddlg->hWnd, 100 + fld_nb);
    EnableWindow(hWndEdit, (io == SCR_OUTPUT) ? FALSE : TRUE);
    return(0);
}

/* ========================================================================
Fixe la position des tabs du contrôle fld_nb LISTBOX de la
boîte wddlg. Cette fonction peut être appelée avant l'édition ou en
cours d'édition (dans une fonction utilisateur). Si une seule tabulation
est passée, elle est répétée automatiquement. Par défaut, on a une
tabulation tous les 16 points de boîte de dialogue, soit
    plus ou moins tous les 4 caractères.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fld_nb = numéro du contrôle LISTBOX
&EN ntabs = nombre de tabulations
&EN tabs = pointeur vers un tableau de positions en unités de boîte de
    dialogue

&EX
    int nTabs[] = {20, 50, 110};

    WDDlgSetTabs(wddlg, 0, 3, nTabs);
&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAdd*(), WDDlgSet*()
================================================================== */

WDDlgSetTabs(WDDLG *wddlg, int i, int ntabs, int *tabs)
{
    int     j;

    SCR_free(wddlg->tabs[i]);
    wddlg->tabs[i] = (int *)SCR_malloc((ntabs + 1) * sizeof(int));
    wddlg->tabs[i][0] = ntabs;
    for(j = 0 ; j < ntabs ; j++)
	wddlg->tabs[i][j + 1] = tabs[j];

    if(wddlg->hWnd == 0) return(0);
    SendDlgItemMessage(wddlg->hWnd, i + 100, LB_SETTABSTOPS,
		       (WPARAM)ntabs, (LPARAM)tabs);

    return(0);
}


/*NH*/
WDDLG *WDDlgGetDlg(HWND hWnd)
{
    WDDLG   *wddlg;
    char    text[40];
    HWND    hWndChild;

    hWndChild = GetDlgItem(hWnd, 99);
    GetWindowText(hWndChild, text, 30);
    sscanf(text, "%ld", &wddlg);
    return(wddlg);
}

/* ========================================================================
Affiche le champ fld_nb de la boîte de dialogue wddlg. Cette fonction
doit être appelée par les fonctions utilisateurs qui modifient le
contenu des contrôles en cours d'édition.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fld_nb = numéro d'ordre du contrôle à afficher

&EX
    TestFn(WDDLG *wddlg, int fld_nb, char *txt, int nNotif)
    {
	static int  i = 0;

	if(fld_nb == 1 && nNotif == WDDLG_CHANGED) { // LISTBOX
	    // Change la valeur d'un contrôle statique texte
	    sprintf(testtext, "Passage %d", i++);
	    // Affiche les champs modifiés
	    WDDlgDisplayField(wddlg, fld_nb);
	    }

	return(0);
    }

&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAdd*(), WDDlgSet*(), WDDlgDisplayFlds()
================================================================== */

WDDlgDisplayField(WDDLG *wddlg, int fld_nb)
{
    int     j;
    HWND    hWnd = wddlg->hWnd, hWndEdit;
    U_ch    **opts, *ptr;
    int     msg1, msg2;

    if(hWnd == 0) return(0);
    wddlg->changing = 1;
    hWndEdit = GetDlgItem(hWnd, 100 + fld_nb);
    WDDlgSetFont(wddlg, fld_nb, wddlg->fonts[fld_nb]);
    WDDlgSetIo(wddlg, fld_nb, wddlg->ios[fld_nb]);
    switch(wddlg->types[fld_nb]) {
    case WDDLG_EDIT     :
    case WDDLG_BUTTON   :
    case WDDLG_STATIC   :
    case WDDLG_GROUPBOX :
	SCR_OemToAnsi(wddlg->ptrs[fld_nb], wddlg->ptrs[fld_nb]);
	SetDlgItemText(hWnd, fld_nb + 100, wddlg->ptrs[fld_nb]);
	SCR_AnsiToOem(wddlg->ptrs[fld_nb], wddlg->ptrs[fld_nb]);
	break;
    case WDDLG_LISTBOX  :
    case WDDLG_COMBOBOX :
	if(wddlg->types[fld_nb] == WDDLG_LISTBOX) msg2 = LB_SETSEL;
	else                            msg2 = 0;  /* FAUX!!! */
	opts = (U_ch **)wddlg->exts[fld_nb];
	if(opts == 0) break;
	for(j = 0 ; opts[j] ; j++) {
	    if(wddlg->ptrs[fld_nb][j] ==
		    SendDlgItemMessage(hWnd, fld_nb + 100, LB_GETSEL, j, 0))
		continue;
	    SendDlgItemMessage(hWnd, fld_nb + 100, msg2, (wddlg->ptrs[fld_nb])[j], (LONG)j);
	}
	break;

    case WDDLG_SLIDE :
	SendDlgItemMessage(hWnd, fld_nb + 100, TBM_SETPOS,
			   (WPARAM)TRUE, (LPARAM)(*(int *)wddlg->ptrs[fld_nb]));
	break;

    case WDDLG_PROGRESS :
	SendDlgItemMessage(hWnd, fld_nb + 100, PBM_SETPOS,
			   (WPARAM)(*(int *)wddlg->ptrs[fld_nb]), (LPARAM)0);
	// Debug("Wparam=%d\n", (WPARAM)(*(int *)wddlg->ptrs[fld_nb]));
	break;
    case WDDLG_LISTVIEW :
	opts = (U_ch **)wddlg->exts[fld_nb];
	if(opts == 0) break;
	for(j = 1 ; opts[j] ; j++) {
	    if(wddlg->ptrs[fld_nb][j - 1]) {
		ListView_SetItemState(hWndEdit, j - 1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	    }
	    else {
		ListView_SetItemState(hWndEdit, j - 1, 0, LVIS_SELECTED);
	    }
	}
	break;
    }

    wddlg->changing = 0;
    SCR_wdispatch(); /* JMP 01-02-03 */
    return(0);
}

/* ========================================================================
Affiche tous les champs de la boîte de dialogue wddlg. Cette fonction
doit être appelée par les fonctions utilisateurs qui modifient le
contenu des contrôles en cours d'édition.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles

&EX
    TestFn(WDDLG *wddlg, int fld_nb, char *txt, int nNotif)
    {
	static int  i = 0;

	if(fld_nb == 1 && nNotif == WDDLG_CHANGED) { // LISTBOX
	    // Change la valeur d'un contrôle statique texte
	    sprintf(testtext, "Passage %d", i++);
	    // Affiche les champs modifiés
	    WDDlgDisplayFlds(wddlg);
	    }

	return(0);
    }

&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAdd*(), WDDlgSet*()
================================================================== */

WDDlgDisplayFlds(WDDLG *wddlg)
{
    int     i;

    for(i = 0 ; i < wddlg->nctrls; i++)
	WDDlgDisplayField(wddlg, i);

    return(0);
}

/*NH*/
WDDlgReadFlds(WDDLG *wddlg)
{
    int     i, j;
    HWND    hWnd = wddlg->hWnd, hWndEdit;
    U_ch    **opts, *ptr;

    for(i = 0 ; i < wddlg->nctrls; i++) {
	hWndEdit = GetDlgItem(hWnd, 100 + i);
	switch(wddlg->types[i]) {
	case WDDLG_EDIT     :
	    GetWindowText(hWndEdit, wddlg->ptrs[i], (int)wddlg->exts[i]);
	    SCR_AnsiToOem(wddlg->ptrs[i], wddlg->ptrs[i]);
	    break;

	case WDDLG_LISTBOX  :
	    opts = (U_ch **)wddlg->exts[i];
	    if(opts == 0) break;
	    for(j = 0 ; opts[j] ; j++) {
		wddlg->ptrs[i][j] =
		    SendDlgItemMessage(hWnd, i + 100, LB_GETSEL, j, 0);
	    }
	    break;
	case WDDLG_LISTVIEW  :
	    opts = (U_ch **)wddlg->exts[i];
	    if(opts == 0) break;
	    for(j = 1 ; opts[j] ; j++) {
		wddlg->ptrs[i][j - 1] =
		    ListView_GetItemState(hWndEdit, j - 1, LVIS_SELECTED);
	    }
	    break;

	case WDDLG_SLIDE :
	    *((int *)wddlg->ptrs[i]) =
		SendDlgItemMessage(hWnd, i + 100, TBM_GETPOS, 0, 0);
	    break;
	}
    }
    return(0);
}

//FARPROC WDDLG_STATICFN_DFT = 0;
//FARPROC WDDLG_STATICFN = 0;
WNDPROC WDDLG_STATICFN_DFT = 0;
WNDPROC WDDLG_STATICFN = 0;

/*NH*/
WINAPI WDDlgStaticProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    if(wMsg == WM_SETCURSOR) {
	HCURSOR hCursor;

//        hCursor = LoadCursor(0, IDC_WAIT);
	hCursor = LoadCursor(hInst, MAKEINTRESOURCE(26004));
	hCursor = SetCursor(hCursor);
	return(0L);
    }

    return(CallWindowProc(WDDLG_STATICFN_DFT, hWnd, wMsg, wParam, lParam));
}

/*NH*/
WDDlgInit(HWND hWnd, WDDLG *wddlg)
{
    HANDLE  hFont;
    int     i, msg1, msg2, j, tabs = 16, cx, cy;
    U_ch    **opts, *ptr, buf[40];
    RECT    rect;
    HWND    hWndEdit;
    extern WNDPROC WDDLG_STATICFN;

    sprintf(buf, "%ld", (long)wddlg);
    SetDlgItemText(hWnd, 99, buf);

//    WDDLG_CUR = wddlg;
    wddlg->hWnd = hWnd;
    if(hMainWnd == 0) hMainWnd = hWnd;

    for(i = 0 ; i < wddlg->nctrls; i++) {
	WDDlgSetHwndVisible(wddlg, i, !(wddlg->attrs[i] & WDDLG_HIDDEN));
	switch(wddlg->types[i]) {
	case WDDLG_EDIT     :
	    j = (int) wddlg->exts[i];
	    SendDlgItemMessage(hWnd, i + 100, EM_LIMITTEXT, j, 0L);
	    break;
	case WDDLG_LISTBOX  :
	case WDDLG_COMBOBOX :
	    if(wddlg->types[i] == WDDLG_LISTBOX) {
		msg1 = LB_ADDSTRING;
		msg2 = LB_SETSEL;
	    }
	    else {
		msg1 = CB_ADDSTRING;
		msg2 = 0; /* ???? */
	    }
	    opts = (U_ch **)wddlg->exts[i];
	    if(opts == 0) break;
	    for(j = 0 ; opts[j] ; j++) {
		SendDlgItemMessage(hWnd, i + 100, msg1, 0, (LONG)opts[j]);
		SendDlgItemMessage(hWnd, i + 100, msg2, (wddlg->ptrs[i])[j], (LONG)j);
	    }
	    if(wddlg->tabs[i])
		SendDlgItemMessage(hWnd, i + 100, LB_SETTABSTOPS,
				   (WPARAM)wddlg->tabs[i][0],
				   (LPARAM)(1 + wddlg->tabs[i]));
	    else
		SendDlgItemMessage(hWnd, i + 100, LB_SETTABSTOPS,
				   (WPARAM)1, (LPARAM)&tabs);
	    break;

	case WDDLG_ICON :
	case WDDLG_BMP :
	    if(wddlg->fns[i] == 0) break;
	    hWndEdit = GetDlgItem(wddlg->hWnd, 100 + i);
	    if(WDDLG_STATICFN == 0)
		WDDLG_STATICFN = MakeProcInstance((FARPROC) WDDlgStaticProc, hInst);
	    if(WDDLG_STATICFN_DFT == 0)
		//WDDLG_STATICFN_DFT = (FARPROC)GetWindowLongPtr(hWndEdit, GWL_WNDPROC);
		WDDLG_STATICFN_DFT = (FARPROC)GetWindowLongPtr(hWndEdit, GWLP_WNDPROC);
	    //SetWindowLongPtr(hWndEdit, GWL_WNDPROC, (long)WDDLG_STATICFN);
	    SetWindowLongPtr(hWndEdit, GWLP_WNDPROC, (long)WDDLG_STATICFN);
	    break;

	case WDDLG_SLIDE :
	    SendDlgItemMessage(hWnd, i + 100, TBM_SETRANGE,
			       (WPARAM)TRUE, (LPARAM)(wddlg->exts[i]));
	    break;
	case WDDLG_PROGRESS :
	    SendDlgItemMessage(hWnd, i + 100, PBM_SETRANGE,
			       (WPARAM)0, (LPARAM)(wddlg->exts[i]));
	    break;
	case WDDLG_LISTVIEW :
	    WDDlgInitListView(wddlg, i);
	    break;
	}
    }

    WDDlgDisplayFlds(wddlg);

    /* ReSize and Move Window */
    GetWindowRect(hWnd, &rect);
    cx  = rect.right - rect.left;
    cy  = rect.bottom - rect.top;
    if(wddlg->x < 0) wddlg->x = (GetSystemMetrics(SM_CXSCREEN) - cx) / 2;
    if(wddlg->x < 0) wddlg->x = 0;
    if(wddlg->y < 0) wddlg->y = (GetSystemMetrics(SM_CYSCREEN) - cy) / 2;
    if(wddlg->y < 0) wddlg->y = 0;

    MoveWindow(hWnd, wddlg->x, wddlg->y, cx, cy, FALSE);

//    WDDlgCreateTooltip(wddlg); /* JMP 09-12-97 */
    return(0);
}

/*NH*/
WDDlgCommand(HWND hWnd, UINT iId, HWND hCtl, UINT nNotif)
{
    int     fld_nb = iId - 100, rc = 0;
#ifdef SCRCPP
    int     (*fn)(_wddlg_ *, int, char *, UINT);
#else
    int     (*fn)();
#endif
    WDDLG   *wddlg;

    wddlg = WDDlgGetDlg(hWnd);
    if(iId == IDCANCEL) {
	wddlg->rc = -1;
	if(wddlg->escfn) wddlg->rc = (*wddlg->escfn)(wddlg);
	return(wddlg->rc);
    }
    if(iId == IDOK) {
	wddlg->rc = 0;
	if(wddlg->okfn) wddlg->rc = (*wddlg->okfn)(wddlg);
	return(wddlg->rc);
    }

    switch(nNotif) {
    case EN_KILLFOCUS :
    case BN_KILLFOCUS :
    case LBN_KILLFOCUS :
    case NM_KILLFOCUS :
	nNotif = WDDLG_KILLFOCUS;
	break;
    case BN_SETFOCUS :
    case EN_SETFOCUS :
    case LBN_SETFOCUS :
    case NM_SETFOCUS :
	nNotif = WDDLG_SETFOCUS;
	break;
    case BN_CLICKED :
	nNotif = WDDLG_CLICKED;
	break;
    case LBN_DBLCLK :
    case NM_DBLCLK :
	nNotif = WDDLG_DBLCLK;
	break;
    case LBN_SELCHANGE :
    case LVN_ITEMCHANGED :
	if(wddlg->changing) return(0);
	nNotif = WDDLG_CHANGED;
	break;
    case WDDLG_CHAR :
	break;
    default :
	return(0);
    }

    if(fld_nb < 0 || fld_nb >= wddlg->nctrls) return(0);
    fn = wddlg->fns[fld_nb];
    if(fn == 0) {
	if(nNotif == WDDLG_DBLCLK || nNotif == WDDLG_CLICKED) return(1);
	else return(0);
    }

    WDDlgReadFlds(wddlg);
    rc = (*fn)(wddlg, fld_nb, wddlg->ptrs[fld_nb], nNotif);
//    WDDlgDisplayFlds(wddlg);
    wddlg->rc = rc;
    return(rc);
}


/*NH*/
BOOL CALLBACK WDDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    WDDLG   *wddlg;
    UINT    iId;

    switch(wMsg) {
    case WM_COMMAND :
	if(WDDlgCommand(hWnd, LOWORD(wParam), (HWND)(lParam), HIWORD(wParam)))
	    goto fin;
	//     if(wParam == IDOK) goto fin;
	return(FALSE);

    case WM_VKEYTOITEM :
	if(LOWORD(wParam) != ' ' && LOWORD(wParam) != 'x') return(-1);
	iId = GetDlgCtrlID((HWND)lParam);
	if(WDDlgCommand(hWnd, iId, (HWND)lParam, (UINT)WDDLG_CHAR)) goto fin;
	return(-2);

    case WM_NOTIFY:
	if(WDDlgNotifyHandler(hWnd, wParam, lParam)) goto fin;
	return(FALSE);

    case WM_SYSCOMMAND :
	if(wParam == SC_CLOSE) goto fin;
	return(FALSE);

    case WM_INITDIALOG :
	WDDlgInit(hWnd, (WDDLG *)lParam);
	return(TRUE);

//        case WM_SIZE :
//            WscrResizeCtrl(hWnd, wParam, lParam);
//            break;

    default :
	break;
    }

    return(FALSE);

fin:
    wddlg = WDDlgGetDlg(hWnd);
    WDDlgReadFlds(wddlg);
    if(wddlg->endfn && (*wddlg->endfn)(wddlg)) return(FALSE);

    if(hMainWnd == hWnd) hMainWnd = 0;
    EndDialog(hWnd, TRUE);
    if(hMainWnd == hWnd) hMainWnd = 0; /* JMP 13-04-00 */
//    WDDlgClearHook();  /* JMP 09-12-97 */
    return(TRUE);
}

/* ============ FONCTIONS DE CREATION ET DE MODIFICATION ===========
Crée une structure de définition d'une boîte de dialogue dynamique.

&EN x = position, en pixels, du coin supérieur gauche de la boîte de
    dialogue. Si cette valeur est négative, la boîte est centrée
    horizontalement.
&EN y = position, en pixels, du coin supérieur gauche de la boîte de
    dialogue. Si cette valeur est négative, la boîte est centrée
    verticalement.
&EN cx = taille, en coordonnées de boîte de dialogue (!), de la boîte de
    dialogue.
&EN cy = taille, en coordonnées de boîte de dialogue (!), de la boîte de
    dialogue.
&EN title (optionnel) = titre de la boîte de dialogue. Si title est nul
    il n'y a pas de titre dans la boîte de dialogue.

&EX
TestDialog()
{
    char    *WDDlgCreate(), *wddlg, **tmp, buf[30], *sel;
    int     rc, pos = 5;

    tmp = SCR_vtom("Option 1\tSuite,\t1\t2\t3\t4\t5\t6,
		    Option 2\tSuite,Option 3,Truc,Brol,
		    x\tSuite\tSuite,x\tSuite\t\tSuite,
		    y\tSuite\tSuite,dfsdfsdf", ',');
    sel = SCR_malloc(SCR_tbl_size(tmp));

    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    WDDlgAddButton(wddlg, 5, 5, 30, 10, "SelectAll", TestButton, 2);
    WDDlgAddButton(wddlg, 40, 5, 50, 10, "Bouton 2", TestButton, 0);
    sel[0] = sel[3] = 1;
    WDDlgAddListbox(wddlg, 5, 20, 85, 50, tmp, sel, TestButton, 1);
    WDDlgAddStatic(wddlg, 5, 95, 50, 12, testtext, 0);
    strcpy(testtext, "Test test");
    WDDlgAddEdit(wddlg, 5, 115, 50, 14, testtext, 20, TestButton, 0);
    WDDlgAddIcon(wddlg, 5, 135, 26002, 0L);
    WDDlgAddHSlide(wddlg, 5, 160, 80, 20, &pos, 0, 10, 0);
    WDDlgAddBmp(wddlg, 5, 190, 26003, 0L);
    WDDlgEdit(wddlg);
    WDDlgFree(wddlg);
    SCR_free(tmp);
    SCR_free(sel);
}
&TX

&RT pointeur vers la structure allouée
&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDLG *WDDlgCreate(int x, int y, int cx, int cy, U_ch *title)
{
    WDDLG   *wddlg;
    WORD    *p;
    DWORD   lStyle;
    int     nchar;
    U_ch    *ptr;

    InitCommonControls();
    wddlg = (WDDLG *)SCR_malloc(sizeof(WDDLG));
    wddlg->wdlg = SCR_malloc(2048);
    wddlg->x = x;
    wddlg->y = y;
    wddlg->rc = -1;
    p = (WORD *) wddlg->wdlg;

    /* start to fill in the dlgtemplate information, addressing by WORDs */
    lStyle = DS_3DLOOK | DS_MODALFRAME | WS_POPUPWINDOW | // DS_SETFONT | /* JMP 09-12-97 */
	     WS_SYSMENU | DS_ABSALIGN | WS_CLIPSIBLINGS | WS_DLGFRAME;
    if(title)
	lStyle |= WS_CAPTION | WS_SYSMENU;

    *p++ = 1;          // DlgVer
    *p++ = 0xFFFF;     // Signature
    *p++ = 0;          // LOWORD HelpID
    *p++ = 0;          // HIWORD HelpID
    *p++ = 0;          // LOWORD(lExtendedStyle)
    *p++ = 0;          // HIWORD(lExtendedStyle)

    *p++ = LOWORD (lStyle);
    *p++ = HIWORD (lStyle);

    *p++ = 0;         // NumberOfItems
    if(x < 0) *p++ = 0;  // x
    else      *p++ = x;
    if(y < 0) *p++ = 0;  // y
    else      *p++ = y;
    *p++ = cx;        // cx
    *p++ = cy;        // cy
    *p++ = 0;         // Menu
    *p++ = 0;         // Class

    /* copy the title of the dialog */
    if(title) {
	SCR_OemToAnsi(title, title);
	nchar = nCopyAnsiToWideChar (p, TEXT(title));
	SCR_AnsiToOem(title, title);
	p += nchar;
    }
    else
	*p++ = 0;  // Null title

    /* Font information because of DS_SETFONT */
//    *p++ = 8;     // point size
//    nchar = nCopyAnsiToWideChar (p, TEXT("MS Sans Serif")); // Face name
//    p += nchar;

    wddlg->p = p;

    WDDlgAddStatic(wddlg, 0, 0, 0, 0, 0L);

    return(wddlg);
}

/*NH*/
WDDlgAddChild(WDDLG *wddlg, WORD type, DWORD lStyle, int x, int y,
	      int cx, int cy, char *ptr, char *ext,
	      int (*fn)(WDDLG *, int, char *, int),
	      int attr)
{
    WORD    *p;
    int     nctrls, nchar;

    p = (WORD *)wddlg->wdlg;
    nctrls = p[8];
    p[8]++;

    p = wddlg->p;
    p = lpwAlign(p);
    *p++ = 0;          // LOWORD (lHelpID)
    *p++ = 0;          // HIWORD (lHelpID)
    *p++ = 0;          // LOWORD (lExtendedStyle)
    *p++ = 0;          // HIWORD (lExtendedStyle)

    if(attr & WDDLG_HIDDEN) lStyle &= ~WS_VISIBLE;
    *p++ = LOWORD (lStyle);
    *p++ = HIWORD (lStyle);

    *p++ = x;         // x
    *p++ = y;         // y
    *p++ = cx;        // cx
    *p++ = cy;        // cy
    if(nctrls == 0) *p++ = 99;    // LOWORD (Control ID)
    else            *p++ = 100 + wddlg->nctrls; // LOWORD (Control ID)

    *p++ = 0;         // HIWORD (Control ID)

    switch(type) {
    case WDDLG_SLIDE :
	nchar = nCopyAnsiToWideChar(p, TEXT(TRACKBAR_CLASS));
	p += nchar;
	*p++ = 0;           // Null title
	break;
    case WDDLG_PROGRESS :
	nchar = nCopyAnsiToWideChar(p, TEXT(PROGRESS_CLASS));
	p += nchar;
	*p++ = 0;           // Null title
	break;
    case WDDLG_LISTVIEW :
	nchar = nCopyAnsiToWideChar(p, TEXT(WC_LISTVIEW));
	p += nchar;
	*p++ = 0;           // Null title
	break;
    case WDDLG_ICON :
    case WDDLG_BMP :
	*p++ = 0xFFFF;      // Class
	*p++ = WDDLG_STATIC;     // Type
	*p++ = 0xFFFF;      // Resource in title
	*p++ = (WORD)ptr;   // Resource in title
	*p++ = 0x0000;      // Resource in title
	break;
    case WDDLG_GROUPBOX :
	*p++ = 0xFFFF;      // Class
	*p++ = WDDLG_BUTTON;     // Type
	*p++ = 0;
	break;
    default :
	*p++ = 0xFFFF;      // Class
	*p++ = type;        // ListBox, ...
	*p++ = 0;           // Null title
	break;
    }

    *p++ = 0;  // advance pointer over nExtraStuff WORD

    wddlg->p = p;
    if(nctrls) {
	wddlg->ptrs = (char **) SCR_realloc(wddlg->ptrs, sizeof(char *),
					    wddlg->nctrls, wddlg->nctrls + 1);
	wddlg->ptrs[wddlg->nctrls] = ptr;

	wddlg->exts = (char **) SCR_realloc(wddlg->exts, sizeof(char *),
					    wddlg->nctrls, wddlg->nctrls + 1);
	wddlg->exts[wddlg->nctrls] = ext;

	wddlg->types = (int *) SCR_realloc(wddlg->types, sizeof(int),
					   wddlg->nctrls, wddlg->nctrls + 1);
	wddlg->types[wddlg->nctrls] = type;

	wddlg->fonts = (int *) SCR_realloc(wddlg->fonts, sizeof(int),
					   wddlg->nctrls, wddlg->nctrls + 1);
	wddlg->fonts[wddlg->nctrls] = 0;

	wddlg->attrs = (int *) SCR_realloc(wddlg->attrs, sizeof(int),
					   wddlg->nctrls, wddlg->nctrls + 1);
	wddlg->attrs[wddlg->nctrls] = attr;

	wddlg->ios = (int *) SCR_realloc(wddlg->ios, sizeof(int),
					 wddlg->nctrls, wddlg->nctrls + 1);
	wddlg->ios[wddlg->nctrls] = 0;

	wddlg->tabs = (int **) SCR_realloc(wddlg->tabs, sizeof(int *),
					   wddlg->nctrls, wddlg->nctrls + 1);
	wddlg->tabs[wddlg->nctrls] = 0;

	wddlg->fns = (char *) SCR_realloc(wddlg->fns, sizeof(char *),
					  wddlg->nctrls, wddlg->nctrls + 1);
	wddlg->fns[wddlg->nctrls] = fn;

	wddlg->nctrls++;
    }

    return(0);
}

/* ===================================================================
Ajoute un bouton à une boîte de dialogue dynamique.

&EN wddlg = pointeur vers la structure de définition de la boîte de
    dialogue dynamique créée par WDDlgCreate()
&EN (x, y) = position, en coordonnées de boîte de dialogue, du coin
    supérieur gauche du contrôle
&EN (cx, cy) = taille du contrôle, en coordonnées de boîte de dialogue
&EN text = pointeur vers le texte du bouton (valeur du contrôle)
&EN fn = pointeur vers la fonction utilisateur de gestion des évènements
    du contrôle
&EN attr = attribut supplémentaire, combinaison de :
&EN2 0 pour un bouton normal,
&EN2 WDDLG_DEFBUTTON pour un bouton "par défaut"
&EN2 WDDLG_HIDDEN pour cacher le contrôle

&IT Syntaxe de fn
─────────────────

La fonction utilisateur a la syntaxe suivante :
&CO
    int fn(WDDLG *wddlg, int fld_nb, char *txt, int nNotif)
&TX
où :
&EN wddlg est le pointeur vers la structure WDDLG courante
&EN fld_nb est le numéro du contrôle courant (où s'est produit
    l'évènement)
&EN txt est la valeur du contrôle (= text de WDDlgAddButton)
&EN nNotif est le message Windows reçu dans le contrôle

Les messages suivants donnent lieu à un passage par cette fonction dans
le cas des boutons :

&EN WDDLG_CLICKED : bouton cliqué
&EN WDDLG_KILLFOCUS : le bouton est quitté (touche TAB par exemple)
&EN WDDLG_SETFOCUS : le focus arrive sur ce bouton

Si la valeur de retour de la fonction est 0, l'édition continue. Sinon,
la boîte de dialogue est quittée et la valeur de fn est retournée par
la fonction WDDlgEdit().

&EX
    WDDlgAddButton(wddlg, 5, 5, 30, 10, "Ok", Button1Fn,
						   WDDLG_DEFBUTTON);
    WDDlgAddButton(wddlg, 40, 5, 50, 10, "Bouton 2", Button2Fn, 0);
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgAddButton(WDDLG *wddlg, int x, int y, int cx, int cy,
	       U_ch *text,
	       int (*fn)(WDDLG *, int, char *, int),
	       int attr)
{
    DWORD lStyle = BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD | WS_TABSTOP;

    if(attr & WDDLG_DEFBUTTON) lStyle |= BS_DEFPUSHBUTTON;

    WDDlgAddChild(wddlg, WDDLG_BUTTON, lStyle, x, y, cx, cy, (char *) text, (char *)0, fn, attr);
    return(0);
}

/* ===================================================================
Ajoute une LISTBOX à une boîte de dialogue dynamique.

&EN wddlg = pointeur vers la structure de définition de la boîte de
    dialogue dynamique créée par WDDlgCreate()
&EN (x, y) = position, en coordonnées de boîte de dialogue, du coin
    supérieur gauche du contrôle
&EN (cx, cy) = taille du contrôle, en coordonnées de boîte de dialogue
&EN opts = pointeur vers un tableau de textes
&EN sel = pointeur vers un tableau de char contenant 0 pour les options
    non sélectionnées et 1 pour les options sélectionnées
&EN fn = pointeur vers la fonction utilisateur de gestion des évènements
    du contrôle
&EN attr = attribut supplémentaire : combinaison de :
&EN2 0 pour une LISTBOX à sélection unique
&EN2 WDDLG_MULTISEL pour une LISTBOX à sélections multiples
&EN2 WDDLG_HIDDEN pour cacher le contrôle

&IT Syntaxe de fn
─────────────────

La fonction utilisateur a la syntaxe suivante :
&CO
    int fn(WDDLG *wddlg, int fld_nb, char *txt, int nNotif)
&TX
où :
&EN wddlg est le pointeur vers la structure WDDLG courante
&EN fld_nb est le numéro du contrôle courant (où s'est produit
    l'évènement)
&EN txt est la valeur du contrôle (= sel de WDDlgAddListbox)
&EN nNotif est le message Windows reçu dans le contrôle

Les messages suivants donnent lieu à un passage par cette fonction dans
le cas des LISTBOX :

&EN WDDLG_DBLCLK    : double clic sur une option
&EN WDDLG_KILLFOCUS : perte du focus
&EN WDDLG_SETFOCUS  : gain du focus
&EN WDDLG_CHANGED : changement de sélection

Si la valeur de retour de la fonction est 0, l'édition continue. Sinon,
la boîte de dialogue est quittée et la valeur de fn est retournée par
la fonction WDDlgEdit().

&EX
    tmp = SCR_vtom("Option 1\tSuite,\t1\t2\t3\t4\t5\t6,
		    Option 2\tSuite,Option 3,Truc,Brol,
		    x\tSuite\tSuite,x\tSuite\t\tSuite,
		    y\tSuite\tSuite,dfsdfsdf", ',');
    sel = SCR_malloc(SCR_tbl_size(tmp));
    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    sel[0] = sel[3] = 1;
    WDDlgAddListbox(wddlg, 5, 20, 85, 50, tmp, sel, ListboxFn,
						    WDDLG_MULTISEL);
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgAddListbox(WDDLG *wddlg, int x, int y, int cx, int cy, U_ch **opts,
		char *sel,
		int (*fn)(WDDLG *, int, char *, int),
		int attr)
{
    DWORD   lStyle = LBS_USETABSTOPS | WS_VISIBLE | WS_CHILD | WS_TABSTOP |
		     LBS_NOTIFY | WS_BORDER | WS_VSCROLL | LBS_WANTKEYBOARDINPUT;
    U_ch    **tbl;
    int     i;

    if(attr & WDDLG_MULTISEL) lStyle |= LBS_MULTIPLESEL | LBS_EXTENDEDSEL;

    tbl = SCR_copy_tbl(opts);
    for(i = 0 ; tbl[i] ; i++)
	SCR_OemToAnsi(tbl[i], tbl[i]);
    WDDlgAddChild(wddlg, WDDLG_LISTBOX, lStyle, x, y, cx, cy, (char *) sel, (char *)tbl, fn, attr);
    return(0);
}

/* ===================================================================
Ajoute un contrôle statique (texte) à une boîte de dialogue dynamique.

&EN wddlg = pointeur vers la structure de définition de la boîte de
    dialogue dynamique créée par WDDlgCreate()
&EN (x, y) = position, en coordonnées de boîte de dialogue, du coin
    supérieur gauche du contrôle
&EN (cx, cy) = taille du contrôle, en coordonnées de boîte de dialogue
&EN text = pointeur vers le texte du contrôle

&EX
    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    WDDlgAddStatic(wddlg, 5, 20, 85, 50, "Minimum");
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgAddStatic(WDDLG *wddlg, int x, int y, int cx, int cy,
	       U_ch *text)
{
    DWORD   lStyle;

    if(text == 0) // Controle spécial pointeur wddlg
	lStyle = WS_CHILD | SS_NOTIFY;
    else
	lStyle = SS_LEFTNOWORDWRAP | WS_CHILD | WS_VISIBLE | WS_GROUP ;

    WDDlgAddChild(wddlg, WDDLG_STATIC, lStyle, x, y, cx, cy, (char *) text, (char *)0, 0, 0);
    return(0);
}

/* ===================================================================
Ajoute un icône dans une boîte de dialogue dynamique.
Si fn est non nulle, le contrôle est "clickable".

&EN wddlg = pointeur vers la structure de définition de la boîte de
    dialogue dynamique créée par WDDlgCreate()
&EN (x, y) = position, en coordonnées de boîte de dialogue, du coin
    supérieur gauche du contrôle
&EN res = identifiant de la ressource

&EX
    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    WDDlgAddIcon(wddlg, 5, 20, 26001, 0L);
&TX
    Dans le fichier .rc
&CO
    26001 ICON "filename.ico"
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgAddIcon(WDDLG *wddlg, int x, int y, int res,
	     int (*fn)(WDDLG *, int, char *, int))

{
    DWORD   lStyle;

    lStyle = SS_ICON | WS_CHILD | WS_VISIBLE;
    if(fn) lStyle |= SS_NOTIFY;

    WDDlgAddChild(wddlg, WDDLG_ICON, lStyle, x, y, 0, 0, (char *)res, (char *)0, fn, 0);
//    lStyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_ICON;
//    WDDlgAddChild(wddlg, WDDLG_ICON, lStyle, x, y, 0, 0, (char *)res, (char *)0, fn, 0);
    return(0);
}

/* ===================================================================
Ajoute un BITMAP dans une boîte de dialogue dynamique.
Si fn est non nulle, le contrôle est "clickable".

&EN wddlg = pointeur vers la structure de définition de la boîte de
    dialogue dynamique créée par WDDlgCreate()
&EN (x, y) = position, en coordonnées de boîte de dialogue, du coin
    supérieur gauche du contrôle
&EN res = identifiant de la ressource

&EX
    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    WDDlgAddBmp(wddlg, 5, 20, 26002, 0L);
&TX
    Dans le fichier .rc
&CO
    26002 BITMAP "filename.bmp"
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgAddBmp(WDDLG *wddlg, int x, int y, int res,
	    int (*fn)(WDDLG *, int, char *, int))
{
    DWORD   lStyle;

    lStyle = SS_BITMAP | WS_CHILD | WS_VISIBLE;
    if(fn) lStyle |= SS_NOTIFY;
    WDDlgAddChild(wddlg, WDDLG_BMP, lStyle, x, y, 0, 0, (char *)res, (char *)0, fn, 0);
    return(0);
}

/* ===================================================================
Ajoute un GROUPBOX dans une boîte de dialogue dynamique.

&EN wddlg = pointeur vers la structure de définition de la boîte de
    dialogue dynamique créée par WDDlgCreate()
&EN (x, y) = position, en coordonnées de boîte de dialogue, du coin
    supérieur gauche du contrôle
&EN (cx, cy) = taille, en coordonnées de boîte de dialogue, du
    group box
&EN title = titre du groupbox

&EX
    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    WDDlgAddGroupbox(wddlg, 5, 20, 50, 50, "Fonts");
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgAddGroupbox(WDDLG *wddlg, int x, int y, int cx, int cy, U_ch *title)
{
    DWORD   lStyle;

    lStyle = BS_GROUPBOX | WS_CHILD | WS_VISIBLE | WS_TABSTOP;
    WDDlgAddChild(wddlg, WDDLG_GROUPBOX, lStyle, x, y, cx, cy, (char *)title, (char *)0, 0, 0);
    return(0);
}

/* ===================================================================
Ajoute un contrôle d'édition de texte d'une seule ligne à une boîte de
dialogue dynamique.

&EN wddlg = pointeur vers la structure de définition de la boîte de
    dialogue dynamique créée par WDDlgCreate()
&EN (x, y) = position, en coordonnées de boîte de dialogue, du coin
    supérieur gauche du contrôle
&EN (cx, cy) = taille du contrôle, en coordonnées de boîte de dialogue
&EN text = pointeur vers le texte du contrôle (valeur du contrôle)
&EN maxlen = longueur maximum du texte
&EN fn = pointeur vers la fonction utilisateur de gestion des évènements
    du contrôle
&EN attr = attribut supplémentaire (non utilisé)

&IT Syntaxe de fn
─────────────────

La fonction utilisateur a la syntaxe suivante :
&CO
    int fn(WDDLG *wddlg, int fld_nb, char *txt, int nNotif)
&TX
où :
&EN wddlg est le pointeur vers la structure WDDLG courante
&EN fld_nb est le numéro du contrôle courant (où s'est produit
    l'évènement)
&EN txt est la valeur du contrôle (= text de WDDlgAddButton)
&EN nNotif est le message Windows reçu dans le contrôle

Les messages suivants donnent lieu à un passage par cette fonction dans
le cas des contrôle d'édition :

&EN WDDLG_KILLFOCUS : le contrôle est quitté (touche TAB par exemple)
&EN WDDLG_SETFOCUS : le focus arrive sur ce contrôle

Si la valeur de retour de la fonction est 0, l'édition continue. Sinon,
la boîte de dialogue est quittée et la valeur de fn est retournée par
la fonction WDDlgEdit().

&EX
    static char testtext[21];

    WDDlgAddEdit(wddlg, 5, 115, 50, 14, testtext, 20, 0L, 0);
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgAddEdit(WDDLG *wddlg, int x, int y, int cx, int cy,
	     U_ch *text, int maxlen,
	     int (*fn)(WDDLG *, int, char *, int),
	     int attr)
{
    DWORD lStyle = ES_LEFT | WS_TABSTOP |  WS_CHILD | ES_AUTOHSCROLL | WS_VISIBLE | WS_BORDER | WS_GROUP;

    WDDlgAddChild(wddlg, WDDLG_EDIT, lStyle, x, y, cx, cy, (char *) text, (char *)maxlen, fn, attr);
    return(0);
}

/* ===================================================================
Ajoute un slider horizontal dans une boîte de dialogue dynamique.

&EN wddlg = pointeur vers la structure de définition de la boîte de
    dialogue dynamique créée par WDDlgCreate()
&EN (x, y) = position, en coordonnées de boîte de dialogue, du coin
    supérieur gauche du contrôle
&EN (cx, cy) = taille du contrôle, en coordonnées de boîte de dialogue
&EN pos = pointeur vers la valeur du contrôle (entier)
&EN nMin et nMax = borne des valeurs du contrôle

&EX
    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    WDDlgAddHSlider(wddlg, 5, 160, 80, 20, &pos, 0, 10);
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgAddHSlider(WDDLG *wddlg, int x, int y, int cx, int cy, int *pos,
		int nMin, int nMax)
{
    DWORD lStyle = TBS_AUTOTICKS | WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_GROUP;

    WDDlgAddChild(wddlg, WDDLG_SLIDE, lStyle, x, y, cx, cy, (char *) pos, (char *)MAKELONG(nMin, nMax), 0L, 0);
    return(0);
}

/* ===================================================================
Ajoute un "progress bar" dans une boîte de dialogue dynamique.

&EN wddlg = pointeur vers la structure de définition de la boîte de
    dialogue dynamique créée par WDDlgCreate()
&EN (x, y) = position, en coordonnées de boîte de dialogue, du coin
    supérieur gauche du contrôle
&EN (cx, cy) = taille du contrôle, en coordonnées de boîte de dialogue
&EN pos = pointeur vers la valeur du contrôle (entier)
&EN nMin et nMax = borne des valeurs du contrôle

&EX
    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    WDDlgAddProgress(wddlg, 5, 160, 80, 10, &pos, 0, 100);
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgAddProgress(WDDLG *wddlg, int x, int y, int cx, int cy, int *pos,
		 int nMin, int nMax)
{
    DWORD lStyle = WS_CHILD | WS_VISIBLE;

    WDDlgAddChild(wddlg, WDDLG_PROGRESS, lStyle, x, y, cx, cy, (char *) pos, (char *)MAKELONG(nMin, nMax), 0L, 0);
    return(0);
}

/*NH*/
WDDlgAddCombobox(WDDLG *wddlg, int x, int y, int cx, int cy,
		 U_ch **opts, U_ch *sel,
		 int (*fn)(WDDLG *, int, char *, int),
		 int attr)
{
    DWORD lStyle = CBS_DROPDOWNLIST | WS_TABSTOP | WS_VSCROLL |  WS_CHILD | ES_AUTOHSCROLL | WS_VISIBLE | WS_BORDER;
    U_ch    **tbl;

    tbl = SCR_copy_tbl(opts);

    WDDlgAddChild(wddlg, WDDLG_COMBOBOX, lStyle, x, y, cx, cy, (char *) sel, (char *)tbl, fn, attr);
    return(0);
}

/* ===================================================================
Edite une boîte de dialogue dynamique construite à l'aide de
WDDlgCreate() et WDDlgAdd*().

&RT Différentes valeurs sont possibles selon la manière choisie
    pour quitter l'édition.
&EN 0 : si ENTER
&EN -1 : si ESCAPE, Alt+F4, cliquer le "x" dans le coin supérieur droit
    ou retour via une fonction utilisateur
&EN autres valeurs = codes retour des fonctions utilisateurs

&EX
    WDDLG   *wddlg;
    int     pos, rc;

    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    WDDlgAddHSlider(wddlg, 5, 160, 80, 20, &pos, 0, 10);
    ...
    rc = WDDlgEdit(wddlg);
    WDDlgFree(wddlg);
    return(rc);
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgEdit(WDDLG *wddlg)
{
    if(wddlg == 0) return(-1);
    if(wddlg->beginfn && (*wddlg->beginfn)(wddlg)) return(-1);

    DialogBoxIndirectParam(hInst, (LPDLGTEMPLATE) (wddlg->wdlg),
			   GetLastActivePopup(hMainWnd),
			   (DLGPROC) WDDlgProc, (LPARAM) wddlg);
    wddlg->hWnd = 0;
    return(wddlg->rc);
}

/* ===================================================================
Affiche une boîte de dialogue dynamique construite à l'aide de
WDDlgCreate() et WDDlgAdd*().

&RT 0 si la fenêtre est créée, -1 sinon

&EX
    WDDLG   *wddlg;
    int     pos, rc;

    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    WDDlgAddHSlider(wddlg, 5, 160, 80, 20, &pos, 0, 10);
    ...
    rc = WDDlgDisplay(wddlg);
    ...
    WDDlgUnDisplay(wddlg);
    WDDlgFree(wddlg);
    return(rc);
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgDisplay(WDDLG *wddlg)
{
    if(wddlg == 0) return(-1);
    if(wddlg->beginfn && (*wddlg->beginfn)(wddlg)) return(-1);

    wddlg->hWnd = CreateDialogIndirectParam(hInst, (LPDLGTEMPLATE) (wddlg->wdlg),
					    GetLastActivePopup(hMainWnd),
					    (DLGPROC) WDDlgProc, (LPARAM) wddlg);
    ShowWindow(wddlg->hWnd, SW_SHOW);
    UpdateWindow(wddlg->hWnd);

    return(0);
}

/* ===================================================================
Ferme une boîte de dialogue dynamique construite à l'aide de
WDDlgCreate() et WDDlgAdd*() et affichée par WDDlgDisplay().

&RT -1 si la fenêtre n'existe pas

&EX
    WDDLG   *wddlg;
    int     pos, rc;

    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    WDDlgAddHSlider(wddlg, 5, 160, 80, 20, &pos, 0, 10);
    ...
    rc = WDDlgDisplay(wddlg);
    ...
    WDDlgUnDisplay(wddlg);
    WDDlgFree(wddlg);
    return(rc);
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgUnDisplay(WDDLG *wddlg)
{
    if(wddlg == 0) return(-1);
    if(wddlg->hWnd == 0) return(-1);

    DestroyWindow(wddlg->hWnd);
    wddlg->hWnd = 0;
    return(0);
}

/* ===================================================================
Libère l'espace alloué pour la création d'une boîte de dialogue
dynamique construite à l'aide de WDDlgCreate() et WDDlgAdd*().

&SA WDDlgCreate(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgFree(WDDLG *wddlg)
{
    int     i;

    if(wddlg == 0) return(0);
    SCR_free(wddlg->wdlg);
    for(i = 0 ; i < wddlg->nctrls ; i++) {
	if(wddlg->types[i] == WDDLG_COMBOBOX || wddlg->types[i] == WDDLG_LISTBOX) {
	    SCR_free_tbl((U_ch **)wddlg->exts[i]);
	    SCR_free(wddlg->tabs[i]);
	}
    }

    SCR_free(wddlg->ptrs);
    SCR_free(wddlg->exts);
    SCR_free(wddlg->fonts);
    SCR_free(wddlg->attrs);
    SCR_free(wddlg->types);
    SCR_free(wddlg->fns);
    SCR_free(wddlg->ios);
    SCR_free(wddlg->tabs);
    SCR_free(wddlg);
    return(0);
}



/*NH*/
WDDlgDblclkFn(WDDLG *wddlg, int fld_nb, char *txt, int nNotif)
{
    if(nNotif == WDDLG_DBLCLK) return(1);
    return(0);
}

/*NH*/
WDDlgOkFn(WDDLG *wddlg, int i, char *a, int j)
{
    return(1);
}

/*NH*/
WDDlgEscFn(WDDLG *wddlg, int i, char *a, int j)
{
    return(-1);
}


/*NH*/
WDDlgSelectAllFn(WDDLG *wddlg, int i, char *a, int k)
{
    int     j = 0;
    char    **opts = (char **)wddlg->exts[0],
	      *sel = wddlg->ptrs[0];

    if(wddlg->types[0] == WDDLG_LISTVIEW) j = 1;
    for(; opts[j] ; j++) sel[j] = 1;
    WDDlgDisplayFlds(wddlg);
    return(0);
}

/* ========================================================================
Affiche une liste d'options dans une boîte de dialogue dynamique et
permet d'en sélectionner une ou plusieurs. Cette fonction fait appel au
groupe de fonctions WDDlg. La sélection s'opère dans une LISTBOX standard
de Windows.

La boîte de dialogue proposée contient une LISTBOX et deux ou trois
boutons : Ok, Cancel et Select All si les sélections multiples sont
permises (selon la valeur de sels).

Attention aux coordonnées utilisées : la position (x, y) est exprimée en
coordonées écran (pixels), tandis que la taille (cx, cy) est exprimée en
coordonnées de boîte de dialogue. Ces dernières représentent la taille
moyenne d'un caractère dans la police utilisée dans la boîte de dialogue
divisée par 4 horizontalement, par 8 verticalement.

On peut quitter la sélection par ENTER, ESCAPE, Alt+F4, double click sur
une option, click sur Ok ou Cancel.

&EN title (optionnel) = titre de la boîte de dialogue. Si title est nul
    il n'y a pas de titre dans la boîte de dialogue.
&EN opts = tableau de pointeurs vers les options à afficher. Les options
    peuvent contenir des tabulations pour organiser la LISTBOX en
    colonnes
&EN sels (optionnel) = tableau de caractères, 0 pour option non
    sélectionnée, 1 pour option sélectionnée. Le nombre d'éléments de
    sels doit correspondre à celui de opts. Si sels est nul, la
    sélection ne peut être multiple et la valeur de retour de la
    fonction indique l'option sélectionnée.

&EN x = position, en pixels, du coin supérieur gauche de la boîte de
    dialogue
&EN y = position, en pixels, du coin supérieur gauche de la boîte de
    dialogue
&EN cx = taille, en coordonnées de boîte de dialogue (!), de la boîte de
    dialogue. Prévoir 60 points minimum pour les boutons sans sélection
    multiple et 100 points avec sélection multiple (un bouton de plus).

&EN cy = taille, en coordonnées de boîte de dialogue (!), de la boîte de
    dialogue. Prévoir 16 points supplémentaires en hauteur pour les
    boutons.

&EN ntabs (optionnel) = nombre de tabs définis
&EN tabs (optionnel) = position des tabs dans la LISTBOX (exprimés en
    coordonnées de la boîte de dialogue). Par défaut, un tab est placé
    tous les 16 points (soit plus ou moins 4 caractères).

&RT
&EN -1 si terminé par ESCAPE, bouton "Cancel" ou Alt-F4
&EN 0 si sélection multiple et ENTER, bouton "Ok" ou double click
&EN numéro de l'option choisie si sélection simple (0, 1, etc)

&EX

    int     nTabs[] = {50, 70, 100};

    MnEdit2(pg, fld_nb, opts)
    PAGE    *pg;
    int     fld_nb;
    U_ch    **opts;
    {
	int     x, y, w, h;

	WscrGetFldPos(pg, fld_nb, &x, &y, &w, &h);
	// La PAGE pg doit être active !! //
	return(WDDlgSelect(0L, opts, 0L, x, y + h, w/2, 140, 3, nTabs));
    }

&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAddListbox()
================================================================== */

WDDlgSelect(U_ch *title, U_ch **opts, char *sels, int x, int y,
	    int cx, int cy, int ntabs, int *tabs)
{
    WDDLG   *wddlg;
    char    *sel;
    int     rc, i, n, attr = 0;

    wddlg = WDDlgCreate(x, y, cx, cy, title);
    if(sels == 0) {
	n = SCR_tbl_size(opts);
	sel = SCR_malloc(n);
	sel[0] = 1;
    }
    else {
	sel = sels;
	attr = WDDLG_MULTISEL;
    }

    WDDlgAddListbox(wddlg, 2, 2, cx - 4, cy - 20, opts, sel, WDDlgDblclkFn, attr);
    if(ntabs) WDDlgSetTabs(wddlg, 0, ntabs, tabs);
    WDDlgAddButton(wddlg, 2, cy - 16, 20, 12, "Ok", WDDlgOkFn, WDDLG_DEFBUTTON);
    WDDlgAddButton(wddlg, 26, cy - 16, 30, 12, "Cancel", WDDlgEscFn, 0);
    if(sels)
	WDDlgAddButton(wddlg, 60, cy - 16, 40, 12, "Select All", WDDlgSelectAllFn, 0);
    // if(sels == 0) WDDlgSetIo(wddlg, 3, 1);

    rc = WDDlgEdit(wddlg);
    WDDlgFree(wddlg);
    if(sels == 0) {
	for(i = 0 ; i < n ; i++) if(sel[i]) break;
	if(i == n) i = -2;
	SCR_free(sel);
    }
    else
	i = -1;

    if(rc < 0) return(-1);
    return(i);
}


/* ========================================================================
Affiche une liste d'options dans une boîte de dialogue dynamique et
permet d'en sélectionner une ou plusieurs. Cette fonction fait appel au
groupe de fonctions WDDlg. La sélection s'opère dans une "LISTVIEW",
c'est-à-dire dans un tableau comportant une ligne de titre et
dont les colonnes ont une largeur modifiable par l'utilisateur.

La boîte de dialogue proposée contient une LISTVIEW et deux ou trois
boutons : Ok, Cancel et Select All si les sélections multiples sont
permises (selon la valeur de sels).

Attention aux coordonnées utilisées : la position (x, y) est exprimée en
coordonées écran (pixels), tandis que la taille (cx, cy) est exprimée en
coordonnées de boîte de dialogue. Ces dernières représentent la taille
moyenne d'un caractère dans la police utilisée dans la boîte de dialogue
divisée par 4 horizontalement, par 8 verticalement.

On peut quitter la sélection par ENTER, ESCAPE, Alt+F4, double click sur
une option, click sur Ok ou Cancel.

&EN title (optionnel) = titre de la boîte de dialogue. Si title est nul
    il n'y a pas de titre dans la boîte de dialogue.
&EN opts = tableau de pointeurs vers les options à afficher. Les options
    doit contenir des tabulations pour organiser la LISTVIEW en
    colonnes. La première option contient les titres de colonnes.
&EN sels (optionnel) = tableau de caractères, 0 pour option non
    sélectionnée, 1 pour option sélectionnée. Le nombre d'éléments de
    sels doit correspondre à celui de opts. Si sels est nul, la
    sélection ne peut être multiple et la valeur de retour de la
    fonction indique l'option sélectionnée.

&EN x = position, en pixels, du coin supérieur gauche de la boîte de
    dialogue
&EN y = position, en pixels, du coin supérieur gauche de la boîte de
    dialogue
&EN cx = taille, en coordonnées de boîte de dialogue (!), de la boîte de
    dialogue. Prévoir 60 points minimum pour les boutons sans sélection
    multiple et 100 points avec sélection multiple (un bouton de plus).

&EN cy = taille, en coordonnées de boîte de dialogue (!), de la boîte de
    dialogue. Prévoir 16 points supplémentaires en hauteur pour les
    boutons.

&RT
&EN -1 si terminé par ESCAPE, bouton "Cancel" ou Alt-F4
&EN 0 si sélection multiple et ENTER, bouton "Ok" ou double click
&EN numéro de l'option choisie si sélection simple (0, 1, etc)

&NO La largeur des colonnes est fixée automatiquement en fonction
    des textes des options (pas des titres).

&EX
    int     nTabs[] = {50, 70, 100};

    MnEdit3(pg, fld_nb, opts)
    PAGE    *pg;
    int     fld_nb;
    U_ch    **opts;
    {
	int     x, y, w, h;

	WscrGetFldPos(pg, fld_nb, &x, &y, &w, &h);
	// La PAGE pg doit être active !! //
	return(WDDlgLVSelect(0L, opts, 0L, x, y + h, w/2, 140));
    }
&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAddListview(), WDDlgSetLVTabs()
================================================================== */

WDDlgLVSelect(U_ch *title, U_ch **opts, char *sels, int x, int y,
	      int cx, int cy)
{
    WDDLG   *wddlg;
    char    *sel;
    int     rc, i, n, attr = 0;

    wddlg = WDDlgCreate(x, y, cx, cy, title);
    if(sels == 0) {
	n = SCR_tbl_size(opts);
	sel = SCR_malloc(n);
	sel[0] = 1;
    }
    else {
	sel = sels;
	attr = WDDLG_MULTISEL;
    }

    WDDlgAddListview(wddlg, 2, 2, cx - 4, cy - 20, opts, sel, WDDlgDblclkFn, attr);
    WDDlgAddButton(wddlg, 2, cy - 16, 20, 12, "Ok", WDDlgOkFn, WDDLG_DEFBUTTON);
    WDDlgAddButton(wddlg, 26, cy - 16, 30, 12, "Cancel", WDDlgEscFn, 0);
    if(sels)
	WDDlgAddButton(wddlg, 60, cy - 16, 40, 12, "Select All", WDDlgSelectAllFn, 0);
    // if(sels == 0) WDDlgSetIo(wddlg, 3, 1);

    rc = WDDlgEdit(wddlg);
    WDDlgFree(wddlg);
    if(sels == 0) {
	for(i = 0 ; i < n - 1 ; i++) if(sel[i]) break;
	if(i == n - 1) i = -2;
	SCR_free(sel);
    }
    else
	i = -1;

    if(rc < 0) return(-1);
    return(i);
}


/* ===================================================================
Ajoute un contrôle LISTVIEW à une boîte de dialogue dynamique.

&EN wddlg = pointeur vers la structure de définition de la boîte de
    dialogue dynamique créée par WDDlgCreate()
&EN (x, y) = position, en coordonnées de boîte de dialogue, du coin
    supérieur gauche du contrôle
&EN (cx, cy) = taille du contrôle, en coordonnées de boîte de dialogue
&EN opts = pointeur vers un tableau de textes
&EN sel = pointeur vers un tableau de char contenant 0 pour les options
    non sélectionnées et 1 pour les options sélectionnées
&EN fn = pointeur vers la fonction utilisateur de gestion des évènements
    du contrôle
&EN attr = attribut supplémentaire
&EN2 0 pour une LISTVIEW à sélection unique, combinaison de:
&EN2 WDDLG_MULTISEL pour une LISTVIEW à sélections multiples
&EN2 WDDLG_HIDDEN pour cacher le contrôle

&IT Syntaxe de fn
─────────────────

La fonction utilisateur a la syntaxe suivante :
&CO
    int fn(WDDLG *wddlg, int fld_nb, char *txt, int nNotif)
&TX
où :
&EN wddlg est le pointeur vers la structure WDDLG courante
&EN fld_nb est le numéro du contrôle courant (où s'est produit
    l'évènement)
&EN txt est la valeur du contrôle (= sel de WDDlgAddListview)
&EN nNotif est le message Windows reçu dans le contrôle

Les messages suivants donnent lieu à un passage par cette fonction dans
le cas des LISTBOX :

&EN WDDLG_DBLCLK    : double clic sur une option
&EN WDDLG_KILLFOCUS : perte du focus
&EN WDDLG_SETFOCUS  : gain du focus
&EN WDDLG_CHANGED : changement de sélection. Ce message est reçu à
    plusieurs reprises lors d'un changement d'option. A n'utiliser
    qu'avec attention.

Si la valeur de retour de la fonction est 0, l'édition continue. Sinon,
la boîte de dialogue est quittée et la valeur de fn est retournée par
la fonction WDDlgEdit().

&EX
    tmp = SCR_vtom("Colonne 1\tColonne 2\Col3\tCol4,
		    Option 1\tSuite,\t1\t2\t3\t4,
		    Option 2\tSuite,Option 3,Truc,Brol,
		    x\tSuite\tSuite,x\tSuite\t\tSuite,
		    y\tSuite\tSuite,dfsdfsdf", ',');
    sel = SCR_malloc(SCR_tbl_size(tmp));
    wddlg = WDDlgCreate(5, 5, 100, 220, 0L);
    sel[0] = sel[3] = 1;
    WDDlgAddListview(wddlg, 5, 20, 85, 50, tmp, sel, ListviewFn,
						    WDDLG_MULTISEL);
&TX

&SA WDDlgFree(), WDDlgAdd*(), WDDlgEdit(), WDDlgSelect()
==================================================================== */

WDDlgAddListview(WDDLG *wddlg, int x, int y, int cx, int cy, U_ch **opts,
		 char *sel,
		 int (*fn)(WDDLG *, int, char *, int),
		 int attr)
{
    U_ch    **tbl;
    int     i;
    DWORD   lStyle = WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT |
		     LVS_SHOWSELALWAYS /* | LVS_EDITLABELS */ | WS_TABSTOP |
		     /*WS_EX_CLIENTEDGE |*/ LVS_NOSORTHEADER;

    if(!(attr & WDDLG_MULTISEL)) lStyle |= LVS_SINGLESEL;

    tbl = SCR_copy_tbl(opts);
    for(i = 0 ; tbl[i] ; i++)
	SCR_OemToAnsi(tbl[i], tbl[i]);
    WDDlgAddChild(wddlg, WDDLG_LISTVIEW, lStyle, x, y, cx, cy,
		  (char *) sel, (char *)tbl, fn, attr);

    return(0);
}


/*NH*/
WDDlgInitListView(WDDLG *wddlg, int fld_nb)
{
    HWND        hWndList;
    LV_ITEM     lvI;
    LV_COLUMN   lvC;
    int         i, j, nc;
    U_ch        **opts, **tbl;

    opts = (U_ch **)wddlg->exts[fld_nb];
    if(opts == 0) return(-1);
    hWndList = GetDlgItem(wddlg->hWnd, 100 + fld_nb);

    // Add the columns
    tbl = SCR_vtom(opts[0], '\t');
    nc = SCR_tbl_size(tbl);

    lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
    lvC.fmt = LVCFMT_LEFT;  // left-align column
    lvC.cx = /*LVSCW_AUTOSIZE */ 75;    // width of column in pixels
//    lvC.cx = LVSCW_AUTOSIZE ;    // width of column in pixels

    for(i = 1; i <= nc; i++) {
	lvC.iSubItem = i;
	lvC.pszText = tbl[i - 1];
	if(ListView_InsertColumn(hWndList, i, &lvC) == -1) return(-1);
    }
    SCR_free_tbl(tbl);

    // Add the actual items to the control.
    lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
    lvI.state = 0;
    lvI.stateMask = 0;
    for(i = 1 ; opts[i] ; i++) {
	lvI.iItem = i - 1;
	lvI.iSubItem = 0;
	lvI.pszText = LPSTR_TEXTCALLBACK;
	lvI.cchTextMax = 80;
	lvI.iImage = 0;
	lvI.lParam = (LPARAM)(i - 1);

	if(ListView_InsertItem(hWndList, &lvI) == -1) return(-1);

	for(j = 0; j < nc; j++)
	    ListView_SetItemText(hWndList, i, j, LPSTR_TEXTCALLBACK);
    }

    WDDlgDisplayLVTabs(wddlg, fld_nb);
    return(0);
}

/*NH*/
WDDlgLVCalcNc(WDDLG *wddlg, int fld_nb)
{
    int     nc;
    U_ch    **opts, **tbl;

    opts = (U_ch **)wddlg->exts[fld_nb];
    if(opts == 0) return(0);
    tbl = SCR_vtom(opts[0], '\t');
    nc = SCR_tbl_size(tbl);
    SCR_free_tbl(tbl);
    return(nc);
}

/* ========================================================================
Fixe la largeur des colonnes du contrôle fld_nb LISTVIEW de la
boîte wddlg. Cette fonction peut être appelée avant l'édition ou en
cours d'édition (dans une fonction utilisateur). Par défaut, les
colonnes ont une largeur calculée en fonction des textes.

Si ntabs est insuffisant, les colonnes non dimensionnées restent
calculées automatiquement.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fld_nb = numéro du contrôle LISTVIEW
&EN ntabs = nombre de tabulations
&EN tabs = pointeur vers un tableau de positions en pixels

&EX
    int nTabs[] = {20, 50, 110};

    WDDlgSetLVTabs(wddlg, 0, 3, nTabs);
&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAddListview(), WDDlgSet*()
================================================================== */

WDDlgSetLVTabs(WDDLG *wddlg, int fld_nb, int ntabs, int *tabs)
{
    int     j, nc;
    HWND    hWndList;

    nc = WDDlgLVCalcNc(wddlg, fld_nb);
    SCR_free(wddlg->tabs[fld_nb]);
    wddlg->tabs[fld_nb] = (int *)SCR_malloc((nc + 1) * sizeof(int));
    wddlg->tabs[fld_nb][0] = nc;
    for(j = 0 ; j < ntabs && j < nc ; j++)
	wddlg->tabs[fld_nb][j + 1] = tabs[j];
    for( ; j < nc ; j++)
	wddlg->tabs[fld_nb][j + 1] = LVSCW_AUTOSIZE;

    WDDlgDisplayLVTabs(wddlg, fld_nb);
    return(0);
}

/*NH*/
WDDlgDisplayLVTabs(WDDLG *wddlg, int fld_nb)
{
    int     j, nc;
    HWND    hWndList;

    if(wddlg->hWnd == 0) return(0);
    nc = WDDlgLVCalcNc(wddlg, fld_nb);
    hWndList = GetDlgItem(wddlg->hWnd, 100 + fld_nb);
    if(wddlg->tabs[fld_nb])
	for(j = 1; j <= nc ; j++)
	    ListView_SetColumnWidth(hWndList, j - 1, wddlg->tabs[fld_nb][j]);
    else
	for(j = 1; j <= nc ; j++)
	    ListView_SetColumnWidth(hWndList, j - 1, LVSCW_AUTOSIZE);

    return(0);
}

/*NH*/
WDDlgNotifyHandler(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LV_DISPINFO *pLvdi   = (LV_DISPINFO *)lParam;
    NM_LISTVIEW *pNm     = (NM_LISTVIEW *)lParam;
    NMHDR       *pnmhdr  = (NMHDR *) lParam;
    int         line     = (int)(pLvdi->item.lParam);
    int         col      = (int)(pLvdi->item.iSubItem);
    int         nline    = (int)(pNm->iItem);
    int         ncol     = (int)(pNm->iSubItem);
    int         iId      = pnmhdr->idFrom;
    int         i, nc;
    U_ch        **tbl, **opts;
    static U_ch buf[80];
    WDDLG       *wddlg;

    switch(pnmhdr->code) {
    case NM_DBLCLK :
    case NM_SETFOCUS :
    case NM_KILLFOCUS :
	return(WDDlgCommand(hWnd, iId, 0L, pnmhdr->code));
    }


    if(pnmhdr->code == TTN_NEEDTEXT) {
	LPTOOLTIPTEXT  lpttt = (LPTOOLTIPTEXT) lParam;
	lpttt->lpszText = "Texte du tooltip";
	return(0);
    }

    switch(pLvdi->hdr.code)
    {
    case LVN_GETDISPINFO:
	wddlg = WDDlgGetDlg(hWnd);
	opts = (U_ch **)wddlg->exts[iId - 100];
	if(opts == 0) return(-1);
	tbl = SCR_vtom(opts[line + 1], '\t');
	nc = SCR_tbl_size(tbl);
	if(nc <= col) pLvdi->item.pszText = "";
	else {
	    SCR_strlcpy(buf, tbl[col], 79);
	    pLvdi->item.pszText = buf;
	}
	SCR_free_tbl(tbl);
	break;

    case LVN_ITEMCHANGED:
	return(WDDlgCommand(hWnd, iId, 0L, LVN_ITEMCHANGED));

	/*
		case LVN_BEGINLABELEDIT:
		{
		    HWND hWndEdit;

		    hWndEdit = (HWND)SendMessage(hWnd, LVM_GETEDITCONTROL,
			0, 0);
		    SendMessage(hWndEdit, EM_SETLIMITTEXT, (WPARAM)20, 0);
		}
		break;

		case LVN_ENDLABELEDIT:
		if ((pLvdi->item.iItem != -1) &&
		    (pLvdi->item.pszText != NULL))
				lstrcpy(pHouse->szAddress, pLvdi->item.pszText);
		break;

		case LVN_COLUMNCLICK:
		    break;

		case LVN_KEYDOWN:
		    i = ListView_GetNextItem(hWnd, -1, LVNI_CUT);
		    PG_display_error("KEYDOWN:Selected:%d", i);
		    break;
		case LVN_ITEMCHANGING:
		    PG_display_error("ITEMCHANGING");
		    break;
	*/

    default:
	break;
    }

    return(0);
}

/* ================================================================
Cache un contrôle.

&EN wddlg = pointeur vers la boîte de dialogue
&EN fld_nb = numéro du contrôle

&SA WDDlgCreate(), WDDlgEdit(), WDDlgShowField()
================================================================== */

WDDlgHideField(WDDLG *wddlg, int fld_nb)
{
    WDDlgSetHwndVisible(wddlg, fld_nb, 0);
    if(wddlg->hWnd == 0) return(0);
    InvalidateRect(wddlg->hWnd, NULL, TRUE);
    return(0);
}

/* ================================================================
Rend un contrôle apparent.

&EN wddlg = pointeur vers la boîte de dialogue
&EN fld_nb = numéro du contrôle

&SA WDDlgCreate(), WDDlgEdit(), WDDlgHideField()
================================================================== */

WDDlgShowField(WDDLG *wddlg, int fld_nb)
{
    WDDlgSetHwndVisible(wddlg, fld_nb, 1);
    if(wddlg->hWnd == 0) return(0);
    InvalidateRect(wddlg->hWnd, NULL, TRUE);
    return(0);
}


/*NH*/
WDDlgSetHwndVisible(WDDLG *wddlg, int fld_nb, int show)
{
    HWND    hWndEdit;
    DWORD   style;

    if(show) wddlg->attrs[fld_nb] &= ~WDDLG_HIDDEN;
    else     wddlg->attrs[fld_nb] |= WDDLG_HIDDEN;
    if(wddlg->hWnd == 0) return(0);
    hWndEdit = GetDlgItem(wddlg->hWnd, 100 + fld_nb);
    style = GetWindowLongPtr(hWndEdit, GWL_STYLE);
    if(show) {
	if(style & WS_VISIBLE) return(0);
	style |= WS_VISIBLE;
    }
    else {
	if(!(style & WS_VISIBLE)) return(0);
	style &= ~WS_VISIBLE;
    }
    SetWindowLongPtr(hWndEdit, GWL_STYLE, style);
    return(0);
}

/*NH*/
WDDlgResize(WDDLG *wddlg, int x, int y, int cx, int cy)
{
    return(0);
}

/* ========================================================================
Crée une boîte de dialogue dynamique temporaire pour poser une question.
La réponse se trouve dans le buffer rep.

&EN title = titre de la boîte de dialogue
&EN qu = question à poser
&EN rep = buffer contenant la réponse
&EN lg = taille maximum de rep

&SA WDDlgCreate(), WDDlgEdit()
================================================================== */

WDDlgAsk(U_ch *title, U_ch *qu, U_ch *rep, int lg)
{
    WDDLG   *wddlg;
    SIZE    sz;
    HDC     hDC;
    HFONT   hFont;
    int     bxy, bx, rc;

    wddlg = WDDlgCreate(-1, -1, 200, 50, title);

    if(hMainWnd) {
	hDC = GetDC(hMainWnd);
	hFont = SelectObject(hDC, W_SCRL_FONT);
	GetTextExtentPoint32(hDC, qu, strlen(qu), &sz);
	SelectObject(hDC, hFont);
	ReleaseDC(hMainWnd, hDC);
	bxy = GetDialogBaseUnits();
	bx = LOWORD(bxy);
	sz.cx = (sz.cx * 4) / bx;
    }
    else {
	sz.cx = 90;
    }

    WDDlgAddStatic(wddlg, 10, 10, sz.cx, 10, qu);
    WDDlgAddEdit(wddlg,  15 + sz.cx, 8, 175 - sz.cx, 10, rep, lg, 0L, 0);
    WDDlgAddButton(wddlg, 60, 30, 35, 12, "Ok",      WDDlgOkFn, 2);
    WDDlgAddButton(wddlg, 105, 30, 35, 12, "Cancel", WDDlgEscFn, 0);
    WDDlgEdit(wddlg);
    rc = wddlg->rc;
    WDDlgFree(wddlg);
    if(rc >= 0) return(0);
    return(rc);
}

/* ========================================================================
Fixe la fonction à exécuter lorsque la touche ESCAPE est pressée pendant
l'édition de la boîte de dialogue. Cette fonction doit retourner une
valeur non nulle pour que la boîte de dialogue soit quittée par ESCAPE
et nulle sinon.

Par défaut, ESCAPE quitte la boîte de dialogue.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fn = pointeur vers une fonction

&IT Prototype de fn
&CO
    int fn(WDDLG *wddlg)
&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgSetEndFn(), WDDlgSetBeginFn()
================================================================== */


WDDlgSetEscFn(WDDLG *wddlg, int (*fn)(WDDLG *, int, char *, int))
{
    wddlg->escfn = fn;
    return(0);
}


/* ========================================================================
Fixe la fonction à exécuter lorsque la touche ENTER est pressée pendant
l'édition de la boîte de dialogue. Cette fonction doit retourner une
valeur non nulle pour que la boîte de dialogue soit quittée par ESCAPE
et nulle sinon.

Par défaut, ENTER exécute le bouton par défaut. En l'absence de bouton
de ce type, ENTER quitte la boîte de dialogue.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fn = pointeur vers une fonction

&IT Prototype de fn
&CO
    int fn(WDDLG *wddlg)
&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgSetEscFn(), WDDlgSetBeginFn()
================================================================== */


WDDlgSetOkFn(WDDLG *wddlg, int (*fn)(WDDLG *, int, char *, int))
{
    wddlg->okfn = fn;
    return(0);
}


/* ========================================================================
Fixe la fonction à exécuter lors du début de l'édition de la boîte de
dialogue wddlg. Cette fonction doit retourner une valeur non nulle pour
que la boîte de dialogue ne soit pas éditée et nulle pour que l'édition
débute.

La fonction est exécutée avant la création de la fenêtre.


&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fn = pointeur vers une fonction

&IT Prototype de fn
&CO
    int fn(WDDLG *wddlg)
&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgSetEscFn(), WDDlgSetEndFn()
================================================================== */


WDDlgSetBeginFn(WDDLG *wddlg, int (*fn)(WDDLG *, int, char *, int))
{
    wddlg->beginfn = fn;
    return(0);
}

/* ========================================================================
Fixe la fonction à exécuter lors du la fin  de l'édition
de la boîte de dialogue wddlg. Cette fonction doit retourner une
valeur nulle pour que la boîte de dialogue puisse être quittée
et non nulle pour que l'édition se poursuive.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fn = pointeur vers une fonction

&IT Prototype de fn
&CO
    int fn(WDDLG *wddlg)
&TX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgSetEscFn(), WDDlgSetBeginFn()
================================================================== */


WDDlgSetEndFn(WDDLG *wddlg, int (*fn)(WDDLG *, int, char *, int))
{
    wddlg->beginfn = fn;
    return(0);
}

/* ========================================================================
Retourne la position de l'option sélectionnée dans la LISTBOX
fld_nb de la boîte wddlg.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fld_nb = numéro du contrôle LISTBOX

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAdd*(), WDDlgSet*()
================================================================== */

WDDlgGetCurSel(WDDLG *wddlg, int fld_nb)
{
    if(wddlg->hWnd == 0) return(-1);
    return(SendDlgItemMessage(wddlg->hWnd, fld_nb + 100, LB_GETCURSEL, 0, 0));
}

/* ========================================================================
Fixe la position de l'option sélectionnée dans la LISTBOX
fld_nb de la boîte wddlg.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fld_nb = numéro du contrôle LISTBOX
&EN opt_nb = numéro de l'option à sélectionner

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAdd*(), WDDlgSet*()
================================================================== */

WDDlgSetCurSel(WDDLG *wddlg, int fld_nb, int opt_nb)
{
    if(wddlg->hWnd == 0) return(-1);
    SendDlgItemMessage(wddlg->hWnd, fld_nb + 100, LB_SETCURSEL, opt_nb, 0);
    return(0);
}

/* ========================================================================
Change le texte de l'option opt_nb de la LISTBOX fld_nb de la boîte wddlg.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fld_nb = numéro du contrôle LISTBOX
&EN opt_nb = numéro de l'option à changer
&EN txt : nouveau texte de l'option

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAdd*(), WDDlgSet*()
================================================================== */

WDDlgLBChangeText(WDDLG *wddlg, int fld_nb, int opt_nb, U_ch * txt)
{
    if(wddlg->hWnd == 0) return(-1);

    SCR_OemToAnsi(txt, txt);
    SendDlgItemMessage(wddlg->hWnd, fld_nb + 100, LB_INSERTSTRING, opt_nb, (LONG)txt);
    SCR_AnsiToOem(txt, txt);
    SendDlgItemMessage(wddlg->hWnd, fld_nb + 100, LB_DELETESTRING, opt_nb + 1, 0);
    return(0);
}

/* ========================================================================
Fixe le focus sur le champ fld_nb de la boîte wddlg.

&EN wddlg = pointeur vers la structure WDDLG contenant la description
    des contrôles
&EN fld_nb = numéro du contrôle

&SA WDDlgCreate(), WDDlgEdit(), WDDlgAdd*(), WDDlgSet*()
================================================================== */

WDDlgSetFocus(WDDLG *wddlg, int fld_nb)
{
    if(wddlg->hWnd == 0) return(-1);
    SetFocus(GetDlgItem(wddlg->hWnd, fld_nb + 100));
    return(0);
}

/*NH*/
WDDlgMsgBoxFn(WDDLG *wddlg, int i, char *a, int j)
{
    return(i);
}

/*NH
Retourne un U_ch ** contenant les lignes d'un texte de largeur max w
coord boite dlg
*/

U_ch **WDDlgSplitText(U_ch *txt, int w)
{
    U_ch    **tbl = 0;
    int     ch, lg, nl = 0, i, prev = 0, last = 0;

    for(i = 0 ; txt[i] ; i++) {
	ch = txt[i];
	if(!U_is_in(ch, ",;.!?: ")) continue;
	lg = WDDlgTxtSizeLg(txt + prev, i - prev + 1);
	if(lg < w) {
	    last = i;
	    continue;
	}
	if(prev == last) last = i;
	SCR_add_ptr_lg_0(&tbl, &nl, txt + prev, 1 + last - prev, 1);
	i = last;
	prev = last + 1;
	last = prev;
    }

    if(txt[prev]) {
	lg = WDDlgTxtSizeLg(txt + prev, i - prev);
	if(lg > w && prev < last) {
	    SCR_add_ptr_lg_0(&tbl, &nl, txt + prev, 1 + last - prev, 1);
	    prev = last + 1;
	}
	SCR_add_ptr_lg_0(&tbl, &nl, txt + prev, i - prev, 1);
    }

    SCR_add_ptr(&tbl, &nl, 0L);
    return(tbl);
}

/*NH
Retourne la longueur d'un string en coordonnées de boite de dialogue
Retourne 0 si hMainWnd n'est pas définie.
*/

WDDlgTxtSizeLg(U_ch *txt, int lg)
{
    SIZE    sz;
    HDC     hDC;
    HFONT   hFont;
    int     bxy, bx;

    if(hMainWnd) {
	hDC = GetDC(hMainWnd);
	hFont = SelectObject(hDC, W_SCRL_FONT);
	GetTextExtentPoint32(hDC, txt, lg, &sz);
	SelectObject(hDC, hFont);
	ReleaseDC(hMainWnd, hDC);
	bxy = GetDialogBaseUnits();
	bx = LOWORD(bxy);
	sz.cx = (sz.cx * 4) / bx;
    }
    else {
	sz.cx = lg * 4;
    }
    return(sz.cx);
}

/*NH*/
WDDlgTxtSize(U_ch *txt)
{
    return(WDDlgTxtSizeLg(txt, strlen(txt)));
}

/* ========================================================================
Crée une boîte de dialogue dynamique temporaire avec une question et plusieurs boutons.

&RT Numéro du bouton sélectionné ou -1 (1 pour le premier, etc).

&EN title = titre de la boîte de dialogue
&EN qu = question à poser
&EN buts = tableau de texte pour les boutons

&SA WDDlgCreate(), WDDlgEdit()
================================================================== */

WDDlgMsgBox(U_ch *title, U_ch *qu, U_ch **buts)     /* BP_M 13-02-2003 16:20 */
{
    WDDLG   *wddlg;
    int     rc, n, i, cx, lg, lgmax = 0,
			      nlbuts, ncbuts, nlqu, x, y, line, col;
    U_ch    **tblqu;

    n = SCR_tbl_size(buts);

    for(i = 0 ; buts[i] ; i++) {
	lg = WDDlgTxtSize(buts[i]);
	if(lgmax < lg) lgmax = lg;
    }

    lgmax += 8;
    if(lgmax > 180) lgmax = 180;

    tblqu = WDDlgSplitText(qu, 180);
    nlqu = SCR_tbl_size(tblqu);
    ncbuts = 400.0 / (3.5 * lgmax);
    if(ncbuts <= 0) ncbuts = 1;     /* BP_M 13-02-2003 10:26 */
    nlbuts = 1 + (n - 1) / ncbuts;

    SCR_AnsiToOem(title, title);
    wddlg = WDDlgCreate(-1, -1, 200, 10 * (nlqu + 2) + nlbuts * 17, title);

    for(i = 0 ; tblqu[i] ; i++)
	WDDlgAddStatic(wddlg, 10, 10 * (i + 1), 180, 10, tblqu[i]);

    cx = 200 / (1 + 3 * ncbuts);


    /* BP_M 13-02-2003 16:20 */
    WDDlgAddButton(wddlg, -1, -1, -1, -1, " ",
		   WDDlgMsgBoxFn, (i == 0) ? WDDLG_DEFBUTTON : 0);

    for(i = 0 ; buts[i] ; i++) {
	line = i / ncbuts;
	col = i % ncbuts;
	x = cx * (1 + 3 * col);
	y = 10 * (nlqu + 1) + 5 + 17 * line;
	WDDlgAddButton(wddlg, x, y, 2 * cx, 12, buts[i],
		       WDDlgMsgBoxFn, (i == 0) ? WDDLG_DEFBUTTON : 0);
    }

    rc = WDDlgEdit(wddlg) - 1;
    WDDlgFree(wddlg);
    SCR_free_tbl(tblqu);

    if(rc < 0) return(-1);
    return(rc - nlqu); /* JMP 14-05-99 */
}


/* ========================================================================
Crée une boîte de dialogue modeless avec un texte variable dans msg.

&RT pointeur vers une structure WDDLG

&EN title = titre de la boîte de dialogue
&EN msg = pointeur vers le texte du message

&SA WDDlgDisplay(), WDDlgUndisplay, WDDlgFree()
================================================================== */

WDDLG *WDDlgCreateInfoBox(U_ch *title, U_ch *msg)
{
    WDDLG   *wddlg;

    wddlg = WDDlgCreate(-1, -1, 300, 16, title);
    WDDlgAddStatic(wddlg, 5, 2, 290, 12, msg);
    WDDlgSetFont(wddlg, -1, SCR_DEFAULT);
    WDDlgDisplay(wddlg);
    return(wddlg);
}



/* ========================================================================
Affiche le menu mn sous forme d'une ListView. La fenêtre ListView est
indépendante. Elle est placée relativement à la PAGE pg et au champ fld_nb.

&EN mn : menu de référence
&EN sep : caractère de séparation des colonnes. Dans chaque ligne du menu,
on a un nombre identique de colonnes séparées par ce caractère.
&EN istitle : indique si la première option du menu contient le titre à afficher comme
    titre du ListView
&EN pg : page de référence pour le positionnement
&EN fld_nb : champ de référence dans la page ou -1 pour placer en haut à gauche de la page

Une seule option peut être sélectionnée. Le fait de presser ENTER retourne
le numéro de l'option courante. ESCAPE retourne -1.


&EX
    TestLV()
    {
	int     opt;
	extern  MENU *testmn;

	opt = WscrMenuLV(testmn, '|', 1, stp_lpmail, 0);
	PG_display_error("Option sélectionnée : %d", opt);
    }

&TX

&SA MN_edit(), MN_create(), MN_auto_create()
================================================================== */

WscrMenuLV(MENU *mn, char sep, int istitle, PAGE *pg, int fld_nb)
{
    U_ch    **tbl = 0, *sels, buf[2], buf2[1024], *tit;
    int     rc, i, nl, nc, ntbl = 0, res, w, h;
    WDDLG   *wddlg;
    int     th, xf, yf, wf, hf;
    OPTION  *opt;

    buf[0] = sep;
    buf[1] = 0;

    if(istitle == 1) {
	SCR_add_ptr(&tbl, &ntbl, mn->mn_opts[0].opt_text);
	SCR_replace(tbl[0], buf, "\t");
    }
    else {
	buf2[0] = 0;
	strcat(buf2, "\t ");
	for(i = 0 ; mn->mn_opts[0].opt_text[i] ; i++)
	    if(mn->mn_opts[0].opt_text[i] == sep)
		strcat(buf2, "\t ");
	SCR_add_ptr(&tbl, &ntbl, buf2);
    }

    /*
	if(mn->mn_title && !istitle) {
	SCR_add_ptr(&tbl, &ntbl, mn->mn_title);
	SCR_replace(tbl[ntbl - 1], buf, " ");
	}
    */
    for(i = istitle ; i < mn->mn_nb_opts ; i++) {
//      SCR_OemToAnsi(mn->mn_opts[i].opt_text, mn->mn_opts[i].opt_text);
	SCR_add_ptr(&tbl, &ntbl, mn->mn_opts[i].opt_text);
	SCR_replace(tbl[ntbl - 1], buf, "\t");
    }

    SCR_add_ptr(&tbl, &ntbl, 0L);
    sels = SCR_malloc(mn->mn_nb_opts);

    nc = mn->mn_nb_column;
    nl = mn->mn_nb_line;
    if(nl > 20) nl = 20;
    if(nc > 70) nc = 70;
    w = nc * 4;
    h = nl * 8;

    WscrGetFldPos(pg, fld_nb, &xf, &yf, &wf, &hf);
    if(fld_nb < 0) {
	th = WscrGetPgTitleHeight(pg);
	h = hf - th;
	yf += th;
	xf += 2;
	w = nc * 12;  // Dlg Units !!
	WscrPixelsToDlg(&w, &h);
    }
    else {
	yf += hf + 2;
    }

    if(mn->mn_title) {
	tit = SCR_stracpy(mn->mn_title);
//        SCR_AnsiToOem(tit, tit);
    }
    else
	tit = 0;

    wddlg = WDDlgCreate(xf, yf, w, h - 2,  tit);

    SCR_free(tit);

    sels[0] = 1;
    WDDlgAddListview(wddlg, 2, 2, w - 4, h - 6, tbl, sels, 0L, 0);
    WDDlgSetEscFn(wddlg, WDDlgEscFn);
    WDDlgSetOkFn(wddlg, WDDlgOkFn);

    if(mn->mn_choice >= 0)
	for(i = 0 ; i < mn->mn_choice && i < mn->mn_nb_opts ; i++)
	    WscrRecordKey2(SCR_CSR_DOWN);

    while(1) {


	rc = WDDlgEdit(wddlg);

	if(rc < 0) break;

	for(i = 0 ; i < mn->mn_nb_opts ; i++)
	    if(sels[i]) break;

	if(i == mn->mn_nb_opts) {
	    i = -2;
	    break;
	}
	else {
	    mn->mn_choice = i;
	    opt = mn->mn_opts + i;
	    if(opt->opt_act == 0) break;
	    if(opt->opt_undisplay == 2) {
		ACT_exec_nu(opt->opt_act);
		break;
	    }
	    ACT_exec_nu(opt->opt_act);
	    if(SCR_APP_ABORT || opt->opt_undisplay == 1)
		break;
	}
    }

    SCR_free_tbl(tbl);
    SCR_free(sels);
    WDDlgFree(wddlg);

    if(rc < 0) return(-1);
    if(istitle) return(i + 2);
    return(i + 1);
}


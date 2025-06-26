//#ifndef WINDOWS

/* ============= DOS, UNIX, DOSWIN =========================== */
#include "scr.h"
PAGE    *SC_PG = 0;

/* ====================================================================
Initialise l'affichage d'un compteur. Dans le cas d'une application
SCR, la page de compteur est scr_incr définie dans scr.f.

Dans le cas d'une application non FULLSCREEN, le compteur est simplement
affiché sur la ligne courante de l'écran.

&EX
    SC_init_display();
    for(i = 0 ; i < 1000 ; i++)
	SC_incr_display((long)i, (long)i);
    SC_end_display();
&TX
&SA SC_end_display(), SC_incr_display()
=======================================================================*/

SC_init_display()
{
    SC_init_display_d(1);
    return(0);
}

/*NH*/
SC_init_display_d(disp)
int     disp;
{
    SCR_clear_key(); /* JMP 15-05-00 */
    if(disp == 0) return(0);
    SC_PG = PG_read("scr_incr");
    if(SC_PG == 0 || ERR_TYPE == 0) {
	Wprintf("\nRead : 0 - Selected : 0");
	return(0);
	}
    PG_reset(SC_PG);
    SCR_set_field_long(SC_PG, 0, 0L);
    SCR_set_field_long(SC_PG, 1, 0L);
    SCR_set_field_text(SC_PG, 2, SCR_err_txt_t("SC_BEGIN"));
//    SC_PG->pg_flds[2].fld_io = SCR_HIDDEN - SCR_INPUT; /* JMP 08-05-00 */
    PG_display_im(SC_PG);
    return(0);
}


/* ====================================================================
Termine l'affichage d'un compteur débuté par SC_init_display().

Dans le cas d'une application fullscreen, affiche le dernier champ de la
PAGE scr_incr, attend un touche, puis efface la PAGE.

Dans le cas d'une application non fullscreen, n'a pas d'effet.
&EX
    SC_init_display();
    for(i = 0 ; i < 1000 ; i++)
	SC_incr_display((long)i, (long)i);
    SC_end_display();
&TX
&SA SC_init_display(), SC_incr_display()
=======================================================================*/

SC_end_display()
{
    if(SC_PG == 0 || ERR_TYPE == 0 || SC_PG->pg_im == 0) {
	Wprintf("\n");    /* JMP 13-10-96 */
	return(0);
	}
//    SC_PG->pg_flds[2].fld_io = SCR_ACCESSIBLE - SCR_INPUT; /* JMP 08-05-00 */
//    Debug("Msg Fin : %s\n", SCR_err_txt_t("SC_END"));
    SCR_set_field_text(SC_PG, 2, SCR_err_txt_t("SC_END"));
    PG_display_flds_no_cmp(SC_PG);
    SCR_get_key();
//    SC_PG->pg_flds[2].fld_io = SCR_HIDDEN - SCR_INPUT;
    PG_undisplay(SC_PG);
    return(0);
}


/* ====================================================================
Incrémente le compteur initialisé par SC_init_display() en plaçant
dans les champs de la PAGE scr_incr les valeurs nb_read et nb_found.

&EX
    SC_init_display();
    for(i = 0 ; i < 1000 ; i++)
	SC_incr_display((long)i, (long)i);
    SC_end_display();
&TX
&SA SC_init_display(), SC_end_display()
=======================================================================*/

SC_incr_display(nb_read, nb_found)
long    nb_read, nb_found;
{
    if(SC_PG == 0 || ERR_TYPE == 0) {
	printf("\rRead : %ld - Selected : %ld", nb_read, nb_found);
	return(0);
	}
    SCR_set_field_long(SC_PG, 0, nb_read);
    SCR_set_field_long(SC_PG, 1, nb_found);
//    PG_display_flds_no_cmp(SC_PG);
    PG_display_field(SC_PG, 0);
    PG_display_field(SC_PG, 1);
    return(0);
}

/* ====================================================================
Vérifie qu'une touche aété pressée. Si c'est la cas, demande
confirmation (question text) et retourne -1 si confirmation.

&RT 0 si pas de touche pressée, -1 si touche pressée et confirmation.

&SA SC_init_display(), SC_end_display()
=======================================================================*/

SC_hit_key(text)
char    *text;
{
    int     rc;

/*  if(SC_PG == 0 || ERR_TYPE == 0) return(0);  */  /* BP_M 19-04-95 */
    if(ERR_TYPE == 0) return(0); /* JMP 25-02-96 */
    if(SCR_SC_HIT_KEY == 0)
	rc = SCR_check_key();
    else
	rc = SCR_hit_key();
    if(rc) {
	SCR_get_key();
	if(SC_PG) { /* JMP 15-05-00 */
	    SCR_set_field_text(SC_PG, 2, SCR_err_txt_t("SC_PAUSE"));
	    PG_display_flds_no_cmp(SC_PG);
	    }
	if(!SCR_confirme(text)) return(-1);
	if(SC_PG) { /* JMP 15-05-00 */
	    SCR_set_field_text(SC_PG, 2, SCR_err_txt_t("SC_BEGIN"));
	    PG_display_flds_no_cmp(SC_PG);
	    }
	}

    return(0);
}

/*NH*/
SC_wait()
{
    return(0);
}

//#else
#if 0

/* ================= VERSION WINDOWS ======================== */

#include "scr4w.h"

FARPROC lpCmptDlgProc = 0;
HWND    CmpthWnd = 0;
int     CmptAbort;

/*NH*/
BOOL FAR PASCAL CmptDlgProc(HWND hWnd, unsigned wMsg, WORD wParam, LONG lParam)
{
    WORD    iId     = LOWORD(wParam);
    WORD    iNotify = HIWORD(wParam);

    switch(wMsg) {
/*        case WM_INITDIALOG:
	    CmptAbort = 0;
	    CmpthWnd = hWnd;
	    return(TRUE);
*/
	case WM_COMMAND :
	    if((iId == 30002 && iNotify == BN_CLICKED) ||
		iId == IDOK || iId == IDCANCEL) {
		    CmptAbort++;
		    /*return(TRUE);*/
		    }
	    break;
	}

    return(FALSE);
}

/*NH*/
SC_init_display()
{
    SC_init_display_d(1);
    return(0);
}

/*NH*/
SC_init_display_d(disp)
int disp;
{
    HWND    hWndEdit;
    HWND    hWndParent;
    PAGE    *pg;

    if(disp == 0) return(0);
    hWndParent = GetLastActivePopup(hMainWnd);
    lpCmptDlgProc = MakeProcInstance((FARPROC)CmptDlgProc, hInst);
    pg= PG_read("scr_incr");
    CmpthWnd = CreateDialog(hInst, MAKEINTRESOURCE(SCR_NU(pg)), hWndParent, lpCmptDlgProc);
    if(CmpthWnd == 0) return(-1);
    CmptAbort = -1;

    SetDlgItemText(CmpthWnd, 30000, "0");
    SetDlgItemText(CmpthWnd, 30001, "0");
    SetDlgItemText(CmpthWnd, 30002, "Stop");
    hWndEdit = GetDlgItem(CmpthWnd, 30000);
    EnableWindow(hWndEdit, FALSE);
    hWndEdit = GetDlgItem(CmpthWnd, 30001);
    EnableWindow(hWndEdit, FALSE);

    ShowWindow(CmpthWnd, SW_SHOW);
    UpdateWindow(CmpthWnd);
    EnableWindow(hWndParent, FALSE);
    return(0);
}

/*NH*/
SC_incr_display(nb_read, nb_found)
long    nb_read, nb_found;
{
    char    buf[40];

    CmptAbort = 0;
    sprintf(buf, "%ld", nb_read);
    SetDlgItemText(CmpthWnd, 30000, buf);
    sprintf(buf, "%ld", nb_found);
    SetDlgItemText(CmpthWnd, 30001, buf);
    UpdateWindow(CmpthWnd);

    SC_wait();
    return(0);
}

/*NH*/
SC_wait()
{
    MSG     msg;

    while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	if(/*CmpthWnd == 0 ||*/ !IsDialogMessage(CmpthWnd, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	    }
    return(0);
}

/*NH*/
SC_end_display()
{
    HWND    hWndParent = GetParent(CmpthWnd);
    MSG     msg;

    SetDlgItemText(CmpthWnd, 30002, "OK");
    UpdateWindow(CmpthWnd);
    while(GetMessage(&msg, (HWND)0, 0, 0)) {
	if(!IsDialogMessage(CmpthWnd, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	    }
	if(CmptAbort) break;
	}

    EnableWindow(hWndParent, TRUE);
    DestroyWindow(CmpthWnd);
    FreeProcInstance(lpCmptDlgProc);
    return(0);
}

/*NH*/
SC_hit_key(text)
char    *text;
{
    SC_wait();
    if(CmptAbort > 0) {
	if(!SCR_confirme(text)) return(-1);
	}

    return(0);
}

#endif







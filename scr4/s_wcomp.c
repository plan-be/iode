#include "scr4w.h"

/* 1. Assurer la compatibilité entre DOS et Windows */

SCR_MessageBox(title, msg, buts)
unsigned char   *title, *msg, *buts[];
{

    return(WDDlgMsgBox(title, msg, buts));

/*
    SCR_OemToAnsi(title, title);
    SCR_OemToAnsi(msg, msg);
    MessageBox(GetLastActivePopup(hMainWnd), msg, title, MB_OK | MB_ICONSTOP | MB_APPLMODAL);
    SCR_AnsiToOem(title, title);
    SCR_AnsiToOem(msg, msg);
    return(0);
*/
}

SCR_MessageBox1(title, msg, buts, ncurb, required)   /* JMP 24-01-96 */
unsigned char   *title, *msg, *buts[];
int             ncurb, required;
{
    return(SCR_MessageBox(title, msg, buts));
}

MMT_edit_file(char *file, int line, int col, int nl, int nc)
{
    char    buf[128];

    wsprintf(buf, "notepad %s", file);
    SCR_strip(buf);
    WinExec(buf, SW_SHOWNORMAL);
    return(0);
}


MMT_show_file(char *file, int line, int col, int nl, int nc)
{
    MMT_edit_file(file, 0, 0, 0, 0);
    return(0);
}

PG_display_file(char *file)
{
    MMT_edit_file(file, 0, 0, 0, 0);
    return(0);
}


/* Annuler certaines fonctions (temporairement) */

WscrInit()
{
    SCR_init();
    return(0);
}


PG_display_field(PAGE *pg, int fld_nb)
{
    int     objnb;
    FIELD   *fld;

    fld = pg->pg_flds + fld_nb;

    if(fld->fld_io == SCR_HIDDEN - SCR_INPUT) return(0);

    /* Set default value */
    PG_set_dft_field(pg, fld_nb);

    /* Execute the display function */
    ACT_exec_nu(fld->fld_disp_act);

    /* Execute the begin function */    /* BP_M 30-03-1995 */
    if(SCR_EXEC_BEG_IN_DISP == 1)       /* BP_M 30-03-1995 */
	ACT_exec_nu(fld->fld_begin_act);  /* BP_M 30-03-1995 */

    objnb = WscrFindPgNb(pg);
    if(objnb < 0) return(0);
    WpgFld2Dlg(WSCR_HWNDS[objnb].sh_hwnd, pg, fld_nb);
    SCR_wdispatch(); /* JMP 05-05-00 */

    return(0);
}

PG_display_reverse_field(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    return(PG_display_field(pg, fld_nb));
}

MN_display_opt(MENU *mn, int opt, int rev)
{
    return(0);
}

//int PG_EXEC_DISP_FN = 1, PG_EXEC_DISP_FN_FLDS = 1;

PG_display_flds_no_cmp(PAGE *pg)
{
    int objnb;

    if(PG_EXEC_DISP_FN_FLDS) ACT_exec_nu(pg->pg_disp_act);
    objnb = WscrFindPgNb(pg);
    if(objnb < 0) return(0);
    Wpg2Dlg(WSCR_HWNDS[objnb].sh_hwnd, pg);
    return(0);
}

PG_display_flds(PAGE *pg)
{
    SCR_apply_all_cmp(pg);
    PG_display_flds_no_cmp(pg);
    return(0);
}

AP_run(APPL *ap)
{
    WapEditAppl(ap);
    SCR_APP_ABORT = SCR_PG_ABORT = 0;
    return(0);
}


HWND hWndDOS = 0;

MN_edit(MENU *mn)
{
    HMENU   hMenu;
    HWND    hWnd;
    int     i, rc, x, y;
    RECT    rect;
//    extern HMENU WapCreateMenu();
    char    *buf;

    MN_read_opt(mn);
    if(ACT_exec_nu(mn->mn_begin_act) != 0) return(-1);

    /* check if at least one option is enabled */
    for(i = rc = 0 ; i< mn->mn_nb_opts ; i++)
	    if(mn->mn_opts[i].opt_enable == 1) rc = 1;
    if(rc == 0) return(-1);

    /* BP_M 14-01-2003 16:51 */
    if(U_is_in('@', mn->mn_opts[0].opt_text))
	for(i = 0 ; i < mn->mn_nb_opts ; i++)
	    SCR_replace(mn->mn_opts[i].opt_text, "@", "\t");


    hMenu = WapCreateMenu(mn, 0);
    if(hMenu == 0) return(-1);

    hWnd = GetLastActivePopup(hMainWnd);
    GetWindowRect(hWnd, &rect);

    x = mn->mn_w_x;
    y = mn->mn_w_y;
    if(x == 0) {
	x = rect.left;
	x += GetSystemMetrics(SM_CXBORDER);
	}
    if(y == 0) {
	y = rect.top;
	y += GetSystemMetrics(SM_CYCAPTION) +
	     GetSystemMetrics(SM_CYMENU) +
	     GetSystemMetrics(SM_CYBORDER);
	}

    /* BP_M 10-12-1999 */
//  SCR_record_key(SCR_CSR_DOWN);
    WscrRecordKey2(SCR_CSR_DOWN);
    /* BP_M 10-12-1999 */
    if(mn->mn_choice >= 0)
	for(i = 0 ; i < mn->mn_choice && i < mn->mn_nb_opts ; i++)
	    WscrRecordKey2(SCR_CSR_DOWN);
//          SCR_record_key(SCR_CSR_DOWN);

    while(1) {
	MN_help_stack(mn);
//      SetMenuDefaultItem(hMenu, 2, TRUE);
//      HiliteMenuItem(hWnd, hMenu, 2, MF_BYPOSITION | MF_HILITE);


	rc = TrackPopupMenu(hMenu, TPM_RETURNCMD, x, y, 0, hWnd, NULL);
	PostMessage(hWnd, WM_NULL, 0, 0);
	SCR_wdispatch();
	HLP_stack_del(1);
	if(rc <= 0) {
	    rc = -1;
	    break;
	    }
	mn->mn_choice = rc - 20000;
	/* BP_M 11-02-2003 17:12 */
	if(rc < 20000 && WscrActExec(rc - WACT_SHIFT) != 0) continue;
	rc -= 20000 - 1;
	if(ACT_exec_nu(mn->mn_end_act) != 0) continue;
	break;
	}

    WscrUnRegisterMenu(hMenu);
    DestroyMenu(hMenu);
    MN_free_opt(mn);
    return(rc);
}

SCR_comment(U_ch *msg)
{
    WscrStatusSetText(0L, msg);
    return(0);
}

PG_edit_window(pg)
PAGE    *pg;
{
    if(SCR_PTR_TYPE(pg) == SCR_TYPE_TPAGES)
	return(WscrEditTabbedPages(0L, pg));
    else
	return(WscrEditPage(pg));
}

PG_edit(PAGE *pg)
{
    return(WscrPG_edit(pg, 1));
}

WscrPG_edit(PAGE *pg, int req)
{
    MSG     msg;
    HWND    hWnd, hWndParent;
    int     objnb, stat;
    BOOL    ParentEn;
    extern int SCR_WGOTO_NEXT_FLD;

    SCR_PG_ABORT = 0;
    pg->pg_abort = 0; /* JMP_M 4.19 11-05-95 */
    PG_read_flds(pg);
    if(ACT_exec_nu(pg->pg_begin_act) != 0) return(-3);

    // .... Check champ éditable
    // ....

    objnb = WscrFindPgNb(pg);
    if(objnb < 0) return(-1);
    hWnd = WSCR_HWNDS[objnb].sh_hwnd;
    stat = WSCR_HWNDS[objnb].sh_status;

    WSCR_HWNDS[objnb].sh_status = 0; // Get IO from fields (std)
    WSCR_HWNDS[objnb].sh_pgedit = 1;
    WpgEnableFlds(hWnd, pg, 0, TRUE);
//  WscrMenuEnable(objnb, FALSE);
    hWndParent = GetParent(hWnd);
    if(hWndParent) {
	ParentEn = IsWindowEnabled(hWndParent);
	EnableWindow(hWndParent, FALSE);
	}
//    PG_display_flds(pg);
    WpgEnableFlds(hWnd, pg, 0, TRUE);
    SetFocus(hWnd);
    /* BP_M 02-05-2003 11:34 */
    if(pg->pg_next_fld >= 0 && SCR_WGOTO_NEXT_FLD) PG_goto_field(pg, pg->pg_next_fld);

    /* LOOP MSG */
    while(GetMessage(&msg, (HWND)0, 0, 0)) {
	if(!IsDialogMessage(hWnd, &msg)) {
	    TranslateMessage(&msg);
	    DispatchMessage(&msg);
	    }
	if(WSCR_HWNDS[objnb].sh_pgedit == 0) break;
	}

    /* RESET */
    WSCR_HWNDS[objnb].sh_status = stat; // Reset IO
    WpgEnableFlds(hWnd, pg, 0, stat ? FALSE : TRUE);
//  WscrMenuEnable(objnb, TRUE);
    if(hWndParent) EnableWindow(hWndParent, ParentEn);
    SetFocus(hWnd);

    if(SCR_APP_ABORT || SCR_PG_ABORT) {
	SCR_PG_ABORT = 0;
	pg->pg_abort = 1;
	return(-2);
	}
//    if(ACT_exec_nu(pg->pg_end_act) != 0) goto ag;
    return(0);
}

PG_display_im(PAGE *pg)
{
    WscrPgDisplay(pg, 1);
    return(0);
}

PG_undisplay(PAGE *pg)
{
    WscrPgUndisplay(pg);
    pg->pg_operation = 0;
    pg->pg_im = 0;
    if(!pg->pg_fld_mem) PG_fld_free(pg);
    return(0);
}

WscrMenuEnable(int objnb, BOOL stat)
{
    HMENU   hMenu;
    int     i;
    APPL    *ap;
    A_BAR   *ab;

    if(WSCR_HWNDS[objnb].sh_act != ACT_APPL_RUN) return(-1);
    hMenu = WSCR_HWNDS[objnb].sh_hmn;
    if(hMenu == 0) return(-1);
    ap = (APPL *)WSCR_HWNDS[objnb].sh_ptr;
    ab = (A_BAR *)SCR_PTR(ap->ap_ab);
    for(i = 0 ; i < ab->ab_nb_opts ; i++)
	if(stat == TRUE)
	    EnableMenuItem(hMenu, i, MF_BYPOSITION | MF_ENABLED);
	else
	    EnableMenuItem(hMenu, i, MF_BYPOSITION | MF_GRAYED);
    DrawMenuBar(WSCR_HWNDS[objnb].sh_hwnd);
    return(0);
}

APPL *WapFindCurrentAp()
{
    int     objnb;

    objnb = WscrFindObjNb(hMainWnd);
    if(objnb < 0) return((APPL *)0);
    if(WSCR_HWNDS[objnb].sh_act != ACT_APPL_RUN) return((APPL *)0);
    return((APPL *)WSCR_HWNDS[objnb].sh_ptr);
}

int SCR_IGNORE_APP = 0;

APP_key(key)
int     key;
{
    int     i;
    U_int   fnk_act;                     /* JMP 28-03-92 */
    APPL    *ap;
    FNKEYS  *fk;
    FNKEY   *fnk;
    char    cmt[100];

    if(SCR_IGNORE_APP) return(0);
    ap = WapFindCurrentAp();
    if(ap == 0) return(0);

    fk = (FNKEYS *)SCR_PTR(ap->ap_fnks);
    if(fk != 0) {
	for(i = 0, fnk = fk->fk_fnks ; i < fk->fk_nb_fnks ; i++, fnk++)
	    if(fnk->fnk_key == key) {
		/* BP_M 05-03-2010 12:12 */
		if(SCR_check_prot_vt(fnk->fnk_prot) != 0) continue;
		/* BP_M 05-03-2010 12:12 */
		fnk_act = fnk->fnk_act;  /* JMP 28-03-92 */
		fnk->fnk_act = 0;        /* JMP 28-03-92 */
		SCR_strlcpy(cmt, SCR_CMT_TXT, 80); /* JMP38 02-11-92 */
		ACT_exec_nu(fnk_act);
		SCR_comment(cmt);           /* JMP38 02-11-92 */
		fnk->fnk_act = fnk_act;  /* JMP 28-03-92 */
		return(1);
	    }
    }
    return(0);
}

SCR_beep()
{
    MessageBeep(0xFFFFFFFF);
    return(0);
}

extern int *SCR_REC_KEYS;
extern int SCR_NB_REC_KEYS;


SCR_get_key()
{
    int         key;
    extern int  SCR_last_key;
    extern int  SCR_hitkey_inuse;

    if(SCR_NB_REC_KEYS > 0) {
	SCR_NB_REC_KEYS--;
	SCR_LKEY = SCR_REC_KEYS[SCR_NB_REC_KEYS];
	if(SCR_FN_KEY != 0) (*SCR_FN_KEY)(SCR_LKEY, 1);
	return(SCR_LKEY);
	}

//    if(SCR_last_key) {
//        SCR_LKEY = SCR_last_key;
//        SCR_last_key = 0;
//        return(SCR_LKEY);
//        }

    while(1) {
	SCR_hitkey_inuse = 0;
	key = SCR_hit_key();
	if(key) break;
	Sleep(20);
	}

//    SCR_last_key = 0;
    SCR_LKEY = WscrGetDosKey();
    if(SCR_FN_KEY != 0) (*SCR_FN_KEY)(SCR_LKEY, 0);

//    if(key == 13) // ENTER BP
//        key = SCR_ENTER;

    return(key);
}


SCR_check_key()
{
    extern int  SCR_hitkey_inuse;

    if(SCR_NB_REC_KEYS > 0) return(1);

    SCR_hitkey_inuse = 0;
    return((SCR_hit_key() == 0) ? 0 : 1);
}

SCR_clear_key()
{
    while(SCR_hit_key() != 0) SCR_get_key();
    return(0);
}

MP_display(mp)
MPAGE   *mp;
{
	PG_display(SCR_PTR(mp->mp_apg));
}

IMAGE *MN_save()           {return(0);}
MN_display_body()   {return(0);}
MN_display_box()    {return(0);}
MN_display()        {return(0);} /* BP_M 14-01-2003 12:57 */
MN_main_loop(mn, ab_key)
MENU    *mn;
int     ab_key;
{
    return(MN_edit(mn));
}
MN_undisplay()      {return(0);}
MP_edit()           {return(0);}

/*extern IMAGE   **PGIM_LIST;
extern int     NB_PGIM;
*/

IMAGE *PG_save(pg)
PAGE    *pg;
{
    extern IMAGE   **PGIM_LIST;
    extern int     NB_PGIM;
    int     i, flag = -1;

    if(PGIM_LIST == 0) PGIM_LIST = (IMAGE **)SCR_malloc(sizeof(IMAGE *) * 1024);

    for(i = 0 ; i < NB_PGIM ; i++) {
	if(PGIM_LIST[i] == 0) flag = i;
	if(PGIM_LIST[i] != 0 && pg->pg_im == PGIM_LIST[i]) break;
    }
    if(i == NB_PGIM) {
	if(flag >= 0)
	    i = flag;
	else
	    NB_PGIM++;
	PGIM_LIST[i] = (IMAGE *)SCR_NU(pg);
	PG_display_im(pg);
    }

    return(PGIM_LIST[i]);
}

PG_display(pg)
PAGE    *pg;
{
    PG_display_im(pg);
}

/*
SCR_display_image(im)
IMAGE   *im;
{
    int     i;
    extern IMAGE   **PGIM_LIST;
    extern int     NB_PGIM;

    if(im == 0) return(0);

    for(i = 0 ; i < NB_PGIM ; i++)
	if(im == PGIM_LIST[i]) break;

    if(i == NB_PGIM)
	return(0);

    PG_undisplay(PAGE_PTR((int)PGIM_LIST[i]));
    PGIM_LIST[i] = 0;
}
SCR_free_image(im)
IMAGE   *im;
{
    int     i;
    extern IMAGE   **PGIM_LIST;
    extern int     NB_PGIM;

    if(im == 0) return(0);

    for(i = 0 ; i < NB_PGIM ; i++)
	if(im == PGIM_LIST[i]) {
	    PG_undisplay(PAGE_PTR((int)PGIM_LIST[i]));
	    PGIM_LIST[i] = 0;
	    return(0);
	    }

    if((unsigned long)im < SCR_READ_FILES.tbl_nb_fl) return(0);

    SCR_free(im->im_cells);
    SCR_free(im);
    return(0);
}
*/

PG_disable(PAGE *pg)
{
    return(0);
}

PG_enable(PAGE *pg)
{
    return(0);
}


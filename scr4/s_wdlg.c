#include "scr4w.h"
#include <commctrl.h>

#define WISBUTTON(pg, fld_nb) ((pg)->pg_flds[fld_nb].fld_io + SCR_INPUT == SCR_ACCESSIBLE)


extern int *SCR_REC_KEYS;
extern int SCR_NB_REC_KEYS;

extern WNDPROC W_DLGEDITFN;
extern int SCR_EXEC_DISP_BEFORE; /* JMP 10-11-97 */

extern WNDPROC W_DLGEDITFN_DFT, W_DLGBUTTONFN_DFT, W_DLGCOMBOFN_DFT,
       W_DLGLISTBOXFN_DFT;
extern int WSCR_AUTOMOVE;
extern int WSCR_PGF1DEF;
extern int SCR_tabenter; /* JMP 16-10-99 */
extern int SCR_RECORD_KEY_WINDOW; /* BP_M 02-11-1999 */
extern PAGE    *WSCR_PG_NOACTS;
extern int WSCR_FLD_NOACTS;

BOOL CALLBACK WscrDlgProc(HWND, UINT, WPARAM, LPARAM);

WscrEditPage(PAGE *pg)
{
    int     rc;

    WscrRegisterObj((char *)pg, pg, ACT_PG_EDIT, 0, 0L);
    rc = WpgEditPage(pg);
    WscrUnRegisterObj((char *)pg);
    return(rc);
}


HWND WscrObjDisplay(PAGE *pg, int objnb, int status)
{
    HWND    hWnd = 0;
    DLGPROC lpDlgProc;

    if(pg->pg_im) return(hWnd);
    pg->pg_im = (IMAGE *)1;
    WSCR_HWNDS[objnb].sh_status = status; /* Disable Input fields */
    WSCR_HWNDS[objnb].sh_pgedit = 2; /* Don't quit page */
    SCR_PG_ABORT = 0;
    pg->pg_abort = 0;
    PG_read_flds(pg);
    lpDlgProc = MakeProcInstance((WNDPROC)WscrDlgProc, hInst);
    WSCR_HWNDS[objnb].sh_data = (char *)lpDlgProc;

    if(SCR_EXEC_DISP_BEFORE) ACT_exec_nu(pg->pg_disp_act); /* JMP 10-11-97 */
    hWnd = CreateDialogParam(hInst, MAKEINTRESOURCE(SCR_NU(pg)), GetLastActivePopup(hMainWnd), lpDlgProc, (LPARAM)pg);
    if(hWnd == 0) return(hWnd);

    /* Execute the begin function */
    if(SCR_EXEC_BEG_IN_DISP == 1) ACT_exec_nu(pg->pg_begin_act);
    if(PG_EXEC_DISP_FN && !SCR_EXEC_DISP_BEFORE) /* JMP 10-11-97 */
        ACT_exec_nu(pg->pg_disp_act);
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
    Wpg2Dlg(hWnd, pg);
    return(hWnd);
}

HWND WscrPgDisplay(PAGE *pg, int status)
{
    int     objnb;

    if(pg->pg_im) return((HWND)0);
    objnb = WscrRegisterObj((char *)pg, pg, ACT_PG_EDIT, 0, 0L);
    return(WscrObjDisplay(pg, objnb, status));
}

HWND WscrPgDisplayEx(PAGE *pg, int status)
{
    int     objnb;

    if(pg->pg_im) return((HWND)0);
    objnb = WscrFindPgNb(pg);
    if(objnb == 0)
        objnb = WscrRegisterObj((char *)pg, pg, ACT_PG_EDIT, 0, 0L);
    return(WscrObjDisplay(pg, objnb, status));
}

WscrObjUndisplay(PAGE *pg)
{
    int     objnb = WscrFindPgNb(pg);
    HWND    hWnd, hWndParent;     /* JMP 19-06-99 */

    if(objnb < 0) return(-1);
    hWnd = WSCR_HWNDS[objnb].sh_hwnd;
    WSCR_HWNDS[objnb].sh_status = WSCR_HWNDS[objnb].sh_pgedit = 0;
    hWndParent = GetParent(hWnd); /* JMP 19-06-99 */
    DestroyWindow(hWnd);
    pg->pg_im = 0;
    FreeProcInstance((WNDPROC) WSCR_HWNDS[objnb].sh_data);
    WscrUnRegisterObj((char *)pg);
    SetFocus(hWndParent); /* JMP 19-06-99 */
    return(0);
}

WscrPgUndisplay(PAGE *pg)
{
    WscrObjUndisplay(pg);
    return(0);
}


WpgEditPage(PAGE *pg)
{
    DLGPROC lpDlgProc;
    HWND    hWnd;

    //WSCR_PG_NOACTS = 0;

    SCR_PG_ABORT = 0;
    pg->pg_abort = 0;
    PG_read_flds(pg);
    if(ACT_exec_nu(pg->pg_begin_act) != 0) {
        return(-3);
    }

    PG_help_stack(pg, -1); /* JMP 20-11-01 */

    /* Edit Dialog Box */
    lpDlgProc = MakeProcInstance((WNDPROC)WscrDlgProc, hInst);
    DialogBoxParam(hInst, MAKEINTRESOURCE(SCR_NU(pg)), GetLastActivePopup(hMainWnd), lpDlgProc, (LPARAM)pg);
    FreeProcInstance(lpDlgProc);

    HLP_stack_del(1);

    PG_fld_free(pg);
    pg->pg_im = 0;
    if(SCR_LKEY == SCR_F3 || SCR_LKEY == SCR_ESCAPE) SCR_PG_ABORT = 1; /* BP_M 31-05-2001 11:54 */
    if(SCR_APP_ABORT || SCR_PG_ABORT) {
        SCR_PG_ABORT = 0;
        pg->pg_abort = 1;
        return(-2);
    }
    return(0);
}


WscrDlgCommand(HWND hWnd, UINT iId, HWND hCtl, UINT nNotify)
{
    static int last_attr;
    int     fld_nb, winkey;
    int     wscrnb = WscrFindObjNb(hWnd);
    PAGE    *pg;
    FIELD   *fld;
    HWND    hWndChild;
    int     i, old_key;         /* BP_M 12-02-2003 16:38 */
    extern  int SCR_tabenter;
    FNKEYS *fk;                 /* BP_M 13-02-2003 10:58 */
    char    *edtxt;             /* BP_M 13-02-2003 10:58 */
    static char *fldtxt;
    static PAGE *pg_fldtxt;

    if(wscrnb < 0) return(0);
    pg = WSCR_HWNDS[wscrnb].sh_pg;

    if(iId == IDOK || iId == IDCANCEL) {
        if(iId == IDOK) winkey = VK_RETURN;
        else            winkey = VK_ESCAPE;

        switch(WscrDlgFnkey(hWnd, pg, pg->pg_csr_fld, winkey)) {
        case 1 :
            if(pg->pg_csr_fld >= 0 && pg->pg_csr_fld < pg->pg_nb_flds) {
                hWndChild = GetDlgItem(hWnd, pg->pg_csr_fld + WFLD_FIRST);
                WscrSendRecord(hWndChild);
            }
            return(0);
        case 2 :
            PostMessage(hWnd, WM_SYSCOMMAND, WM_USER + 513, 0L);
            return(0);
        default:
            if(iId == IDOK && SCR_tabenter && pg->pg_tabenter) {
                hWndChild = GetDlgItem(hWnd, pg->pg_csr_fld + WFLD_FIRST);
                PostMessage(hWndChild, WM_KEYDOWN, VK_TAB, 0x08000001);
                PostMessage(hWndChild, WM_KEYUP, VK_TAB, 0xC8000001);
                return(0);
            }
            break;
        }

        if(pg->pg_operation == 4 && iId == IDOK) {
            PostMessage(hWnd, WM_SYSCOMMAND, WM_USER + 513, 0L);
            return(0);
        }
    }

    if(iId < WFLD_FIRST) {
        WpgDlg2Pg(hWnd, pg);
        if(WSCR_HWNDS[wscrnb].sh_fn == 0) WscrActExec(iId - WACT_SHIFT);
        else (WSCR_HWNDS[wscrnb].sh_fn)(hWnd, WSCR_HWNDS[wscrnb].sh_ptr, iId);

        if(SCR_APP_ABORT || SCR_PG_ABORT)
            PostMessage(hWnd, WM_SYSCOMMAND, WM_USER + 513, 0L);
        else
            Wpg2Dlg(hWnd, pg);
        return(0);
    }

    if(iId >= WFLD_DIRBUTTON) {
        if(nNotify != BN_CLICKED) return(0);
        fld_nb = iId - WFLD_DIRBUTTON;
        if(fld_nb < 0) return(0);
        if(WscrEditDirFld(hWnd, pg, fld_nb)) return(0);
        WpgFld2Dlg(hWnd, pg, fld_nb);
        return(0);
    }

    fld_nb = iId - WFLD_FIRST;
    if(fld_nb < 0 || fld_nb >= pg->pg_nb_flds) return(0);
    fld = pg->pg_flds + fld_nb;

    if(nNotify == EN_SETFOCUS || nNotify == CBN_SETFOCUS ||
            nNotify == BN_SETFOCUS) {
        HLP_stack_del(2); /* JMP 20-11-01 */
        PG_help_stack(pg, fld_nb); /* JMP 20-11-01 */
        PG_calc_cmt(pg, fld_nb);
        WscrDisplayCmt(hWnd);
        pg->pg_csr_fld = fld_nb;
        if(WSCR_PG_NOACTS == pg && WSCR_FLD_NOACTS == fld_nb) WSCR_PG_NOACTS = 0;
        if(nNotify == EN_SETFOCUS) {
            if(fld->fld_begin_act && pg != WSCR_PG_NOACTS) {      /* BP_M 10-01-2000 */
                old_key = SCR_LKEY;
                ACT_exec_nu(fld->fld_begin_act);
                SCR_LKEY = old_key;
            }
            Wpg2Dlg(hWnd, pg);
            if(fld->fld_nlv < 2) {
                /*
                if(pg->pg_flds[fld_nb].fld_compulsory + SCR_NOT_COMPULSORY ==
                    SCR_REQUIRED) {
                	last_attr = pg->pg_flds[fld_nb].fld_attributes;
                	pg->pg_flds[fld_nb].fld_attributes = SCR_RED;
                	}
                */
                if(fld->fld_fmt == 0)
                    SendDlgItemMessage(hWnd, iId, EM_SETSEL, 0, -1L); /* JMP 12-04-00 */
                else {
                    char   fmt[80];
                    strcpy(fmt, PG_fld_fmt(pg, fld_nb));
                    if(fld->fld_type == SCR_DATE || fld->fld_type == SCR_TIME)
                        SCR_fmt_dt(fmt, fmt);
                    WscrDlgSetCurRight(hCtl, -1, fmt); /* JMP 13-05-00 */
                }
            }
            if(fldtxt) SCR_free(fldtxt);            /* BP_M 17-02-2003 17:02 */
            fldtxt = PG_aget_rec_txt(pg, fld_nb);   /* BP_M 17-02-2003 17:02 */
            pg_fldtxt = pg;
            fld->fld_modified = 0;
        }
    }

    if(nNotify == EN_KILLFOCUS || nNotify == CBN_KILLFOCUS ||
            nNotify == BN_KILLFOCUS) {

        if(PG_is_fnk(pg, fld_nb)) {
            fk = (FNKEYS *)SCR_PTR(fld->fld_fnks);
            i = FNK_is_fnk(fk, SCR_LKEY);
            if(i >= 0) {
                if(fk->fk_fnks[i].fnk_act > 0)
                    return(0);
            }
            else {
                fk = (FNKEYS *)SCR_PTR(pg->pg_fnks);
                i = FNK_is_fnk(fk, SCR_LKEY);
                if(i >= 0) {
                    if(fk->fk_fnks[i].fnk_act > 0)
                        return(0);
                }
            }
        }


        HLP_stack_del(2);  /* JMP 20-11-01 */
        if(SCR_APP_ABORT == 0 && SCR_PG_ABORT == 0) {
            WpgDlg2Fld(hWnd, pg, fld_nb);
            WSCR_PG_NOACTS = pg;
            WSCR_FLD_NOACTS = fld_nb;
            if(SCR_apply_cmp(pg, fld_nb)) SetFocus(hCtl);
            else {
                WSCR_PG_NOACTS = 0;
                WSCR_FLD_NOACTS = 0;
            }

            edtxt = PG_aget_rec_txt(pg, fld_nb);
            PG_user_fn(pg, fld_nb, edtxt);

            /* BP_M 17-02-2003 17:12 */
            if(pg_fldtxt == pg && strncmp(edtxt, fldtxt, max(strlen(edtxt), strlen(fldtxt)) - 1) != 0)
                fld->fld_modified = 1;
            else
                fld->fld_modified = 0;

            SCR_free(edtxt);

            if(fld->fld_end_act && WSCR_PG_NOACTS != pg) {      /* BP_MODIF 22-02-99 */
                old_key = SCR_LKEY;
                WSCR_PG_NOACTS = pg;
                WSCR_FLD_NOACTS = fld_nb;

                /* BP_M 13-02-2003 11:00 */

                i = ACT_exec_nu(fld->fld_end_act);
                SCR_LKEY = old_key;
                if(i != 0) SetFocus(hCtl);
                else {
                    WSCR_PG_NOACTS = 0;
                    WSCR_FLD_NOACTS = 0;
                    if(fld->fld_next >= 0 && fld->fld_next <= pg->pg_nb_flds)  /* BP_M 15-03-2012 14:22 */
                        pg->pg_next_fld = fld->fld_next;
//                      PG_goto_field(pg, fld->fld_next);
                }
            }
            Wpg2Dlg(hWnd, pg);
            /*
            	    if(pg->pg_order == 0) {
            		for(i = fld_nb + 1 ; i < pg->pg_nb_flds ; i++)
            		    if(pg->pg_flds[i].fld_io == 0) break;
            		if(i == pg->pg_nb_flds)
            		    for(i = 0 ; i < fld_nb ; i++)
            			if(pg->pg_flds[i].fld_io == 0) break;
            		if(i != fld_nb)
            		    PG_goto_field(pg, i);
            	    }
            */
            /* pg->pg_flds[fld_nb].fld_attributes = last_attr; /* JMP 09-05-00 */
        }
    }
    else {
        if(!WISBUTTON(pg, fld_nb)) return(0);
        if(nNotify != BN_CLICKED) return(0);
        SCR_LKEY = SCR_ENTER;
        PG_fnkeys(pg, fld_nb);
        PostMessage(hCtl, WM_USER + 512, 0, 0L);
    }
    return(0);
}

WscrActExec(int act_nu)
{

    if(act_nu < 0 || act_nu >= SCR_READ_FILES.tbl_nb_fl) return(0);
    if(SCR_READ_FILES.tbl_fl[act_nu]->fl_type != TYPE5(SCR_TYPE_ACT)) return(0);
    return(ACT_exec_nu(act_nu));
}

WscrSendRecord(HWND hWnd)
{
    int     key;

    return(0); /* JMP 07-05-00 */

    if(SCR_RECORD_KEY_WINDOW != 0) {
        key = SCR_RECORD_KEY_WINDOW;
        SCR_RECORD_KEY_WINDOW = 0;
    }
    else {
//      if(hWnd != GetFocus() || SCR_NB_REC_KEYS <= 0) return(0);
        if(SCR_NB_REC_KEYS <= 0) return(0);
        SCR_NB_REC_KEYS--;
        key = SCR_REC_KEYS[SCR_NB_REC_KEYS];
    }
    if(key == SCR_HELP_KEY) {
        HLP_stack_edit();
        return(0);
    }
    return(WscrRecordKey2(key));
}
/*
	key = WscrKeyScrToWin(key);
	if(key == 0) return(0);
	PostMessage(hWnd, WM_KEYDOWN, key, 0x08000001);
	PostMessage(hWnd, WM_KEYUP, key, 0xC8000001);
	}
    else {
	PostMessage(hWnd, WM_CHAR, key, 0L);
	}
    return(0);
*/


PG_goto_vobj(OBJ obj, int i)
{
    return(PG_goto_field((PAGE *)obj.obj_ptr, obj.obj_fld + i));
}

PG_goto_field(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    HWND    hWnd, hWndFld;

//    if(pg->pg_csr_fld == fld_nb) return(0);
    pg->pg_next_fld = fld_nb;
    SCR_FLD_SKIP = 1;

    hWnd = WscrFindHwndPg(pg);
    hWndFld = GetDlgItem(hWnd, fld_nb + WFLD_FIRST);
    SetFocus(hWndFld);

    return(0);
}

#include "s_udfld.h"

WscrDlgFnkey(HWND hWnd, PAGE *pg, int fld_nb, int winkey)
{
    int     scrkey, wscrnb, i, rc;

    SCR_LKEY = WscrKeyWinToScr(winkey);

    if(SCR_LKEY == 0) return(0);

    /* BP_M 02-11-1999 */
    if(SCR_tabenter && pg->pg_flds[fld_nb].fld_nb_line == 1) {
        switch(SCR_LKEY) {
        case SCR_CSR_UP:
            i = PG_up_field(pg, pg->pg_csr_fld);
            PG_goto_field(pg, i);
            break;
        case SCR_CSR_DOWN:
            i = PG_dn_field(pg, pg->pg_csr_fld);
            PG_goto_field(pg, i);
            break;
        }
    }


    //  if(SCR_app_key() && SCR_LKEY == SCR_F1) WSCR_PGF1DEF = 1;
    if(SCR_LKEY == SCR_F1) WSCR_PGF1DEF = 1;
    if(APP_key(SCR_LKEY)) return(1);
    if(SCR_LKEY == SCR_F1) WSCR_PGF1DEF = 0;
    if(SCR_LKEY == SCR_ENTER &&
            fld_nb >= 0 && fld_nb < pg->pg_nb_flds &&
            WISBUTTON(pg, fld_nb)) return(0);

    pg->pg_last_key = SCR_LKEY;
    if(PG_is_fnk(pg, fld_nb)) {
        if(SCR_LKEY == SCR_F1) WSCR_PGF1DEF = 1;
        WpgDlg2Fld(hWnd, pg, fld_nb);
        if(SCR_LKEY == SCR_F10 /* && SCR_tabenter && pg->pg_tabenter */) {
            wscrnb = WscrFindObjNb(hWnd);
            if(WSCR_HWNDS[wscrnb].sh_act == ACT_MP_EDIT_ISAM) {
                if(WSCR_HWNDS[wscrnb].sh_fn != 0)
                    (WSCR_HWNDS[wscrnb].sh_fn)(hWnd, WSCR_HWNDS[wscrnb].sh_ptr, IDOK);

                if(SCR_APP_ABORT || SCR_PG_ABORT)
                    PostMessage(hWnd, WM_SYSCOMMAND, WM_USER + 513, 0L);
                else {
                    SetFocus(hWnd);
                    Wpg2Dlg(hWnd, pg);
                }

                return(0);
            }
        }
        //if(PG_fnkeys(pg, fld_nb) || SCR_PG_ABORT || SCR_APP_ABORT) return(2); /* JMP 14-04-00 */
        rc = PG_fnkeys(pg, fld_nb); /* JMP 14-04-00 */
        if(rc == 1 || SCR_PG_ABORT || SCR_APP_ABORT) return(2); /* JMP 14-04-00 */

        if(SCR_EXPORT_YN) {
            PG_set_rec(pg, fld_nb, SCR_EXPORT_TXT);
            // PG_display_field(pg, fld_nb);
            SCR_EXPORT_YN = 0;
        }

        Wpg2Dlg(hWnd, pg);
        if(fld_nb >= 0) SetFocus(GetDlgItem(hWnd, fld_nb + WFLD_FIRST)); /* JMP 13-09-97 */
        return(1);
    }

    /*
        if(SCR_app_key()) return(2);
    */

    return(0);
}


LONG FAR PASCAL WscrDlgEditChild(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    int             fld_nb, iId, i;
    HWND            hWndParent = GetParent(hWnd);
    PAGE            *pg = WscrFindPage(hWndParent);
    FIELD           *fld;
    char            buf[30], *allowed;
    unsigned long   lval;
//    extern long     atol();
//    LRESULT         *lpWinProc(HWND, UINT, WPARAM, LPARAM);
    WNDPROC         lpWinProc;

    iId = fld_nb = GetDlgCtrlID(hWnd) - WFLD_FIRST;

    if(fld_nb >= 1000) fld_nb -= 1000;
    if(fld_nb < 0) goto dft;
    fld = pg->pg_flds + fld_nb;
    // WscrSendRecord(hWnd); /* JMP 04-05-00 */

    if(wMsg == WM_KEYDOWN || wMsg == WM_SYSKEYDOWN || wMsg == WM_USER + 512) {
        switch(WscrDlgFnkey(GetParent(hWnd), pg, fld_nb, wParam)) {
        case 1 :
            WscrSendRecord(hWnd);
            return(0L);
        case 2 :
            PostMessage(hWndParent, WM_SYSCOMMAND, WM_USER + 513, 0L);
            return(0L);
        default:
            break;
        }
    }
    else
        WscrSendRecord(hWnd); /* JMP 04-05-00 */

    if(pg->pg_flds[fld_nb].fld_fmt &&
            (wMsg == WM_CHAR || wMsg == WM_KEYDOWN || wMsg == WM_LBUTTONUP)) {
        WscrDlgEditFmt(hWnd, wMsg, wParam, lParam, pg, fld_nb);
        return(0L);
    }

    /* BP_M 12-05-2010 18:11 */
    if(wMsg == WM_CHAR && wParam != VK_BACK && wParam != 3 && wParam != 22) {
        allowed = (unsigned char *)PG_fld_allowed(pg, fld_nb);
        if(allowed == 0)
            switch(fld->fld_type) {
            case SCR_NATURAL :
                allowed = SCR_NATS;
                break;
            case SCR_INTEGER :
                allowed = SCR_INTS;
                break;
            case SCR_REAL    :
                allowed = SCR_REALS;
                break;
            case SCR_CHECK_BOX:
                allowed = SCR_AL_CHECK_BOX;
                break;
            case SCR_DATE    :
                allowed = SCR_DATS;
                break;
            case SCR_TIME    :
                allowed = SCR_TIMS;
                break;
            default :
                break;
            }
        if(allowed != 0) {
            for(i = 0 ; allowed[i] ; i++)
                if(allowed[i] == wParam) break;
            if(allowed[i] == 0) {

                if(fld->fld_if_alfa) {
                    SCR_record_key(wParam);
//                  SCR_RECORD_KEY_WINDOW = wParam;
                    SCR_sleep(3);
                    ACT_exec_nu(fld->fld_if_alfa);
                    return(0L);
                }

                SCR_beep();
                return(0L);
            }
        }

        switch(fld->fld_type) {
        case SCR_NATURAL :
            if((wParam >= '0' && wParam <= '9')) goto dft;
            GetWindowText(hWnd, buf, 20);
            lval = atol(buf);
            if(wParam == '-') {
                if(lval == 0) break;
                lval--;
            }
            else if(wParam == '+') lval++;
            else break;
            sprintf(buf, "%lu", lval);    /* BP_M 20-02-2003 15:35 */
            SetWindowText(hWnd, buf);
            return(0L);

        case SCR_DATE :
        {
            int       from, to;
            SendMessage(hWnd, EM_GETSEL, (WPARAM)&from, (LPARAM)&to); /* JMP 12-04-00 */
            PG_display_error("Pos : %d-%d", from, to);
        }
        if(wParam != 'd' && wParam != 'D' &&
                wParam != 'j' && wParam != 'J') goto dft;
        SCR_set_field_long(pg, fld_nb, SCR_current_date());
        WpgFld2Dlg(hWndParent, pg, fld_nb);
        return(0L);

        case SCR_TIME :
            if(wParam != 't' && wParam != 'T') goto dft;
            SCR_set_field_long(pg, fld_nb, SCR_current_time());
            WpgFld2Dlg(hWndParent, pg, fld_nb);
            return(0L);

        case SCR_INTEGER:
            if((wParam >= '0' && wParam <= '9') || wParam == '-') goto dft;
            break;

        case SCR_REAL   :
            if((wParam >= '0' && wParam <= '9') ||
                    wParam == '-'  ||
                    wParam == '+'  ||
                    wParam == '.'  ||
                    wParam == 'e'  || wParam == 'E') goto dft;
            break;

        case SCR_TYPE_MENU :
        case SCR_STRING :
        case SCR_PASSWD :
        case SCR_DIR :
        case SCR_EDITOR :

        default :
            goto dft;
        }
        SCR_beep();
        return(0L);
    }

dft:

    if(iId == fld_nb + 1000)  // SCR_DIR
        lpWinProc = W_DLGBUTTONFN_DFT;
    else if(fld->fld_io + SCR_INPUT == SCR_ACCESSIBLE)
        lpWinProc = W_DLGBUTTONFN_DFT;
    else if(fld->fld_type == SCR_TYPE_MENU)
        lpWinProc = W_DLGCOMBOFN_DFT;
    else if(fld->fld_type == SCR_TYPE_RADIO || fld->fld_type == SCR_CHECK_BOX)
        lpWinProc = W_DLGBUTTONFN_DFT;
    else lpWinProc = W_DLGEDITFN_DFT;

    return(CallWindowProc(lpWinProc, hWnd, wMsg, wParam, lParam));
}

WscrDlgGetCurPos(HWND hWnd)
{
    int       from, to;

    SendMessage(hWnd, EM_GETSEL, (WPARAM)&from, (LPARAM)&to);
    return(from);
}

WscrDlgSetCurPos(HWND hWnd, int pos)
{
    SendMessage(hWnd, EM_SETSEL, (WPARAM)pos, (LPARAM)(pos + 1));
}

WscrDlgCalcCurRight(int pos, char *fmt)
{
    pos++;
    while(!U_is_in(fmt[pos], "09Xx") && pos < strlen(fmt)) pos++;
    if(pos >= strlen(fmt)) pos = strlen(fmt) - 1;
    return(pos);
}

WscrDlgSetCurRight(HWND hWnd, int pos, char *fmt)
{
    pos = WscrDlgCalcCurRight(pos, fmt);
    WscrDlgSetCurPos(hWnd, pos);
}

WscrDlgCalcCurLeft(int pos, char *fmt)
{
    pos--;
    while(!U_is_in(fmt[pos], "09Xx") && pos >= 0) pos--;
    if(pos < 0) pos = 0;
    return(pos);
}

WscrDlgSetCurLeft(HWND hWnd, int pos, char *fmt)
{
    pos = WscrDlgCalcCurLeft(pos, fmt);
    WscrDlgSetCurPos(hWnd, pos);
}

WscrDlgEditFmt(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam, PAGE *pg, int fld_nb)
{
    int     pos;
    char    fmt[80], buf[80];
    FIELD   *fld = pg->pg_flds + fld_nb;


    GetWindowText(hWnd, buf, 80);

    strcpy(fmt, PG_fld_fmt(pg, fld_nb));
    if(fld->fld_type == SCR_DATE || fld->fld_type == SCR_TIME)
        SCR_fmt_dt(fmt, fmt);

    pos = WscrDlgGetCurPos(hWnd);
    if(!U_is_in(fmt[pos], "09xX")) pos = WscrDlgCalcCurRight(pos, fmt);
    if(!U_is_in(fmt[pos], "09xX")) pos = WscrDlgCalcCurLeft(pos, fmt);

    if(wMsg == WM_LBUTTONUP) {
        pos = WscrDlgGetCurPos(hWnd);
        if(!U_is_in(fmt[pos], "09xX")) pos = WscrDlgCalcCurRight(pos, fmt);
        if(!U_is_in(fmt[pos], "09xX")) pos = WscrDlgCalcCurLeft(pos, fmt);
        WscrDlgSetCurPos(hWnd, pos);
        goto dft;
    }
    if(wMsg == WM_KEYDOWN) {
        switch(wParam) {
        case VK_RIGHT :
        case VK_DOWN :
            WscrDlgSetCurRight(hWnd, pos, fmt);
            return(0L);

        case VK_LEFT :
        case VK_UP :
            WscrDlgSetCurLeft(hWnd, pos, fmt);
            return(0L);

        case VK_DELETE :
            if(U_is_in(fmt[pos], "90")) buf[pos] = '0';
            else                        buf[pos] = ' ';
            SetWindowText(hWnd, buf);
            WscrDlgSetCurRight(hWnd, pos, fmt);
            return(0L);

        case VK_HOME :
            WscrDlgSetCurRight(hWnd, -1, fmt);
            return(0L);
        case VK_END :
            WscrDlgSetCurLeft(hWnd, strlen(fmt), fmt);
            return(0L);
        default :
            break;
        }
    }

    else if(wMsg == WM_CHAR) {
        if(wParam == VK_BACK) {
            if(U_is_in(fmt[pos], "90")) buf[pos] = '0';
            else                        buf[pos] = ' ';
            SetWindowText(hWnd, buf);
            WscrDlgSetCurLeft(hWnd, pos, fmt);
            return(0L);
        }

        if(fld->fld_type == SCR_DATE && (SCR_upper_char(wParam) == 'J' ||
                                         SCR_upper_char(wParam) == 'D')) {
            SCR_set_field_long(pg, fld_nb, SCR_current_date());
            WpgFld2Dlg(GetParent(hWnd), pg, fld_nb);
            WscrDlgSetCurRight(hWnd, -1, fmt);
            return(0L);
        }
        if(fld->fld_type == SCR_TIME && (SCR_upper_char(wParam) == 'T')) {
            SCR_set_field_time(pg, fld_nb, SCR_current_time());
            WpgFld2Dlg(GetParent(hWnd), pg, fld_nb);
            WscrDlgSetCurRight(hWnd, -1, fmt);
            return(0L);
        }

        if(WscrIsCtrl() && U_is_in(wParam, "cC")) goto dft;
        switch(fmt[pos]) {
        case '9':
        case '0':
            if((wParam < '0' || wParam > '9')) {
                SCR_beep();
                return(0L);
            }
            else {
                buf[pos] = wParam;
            }
            break;

        default:
            if(fld->fld_case == 2) wParam = SCR_upper_char(wParam);
            if(fld->fld_case == 1) wParam = SCR_lower_char(wParam);
            buf[pos] = wParam;
            break;
        }

        SetWindowText(hWnd, buf);
        WscrDlgSetCurRight(hWnd, pos, fmt);
        return(0L);
    }
dft:
    return(CallWindowProc(W_DLGEDITFN_DFT, hWnd, wMsg, wParam, lParam));
}

WscrDlgInit(HWND hWnd, PAGE *pg, int props)
{
    APPL    *ap;
    MPAGE   *mp;
    BC      *bc;
    FIELD   *fld;
    MENU    *mn;
    int     i, j, wscrnb;
    HWND    hWndEdit, hWndDir;
    HANDLE  hFont;
    U_ch    *ptr, buf[40];
    long    style = GetWindowLongPtr(hWnd, GWL_STYLE);

    WSCR_AUTOMOVE = 1;
    WscrRegisterPgHwnd(pg, hWnd);


    wscrnb = WscrFindObjNb(hWnd);
    if(wscrnb < 0) return(-1);
    pg->pg_im = (IMAGE *)1;
    if(WSCR_HWNDS[wscrnb].sh_hmn)
        SetMenu(hWnd, WSCR_HWNDS[wscrnb].sh_hmn);

    if(W_DLGEDITFN == 0)
        W_DLGEDITFN = MakeProcInstance((WNDPROC) WscrDlgEditChild, hInst);

    // MPAGE 28000
    if(WSCR_HWNDS[wscrnb].sh_act == ACT_MP_EDIT_ISAM) {
        hWndEdit = GetDlgItem(hWnd, WMP_SCROLLID);
        if(hWndEdit) {
            mp = (MPAGE *)WSCR_HWNDS[wscrnb].sh_ptr;
            WmpSetScroll(hWndEdit, mp);
            EnableWindow(hWndEdit, FALSE);
            if(WscrMpNbFlds(hWnd, mp) == 0) WmpEdit(hWnd, mp);
        }
    }

    // BC 27000
    if(WSCR_HWNDS[wscrnb].sh_act == ACT_BC_EDIT_ISAM) {
        hWndEdit = GetDlgItem(hWnd, WBC_SCROLLID);
        if(hWndEdit) {
            bc = (BC *)WSCR_HWNDS[wscrnb].sh_ptr;
            WbcSetScroll(hWndEdit, bc);
            EnableWindow(hWndEdit, FALSE);
        }
    }

    // APPL
    if(WSCR_HWNDS[wscrnb].sh_act == ACT_APPL_RUN) {
        ap = (APPL *)WSCR_HWNDS[wscrnb].sh_ptr;
        switch(ap->ap_type) {
        case SCR_TYPE_PAGE :
            WpgEnableFlds(hWnd, pg, 0, !WSCR_HWNDS[wscrnb].sh_status); /* JMP 03-05-97 */
            SetFocus(hWnd);
            break;
        case SCR_TYPE_BC :
            bc = (BC *)SCR_PTR(ap->ap_ptr);
            hWndEdit = GetDlgItem(hWnd, WBC_SCROLLID);
            if(hWndEdit) {
                WbcSetScroll(hWndEdit, bc);
                EnableWindow(hWndEdit, FALSE);
            }
            WpgEnableFlds(hWnd, pg, 0, !WSCR_HWNDS[wscrnb].sh_status);
        }
        if(hMainWnd == 0) hMainWnd = hWnd;
    }

    // PG_DISPLAY
    if(WSCR_HWNDS[wscrnb].sh_act == ACT_PG_EDIT &&
            WSCR_HWNDS[wscrnb].sh_status) {
        WpgEnableFlds(hWnd, pg, 0, FALSE);
    }

    pg->pg_csr_fld = -1;
    for(i = 0 ; i < pg->pg_nb_flds; i++) {
        fld = pg->pg_flds + i;
        hWndEdit = GetDlgItem(hWnd, i + WFLD_FIRST);

        if(WISBUTTON(pg, i)) {
            if(W_DLGBUTTONFN_DFT == 0)
                W_DLGBUTTONFN_DFT = (WNDPROC)GetWindowLongPtr(hWndEdit, GWLP_WNDPROC);
            SetWindowLongPtr(hWndEdit, GWLP_WNDPROC, (long)W_DLGEDITFN);
            if(SCR_tabenter && pg->pg_tabenter) {               /* JMP 16-10-99 */
                style = GetWindowLongPtr(hWndEdit, GWL_STYLE);     /* JMP 16-10-99 */
                if(style & BS_DEFPUSHBUTTON) {                  /* JMP 16-10-99 */
                    style &= ~BS_DEFPUSHBUTTON;                 /* JMP 16-10-99 */
                    SetWindowLongPtr(hWndEdit, GWL_STYLE, style);  /* JMP 16-10-99 */
                }                                           /* JMP 16-10-99 */
            }                                               /* JMP 16-10-99 */
        }

        else switch(fld->fld_type) {
            case SCR_DATE :
            case SCR_TIME :
            case SCR_STRING :
            case SCR_PASSWD :
            case SCR_EDITOR :
            case SCR_INTEGER :
            case SCR_NATURAL :
            case SCR_DIR :
            case SCR_REAL :
                if(W_DLGEDITFN_DFT == 0)
                    W_DLGEDITFN_DFT = (WNDPROC)GetWindowLongPtr(hWndEdit, GWLP_WNDPROC);
                SetWindowLongPtr(hWndEdit, GWLP_WNDPROC, (long)W_DLGEDITFN);

                if(fld->fld_type == SCR_DIR) {
                    hWndDir =  GetDlgItem(hWnd, i + WFLD_FIRST + 1000);
                    if(W_DLGBUTTONFN_DFT == 0)
                        W_DLGBUTTONFN_DFT = (WNDPROC)GetWindowLongPtr(hWndDir, GWLP_WNDPROC);
                    SetWindowLongPtr(hWndDir, GWLP_WNDPROC, (long)W_DLGEDITFN);
                }
                break;

            case SCR_TYPE_RADIO :
            case SCR_CHECK_BOX :
                if(W_DLGBUTTONFN_DFT == 0)
                    W_DLGBUTTONFN_DFT = (WNDPROC)GetWindowLongPtr(hWndEdit, GWLP_WNDPROC);
                SetWindowLongPtr(hWndEdit, GWLP_WNDPROC, (long)W_DLGEDITFN);
                break;

            case SCR_TYPE_MENU :
                if(W_DLGCOMBOFN_DFT == 0)
                    W_DLGCOMBOFN_DFT = (WNDPROC)GetWindowLongPtr(hWndEdit, GWLP_WNDPROC);
                SetWindowLongPtr(hWndEdit, GWLP_WNDPROC, (long)W_DLGEDITFN);

                mn = (MENU *) SCR_PTR(fld->fld_menu);
                for(j = 0 ; j < mn->mn_nb_opts ; j++) {
                    ptr = WscrOemToAnsi(mn->mn_opts[j].opt_text);
                    SendDlgItemMessage(hWnd, i + WFLD_FIRST, CB_ADDSTRING, 0, (LONG)ptr);
                    SCR_free(ptr);
                }
                break;

            default:
                break;
            }

        if(fld->fld_type != SCR_TYPE_RADIO && fld->fld_type != SCR_CHECK_BOX) {
            //  if(fld->fld_fmt)
            //      hFont = WscrGetFont(SCR_WIN_FIXED);
            //  else
            hFont = WscrGetFont(fld->fld_attributes);

            SendMessage(hWndEdit, WM_SETFONT, (WPARAM) hFont, TRUE);
        }

        j = 0;
        if(fld->fld_type != SCR_EDITOR)
            j = fld->fld_nb_line * fld->fld_nb_column;
        if(fld->fld_type != SCR_TYPE_RADIO && fld->fld_type != SCR_CHECK_BOX)
            SendDlgItemMessage(hWnd, i + WFLD_FIRST, EM_LIMITTEXT, j, 0L);
    }

    /* SET GROUPS PROPS */
    for(i = 0 ; i < pg->pg_nb_grpbox; i++) {
        SendDlgItemMessage(hWnd, WGRP_FIRST + i, WM_SETFONT, (WPARAM) W_SCRL_FONTB, TRUE);
    }

    /* Set Values */
    if(PG_EXEC_DISP_FN) ACT_exec_nu(pg->pg_disp_act);
    Wpg2Dlg(hWnd, pg);

    if(!props) {   // Si TOOLBAR associé à l'objet et pas PROPS
        if(pg->pg_nb_tb) {
            WscrMoveDlgChildren(hWnd, 0, 25);
            WscrCreateToolbar(hWnd, (WTOOLBAR *)SCR_PTR(pg->pg_toolb[0]));
        }
    }
    if(!props && SCR_CMT.c_line >= 0) { // Si comments et pas PROPS
        WscrStatusCreate(hWnd);
        WscrStatusSetText(hWnd, "");
    }
    WscrResizeDlgChildren(hWnd);
    WscrCenterWindow(hWnd, pg->pg_w_x, pg->pg_w_y);
    WSCR_AUTOMOVE = 0;

    if(SCR_RECORD_KEY_WINDOW) WscrSendRecord(hWnd);

    return(0);
}


WscrDlgSysCommand(HWND hWnd, WORD wP, int x, int y)
{
    int     wscrnb = WscrFindObjNb(hWnd), rc = 0;
    PAGE    *pg;

    if(wscrnb < 0) return(FALSE);
    pg = WSCR_HWNDS[wscrnb].sh_pg;

    if(WSCR_HWNDS[wscrnb].sh_act == 1) return(FALSE);
    if(wP != SC_CLOSE && wP != WM_USER + 513) return(FALSE);
    WpgDlg2Pg(hWnd, pg);
    if(pg->pg_operation != 4) rc = PG_check_required(pg);
    if(SCR_PG_ABORT || SCR_APP_ABORT || wP == SC_CLOSE ||
            (rc == 0 && ACT_exec_nu(pg->pg_end_act) == 0)) {
        if(wP == SC_CLOSE) {
            pg->pg_last_key = SCR_LKEY = PG_find_abkey(pg);
            SCR_PG_ABORT = 1;
            pg->pg_abort = 1;
        }

        switch(WSCR_HWNDS[wscrnb].sh_pgedit) {
        case 0 :
            pg->pg_im = (IMAGE *)1;
            EndDialog(hWnd, TRUE);
            if(hMainWnd == hWnd) hMainWnd = 0; /* JMP 13-04-00 */
            return(TRUE);
        case 1 :
            WSCR_HWNDS[wscrnb].sh_pgedit = 0;
            return(TRUE);
        case 2 :
            return(TRUE);
        }
    }

    if(rc) {
        // pg->pg_flds[pg->pg_csr_fld].fld_attributes = SCR_RED; /* JMP 09-05-00 */
        PG_goto_field(pg, pg->pg_csr_fld); /* JMP 08-05-00 */
        SCR_beep();
    }
    return(TRUE);
}

HBRUSH WscrDlgColor(PAGE *pg, int fld_nb, HDC hDC)
{
    int         attr;
    COLORREF    color;
    HBRUSH      hBrush;
    extern      COLORREF WSCR_COLORS[][2];

    if(WSCR_NOCOLOR || fld_nb < 0) return((HBRUSH) 0); /* JMP 29-04-00 */
    fld_nb -= WFLD_FIRST;
    if(fld_nb < 0 || fld_nb >= pg->pg_nb_flds) return((HBRUSH)0);
    if(pg->pg_flds[fld_nb].fld_nb_line > 1) return((HBRUSH)0);
    /*    if(pg->pg_flds[fld_nb].fld_compulsory + SCR_NOT_COMPULSORY == SCR_REQUIRED)
    	attr = SCR_RED - 1;
        else
    /* JMP 09-05-00 */
    attr = pg->pg_flds[fld_nb].fld_attributes - 1;
    if(attr < 0 || attr > 16) return((HBRUSH)0);

    SetBkMode(hDC, TRANSPARENT);
//    SetBkColor(hDC, RGB(128,128,128));
    color = WSCR_COLORS[attr][1];
    hBrush = WSCR_BRUSHES[attr];
//  hBrush = 0;
    SetTextColor(hDC, color);
    return(hBrush);
}

BOOL CALLBACK WscrDlgProc(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
    static int  nb = 0;
    HD_NOTIFY   *hdn;
    HD_ITEM     hdi;
    HWND        hWndEdit;
    NMHDR       *nmh = (NMHDR *)lParam;
    WORD        wNotify  = HIWORD(wParam),
                wId      = LOWORD(wParam);
    PAGE        *pg;
    int         fld_nb;
    HBRUSH      hBrush;

    switch(wMsg) {
    case WM_HELP:
        if(WscrIsShift() || WscrIsCtrl()) return(TRUE); /* JMP 23-08-97 */
        if(WSCR_PGF1DEF) WSCR_PGF1DEF = 0; /* JMP 23-08-97 */
        else             HLP_stack_edit(); /* JMP 23-08-97 */
        return(TRUE);

    case WM_COMMAND :
#ifdef WIN32
        WscrDlgCommand(hWnd, LOWORD(wParam), (HWND)(lParam), HIWORD(wParam));
#else
        WscrDlgCommand(hWnd, wParam, (HWND)LOWORD(lParam), HIWORD(lParam));
#endif
        return(TRUE);
    case WM_SYSCOMMAND :
        return(WscrDlgSysCommand(hWnd, wParam, LOWORD(lParam), HIWORD(lParam)));

    case WM_INITDIALOG :
        WscrDlgInit(hWnd, (PAGE *)lParam, 0);
        return(TRUE);

    case WM_SIZE :
        WscrResizeCtrl(hWnd, wParam, lParam);
        break;

    case WM_NOTIFY :
        return(WscrToolbarNotify(hWnd, wParam, lParam));

        /*            if(nmh->code == HDN_ITEMCHANGED) {
        		hdn = (HD_NOTIFY *)lParam;
        		hdi.mask = HDI_WIDTH;
        		hWndEdit = GetDlgItem(hWnd, 10000);
        		Header_GetItem(hWndEdit, hdn->iItem, &hdi);
        		PG_display_error("Endtrack item%d;size=%d", hdn->iItem, hdi.cxy);
        		}
        	    if(nmh->code == HDN_ITEMCLICK) {
        		hdn = (HD_NOTIFY *)lParam;
        		PG_display_error("Clicked item%d", hdn->iItem);
        		}
        	    break;
        */

    case WM_MENUSELECT:
        WscrMenuSelect(hWnd, (HMENU) lParam, wId, wNotify);
        break;

    case WM_KEYDOWN :
    case WM_SYSKEYDOWN :
        return(WscrProcKeydown(hWnd, wParam));

    case WM_MOVE :
        pg = WscrFindPage(hWnd);
        if(pg && WSCR_AUTOMOVE == 0) {
            pg->pg_w_x = LOWORD(lParam);
            pg->pg_w_y = HIWORD(lParam);
        }
        break;

    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
//      case WM_CTLCOLORBTN:
//      case WM_CTLCOLORLISTBOX:
        pg = WscrFindPage(hWnd);
        if(pg == 0) break;
        fld_nb = GetDlgCtrlID((HWND) lParam);
        hBrush =  WscrDlgColor(pg, fld_nb, (HDC)wParam);
        if(hBrush == 0) break;
        return((BOOL)hBrush);

    default :
        break;
    }

    return(FALSE);
}

WscrEditDirFld(hWnd, pg, fld_nb)
HWND    hWnd;
PAGE     *pg;
int     fld_nb;
{
    char    filename[256];
    FIELD    *fld = pg->pg_flds + fld_nb;

    PG_get_rec_txt(pg, fld_nb, filename, 0);
    /*
        if(fld->fld_dtext == 0 || fld->fld_dtext[0] == 0)
    	strcpy(buf, "All Files|*.*|");
        else {
    	tbl = (char **)SCR_vtom(fld->fld_dtext, '|');
    	buf[0] = 0;
    	for(i = 0 ; tbl[i] ; i++)
    	    sprintf(buf + strlen(buf), "(%s)|%s|", tbl[i], tbl[i]);
    	SCR_free_tbl(tbl);
    	}
    */
    if(WscrGetFilename(hWnd, 1, "Select file", filename,
                       filename, 0L) == 0) {
        PG_set_rec_txt(pg, fld_nb, filename);
        return(0);
    }

    return(-1);
}


WpgEnableFlds(HWND hWnd, PAGE *apg, int iScroll, BOOL status)
{
    HWND    hWndCtrl;
    int     i, sta, style;

    for(i = 0 ; i < apg->pg_nb_flds ; i++) {
        hWndCtrl = GetDlgItem(hWnd, WFLD_FIRST + i);
        if(hWndCtrl == 0) continue;
        sta = status;
        if(apg->pg_flds[i].fld_io == SCR_OUTPUT - SCR_INPUT) sta = 0;
        if(sta) {
            SendMessage(hWndCtrl, EM_SETREADONLY, FALSE, 0);
            /*
            style = GetWindowLongPtr(hWndCtrl, GWL_STYLE);
            style = style & WS_TABSTOP;
            SetWindowLongPtr(hWndCtrl, GWL_STYLE, style);
            */
        }

        EnableWindow(hWndCtrl, sta);
        if(apg->pg_flds[i].fld_type == SCR_DIR) {
            hWndCtrl = GetDlgItem(hWnd, WFLD_DIRBUTTON + i);
            EnableWindow(hWndCtrl, sta);
        }
    }

    if(iScroll == 0) return(0);

    hWndCtrl = GetDlgItem(hWnd, iScroll);
    EnableWindow(hWndCtrl, !status);

    if(status == FALSE) SetFocus(hWndCtrl);
    else                SetFocus(hWnd);

    return(0);
}

/*
WpgEnableFlds(HWND hWnd, PAGE *apg, int iScroll, BOOL status)
{
    HWND    hWndNext;

    hWndNext = GetWindow(hWnd, GW_CHILD | GW_HWNDFIRST);
    while(hWndNext != NULL) {
	if(GetDlgCtrlID(hWndNext) > 0)
	    EnableWindow(hWndNext, status);
	hWndNext = GetWindow(hWndNext, GW_HWNDNEXT);
	}

    if(iScroll) {
	hWndNext = GetDlgItem(hWnd, iScroll);
	EnableWindow(hWndNext, !status);
	if(status == FALSE) SetFocus(hWndNext);
	}

    if(status == TRUE) SetFocus(hWnd);
}
*/






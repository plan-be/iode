#include "scr4w.h"

/* ==================== PAGE vs DIALOG ================== */

U_ch *WpgAddAlt(U_ch *buf, int qkey)
{
    U_ch    *res;
    int     i, j, done = 0;

    if(qkey == 0 || buf == 0) return(buf);

    qkey += 'A' - 1;
    res = SCR_malloc(strlen(buf) + 3);
    for(i = j = 0 ; buf[i] ; i++) {
	if(!done && (buf[i] == qkey || buf[i] == qkey + 'a' - 'A')) {
	    res[j++] = '&';
	    done = 1;
	    }
	res[j++] = buf[i];
	}
    res[j] = 0;
    SCR_free(buf);
    return(res);
}
WpgFld2Dlg(hWnd, pg, fld_nb)
HWND    hWnd;
PAGE    *pg;
int     fld_nb;
{
    FIELD   *fld;
    char    *ptr, **pptr, *tmp, buf[80];
    HWND    hWndEdit;
    WORD    id = fld_nb + WFLD_FIRST;
    extern PAGE *WSCR_PG_NOACTS;
//    U_ch    *WpgAddAlt();

/*    PG_read_flds(pg); */

    if(fld_nb < 0 || fld_nb >= pg->pg_nb_flds) return(0);
    fld = pg->pg_flds + fld_nb;

    if(fld->fld_type == SCR_HIDDEN - SCR_INPUT) return(0); /* JMP 12-09-97 */
    PG_set_dft_field(pg, fld_nb);

    /* Execute the disp & begin functions */
    if(WSCR_PG_NOACTS != pg) ACT_exec_nu(fld->fld_disp_act);
    if(WSCR_PG_NOACTS != pg && SCR_EXEC_BEG_IN_DISP == 1) ACT_exec_nu(fld->fld_begin_act);

    switch(fld->fld_type) {
	case SCR_TYPE_RADIO :
	    CheckDlgButton(hWnd, id, WscrIsFldRadioChecked(pg, fld_nb));
	    break;

	case SCR_CHECK_BOX :
	    CheckDlgButton(hWnd, id, SCR_get_field_short(pg, fld_nb));
	    break;

	case SCR_DATE   :
	case SCR_TIME   :
	case SCR_NATURAL:
	case SCR_INTEGER:
	case SCR_REAL:
	    PG_get_rec_txt(pg, fld_nb, buf, 1);
	    U_ljust_text(buf);
	    if(!fld->fld_fmt) SCR_strip(buf); /* JMP 14-05-00 */
	    SetDlgItemText(hWnd, id, buf);
	    break;
	case SCR_STRING :
	case SCR_DIR    :
	case SCR_PASSWD :
	    ptr = PG_aget_rec_txt(pg, fld_nb);
	    tmp = WscrOemToAnsi(ptr);
	    if(fld->fld_io + SCR_INPUT == SCR_ACCESSIBLE) {
		U_ljust_text(tmp);
		if(fld->fld_qkey) tmp = WpgAddAlt(tmp, fld->fld_qkey);
		}
	    if(!fld->fld_fmt) SCR_strip(tmp); /* JMP 14-05-00 */
	    SetDlgItemText(hWnd, id, tmp);
	    SCR_free(ptr);
	    SCR_free(tmp);
	    break;
	case SCR_EDITOR :
	    pptr = (char **)PG_get_text(pg, fld_nb);
	    ptr = *pptr;
	    if(ptr == 0) SetDlgItemText(hWnd, id, "");
	    else {
		tmp = WscrOemToAnsi(ptr);
		SCR_strip(tmp);
		SetDlgItemText(hWnd, id, tmp);
		SCR_free(tmp);
		}
	    break;
	case SCR_TYPE_MENU   :
	    ptr = PG_get_text(pg, fld_nb);
	    SendDlgItemMessage(hWnd, id, CB_SETCURSEL, *(short *)ptr, 0L);
	    break;
	default         : return(0);
	}

    if(WscrFindStatus(hWnd) == 0) { /*  Get IO from fld (standard) */
	hWndEdit = GetDlgItem(hWnd, id);
	EnableWindow(hWndEdit, (fld->fld_io == SCR_OUTPUT-SCR_INPUT) ? FALSE : TRUE);
	}

    SCR_wdispatch(); /* JMP 08-05-00 */
    return(0);
}


WpgDlg2Fld(hWnd, pg, fld_nb)
HWND    hWnd;
PAGE     *pg;
int     fld_nb;
{
    FIELD   *fld;
    WORD    id = fld_nb + WFLD_FIRST;
    char    *ptr, **pptr;
    int     lg;
    HWND    hWndEdit;

/*    PG_read_flds(pg); */
    if(fld_nb < 0 || fld_nb >= pg->pg_nb_flds) return(0);
    fld = pg->pg_flds + fld_nb;
    if(fld->fld_type == SCR_HIDDEN - SCR_INPUT) return(0); /* JMP 12-09-97 */

    hWndEdit = GetDlgItem(hWnd, id);
    if(hWndEdit == 0) return(0);

    switch(fld->fld_type) {
	case SCR_CHECK_BOX :
	    SCR_set_field_short(pg, fld_nb, IsDlgButtonChecked(hWnd, id));
	    break;

	case SCR_TYPE_RADIO :
	    WscrDlg2FldRadio(pg, fld_nb, IsDlgButtonChecked(hWnd, id));
	    break;

	case SCR_INTEGER:
	case SCR_NATURAL:
	case SCR_REAL   :
	case SCR_DATE   :
	case SCR_TIME   :
	case SCR_STRING :
	case SCR_DIR    :
	case SCR_PASSWD :
	    if(fld->fld_io + SCR_INPUT == SCR_ACCESSIBLE) break; /* JMP 29-04-97 */
	    ptr = WscrGetWindowTextOem(hWndEdit);
	    if(ptr == 0) {
		PG_set_rec_txt(pg, fld_nb, "");
		break;
		}
	    PG_set_rec_txt(pg, fld_nb, ptr);
	    SCR_free(ptr);
	    break;

	case SCR_EDITOR :
	    ptr = WscrGetWindowTextOem(hWndEdit);
	    if(ptr == 0) ptr = SCR_stracpy("");
	    PG_set_rec_txt(pg, fld_nb, ptr);
	    break;

	case SCR_TYPE_MENU   :
	    ptr = PG_get_text(pg, fld_nb);
	    *(short *)ptr =
		SendDlgItemMessage(hWnd, id, CB_GETCURSEL, 0, 0L);
	    break;
	default : break;
	}
    PG_set_dft_field(pg, fld_nb);
    return(0);
}

Wpg2Dlg(hWnd, pg)
HWND    hWnd;
PAGE     *pg;
{
    int     i;

    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	WpgFld2Dlg(hWnd, pg, i);
    return(0);
}

WpgDlg2Pg(hWnd, pg)
HWND    hWnd;
PAGE     *pg;
{
    int     i;

    for(i = 0 ; i < pg->pg_nb_flds ; i++)
	WpgDlg2Fld(hWnd, pg, i);

    return(0);
}



/* =============== RADIO BUTTONS ===================== */

WscrCalcRadioNb(pg, fld_nb)
PAGE    *pg;
int     fld_nb;
{
    FIELD   *fld = pg->pg_flds + fld_nb;
    int     i, group, fradio;

    if(!fld->fld_radio) return(-1);
    if(fld->fld_radio < 128) return(0);
    group = fld->fld_radio - 127;
    for(i = fld_nb - 1 ; i >= 0 ; i--) {
	fradio = pg->pg_flds[i].fld_radio;
	if(fradio < 128) {
	    if(fradio != group) break;
	    }
	else if(fradio - 127 != group) break;
	}

    return(fld_nb - i - 1);
}

WscrIsFldRadioChecked(pg, nfld)
PAGE    *pg;
int     nfld;
{
    int     radionb, v;

    radionb = WscrCalcRadioNb(pg, nfld);
    if(radionb < 0) return(0);
    v = SCR_get_field_short(pg, nfld);
    return(radionb == v);
}

WscrDlg2FldRadio(pg, fld_nb, v)
PAGE    *pg;
int     fld_nb, v;
{
    int     radionb;

    if(v == 0) return(0);
    radionb = WscrCalcRadioNb(pg, fld_nb);
    if(radionb < 0) return(0);
    SCR_set_field_short(pg, fld_nb, radionb);
    return(0);
}



#include "scr4w.h"

HMENU WapCreateMenu(MENU *mn, int owner)
{
    int     i, attr;
    HMENU   hMenu, hSubMenu;
    char    buf[80];
    OPTION  *opt;
    MENU    *submn;

    MN_read_opt(mn);
    hMenu = CreatePopupMenu();
    for(i = 0, opt = mn->mn_opts ; i < mn->mn_nb_opts; i++, opt++) {
	if(SCR_check_prot_vt(opt->opt_prot)) continue; /* JMP 04-06-01 */
	WapTransOptText(buf, opt->opt_text, opt->opt_key, '&');
	submn = WapFindActMenu(opt->opt_act);
	attr = MF_STRING;
	if(i && i % 20 == 0) attr |= MF_MENUBARBREAK; /* JMP 04-05-97 */
	if(opt->opt_enable == 0) attr |= MF_GRAYED;
	if(opt->opt_text[0] == 0304 || opt->opt_text[0] == 0315) {
	    AppendMenu(hMenu, MF_SEPARATOR, 0, 0L);
	    }
	else if(submn == 0) {
	    if(opt->opt_act) AppendMenu(hMenu, attr, opt->opt_act + WACT_SHIFT, buf);
	    else             AppendMenu(hMenu, attr, 20000 + i, buf);
	    }
	else {
	    hSubMenu = WapCreateMenu(submn, owner);
	    AppendMenu(hMenu, attr | MF_POPUP, (UINT)hSubMenu, buf);
	    }
	}

    MN_free_opt(mn);

    i = WscrRegisterObj((char *)mn, (PAGE *)0, ACT_MN_EDIT, hMenu, 0L);
    WSCR_HWNDS[i].sh_status = owner;

    return(hMenu);
}

HMENU WapCreateApplMenu(APPL *ap)
{
    HMENU   hMenu, hSubMenu;
    int     i, objnb;
    A_BAR   *ab = (A_BAR *)SCR_PTR(ap->ap_ab);
    A_OPT   *ao;
    ACTION  *act;
    MENU    *mn;
    char    buf[120];

    if(ab == 0) return(0); /* JMP 13-04-00 */
//    WapCreateAccelerator(ap);
    hMenu = CreateMenu();
    objnb = WscrRegisterObj((char *)ap, (PAGE *)0, ACT_AB_EDIT, hMenu, 0L);

    for(i = 0, ao = ab->ab_opts ; i < ab->ab_nb_opts ; i++, ao++) {
	if(ACT_exec_nu(ao->ao_act) != 0) continue;
	if(SCR_check_prot_vt(ao->ao_prot)) continue; /* JMP 04-06-01 */
	WapTransOptText(buf, ao->ao_text, ao->ao_key, '&');
	mn = (MENU *) SCR_PTR(ao->ao_ptr);
	switch(ao->ao_type) {
	    case SCR_TYPE_ACT:
		mn = WapFindActMenu(ao->ao_ptr);
		if(mn == 0) {
		    AppendMenu(hMenu, MF_STRING, ao->ao_ptr + WACT_SHIFT, buf);
		    break;
		    }
		/* NO BREAK */
	    case SCR_TYPE_MENU:
		hSubMenu = WapCreateMenu(mn, objnb);
		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, buf);
/*                ModifyMenu(hMenu, i, MF_BYPOSITION | MF_POPUP, hSubMenu, buf); */
		break;

	    default : break;
	    }
//        if(ACT_exec_nu(ao->ao_act) != 0)            /* BP_M 02-07-1997 */
//            EnableMenuItem(hMenu, i, MF_BYPOSITION | MF_GRAYED);
	}

    return(hMenu);
}
/*
WapCreateAccelerator(APPL *ap)
{
    FNKEYS          *fnks = SCR_PTR(ap->ap_fnks);
    ACCEL           *acs;
    int             i;
    extern HACCEL   hAccel;

    if(fnks == 0) return;
    if(hAccel) DestroyAcceleratorTable(hAccel);
    acs = (ACCEL *)SCR_malloc(sizeof(ACCEL) * fnks->fk_nb_fnks);
    for(i = 0 ; i < fnks->fk_nb_fnks ; i++) {
	acs[i].fVirt = FVIRTKEY;
	acs[i].key   = VK_F5 + i;//WscrKeyScrToWin(fnks->fk_fnks[i].fnk_key);
	acs[i].cmd   = i; //WACT_SHIFT + fnks->fk_fnks[i].fnk_act;
	}
    hAccel = CreateAcceleratorTable(acs, fnks->fk_nb_fnks);
}
*/
HWND WapEditAppl(APPL *ap)
{
    PAGE    *pg;
    BC      *bc;
    HMENU   hMenu;
    char    *ptr = SCR_PTR(ap->ap_ptr);
    HWND    hWnd = 0;
    int     objnb;

    if(ACT_exec_nu(ap->ap_begin_act)) return(0);  /* JMP 23-10-97 */
    hMenu = WapCreateApplMenu(ap);


//    keybd_event(VK_MENU, 0, 0, 0);                  /* JMP 07-11-99 */
//    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);    /* JMP 07-11-99 */
//    keybd_event(VK_DOWN, 0, 0, 0);                  /* JMP 07-11-99 */
//    keybd_event(VK_DOWN, 0, KEYEVENTF_KEYUP, 0);    /* JMP 07-11-99 */

    if(ap->ap_ptr != 0) { /* APPL AVEC PAGE OU BC */
	if(ap->ap_type == SCR_TYPE_PAGE) {
	    pg = (PAGE *) ptr;
	    objnb = WscrRegisterObj((char *)ap, ptr, ACT_APPL_RUN, hMenu, 0L);
	 //   hWnd = WscrObjDisplay(pg, objnb, !ap->ap_enable);
	 //   WSCR_HWNDS[objnb].sh_pgedit = 0;
	    WSCR_HWNDS[objnb].sh_status = !ap->ap_enable; /* Disable Input fields */
	    WpgEditPage((PAGE *)ptr);
	    WscrUnRegisterObj((char *)ap);
	    }
	if(ap->ap_type == SCR_TYPE_BC) {
	    bc = (BC *)ptr;
	    pg = (PAGE *) SCR_PTR(bc->bc_apg);
	    objnb = WscrRegisterObj((char *)ap, pg, ACT_APPL_RUN, hMenu, 0L);
	    WSCR_HWNDS[objnb].sh_status = !ap->ap_enable; /* Disable Input fields */
	    WpgEditPage(pg);
	    WscrUnRegisterObj((char *)ap);
	    pg->pg_im = 0;
	    }
	}

    else {  /* APPLICATION SIMPLE */
	objnb = WscrRegisterObj((char *)ap, ptr, ACT_APPL_RUN, hMenu, 0L);
	hWnd = WscrCreateWscr(hMenu);
	WscrRegisterHwnd((char *)ap, hWnd);
	if(ap->ap_nb_tb)
	    WscrCreateToolbar(hWnd, (WTOOLBAR *)SCR_PTR(ap->ap_toolb[0]));

     /*   WscrShrinkWindow(hWnd);
	WscrMoveWindow(hWnd, 0, 10001, 10001);
	WscrMoveAnimateWindow(hWnd, 0, 10001, 10000);
    */
	return(hWnd);
	}

    if(ACT_exec_nu(ap->ap_end_act)) return(0);  /* BP_M 07-12-1999 */

    return(hWnd);
}

WapResetMenu(APPL *ap)
{
    HMENU   hMenu, hMenu2;
    HWND    hWnd;
    int     objnb;
    WSCRHWND wh;

    objnb = WscrFindPtrNb((char *)ap);
    if(objnb < 0) return;
    memcpy(&wh, WSCR_HWNDS + objnb, sizeof(wh));
    hMenu = WSCR_HWNDS[objnb].sh_hmn;
    hWnd  = WSCR_HWNDS[objnb].sh_hwnd;
    WscrUnRegisterObj((char *)ap);

    hMenu2 = WapCreateApplMenu(ap);
    objnb  = WscrFindPtrNb((char *)ap);
    if(objnb < 0) return;

    wh.sh_hmn  = hMenu2;
    memcpy(WSCR_HWNDS + objnb, &wh, sizeof(wh));
    SetMenu(hWnd, hMenu2);

    if(hMenu) DestroyMenu(hMenu);
}

WscrTrackMenu(HWND hWnd, U_ch **txt, int x, int y)
{
    int     i, rc;
    HMENU   hMenu;
    U_ch    **tbl;

    hMenu = CreatePopupMenu();
    tbl = SCR_vtom(txt, ';');
    for(i = 0 ; tbl[i] ; i++) {
	if(tbl[i][0] == 0304 || tbl[i][0] == 0315) {
	    AppendMenu(hMenu, MF_SEPARATOR, 0, 0L);
	    }
	else {
	    WapTransOptText(tbl[i], tbl[i], 0, '&');
      //      SCR_OemToAnsi(tbl[i], tbl[i]);
	    AppendMenu(hMenu, MF_STRING, i + 1, tbl[i]);
	    }
	}

    SCR_free_tbl(tbl);

    if(hWnd == 0) hWnd = hMainWnd;
    rc = TrackPopupMenu(hMenu, TPM_RETURNCMD, x, y, 0, hWnd, NULL);
    DestroyMenu(hMenu);
    return(rc);
}




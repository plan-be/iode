#include "scr4w.h"

WSCRHWND    *WSCR_HWNDS = 0;
int         WSCR_NB_HWNDS = 0, WSCR_MAX_HWNDS = 0;;

WscrRegisterObj(
char    *ptr,
PAGE    *pg,
int     act,
HMENU   hMenu,
int     (*fn)(HWND, PAGE *, int)
)
{
    if(WSCR_NB_HWNDS >= WSCR_MAX_HWNDS) {
	WSCR_HWNDS = (WSCRHWND *) SCR_realloc(WSCR_HWNDS, sizeof(WSCRHWND), WSCR_MAX_HWNDS, WSCR_MAX_HWNDS + 10);
	WSCR_MAX_HWNDS += 10;
	}
    WSCR_HWNDS[WSCR_NB_HWNDS].sh_ptr   = ptr;
    WSCR_HWNDS[WSCR_NB_HWNDS].sh_pg    = pg;
    WSCR_HWNDS[WSCR_NB_HWNDS].sh_act   = act;
    WSCR_HWNDS[WSCR_NB_HWNDS].sh_hwnd  = 0;
    WSCR_HWNDS[WSCR_NB_HWNDS].sh_hmn   = hMenu;
    WSCR_HWNDS[WSCR_NB_HWNDS].sh_fn    = fn;
    WSCR_HWNDS[WSCR_NB_HWNDS].sh_status= 0;
    WSCR_NB_HWNDS++;
    return(WSCR_NB_HWNDS - 1);
}

WscrRegisterHwnd(char *ptr, HWND hWnd)
{
    int     i;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--)
	if(WSCR_HWNDS[i].sh_ptr == ptr && WSCR_HWNDS[i].sh_hwnd == 0) {
	    WSCR_HWNDS[i].sh_hwnd = hWnd;
	    return(i);
	    }

    return(-1);
}

HWND WscrFindHwnd(char *ptr)
{
    int     i;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--)
	if(WSCR_HWNDS[i].sh_ptr == ptr)
	    return(WSCR_HWNDS[i].sh_hwnd);

    return(0);
}

HWND WscrFindHwndPg(PAGE *pg)
{
    int     i;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--)
	if(WSCR_HWNDS[i].sh_pg == pg)
	    return(WSCR_HWNDS[i].sh_hwnd);

    return(0);
}

WscrRegisterPgHwnd(PAGE *pg, HWND hWnd)
{
    int     i;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--)
	if(WSCR_HWNDS[i].sh_pg == pg && WSCR_HWNDS[i].sh_hwnd == 0) {
	    WSCR_HWNDS[i].sh_hwnd = hWnd;
	    return(i);
	    }

    return(-1);
}

int WscrUnRegisterObjPg(char *ptr, PAGE *pg)
{
    int     i;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--) {
	if(WSCR_HWNDS[i].sh_ptr == ptr) {
	    if(pg == 0 || pg == WSCR_HWNDS[i].sh_pg) {
		if(WSCR_HWNDS[i].sh_hmn) {
		 //   DestroyMenu(WSCR_HWNDS[i].sh_hmn);
		    WscrUnRegisterSubMn(WSCR_HWNDS[i].sh_hmn);
		    }
		memset(WSCR_HWNDS + i, 0, sizeof(WSCRHWND));
		break;
		}
	    }
	}

    WRegCleanup();
    return(i);
}

int WscrUnRegisterObj(char *ptr)
{
    return(WscrUnRegisterObjPg(ptr, (PAGE *)0));
}

WscrUnRegisterMenu(HMENU hMenu)
{
    int     i;

    i = WscrFindApMn(hMenu, ACT_MN_EDIT);
    if(i < 0) return(0);
    memset(WSCR_HWNDS + i, 0, sizeof(WSCRHWND));
    WRegCleanup();
    return(0);
}

WscrUnRegisterSubMn(HMENU hMenu)
{
    int     i, objnb;

    objnb = WscrFindApMn(hMenu, ACT_AB_EDIT);
    if(objnb < 0) return(0);
    memset(WSCR_HWNDS + objnb, 0, sizeof(WSCRHWND));
    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--)
	if(WSCR_HWNDS[i].sh_status == objnb &&
	   WSCR_HWNDS[i].sh_act == ACT_MN_EDIT)
		memset(WSCR_HWNDS + i, 0, sizeof(WSCRHWND));


    WRegCleanup();
    return(0);
}

WscrFindObjNb(HWND hWnd)
{
    int     i;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--)
	if(WSCR_HWNDS[i].sh_hwnd == hWnd) return(i);

    return(-1);
}

WscrFindPgNb(PAGE *pg)
{
    int     i;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--)
	if(WSCR_HWNDS[i].sh_pg == pg) return(i);

    return(-1);
}

WscrFindPtrNb(char *ptr)
{
    int     i;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--)
	if(WSCR_HWNDS[i].sh_ptr == ptr) return(i);

    return(-1);
}

WscrFindMnNb(HMENU hmn)
{
    int     i;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--)
	if(WSCR_HWNDS[i].sh_hmn == hmn) return(i);

    return(-1);
}

WscrFindBcApplNb(BC *bc)
{
    int     i;
    APPL    *ap;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--) {
	if(WSCR_HWNDS[i].sh_act == ACT_APPL_RUN) {
	    ap = (APPL *)WSCR_HWNDS[i].sh_ptr;
	    if(ap->ap_type != SCR_TYPE_BC) continue;
	    if(bc == (BC *)SCR_PTR(ap->ap_ptr)) return(i);
	    }
	else if(WSCR_HWNDS[i].sh_act == ACT_BC_EDIT_ISAM) {
	    if(bc == (BC *)WSCR_HWNDS[i].sh_ptr) return(i);
	    }
	}

    return(-1);
}

PAGE *WscrFindPage(HWND hWnd)
{
    int     objnb;

    objnb = WscrFindObjNb(hWnd);
    if(objnb < 0) return((PAGE *)0);
    return(WSCR_HWNDS[objnb].sh_pg);
}

char *WscrFindObjPtr(HWND hWnd)
{
    int     objnb;

    objnb = WscrFindObjNb(hWnd);
    if(objnb < 0) return((char *)0);
    return(WSCR_HWNDS[objnb].sh_ptr);
}

WscrFindStatus(HWND hWnd)
{
    int     objnb;

    objnb = WscrFindObjNb(hWnd);
    if(objnb < 0) return(0);
    return(WSCR_HWNDS[objnb].sh_status);
}


WscrFindApMn(HMENU hMenu, int act)
{
    int     i;

    for(i = WSCR_NB_HWNDS - 1 ; i >= 0 ; i--)
	if(WSCR_HWNDS[i].sh_hmn == hMenu &&
	   WSCR_HWNDS[i].sh_act == act) break;

    return(i);
}

MENU *WscrFindMenu(HMENU hMenu)
{
    int     i;

    i = WscrFindApMn(hMenu, ACT_MN_EDIT);
    if(i < 0) return((MENU *)0);
    return((MENU *)WSCR_HWNDS[i].sh_ptr);
}

APPL *WscrFindAppl(HMENU hMenu)
{
    int     i;

    i = WscrFindApMn(hMenu, ACT_AB_EDIT);
    if(i < 0) return((APPL *)0);
    return((APPL *)WSCR_HWNDS[i].sh_ptr);
}


WRegCleanup()
{
    for( ; WSCR_NB_HWNDS > 0 ; WSCR_NB_HWNDS--)
	if(WSCR_HWNDS[WSCR_NB_HWNDS - 1].sh_ptr != 0) break;

    return(0);
}

WscrChangeMenu(char *ptr, HMENU hMenu)
{
    int     objnb;

    objnb = WscrFindPtrNb(ptr);

    WSCR_HWNDS[objnb].sh_hmn = hMenu;
}

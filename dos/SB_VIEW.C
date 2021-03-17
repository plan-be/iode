#include <iodedos.h>

SB_ViewByTbl()
{
    PG_edit_window(vkp_viewbytbl);
    return(0);
}

C_ViewByTbl()
{
    int     rc;
    char    *arg;

    arg = SW_nalloc(strlen(vkp_viewbytbl_TABS) + 20);
    sprintf(arg, "%s:%d %s", PER_pertoa(&(KSMPL(KV_WS)->s_p1), NULL), KSMPL(KV_WS)->s_nb, vkp_viewbytbl_TABS);
    rc = B_ViewByTbl(arg);
    SW_nfree(arg);
    return(rc);
}

SB_ViewPrintTbl()
{
    PG_edit_window(vkp_viewtbl);
    return(0);
}

SB_ViewPrintGr()
{
    PG_edit_window(vkp_viewgr);
    return(0);
}

SB_ViewPrintVar()
{
    PG_edit_window(vkp_viewvar);
    return(0);
}

char *C_ViewTblArgs(smpl, tbls)
char    *smpl, *tbls;
{
    char    *buf = 0;

    if(tbls == NULL || smpl == NULL) return(buf);
    buf = SW_nalloc(strlen(tbls) + strlen(smpl) + 10);
    if(buf == NULL) return(buf);

    sprintf(buf, "%s %s", smpl, tbls);
    return(buf);
}

C_ViewTblFiles(f2, f3, f4, f5)
char    *f2, *f3, *f4, *f5;
{
	char    buf[2 * K_MAX_FILE], *fs[5]; // JMP 26/08/2015
    int     i;

    fs[0] = f2;
    fs[1] = f3;
    fs[2] = f4;
    fs[3] = f5;

    for(i = 0 ; i < 4 ; i++) {
	sprintf(buf, "%d %s", i + 2, fs[i]);
	if(B_ViewTblFile(buf) < 0) return(-1);
	}
    return(0);
}



C_ViewPrintTbl(dest)
int     dest;
{
    extern int  B_ViewTbl(), B_PrintTbl();

    global_VTN = vkp_viewtbl_NDEC;
    return(C_ViewPrintGrTblVar(dest, /* GB 08-97 */
			    (int)vkp_viewtbl_NDEC, vkp_viewtbl_LANG,
			    vkp_viewtbl_SMPL, vkp_viewtbl_TABS,
			    vkp_viewtbl_F2,
			    vkp_viewtbl_F3,
			    vkp_viewtbl_F4,
			    vkp_viewtbl_F5,
			    0, B_ViewTbl, B_PrintTbl));
}

C_ViewPrintVar(dest)
int     dest;
{
    extern int  B_ViewVar(), B_PrintVar();

    global_VTN = vkp_viewvar_NDEC;
    return(C_ViewPrintGrTblVar(dest, /* GB 08-97 */
			    (int)vkp_viewvar_NDEC, vkp_viewvar_LANG,
			    vkp_viewvar_SMPL, vkp_viewvar_VARS,
			    vkp_viewvar_F2,
			    vkp_viewvar_F3,
			    vkp_viewvar_F4,
			    vkp_viewvar_F5,
			    0, B_ViewVar, B_PrintVar));
}

C_ViewPrintGr(dest)
int     dest;
{
    extern int  B_ViewGr(), B_PrintGr();

    return(C_ViewPrintGrTblVar(dest,               /* GB 08-97 */
			    -1, vkp_viewgr_LANG,   /* JMP 14-07-96 */
			    vkp_viewgr_SMPL, vkp_viewgr_TABS,
			    vkp_viewgr_F2,
			    vkp_viewgr_F3,
			    vkp_viewgr_F4,
			    vkp_viewgr_F5,
			    0, B_ViewGr, B_PrintGr));
}

C_ViewPrintGrTblVar(dest, ndec, lang,  smpl, tabs,  /* GB 08-97 */
		    f2, f3, f4, f5, what, viewfn, printfn)
int     dest, ndec, lang, what, (*viewfn)(), (*printfn)();
char    *smpl, *tabs, *f2, *f3, *f4, *f5;
{
    char    *args;
    int     rc = 0, view;
    extern char *C_ViewTblArgs();
/*  extern int B_what; */

    if(dest == 0) view = 1;
    else view = 0;

    if(C_PrintDestDecLang(view, ndec, lang)) return(-1); /* GB 08-97 */

    args = C_ViewTblArgs(smpl, tabs);
    if(args == NULL) return(-1);

    rc = C_ViewTblFiles(f2, f3, f4, f5);
/*    B_what = what; */
    if(rc == 0) {
	if(dest == 0) rc = (*viewfn)(args);
	else  {
	    rc = (*printfn)(args);
	    W_close();
	}
    }

    if(rc) B_display_last_error();
    else if(dest) ODE_error(OM_TBLS_PRINTED);

err:
    B_ViewTblEnd();
    SW_nfree(args);
    return(rc);
}










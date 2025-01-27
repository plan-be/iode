// copied/pasted from the /cli directory

/************ Fonctions annul�es ou a remplacees ************************/
#include "iode.h"
#include <s_a2m.h>

/* sb_pdest.c */
double   A2M_GWIDTH = 9.5;
double   A2M_GHEIGHT = 6.5;
int      A2M_BOXWIDTH = 1;
int      A2M_BACKBRUSH = 50;
int      A2M_BACKCOLOR = 'b';

void B_A2mGetGnlParms()
{
}

void B_A2mGetRtfParms()
{
    B_A2mGetGnlParms();
}

void B_A2mGetCsvParms()
{
    B_A2mGetGnlParms();
}

void B_A2mGetHtmlParms()
{
    B_A2mGetGnlParms();
}

void B_A2mGetGdiParms()
{

    B_A2mGetGnlParms();
}

void B_A2mGetMifParms()
{
    B_A2mGetGnlParms();
}

void B_A2mGetGIFParms()
{
}


/* o_gr.c */
o_estgr(titles, smpl, mlhs, mrhs, view, res)
char** titles;
SAMPLE* smpl;
MAT* mlhs, * mrhs;
int     view, res;
{
}

/* sb_ws.c */
SAMPLE* K_ask_smpl()
{
    SAMPLE* smpl = KSMPL(K_WS[VARIABLES]);
    return(smpl);
}

/* b_gui.c */
B_DataEditGlobal(VM, VN)
int VM, VN;
{
}


/* ode.c */
char* L_expand(name)
char* name;
{
    return(A_expand(name));
}
A_error(txt)
char* txt;
{
    B_seterror("%.80s", txt);
}

char* A_expand(name)
char* name;
{
    int     pos;

    pos = K_find(KL_WS, name);
    if (pos < 0) return(NULL);
    return((char*)KLVAL(KL_WS, pos));
}


/* b_rep.c */
extern int  SB_FileDelete() { return(0); }
extern int  SB_FileRename() { return(0); }
extern int  SB_FileCopy() { return(0); }
extern int  SB_FileEdit() { return(0); }
extern int  SB_FileList() { return(0); }
extern int  SB_FileImport() { return(0); }
extern int  SB_ReportExec() { return(0); }
extern int  SB_ReportEdit() { return(0); }
extern int  SB_ReportPrompt() { return(0); }
extern int  SB_PrintObjDef() { return(0); }
extern int  SB_PrintCnf() { return(0); }
extern int  SB_ViewPrintGr() { return(0); }
extern int  SB_ViewPrintTbl() { return(0); }
extern int  SB_ViewByTbl() { return(0); }
extern int  SB_DataEdit() { return(0); }
extern int  SB_DataEditScroll() { return(0); }
extern int  SB_DataSearch() { return(0); }
extern int  SB_DataDuplicate() { return(0); }
extern int  SB_DataList() { return(0); }
extern int  SB_DataCompare() { return(0); }
extern int  SB_DataCalcLst() { return(0); }
extern int  SB_DataListSort() { return(0); }
extern int  SB_DataEditGraph() { return(0); }
extern int  SB_DataEditCnf() { return(0); }
extern int  SB_DataScan() { return(0); }
extern int  SB_StatUnitRoot() { return(0); }
extern int  SB_WsLoad() { return(0); }
extern int  SB_WsSave() { return(0); }
extern int  SB_WsMerge() { return(0); }
extern int  SB_WsDescr() { return(0); }
extern int  SB_WsCopy() { return(0); }
extern int  SB_WsClear() { return(0); }
extern int  SB_WsSample() { return(0); }
extern int  SB_WsExtrapolate() { return(0); }
extern int  SB_WsHtoL() { return(0); }
extern int  SB_WsLtoH() { return(0); }
extern int  SB_WsSeasonAdj() { return(0); }
extern int  SB_WsTrend() { return(0); }
extern int  SB_WsAggregate() { return(0); }
extern int  SB_ModelSimulate() { return(0); }
extern int  SB_ModelCompile() { return(0); }
extern int  SB_IdtExecute() { return(0); }
extern int  SB_EqsEstimate() { return(0); }
extern int  SB_Dir() { return(0); }
extern int  SB_XodeRuleImport() { return(0); }


B_exec_system(char* arg)
{
    int     rc;

    kmsg("Executing %s", arg);
    system(arg); // JMP 3/1/2012
    return(rc);
}

B_shellexec(char* arg)
{
    SHELLEXECUTEINFO    sei;

    memset(&sei, 0, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_FLAG_DDEWAIT;
    //sei.hwnd = hWndDOS;
    sei.hwnd = 0;
    sei.nShow = SW_SHOW;
    sei.lpFile = arg;
    //    sei.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(1));

    ShellExecuteEx(&sei);
    return(0);
}

extern B_DataDisplayGraph() { return(0); }
extern B_DataPrintGraph() { return(0); }
extern B_WindowMinimize() { return(0); }
extern B_WindowMaximize() { return(0); }


/* b_view.c */
ODE_scroll(KDB* kdb, char** lst) { return(0); }
int T_view_tbl(char* name, char *smpl, char** vars_names); { return(0); }

/* b_global.c */
// B_ScrollSet -> b_base
// B_ScrollSet(char *arg, long *plong, int inf, int sup) {return(0);}

B_ScrollVarW(char* arg) { return(0); }
B_ScrollVarN(char* arg) { return(0); }
B_ScrollVarM(char* arg) { return(0); }
B_ScrollVarS(char* arg) { return(0); }
B_ScrollSclW(char* arg) { return(0); }
B_ScrollSclN(char* arg) { return(0); }
B_ScrollTblW(char* arg) { return(0); }
B_ScrollVTW(char* arg) { return(0); }
B_ScrollVTW0(char* arg) { return(0); }
B_ScrollVTN(char* arg) { return(0); }

/* b_pdest.c */

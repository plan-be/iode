#include "iode.h"

/************** RP_ALLOC ********************/
// Nouvelle librairie test d'allocations

#define RP_STDALLOC     1
#define MEMBLK_BLK      100
#define MEMBLK_MAXLOST  512
#define MEMBLK_ADDSIZE  10


typedef struct _memblk_ {
    int  used, size;
    char *ptr;
} MEMBLK;

MEMBLK *RP_MEMBLKS = 0;
int     RP_NBMEMBLKS = 0;

RP_alloc_ptrs()
{
    if(RP_MEMBLKS == 0)
        RP_MEMBLKS = (MEMBLK *) SCR_malloc(MEMBLK_BLK * sizeof(MEMBLK)); // JMP 26/8/2012
    else
        RP_MEMBLKS = (MEMBLK *) SCR_realloc((char *)RP_MEMBLKS, sizeof(MEMBLK), RP_NBMEMBLKS, RP_NBMEMBLKS + MEMBLK_BLK); // JMP 26/8/2012

    // memset(RP_MEMBLKS + RP_NBMEMBLKS, 0, MEMBLK_BLK * sizeof(MEMBLK)); // JMP 26/8/2012
    RP_NBMEMBLKS += MEMBLK_BLK;
    return(0);
}

char *RP_alloc(int size)
{
    int         i, pos, iunallocated = -1, iminsize = -1;

    // Taille nulle ==> retour NULL
    if(size <= 0) return((char *)0);

    if(RP_STDALLOC) {
        char *ptr = SCR_malloc(size); // JMP 26/8/2012
        //memset(ptr, 0, size); // JMP 26/8/2012
        return(ptr);
    }

    // 1. Recherche le plus petit pointeur >= size et t.q. used=0 --> minsize
    //    Note également le premier pointeur non alloué --> iunallocated
    for(i = 0 ; i < RP_NBMEMBLKS ; i++) {
        if(RP_MEMBLKS[i].used == 0) {
            if(RP_MEMBLKS[i].ptr == 0) iunallocated = i;
            else if(RP_MEMBLKS[i].size >= size && RP_MEMBLKS[i].size <= size + MEMBLK_MAXLOST) { // On ne veut pas perdre plus de MAXLOST - 1 bytes
                if(iminsize < 0) iminsize = i;
                else if(RP_MEMBLKS[iminsize].size > RP_MEMBLKS[i].size) iminsize = i;
            }
        }
    }

    // 2. Si iminsize >= 0, utiliser celui-là
    if(iminsize >= 0) {
        RP_MEMBLKS[iminsize].used = 1;
        memset(RP_MEMBLKS[iminsize].ptr, 0, size);
        return(RP_MEMBLKS[iminsize].ptr);
    }

    // 3. Position du premier libre (si iunallocated < 0, on ajoute des pointeurs)
    if(iunallocated >= 0) {
        pos = iunallocated;
    }
    else {
        pos = RP_NBMEMBLKS; // Position du premier libre (qui va être alloué)
        RP_alloc_ptrs();
    }

    // Allocation d'un bloc
    RP_MEMBLKS[pos].used = 1;
    RP_MEMBLKS[pos].size = size + MEMBLK_ADDSIZE;
    RP_MEMBLKS[pos].ptr = SCR_malloc(size + MEMBLK_ADDSIZE); // JMP 26/8/2012
    if(RP_MEMBLKS[pos].ptr == 0) return(NULL);
    memset(RP_MEMBLKS[pos].ptr, 0, size + MEMBLK_ADDSIZE);
    return(RP_MEMBLKS[pos].ptr);
}


RP_find_ptr(char *ptr)
{
    int i;

    for(i = 0 ; i < RP_NBMEMBLKS ; i++) {
        if(RP_MEMBLKS[i].ptr == ptr) {
            return(i);
        }
    }
    return(-1);
}

RP_free(char *ptr)
{
    int i;

    if(RP_STDALLOC) {
        if(ptr) SCR_free(ptr); // JMP 26/8/2012
        return(0);
    }

    for(i = 0 ; i < RP_NBMEMBLKS ; i++) {
        if(RP_MEMBLKS[i].ptr == ptr) {
            RP_MEMBLKS[i].used = 0;
            return(0);
        }
    }
    return(-1);
}

/* JMP 12-12-2012 */
RP_free_bufs()
{
    extern char *RP_VSEPS;
    int i;

    if(RP_VSEPS) SCR_free(RP_VSEPS);
    RP_VSEPS = 0;

    // MEMBLKS
    for(i = 0 ; i < RP_NBMEMBLKS ; i++) {
        SCR_free(RP_MEMBLKS[i].ptr);
        RP_MEMBLKS[i].ptr = 0;
    }

    SCR_free(RP_MEMBLKS);
    RP_MEMBLKS = 0;
    RP_NBMEMBLKS = 0;
}

char *RP_stracpy(char *ptr)
{
    char        *ptr1;

    if(RP_STDALLOC) return(SCR_stracpy(ptr));
    ptr1 = RP_alloc(strlen(ptr) + 1);
    strcpy(ptr1, ptr);
    return(ptr1);
}

unsigned char **RP_vtoms(str, seps)
unsigned char    *str,
         *seps;
{
    U_ch    **tbl = 0, *str2 = 0, *str1, tmp;
    int     ntbl = 0, i;

    if(RP_STDALLOC) {
        tbl = SCR_vtoms(str, seps);
        return(tbl);
    }

    if(str == 0) return(tbl);
    str1 = str; // Mémoire du ptr initial

    // 1. Compter les strings entre séparateurs
    while(1) {
        for(i = 0 ; str[i] ; i++)
            if(!U_is_in(str[i], seps)) break; /* SKIP SEPS */
        if(str[i] == 0) break;
        str += i;
        for(i = 0 ; str[i] ; i++)
            if(U_is_in(str[i], seps)) break;
        ntbl++;
        if(str[i] == 0) break; /* JMP 07-06-96 */
        str += i + 1;
    }

    tbl = RP_alloc((ntbl + 1) * sizeof(char *));
    str = str1; // reset valeur initiale
    ntbl = 0;
    while(1) {
        for(i = 0 ; str[i] ; i++)
            if(!U_is_in(str[i], seps)) break; /* SKIP SEPS */
        if(str[i] == 0) break;
        str += i;
        for(i = 0 ; str[i] ; i++)
            if(U_is_in(str[i], seps)) break;

        if(str2 == 0) {
            str2 = RP_stracpy(str);
            str = str2;
        }
        tbl[ntbl] = str;
        ntbl++;
        if(str[i] == 0) break; /* JMP 07-06-96 */
        str[i] = 0;
        str += i + 1;
    }

    return(tbl);
}

unsigned char **RP_vtom(str, sep)
unsigned char   *str;
int             sep;
{
    U_ch    seps[2];

    seps[0] = sep;
    seps[1] = 0;

    return(RP_vtoms(str, seps));
}

RP_free_tbl(unsigned char **tbl)
{
    if(RP_STDALLOC) {
        SCR_free_tbl(tbl);
        return(0);
    }

    RP_free(tbl[0]);
    RP_free(tbl);
    return(0);
}

RP_tbl_size(unsigned char **tbl)
{
    int i;

    if(tbl == 0) return(0);
    for(i = 0 ; tbl[i] ; i++);
    return(i);
}


/***** FIN RP_ALLOC **********************/



/* DEFINITION DES FONCTIONS DE BASE B_*() */

extern int  RP_label(),
       RP_goto(),
       RP_ask(),
       RP_abort(),
       RP_quitode(),
       RP_onerror(),
       RP_return(),
       RP_message(),
       RP_warning(),
       RP_beep(),
       RP_system(),
       RP_chdir(),
       RP_mkdir(),
       RP_rmdir(),
       RP_settime(),
       RP_incrtime(),
       RP_shift(),
       RP_setdebug(),
       RP_repeat(),  /* JMP 29-04-99 */
       RP_repeatstring(),  /* JMP 29-04-99 */
       RP_define(),
       RP_vseps(),          // JMP 12-12-12
       RP_foreach(),        // JMP 13-12-12
       RP_next(),           // JMP 13-12-12
       RP_procdef(),        // JMP 14-02-13
       RP_procexec(),       // JMP 14-02-13
       RP_setindent();      // JMP 18-02-13


extern int  B_FileDelete(),
       B_FileRename(),
       B_FileCopy(),
       B_FileImportCmt(),
       B_FileImportVar(),
       /*          B_FilePrint(), */
       //  B_FileEdit(),
       B_FileList(),
       B_FileList2(),
       B_ReportExec(),
       B_PrintObjDef(),
       B_PrintTbl(),
       B_PrintVar(),
       B_PrintGr(),
       /*            B_PrintGrAll(),
       	    B_PrintGrData(),
       	    B_PrintGrWin(),
       */
       B_GraphDefault(), /*GB 03/10/2011 */
       B_ViewTbl(),
       B_ViewByTbl(),
       B_ViewTblFile(),
       B_ViewGr(),
       B_ViewVar(),
       B_PrintDest(),
       B_PrintDestNew(),
       B_PrintNbDec(),
       B_PrintMulti(), /* GB 30/10/2007*/
       B_PrintLang(),
       B_PrintObjTblTitle(),
       B_PrintObjLec(),
       B_PrintObjEqsInfos(),

       B_DataUpdate(),
       B_DataUpdateBlock(),
       B_DataCreate(),
       B_DataExist(),
       B_DataAppend(),
       B_DataDelete(),
       B_DataRename(),
       B_DataSearch(),
       B_DataDuplicate(),
       B_DataList(),
       B_DataCompare(),
       B_DataCompareEps(),
       B_DataListSort(),
       B_DataPrintGraph(),
       B_DataDisplayGraph(),
       B_DataEditCnf(),
       B_DataScan(),
       B_DataPattern(), /* GB 01/2005 */

       B_StatUnitRoot(),

       B_ExcelGet(), /* GB 10/97 */
       B_ExcelLang(),
       B_ExcelDecimal(),
       B_ExcelThousand(),
       B_ExcelCurrency(),
       B_ExcelSet(),
       B_ExcelExecute(),
       B_ExcelOpen(),
       B_ExcelNew(),
       B_ExcelClose(),
       B_ExcelPrint(),
       B_ExcelSave(),
       B_ExcelSaveAs(),
       B_ExcelSendKeys(),
       B_ExcelWrite(),

       B_DSImportDb(), /* GB XMAS 97*/

       B_ScrollVarW(),
       B_ScrollVarN(),
       B_ScrollVarM(),
       B_ScrollVarS(),
       B_ScrollTblW(),
       B_ScrollSclW(),
       B_ScrollSclN(),
       B_ScrollVTW(),
       B_ScrollVTW0(),
       B_ScrollVTN(),

       B_WsLoad(),
       B_WsCopy(),
       B_WsImport(),
       B_WsExport(),
       B_WsSave(),
       B_WsSaveCmp(),
       B_WsMerge(),
       B_WsDescr(),
       B_WsSample(),
       B_WsClear(),
       B_WsClearAll(),
       B_WsExtrapolate(),
       B_WsHtoLLast(),
       B_WsHtoLSum(),
       B_WsHtoLMean(),
       B_WsLtoHStock(),
       B_WsLtoHFlow(),
       B_WsSeasonAdj(),
       B_WsTrend(),
       B_WsTrendStd(),
       B_WsAggrChar(),
       B_WsAggrSum(),
       B_WsAggrMean(),
       B_WsAggrProd(),
       B_WindowMinimize(),
       B_WindowMaximize(),
       B_Sleep(),
       B_WsImportEviews(),

       B_ModelSimulate(),
       B_ModelExchange(),
       B_ModelSimulateParms(),
       B_ModelCompile(),
       B_ModelCalcSCC(),       // JMP 6/3/2012
       B_ModelSimulateSCC(),   // JMP 6/3/2012
       B_ModelSimulateSaveNIters(), // JMP 7/3/2019
       B_ModelSimulateSaveNorms(),  // JMP 7/3/2019
    

       B_IdtExecute(),
       B_IdtExecuteVarFiles(),
       B_IdtExecuteSclFiles(),
       B_IdtExecuteTrace(),
       B_EqsEstimate(),
       B_EqsStepWise(),
       B_EqsSetMethod(),
       B_EqsSetBloc(),
       B_EqsSetSample(),
       B_EqsSetInstrs(),
       B_EqsSetCmt(),
       B_DataCalcVar(),
       B_DataCalcLst(),
       B_DataRasVar(),   /* GB 12/04 */

       B_PrintA2mAppend(),
       B_PrintFont(),
       B_PrintTFont(),
       B_PrintTBox(),
       B_PrintTColor(),
       B_PrintTWidth(),
       B_PrintTBreak(),
       B_PrintTPage(),

       B_PrintGColor(),
       B_PrintGBox(),
       B_PrintGBrush(),
       B_PrintGSize(),
       B_PrintGPage(),
       B_PrintGTheme(),
       B_PrintGBand(),

       B_PrintRtfHelp(),
       B_PrintHtmlHelp(),
       B_PrintRtfTopic(),
       B_PrintRtfLevel(),
       B_PrintRtfTitle(),
       B_PrintRtfCopy(),
       B_PrintParaNum(),
       B_PrintPageHeader(),
       B_PrintPageFooter(),
       B_PrintGdiOrient(),
       B_PrintGdiDuplex(),
       B_PrintGdiPrinter(),
       B_PrintHtmlStrip(),
       B_PrintHtmlStyle(),

       B_PrintGIFTransColor(), /* JMP 26-08-98 */
       B_PrintGIFBackColor(),
       B_PrintGIFInterlaced(),
       B_PrintGIFTransparent(),
       B_PrintGIFFilled(),
       B_PrintGIFFont(),

       B_DDEGet(),
       B_SysRename(),
       B_SysCopy(),
       B_SysAppend(),
       B_SysDelete(),
       B_shellexec(),
       B_SysOemToUTF8(),
       B_SysAnsiToUTF8(),
       B_SysOemToAnsi(),
       B_SysAnsiToOem(),

       B_A2mToHtml(),
       B_A2mToMif(),
       B_A2mToRtf(),
       B_A2mToCsv(),
       B_A2mToPrinter(),

       B_PrintHtmlTableClass(),
       B_PrintHtmlTRClass(),
       B_PrintHtmlTHClass(),
       B_PrintHtmlTDClass(),

       B_CsvSave(),
       B_CsvNbDec(),
       B_CsvDec(),
       B_CsvSep(),
       B_CsvNaN(),
       B_CsvAxes(),

       SB_FileDelete(),
       SB_FileRename(),
       SB_FileCopy(),
       /*          SB_FilePrint(), */
       //SB_FileEdit(),
       SB_FileList(),
       SB_FileImport(),
       SB_ReportExec(),
       SB_ReportEdit(),
       SB_ReportPrompt(),
       SB_PrintObjDef(),
       SB_PrintCnf(),
       SB_ViewPrintGr(),
       SB_ViewPrintTbl(),
       SB_ViewByTbl(),
       SB_DataEdit(),
       SB_DataEditScroll(),
       SB_DataSearch(),
       SB_DataDuplicate(),
       SB_DataList(),
       SB_DataCompare(),
       SB_DataCalcLst(),
       SB_DataListSort(),
       SB_DataEditGraph(),
       SB_DataEditCnf(),
       SB_DataScan(),

       SB_StatUnitRoot(),

       /*            SB_WsStatus(), */
       SB_WsLoad(),
       SB_WsSave(),
       SB_WsMerge(),
       SB_WsDescr(),
       SB_WsCopy(),
       SB_WsClear(),
       SB_WsSample(),
       SB_WsExtrapolate(),
       SB_WsHtoL(),
       SB_WsLtoH(),
       SB_WsSeasonAdj(),
       SB_WsTrend(),
       SB_WsAggregate(),

       SB_ModelSimulate(),
       SB_ModelCompile(),
       SB_IdtExecute(),
       SB_EqsEstimate(),
       SB_XodeRuleImport(),    /* JMP 15-03-07 */
       SB_Dir();

typedef struct _repfns_ {
    char    *keyw;
    int     (*fn)();
    int     (*sfn)();
    int     type;
} BFNS;

int RP_RT = 0;
/* 0 : ignore
   -2: return
   -3: abort
   -4: quit ode */

int RP_PRINT = 2;   /* 2 print and display messages, 1 print, 0: do not */
int     RP_T = 0;
PERIOD  RP_PER;

/*
typedef struct _repfile_ {
    char                *filename;
    unsigned char       **tbl;
    int                 curline,
                        nblines;
} REPFILE;
*/

REPFILE *CUR_REPFILE = 0;

extern KDB     *RP_MACRO; // see b_defs.c
char    **RP_argv = NULL;
char    *RP_fdbuf = NULL;
int     RP_fdpos = 0;
int     RP_depth = 0;
int     RP_arg0 = 0;
int     RP_debug = 0; /* JMP 06-08-98 */
int     RP_indent_allowed = 0; /* JMP 18-02-13 */

BFNS B_fns[] = {

    /* NOMS                     FN NON FSCREEN  FN FULLSCREEN TYPE:0=sans ext
    							   1=ext obl. WS
    							   2=ext oblig. tt fichier
    							   3=ext 1 obl non FS, libre FS
    							   4=ext 2 obl non FS, libre FS  */
    /* ex‚cution du rapport */
    "label",                    RP_label    ,   RP_label    ,      0,
    "goto",                     RP_goto     ,   RP_goto     ,      0,
    "ask",                      RP_ask      ,   RP_ask      ,      0,
    "abort",                    RP_abort    ,   RP_abort    ,      0,
    "quitode",                  RP_quitode  ,   RP_quitode  ,      0,
    "quit",                     RP_quitode  ,   RP_quitode  ,      0, /* JMP 16-03-11 */
    "onerror",                  RP_onerror  ,   RP_onerror  ,      0,
    "return",                   RP_return   ,   RP_return   ,      0,
    "show",                     RP_message  ,   RP_message  ,      0,
    "msg",                      RP_warning  ,   RP_warning  ,      0,
    "beep",                     RP_beep     ,   RP_beep     ,      0,
    "system",                   RP_system   ,   RP_system   ,      0,
    "chdir",                    RP_chdir    ,   RP_chdir    ,      0,
    "mkdir",                    RP_mkdir    ,   RP_mkdir    ,      0,
    "rmdir",                    RP_rmdir    ,   RP_rmdir    ,      0,
    "settime",                  RP_settime  ,   RP_settime  ,      0,
    "incrtime",                 RP_incrtime ,   RP_incrtime ,      0,
    "shift",                    RP_shift ,      RP_shift,          0,
    "define",                   RP_define   ,   RP_define   ,      0,
    "debug",                    RP_setdebug,    RP_setdebug ,      0, /* JMP 06-08-98 */
    "indent",                   RP_setindent,   RP_setindent,      0, /* JMP 18-02-13 */
    "shellexec",                B_shellexec,    B_shellexec,       0, /* JMP 02-02-99 */
    "repeat",                   RP_repeat,      RP_repeat,         0, /* JMP 29-04-99 */
    "repeatstring",             RP_repeatstring, RP_repeatstring,  0, /* JMP 29-04-99 */
    "vseps",                    RP_vseps,       RP_vseps,          0, /* JMP 12-12-12 */
    "foreach",                  RP_foreach,     RP_foreach,        0, /* JMP 13-12-12 */
    "next",                     RP_next,        RP_next,           0, /* JMP 13-12-12 */
    "procdef",                  RP_procdef,     RP_procdef,        0, /* JMP 14-2-13 */
    "procexec",                 RP_procexec,    RP_procexec,       0, /* JMP 14-2-13 */

    /* fonctions utilisateur */
    "fileimportvar",            B_FileImportVar, SB_XodeRuleImport,     0,
    "fileimportcmt",            B_FileImportCmt, SB_XodeRuleImport,     0,
    "filedelete",               B_FileDelete,   NULL,              4,
    "filerename",               B_FileRename,   NULL,              4,
    "filecopy",                 B_FileCopy,     NULL,              4,
    "wssample",                 B_WsSample,     SB_WsSample,      0,
    "wsload",                   B_WsLoad,       SB_WsLoad,        3,
    "wscopy",                   B_WsCopy,       SB_WsCopy,        3,
    "wssave",                   B_WsSave,       SB_WsSave,        3,
    "wssavecmp",                B_WsSaveCmp,    SB_WsSave,        3,
    "wsimport",                 B_WsImport,     SB_WsLoad,        3,
    "wsexport",                 B_WsExport,     SB_WsSave,        3,
    "wsmerge",                  B_WsMerge,      SB_WsMerge,       3,
    "wsclear",                  B_WsClear,      SB_WsClear,       3,
    "wsclearall",               B_WsClearAll,   NULL,             0,
    "wsdescr",                  B_WsDescr,      SB_WsDescr,       0,
    "wsextrapolate",            B_WsExtrapolate,SB_WsExtrapolate, 0,
    "wsaggrchar",               B_WsAggrChar,   NULL,             0,
    "wsaggrsum",                B_WsAggrSum,    SB_WsAggregate,   0,
    "wsaggrmean",               B_WsAggrMean,   SB_WsAggregate,   0,
    "wsaggrprod",               B_WsAggrProd,   SB_WsAggregate,   0,
    "wshtollast",               B_WsHtoLLast,   SB_WsHtoL,        0,
    "wshtolsum",                B_WsHtoLSum,    SB_WsHtoL,        0,
    "wshtolmean",               B_WsHtoLMean,   SB_WsHtoL,        0,
    "wsltohflow",               B_WsLtoHFlow,   SB_WsLtoH,        0,
    "wsltohstock",              B_WsLtoHStock,  SB_WsLtoH,        0,
    "wsseasonadj",              B_WsSeasonAdj,  SB_WsSeasonAdj,   0,
    "wstrend",                  B_WsTrend,      SB_WsTrend,       0,
    "wstrendstd",               B_WsTrendStd,   SB_WsTrend,       0,

    "wsimporteviews",			B_WsImportEviews, NULL, 		  0,	// JMP 4/5/2012

    "dataedit",                 NULL,           SB_DataEditScroll,1,
    "dataupdate",               B_DataUpdate,   NULL,             1,
//    "dataupdateblock",          B_DataUpdateBlock, NULL,          1,
    "dataexist",                B_DataExist,    NULL,             1,
    "dataappend",               B_DataAppend,   NULL,             1,
    "datacreate",               B_DataCreate,   NULL,             1,
    "datadelete",               B_DataDelete,   NULL,             1,
    "datarename",               B_DataRename,   NULL,             1,
    "datasearch",               B_DataSearch,   SB_DataSearch,    1,
    "dataduplicate",            B_DataDuplicate,SB_DataDuplicate, 1,
    "datalist",                 B_DataList,     SB_DataList,      1,
    "datacompare",              B_DataCompare,  SB_DataCompare,   1,
    "datacompareeps",           B_DataCompareEps, SB_DataCompare,  0,
    "datalistsort",             B_DataListSort, SB_DataListSort,  0,
    "datadisplaygraph",         B_DataDisplayGraph, SB_DataEditGraph, 0,
    "dataprintgraph",           B_DataPrintGraph, SB_DataEditGraph, 0,
    "dataeditcnf",              B_DataEditCnf,  NULL/*SB_DataEditCnf*/,   0,
    "datacalcvar",              B_DataCalcVar,  NULL          ,   0,
    "datacalclst",              B_DataCalcLst,  SB_DataCalcLst,   0,
    "datarasvar",               B_DataRasVar,   NULL          ,   0,  /* GB 12/2004 */
    "datascan",                 B_DataScan,     SB_DataScan,      1,
    "datapattern",              B_DataPattern,  NULL,             1,  /* GB 01/2005 */

    "excelget",                 B_ExcelGet,     NULL,             1,
    "excellang",                B_ExcelLang,    NULL,             0,
    "exceldecimal",             B_ExcelDecimal, NULL,             0,
    "excelthousand",            B_ExcelThousand, NULL,             0,
    "excelcurrency",            B_ExcelCurrency, NULL,             0,
    "excelset",                 B_ExcelSet,     NULL,             1,
    "excelexecute",             B_ExcelExecute, NULL,             0,
    "excelopen",                B_ExcelOpen,    NULL,             0,
    "excelnew",                 B_ExcelNew,     NULL,             0,
    "excelclose",               B_ExcelClose,   NULL,             0,
    "excelprint",               B_ExcelPrint,   NULL,             0,
    "excelsave",                B_ExcelSave,    NULL,             0,
    "excelsaveas",              B_ExcelSaveAs,  NULL,             0,
    "excelwrite",               B_ExcelWrite,   NULL,             0,

    "dsimportdb",               B_DSImportDb,   NULL,             0,

    "statunitroot",             B_StatUnitRoot, SB_StatUnitRoot,  0,

    "datawidthvar",             B_ScrollVarW,   NULL          ,   0,
    "datandecvar",              B_ScrollVarN,   NULL          ,   0,
    "datamodevar",              B_ScrollVarM,   NULL          ,   0,
    "datastartvar",             B_ScrollVarS,   NULL          ,   0,
    "datawidthtbl",             B_ScrollTblW,   NULL          ,   0,
    "datawidthscl",             B_ScrollSclW,   NULL          ,   0,
    "datandecscl",              B_ScrollSclN,   NULL          ,   0,
    "viewwidth",                B_ScrollVTW,    NULL          ,   0,
    "viewwidth0",               B_ScrollVTW0,   NULL          ,   0,
    "viewndec",                 B_ScrollVTN,    NULL          ,   0,
    "printobjtitle",            B_PrintObjTblTitle,NULL          ,   0,
    "printobjlec",              B_PrintObjLec,  NULL          ,   0,
    "printobjinfos",            B_PrintObjEqsInfos,NULL          ,   0,

    "printobjdef",              B_PrintObjDef,  SB_PrintObjDef,   1,
    "printdest",                B_PrintDest,    NULL,             0,
    "printdestnew",             B_PrintDestNew, NULL,             0,
    "printmulti",               B_PrintMulti,   NULL,             0,
    "printnbdec",               B_PrintNbDec,   NULL,             0,
    "printlang",                B_PrintLang,    NULL,             0,
    "printtblfile",             B_ViewTblFile,  NULL,             0,
    "printtbl",                 B_PrintTbl,     SB_ViewPrintTbl,  0,
    "printvar",                 B_PrintVar,     NULL,             0,

    "viewtblfile",              B_ViewTblFile,  NULL,             0,
    "viewtbl",                  B_ViewTbl,      SB_ViewPrintTbl,  0,
    "viewvar",                  B_ViewVar,      NULL,             0,
    "viewbytbl",                B_ViewByTbl,    SB_ViewByTbl,     0,

    "viewgr",                   B_ViewGr,       SB_ViewPrintGr,   0,
    /*    "printgrall",               B_PrintGrAll,   SB_ViewPrintGr,   0,
        "printgrdata",              B_PrintGrData,  SB_ViewPrintGr,   0,
        "printgrwin",               B_PrintGrWin,   SB_ViewPrintGr,   0,
    /* JMP 02-12-97 */

    "graphdefault",                         B_GraphDefault, NULL,                     0,

    "printgrall",               B_PrintGr,      SB_ViewPrintGr,   0,
    "printgr",                  B_PrintGr,      SB_ViewPrintGr,   0,

    "modelsimulate",            B_ModelSimulate,SB_ModelSimulate,   0,
    "modelsimulateparms",       B_ModelSimulateParms, NULL,         0,
    "modelexchange",            B_ModelExchange,      NULL,         0,
    "modelcompile",             B_ModelCompile, 	  SB_ModelCompile,    0,

    "modelcalcscc",             B_ModelCalcSCC,       NULL,         0,
    "modelsimulatescc",         B_ModelSimulateSCC,   NULL,         0,
    
    "modelsimulatesaveniters",  B_ModelSimulateSaveNIters,  NULL,   0,
    "modelsimulatesavenorms",   B_ModelSimulateSaveNorms,  NULL,   0,
    

    "idtexecute",               B_IdtExecute,         SB_IdtExecute,0,
    "idtexecutevarfiles",       B_IdtExecuteVarFiles, NULL,         0,
    "idtexecutesclfiles",       B_IdtExecuteSclFiles, NULL,         0,
    "idtexecutetrace",          B_IdtExecuteTrace,    NULL,         0,

    "eqsestimate",              B_EqsEstimate,        SB_EqsEstimate,0,
    "eqsstepwise",              B_EqsStepWise,        NULL,          0,
    "eqssetmethod",             B_EqsSetMethod,       NULL,          0,
    "eqssetbloc",               B_EqsSetBloc,         NULL,          0,
    "eqssetblock",              B_EqsSetBloc,         NULL,          0,
    "eqssetsample",             B_EqsSetSample,       NULL,          0,
    "eqssetinstrs",             B_EqsSetInstrs,       NULL,          0,
    "eqssetcmt",                B_EqsSetCmt,          NULL,          0,

    "reportexec",               B_ReportExec,         SB_ReportExec,    0,
    //"reportedit",               B_FileEdit,         SB_ReportEdit,    0,
    "reportedit",               NULL,                 SB_ReportEdit,    0,
    "prompt",                   NULL,                 SB_ReportPrompt,  0,
    "minimize",                 B_WindowMinimize, NULL,  0,
    "maximize",                 B_WindowMaximize, NULL,  0,
    "sleep",                    B_Sleep,          NULL,  0,

    "printa2mappend",           B_PrintA2mAppend, NULL,           0,
    "printfont",                B_PrintFont,      NULL,           0,
    "printtablefont",           B_PrintTFont,     NULL,           0,
    "printtablebox",            B_PrintTBox,      NULL,           0,
    "printtablecolor",          B_PrintTColor,    NULL,           0,
    "printtablewidth",          B_PrintTWidth,    NULL,           0,
    "printtablebreak",          B_PrintTBreak,    NULL,           0,
    "printtablepage",           B_PrintTPage,     NULL,           0,

    "printhtmltableclass",      B_PrintHtmlTableClass, NULL,           0,
    "printhtmltrclass",         B_PrintHtmlTRClass, NULL,           0,
    "printhtmlthclass",         B_PrintHtmlTHClass, NULL,           0,
    "printhtmltdclass",         B_PrintHtmlTDClass, NULL,           0,

    "printbackground",          B_PrintGColor,    NULL,           0,
    "printgraphbox",            B_PrintGBox,      NULL,           0,
    "printgraphbrush",          B_PrintGBrush,    NULL,           0,
    "printgraphsize",           B_PrintGSize,     NULL,           0,
    "printgraphpage",           B_PrintGPage,     NULL,           0,
    "printgraphtheme",          B_PrintGTheme,    NULL,           0,
    "printgraphband",           B_PrintGBand,     NULL,           0,

    "printrtfhelp",             B_PrintRtfHelp,   NULL,           0,
    "printhtmlhelp",            B_PrintHtmlHelp,  NULL,           0,
    "printrtftopic",            B_PrintRtfTopic,  NULL,           0,
    "printrtflevel",            B_PrintRtfLevel,  NULL,           0,
    "printrtftitle",            B_PrintRtfTitle,  NULL,           0,
    "printrtfcopyright",        B_PrintRtfCopy,   NULL,           0,
    "printparanum",             B_PrintParaNum,   NULL,           0,
    "printpageheader",          B_PrintPageHeader,   NULL,           0,
    "printpagefooter",          B_PrintPageFooter,   NULL,           0,
    "printorientation",         B_PrintGdiOrient,   NULL,           0,
    "printduplex",              B_PrintGdiDuplex,   NULL,           0,
    "setprinter",               B_PrintGdiPrinter,  NULL,           0,

    "printgiftranscolor",       B_PrintGIFTransColor,  NULL,  0, /* JMP 26-08-98 */
    "printgifbackcolor",        B_PrintGIFBackColor,  NULL,   0,
    "printgifinterlaced",       B_PrintGIFInterlaced,  NULL,  0,
    "printgiftransparent",      B_PrintGIFTransparent,  NULL, 0,
    "printgiffilled",           B_PrintGIFFilled,  NULL,      0,
    "printgiffont",             B_PrintGIFFont,  NULL,        0,

    "printhtmlstrip",           B_PrintHtmlStrip,  NULL,        0,
    "printhtmlstyle",           B_PrintHtmlStyle,  NULL,        0,

    "ddeget",                   B_DDEGet,          NULL,       0,
    "sysmovefile",              B_SysRename,       NULL,       0,
    "syscopyfile",              B_SysCopy,         NULL,       0,
    "sysappendfile",            B_SysAppend,       NULL,       0,
    "sysdeletefile",            B_SysDelete,       NULL,       0,
    "sysansitoutf8",            B_SysAnsiToUTF8,   NULL,       0,
    "sysoemtoutf8",             B_SysOemToUTF8,    NULL,       0,
    "sysansitooem",             B_SysAnsiToOem,    NULL,       0,
    "sysoemtoansi",             B_SysOemToAnsi,    NULL,       0,

    "a2mtohtml",                B_A2mToHtml,       NULL,       0,
    "a2mtomif",                 B_A2mToMif,        NULL,       0,
    "a2mtocsv",                 B_A2mToCsv,        NULL,       0,
    "a2mtortf",                 B_A2mToRtf,        NULL,       0,
    "a2mtoprinter",             B_A2mToPrinter,    NULL,       0,

    "csvsave",                  B_CsvSave,         NULL,       3, // JMP 2-3-2016
    "csvdigits",                B_CsvNbDec,        NULL,       0, // JMP 2-3-2016
    "csvsep",                   B_CsvSep,          NULL,       0, // JMP 2-3-2016
    "csvdec",                   B_CsvDec,          NULL,       0, // JMP 2-3-2016
    "csvnan",                   B_CsvNaN,          NULL,       0, // JMP 2-3-2016
    "csvaxes",                  B_CsvAxes,         NULL,       0, // GB 23-4-2016

    0, 0, 0, 0
};



/* EXECUTION D'UN RAPPORT */

char *RP_RPTSTR = 0, *RP_VSEPS = 0;   /* JMP 06-06-99 */

RP_vseps(seps)   /* JMP 12-12-12 */
char    *seps;
{
    if(seps == 0 || strlen(seps) == 0) seps = ";, ";
    SCR_free(RP_VSEPS);
    RP_VSEPS = SCR_stracpy(seps);
    return(0);
}

RP_repeatstring(buf)   /* JMP 06-06-99 */
char    *buf;
{
    SCR_sqz(buf);
    if(strlen(buf) == 0) buf = "_";
    SCR_free(RP_RPTSTR);
    RP_RPTSTR = SCR_stracpy(buf);
    return(0);
}

// Nouvelle version améliorée pour les allocs JAN 2009
RP_repeat(buf)
char    *buf;
{
    U_ch    *line = 0, *cmd;
    U_ch    **args;
    int     rc, i, pos1, pos2, maxlg = 0, lg;

    rc = RP_expand(&line, buf);
    if(rc) return(rc);

    pos1 = U_pos('"', line); // Position du premier " dans line
    if(pos1 < 0) {
        SCR_free(line);
        return(-1);
    }
    pos2 = pos1 + 1 + U_pos('"', line + pos1 + 1); // Position du second " dans line
    line[pos2] = 0; // line + pos1  devient la commande ($repeat "$DatadeleteVar _ _" donne dans line + pos 1 = [$DataDeleteVar _ _] par exemple)
    args = B_ainit_chk(line + pos2 + 1, NULL, 0); // Arguments sur lesquels il faut boucler

    if(args == NULL) {
        SCR_free(line);
        return(-1);
    }

    if(RP_RPTSTR == 0) RP_repeatstring("_");

    // Calcule la longueur max d'un arg et alloue une commande assez longue
    for(maxlg = i = 0 ; args[i] ; i++) {
        lg = strlen(args[i]);
        if(lg > maxlg) maxlg = lg;
    }
    //cmd = SCR_malloc(strlen(line + pos1 + 1) + 10 * maxlg + 64);
    cmd = RP_alloc(strlen(line + pos1 + 1) + 10 * maxlg + 64);

    // Démarre la boucle
    for(i = 0 ; args[i] ; i++) {
        strcpy(cmd, line + pos1 + 1); // Commande
        SCR_replace(cmd, RP_RPTSTR, args[i]);
        rc = B_ReportLine(cmd);
        if(rc) break;
    }
    //SCR_free(cmd);
    RP_free(cmd);
    SCR_free(line); //GB 20/08/2012 solved memory leak
    SCR_free_tbl(args);
    return(0);
}
B_ReportLine(line)
char    *line;
{
    unsigned char       **argv = NULL, **o_argv,
                          **SCR_vtomsq();
    int                 rc = 0, o_arg0,
                        B_ReportExec_1();
    REPFILE             *rf;

    // Save old argv + cmd (arg0)
    o_argv = RP_argv;
    o_arg0 = RP_arg0;

    argv = B_ainit_chk("temp.rep", NULL, 0);
    if(argv == NULL || SCR_tbl_size(argv) == 0) {
        rc = -1;
        goto done;
    }

    // Premier rapport ? (début de session de rapport)
    if(RP_depth == 0) {
        RP_T = 0;
        memset(&RP_PER, 0, sizeof(PERIOD));
    }

    // Sauve les arguments pour usage dans les ss fonctions
    RP_argv = argv + 1;
    RP_arg0 = 0;
    RP_depth ++;

    // Crée une struct REPFILE
    rf = RP_alloc(sizeof(REPFILE));
    rf->filename = RP_stracpy("temp.rep");// JMP 4/02/09
    rf->tbl = RP_vtom(line, '\n');
    rf->curline = 0;
    rf->nblines = RP_tbl_size(rf->tbl); // JMP 4/02/09

    // Exécute la ligne (non expandée à ce stade)
    rc = B_ReportExec_tbl(rf);

    // Libère : rf et remet CUR_REPFILE à 0
    B_free_repfile(rf);

    // Remonte un niveau de profondeur d'exécution des rapports
    RP_depth --;

done:
    // Free le tableau des arguments utilisé dans les ss fonctions
    // Remet les anciens args
    SCR_free_tbl(argv);
    RP_argv = o_argv;
    RP_arg0 = o_arg0;

    // Si fin des rapports vide les macros
    if(RP_depth == 0) {
        K_free(RP_MACRO);
        RP_MACRO = NULL;
    }
    return(rc);
}

B_ReportExec(arg)
char    *arg;
{
    unsigned char   **argv = NULL, **o_argv,
                      **SCR_vtomsq();
    int     rc = 0, o_arg0,
            B_ReportExec_1();

    o_argv = RP_argv;
    o_arg0 = RP_arg0;

    /*    argv = SCR_vtomsq(arg, B_SEPS, '"'); */
    argv = B_ainit_chk(arg, NULL, 0);
    if(argv == NULL || SCR_tbl_size(argv) == 0) {
        rc = -1;
        goto done;
    }

    if(RP_depth == 0) {
        RP_T = 0;
        memset(&RP_PER, 0, sizeof(PERIOD));
    }
    RP_argv = argv + 1;
    RP_arg0 = 0;
    RP_depth ++;
    rc = B_ReportExec_1(argv[0]);
    RP_depth --;

done:
    SCR_free_tbl(argv);
    RP_argv = o_argv;
    RP_arg0 = o_arg0;
    if(RP_depth == 0) {
        K_free(RP_MACRO);
        RP_MACRO = NULL;
    }
    return(rc);
}

unsigned char **B_read_file(filename)
char    *filename;
{
    FILE            *fd;
    unsigned char   **txt = 0,
                      buffer[LINELENGTH + 1];
    int             len = 0, ch,
                    nb_line = 0,
                    i,
                    o_add_ptr = SCR_ADD_PTR_CHUNCK; // JMP 13-12-2012;

    /* OPEN INFO FILE */
    fd = fopen(filename, "r");
    if(fd == NULL) return(txt);

    /* READ INFO */
    SCR_ADD_PTR_CHUNCK = 1000; // JMP 13-12-2012
    while(1) {
        ch = getc(fd);
        switch(ch) {
        case EOF:
            buffer[len] = 0;
            SCR_add_ptr(&txt, &nb_line, buffer);
            SCR_add_ptr(&txt, &nb_line, NULL);
            fclose(fd);
            SCR_ADD_PTR_CHUNCK = o_add_ptr; // JMP 13-12-2012
            return(txt);
        case '\n':
            buffer[len] = 0;
            SCR_add_ptr(&txt, &nb_line, buffer);
            len = 0;
            break;
            /* case '\t':
            for(i = len ; i < len + 8 - len%8 ; i++)
                buffer[i] = ' ';
            len = i - 1;
            break;
            */
        default:
            if(len < sizeof(buffer) - 1) buffer[len++] = ch;
            else {
                B_seterror("Report line exceeds %d characters", LINELENGTH);
                SCR_add_ptr(&txt, &nb_line, NULL);
                fclose(fd);
                SCR_free_tbl(txt);
                SCR_ADD_PTR_CHUNCK = o_add_ptr; // JMP 13-12-2012
                return(NULL);
            }
            break;
        }
    }
}

RP_readline(rf, line, mode)
REPFILE *rf;
char    **line;
int     mode;
{
    int     rc = 0, i;

    if(rf->curline >= rf->nblines) return(EOF); // EOF

    if(mode) { // expand macros, {}, ...
        rc = RP_expand(line, rf->tbl[rf->curline]);
        if(rc) return(rc);
    }
    else
        *line = SCR_stracpy(rf->tbl[rf->curline]);

    // Shift text to the margin if $ or # and indent allowed
    if(*line && RP_indent_allowed) {
        for(i = 0 ; (*line)[i] && SCR_is_space((*line)[i]); i++) ; // Goto 1st non space
        if(U_is_in((*line)[i], "$#")) U_ljust_text(*line);
    }
    rf->curline++;
    return(rc);
}

/*
Execute the REPFILE struct rf.
- save the current context (CUR_REPFILE) in local variable oldrf
- set the context to rf
- execute the lines from the REPFILE data line by line
- flush the output
- reset the context to the old rf
*/


B_ReportExec_tbl(REPFILE *rf)
{
    int     rc, rt = 0;
    int     ignore = 0; // ignore: 0 ignore code retour, 1 non
    char    *line = NULL, *arg, name[31], buf[512];
    long    cline = 0L;
    REPFILE     *oldrf;
    char        *M_mem(), filename[256], debug_line[1024];

    oldrf = CUR_REPFILE;
    CUR_REPFILE = rf;

    while(1) {
        SW_nfree(line);
        line = 0;
        //if(RP_debug) kmsg("%s[%d] - %s", rf->filename, rf->curline + 1, M_mem(buf));
        rc = RP_readline(rf, &line, 1);
        if(RP_debug) {
            SCR_strlcpy(filename, rf->filename, 255);
            SCR_strlcpy(debug_line, line, 1023);
            SCR_strip(debug_line);
            if(debug_line[0]) kmsg("%s[%d] - %s", filename, rf->curline, debug_line); // JMP 14/2/2013
        }

        switch(rc) {
        case -2  : // expand of macro failed, or LEC returns a NAN
            rt = -3;
            goto done;

        case EOF :
            goto done;
        }

        if((line[0] == '#' || line[0] == '$') && line[1] != line[0]) {
            if(line[1] == 0 || U_is_in(line[1], " \n\r\t")) continue;

            ignore = RP_chk_ignore(line + 1);
            if(ignore)
                rt = RP_splitline(line + 1, name, &arg, 30);
            else
                rt = RP_splitline(line + 2, name, &arg, 30);

            if(rt == 0) {
                if(name[0] == 0) rt = 0;
                else             rt = RP_exec_fn(name, arg, line[0]);
                if(RP_debug > 1) M_debug(line); // GB MEM
            }

            CUR_REPFILE = rf; // recursivity
            switch(rt)   {
            case -1:                            // error
                if(ignore == 0) {
                    rt = 0;
                    break;
                }
                if(RP_PRINT) RP_err_dump(name, arg);
                rt = RP_RT;                     // positionn‚ par OnError
                switch(rt)  {
                case  0:
                    break;             // ignore
                case -2:
                    rt = 0;
                    goto done; // return
                case -3:                    // abort
                case -4:
                    goto done;         // quitode
                default:
                    goto done;
                }
                break;
            case -2:
                rt = 0;
                goto done; // return
            case -3:                    // abort
            case -4:
                goto done;         // quitode
            default:
                rt = 0;
                break;     // no error
            }
        }
        else {
            W_printf("%s\n", line);
            continue;
        }
    }

done:
    SW_nfree(line);
    line = 0;
//    W_close();
    W_putc('\n');
    W_flush();
    CUR_REPFILE = oldrf;
    return(rt);
}

/*
Execute a report file.
- Read the report into a REPFILE struct
- execute the lines from the REPFILE data
- free the REPFILE struct
*/

REPFILE *B_create_repfile(char *filename, unsigned char **tbl)
{
    REPFILE *rf;

    rf = SCR_malloc(sizeof(REPFILE));
    rf->filename = SCR_stracpy(filename);
    rf->tbl = tbl;
    rf->curline = 0;
    rf->nblines = SCR_tbl_size(tbl);
    return(rf);
}

B_free_repfile(REPFILE *rf)
{
    if(rf) { // CORR JMP 23/5/2019
        SCR_free_tbl(rf->tbl);
        SCR_free(rf->filename);
        SCR_free(rf);
    }
}

B_ReportExec_1(file)
char    *file;
{
	char                filename[K_MAX_FILE + 1];
    unsigned char       **tbl;
    int                 rc;
    REPFILE             *rf;

    strcpy(filename, file);
    K_set_ext(filename, filename, K_REP);
    tbl = B_read_file(filename);
    if(tbl == 0) {
        B_seterrn(52, filename);
        return(-1);
    }
    rf = B_create_repfile(filename, tbl);
    rc = B_ReportExec_tbl(rf);
    B_free_repfile(rf);
    CUR_REPFILE = 0;
    return(rc);
}

RP_chk_ignore(arg)
char    *arg;
{
    int     ch;

    if(arg[0] == '-') return(0);
    return(1);
}


RP_splitline(text, cmd, arg, lg)
char    *text, *cmd, **arg;
int     lg;
{
    int     i = 0, j = 0, rt = 0;

    if(text == NULL || text[0] == '\0' || text[0] == '\n') {
        rt = -1;
        goto done;
    }

    while(1) {
        if(SCR_is_anum(text[j]) || text[j] == '_') {
            if(i < lg)  cmd[i] = text[j];
        }
        else break;

        i++;
        j++;
    }

done :
    cmd[i] = 0;
    *arg = text + j;
    U_ljust_text(*arg);
    SCR_strip(*arg);
    return(rt);
}

extern char k_ext[][4];

RP_find_fn(name, type, fs)
char    *name;
int     *type;
int     fs;
{
    int     i, lg, j;

    SCR_lower(name);
    for(i = 0 ; B_fns[i].keyw ; i++) {
        if(B_fns[i].type == 0) {
            if(strcmp(name, B_fns[i].keyw) == 0) return(i);
            continue;
        }
        else {
            lg = strlen(B_fns[i].keyw);
            if(strncmp(name, B_fns[i].keyw, lg)) continue;
            for(j = 0 ; k_ext[j] ; j++) {
                if(j > K_VAR && B_fns[i].type % 2) break;
                if(strcmp(k_ext[j], name + lg) == 0) {
                    *type = j;
                    return(i);
                }
            }
            if(B_fns[i].type > 2 && fs == "#") {
                *type = 0;
                return(i);
            }

            /*          return(-1); */
        }
    }
    return(-1);
}

RP_exec_fn(name, arg, type)
char    *name, *arg;
int     type;
{
    int     i, parm;

    i = RP_find_fn(name, &parm, type);
    if(i < 0) return(-1);
    if(type == '#' && B_fns[i].sfn != 0)
        return((*B_fns[i].sfn)(arg, parm));
    else if(type == '$' && B_fns[i].fn != 0)
        return((*B_fns[i].fn)(arg, parm));
    return(-1);
}

RP_err_dump(name, arg)
char    *name, *arg;
{
    if(RP_PRINT == 1) { /* File */
        W_printf("Error : %s %s\n", name, arg);
        B_print_last_error();
        return(0);
    }

    /* Display */
    B_seterror("Error : %.70s %.70s", name, arg);
    B_display_last_error();
    return(0);
}

RP_onerror_1(arg)
char *arg;
{
    SCR_upper(arg);

    if(strcmp(arg, "IGNORE")  == 0) RP_RT = 0;
    else    if(strcmp(arg, "RETURN")  == 0) RP_RT = -2;
    else    if(strcmp(arg, "ABORT")   == 0) RP_RT = -3;
    else    if(strcmp(arg, "QUITODE") == 0) RP_RT = -4;
    else    if(strcmp(arg, "DISPLAY") == 0) RP_PRINT = 2;
    else    if(strcmp(arg, "PRINT")   == 0) RP_PRINT = 1;
    else    if(strcmp(arg, "NOPRINT") == 0) RP_PRINT = 0;
    return(0);
}

RP_onerror(arg)
char *arg;
{
    return(B_ainit_loop(arg, RP_onerror_1, NULL));
}


RP_label(arg)
char *arg;
{
    return(0);
}

RP_abort(arg)
char *arg;
{
    return(-3);
}

RP_quitode(arg)
char *arg;
{
    /*    ODE_quit();       /* JMP 11-11-93 */
    SCR_APP_ABORT = ACT_APPL_QUIT;
    K_end_ws(0);
    ODE_end(1);
    exit(0);
    /*    return(-4); */
}

RP_return(arg)
char *arg;
{
    return(-2);
}

RP_goto(arg)
char    *arg;
{
    unsigned    char    **args, **SCR_vtomsq();
    int                 rc = 0;

    args = SCR_vtomsq(arg, B_SEPS, '"');
    switch(SCR_tbl_size(args)) {
    case 0 : // JMP 13-12-12: erreur si pas d'arg uniquement
        rc = -3;
        break;

    case 1 :
        rc = RP_goto_label("label", args[0]);
        break;

    default: // JMP 13-12-12: Ok si plus de 2 args
        rc = atoi(args[1]);
        switch(rc) {
        case  0 :
        case -1 :
            break;
            //case  1 : // JMP 12-12-12 !!
        default :   // JMP 12-12-12 !!
            rc = RP_goto_label("label", args[0]);
            break;
        }
        break;

    }

    SCR_free_tbl(args);
    return(rc);
}

// Vérifie qu'un ligne est une commande
RP_is_cmd(char *line)
{
    if(line == 0) return(0);
    if( (line[0] == '#' || line[0] == '$') &&
        line[1] != line[0] &&
        line[1] != 0 &&
        !U_is_in(line[1], " \n\r\t")
      )
        return(1);
    else
        return(0);
}

/**********************************************************************
Recherche la ligne $ ou # <command> suivi de parm.
Utilisé dans
    $goto <parm>     : cherche $label <parm> (à partir du début du fichier)
    $foreach <parm>  : cherche $next <parm> quand la liste est épuisée
                        (à partir de la ligne courante)
**********************************************************************/
RP_goto_label(char *command, char *parm)
{
    int     rc, beg = 1;
    char    *line = NULL, name[128], *arg;

    if(strcmp(command, "label") == 0)  // JMP 14/2/2013
        CUR_REPFILE->curline = 0;
    SCR_sqz(parm);
    SCR_lower(parm);

    while(1) {
        SW_nfree(line);
        line = 0;
        rc = RP_readline(CUR_REPFILE, &line, 0);
        if(rc == EOF) {
            B_seterror("%s %.80s not found", command, parm);
            rc = -3;
            goto done;
        }

        //if((line[0] == '#' || line[0] == '$') && line[1] != line[0]) {
        //    if(line[1] == 0 || U_is_in(line[1], " \n\r\t")) continue;
        if(!RP_is_cmd(line)) continue;
        if(RP_splitline(line + 1, name, &arg, 30) < 0) {
            rc = -1;
            goto done;
        }

        SCR_lower(name);
        SCR_lower(arg);
        if(strcmp(name, command) == 0 && strcmp(parm, arg) == 0) {
            rc = 0;
            goto done;
        }
    }

done:
    SW_nfree(line);
    line = 0;
    return(rc);
}

RP_next_index(char *name)
{
    unsigned char   buf[512], *line = 0;
    int             n = 0;

    // compte le nb d'éléments restant dans la liste FOREACH__name
    sprintf(buf, "@vcount(%%FOREACH__%s%%)", name);
    RP_expand(&line, buf);
    if(line) {
        n = atol(line);
        SW_nfree(line);
    }
    if(n == 0) { // On a atteint la fin de la liste
        sprintf(buf, "FOREACH__%s", name);
        RP_undef_1(name);
        RP_undef_1(buf);
        RP_define_restore(name);
        RP_define_restore(buf);
        return(-1);
    }
    // Place le premier élément dans l'index de la boucle
    sprintf(buf, "$define %s @vtake(1,%%FOREACH__%s%%)", name, name);
    B_ReportLine(buf);

    // Drop le premier indice
    sprintf(buf, "$define FOREACH__%s @vdrop(1,%%FOREACH__%s%%)", name, name);
    B_ReportLine(buf);

    return(n - 1);
}

// Execute foreach statement

RP_foreach(arg)
char    *arg;
{
    unsigned char   name[128], buf[512], *list;
    int             rc = 0;

    // Crée la liste d'index dans un define (name)
    RP_define(arg);

    if(RP_splitline(arg, name, &list, 30) < 0) return(-1);

    // Sauve les valeurs temp de l'indice de la boucle et de FOREACH_indice
    RP_define_save(name);
    sprintf(buf, "FOREACH__%s", name);
    RP_define_save(buf);

    // Crée un define identique avec comme nom FOREACH__<nom>
    sprintf(buf, "$define FOREACH__%s %%%s%%", name, name);
    B_ReportLine(buf);

    // Place la première valeur dans name et supprime cette valeur de la liste
    // Si la liste est vide, saute à la fin ($next name)
    if(RP_next_index(name) < 0)
        RP_goto_label("next", name);

    return(rc);
}

// Saute jusqu'à la fin de la boucle ($next name)
RP_foreach_break(char *name)
{

}

RP_next(arg)
char    *arg;
{
    unsigned    char    **args, **SCR_vtomsq();
    int                 rc = 0, curline = CUR_REPFILE->curline;

    args = SCR_vtomsq(arg, B_SEPS, '"');
    switch(SCR_tbl_size(args)) {
    case 1 :
        rc = RP_goto_next(args[0]);
        break;

    default:
        rc = -3;
        break;
    }

    SCR_free_tbl(args);
    if(rc != 0) {
        CUR_REPFILE->curline = curline;
        rc = 0;
    }
    return(rc);
}

RP_goto_next(label)
char *label;
{
    int     rc, beg = 1;
    char    *line = NULL, name[128], *arg, **parms;

    SCR_sqz(label);

    while(1)
    {
        // Stoppe si début du fichier atteint
        if(CUR_REPFILE->curline <= 1) return(-3);
        CUR_REPFILE->curline -= 2;
        SW_nfree(line);
        line = 0;
        rc = RP_readline(CUR_REPFILE, &line, 0);

        if((line[0] == '#' || line[0] == '$') && line[1] != line[0]) {
            if(line[1] == 0 || U_is_in(line[1], " \n\r\t")) continue;
            if(RP_splitline(line + 1, name, &arg, 30) < 0) {
                rc = -1;
                goto done;
            }

            SCR_lower(name);
            if(strcmp(name, "foreach") == 0) {
                parms = SCR_vtoms(arg, " ");
                if(parms && parms[0] && strcmp(label, parms[0]) == 0) {
                    SCR_free_tbl(parms);
                    // Place la première valeur dans name et supprime cette valeur de la liste
                    rc = RP_next_index(label);
                    if(rc > 0) rc = 0;
                    goto done;
                }
                SCR_free_tbl(parms);
            }
        }
    }

done:
    SW_nfree(line);
    line = 0;
    return(rc);
}

RP_ask(arg)
char *arg;
{
    int     lg;
    U_ch    name[31];

    lg = B_get_arg0(name, arg, 30);
    if(SCR_confirme(arg + lg + 1) != 0) return(0); /* rep. NON -> ligne suivante */
    /* rep OUI -> va en label */
    if(RP_goto_label("label", name) != 0) return(-3);
    return(0);
}


RP_message(arg)
char *arg;
{
    char    fmt[2048];

    if(arg == NULL || arg[0] == 0) return(0);

    //kmsg("%.80s", arg);

    sprintf(fmt, "%%.%ds", SCR_PAGE_SIZE[1]-20);   /* JMP 17-03-11 */
    kmsg(fmt, arg); // JMP 4/02/09                 /* JMP 17-03-11 */
    /*    SCR_beep();    /* JMP 17-12-93 */
    return(0);
}

RP_warning(arg)            /* JMP 11-07-96 */
char *arg;
{
    if(arg == NULL || arg[0] == 0) return(0);

    if(strlen(arg) > 512) arg[512] = 0;
    PG_display_error("%s", arg);
    return(0);
}

RP_beep()
{
    SCR_beep();
    return(0);
}

RP_setdebug(arg)   /* JMP 06-08-98 */
char    *arg;
{
    switch(SCR_upper_char(arg[0])) {
    case '0' :
    case 'n':
    case 'N':
        RP_debug = 0;
        break;
    case '2' :
    case 'F' :
    case 'f' :
        RP_debug = 2;
        break;

    default :
        RP_debug = 1;
        break;
    }
    return(0);
}

RP_setindent(arg)  // 18/02/2013
char    *arg;
{
    switch(SCR_upper_char(arg[0])) {
        case '0' :
        case 'n':
        case 'N':
            RP_indent_allowed = 0;
            break;

        default :
            RP_indent_allowed = 1;
            break;
    }
    return(0);
}


RP_system(arg)
char *arg;
{
    return(B_exec_system(arg));
}

RP_chdir(arg)
char *arg;
{   
    int rc;
    
    rc = chdir(arg); // JMP 9/4/2019
    ODE_settitle(); 
    return(rc);
}

RP_rmdir(arg)
char *arg;
{
    return(rmdir(arg));
}

RP_mkdir(arg)
char *arg;
{
    return(mkdir(arg));
}

/*
int RP_shift(arg)
char    *arg;
{
    RP_arg0++;
    return(0);
}
*/
int RP_shift(arg)
char    *arg;
{

    int     incr = 1;

    SCR_strip(arg);
    if(arg[0]) incr = atoi(arg);

    RP_arg0+=incr;
    return(0);
}


RP_add(line, lg, j , res)
char    **line, *res;
int     *lg, *j;
{
    int     i, add;

    if(res == NULL) return(0);
    add = strlen(res);
    *line = SW_nrealloc(*line, *lg, *lg + add);
    if(*line == NULL) {
        *lg = 0;
        return(-2);
    }

    for(i = 0; i < add; i++) {
        (*line)[(*j)] = res[i];
        (*j)++;
    }

    (*lg) += add;
    return(0);
}

RP_expand(line, buf)
char    **line, *buf;
{
    int     rc = 0, i, j, k, lg, upper, sqz;  /* JMP 07-09-98 */
    char    *w = NULL, *res = NULL;
    char    *RP_extract(), *RP_gmacro(), *RP_gcmd(), *RP_extractpar();

    lg = strlen(buf);
    *line = SW_nalloc(lg + 1);

    for(i = 0, j = 0; buf[i]; i++) {
        upper = 0;
        sqz = 0;
        switch(buf[i]) {
        case '%' : /* macro */
            i++;
            if(buf[i] == '%') goto dft;  /* JMP 06-08-98 */
            if(buf[i] == '!')      upper = -1; /* JMP 11-07-96 */
            else if(buf[i] == '#') upper = 1;  /* JMP 11-07-96 */
            else if(buf[i] == '-') sqz = 1;  /* JMP 07-09-98 */
            if(upper || sqz) i++;            /* JMP 07-09-98 */
            w = RP_extract(buf, &i, '%'); //JMP 16:45 6/02/2009
            if(w != NULL) {
                res = RP_gmacro(w);
                if(res == 0) {
                    res = SW_nalloc(10 + strlen(w)); // JMP 3/1/2017
                    sprintf(res, "%%%s%%", w);
                }
                if(sqz) SCR_asqz(res, "_"); 
                if(upper < 0) SCR_lower(res);
                if(upper > 0) SCR_upper(res);
                
                rc = RP_add(line, &lg, &j, res);
                SW_nfree(res);
                RP_free(w); // JMP 6/02/09
            }
            else {
                i--;
                (*line)[j] = buf[i];
                j++;
            }

            if(rc < 0) goto done;
            break;

        case '{' : /* command or LEC */
            i++;
            if(buf[i] == '{') goto dft;  /* JMP 06-08-98 */
            w = RP_extract(buf, &i, '}');
            res = RP_gcmd(w);
            rc = RP_add(line, &lg, &j, res);

            SW_nfree(res);
            RP_free(w); // JMP 6/02/09

            if(rc < 0) goto done;
            break;

        case '@' : /* command or LEC */
            i++;
            if(buf[i] == '@') goto dft;  /* JMP 06-08-98 */
            w = RP_extractpar(buf, &i, "()");
            res = RP_gfn(w);
            if(res == 0) {
                i--;
                goto dft; /* JMP 17-03-00 */
            }
            rc = RP_add(line, &lg, &j, res);

            SW_nfree(res);
            SW_nfree(w);

            if(rc < 0) goto done;
            break;

        default  :
dft:
            (*line)[j] = buf[i];
            j++;
            break;
        }
        // if(*line) AddDebug(*line);
    }

done:
    if(rc < 0) {
        SW_nfree(*line);
        *line = NULL;
    }
    return(rc);

}

char    *RP_extract(buf, i, ch)
char    *buf, ch;
int     *i;
{
    int     pos, j;
    char    *ptr, *res;

    pos = U_pos(ch, buf + (*i));
    if(pos < 0) return(NULL);

    res = RP_alloc(pos + 1); // JMP 6/02/09
    for(j = 0; j < pos; j++, (*i)++) res[j] = buf[*i];

    return(res);
}

char    *RP_gmacro(str) // JMP&GB 26/1/09
char    *str;
{
    int     pos;
    char    *res = NULL, buf[20], *tmp = 0;

    if(str == NULL) return(NULL);

    if(U_pos('%', str) >= 0 || U_pos('{', str) >= 0) {
        RP_expand(&tmp, str);
    }
    else {
        tmp = SCR_stracpy(str);
    }

//        if(isdigit(tmp[0])) {
    if(tmp[0] >= '0' && tmp[0] <= '9') {
        pos = atoi(tmp);

        if(pos == 0) {
            sprintf(buf, "%d", SCR_tbl_size(RP_argv) - RP_arg0);
            res = SCR_stracpy(buf);
        }
        else {
            pos += RP_arg0;
            if(pos < 0 || pos > SCR_tbl_size(RP_argv)) {
                B_seterrn(252, pos);
                SCR_free(tmp); // JMP 24/8/2012
                return(NULL);
            }
            res = SCR_stracpy(RP_argv[pos - 1]);
        }
    }
    else {
        if (tmp[0] == '*') res = SCR_mtov(RP_argv + RP_arg0, ';');

        else {
            pos = K_find(RP_MACRO, tmp);
            if(pos < 0) {
                B_seterrn(253, tmp);
                SCR_free(tmp); // JMP&GB 26/1/09
                return(NULL);
            }
            res = SCR_stracpy(KOVAL(RP_MACRO, pos));
        }
    }

    SCR_free(tmp);
    return(res);
}
char    *RP_gcmd(str) // JMP&GB 26/1/09
char    *str;
{
    char    *res = NULL, *tmp = 0;

    if(str == NULL) return(NULL);

    if(U_pos('%', str) >= 0 || U_pos('{', str) >= 0 || U_pos('@', str) >= 0) {
        RP_expand(&tmp, str);
    }
    else
        tmp = SCR_stracpy(str); // JMP&GB 26/1/09

    RP_eval(&res, tmp);
    SCR_free(tmp);              // JMP&GB 26/1/09
    return(res);
}

 IODE_REAL    RP_evallec(lec)
char    *lec;
{
    CLEC    *clec;
    IODE_REAL    x = L_NAN;

    if(RP_evaltime() < 0) return(L_NAN);

    SCR_strip(lec);
    if(lec[0]) {
        clec = L_cc(lec);
        if(clec == NULL) {
            B_seterror("Syntax error %.80s", L_error());
            return(x);
        }
        if(clec != 0 && !L_link(K_WS[K_VAR], K_WS[K_SCL], clec))
            x = L_exec(K_WS[K_VAR], K_WS[K_SCL], clec, RP_T);
        SW_nfree(clec);
    }

    return(x);
}

RP_eval(res, farg)
char    *farg, **res;
{
    char    ch = farg[0], name[31],
            *lec = NULL, *fmt = NULL;
    int     rc, lg, rp_rt, pos, inv = 0;
    IODE_REAL    x, RP_evallec();

    *res = SW_nalloc(41);

    /* Report command to execute */
    if(ch == '$' || ch == '#') {
        rp_rt = RP_RT;
        RP_RT = 0;
        if(farg[1] == '!') {
            inv = 1;
            B_get_arg0(name, farg + 2, 30);
        }
        else B_get_arg0(name, farg + 1, 30);

        rc = RP_exec_fn(name, farg + strlen(name) + 2, ch);
        RP_RT = rp_rt;

        if(rc < 0) rc = 0;
        else rc = 1;

        if(inv) rc = !rc;

        sprintf(*res, "%d", rc);
        return(0);
    }

    /* Excel get */
    if(ch == '=') {
        SCR_free(*res);
        *res = B_ExcelGetItem(farg + 1);
        if(*res == NULL) return(-1);
        SCR_replace(*res, "\t", " ");
        SCR_replace(*res, "\r\n", " ");
        return(0);
    }

    /* LEC command to execute */
    pos = U_pos('@', farg);
    if(pos < 0) lec = farg;
    else {
        farg[pos] = '\0';
        lec = farg;
        fmt = farg + pos + 1;
    }

    x = RP_evallec(lec);
    if(!L_ISAN(x)) return(-1);

    RP_fmt(*res, fmt, x);
    return(0);
}

RP_settime(arg)
char *arg;
{
    PERIOD *rp_per;

    rp_per = PER_atoper(arg);
    if(rp_per == NULL) {
        B_seterror("SetTime %.80s: invalid syntax", arg);
        return(-1);
    }

    memcpy(&RP_PER, rp_per, sizeof(PERIOD));
    SCR_free(rp_per); // JMP 15/2/2013
    return(0);
}

RP_incrtime(arg)
char *arg;
{
    int     incr = 1;
    PERIOD  *per;

    SCR_strip(arg);
    if(arg[0]) incr = atoi(arg);

    per = PER_addper(&RP_PER, incr);
    memcpy(&RP_PER, per, sizeof(PERIOD));
    return(0);
}

RP_evaltime()
{
    RP_T = 0;
    if(RP_PER.p_y == 0) return(0);
    RP_T = PER_diff_per(&RP_PER, &(KSMPL(KV_WS)->s_p1));
    if(RP_T < 0) return(-3);
    return(0);
}

RP_fmt(buf, format, value)
char    *buf, *format;
 IODE_REAL    value;
{
    int     t, lg = 20, nbdec = -1;
    char    **fmt;

    if(format == NULL) goto normal;

    U_ljust_text(format);
    SCR_strip(format);
    SCR_upper(format);
    if(format[0] == '\0') goto normal;

    if(format[0] == 'T') {
        // t = (int) fabs((double) value); /* JMP 24-05-00 */
        t = (int) value; /* JMP 24-05-00 */
        strcpy(buf, PER_pertoa(PER_addper(&(KSMPL(KV_WS)->s_p1), t), NULL));
        return(0);
    }

    fmt = SCR_vtom(format, '.');
    if(fmt == NULL || fmt[0] == NULL) goto normal;

    lg = strlen(fmt[0]);
    if(fmt[1] != NULL) {
        nbdec = strlen(fmt[1]);
        lg += nbdec + 1;
    }
    else if(format[0] == '.') { /* JMP 17-03-00 */
        lg = 30;
        nbdec = strlen(fmt[0]);
    }
    else nbdec = 0; /* JMP 20-03-00 */

    SCR_free_tbl(fmt);

normal:
    if(value > 0) SCR_fmt_dbl((double) value, buf, lg, nbdec);
    else          SCR_fmt_dbl((double) value, buf, lg + 1, nbdec);
    SCR_sqz(buf); /* JMP 01-10-96 */
    return(0);
}


/* drop quote */
U_ch    **SCR_vtomsq(str, seps, quote)
char    *str, *seps, quote;
{
    U_ch    **tbl = 0, tmp;
    int     ntbl = 0, i, inquote;

    inquote = 0;
    while(1) {
        for(i = 0; str[i] && !inquote; i++) {
            if(U_is_in(str[i], seps)) continue; /* SKIP SEPS */
            if(str[i] == quote) {
                inquote = 1;
                continue;
            }
            break;
        }
        if(str[i] == 0) break;

        str += i;
        if(inquote) {
            for(i = 0; str[i]; i++)
                if(str[i] == quote) break;
            inquote = 0;
        }
        else {
            for(i = 0; str[i]; i++)
                if(U_is_in(str[i], seps) ) break;
            if(str[i] == quote) {
                inquote = 1;
                break;
            }
        }

        tmp = str[i];
        str[i] = 0;

        SCR_add_ptr(&tbl, &ntbl, str);
        str[i] = tmp;
        if(tmp == 0) break;
        str += i + 1;
    }

    SCR_add_ptr(&tbl, &ntbl, NULL);
    return(tbl);
}



B_ReportPrompt(arg)
char    *arg;
{
    return(RP_define(arg));
}


B_Sleep(arg)
char    *arg;
{
    int     ms = atoi(arg);

    SCR_sleep(ms);
}



/***********************************
IF ELSE ENDIF

Examples :

$if {%i% < 2}
    $wsloadvar toto.var
    $goto Simulate
$else
    $label suite
    $if {t < 1990Y1}
        $PrintDest toto
    $endif
$endif

$label simulate
    $goto suite


Questions : quid avec les goto qui sortent du bloc?

Solution 1 : implémentatio naïve (1 ligne à la fois, comme le $goto)

lire $if cond
    si true :
        noter le numéro de ligne et true
        aller à la ligne suivante
        si ligne suivante = $endif, passer à la suivante
                          = $else : aller à $endif de même niveau
                          = autre : exec

    si false :
        lire la ligne suivante :
            si $endif de même niveau : aller à la suivante et décrément $ifs
            si $else de même niveau  :
                exécuter jusqu'au $endif de même niveau
            si fin de fichier : erreur
            si autre : sauter à la suivante
        aller à $endif ou $else de même niveau














************************************/

/*
RP_if(char *parm)
{
    int     cond = 0; // par dft : $if false
    char    *line = NULL, name[128];

    if(parm && atoi(parm) != 0) cond = 1;

    // Lit les lignes 1 à 1 et exécute
    while(1) {
        SW_nfree(line);
        line = 0;
        rc = RP_readline(CUR_REPFILE, &line, 0);
        if(rc == EOF) {
            B_seterror("%s %.80s not found", command, parm);
            rc = -3;
            goto done;
        }

        //if((line[0] == '#' || line[0] == '$') && line[1] != line[0]) {
        //    if(line[1] == 0 || U_is_in(line[1], " \n\r\t")) continue;
        if(!RP_is_cmd(line)) continue;
        if(RP_splitline(line + 1, name, &arg, 30) < 0) {
            rc = -1;
            goto done;
        }

        SCR_lower(name);
        SCR_lower(arg);
        if(strcmp(name, command) == 0 && strcmp(parm, arg) == 0) {
            rc = 0;
            goto done;
        }
    }
 done:
    SW_nfree(line);
}


*/



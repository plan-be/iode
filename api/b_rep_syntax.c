/**
 *  @header4iode
 *  
 *  IODE report available commands and @functions
 *  ---------------------------------------------
 *  
 *  Definition of the global tables:
 *      - B_fns[] that lists all report $commands and #commands names and pointers.
 *      - RP_FNS[] that lists all report @functions names and pointers.
 *  
 *  To add a new $command to IODE reports, add a new line in B_fns[] with the name in lowercase
 *  and the function pointer(s).
 *  
 *  To add a new @function to IODE reports, add a new line in RP_FNS[] with the name in lowercase
 *  and the function pointer.
 *  
 *  List of global variables
 *  ------------------------
 *      BFNS B_fns[]    Names of the report commands + GUI and non-GUI function pointers + type defining the allowed suffixes).
 *      RPFN RP_FNS[]   Names of the report @functions and their function pointers
 */

#include "iode.h"

/* 
    Names of the report $commands + called functions in non GUI and GUI modes + type (allowed suffixes).

    Possible values of of B_fns[].type: 
        0 = the command has no suffix allowed (eg. goto)
        1 = a suffix is required and must be an object type => $DataUpdateIdt: name = DataUpdateIdt
        3 = for $command: a suffix is required and must be an object type => $WsLoadEqs: name = WsLoadEqs
            for #command: a suffix is not required  => #WsLoad: name = WsLoad
        4 = for $command: suffix required = file type => $FileRenameTxt : name = FileRenameTxt
            for #command: does not exist => #FileRename

*/

BFNS B_fns[] = {

    // NOMS                     FN NON FSCREEN          FN FULLSCREEN       TYPE   
    // Report internal fns 
    "label",                    RP_label,               RP_label,           0,
    "goto",                     RP_goto,                RP_goto,            0,
    "ask",                      RP_ask,                 RP_ask,             0,
    "abort",                    RP_abort,               RP_abort,           0,
    "quitode",                  RP_quitode,             RP_quitode,         0,
    "quit",                     RP_quitode,             RP_quitode,         0,     /* JMP 16-03-11 */
    "onerror",                  RP_onerror,             RP_onerror,         0,
    "return",                   RP_return,              RP_return,          0,
    "show",                     RP_message,             RP_message,         0,
    "msg",                      RP_warning,             RP_warning,         0,
    "beep",                     RP_beep,                RP_beep,            0,
    "system",                   RP_system,              RP_system,          0,
    "chdir",                    RP_chdir,               RP_chdir,           0,
    "mkdir",                    RP_mkdir,               RP_mkdir,           0,
    "rmdir",                    RP_rmdir,               RP_rmdir,           0,
    "settime",                  RP_settime,             RP_settime,         0,
    "incrtime",                 RP_incrtime,            RP_incrtime,        0,
    "shift",                    RP_shift_args,          RP_shift_args,      0,
    "shift_args",               RP_shift_args,          RP_shift_args,      0,
    "define",                   RP_define,              RP_define,          0,
    "undef",                    RP_undef,               RP_undef,           0, // JMP 22/10/2022
    "undefine",                 RP_undef,               RP_undef,           0, // JMP 22/10/2022
    "debug",                    RP_setdebug,            RP_setdebug,        0, /* JMP 06-08-98 */
    "indent",                   RP_setindent,           RP_setindent,       0, /* JMP 18-02-13 */
    "multiline",                RP_setmultiline,        RP_setmultiline,    0, /* JMP 15-10-22 */
    "noparsing",                RP_noparsing,           RP_noparsing,       0, /* JMP 15-10-22 */
    "shellexec",                B_shellexec,            B_shellexec,        0, /* JMP 02-02-99 */
    "repeat",                   RP_repeat,              RP_repeat,          0, /* JMP 29-04-99 */
    "repeatstring",             RP_repeatstring,        RP_repeatstring,    0, /* JMP 29-04-99 */
    "vseps",                    RP_vseps,               RP_vseps,           0, /* JMP 12-12-12 */
    "foreach",                  RP_foreach,             RP_foreach,         0, /* JMP 13-12-12 */
    "next",                     RP_foreach_next,        RP_foreach_next,    0, /* JMP 13-12-12 */
    "procdef",                  RP_procdef,             RP_procdef,         0, /* JMP 14-2-13 */
    "procexec",                 RP_procexec,            RP_procexec,        0, /* JMP 14-2-13 */
    
    // Report commands 
    "fileimportvar",            B_FileImportVar,        SB_XodeRuleImport,  0,
    "fileimportcmt",            B_FileImportCmt,        SB_XodeRuleImport,  0,
    "filedelete",               B_FileDelete,           NULL,               4,
    "filerename",               B_FileRename,           NULL,               4,
    "filecopy",                 B_FileCopy,             NULL,               4,
    "wssample",                 B_WsSample,             SB_WsSample,        0,
    "wsload",                   B_WsLoad,               SB_WsLoad,          3,
    "wscopy",                   B_WsCopy,               SB_WsCopy,          3,
    "wssave",                   B_WsSave,               SB_WsSave,          3,
    "wssavecmp",                B_WsSaveCmp,            SB_WsSave,          3,
    "wsimport",                 B_WsImport,             SB_WsLoad,          3,
    "wsexport",                 B_WsExport,             SB_WsSave,          3,
    "wsmerge",                  B_WsMerge,              SB_WsMerge,         3,
    "wsclear",                  B_WsClear,              SB_WsClear,         3,
    "wsclearall",               B_WsClearAll,           NULL,               0,
    "wsdescr",                  B_WsDescr,              SB_WsDescr,         0,
    "wsextrapolate",            B_WsExtrapolate,        SB_WsExtrapolate,   0,
    "wsaggrchar",               B_WsAggrChar,           NULL,               0,
    "wsaggrsum",                B_WsAggrSum,            SB_WsAggregate,     0,
    "wsaggrmean",               B_WsAggrMean,           SB_WsAggregate,     0,
    "wsaggrprod",               B_WsAggrProd,           SB_WsAggregate,     0,
    "wshtollast",               B_WsHtoLLast,           SB_WsHtoL,          0,
    "wshtolsum",                B_WsHtoLSum,            SB_WsHtoL,          0,
    "wshtolmean",               B_WsHtoLMean,           SB_WsHtoL,          0,
    "wsltohflow",               B_WsLtoHFlow,           SB_WsLtoH,          0,
    "wsltohstock",              B_WsLtoHStock,          SB_WsLtoH,          0,
    "wsseasonadj",              B_WsSeasonAdj,          SB_WsSeasonAdj,     0,
    "wstrend",                  B_WsTrend,              SB_WsTrend,         0,
    "wstrendstd",               B_WsTrendStd,           SB_WsTrend,         0,
    
    "wsimporteviews",			B_WsImportEviews,       NULL, 		        0,	// JMP 4/5/2012

    "dataedit",                 NULL,                   SB_DataEditScroll,  1,
    "dataupdate",               B_DataUpdate,           NULL,               1,
//  "dataupdateblock",          B_DataUpdateBlock,      NULL,               1,
    "dataexist",                B_DataExist,            NULL,               1,
    "dataappend",               B_DataAppend,           NULL,               1,
    "datacreate",               B_DataCreate,           NULL,               1,
    "datadelete",               B_DataDelete,           NULL,               1,
    "datarename",               B_DataRename,           NULL,               1,
    "datasearch",               B_DataSearch,           SB_DataSearch,      1,
    "dataduplicate",            B_DataDuplicate,        SB_DataDuplicate,   1,
    "datalist",                 B_DataList,             SB_DataList,        1,
    "datacompare",              B_DataCompare,          SB_DataCompare,     1,
    "datacompareeps",           B_DataCompareEps,       SB_DataCompare,     0,
    "datalistsort",             B_DataListSort,         SB_DataListSort,    0,
    "datadisplaygraph",         B_DataDisplayGraph,     SB_DataEditGraph,   0,
    "dataprintgraph",           B_DataPrintGraph,       SB_DataEditGraph,   0,
    "dataeditcnf",              B_DataEditCnf,          NULL/*SB_DataEditCnf*/,   0,
    "datacalcvar",              B_DataCalcVar,          NULL,               0,
    "datacalclst",              B_DataCalcLst,          SB_DataCalcLst,     0,
    "datarasvar",               B_DataRasVar,           NULL,               0,            /* GB 12/2004 */
    "datascan",                 B_DataScan,             SB_DataScan,        1,
    "datapattern",              B_DataPattern,          NULL,               1,  /* GB 01/2005 */
    
    "excelget",                 B_ExcelGet,             NULL,               1,
    "excellang",                B_ExcelLang,            NULL,               0,
    "exceldecimal",             B_ExcelDecimal,         NULL,               0,
    "excelthousand",            B_ExcelThousand,        NULL,               0,
    "excelcurrency",            B_ExcelCurrency,        NULL,               0,
    "excelset",                 B_ExcelSet,             NULL,               1,
    "excelexecute",             B_ExcelExecute,         NULL,               0,
    "excelopen",                B_ExcelOpen,            NULL,               0,
    "excelnew",                 B_ExcelNew,             NULL,               0,
    "excelclose",               B_ExcelClose,           NULL,               0,
    "excelprint",               B_ExcelPrint,           NULL,               0,
    "excelsave",                B_ExcelSave,            NULL,               0,
    "excelsaveas",              B_ExcelSaveAs,          NULL,               0,
    "excelwrite",               B_ExcelWrite,           NULL,               0,
        
    "dsimportdb",               B_DSImportDb,           NULL,               0,
        
    "statunitroot",             B_StatUnitRoot,         SB_StatUnitRoot,    0,
        
    "datawidthvar",             B_ScrollVarW,           NULL,               0,
    "datandecvar",              B_ScrollVarN,           NULL,               0,
    "datamodevar",              B_ScrollVarM,           NULL,               0,
    "datastartvar",             B_ScrollVarS,           NULL,               0,
    "datawidthtbl",             B_ScrollTblW,           NULL,               0,
    "datawidthscl",             B_ScrollSclW,           NULL,               0,
    "datandecscl",              B_ScrollSclN,           NULL,               0,
    "viewwidth",                B_ScrollVTW,            NULL,               0,
    "viewwidth0",               B_ScrollVTW0,           NULL,               0,
    "viewndec",                 B_ScrollVTN,            NULL,               0,
    "printobjtitle",            B_PrintObjTblTitle,     NULL,               0,
    "printobjlec",              B_PrintObjLec,          NULL,               0,
    "printobjinfos",            B_PrintObjEqsInfos,     NULL,               0,
        
    "printobjdef",              B_PrintObjDef,          SB_PrintObjDef,     1,
    "printdest",                B_PrintDest,            NULL,               0,
    "printdestnew",             B_PrintDestNew,         NULL,               0,
    "printmulti",               B_PrintMulti,           NULL,               0,
    "printnbdec",               B_PrintNbDec,           NULL,               0,
    "printlang",                B_PrintLang,            NULL,               0,
    "printtblfile",             B_ViewTblFile,          NULL,               0,
    "printtbl",                 B_PrintTbl,             SB_ViewPrintTbl,    0,
    "printvar",                 B_PrintVar,             NULL,               0,
            
    "viewtblfile",              B_ViewTblFile,          NULL,               0,
    "viewtbl",                  B_ViewTbl,              SB_ViewPrintTbl,    0,
    "viewvar",                  B_ViewVar,              NULL,               0,
    "viewbytbl",                B_ViewByTbl,            SB_ViewByTbl,       0,
            
    "viewgr",                   B_ViewGr,               SB_ViewPrintGr,     0,
//  "printgrall",               B_PrintGrAll,           SB_ViewPrintGr,     0,
//  "printgrdata",              B_PrintGrData,          SB_ViewPrintGr,     0,
//  "printgrwin",               B_PrintGrWin,           SB_ViewPrintGr,     0,


    "graphdefault",             B_GraphDefault,         NULL,               0,
    
    "printgrall",               B_PrintGr,              SB_ViewPrintGr,     0,
    "printgr",                  B_PrintGr,              SB_ViewPrintGr,     0,
    
    "modelsimulate",            B_ModelSimulate,        SB_ModelSimulate,   0,
    "modelsimulateparms",       B_ModelSimulateParms,   NULL,               0,
    "modelexchange",            B_ModelExchange,        NULL,               0,
    "modelcompile",             B_ModelCompile, 	    SB_ModelCompile,    0,
    
    "modelcalcscc",             B_ModelCalcSCC,         NULL,               0,
    "modelsimulatescc",         B_ModelSimulateSCC,     NULL,               0,

    "modelsimulatesaveniters",  B_ModelSimulateSaveNIters,  NULL,           0,
    "modelsimulatesavenorms",   B_ModelSimulateSaveNorms,   NULL,           0,


    "idtexecute",               B_IdtExecute,           SB_IdtExecute,      0,
    "idtexecutevarfiles",       B_IdtExecuteVarFiles,   NULL,               0,
    "idtexecutesclfiles",       B_IdtExecuteSclFiles,   NULL,               0,
    "idtexecutetrace",          B_IdtExecuteTrace,      NULL,               0,
    
    "eqsestimate",              B_EqsEstimate,          SB_EqsEstimate,     0,
    "eqsstepwise",              B_EqsStepWise,          NULL,               0,
    "eqssetmethod",             B_EqsSetMethod,         NULL,               0,
    "eqssetbloc",               B_EqsSetBloc,           NULL,               0,
    "eqssetblock",              B_EqsSetBloc,           NULL,               0,
    "eqssetsample",             B_EqsSetSample,         NULL,               0,
    "eqssetinstrs",             B_EqsSetInstrs,         NULL,               0,
    "eqssetcmt",                B_EqsSetCmt,            NULL,               0,
    
    "reportexec",               B_ReportExec,           SB_ReportExec,      0,
    //"reportedit",               B_FileEdit,           SB_ReportEdit,      0,
    "reportedit",               NULL,                   SB_ReportEdit,      0,
    "prompt",                   NULL,                   SB_ReportPrompt,    0,
    "minimize",                 B_WindowMinimize,       NULL,               0,
    "maximize",                 B_WindowMaximize,       NULL,               0,
    "sleep",                    B_Sleep,                NULL,               0,
        
    "printa2mappend",           B_PrintA2mAppend,       NULL,               0,
    "printfont",                B_PrintFont,            NULL,               0,
    "printtablefont",           B_PrintTFont,           NULL,               0,
    "printtablebox",            B_PrintTBox,            NULL,               0,
    "printtablecolor",          B_PrintTColor,          NULL,               0,
    "printtablewidth",          B_PrintTWidth,          NULL,               0,
    "printtablebreak",          B_PrintTBreak,          NULL,               0,
    "printtablepage",           B_PrintTPage,           NULL,               0,

    "printhtmltableclass",      B_PrintHtmlTableClass,  NULL,               0,
    "printhtmltrclass",         B_PrintHtmlTRClass,     NULL,               0,
    "printhtmlthclass",         B_PrintHtmlTHClass,     NULL,               0,
    "printhtmltdclass",         B_PrintHtmlTDClass,     NULL,               0,

    "printbackground",          B_PrintGColor,          NULL,               0,
    "printgraphbox",            B_PrintGBox,            NULL,               0,
    "printgraphbrush",          B_PrintGBrush,          NULL,               0,
    "printgraphsize",           B_PrintGSize,           NULL,               0,
    "printgraphpage",           B_PrintGPage,           NULL,               0,
    "printgraphtheme",          B_PrintGTheme,          NULL,               0,
    "printgraphband",           B_PrintGBand,           NULL,               0,
            
    "printrtfhelp",             B_PrintRtfHelp,         NULL,               0,
    "printhtmlhelp",            B_PrintHtmlHelp,        NULL,               0,
    "printrtftopic",            B_PrintRtfTopic,        NULL,               0,
    "printrtflevel",            B_PrintRtfLevel,        NULL,               0,
    "printrtftitle",            B_PrintRtfTitle,        NULL,               0,
    "printrtfcopyright",        B_PrintRtfCopy,         NULL,               0,
    "printparanum",             B_PrintParaNum,         NULL,               0,
    "printpageheader",          B_PrintPageHeader,      NULL,               0,
    "printpagefooter",          B_PrintPageFooter,      NULL,               0,
    "printorientation",         B_PrintGdiOrient,       NULL,               0,
    "printduplex",              B_PrintGdiDuplex,       NULL,               0,
    "setprinter",               B_PrintGdiPrinter,      NULL,               0,
    
    "printgiftranscolor",       B_PrintGIFTransColor,   NULL,               0, /* JMP 26-08-98 */
    "printgifbackcolor",        B_PrintGIFBackColor,    NULL,               0,
    "printgifinterlaced",       B_PrintGIFInterlaced,   NULL,               0,
    "printgiftransparent",      B_PrintGIFTransparent,  NULL,               0,
    "printgiffilled",           B_PrintGIFFilled,       NULL,               0,
    "printgiffont",             B_PrintGIFFont,         NULL,               0,
    
    "printhtmlstrip",           B_PrintHtmlStrip,       NULL,               0,
    "printhtmlstyle",           B_PrintHtmlStyle,       NULL,               0,
            
    "ddeget",                   B_DDEGet,               NULL,               0,
    "sysmovefile",              B_SysRename,            NULL,               0,
    "syscopyfile",              B_SysCopy,              NULL,               0,
    "sysappendfile",            B_SysAppend,            NULL,               0,
    "sysdeletefile",            B_SysDelete,            NULL,               0,
    "sysansitoutf8",            B_SysAnsiToUTF8,        NULL,               0,
    "sysoemtoutf8",             B_SysOemToUTF8,         NULL,               0,
    "sysansitooem",             B_SysAnsiToOem,         NULL,               0,
    "sysoemtoansi",             B_SysOemToAnsi,         NULL,               0,
                
    "a2mtohtml",                B_A2mToHtml,            NULL,               0,
    "a2mtomif",                 B_A2mToMif,             NULL,               0,
    "a2mtocsv",                 B_A2mToCsv,             NULL,               0,
    "a2mtortf",                 B_A2mToRtf,             NULL,               0,
    "a2mtoprinter",             B_A2mToPrinter,         NULL,               0,
                
    "csvsave",                  B_CsvSave,              NULL,               3, // JMP 2-3-2016
    "csvdigits",                B_CsvNbDec,             NULL,               0, // JMP 2-3-2016
    "csvsep",                   B_CsvSep,               NULL,               0, // JMP 2-3-2016
    "csvdec",                   B_CsvDec,               NULL,               0, // JMP 2-3-2016
    "csvnan",                   B_CsvNaN,               NULL,               0, // JMP 2-3-2016
    "csvaxes",                  B_CsvAxes,              NULL,               0, // GB 23-4-2016

    0, 0, 0, 0
};

// ========= Definition of report @-functions =========

RPFN RP_FNS[] = {
    "upper",        RPF_upper,
    "date",         RPF_date,
    "time",         RPF_time,
    "lower",        RPF_lower,
    
    "take",         RPF_take,                /* JMP 18-12-01 */
    "drop",         RPF_drop,                /* JMP 18-12-01 */
    "count",        RPF_count,               /* JMP 18-12-01 */
    "index",        RPF_index,               /* JMP 18-12-01 */
    "fmt",          RPF_fmtint,              /* JMP 06-11-2003 */
    "sqz",          RPF_sqz,                 /* JMP 10-11-2004 */
    "strip",        RPF_strip,               /* JMP 10-11-2004 */
    
    "ttitle",       RPF_ttitle,
    "cvalue",       RPF_cvalue,
    "lvalue",       RPF_lvalue,
    "vvalue",       RPF_vvalue,
    "sample",       RPF_sample,
    "replace",      RPF_replace,
    "month",        RPF_month,
    "ansi",         RPF_ansi,
    "equal",        RPF_equal,
    
    "ivalue",       RPF_ivalue,              /* JMP 18-12-01 */
    
    "evalue",       RPF_evalue,              /* GB  14-02-05 */

    "eqsample",     RPF_eqsample,      /* JMP 27-07-11 */
    "eqsamplefrom", RPF_eqsamplefrom,  /* JMP 27-07-11 */
    "eqsampleto",   RPF_eqsampleto,    /* JMP 27-07-11 */
    "eqlhs",        RPF_eqlhs,         /* JMP 27-07-11 */
    "eqrhs",        RPF_eqrhs,         /* JMP 27-07-11 */

    "vliste",       RPF_vliste,
    "sliste",       RPF_sliste,
    
    "cexpand",      RPF_cexpand,
    "eexpand",      RPF_eexpand,
    "iexpand",      RPF_iexpand,
    "lexpand",      RPF_lexpand,
    "sexpand",      RPF_sexpand,
    "texpand",      RPF_texpand,
    "vexpand",      RPF_vexpand,
    
    "fappend",      RPF_fappend,
    "fdelete",      RPF_fdelete,
    
    "sstderr",      RPF_sstderr,
    
    "sqlopen",      RPS_Open,
    "sqlquery",     RPS_Query,
    "sqlnext",      RPS_Next,
    "sqlrecord",    RPS_Record,
    "sqlfield",     RPS_Field,
    "sqlnbflds",    RPS_NbFlds,
    "sqlclose",     RPS_Close,
    
    "srelax",       RPF_srelax,
    
    "memory",       RPF_memory,

    "chronoreset",  RPF_ChronoReset,
    "chronoget",    RPF_ChronoGet,

    "simeps",		RPF_SimEps,
    "simmaxit",		RPF_SimMaxit,
    "simrelax",		RPF_SimRelax,
    "simnorm",		RPF_SimNorm,
    "simniter",		RPF_SimNIter,

    //"vseps",		RPF_vseps,
    "vtake",		RPF_vtake,
    "vdrop",		RPF_vdrop,
    "vcount",		RPF_vcount,

    "getdir",       RPF_getdir, // 9/4/2019
    "chdir",        RPF_chdir,  // 9/4/2019
    "mkdir",        RPF_mkdir,  // 9/4/2019
    "rmdir",        RPF_rmdir,  // 9/4/2019

    "void",         RPF_void,   // 9/4/2019
    0, 0
};


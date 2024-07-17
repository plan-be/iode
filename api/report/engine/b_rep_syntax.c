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

    Values of B_fns[].group are used to defined groups for the color highlighting.

*/

BFNS B_fns[] = {

    // NAMES                  CamelCase NAMES             FN NON GUI              FN GUI              TYPE      GROUP   
    // Report internal fns 
    "label",                  "label",                    RP_label,               RP_label,           0,        0,
    "goto",                   "goto",                     RP_goto,                RP_goto,            0,        0,
    "ask",                    "ask",                      RP_ask,                 RP_ask,             0,        0,
    "abort",                  "abort",                    RP_abort,               RP_abort,           0,        0,
    "quitode",                "quitode",                  RP_quitode,             RP_quitode,         0,        0,
    "quit",                   "quit",                     RP_quitode,             RP_quitode,         0,        0,     /* JMP 16-03-11 */
    "onerror",                "onerror",                  RP_onerror,             RP_onerror,         0,        0,
    "return",                 "return",                   RP_return,              RP_return,          0,        0,
    "show",                   "show",                     RP_message,             RP_message,         0,        0,
    "silent",                 "silent",                   RP_silent,              RP_silent,          0,        0,
    //"showoff",                "showoff",                  RP_message_off,         RP_message_off,     0,        0,
    "msg",                    "msg",                      RP_warning,             RP_warning,         0,        0,
    "beep",                   "beep",                     RP_beep,                RP_beep,            0,        0,
    "system",                 "system",                   RP_system,              RP_system,          0,        0,
    "chdir",                  "chdir",                    RP_chdir,               RP_chdir,           0,        0,
    "mkdir",                  "mkdir",                    RP_mkdir,               RP_mkdir,           0,        0,
    "rmdir",                  "rmdir",                    RP_rmdir,               RP_rmdir,           0,        0,
    "settime",                "settime",                  RP_settime,             RP_settime,         0,        0,
    "incrtime",               "incrtime",                 RP_incrtime,            RP_incrtime,        0,        0,
    "shift",                  "shift",                    RP_shift_args,          RP_shift_args,      0,        0,
    "shift_args",             "shift_args",               RP_shift_args,          RP_shift_args,      0,        0,
    "define",                 "define",                   RP_define,              RP_define,          0,        0,
    "undef",                  "undef",                    RP_undef,               RP_undef,           0,        0, // JMP 22/10/2022
    "undefine",               "undefine",                 RP_undef,               RP_undef,           0,        0, // JMP 22/10/2022
    "debug",                  "debug",                    RP_setdebug,            RP_setdebug,        0,        0, /* JMP 06-08-98 */
    "indent",                 "indent",                   RP_setindent,           RP_setindent,       0,        0, /* JMP 18-02-13 */
    "multiline",              "multiline",                RP_setmultiline,        RP_setmultiline,    0,        0, /* JMP 15-10-22 */
    "noparsing",              "noparsing",                RP_noparsing,           RP_noparsing,       0,        0, /* JMP 15-10-22 */
    "shellexec",              "shellexec",                B_shellexec,            B_shellexec,        0,        0, /* JMP 02-02-99 */
    "repeat",                 "repeat",                   RP_repeat,              RP_repeat,          0,        0, /* JMP 29-04-99 */
    "repeatstring",           "repeatstring",             RP_repeatstring,        RP_repeatstring,    0,        0, /* JMP 29-04-99 */
    "vseps",                  "vseps",                    RP_vseps,               RP_vseps,           0,        0, /* JMP 12-12-12 */
    "foreach",                "foreach",                  RP_foreach,             RP_foreach,         0,        0, /* JMP 13-12-12 */
    "next",                   "next",                     RP_foreach_next,        RP_foreach_next,    0,        0, /* JMP 13-12-12 */
    "procdef",                "procdef",                  RP_procdef,             RP_procdef,         0,        0, /* JMP 14-2-13 */
    "procexec",               "procexec",                 RP_procexec,            RP_procexec,        0,        0, /* JMP 14-2-13 */

    // Report commands
    "fileimportvar",          "FileImportVar",            B_FileImportVar,        SB_XodeRuleImport,  0,        1,
    "fileimportcmt",          "FileImportCmt",            B_FileImportCmt,        SB_XodeRuleImport,  0,        1,
    "filedelete",             "FileDelete",               B_FileDelete,           NULL,               4,        1,
    "filerename",             "FileRename",               B_FileRename,           NULL,               4,        1,
    "filecopy",               "FileCopy",                 B_FileCopy,             NULL,               4,        1,
    "wssample",               "WsSample",                 B_WsSample,             SB_WsSample,        0,        1,
    "wsload",                 "WsLoad",                   B_WsLoad,               SB_WsLoad,          3,        1,
    "wscopy",                 "WsCopy",                   B_WsCopy,               SB_WsCopy,          3,        1,
    "wssave",                 "WsSave",                   B_WsSave,               SB_WsSave,          3,        1,
    "wssavecmp",              "WsSaveCmp",                B_WsSaveCmp,            SB_WsSave,          3,        1,
    "wsimport",               "WsImport",                 B_WsImport,             SB_WsLoad,          3,        1,
    "wsexport",               "WsExport",                 B_WsExport,             SB_WsSave,          3,        1,
    "wsmerge",                "WsMerge",                  B_WsMerge,              SB_WsMerge,         3,        1,
    "wsclear",                "WsClear",                  B_WsClear,              SB_WsClear,         3,        1,
    "wsclearall",             "WsClearAll",               B_WsClearAll,           NULL,               0,        1,
    "wsdescr",                "WsDescr",                  B_WsDescr,              SB_WsDescr,         0,        1,
    "wsextrapolate",          "WsExtrapolate",            B_WsExtrapolate,        SB_WsExtrapolate,   0,        1,
    "wsaggrchar",             "WsAggrChar",               B_WsAggrChar,           NULL,               0,        1,
    "wsaggrsum",              "WsAggrSum",                B_WsAggrSum,            SB_WsAggregate,     0,        1,
    "wsaggrmean",             "WsAggrMean",               B_WsAggrMean,           SB_WsAggregate,     0,        1,
    "wsaggrprod",             "WsAggrProd",               B_WsAggrProd,           SB_WsAggregate,     0,        1,
    "wshtollast",             "WsHtoLLast",               B_WsHtoLLast,           SB_WsHtoL,          0,        1,
    "wshtolsum",              "WsHtoLSum",                B_WsHtoLSum,            SB_WsHtoL,          0,        1,
    "wshtolmean",             "WsHtoLMean",               B_WsHtoLMean,           SB_WsHtoL,          0,        1,
    "wsltohflow",             "WsLtoHFlow",               B_WsLtoHFlow,           SB_WsLtoH,          0,        1,
    "wsltohstock",            "WsLtoHStock",              B_WsLtoHStock,          SB_WsLtoH,          0,        1,
    "wsseasonadj",            "WsSeasonAdj",              B_WsSeasonAdj,          SB_WsSeasonAdj,     0,        1,
    "wstrend",                "WsTrend",                  B_WsTrend,              SB_WsTrend,         0,        1,
    "wstrendstd",             "WsTrendStd",               B_WsTrendStd,           SB_WsTrend,         0,        1,
    
    "wsimporteviews",		  "WsImportEviews",			  B_WsImportEviews,       NULL, 		      0,        1,	// JMP 4/5/2012

    "dataedit",               "DataEdit",                 NULL,                   SB_DataEditScroll,  1,        1,
    "dataupdate",             "DataUpdate",               B_DataUpdate,           NULL,               1,        1,
//  "dataupdateblock",        "DataUpdateBlock",          B_DataUpdateBlock,      NULL,               1,        1,
    "dataexist",              "DataExist",                B_DataExist,            NULL,               1,        1,
    "dataappend",             "DataAppend",               B_DataAppend,           NULL,               1,        1,
    "datacreate",             "DataCreate",               B_DataCreate,           NULL,               1,        1,
    "datadelete",             "DataDelete",               B_DataDelete,           NULL,               1,        1,
    "datarename",             "DataRename",               B_DataRename,           NULL,               1,        1,
    "datasearch",             "DataSearch",               B_DataSearch,           SB_DataSearch,      1,        1,
    "dataduplicate",          "DataDuplicate",            B_DataDuplicate,        SB_DataDuplicate,   1,        1,
    "datalist",               "DataList",                 B_DataList,             SB_DataList,        1,        1,
    "datacompare",            "DataCompare",              B_DataCompare,          SB_DataCompare,     1,        1,
    "datacompareeps",         "DataCompareEps",           B_DataCompareEps,       SB_DataCompare,     0,        1,
    "datalistsort",           "DataListSort",             B_DataListSort,         SB_DataListSort,    0,        1,
    "datadisplaygraph",       "DataDisplayGraph",         B_DataDisplayGraph,     SB_DataEditGraph,   0,        1,
    "dataprintgraph",         "DataPrintGraph",           B_DataPrintGraph,       SB_DataEditGraph,   0,        1,
    "dataeditcnf",            "DataEditCnf",              B_DataEditCnf,          NULL/*SB_DataEditCnf*/,   0,  1,
    "datacalcvar",            "DataCalcVar",              B_DataCalcVar,          NULL,               0,        1,
    "datacalclst",            "DataCalcLst",              B_DataCalcLst,          SB_DataCalcLst,     0,        1,
    "datarasvar",             "DataRasVar",               B_DataRasVar,           NULL,               0,        1,          /* GB 12/2004 */
    "datascan",               "DataScan",                 B_DataScan,             SB_DataScan,        1,        1,
    "datapattern",            "DataPattern",              B_DataPattern,          NULL,               1,        1,          /* GB 01/2005 */
    
#ifndef __GNUC__
    "excelget",               "ExcelGet",                 B_ExcelGet,             NULL,               1,        1,
    "excellang",              "ExcelLang",                B_ExcelLang,            NULL,               0,        1,
    "exceldecimal",           "ExcelDecimal",             B_ExcelDecimal,         NULL,               0,        1,
    "excelthousand",          "ExcelThousand",            B_ExcelThousand,        NULL,               0,        1,
    "excelcurrency",          "ExcelCurrency",            B_ExcelCurrency,        NULL,               0,        1,
    "excelset",               "ExcelSet",                 B_ExcelSet,             NULL,               1,        1,
    "excelexecute",           "ExcelExecute",             B_ExcelExecute,         NULL,               0,        1,
    "excelopen",              "ExcelOpen",                B_ExcelOpen,            NULL,               0,        1,
    "excelnew",               "ExcelNew",                 B_ExcelNew,             NULL,               0,        1,
    "excelclose",             "ExcelClose",               B_ExcelClose,           NULL,               0,        1,
    "excelprint",             "ExcelPrint",               B_ExcelPrint,           NULL,               0,        1,
    "excelsave",              "ExcelSave",                B_ExcelSave,            NULL,               0,        1,
    "excelsaveas",            "ExcelSaveAs",              B_ExcelSaveAs,          NULL,               0,        1,
    "excelwrite",             "ExcelWrite",               B_ExcelWrite,           NULL,               0,        1,
#endif  

    "dsimportdb",             "DSImportDb",               B_DSImportDb,           NULL,               0,        1,
   
    "statunitroot",           "StatUnitRoot",             B_StatUnitRoot,         SB_StatUnitRoot,    0,        1,
     
    "datawidthvar",           "ScrollVarW",               B_ScrollVarW,           NULL,               0,        1,
    "datandecvar",            "ScrollVarN",               B_ScrollVarN,           NULL,               0,        1,
    "datamodevar",            "ScrollVarM",               B_ScrollVarM,           NULL,               0,        1,
    "datastartvar",           "ScrollVarS",               B_ScrollVarS,           NULL,               0,        1,
    "datawidthtbl",           "ScrollTblW",               B_ScrollTblW,           NULL,               0,        1,
    "datawidthscl",           "ScrollSclW",               B_ScrollSclW,           NULL,               0,        1,
    "datandecscl",            "ScrollSclN",               B_ScrollSclN,           NULL,               0,        1,
    "viewwidth",              "ScrollVTW",                B_ScrollVTW,            NULL,               0,        1,
    "viewwidth0",             "ScrollVTW0",               B_ScrollVTW0,           NULL,               0,        1,
    "viewndec",               "ScrollVTN",                B_ScrollVTN,            NULL,               0,        1,
    "printobjtitle",          "PrintObjTblTitle",         B_PrintObjTblTitle,     NULL,               0,        1,
    "printobjlec",            "PrintObjLec",              B_PrintObjLec,          NULL,               0,        1,
    "printobjinfos",          "PrintObjEqsInfos",         B_PrintObjEqsInfos,     NULL,               0,        1,
    
    "printobjdef",            "PrintObjDef",              B_PrintObjDef,          SB_PrintObjDef,     1,        1,
    "printdest",              "PrintDest",                B_PrintDest,            NULL,               0,        1,
    "printdestnew",           "PrintDestNew",             B_PrintDestNew,         NULL,               0,        1,
    "printmulti",             "PrintMulti",               B_PrintMulti,           NULL,               0,        1,
    "printnbdec",             "PrintNbDec",               B_PrintNbDec,           NULL,               0,        1,
    "printlang",              "PrintLang",                B_PrintLang,            NULL,               0,        1,
    "printtblfile",           "ViewTblFile",              B_ViewTblFile,          NULL,               0,        1,
    "printtbl",               "PrintTbl",                 B_PrintTbl,             SB_ViewPrintTbl,    0,        1,
    "printvar",               "PrintVar",                 B_PrintVar,             NULL,               0,        1,
      
    "viewtblfile",            "ViewTblFile",              B_ViewTblFile,          NULL,               0,        1,
    "viewtbl",                "ViewTbl",                  B_ViewTbl,              SB_ViewPrintTbl,    0,        1,
    "viewvar",                "ViewVar",                  B_ViewVar,              NULL,               0,        1,
    "viewbytbl",              "ViewByTbl",                B_ViewByTbl,            SB_ViewByTbl,       0,        1,
     
    "viewgr",                 "ViewGr",                   B_ViewGr,               SB_ViewPrintGr,     0,        1,
//  "printgrall",             "PrintGrAll",               B_PrintGrAll,           SB_ViewPrintGr,     0,        1,
//  "printgrdata",            "PrintGrData",              B_PrintGrData,          SB_ViewPrintGr,     0,        1,
//  "printgrwin",             "PrintGrWin",               B_PrintGrWin,           SB_ViewPrintGr,     0,        1,

    "graphdefault",           "GraphDefault",             B_GraphDefault,         NULL,               0,        1,
    
    "printgrall",             "PrintGrAll",               B_PrintGr,              SB_ViewPrintGr,     0,        1,
    "printgr",                "PrintGr",                  B_PrintGr,              SB_ViewPrintGr,     0,        1,
    
    "modelsimulate",          "ModelSimulate",            B_ModelSimulate,        SB_ModelSimulate,   0,        1,
    "modelsimulateparms",     "ModelSimulateParms",       B_ModelSimulateParms,   NULL,               0,        1,
    "modelexchange",          "ModelExchange",            B_ModelExchange,        NULL,               0,        1,
    "modelcompile",           "ModelCompile",             B_ModelCompile, 	      SB_ModelCompile,    0,        1,
    
    "modelcalcscc",           "ModelCalcSCC",             B_ModelCalcSCC,         NULL,               0,        1,
    "modelsimulatescc",       "ModelSimulateSCC",         B_ModelSimulateSCC,     NULL,               0,        1,

    "modelsimulatesaveniters", "ModelSimulateSaveNIters",  B_ModelSimulateSaveNIters,  NULL,          0,        1,
    "modelsimulatesavenorms",  "ModelSimulateSaveNorms",   B_ModelSimulateSaveNorms,   NULL,          0,        1,

    "idtexecute",             "IdtExecute",               B_IdtExecute,           SB_IdtExecute,      0,        1,
    "idtexecutevarfiles",     "IdtExecuteVarFiles",       B_IdtExecuteVarFiles,   NULL,               0,        1,
    "idtexecutesclfiles",     "IdtExecuteSclFiles",       B_IdtExecuteSclFiles,   NULL,               0,        1,
    "idtexecutetrace",        "IdtExecuteTrace",          B_IdtExecuteTrace,      NULL,               0,        1,
    
    "eqsestimate",            "EqsEstimate",              B_EqsEstimate,          SB_EqsEstimate,     0,        1,
    "eqsstepwise",            "EqsStepWise",              B_EqsStepWise,          NULL,               0,        1,
    "eqssetmethod",           "EqsSetMethod",             B_EqsSetMethod,         NULL,               0,        1,
    "eqssetbloc",             "EqsSetBloc",               B_EqsSetBloc,           NULL,               0,        1,
    "eqssetblock",            "EqsSetBloc",               B_EqsSetBloc,           NULL,               0,        1,
    "eqssetsample",           "EqsSetSample",             B_EqsSetSample,         NULL,               0,        1,
    "eqssetinstrs",           "EqsSetInstrs",             B_EqsSetInstrs,         NULL,               0,        1,
    "eqssetcmt",              "EqsSetCmt",                B_EqsSetCmt,            NULL,               0,        1,
    
    "reportexec",             "ReportExec",               B_ReportExec,           SB_ReportExec,      0,        1,
    //"reportedit",             "FileEdit",               B_FileEdit,             SB_ReportEdit,      0,        1,
    "reportedit",             "ReportEdit",               NULL,                   SB_ReportEdit,      0,        1,
    "prompt",                 "ReportPrompt",             NULL,                   SB_ReportPrompt,    0,        1,
    "minimize",               "WindowMinimize",           B_WindowMinimize,       NULL,               0,        1,
    "maximize",               "WindowMaximize",           B_WindowMaximize,       NULL,               0,        1,
    "sleep",                  "Sleep",                    B_Sleep,                NULL,               0,        1,
    
    "printa2mappend",         "PrintA2mAppend",           B_PrintA2mAppend,       NULL,               0,        1,
    "printfont",              "PrintFont",                B_PrintFont,            NULL,               0,        1,
    "printtablefont",         "PrintTFont",               B_PrintTFont,           NULL,               0,        1,
    "printtablebox",          "PrintTBox",                B_PrintTBox,            NULL,               0,        1,
    "printtablecolor",        "PrintTColor",              B_PrintTColor,          NULL,               0,        1,
    "printtablewidth",        "PrintTWidth",              B_PrintTWidth,          NULL,               0,        1,
    "printtablebreak",        "PrintTBreak",              B_PrintTBreak,          NULL,               0,        1,
    "printtablepage",         "PrintTPage",               B_PrintTPage,           NULL,               0,        1,

    "printhtmltableclass",    "PrintHtmlTableClass",      B_PrintHtmlTableClass,  NULL,               0,        1,
    "printhtmltrclass",       "PrintHtmlTRClass",         B_PrintHtmlTRClass,     NULL,               0,        1,
    "printhtmlthclass",       "PrintHtmlTHClass",         B_PrintHtmlTHClass,     NULL,               0,        1,
    "printhtmltdclass",       "PrintHtmlTDClass",         B_PrintHtmlTDClass,     NULL,               0,        1,

    "printbackground",        "PrintGColor",              B_PrintGColor,          NULL,               0,        1,
    "printgraphbox",          "PrintGBox",                B_PrintGBox,            NULL,               0,        1,
    "printgraphbrush",        "PrintGBrush",              B_PrintGBrush,          NULL,               0,        1,
    "printgraphsize",         "PrintGSize",               B_PrintGSize,           NULL,               0,        1,
    "printgraphpage",         "PrintGPage",               B_PrintGPage,           NULL,               0,        1,
    "printgraphtheme",        "PrintGTheme",              B_PrintGTheme,          NULL,               0,        1,
    "printgraphband",         "PrintGBand",               B_PrintGBand,           NULL,               0,        1,

    "printrtfhelp",           "PrintRtfHelp",             B_PrintRtfHelp,         NULL,               0,        1,
    "printhtmlhelp",          "PrintHtmlHelp",            B_PrintHtmlHelp,        NULL,               0,        1,
    "printrtftopic",          "PrintRtfTopic",            B_PrintRtfTopic,        NULL,               0,        1,
    "printrtflevel",          "PrintRtfLevel",            B_PrintRtfLevel,        NULL,               0,        1,
    "printrtftitle",          "PrintRtfTitle",            B_PrintRtfTitle,        NULL,               0,        1,
    "printrtfcopyright",      "PrintRtfCopy",             B_PrintRtfCopy,         NULL,               0,        1,
    "printparanum",           "PrintParaNum",             B_PrintParaNum,         NULL,               0,        1,
    "printpageheader",        "PrintPageHeader",          B_PrintPageHeader,      NULL,               0,        1,
    "printpagefooter",        "PrintPageFooter",          B_PrintPageFooter,      NULL,               0,        1,
    "printorientation",       "PrintGdiOrient",           B_PrintGdiOrient,       NULL,               0,        1,
    "printduplex",            "PrintGdiDuplex",           B_PrintGdiDuplex,       NULL,               0,        1,
    "setprinter",             "PrintGdiPrinter",          B_PrintGdiPrinter,      NULL,               0,        1,

    "printgiftranscolor",     "PrintGIFTransColor",       B_PrintGIFTransColor,   NULL,               0,        1, /* JMP 26-08-98 */
    "printgifbackcolor",      "PrintGIFBackColor",        B_PrintGIFBackColor,    NULL,               0,        1,
    "printgifinterlaced",     "PrintGIFInterlaced",       B_PrintGIFInterlaced,   NULL,               0,        1,
    "printgiftransparent",    "PrintGIFTransparent",      B_PrintGIFTransparent,  NULL,               0,        1,
    "printgiffilled",         "PrintGIFFilled",           B_PrintGIFFilled,       NULL,               0,        1,
    "printgiffont",           "PrintGIFFont",             B_PrintGIFFont,         NULL,               0,        1,

    "printhtmlstrip",         "PrintHtmlStrip",           B_PrintHtmlStrip,       NULL,               0,        1,
    "printhtmlstyle",         "PrintHtmlStyle",           B_PrintHtmlStyle,       NULL,               0,        1,

    "ddeget",                 "DDEGet",                   B_DDEGet,               NULL,               0,        1,
    "sysmovefile",            "SysRename",                B_SysRename,            NULL,               0,        1,
    "syscopyfile",            "SysCopy",                  B_SysCopy,              NULL,               0,        1,
    "sysappendfile",          "SysAppend",                B_SysAppend,            NULL,               0,        1,
    "sysdeletefile",          "SysDelete",                B_SysDelete,            NULL,               0,        1,
    "sysansitoutf8",          "SysAnsiToUTF8",            B_SysAnsiToUTF8,        NULL,               0,        1,
    "sysoemtoutf8",           "SysOemToUTF8",             B_SysOemToUTF8,         NULL,               0,        1,
    "sysansitooem",           "SysAnsiToOem",             B_SysAnsiToOem,         NULL,               0,        1,
    "sysoemtoansi",           "SysOemToAnsi",             B_SysOemToAnsi,         NULL,               0,        1,

    "a2mtohtml",              "A2mToHtml",                B_A2mToHtml,            NULL,               0,        1,
    "a2mtomif",               "A2mToMif",                 B_A2mToMif,             NULL,               0,        1,
    "a2mtocsv",               "A2mToCsv",                 B_A2mToCsv,             NULL,               0,        1,
    "a2mtortf",               "A2mToRtf",                 B_A2mToRtf,             NULL,               0,        1,
    "a2mtoprinter",           "A2mToPrinter",             B_A2mToPrinter,         NULL,               0,        1,

    "csvsave",                "CsvSave",                  B_CsvSave,              NULL,               3,        1, // JMP 2-3-2016
    "csvdigits",              "CsvNbDec",                 B_CsvNbDec,             NULL,               0,        1, // JMP 2-3-2016
    "csvsep",                 "CsvSep",                   B_CsvSep,               NULL,               0,        1, // JMP 2-3-2016
    "csvdec",                 "CsvDec",                   B_CsvDec,               NULL,               0,        1, // JMP 2-3-2016
    "csvnan",                 "CsvNaN",                   B_CsvNaN,               NULL,               0,        1, // JMP 2-3-2016
    "csvaxes",                "CsvAxes",                  B_CsvAxes,              NULL,               0,        1, // GB 23-4-2016

    0, 0, 0, 0, 0, 0
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
    "simsortnbpasses", RPF_SimSortNbPasses,
    "simsortalgo",    RPF_SimSortAlgo,
    "siminitvalues", RPF_SimInitValues,

    "simnorm",		RPF_SimNorm,
    "simniter",		RPF_SimNIter,
    "simcpu",		RPF_SimCpu,
    "simcpuscc",    RPF_SimCpuSCC,
    "simcpusort",   RPF_SimCpuSort,
    

    //"vseps",		RPF_vseps,
    "vtake",		RPF_vtake,
    "vdrop",		RPF_vdrop,
    "vcount",		RPF_vcount,

    "getdir",       RPF_getdir, // 9/4/2019
    "chdir",        RPF_chdir,  // 9/4/2019
    "mkdir",        RPF_mkdir,  // 9/4/2019
    "rmdir",        RPF_rmdir,  // 9/4/2019

    "void",         RPF_void,   // 9/4/2019
    "version",      RPF_IodeVersion, 
    
    0, 0
};


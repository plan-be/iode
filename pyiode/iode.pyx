# IODE EXTENSION FOR PYTHON
# =========================
#
#    @header4iode
# 
# PYIODE API
# ==========
# 
# 
# Interface IODE - Python - IODE
# ------------------------------
# During a PYIODE session, IODE data are grouped in workspaces, one by object type (variables, scalars, equations...) 
# and stored in memory using a proprietary memory management system (IODE-MEM).
# 
# IODE functions operate inside IODE-MEM. 
# Exchanges between IODE objects (in IODE-MEM) and LArray objects (python memory) are made possible via interface functions.
# 
# Source files (py and c)
# -----------------------
# The python functions are split according to their specific topics in pyiode_*.pyx where * can be ws, sample, objects, larray, reports...
# Some utility functions have also been added in the pyiode_util.pyx module.
# The C-api function signatures are found in iode.pxi.
# 
# The C-api functions are declared in iodeapi.h and defined (for the most part) in b_api.c.
# 
# 
# How to add a new Py-function based on a C-function
# --------------------------------------------------
# 
# 1. In C
#     Create the new function in one of the iodeapi C module, say int IodeMyFn(char*) in api/b_api.c
#     Declare the new function in api/iodeapi.h
#     Goto api/vc64 
#     Execture set64.bat to create the environment variables for VC64 nmake
#     Execute nmake to create 64 bits iodeapi.lib => api/vc64/nmake
#     
# 2. In Python (pyx files)
#     Add the new function declaration in iode.pxi in the section "cdef extern from "iodeapi.h":":
#         cdef int IodeMyFn(char *name)  
#     (Note that there is no semi-colon at thenend of the line !)
#     
#     Create the Python equivalent to call IodeMyFn() in the appropriate module pyiode_*.pyx.
#     Use the utility functions:
#         - cstr() to translate python strings (utf8) to C char* (ansi code cp850)
#         - pystr() to do the reverse
#         
#         def myfn(arg):
#             return IodeMyFn(cstr(arg))
# 
# 3. In a DOS shell
#     cd  c:/usr/iode_src/pyiode/vc64
#     set64
#     activate 
#     makepy

# cython: binding=True, language_level=3, embedsignature=True

# PYIODE API
# ----------

include "iode.pxi" 

# Included modules by topic
# -------------------------

include "pyiode_cdef.pyx"
include "pyiode_util.pyx"
include "pyiode_lec.pyx"
include "pyiode_sample.pyx"
include "pyiode_ws.pyx"
include "pyiode_data.pyx"
include "pyiode_estim.pyx"
include "pyiode_model.pyx"
include "pyiode_exec.pyx"
include "pyiode_print.pyx"
include "pyiode_reports.pyx"
include "pyiode_objs.pyx"
include "pyiode_larray.pyx"


# MAIN
# ----

# Numpy must be initialized before we can use it safely.
np.import_array()

# IODE ws must initialize SWAP memory and create empty workspaces 
IodeInit()

# Super fns could be replaced here (optional)
ODE_assign_super_PYIODE() 


# ------------------------------------------------------------------------------------------
# API to be implemented (from report api)
# ---------------------------------------
#
# - fileimportvar",            B_FileImportVar,     
# - fileimportcmt",            B_FileImportCmt,     
# 
# x ws_content  
# x ws_sample
# x wsload",                   B_WsLoad,            
# - wscopy",                   B_WsCopy,            
# x wssave",                   B_WsSave,            
# - wssavecmp",                B_WsSaveCmp,         
# - wsimport",                 B_WsImport,          
# - wsexport",                 B_WsExport,          
# - wsmerge",                  B_WsMerge,           
# x wsclear",                  B_WsClear,           
# x wsclearall",               B_WsClearAll,        
# - wsdescr",                  B_WsDescr,           
# - wsextrapolate",            B_WsExtrapolate,     
# - wsaggrchar",               B_WsAggrChar,        
# - wsaggrsum",                B_WsAggrSum,         
# - wsaggrmean",               B_WsAggrMean,        
# - wsaggrprod",               B_WsAggrProd,        
# - wshtollast",               B_WsHtoLLast,        
# - wshtolsum",                B_WsHtoLSum,         
# - wshtolmean",               B_WsHtoLMean,        
# - wsltohflow",               B_WsLtoHFlow,        
# - wsltohstock",              B_WsLtoHStock,       
# - csvsave",                  B_CsvSave,           
# - csvdigits",                B_CsvNbDec,          
# - csvsep",                   B_CsvSep,            
# - csvdec",                   B_CsvDec,            
# - csvnan",                   B_CsvNaN,            
# - csvaxes",                  B_CsvAxes,           
# - statunitroot",             B_StatUnitRoot,      
# 
# - wsseasonadj",              B_WsSeasonAdj,       
# - wstrend",                  B_WsTrend,           
# - wstrendstd",               B_WsTrendStd,        
#   
# - wsimporteviews",			B_WsImportEviews,   
# 
# 
# 
# 
# 
# From b_data.c 
# -------------
# x dataupdate",               B_DataUpdate,      
# - datadelete",               B_DataDelete,      
# - datarename",               B_DataRename,      
# - datasearch",               B_DataSearch,      
# - dataduplicate",            B_DataDuplicate,   
# - datalist",                 B_DataList,        
# - datacompare",              B_DataCompare,     
# - datacompareeps",           B_DataCompareEps,  
# - datalistsort",             B_DataListSort,    
# - dataeditcnf",              B_DataEditCnf,     
# - datacalcvar",              B_DataCalcVar,     
# - datacalclst",              B_DataCalcLst,     
# - datapattern",              B_DataPattern,     
# - datadisplaygraph",         B_DataDisplayGraph,
# - dataprintgraph",           B_DataPrintGraph,  
# - datascan",                 B_DataScan,        
# 
#  b_ras.c
#  -------
# -    "datarasvar",               B_DataRasVar,           NULL,               0,
#  
#  b_est.c
#  -------
# - eqsestimate",              B_EqsEstimate,          SB_EqsEstimate,     0,
# - eqssetmethod",             B_EqsSetMethod,         NULL,               0,
# - eqssetbloc",               B_EqsSetBloc,           NULL,               0,
# - eqssetblock",              B_EqsSetBloc,           NULL,               0,
# - eqssetsample",             B_EqsSetSample,         NULL,               0,
# - eqssetinstrs",             B_EqsSetInstrs,         NULL,               0,
# - eqssetcmt",                B_EqsSetCmt,            NULL,               0,
# 
#  b_step.c
#  -------- 
#  - eqsstepwise",              B_EqsStepWise,          NULL,               0,
#  
#  - printobjtitle",            B_PrintObjTblTitle,     NULL,               0,
#  - printobjlec",              B_PrintObjLec,          NULL,               0,
#  - printobjinfos",            B_PrintObjEqsInfos,     NULL,               0,
#   
#  - printobjdef",              B_PrintObjDef,          SB_PrintObjDef,     1,
#  - printdest",                B_PrintDest,            NULL,               0,
#  - printdestnew",             B_PrintDestNew,         NULL,               0,
#  - printmulti",               B_PrintMulti,           NULL,               0,
#  - printnbdec",               B_PrintNbDec,           NULL,               0,
#  - printlang",                B_PrintLang,            NULL,               0,
#  - printtblfile",             B_ViewTblFile,          NULL,               0,
#  - printtbl",                 B_PrintTbl,             SB_ViewPrintTbl,    0,
#  - printvar",                 B_PrintVar,             NULL,               0,
#   
#  - graphdefault",             B_GraphDefault,         NULL,               0,
#   
#  - printgrall",               B_PrintGr,              SB_ViewPrintGr,     0,
#  - printgr",                  B_PrintGr,              SB_ViewPrintGr,     0,
#     
#  - modelsimulate",            B_ModelSimulate,        SB_ModelSimulate,   0,
#  - modelsimulateparms",       B_ModelSimulateParms,   NULL,               0,
#  - modelexchange",            B_ModelExchange,        NULL,               0,
#  - modelcompile",             B_ModelCompile, 	    SB_ModelCompile,    0,
#  
#  - modelcalcscc",             B_ModelCalcSCC,         NULL,               0,
#  - modelsimulatescc",         B_ModelSimulateSCC,     NULL,               0,
# 
#  - modelsimulatesaveniters",  B_ModelSimulateSaveNIters,  NULL,           0,
#  - modelsimulatesavenorms",   B_ModelSimulateSaveNorms,   NULL,           0,
# 
#  - idtexecute",               B_IdtExecute,           SB_IdtExecute,      0,
#  - idtexecutevarfiles",       B_IdtExecuteVarFiles,   NULL,               0,
#  - idtexecutesclfiles",       B_IdtExecuteSclFiles,   NULL,               0,
#  - idtexecutetrace",          B_IdtExecuteTrace,      NULL,               0,
#  
#  X reportexec",               B_ReportExec,           SB_ReportExec,      0,
#  - sleep",                    B_Sleep,                NULL,               0,
#         
#     "printa2mappend",           B_PrintA2mAppend,       NULL,               0,
#     "printfont",                B_PrintFont,            NULL,               0,
#     "printtablefont",           B_PrintTFont,           NULL,               0,
#     "printtablebox",            B_PrintTBox,            NULL,               0,
#     "printtablecolor",          B_PrintTColor,          NULL,               0,
#     "printtablewidth",          B_PrintTWidth,          NULL,               0,
#     "printtablebreak",          B_PrintTBreak,          NULL,               0,
#     "printtablepage",           B_PrintTPage,           NULL,               0,
# 
#     "printhtmltableclass",      B_PrintHtmlTableClass,  NULL,               0,
#     "printhtmltrclass",         B_PrintHtmlTRClass,     NULL,               0,
#     "printhtmlthclass",         B_PrintHtmlTHClass,     NULL,               0,
#     "printhtmltdclass",         B_PrintHtmlTDClass,     NULL,               0,
# 
#     "printbackground",          B_PrintGColor,          NULL,               0,
#     "printgraphbox",            B_PrintGBox,            NULL,               0,
#     "printgraphbrush",          B_PrintGBrush,          NULL,               0,
#     "printgraphsize",           B_PrintGSize,           NULL,               0,
#     "printgraphpage",           B_PrintGPage,           NULL,               0,
#     "printgraphtheme",          B_PrintGTheme,          NULL,               0,
#     "printgraphband",           B_PrintGBand,           NULL,               0,
#             
#     "printrtfhelp",             B_PrintRtfHelp,         NULL,               0,
#     "printhtmlhelp",            B_PrintHtmlHelp,        NULL,               0,
#     "printrtftopic",            B_PrintRtfTopic,        NULL,               0,
#     "printrtflevel",            B_PrintRtfLevel,        NULL,               0,
#     "printrtftitle",            B_PrintRtfTitle,        NULL,               0,
#     "printrtfcopyright",        B_PrintRtfCopy,         NULL,               0,
#     "printparanum",             B_PrintParaNum,         NULL,               0,
#     "printpageheader",          B_PrintPageHeader,      NULL,               0,
#     "printpagefooter",          B_PrintPageFooter,      NULL,               0,
#     "printorientation",         B_PrintGdiOrient,       NULL,               0,
#     "printduplex",              B_PrintGdiDuplex,       NULL,               0,
#     "setprinter",               B_PrintGdiPrinter,      NULL,               0,
#     
#     "printgiftranscolor",       B_PrintGIFTransColor,   NULL,               0,
#     "printgifbackcolor",        B_PrintGIFBackColor,    NULL,               0,
#     "printgifinterlaced",       B_PrintGIFInterlaced,   NULL,               0,
#     "printgiftransparent",      B_PrintGIFTransparent,  NULL,               0,
#     "printgiffilled",           B_PrintGIFFilled,       NULL,               0,
#     "printgiffont",             B_PrintGIFFont,         NULL,               0,
#     
#     "printhtmlstrip",           B_PrintHtmlStrip,       NULL,               0,
#     "printhtmlstyle",           B_PrintHtmlStyle,       NULL,               0,
#             
#  - sysansitoutf8",            B_SysAnsiToUTF8,        NULL,               0,
#  - sysoemtoutf8",             B_SysOemToUTF8,         NULL,               0,
#  - sysansitooem",             B_SysAnsiToOem,         NULL,               0,
#  - sysoemtoansi",             B_SysOemToAnsi,         NULL,               0,
#                 
#  - a2mtohtml",                B_A2mToHtml,            NULL,               0,
#  - a2mtomif",                 B_A2mToMif,             NULL,               0,
#  - a2mtocsv",                 B_A2mToCsv,             NULL,               0,
#  - a2mtortf",                 B_A2mToRtf,             NULL,               0,
#  - a2mtoprinter",             B_A2mToPrinter,         NULL,               0,
# 